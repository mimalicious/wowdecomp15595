#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkBuf.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/PolyClipper.h"
#include "WoW/Source/WorldClient/WorldMap.h"

typedef void (*CHUNK_VERTEX_FILL)(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices);
typedef void (*AREA_VERTEX_FILL)(const CMapAreaLow* area, const C44Matrix& transform, float** vertices);
typedef int32_t (*MAPOBJ_VERTICES_FILL)(CMapObjGroup* group, const CImVector& color, float texCoord1, void*& vertices);
typedef void (*MAPOBJ_VERTEX_FILL)(CMapObjGroup* group, const C3Vector& pos, const float* depthTable, const SMOLiquidVert* vertex, const CImVector& color, float texCoord1, void*& vertices);

// STATIC: 0x00D02A20
static float s_mapObjLiquidTexScale = 0.24f;

// STATIC: 0x00E89138
static uint32_t s_smolVertCacheSize;

// STATIC: 0x00E8913C
CDataAllocator CMeshGeomFactory::s_allocator(sizeof(CMeshGeomFactory), 4);

// STATIC: 0x00E89150
static SMOLiquidVert s_smolVertCache[64];

// STATIC: 0x00E89350
CDataAllocator CChunkGeomFactory::s_allocator(sizeof(CChunkGeomFactory), 16);

// STATIC: 0x00E89364
CDataAllocator CAreaGeomFactory::s_allocator(sizeof(CAreaGeomFactory), 16);

// STATIC: 0x00E89378
static TSGrowableArray<uint16_t> s_liquidVertexCounts;

static float ChunkStep(int32_t cell) {
    return -((17066.666f - static_cast<float>(cell) * 33.333332f) - (17066.666f - static_cast<float>(cell + 1) * 33.333332f)) * 0.125f;
}

static uint8_t DepthAlpha(const CChunkLiquid* liquid, float height, const float* terrain) {
    float alpha = (height - *terrain) * 0.5f;

    if (alpha < 0.0f) {
        alpha = 0.0f;
    } else if (alpha >= 1.0f) {
        alpha = 1.0f;
    }

    return static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f + 0.5f));
}

// OFFSET: 0x0076CA40/0x140455040
static uint16_t* BuildGridIndices(uint16_t* indices, uint16_t base, uint16_t width, uint16_t step) {
    uint16_t rowStep = step * width;
    uint16_t lastRow = step * (width - 1);

    for (uint32_t row = 0, y = 0; y < 8; row++, y += step) {
        uint16_t rowBase = base;
        uint16_t topRight = base + step;
        uint16_t bottomLeft = base + step + lastRow;
        uint16_t bottomRight = base + step + rowStep;

        for (uint32_t col = 0, x = 0; x < 8; col++, x += step) {
            indices[0] = base;

            if ((row & 1) != (col & 1)) {
                indices[1] = bottomRight;
                indices[2] = bottomLeft;
                indices[3] = bottomRight;
                indices[4] = base;
                indices[5] = topRight;
            } else {
                indices[1] = topRight;
                indices[2] = bottomLeft;
                indices[3] = bottomLeft;
                indices[4] = topRight;
                indices[5] = bottomRight;
            }

            indices += 6;
            topRight += step;
            base += step;
            bottomRight += step;
            bottomLeft += step;
        }

        base = rowBase + rowStep;
    }

    return indices;
}

// OFFSET: 0x0076CB60/0x140455170
static uint16_t* BuildAreaIndices(uint16_t* indices) {
    uint16_t center = 289;
    uint16_t topLeft = 0;

    for (uint32_t y = 0; y < 16; y++) {
        uint16_t bottomRight = topLeft + 18;

        for (uint32_t x = 0; x < 16; x++) {
            indices[0] = center;
            indices[1] = bottomRight - 17;
            indices[2] = topLeft;
            indices[3] = center;
            indices[4] = bottomRight;
            indices[5] = bottomRight - 17;
            indices[6] = center;
            indices[7] = bottomRight - 1;
            indices[8] = bottomRight;
            indices[9] = center;
            indices[10] = topLeft;
            indices[11] = bottomRight - 1;
            center++;
            topLeft++;
            bottomRight++;
            indices += 12;
        }

        topLeft++;
    }

    return indices;
}

// OFFSET: 0x0076CDB0/0x1404553E0
Liquid::IGeomFactory::~IGeomFactory() {
}

// PROVEN: 1306526c IDENTICAL/NONE
// OFFSET: 0x0076CA00/0x140455010
void CMeshGeomFactory::SetTexCoord1ValueX(float value) {
    this->m_texCoord1ValueX = value;
}

// PROVEN: bf723edc IDENTICAL/IDENTICAL
// OFFSET: 0x0076CA20/0x140586E10
float CMeshGeomFactory::GetHeight() {
    return 0.0f;
}

// PROVEN: b08f3ba8 IDENTICAL/NONE
// OFFSET: 0x0076CA30/0x140455020
void CMeshGeomFactory::Purge() {
    if (this->m_mapObjGroup) {
        this->m_mapObjGroup->FreeLiquidVB();
    }
}

// PROVEN: 2ce6c080 IDENTICAL/NONE
// OFFSET: 0x0076CB40/0x140455140
void CChunkGeomFactory::Purge() {
    if (this->m_buf) {
        CMapChunkBuf::Free(this->m_buf);
        this->m_buf = NULL;
    }
}

// PROVEN: 6d5fc22b IDENTICAL/NONE
// OFFSET: 0x0076CBD0/0x1404551F0
void CAreaGeomFactory::Purge() {
    if (this->m_buf) {
        CMapChunkBuf::Free(this->m_buf);
        this->m_buf = NULL;
    }
}

