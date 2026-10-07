#ifndef WOW_SOURCE_WORLDCLIENT_MAP_H
#define WOW_SOURCE_WORLDCLIENT_MAP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/RectArray.h"

namespace World {
    struct FacetData;
    struct LiquidQueryResult;
    struct TriData;
}

class CChunkLiquid;
class CFacet;
class CImVector;
class CMapChunk;
class CMapChunkBuf;
class CMapDoodadDef;
class CMapEntity;
class CFrustum;
class CMapLight;
class CMapObj;
class CMapObjDef;
class CMapObjDefGroup;
class CMapObjGroup;
class CMapRenderChunk;
class CGxShader;
class CTexture;
class CShadowQuery;
class CWorldMap;
struct CShadowCacheQueryData;
class CWorldView;
class C44Matrix;
class SFileImpl;

struct SMMapHeader {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008[6];
};

struct SMAreaInfo {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_asyncId;
};

struct SMMapObjDef {
    /* 0000 */ uint32_t m_nameId;
    /* 0004 */ uint32_t m_uniqueId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C3Vector m_rotation;
    /* 0020 */ CAaBox m_extents;
    /* 0038 */ uint16_t m_flags;
    /* 003A */ uint16_t m_doodadSet;
    /* 003C */ uint16_t m_nameSet;
    /* 003E */ uint16_t m_scale;
};

struct SMDoodadDef {
    /* 0000 */ uint32_t m_nameId;
    /* 0004 */ uint32_t m_uniqueId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C3Vector m_rotation;
    /* 0020 */ uint16_t m_scale;
    /* 0022 */ uint16_t m_flags;
};

class CMapDynamicTexture : public TSHashObject<CMapDynamicTexture, HASHKEY_DWORD> {
    public:
    /* 0018 */ char m_fileName[260];
    /* 011C */ uint32_t m_flags;
    /* 0120 */ CTexture* m_texture;
};

class CMap : public CMapBaseObj {
    public:
    enum {
        Flag_GlobalMapObj = 0x1,
        Flag_Preload = 0x2,
        Flag_Current = 0x4,
        Flag_Remove = 0x8,
    };

    struct CMapAreaTableEntry {
        /* 0000 */ uint32_t m_flags;
        /* 0004 */ CMapArea* m_area;

        CMapAreaTableEntry();
    };

