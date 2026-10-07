#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJGROUP_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJGROUP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Common/AaBsp.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapObj.h"

class C44Matrix;
class CAsyncObject;
class CMapObjDef;
struct CGxBuf;
struct CGxPool;

namespace World {
    struct TriData {
        struct Batch {
            /* 0000 */ const C44Matrix* m_matrix;
            /* 0004 */ C3Vector* m_vertexList;
            /* 0008 */ C3Vector* m_normalList;
            /* 000C */ uint32_t unk_000C;
            /* 0010 */ uint32_t unk_0010;
            /* 0014 */ uint16_t* m_indices;
            /* 0018 */ uint16_t* m_faceIndices;
            /* 001C */ uint16_t m_indexCount;
            /* 001E */ uint16_t m_faceCount;
            /* 0020 */ uint16_t m_minVertexIndex;
            /* 0022 */ uint16_t m_maxVertexIndex;
            /* 0024 */ const CMapObjDef* m_def;
        };

        static uint32_t s_statusFlags;
        static uint32_t s_nBatches;
        static uint32_t s_faceIndexCursor;
        static uint32_t s_indexCursor;
        static uint32_t unk_E4FE48;
        static uint16_t s_faceIndexPool[0x4000];
        static uint16_t s_indexPool[0xC000];
        static Batch s_batches[32];

        static Batch* AllocBatch();
        static Batch* AllocBatch(uint32_t indexCount, uint32_t faceCount);
        static uint16_t* AllocIndices(uint32_t count);
    };
}

struct BspQuery {
    static uint16_t s_testFaces[0x2000];
    static uint16_t s_hitFaces[0x2000];
    static uint32_t s_testFaceSub;
    static uint32_t s_hitFaceSub;

    /* 0000 */ uint32_t* m_overflowFlags;
    /* 0004 */ SMOPoly* m_faces;
    /* 0008 */ C3Vector* m_vertexList;
    /* 000C */ uint16_t* m_indices;

    ~BspQuery();
};

struct BspQuery_Segment : public BspQuery {
    /* 0010 */ float* m_hitT;
    /* 0014 */ float m_origHitT;
    /* 0018 */ C3Segment m_seg;
    /* 0030 */ C3Ray m_ray;
    /* 0048 */ float m_oosegMag;
    /* 004C */ float m_maxT;
    /* 0050 */ SMOMaterial* m_materials;
    /* 0054 */ uint16_t m_faceIgnoreFlags;

    BspQuery_Segment(SMOPoly* faces, C3Vector* vertexList, uint16_t* indices, const C3Segment& seg, float* hitT, uint16_t faceIgnoreFlags, SMOMaterial* materials);
    void operator()(uint16_t face);
    bool GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node);
};

struct SMOBatch {
    /* 0000 */ int16_t m_box[6];
    /* 000C */ uint32_t m_startIndex;
    /* 0010 */ uint16_t m_count;
    /* 0012 */ uint16_t m_minIndex;
    /* 0014 */ uint16_t m_maxIndex;
    /* 0016 */ uint8_t m_flags;
    /* 0017 */ uint8_t m_materialId;
};

struct SMOGroupHeader {
    /* 0000 */ int32_t m_nameOffset;
    /* 0004 */ int32_t m_descNameOffset;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ CAaBox m_bbox;
    /* 0024 */ uint16_t m_portalStart;
    /* 0026 */ uint16_t m_portalCount;
    /* 0028 */ uint16_t m_transBatchCount;
    /* 002A */ uint16_t m_intBatchCount;
    /* 002C */ uint16_t m_extBatchCount;
    /* 002E */ uint16_t m_pad;
    /* 0030 */ uint8_t m_fogs[4];
    /* 0034 */ uint32_t m_liquidType;
    /* 0038 */ uint32_t m_groupId;
    /* 003C */ uint32_t m_flags2;
    /* 0040 */ uint32_t unk_0040;
};

struct SMOStripBatch {
    /* 0000 */ uint32_t m_startIndex;
    /* 0004 */ uint16_t m_count;
    /* 0006 */ uint16_t m_pad;
};

struct SMOLiquidVert {
    /* 0000 */ uint8_t m_flow[4];
    /* 0004 */ float m_height;
};

class VBBList {
    public:
    class Block {
        public:
        /* 0000 */ TSLink<Block> m_link;
        /* 0008 */ uint32_t m_lastUsedFrame;
        /* 000C */ uint32_t m_offset;
        /* 0010 */ uint32_t m_size;
        /* 0014 */ CGxPool* m_pool;
        /* 0018 */ CGxBuf* m_buf;
        /* 001C */ Block** m_owner;