// OFFSET: 0x0076CE00
void CMeshGeomFactory::Destroy() {
    CMeshGeomFactory::s_allocator.Clear(typeid(CMeshGeomFactory).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x0076CE20/0x140455450
void CMeshGeomFactory::SetDiffuseColor(const CImVector& color) {
    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        this->m_diffuseColor = CImVector(color.r, color.g, color.b, color.a);
    } else {
        this->m_diffuseColor = color;
    }
}

// OFFSET: 0x0076CE70/0x140455490
void CMeshGeomFactory::Release() {
    if (--this->m_refCount == 0) {
        CMeshGeomFactory::s_allocator.Delete(this);
    }
}

// OFFSET: 0x0076CEA0/0x1404554D0
static void BuildMapObjLiquidIndices(CMapObjGroup* group, uint16_t*& indices) {
    uint16_t last = 0;
    int32_t started = 0;

    for (int32_t row = 0; row < group->m_liquidTiles.y; row++) {
        uint16_t i0 = static_cast<uint16_t>(row * group->m_liquidVerts.x);
        uint16_t i1 = i0 + static_cast<uint16_t>(group->m_liquidVerts.x);
        uint16_t next = i0 + 1;

        for (int32_t col = 0; col < group->m_liquidTiles.x; col++) {
            uint8_t flags = group->m_liquidTileFlags[col + row * group->m_liquidTiles.x];

            if ((flags & 0xF) == 0xF || static_cast<int8_t>(flags) < 0) {
                if (started) {
                    *indices++ = last;
                    started = 0;
                }
            } else {
                if (!started) {
                    *indices++ = i1;
                    *indices++ = i1;
                    *indices++ = i0;
                    started = 1;
                }

                *indices++ = i1 + 1;
                *indices++ = next;
                last = next;
            }

            next++;
            i0++;
            i1++;
        }

        if (started) {
            *indices++ = last;
            started = 0;
        }
    }
}

// PROVEN: 462e946d IDENTICAL/NONE
// OFFSET: 0x0076CFC0/0x140455610
static void* PolyClipVtxLerpTexCoords(const void* a, const void* b, float t) {
    const uint16_t* from = static_cast<const uint16_t*>(a);
    const uint16_t* to = static_cast<const uint16_t*>(b);
    uint16_t* vert = reinterpret_cast<uint16_t*>(&s_smolVertCache[s_smolVertCacheSize++]);
    vert[0] = static_cast<uint16_t>(static_cast<int32_t>((static_cast<float>(to[0]) - static_cast<float>(from[0])) * t + static_cast<float>(from[0])));
    vert[1] = static_cast<uint16_t>(static_cast<int32_t>((static_cast<float>(to[1]) - static_cast<float>(from[1])) * t + static_cast<float>(from[1])));
    return vert;
}

// PROVEN: bbad3458 IDENTICAL/NONE
// OFFSET: 0x0076D030/0x140455690
static void* PolyClipVtxLerpDepth(const void* a, const void* b, float t) {
    const uint8_t* from = static_cast<const uint8_t*>(a);
    const uint8_t* to = static_cast<const uint8_t*>(b);
    uint8_t* vert = reinterpret_cast<uint8_t*>(&s_smolVertCache[s_smolVertCacheSize++]);
    vert[0] = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(to[0]) - static_cast<float>(from[0])) * t + static_cast<float>(from[0])));
    return vert;
}

// OFFSET: 0x0076D070/0x1404556E0
static int32_t ClipGroupGeometry(CMapObjGroup* group, CMapObj* mapObj, const CImVector& color, float texCoord1, void*& vertices, uint16_t*& indices, uint16_t baseVertex, MAPOBJ_VERTEX_FILL fill, PCInterpolateFunc lerp) {
    const float* depthTable = CWorldMap::GetDepthCoordTable(group->GetLiquidType());
    uint16_t nextIndex = baseVertex;
    int32_t vertexCount = 0;
    SMOPortalRef* refs = &mapObj->m_portalRefs[group->m_portalStart];
    const uint8_t* tile = group->m_liquidTileFlags;

    for (int32_t row = 0; row < group->m_liquidTiles.y; row++) {
        for (int32_t col = 0; col < group->m_liquidTiles.x; col++, tile++) {
            if ((*tile & 0xF) == 0xF || static_cast<int8_t>(*tile) >= 0) {
                continue;
            }

            int32_t row0 = row * group->m_liquidVerts.x;
            int32_t row1 = row0 + group->m_liquidVerts.x;
            int32_t corners[4];
            corners[0] = col + row0;
            corners[1] = col + row1;
            corners[3] = col + row0 + 1;
            corners[2] = col + row1 + 1;
            int32_t offsets[4][2] = { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 } };

            PolyClipper clipper;
            C3Vector points[4];

            for (int32_t i = 0; i < 4; i++) {
                SMOLiquidVert* liquidVert = &group->m_liquidVertices[corners[i]];
                float x = static_cast<float>(col + offsets[i][0]) * 4.1666665f + group->m_liquidCorner.x;
                float y = static_cast<float>(row + offsets[i][1]) * 4.1666665f + group->m_liquidCorner.y;
                points[i] = C3Vector(x, y, liquidVert->m_height);
                clipper.AddVertex(points[i], liquidVert);
            }

            clipper.Begin();

            for (uint32_t p = 0; p < group->m_portalCount; p++) {
                SMOPortalRef& ref = refs[p];
                CMapObjGroup* other = mapObj->GetGroup(ref.m_groupIndex, 0);

                if (other && other->m_liquidCorner.x < points[2].x && other->m_liquidCorner.y < points[2].y && points[0].x < static_cast<float>(other->m_liquidTiles.x) * 4.1666665f + other->m_liquidCorner.x && points[0].y < static_cast<float>(other->m_liquidTiles.y) * 4.1666665f + other->m_liquidCorner.y) {
                    clipper.Clip(mapObj->m_portalInfos[ref.m_portalIndex].m_plane, ref.m_side);
                }
            }

            clipper.End();
            s_smolVertCacheSize = 0;
            int32_t started = 0;

            for (PCResultsIterator it = clipper.GetResults(lerp); !it.HasFinished(); it.Next()) {
                C3Vector pos(0.0f, 0.0f, 0.0f);
                const void* data;
                it.GetVertex(pos, data);

                if (!started) {
                    *indices++ = nextIndex;
                    started = 1;
                }

                fill(group, pos, depthTable, static_cast<const SMOLiquidVert*>(data), color, texCoord1, vertices);
                vertexCount++;
                *indices++ = nextIndex;
                nextIndex++;
            }

            if (started) {
                *indices++ = nextIndex - 1;
            }
        }
    }

    return vertexCount;
}

