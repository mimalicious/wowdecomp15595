#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkBuf.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/WorldMap.h"

typedef void (*CHUNK_VERTEX_FILL)(const CChunkLiquid* liquid, const C44Matrix& transform, float** vertices);
typedef void (*AREA_VERTEX_FILL)(const CMapAreaLow* area, const C44Matrix& transform, float** vertices);

// STATIC: 0x00E8913C
CDataAllocator CMeshGeomFactory::s_allocator(sizeof(CMeshGeomFactory), 4);

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
// OFFSET: 0x0076CA00
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

// OFFSET: 0x0076CE20
void CMeshGeomFactory::SetDiffuseColor(const CImVector& color) {
    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        this->m_diffuseColor = CImVector(color.r, color.g, color.b, color.a);
    } else {
        this->m_diffuseColor = color;
    }
}

// OFFSET: 0x0076CE70/0x140455490
void CMeshGeomFactory::Release() {
    if (this->m_refCount-- == 1) {
        CMeshGeomFactory::s_allocator.Delete(this);
    }
}

// OFFSET: 0x0076D420
void CChunkGeomFactory::Destroy() {
    CChunkGeomFactory::s_allocator.Clear(typeid(CChunkGeomFactory).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x0076D440/0x140455C90
void CChunkGeomFactory::Release() {
    if (this->m_refCount-- == 1) {
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
    if (!this->m_liquids.Count()) {
        return 0.0f;
    }

    const CChunkLiquid* liquid = this->m_liquids.m_data[0];
    return Liquid::GetChunkHeight(liquid->m_liquidObject, liquid->m_vertexData, liquid->m_vertexCount, 0);
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
    if (this->m_refCount-- == 1) {
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
    this->unk_0010 = 0;
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