        void Set(uint32_t itemSize, uint32_t itemCount, uint32_t offset);
    };

    /* 0000 */ int32_t m_pooled;
    /* 0004 */ EGxPoolTarget m_target;
    /* 0008 */ EGxPoolUsage m_usage;
    /* 000C */ CGxPool* m_pool;
    /* 0010 */ TSExplicitList<Block, offsetof(Block, m_link)> m_freeBlocks;
    /* 001C */ TSExplicitList<Block, offsetof(Block, m_link)> m_blocks;

    Block* AllocBlock();
    void AllocVBB(Block** owner, uint32_t itemSize, uint32_t itemCount);
    void Destroy();
    void FreeVBB(Block* block);
};

class CMapObjGroup : public CMapBaseObj {
    public:
    class VertArray {
        public:
        /* 0000 */ TSLink<VertArray> m_link;
        /* 0008 */ TSFixedArray<C3Vector> m_verts;
    };

    static VBBList s_vertexList;
    static VBBList s_indexList;
    static TSExplicitList<VertArray, offsetof(VertArray, m_link)> s_freeVertArrays;

    static void AllocVertArray(VertArray*& array, uint32_t count);
    static void AsyncCallback(CAsyncObject* object);
    static void AsyncFailedCallback(CAsyncObject* object);
    static void AsyncFreeCallback(CAsyncObject* object);
    static void ComputeTangents(const SMOBatch* batch, const uint16_t* indices, const C3Vector* vertices, const C3Vector* normals, const C2Vector* texCoords, C4Vector* tangents);
    static void Destroy();
    static void Initialize();
    static void LinkToAsyncHead(void* param);
    static void PurgeVertArrayList();
    static void VectorIntersectLiquidSX(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles);
    static void VectorIntersectLiquidSY(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles);

    /* 0010 */ VBBList::Block* m_vertsBlock;
    /* 0014 */ VBBList::Block* m_transVertsBlock;
    /* 0018 */ VBBList::Block* m_indicesBlock;
    /* 001C */ VBBList::Block* m_liquidVertsBlock;
    /* 0020 */ VBBList::Block* m_liquidIndicesBlock;
    /* 0024 */ uint32_t m_unusedFrames;
    /* 0028 */ VertArray* m_liquidVertArray;
    /* 002C */ CGxBatch m_liquidBatch;
    /* 003C */ CMapObj* m_parent;
    /* 0040 */ uint32_t m_groupIndex;
    /* 0044 */ CAaBox m_bbox;
    /* 005C */ uint32_t m_frameStamp;
    /* 0060 */ uint32_t m_groupFlags;
    /* 0064 */ float m_distSq;
    /* 0068 */ CAsyncObject* m_asyncObject;
    /* 006C */ uint32_t m_renderFlags;
    /* 0070 */ uint32_t m_wmoGroupId;
    /* 0074 */ EGxVertexBufferFormat m_vertexFormat;
    /* 0078 */ TSExplicitList<CMapObjOccluder, offsetof(CMapObjOccluder, m_link)> m_occluders;
    /* 0084 */ uint8_t* m_data;
    /* 0088 */ uint32_t m_dataSize;
    /* 008C */ uint32_t m_portalStart;
    /* 0090 */ uint32_t m_portalCount;
    /* 0094 */ uint8_t m_fogs[4];
    /* 0098 */ uint16_t m_transBatchCount;
    /* 009A */ uint16_t m_intBatchCount;
    /* 009C */ uint16_t m_extBatchCount;
    /* 009E */ uint16_t unk_009E;
    /* 00A0 */ CAaBsp m_bsp;
    /* 0108 */ uint32_t unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ const char* m_groupName;
    /* 0118 */ SMOPoly* m_polys;
    /* 011C */ uint16_t* m_indices;
    /* 0120 */ uint16_t* m_stripIndices;
    /* 0124 */ C3Vector* m_vertices;
    /* 0128 */ C3Vector* m_normals;
    /* 012C */ C2Vector* m_texCoords;
    /* 0130 */ C2Vector* m_texCoords2;
    /* 0134 */ SMOBatch* m_batches;
    /* 0138 */ SMOBatch* m_shadowBatches;
    /* 013C */ SMOStripBatch* m_stripBatches;
    /* 0140 */ uint16_t* m_lightRefs;
    /* 0144 */ uint16_t* m_doodadRefs;
    /* 0148 */ CImVector* m_colors;
    /* 014C */ CImVector* m_colors2;
    /* 0150 */ uint16_t* m_tangentStarts;
    /* 0154 */ C4Vector* m_tangents;
    /* 0158 */ int32_t m_liquidVBRefCount;
    /* 015C */ C2iVector m_liquidVerts;
    /* 0164 */ C2iVector m_liquidTiles;
    /* 016C */ C3Vector m_liquidCorner;
    /* 0178 */ uint16_t m_liquidMaterialId;
    /* 017A */ uint16_t unk_017A;
    /* 017C */ SMOLiquidVert* m_liquidVertices;
    /* 0180 */ uint8_t* m_liquidTileFlags;
    /* 0184 */ float m_liquidMinHeight;
    /* 0188 */ float m_liquidMaxHeight;
    /* 018C */ uint32_t m_liquidType;
    /* 0190 */ int32_t m_numSharedTiles;
    /* 0194 */ uint32_t unk_0194;
    /* 0198 */ uint32_t m_polyCount;
    /* 019C */ uint32_t m_indexCount;
    /* 01A0 */ uint32_t m_stripIndexCount;
    /* 01A4 */ uint32_t m_vertexCount;
    /* 01A8 */ uint32_t m_normalCount;
    /* 01AC */ uint32_t m_texCoordCount;
    /* 01B0 */ uint32_t m_texCoord2Count;
    /* 01B4 */ uint32_t m_batchCount;
    /* 01B8 */ uint32_t m_shadowBatchCount;
    /* 01BC */ uint32_t m_lightRefCount;
    /* 01C0 */ uint32_t m_doodadRefCount;
    /* 01C4 */ uint32_t m_colorCount;
    /* 01C8 */ uint32_t m_color2Count;
    /* 01CC */ uint32_t m_indexStart;
    /* 01D0 */ uint32_t m_indexEnd;
    /* 01D4 */ uint16_t m_minVertex;
    /* 01D6 */ uint16_t m_maxVertex;