// OFFSET: [0x0076D790<13, 1>, 0x0076D9E0<13, 0>, 0x0076DBF0<9, 1>, 0x0076DE80<9, 0>, 0x0076E0C0<10, 1>, 0x0076E3F0<10, 0>, 0x0076E6D0<7, 1>, 0x0076EA80<7, 0>]
template <uint32_t FORMAT, int32_t LOCAL>
int32_t FillMapObjVertices(CMapObjGroup* group, const CImVector& color, float texCoord1, void*& vertices) {
    const float* depthTable = FORMAT == 7 || FORMAT == 10 ? CWorldMap::GetDepthCoordTable(group->GetLiquidType()) : NULL;
    int32_t rows = group->m_liquidVerts.y;
    const SMOLiquidVert* vertex = group->m_liquidVertices;
    float y = group->m_liquidCorner.y;
    int32_t cols = group->m_liquidVerts.x;
    float*& out = reinterpret_cast<float*&>(vertices);

    for (int32_t row = 0; row < rows; row++) {
        float x = group->m_liquidCorner.x;

        for (int32_t col = 0; col < cols; col++) {
            out[0] = x;
            out[1] = y;
            out[2] = vertex->m_height;
            uint32_t uv = 3;

            if (FORMAT == 7) {
                out[3] = 0.0f;
                out[4] = 0.0f;
                out[5] = 1.0f;
                *reinterpret_cast<CImVector*>(&out[6]) = color;
                uv = 7;
            } else if (FORMAT == 9 || FORMAT == 10) {
                *reinterpret_cast<CImVector*>(&out[3]) = color;
                uv = 4;
            }

            if (LOCAL) {
                out[uv] = static_cast<float>(vertex->m_texCoord[0]) * 0.00390625f;
                out[uv + 1] = static_cast<float>(vertex->m_texCoord[1]) * 0.00390625f;
            } else {
                out[uv] = static_cast<float>(col);
                out[uv + 1] = static_cast<float>(row);
            }

            if (FORMAT == 7 || FORMAT == 10) {
                out[uv + 2] = texCoord1;
                out[uv + 3] = depthTable[vertex->m_flow[0]];
            }

            out += FORMAT == 7 ? 11 : FORMAT == 10 ? 8 : FORMAT == 9 ? 6 : 5;
            vertex++;
            x += 4.1666665f;
        }

        y += 4.1666665f;
    }

    return cols * rows;
}

// OFFSET: [0x0076D980<13, 1>, 0x0076DB90<13, 0>, 0x0076DE20<9, 1>, 0x0076E050<9, 0>, 0x0076E370<10, 1>, 0x0076E640<10, 0>, 0x0076E9F0<7, 1>, 0x0076ED30<7, 0>]
template <uint32_t FORMAT, int32_t LOCAL>
void FillMapObjVertex(CMapObjGroup* group, const C3Vector& pos, const float* depthTable, const SMOLiquidVert* vertex, const CImVector& color, float texCoord1, void*& vertices) {
    float*& out = reinterpret_cast<float*&>(vertices);
    *reinterpret_cast<C3Vector*>(out) = pos;
    uint32_t uv = 3;

    if (FORMAT == 7) {
        out[3] = 0.0f;
        out[4] = 0.0f;
        out[5] = 1.0f;
        *reinterpret_cast<CImVector*>(&out[6]) = color;
        uv = 7;
    } else if (FORMAT == 9 || FORMAT == 10) {
        *reinterpret_cast<CImVector*>(&out[3]) = color;
        uv = 4;
    }

    if (LOCAL) {
        float v = static_cast<float>(vertex->m_texCoord[1]) * 0.00390625f;
        float u = static_cast<float>(vertex->m_texCoord[0]) * 0.00390625f;
        out[uv] = u;
        out[uv + 1] = v;
    } else {
        float v = (pos.y - group->m_liquidCorner.y) * s_mapObjLiquidTexScale;
        float u = (pos.x - group->m_liquidCorner.x) * s_mapObjLiquidTexScale;
        out[uv] = u;
        out[uv + 1] = v;
    }

    if (FORMAT == 7 || FORMAT == 10) {
        out[uv + 2] = texCoord1;
        out[uv + 3] = depthTable[vertex->m_flow[0]];
    }

    out += FORMAT == 7 ? 11 : FORMAT == 10 ? 8 : FORMAT == 9 ? 6 : 5;
}

