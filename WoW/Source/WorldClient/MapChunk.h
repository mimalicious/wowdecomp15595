#ifndef WOW_SOURCE_WORLDCLIENT_MAPCHUNK_H
#define WOW_SOURCE_WORLDCLIENT_MAPCHUNK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"

class C3Ray;
class C2Vector;
class C4Plane;
struct C3Segment;
class CAaBox;
class CDetailDoodadInst;
class CFrustum;
class CiRect;
class CMap;
class CMapArea;
class CMapRenderChunk;
class CTexture;
class CWorldView;
class CWorldMap;
namespace World { struct FacetData; }

struct SMChunk {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_indexX;
    /* 0008 */ uint32_t m_indexY;
    /* 000C */ uint32_t m_nLayers;
    /* 0010 */ uint32_t m_nDoodadRefs;
    /* 0014 */ uint32_t m_ofsHeight;
    /* 0018 */ uint32_t m_ofsNormal;
    /* 001C */ uint32_t m_ofsLayer;
    /* 0020 */ uint32_t m_ofsRefs;
    /* 0024 */ uint32_t m_ofsAlpha;
    /* 0028 */ uint32_t m_sizeAlpha;
    /* 002C */ uint32_t m_ofsShadow;
    /* 0030 */ uint32_t m_sizeShadow;
    /* 0034 */ uint32_t m_areaId;
    /* 0038 */ uint32_t m_nMapObjRefs;
    /* 003C */ uint16_t m_holes;
    /* 003E */ uint16_t m_holesHigh;
    /* 0040 */ uint8_t m_lowQualityTexMap[16];
    /* 0050 */ uint8_t m_noEffectDoodad[8];
    /* 0058 */ uint32_t m_ofsSndEmitters;
    /* 005C */ uint32_t m_nSndEmitters;
    /* 0060 */ uint32_t m_ofsLiquid;
    /* 0064 */ uint32_t m_sizeLiquid;
    /* 0068 */ C3Vector m_position;
    /* 0074 */ uint32_t m_ofsMCCV;
    /* 0078 */ uint32_t m_ofsMCLV;
    /* 007C */ uint32_t unk_007C;
};

class CMapChunkHeader {
    public:
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_areaId;
    /* 0008 */ uint16_t m_nDoodadRefs;
    /* 000A */ uint16_t m_nMapObjRefs;
    /* 000C */ uint16_t m_nLayers;
    /* 000E */ uint16_t m_nSndEmitters;
    /* 0010 */ uint8_t m_lowQualityTexMap[16];
    /* 0020 */ uint8_t m_noEffectDoodad[8];

    void Set(const SMChunk& chunk);
};

struct SMLayer {
    /* 0000 */ uint32_t m_textureId;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ uint32_t m_offsetInMCAL;
    /* 000C */ uint32_t m_effectId;
};

class CMapChunk : public CMapBaseObj {
    public:
    /* 0010 */ CMapArea* m_area;
    /* 0014 */ CMap* m_map;
    /* 0018 */ CWorldMap* m_worldMap;
    /* 001C */ TSLink<CMapChunk> m_areaLink;
    /* 0024 */ int32_t m_indexX;
    /* 0028 */ int32_t m_indexY;
    /* 002C */ uint16_t m_holes;
    /* 002E */ uint16_t m_holesHigh;
    /* 0030 */ CAaBox m_bbox;
    /* 0048 */ int32_t m_chunkX;
    /* 004C */ int32_t m_chunkY;
    /* 0050 */ CMapChunkHeader m_header;
    /* 0078 */ uint32_t m_chunkFlags;
    /* 007C */ CAaBox m_terrainBox;
    /* 0094 */ CAaBox m_liquidBox;
    /* 00AC */ C3Vector m_position;
    /* 00B8 */ CDetailDoodadInst* m_detailDoodadInst;
    /* 00BC */ CMapRenderChunk* m_renderChunk;
    /* 00C0 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_doodadDefLinkList;
    /* 00CC */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_entityLinkList;
    /* 00D8 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_mapObjDefLinkList;
    /* 00E4 */ TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_chunkLink)> m_liquidList;
    /* 00F0 */ const uint8_t* m_lowQualityTexMap;
    /* 00F4 */ const uint8_t* m_noEffectDoodad;
    /* 00F8 */ const float* m_vertices;
    /* 00FC */ const uint32_t* m_vertexColors;
    /* 0100 */ const uint32_t* m_vertexLighting;
    /* 0104 */ const int8_t* m_normals;
    /* 0108 */ const uint8_t* m_shadowMap;
    /* 010C */ const SMLayer* m_layers;
    /* 0110 */ const uint8_t* m_alphaMaps;
    /* 0114 */ const uint32_t* m_refs;
    /* 0118 */ const uint32_t* m_doodadRefs;
    /* 011C */ const uint32_t* m_mapObjRefs;
    /* 0120 */ const void* m_mcLiquid;
    /* 0124 */ void* m_liquidChunk;
    /* 0128 */ uint32_t m_materialId;
    /* 012C */ CTexture* m_shaderTexture;
    /* 0130 */ uint8_t m_shaderType;

    static C3Vector vertexListXY[145];
    static float s_geoToTex;
    static uint32_t s_vertexCount;
    static uint16_t s_indexCount;

    static void Destroy();
    static void InitVertexListXY();
    static void Initialize();
    static int32_t TestBit(const uint8_t* bits, uint32_t index);
    static void LinkToAsyncHead(void* param);

    virtual ~CMapChunk();
    virtual void SelectLights(CM2Lighting* lighting);

    CMapChunk();
    int32_t AreTexturesLoaded();
    void Batch();
    void Create(int32_t isNew);
    void CreateBounds();
    void CreateDetailDoodads();
    uint32_t CreateIndices(uint16_t* indices, uint16_t base, int32_t lowRes);
    void CreateIndices(uint16_t* indices, uint16_t base, uint32_t& count, int32_t lowRes);
    void CreateLiquids(int32_t isNew);
    void CreateMaterial();
    void CreatePointers(uint8_t* data, int32_t isNew, int32_t root);
    void CreateRefs(CMapArea* area);
    void CreateVertices(void* vertices, int32_t index, const C3Vector& origin);
    void CreateVerticesLocalPN(float* vertices);
    void CreateVerticesLocalPNC(float* vertices);
    void CreateVerticesWorldPN(float* vertices, const C3Vector& origin);
    void CreateVerticesWorldPNC(float* vertices, const C3Vector& origin);
    void CreateVerticesWorldPNC2(float* vertices, const C3Vector& origin);
    int32_t LoadDetailDoodadModels();
    void RenderPrep(CWorldView* view, float distSqr);
    void GetLiquidBox(CAaBox& box);
    int32_t GetLiquidHeight(const C2Vector& pos, float& height);
    bool Intersect(int32_t subX, int32_t subY, C3Ray& ray, float& t);
    bool Intersect(int32_t subX, int32_t subY, C3Ray& ray, C4Plane& plane, float& t);
    void Intersect(const CiRect& rect, CAaBox& box, World::FacetData& facets);
    void Intersect(const CiRect& rect, const CFrustum& frustum, World::FacetData& facets);
    void ProcessIffChunks(uint32_t* data, int32_t size, int32_t isNew);
    void Purge();
};

#endif