    static TSObjectAllocPool<CMapLight>* s_lightHeap;
    static TSObjectAllocPool<CMapObjGroup>* s_mapObjGroupHeap;
    static TSObjectAllocPool<CMapObj>* s_mapObjHeap;
    static TSObjectAllocPool<CMapBaseObjLink>* s_baseObjLinkHeap;
    static TSObjectAllocPool<CMapArea>* s_areaHeap;
    static TSObjectAllocPool<CMapAreaMed>* s_areaMedHeap;
    static TSObjectAllocPool<CMapAreaLow>* s_areaLowHeap;
    static TSObjectAllocPool<CMapChunk>* s_chunkHeap;
    static TSObjectAllocPool<CMapRenderChunk>* s_renderChunkHeap;
    static TSObjectAllocPool<CMapChunkBuf>* s_chunkBufHeap;
    static TSObjectAllocPool<CMapChunkInfo>* s_chunkInfoHeap;
    static TSObjectAllocPool<CMapDoodadDef>* s_doodadDefHeap;
    static TSObjectAllocPool<CMapEntity>* s_entityHeap;
    static TSObjectAllocPool<CMapObjDefGroup>* s_mapObjDefGroupHeap;
    static TSObjectAllocPool<CMapObjDef>* s_mapObjDefHeap;
    static TSObjectAllocPool<CChunkLiquid>* s_chunkLiquidHeap;
    static int32_t enableTerrainShaderVertex;
    static int32_t enableTerrainShaderPixel;
    static int32_t enableChunkBatching;
    static int32_t enableTriangleStrips;
    static CGxShader* vs_terrain;
    static CGxShader* hs_terrain;
    static CGxShader* ds_terrain;
    static CGxShader* ps_terrain;
    static CTexture* s_blackTexture;
    static uint32_t counts[10];
    static uint32_t freeCounts[10];
    static TSExplicitList<CMapAreaLow, offsetof(CMapAreaLow, m_renderLink)> s_mapAreaLowUpdateList;
    static TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_link)> s_mapRenderChunkUpdateList;
    static TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_updateLink)> s_mapChunkLiquidUpdateList;
    static TSExplicitList<CMapDoodadDef, offsetof(CMapDoodadDef, m_updateLink)> s_doodadDefLoadingList;
    static TSExplicitList<CMapDoodadDef, offsetof(CMapDoodadDef, m_updateLink)> s_doodadDefLoadedList;
    static int32_t s_globalUniqueId;
    static TSGrowableArray<CGxVertexPC> s_testQueryVertices;
    static TSGrowableArray<uint16_t> s_testQueryIndices;
    static int32_t s_areasUnloaded;

    static CMapArea* AllocArea();
    static CMapAreaLow* AllocAreaLow();
    static CMapBaseObjLink* AllocBaseObjLink(CMapBaseObj* owner, CMapBaseObj* ref);
    static CMapChunk* AllocChunk();
    static CMapChunkBuf* AllocChunkBuf();
    static CMapChunkInfo* AllocChunkInfo();
    static CChunkLiquid* AllocChunkLiquid();
    static CMapDoodadDef* AllocDoodadDef();
    static CMapEntity* AllocEntity();
    static CMapObj* AllocMapObj();
    static CMapObjDef* AllocMapObjDef();
    static CMapObjDefGroup* AllocMapObjDefGroup();
    static CMapObjGroup* AllocMapObjGroup();
    static void* AllocRawAreaData(uint32_t size);
    static CMapRenderChunk* AllocRenderChunk();
    static TSHashTable<CMapDynamicTexture, HASHKEY_DWORD> s_dynamicTextures;

    static void ClearDynamicTextures();
    static CTexture* GetDynamicTexture(uint32_t textureId);
    static void Destroy();
    static void FreeArea(CMapArea* area);
    static void FreeAreaLow(CMapAreaLow* areaLow);
    static void FreeBaseObjLink(CMapBaseObjLink* link);
    static void FreeChunk(CMapChunk* chunk);
    static void FreeChunkBuf(CMapChunkBuf* chunkBuf);
    static void FreeChunkInfo(CMapChunkInfo* chunkInfo);
    static void FreeChunkLiquid(CChunkLiquid* liquid);
    static void FreeDoodadDef(CMapDoodadDef* doodadDef);
    static void FreeEntity(CMapEntity* entity);
    static void FreeMapObj(CMapObj* mapObj);
    static void FreeMapObjDef(CMapObjDef* mapObjDef);
    static bool GetDoodadDefFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags, uint64_t guid);
    static bool GetEntityFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags);
    static void FreeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup);
    static void FreeMapObjGroup(CMapObjGroup* mapObjGroup);
    static void FreeRawAreaData(void* data);
    static void FreeRenderChunk(CMapRenderChunk* renderChunk);
    static CTexture* LoadTerrainTexture(const char* fileName, uint32_t flags);
    static CTexture* LoadTexture(const char* fileName);
    static int32_t GetHeightTerrain(const CMapChunk* chunk, const C3Vector& pos, int32_t subX, int32_t subY, int32_t a5, float& height);
    static uint32_t GetTerrainPSPerm(uint32_t materialType, uint32_t layers, uint32_t shadowMode);
    static uint32_t GetTerrainVSPerm(int32_t displacement, int32_t lighting, uint32_t layers, uint32_t format, uint32_t material, int32_t shadow, uint32_t clip);
    static void Initialize();
    static void UpdateDynamicTextures();
    static void MapMemDestroy();
    static void ProcessAreaLowUpdateList();
    static void ProcessChunkLiquidUpdateList();
    static void ProcessDetailDoodadUpdateList();
    static void ProcessDoodadDefUpdateList();
    static void ProcessRenderChunkUpdateList();
    static void MapMemInitialize();
    static int32_t SafeOpen(const char* fileName, SFileImpl** file, int32_t warn);
    static int32_t SafeRead(const char* fileName, SFileImpl* file, void* buffer, uint32_t bytes);
    static void SnapBaseObjToSubChunk(CMapBaseObj* baseObj, C3Vector& pos, float facing);
    static CMapBaseObjLink* LinkObjectToMapObjDefGroup(CMapBaseObj* object, CMapObjDefGroup* group);
    static bool GetHeightMapObjDef(CMapObjDef* mapObjDef, const C3Vector& start, const C3Vector& end, float& t);
    static bool GetHeightDoodadDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, const C3Vector& start, const C3Vector& end);
    static bool GetHeightMapObjDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, const C3Vector& start, const C3Vector& end, float& t);
    static void GetTrisSubchunk(CMapChunk* chunk, const CiRect& rect, const CAaBox& box, World::TriData& tris);
    static void GetTrisChunk(CMapChunk* chunk, const CAaBox& box, const CiRect& rect, World::TriData& tris, uint32_t flags);
    static void GetChunkLiquidFacets(CChunkLiquid* liquid, const CAaBox& box, const CiRect& rect, World::FacetData& facets);
    static bool GetMapObjDefFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags);
    static void GetMapObjDefFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CFrustum& frustum, World::FacetData& facets, uint32_t flags);
    static void CreateImpassableFacets(CMapChunk* chunk, const CAaBox& box, World::FacetData& facets, uint32_t flags);
    static bool GetChunkFacets(CMapChunk* chunk, const CFrustum& frustum, const CAaBox& bounds, const CiRect& rect, World::FacetData& facets, uint32_t flags);
    static bool QueryLiquidMapObjDef(CMapObjDef* mapObjDef, const C3Vector& pos, int32_t nearest, uint32_t& liquidType, float& height);
    static void QueryLightingColorChunk(const CMapChunk* chunk, const C3Vector& pos, int32_t cellY, int32_t cellX, CImVector& color);
    static bool QueryLightingColorTerrain(CMapChunk* chunk, const C3Vector& pos, CImVector& color);
    static void TestQueryAdd(const CFacet& facet, CImVector color, const C44Matrix* mat);
    static void TestQueryRender();
    static void TestQueryReset();

    /* 0010 */ CWorldMap* m_worldMap;
    /* 0014 */ TSExplicitList<CMapArea, offsetof(CMapArea, m_mapLink)> m_areaList;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ SMMapHeader m_header;
    /* 0044 */ int32_t m_mapId;
    /* 0048 */ uint32_t m_mapFlags;
    /* 004C */ char m_mapPath[256];
    /* 014C */ char m_mapName[256];
    /* 024C */ char m_wdtFileName[256];
    /* 034C */ char unk_034C[256];
    /* 044C */ CMapObjDef* m_globalMapObjDef;
    /* 0450 */ CiRect m_areaExtents;
    /* 0460 */ TSRectArray<CMapAreaTableEntry> m_areaTable;
    /* 0488 */ CMapLowDetail m_lowDetail;

    virtual ~CMap();

    CMap();
    void ClearChunkBufs();
    uint32_t QueryAreaIdTerrain(const C3Vector& pos);
    int32_t LinkStaticEntityGetChunk(const C3Vector& pos, float& height, CMapChunk*& chunk);
    int32_t LinkObjectToExterior(CMapBaseObj* object, const CAaBox& box, const CiRect& rect, int32_t linkChunks, int32_t linkMapObjs);
    void GetTrisTerrain(const CAaBox& box, const CiRect& chunkRect, const CiRect& subChunkRect, World::TriData& tris, uint32_t flags);
    bool GetFacets(const CAaBox& box, const CAaBox& box2, const CiRect& chunkRect, const CiRect& subChunkRect, World::FacetData& facets, uint32_t flags);
    bool GetFacets(const CFrustum& frustum, const CAaBox& bounds, const CiRect& chunkRect, const CiRect& subChunkRect, World::FacetData& facets, uint32_t flags);
    bool CreateFlightBoundsFacets(int32_t x, int32_t y, const CAaBox& box, World::FacetData& facets, uint32_t flags);
    bool GetChunkFacets(CMapChunk* chunk, const CAaBox& box, const CAaBox& box2, const CiRect& rect, World::FacetData& facets, uint32_t flags);
    int32_t QueryLiquidTerrain(const C3Vector& pos, uint32_t& liquidType, float& height);
    bool QueryLiquid(const C3Vector& pos, int32_t nearest, uint32_t& liquidType, float& height);
    bool QueryShadowTerrain(const C3Vector& pos);
    bool QueryGroundTypeTerrain(const C3Vector& pos, uint32_t& groundType);
    int32_t GetFlightBounds(const C3Vector& pos, float& z, int32_t lower);
    void RenderFlightBounds(const C3Vector& pos);
    void ShadowQueryTerrain(CShadowQuery& query, CShadowCacheQueryData& data, CiRect& rect, int32_t startBand, int32_t endBand);
    void ShadowQueryTerrain(CShadowQuery& query, CShadowCacheQueryData& data, CiRect& rect, int32_t band);
    void QueryLiquidSounds(const C3Vector& pos, float radius, TSGrowableArray<World::LiquidQueryResult>& results);
    void ClearChunkDetailDoodads();
    void ClearRenderChunks();
    void CreateAreaTable(const SMAreaInfo* areaInfo);
    void Gather(CWorldView* view, const CiRect& rect);
    void GatherInfiniteOcean(CWorldView* view, int32_t x, int32_t y);
    void GatherLowDetail(CWorldView* view, int32_t x, int32_t y);
    void GatherMapObjDefs(CWorldView* view, CMapArea* area);
    int32_t Load(const char* mapName, CWorldMap* worldMap, int32_t mapId, int32_t preload);
    void LoadArea(CMapArea* area);
    void LoadTextureBlob();
    int32_t LoadWdt();
    void Purge();
    void UnloadXXX();
    void UpdateArea(CMapArea* area, const CiRect& rect, int32_t depth);
    void UpdateAreas(int32_t a2, int32_t a3);
    void PreUpdateAreas();
    CMapArea* PrepareArea(int32_t x, int32_t y);
    void UpdateAreaDistances();
    CMapChunk* GetChunk(const C3Vector& pos);
    int32_t PreloadAreasExist(const CiRect& rect);
    void PreloadUpdateMap(const CiRect& rect);
    void PreUpdateMap();
    void UpdateLowDetailMapObjDefs();
    void UpdateMap();
    void WaterReset();
};

#include "WoW/Source/WorldClient/Map.inl"


#endif