// OFFSET: 0x0076D420
void CChunkGeomFactory::Destroy() {
    CChunkGeomFactory::s_allocator.Clear(typeid(CChunkGeomFactory).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x0076D440/0x140455C90
void CChunkGeomFactory::Release() {
    if (--this->m_refCount == 0) {
        CChunkGeomFactory::s_allocator.Delete(this);
    }
}

// PROVEN: 80a9b65f IDENTICAL/NONE
// OFFSET: 0x0076D470/0x140455CD0
bool CChunkGeomFactory::SetTransform(const C44Matrix& transform) {
    if (this->m_transform == transform) {
        return true;
    }

    this->m_transform = transform;
    this->m_dirty = 1;
    return true;
}

// OFFSET: 0x0076D4B0/0x140455D70
float CChunkGeomFactory::GetHeight() {
    if (this->m_liquids.Count()) {
        const CChunkLiquid* liquid = this->m_liquids.m_data[0];
        return Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, 0);
    }

    return 0.0f;
}

// OFFSET: 0x0076D4E0/0x140455DA0
static void BuildChunkIndices(const CChunkLiquid* liquid, uint16_t* indices, uint16_t base, CGxBatch& batch) {
    if (!liquid->HasData()) {
        batch.m_count = static_cast<uint16_t>(BuildGridIndices(indices, base, 9, 2) - indices + batch.m_count);
        return;
    }

    uint16_t* out = indices;
    uint16_t topLeft = base;
    uint16_t topRight = base + 1;
    uint16_t bottomLeft = static_cast<uint16_t>(liquid->m_height - liquid->m_yOffset + 1 + base);
    uint16_t bottomRight = bottomLeft + 1;

    for (int32_t x = liquid->m_xOffset; x < liquid->m_width; x++) {
        for (int32_t y = liquid->m_yOffset; y < liquid->m_height; y++) {
            if (liquid->TileExists(y, x)) {
                out[0] = topLeft;

                if ((x & 1) == (y & 1)) {
                    out[1] = topRight;
                    out[2] = bottomLeft;
                    out[3] = bottomLeft;
                    out[4] = topRight;
                    out[5] = bottomRight;
                } else {
                    out[1] = bottomRight;
                    out[2] = bottomLeft;
                    out[3] = bottomRight;
                    out[4] = topLeft;
                    out[5] = topRight;
                }

                out += 6;
            }

            bottomLeft++;
            bottomRight++;
            topLeft++;
            topRight++;
        }

        bottomLeft++;
        bottomRight++;
        topLeft++;
        topRight++;
    }

    batch.m_count = static_cast<uint16_t>(batch.m_count + (out - indices));
}

// OFFSET: 0x0076D660/0x140455F70
void CAreaGeomFactory::Release() {
    if (--this->m_refCount == 0) {
        CAreaGeomFactory::s_allocator.Delete(this);
    }
}

// PROVEN: e609827c IDENTICAL/NONE
// OFFSET: 0x0076D690/0x140455FB0
bool CAreaGeomFactory::SetTransform(const C44Matrix& transform) {
    if (this->m_transform == transform) {
        return true;
    }

    this->m_transform = transform;
    this->m_dirty = 1;
    return true;
}

// OFFSET: 0x0076D6D0
CAreaGeomFactory::CAreaGeomFactory() {
    this->m_buf = NULL;
    this->m_refCount = 1;
    this->m_dirty = 1;
    this->m_batch.m_primType = GxPrim_TriangleFan;
    this->m_batch.m_start = 0;
    this->m_batch.m_count = 0;
    this->m_batch.m_minIndex = 0;
    this->m_batch.m_maxIndex = 0;
}

// OFFSET: 0x0076EDD0
static void FillChunkVertexP3T2(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepI = ChunkStep(owner->m_chunkY);
    float stepJ = ChunkStep(owner->m_chunkX);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[3] = static_cast<float>(uv[0]) * 0.01171875f;
            v[4] = static_cast<float>(uv[1]) * 0.01171875f;
            *vertices = v + 5;
        }
    }
}

// OFFSET: 0x0076EF90
static void FillChunkVertexP3T2World(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepI = ChunkStep(owner->m_chunkY);
    float stepJ = ChunkStep(owner->m_chunkX);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            v[3] = x * 0.060000002f;
            v[4] = y * 0.060000002f;
            *vertices = v + 5;
        }
    }
}

// OFFSET: 0x0076F130
static void FillChunkVertexP3C4T2(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepI = ChunkStep(owner->m_chunkY);
    float stepJ = ChunkStep(owner->m_chunkX);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            *reinterpret_cast<uint32_t*>(&v[3]) = 0xFFFFFFFF;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[4] = static_cast<float>(uv[0]) * 0.01171875f;
            v[5] = static_cast<float>(uv[1]) * 0.01171875f;
            *vertices = v + 6;
        }
    }
}

// OFFSET: 0x0076F300
static void FillChunkVertexP3C4T2World(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepI = ChunkStep(owner->m_chunkY);
    float stepJ = ChunkStep(owner->m_chunkX);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            *reinterpret_cast<uint32_t*>(&v[3]) = 0xFFFFFFFF;
            v[4] = x * 0.060000002f;
            v[5] = y * 0.060000002f;
            *vertices = v + 6;
        }
    }
}

// OFFSET: 0x0076F4B0
static void FillChunkVertexP3C4T2Alpha(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepI = ChunkStep(owner->m_chunkY);
    float stepJ = ChunkStep(owner->m_chunkX);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        const float* terrain = owner->m_vertices + (i + liquid->m_yOffset + 16 * i);

        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float height = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = height + zOffset;
            uint8_t* color = reinterpret_cast<uint8_t*>(&v[3]);
            color[3] = DepthAlpha(liquid, height, terrain);
            color[2] = 0xFF;
            color[1] = 0xFF;
            color[0] = 0xFF;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[4] = static_cast<float>(uv[0]) * 0.01171875f;
            v[5] = static_cast<float>(uv[1]) * 0.01171875f;
            *vertices = v + 6;
        }
    }
}