    virtual ~CMapObjGroup();

    CMapObjGroup();
    void AllocLiquidVB(EGxVertexBufferFormat format, uint32_t vertexCount, uint32_t indexCount, CGxBatch*& batch);
    void AllocVB();
    void Clear(int32_t force);
    uint32_t ComputePassCount(int32_t unified, uint32_t batch) const;
    void Create();
    void CreateDataPointers(uint8_t* data, uint8_t* end);
    void CreateOccluders();
    void FillTransparencyVB(CGxBuf* buf) const;
    void FillVertexVB(CGxBuf* buf, EGxVertexBufferFormat format) const;
    void FixColorVertexAlpha();
    void FreeLiquidVB();
    void FreeVB();
    void GenLiquidVerts();
    bool GetTris(const C3Segment& seg, float* dist, uint32_t flags, uint16_t faceIgnoreFlags, World::TriData* triData, const CMapObjDef* mapObjDef);
    void GetTrisFromQuery(World::TriData* triData, BspQuery* query, const CMapObjDef* mapObjDef, uint32_t statusFlags);
    uint32_t GetLiquidType() const;
    uint32_t GetLiquidType(uint32_t legacyType);
    int32_t GetNumSharedTiles();
    int32_t HasLiquidVB(CGxBatch*& batch);
    uint32_t IdentifyLegacyLiquidType();
    void Init();
    void InitPtrs();
    int32_t QueryLighting(const C3Vector& pos, uint16_t polyIndex, CImVector& color, bool& exterior);
    int32_t QueryLiquid(const C3Vector& pos, int32_t requireTiles, uint32_t& liquidType, float& height);
    float DistFromClosestExtPortal(const C3Vector& pos, int32_t a2);
    void SetIndexVB() const;
    void SetTransparencyVB() const;
    void SetVertexVB() const;
    void UploadIndexBuffer(CGxBuf* buf) const;
    bool VectorIntersectLiquid(const C3Segment& seg, float* t, uint32_t flags, World::TriData* triData, const CMapObjDef* mapObjDef) const;
    void VectorIntersectLiquidDX(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) const;
    void VectorIntersectLiquidDY(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) const;
    bool VectorIntersectLiquidTiles(const C3Segment& seg, float* t, uint32_t flags, const uint32_t* tileCount, const uint8_t* tiles, World::TriData* triData, const CMapObjDef* mapObjDef) const;
};

#endif