// OFFSET: 0x0076F6F0
static void FillChunkVertexP3C4T2AlphaWorld(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepI = ChunkStep(owner->m_chunkY);
    float stepJ = ChunkStep(owner->m_chunkX);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        const float* terrain = owner->m_vertices + (i + liquid->m_yOffset + 16 * i);

        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float height = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = height + zOffset;
            uint8_t* color = reinterpret_cast<uint8_t*>(&v[3]);
            color[2] = 0xFF;
            color[1] = 0xFF;
            color[0] = 0xFF;
            color[3] = DepthAlpha(liquid, height, terrain);
            v[4] = x * 0.060000002f;
            v[5] = y * 0.060000002f;
            *vertices = v + 6;
        }
    }
}

// OFFSET: 0x0076F910
static void FillChunkVertexP3C4T2D2(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            *reinterpret_cast<uint32_t*>(&v[3]) = 0xFFFFFFFF;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[4] = static_cast<float>(uv[0]) * 0.01171875f;
            v[5] = static_cast<float>(uv[1]) * 0.01171875f;
            v[6] = 0.0f;
            v[7] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 8;
        }
    }
}

// OFFSET: 0x0076FB30
static void FillChunkVertexP3C4T2D2World(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            *reinterpret_cast<uint32_t*>(&v[3]) = 0xFFFFFFFF;
            v[4] = x * 0.060000002f;
            v[5] = y * 0.060000002f;
            v[6] = 0.0f;
            v[7] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 8;
        }
    }
}

// OFFSET: 0x0076FD30
static void FillChunkVertexP3C4T2D2Alpha(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        const float* terrain = owner->m_vertices + (i + liquid->m_yOffset + 16 * i);

        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float height = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = height + zOffset;
            uint8_t* color = reinterpret_cast<uint8_t*>(&v[3]);
            color[3] = DepthAlpha(liquid, height, terrain);
            color[2] = 0xFF;
            color[1] = 0xFF;
            color[0] = 0xFF;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[5] = static_cast<float>(uv[1]) * 0.01171875f;
            v[4] = static_cast<float>(uv[0]) * 0.01171875f;
            v[6] = 0.0f;
            v[7] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 8;
        }
    }
}

// OFFSET: 0x0076FFC0
static void FillChunkVertexP3C4T2D2AlphaWorld(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        const float* terrain = owner->m_vertices + (i + liquid->m_yOffset + 16 * i);

        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float height = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = height + zOffset;
            uint8_t* color = reinterpret_cast<uint8_t*>(&v[3]);
            color[2] = 0xFF;
            color[1] = 0xFF;
            color[0] = 0xFF;
            color[3] = DepthAlpha(liquid, height, terrain);
            v[4] = x * 0.060000002f;
            v[5] = y * 0.060000002f;
            v[6] = 0.0f;
            v[7] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 8;
        }
    }
}

// OFFSET: 0x00770230
static void FillChunkVertexP3N3C4T2D2(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            v[3] = 0.0f;
            v[4] = 0.0f;
            v[5] = 1.0f;
            *reinterpret_cast<uint32_t*>(&v[6]) = 0xFFFFFFFF;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[8] = static_cast<float>(uv[1]) * 0.01171875f;
            v[7] = static_cast<float>(uv[0]) * 0.01171875f;
            v[9] = 0.0f;
            v[10] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 11;
        }
    }
}

// OFFSET: 0x00770470
static void FillChunkVertexP3N3C4T2D2World(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index) + zOffset;
            v[3] = 0.0f;
            v[4] = 0.0f;
            v[5] = 1.0f;
            *reinterpret_cast<uint32_t*>(&v[6]) = 0xFFFFFFFF;
            v[7] = x * 0.060000002f;
            v[8] = y * 0.060000002f;
            v[9] = 0.0f;
            v[10] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 11;
        }
    }
}

// OFFSET: 0x00770680
static void FillChunkVertexP3N3C4T2D2Alpha(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        const float* terrain = owner->m_vertices + (i + liquid->m_yOffset + 16 * i);

        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float height = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[0] = transform.d0 + static_cast<float>(i) * stepI;
            v[1] = static_cast<float>(j) * stepJ + transform.d1;
            v[2] = height + zOffset;
            v[3] = 0.0f;
            v[4] = 0.0f;
            v[5] = 1.0f;
            uint8_t* color = reinterpret_cast<uint8_t*>(&v[6]);
            color[3] = DepthAlpha(liquid, height, terrain);
            color[2] = 0xFF;
            color[1] = 0xFF;
            color[0] = 0xFF;
            const uint16_t* uv = Liquid::GetChunkTexCoord0(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            v[8] = static_cast<float>(uv[1]) * 0.01171875f;
            v[7] = static_cast<float>(uv[0]) * 0.01171875f;
            v[9] = 0.0f;
            v[10] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 11;
        }
    }
}

// OFFSET: 0x00770920
static void FillChunkVertexP3N3C4T2D2AlphaWorld(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices) {
    const CMapChunk* owner = liquid->m_owner;
    float stepJ = ChunkStep(owner->m_chunkX);
    float stepI = ChunkStep(owner->m_chunkY);
    const float* depthTable = CWorldMap::GetDepthCoordTable(liquid->m_liquidType);
    float zOffset = transform.d2 - owner->m_position.z;
    uint32_t index = 0;

    for (int32_t i = liquid->m_xOffset; i < liquid->m_width + 1; i++) {
        const float* terrain = owner->m_vertices + (i + liquid->m_yOffset + 16 * i);

        for (int32_t j = liquid->m_yOffset; j < liquid->m_height + 1; j++, index++) {
            float* v = *vertices;
            float height = Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index);
            float x = transform.d0 + static_cast<float>(i) * stepI;
            float y = static_cast<float>(j) * stepJ + transform.d1;
            v[0] = x;
            v[1] = y;
            v[2] = height + zOffset;
            v[3] = 0.0f;
            v[4] = 0.0f;
            v[5] = 1.0f;
            uint8_t* color = reinterpret_cast<uint8_t*>(&v[6]);
            color[2] = 0xFF;
            color[1] = 0xFF;
            color[0] = 0xFF;
            color[3] = DepthAlpha(liquid, height, terrain);
            v[7] = x * 0.060000002f;
            v[8] = y * 0.060000002f;
            v[9] = 0.0f;
            v[10] = depthTable[Liquid::GetChunkDepth(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, index)];
            *vertices = v + 11;
        }
    }
}

static void FillAreaVertex(float*& v, uint32_t format, float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;

    switch (format) {
        case 13:
            v[3] = x * 0.060000002f;
            v[4] = y * 0.060000002f;
            v += 5;
            break;

        case 9:
            *reinterpret_cast<uint32_t*>(&v[3]) = 0xFFFFFFFF;
            v[4] = x * 0.060000002f;
            v[5] = y * 0.060000002f;
            v += 6;
            break;

        case 10:
            *reinterpret_cast<uint32_t*>(&v[3]) = 0xFFFFFFFF;
            v[4] = x * 0.060000002f;
            v[5] = y * 0.060000002f;
            v[6] = 0.0f;
            v[7] = 1.0f;
            v += 8;
            break;

        default:
            v[3] = 0.0f;
            v[4] = 0.0f;
            v[5] = 1.0f;
            *reinterpret_cast<uint32_t*>(&v[6]) = 0xFFFFFFFF;
            v[7] = x * 0.060000002f;
            v[8] = y * 0.060000002f;
            v[9] = 0.0f;
            v[10] = 1.0f;
            v += 11;
            break;
    }
}

static void FillAreaVertices(const CMapAreaLow* area, const C44Matrix& transform, float** vertices, uint32_t format) {
    float z = transform.d2 - area->m_oceanHeight;
    float* v = *vertices;
    float x = 0.0f;

    for (uint32_t i = 0; i < 17; i++, x -= 33.333332f) {
        float y = 0.0f;

        for (uint32_t j = 0; j < 17; j++, y -= 33.333332f) {
            FillAreaVertex(v, format, transform.d0 + x, y + transform.d1, z);
        }
    }

    x = -16.666666f;

    for (uint32_t i = 0; i < 16; i++, x -= 33.333332f) {
        float y = -16.666666f;

        for (uint32_t j = 0; j < 16; j++, y -= 33.333332f) {
            FillAreaVertex(v, format, transform.d0 + x, y + transform.d1, z);
        }
    }

    *vertices = v;
}

// OFFSET: 0x00770BA0
static void FillAreaVertexP3T2(const CMapAreaLow* area, const C44Matrix& transform, float** vertices) {
    FillAreaVertices(area, transform, vertices, 13);
}

// OFFSET: 0x00771130
static void FillAreaVertexP3C4T2(const CMapAreaLow* area, const C44Matrix& transform, float** vertices) {
    FillAreaVertices(area, transform, vertices, 9);
}

// OFFSET: 0x00771720
static void FillAreaVertexP3C4T2D2(const CMapAreaLow* area, const C44Matrix& transform, float** vertices) {
    FillAreaVertices(area, transform, vertices, 10);
}

// OFFSET: 0x00771DC0
static void FillAreaVertexP3N3C4T2D2(const CMapAreaLow* area, const C44Matrix& transform, float** vertices) {
    FillAreaVertices(area, transform, vertices, 7);
}

// OFFSET: 0x00772560
CMeshGeomFactory* CMeshGeomFactory::Create(CMapObj* mapObj, CMapObjGroup* mapObjGroup) {
    CMeshGeomFactory* factory = CMeshGeomFactory::s_allocator.New<CMeshGeomFactory>(0);
    factory->m_mapObj = mapObj;
    factory->m_mapObjGroup = mapObjGroup;
    return factory;
}

CMeshGeomFactory::CMeshGeomFactory() {
    this->m_refCount = 1;
    this->m_mapObj = NULL;
    this->m_mapObjGroup = NULL;
    this->m_batch = NULL;
    this->m_diffuseColor = CImVector(0xFFFFFFFF);
    this->m_texCoord1ValueX = 0.0f;
}

// OFFSET: 0x007725C0/0x14045BF50
CMeshGeomFactory::~CMeshGeomFactory() {
    if (this->m_mapObjGroup) {
        this->m_mapObjGroup->FreeLiquidVB();
    }
}

// OFFSET: 0x00772600
CAreaGeomFactory* CAreaGeomFactory::Create(const CMapAreaLow* area) {
    CAreaGeomFactory* factory = CAreaGeomFactory::s_allocator.New<CAreaGeomFactory>(0);
    factory->m_area = area;
    return factory;
}

// OFFSET: 0x00772620/0x14045C060
CAreaGeomFactory::~CAreaGeomFactory() {
    if (this->m_buf) {
        CMapChunkBuf::Free(this->m_buf);
        this->m_buf = NULL;
    }
}

// OFFSET: 0x007726D0/0x14045C180
void CMeshGeomFactory::GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5) {
    if (this->m_mapObjGroup->m_liquidMaterialId >= this->m_mapObj->m_materialCount) {
        SErrDisplayAppFatal("Water in chunk \"%s\" of object \"%s\" has no materialId.", this->m_mapObjGroup->m_groupName, static_cast<const char*>(this->m_mapObj->m_name));
    }

    uint32_t vertexCount = this->m_mapObjGroup->m_liquidVerts.x * this->m_mapObjGroup->m_liquidVerts.y + 6 * this->m_mapObjGroup->GetNumSharedTiles();

    if (!this->m_mapObjGroup->HasLiquidVB(this->m_batch)) {
        this->m_mapObjGroup->AllocLiquidVB(format, vertexCount, 3 * vertexCount, this->m_batch);
    }

    vertexBuf = this->m_mapObjGroup->m_liquidVertsBlock->m_buf;
    indexBuf = this->m_mapObjGroup->m_liquidIndicesBlock->m_buf;

    if (vertexBuf->unk_001C && vertexBuf->unk_001D && indexBuf->unk_001C && indexBuf->unk_001D) {
        batch = *this->m_batch;
        return;
    }

    void* vertices = g_theGxDevicePtr->BufLock(vertexBuf);
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    uint16_t* indexStart = indices;

    if (!vertices || !indices) {
        if (vertices) {
            CGxBuf* buf = vertexBuf;
            g_theGxDevicePtr->BufUnlock(buf, 0);
            buf->unk_001C = 1;
        } else if (indices) {
            CGxBuf* buf = indexBuf;
            g_theGxDevicePtr->BufUnlock(buf, 0);
            buf->unk_001C = 1;
        }

        return;
    }

    MAPOBJ_VERTEX_FILL fillVertex = NULL;
    MAPOBJ_VERTICES_FILL fillVertices = NULL;
    PCInterpolateFunc lerp = NULL;

    switch (format) {
        case 7:
            if (a5) {
                fillVertices = FillMapObjVertices<7, 1>;
                fillVertex = FillMapObjVertex<7, 1>;
            } else {
                fillVertices = FillMapObjVertices<7, 0>;
                fillVertex = FillMapObjVertex<7, 0>;
            }

            lerp = PolyClipVtxLerpDepth;
            break;

        case 9:
            if (a5) {
                fillVertices = FillMapObjVertices<9, 1>;
                fillVertex = FillMapObjVertex<9, 1>;
            } else {
                fillVertices = FillMapObjVertices<9, 0>;
                fillVertex = FillMapObjVertex<9, 0>;
            }

            lerp = PolyClipVtxLerpTexCoords;
            break;

        case 10:
            if (a5) {
                fillVertices = FillMapObjVertices<10, 1>;
                fillVertex = FillMapObjVertex<10, 1>;
            } else {
                fillVertices = FillMapObjVertices<10, 0>;
                fillVertex = FillMapObjVertex<10, 0>;
            }

            lerp = PolyClipVtxLerpDepth;
            break;

        case 13:
            if (a5) {
                fillVertices = FillMapObjVertices<13, 1>;
                fillVertex = FillMapObjVertex<13, 1>;
            } else {
                fillVertices = FillMapObjVertices<13, 0>;
                fillVertex = FillMapObjVertex<13, 0>;
            }

            lerp = PolyClipVtxLerpTexCoords;
            break;

        default:
            break;
    }

    int32_t count = fillVertices(this->m_mapObjGroup, this->m_diffuseColor, this->m_texCoord1ValueX, vertices);
    BuildMapObjLiquidIndices(this->m_mapObjGroup, indices);
    count += ClipGroupGeometry(this->m_mapObjGroup, this->m_mapObj, this->m_diffuseColor, this->m_texCoord1ValueX, vertices, indices, static_cast<uint16_t>(count), fillVertex, lerp);

    if (vertices) {
        CGxBuf* buf = vertexBuf;
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
    }

    if (indices) {
        CGxBuf* buf = indexBuf;
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
    }

    CGxBatch* meshBatch = this->m_batch;
    meshBatch->m_primType = GxPrim_TriangleStrip;
    meshBatch->m_start = 0;
    meshBatch->m_count = static_cast<uint32_t>(indices - indexStart);
    meshBatch->m_minIndex = 0;
    meshBatch->m_maxIndex = static_cast<uint16_t>(count - 1);
    batch = *this->m_batch;
}

// OFFSET: 0x007729D0
CChunkGeomFactory::CChunkGeomFactory() {
    this->m_refCount = 1;
    this->m_dirty = 1;
    this->m_buf = NULL;
    this->m_batch.m_primType = GxPrim_TriangleFan;
    this->m_batch.m_start = 0;
    this->m_batch.m_count = 0;
    this->m_batch.m_minIndex = 0;
    this->m_batch.m_maxIndex = 0;
}

// OFFSET: 0x00772A70/0x14045C520
CChunkGeomFactory::~CChunkGeomFactory() {
    if (this->m_buf) {
        CMapChunkBuf::Free(this->m_buf);
        this->m_buf = NULL;
    }
}

// OFFSET: 0x00772AE0/0x14045C5B0
void CAreaGeomFactory::GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5) {
    if (this->m_buf && this->m_buf->Valid() && !this->m_dirty) {
        vertexBuf = this->m_buf->m_vertexBuf;
        indexBuf = this->m_buf->m_indexBuf;
        batch = this->m_batch;
        return;
    }

    if (!this->m_buf) {
        this->m_buf = CMapChunkBuf::Alloc(545, 3072, format);
    }

    vertexBuf = this->m_buf->m_vertexBuf;
    indexBuf = this->m_buf->m_indexBuf;

    if (this->m_dirty) {
        vertexBuf->unk_001C = 0;
        indexBuf->unk_001C = 0;
    }

    if (!vertexBuf->unk_001C || !vertexBuf->unk_001D) {
        float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(vertexBuf));

        if (vertices) {
            AREA_VERTEX_FILL fill = NULL;

            switch (format) {
                case 9:
                    fill = FillAreaVertexP3C4T2;
                    break;

                case 10:
                    fill = FillAreaVertexP3C4T2D2;
                    break;

                case 13:
                    fill = FillAreaVertexP3T2;
                    break;

                default:
                    fill = FillAreaVertexP3N3C4T2D2;
                    break;
            }

            fill(this->m_area, this->m_transform, &vertices);
            g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
            vertexBuf->unk_001C = 1;
        }
    }

    if (!indexBuf->unk_001C || !indexBuf->unk_001D) {
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = 3072;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 544;
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
        BuildAreaIndices(indices);
        g_theGxDevicePtr->BufUnlock(indexBuf, 0);
        indexBuf->unk_001C = 1;
        this->m_batch = batch;
    }

    this->m_format = format;
    this->m_dirty = 0;
}

// OFFSET: 0x00772CB0
CChunkGeomFactory* CChunkGeomFactory::Create(const TSGrowableArray<CChunkLiquid*>& liquids) {
    CChunkGeomFactory* factory = CChunkGeomFactory::s_allocator.New<CChunkGeomFactory>(0);

    for (uint32_t i = 0; i < liquids.m_count; i++) {
        factory->m_liquids.Add(1, &liquids.m_data[i]);
    }

    return factory;
}

// OFFSET: 0x00772DA0/0x14045C8E0
void CChunkGeomFactory::GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5) {
    if (this->m_buf && this->m_buf->Valid() && !this->m_dirty) {
        vertexBuf = this->m_buf->m_vertexBuf;
        indexBuf = this->m_buf->m_indexBuf;
        batch = this->m_batch;
        return;
    }

    s_liquidVertexCounts.Clear();

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    for (uint32_t i = 0; i < this->m_liquids.Count(); i++) {
        const CChunkLiquid* liquid = this->m_liquids.m_data[i];
        uint16_t height = static_cast<uint16_t>(liquid->m_height - liquid->m_yOffset);
        uint16_t width = static_cast<uint16_t>(liquid->m_width - liquid->m_xOffset);
        uint16_t count = static_cast<uint16_t>((height + 1) * (width + 1));
        s_liquidVertexCounts.Add(1, &count);
        vertexCount += count;
        indexCount += 6 * height * width;
    }

    if (!this->m_buf) {
        this->m_buf = CMapChunkBuf::Alloc(static_cast<uint16_t>(vertexCount), static_cast<uint16_t>(indexCount), format);
    }

    vertexBuf = this->m_buf->m_vertexBuf;
    indexBuf = this->m_buf->m_indexBuf;

    if (this->m_dirty) {
        vertexBuf->unk_001C = 0;
        indexBuf->unk_001C = 0;
    }

    if (!vertexBuf->unk_001C || !vertexBuf->unk_001D) {
        float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(vertexBuf));

        if (vertices) {
            const CChunkLiquid* first = this->m_liquids.m_data[0];
            CHUNK_VERTEX_FILL fill = NULL;

            switch (format) {
                case 7:
                    if (Liquid::GetWaterDetail()) {
                        fill = a5 ? FillChunkVertexP3N3C4T2D2Alpha : FillChunkVertexP3N3C4T2D2AlphaWorld;
                    } else {
                        fill = a5 ? FillChunkVertexP3N3C4T2D2 : FillChunkVertexP3N3C4T2D2World;
                    }

                    break;

                case 9:
                    if (Liquid::GetWaterDetail()) {
                        fill = a5 ? FillChunkVertexP3C4T2Alpha : FillChunkVertexP3C4T2AlphaWorld;
                    } else {
                        fill = a5 ? FillChunkVertexP3C4T2 : FillChunkVertexP3C4T2World;
                    }

                    break;

                case 10:
                    if (Liquid::GetWaterDetail()) {
                        fill = a5 ? FillChunkVertexP3C4T2D2Alpha : FillChunkVertexP3C4T2D2AlphaWorld;
                    } else {
                        fill = a5 ? FillChunkVertexP3C4T2D2 : FillChunkVertexP3C4T2D2World;
                    }

                    break;

                case 13:
                    fill = a5 ? FillChunkVertexP3T2 : FillChunkVertexP3T2World;
                    break;

                default:
                    break;
            }

            for (uint32_t i = 0; i < this->m_liquids.Count(); i++) {
                const CChunkLiquid* liquid = this->m_liquids.m_data[i];
                C44Matrix transform = this->m_transform;
                C3Vector offset(liquid->m_position.x - first->m_position.x, liquid->m_position.y - first->m_position.y, liquid->m_position.z - first->m_position.z);
                transform.Translate(offset);
                fill(liquid, transform, &vertices);
            }

            g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
            vertexBuf->unk_001C = 1;
        }
    }

    if (!indexBuf->unk_001C || !indexBuf->unk_001D) {
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = 0;
        batch.m_minIndex = 0;
        batch.m_maxIndex = static_cast<uint16_t>(vertexCount - 1);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
        uint16_t base = 0;

        for (uint32_t i = 0; i < this->m_liquids.Count(); i++) {
            BuildChunkIndices(this->m_liquids.m_data[i], indices + batch.m_count, base, batch);
            base += s_liquidVertexCounts.m_data[i];
        }

        g_theGxDevicePtr->BufUnlock(indexBuf, 0);
        indexBuf->unk_001C = 1;
        this->m_batch = batch;
    }

    this->m_dirty = 0;
    this->m_format = format;
}

// PROVEN: 56b853f9 IDENTICAL/IDENTICAL
// OFFSET: 0x0076CA20/0x140586E10
float CAreaGeomFactory::GetHeight() {
    return 0.0f;
}

// PROVEN: 442796bc IDENTICAL/NONE
// OFFSET: 0x007AAB20
bool CMeshGeomFactory::SetTransform(const C44Matrix& transform) {
    return false;
}

// PROVEN: 82f48af9 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CMeshGeomFactory::GetWorldSpace() {
    return false;
}

// PROVEN: e7240de9 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CChunkGeomFactory::GetWorldSpace() {
    return true;
}

// PROVEN: 2569a902 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CAreaGeomFactory::GetWorldSpace() {
    return true;
}
