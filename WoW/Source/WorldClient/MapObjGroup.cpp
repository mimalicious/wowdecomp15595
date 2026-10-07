#include "WoW/Source/WorldClient/MapObjGroup.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Common/Types.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D03F34
static float s_invLiquidTileSize = 0.24000001f;

// STATIC: 0x00E4FE38
uint32_t World::TriData::s_statusFlags;

// STATIC: 0x00E4FE3C
uint32_t World::TriData::s_nBatches;

// STATIC: 0x00E4FE40
uint32_t World::TriData::s_faceIndexCursor;

// STATIC: 0x00E4FE44
uint32_t World::TriData::s_indexCursor;

// STATIC: 0x00E4FE48
uint32_t World::TriData::unk_E4FE48;

// STATIC: 0x00E4FE50
uint16_t World::TriData::s_faceIndexPool[0x4000];

// STATIC: 0x00E57E50
uint16_t World::TriData::s_indexPool[0xC000];

// STATIC: 0x00E706E8
World::TriData::Batch World::TriData::s_batches[32];

// STATIC: 0x00EAB230
uint16_t BspQuery::s_testFaces[0x2000];

// STATIC: 0x00EAF230
uint16_t BspQuery::s_hitFaces[0x2000];

// STATIC: 0x00EB3230
uint32_t BspQuery::s_testFaceSub;

// STATIC: 0x00EB3234
uint32_t BspQuery::s_hitFaceSub;

// STATIC: 0x00D03F38
VBBList CMapObjGroup::s_vertexList;

// STATIC: 0x00D03F60
VBBList CMapObjGroup::s_indexList;

// STATIC: 0x00D03F88
TSExplicitList<CMapObjGroup::VertArray, offsetof(CMapObjGroup::VertArray, m_link)> CMapObjGroup::s_freeVertArrays;

static uint32_t SwapColor(uint32_t color) {
    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        return (color & 0xFF00FF00) | ((color >> 16) & 0xFF) | ((color & 0xFF) << 16);
    }

    return color;
}

// OFFSET: 0x00740730
World::TriData::Batch* World::TriData::AllocBatch() {
    if (World::TriData::s_nBatches + 1 >= 32) {
        World::TriData::s_statusFlags |= 0x1;
        return NULL;
    }

    Batch* batch = &World::TriData::s_batches[World::TriData::s_nBatches++];
    batch->m_matrix = NULL;
    batch->m_vertexList = NULL;
    batch->m_normalList = NULL;
    batch->unk_000C = 0;
    batch->unk_0010 = 0;
    batch->m_indices = NULL;
    batch->m_faceIndices = NULL;
    batch->m_indexCount = 0;
    batch->m_faceCount = 0;
    batch->m_minVertexIndex = 0;
    batch->m_maxVertexIndex = 0;
    batch->m_def = NULL;
    batch->m_minVertexIndex = 0xFFFF;
    return batch;
}

// OFFSET: 0x00789710
uint16_t* World::TriData::AllocIndices(uint32_t count) {
    uint32_t cursor = World::TriData::s_indexCursor;

    if (cursor + count >= 0xC000) {
        World::TriData::s_statusFlags |= 0x1;
        return NULL;
    }

    World::TriData::s_indexCursor += count;
    return &World::TriData::s_indexPool[cursor];
}

// OFFSET: 0x0078D740
BspQuery::~BspQuery() {
    while (BspQuery::s_testFaceSub) {
        BspQuery::s_testFaceSub--;
        this->m_faces[BspQuery::s_testFaces[BspQuery::s_testFaceSub]].m_flags &= ~0x80;
    }

    BspQuery::s_hitFaceSub = 0;
    BspQuery::s_testFaceSub = 0;
}

// OFFSET: 0x0078DB40
void BspQuery_Segment::operator()(uint16_t face) {
    uint16_t ignore = this->m_faceIgnoreFlags;
    SMOPoly& poly = this->m_faces[face];

    if (ignore & poly.m_flags) {
        return;
    }

    uint16_t ignoreMask = (poly.m_materialId == 0xFF || !this->m_materials[poly.m_materialId].m_blendMode) ? 0x200 : 0x100;

    if (ignore & ignoreMask) {
        return;
    }

    if (BspQuery::s_testFaceSub >= 0x2000) {
        if (this->m_overflowFlags) {
            *this->m_overflowFlags |= 0x1;
        }

        return;
    }

    BspQuery::s_testFaces[BspQuery::s_testFaceSub++] = face;
    this->m_faces[face].m_flags |= 0x80;

    float t = 0.0f;

    if (!NTempest::Intersect(this->m_ray, this->m_vertexList, &this->m_indices[3 * face], &t, NULL, 0.0020000001f)) {
        return;
    }

    if (t < 0.0f || t > this->m_maxT) {
        return;
    }

    this->m_maxT = t;
    BspQuery::s_hitFaces[0] = face;
    BspQuery::s_hitFaceSub = 1;
    *this->m_hitT = this->m_oosegMag * t;

    if (*this->m_hitT > this->m_origHitT) {
        *this->m_hitT = this->m_origHitT;
    }
}

// OFFSET: 0x0078DC60
bool BspQuery_Segment::GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node) {
    if (!g_BspDigestCache) {
        return false;
    }

    CAaBspNodeDigest* digest = g_BspDigestCache->GetDigest(aaBsp, node, this->m_faces, this->m_vertexList, this->m_indices);

    if (!digest) {
        return false;
    }

    float minX = this->m_seg.b.x;
    float maxX = this->m_seg.t.x;
    float minY = this->m_seg.b.y;
    float minZ = this->m_seg.b.z;
    float maxY = this->m_seg.t.y;
    float maxZ = this->m_seg.t.z;

    if (minX > maxX) {
        minX = this->m_seg.t.x;
        maxX = this->m_seg.b.x;
    }

    if (minY > maxY) {
        minY = this->m_seg.t.y;
        maxY = this->m_seg.b.y;
    }

    if (minZ > maxZ) {
        minZ = this->m_seg.t.z;
        maxZ = this->m_seg.b.z;
    }

    minX -= 0.0099999998f;
    maxX += 0.0099999998f;
    minY -= 0.0099999998f;
    maxY += 0.0099999998f;
    minZ -= 0.0099999998f;
    maxZ += 0.0099999998f;

    uint8_t outcodes[452];

    for (int32_t i = 0; i < static_cast<int32_t>(digest->m_nVertices); i++) {
        const C3Vector& vertex = digest->m_vertices[i];
        uint8_t code = 0;

        if (minX > vertex.x) {
            code = 0x20;
        }

        if (vertex.x > maxX) {
            code |= 0x10;
        }

        if (minY > vertex.y) {
            code |= 0x8;
        }

        if (vertex.y > maxY) {
            code |= 0x4;
        }

        if (minZ > vertex.z) {
            code |= 0x2;
        }

        if (vertex.z > maxZ) {
            code |= 0x1;
        }

        outcodes[i] = code;
    }

    for (int32_t i = 0; i < digest->m_nFaces; i++) {
        uint16_t ignore = this->m_faceIgnoreFlags;
        uint16_t face = digest->m_faceIndices[i];

        if (ignore & digest->m_faceFlags[i]) {
            continue;
        }

        SMOPoly& poly = this->m_faces[face];

        if (ignore & poly.m_flags) {
            continue;
        }

        uint16_t ignoreMask = (poly.m_materialId == 0xFF || !this->m_materials[poly.m_materialId].m_blendMode) ? 0x200 : 0x100;

        if (ignore & ignoreMask) {
            continue;
        }

        if (BspQuery::s_testFaceSub >= 0x2000) {
            if (this->m_overflowFlags) {
                *this->m_overflowFlags |= 0x1;
            }

            break;
        }

        BspQuery::s_testFaces[BspQuery::s_testFaceSub++] = face;
        poly.m_flags |= 0x80;

        const uint16_t* verts = &digest->m_faceVertexIndices[3 * i];

        if (outcodes[verts[0]] & outcodes[verts[1]] & outcodes[verts[2]] & 0x3F) {
            continue;
        }

        float t = 0.0f;

        if (!NTempest::Intersect(this->m_ray, digest->m_vertices, verts, &t, NULL, 0.0020000001f)) {
            continue;
        }

        if (t < 0.0f || t > this->m_maxT) {
            continue;
        }

        this->m_maxT = t;
        BspQuery::s_hitFaces[0] = face;
        BspQuery::s_hitFaceSub = 1;
        *this->m_hitT = this->m_oosegMag * t;

        if (*this->m_hitT > this->m_origHitT) {
            *this->m_hitT = this->m_origHitT;
        }
    }

    return true;
}

// OFFSET: 0x0078E370/0x14047F230
void CMapObjGroup::AsyncFreeCallback(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// OFFSET: 0x0078E3A0/0x14047F270
int32_t CMapObjGroup::HasLiquidVB(CGxBatch*& batch) {
    if (!this->m_liquidVertsBlock || !this->m_liquidIndicesBlock) {
        return 0;
    }

    batch = &this->m_liquidBatch;
    return 1;
}

// OFFSET: 0x0078E3D0/0x14047F290
uint32_t CMapObjGroup::GetLiquidType() const {
    return this->m_liquidType;
}

// OFFSET: 0x0078E4C0
World::TriData::Batch* World::TriData::AllocBatch(uint32_t indexCount, uint32_t faceCount) {
    if (World::TriData::s_nBatches + 1 >= 32 || indexCount + World::TriData::s_indexCursor >= 0xC000 || faceCount + World::TriData::s_faceIndexCursor >= 0x4000) {
        World::TriData::s_statusFlags |= 0x1;
        return NULL;
    }

    Batch* batch = &World::TriData::s_batches[World::TriData::s_nBatches++];
    batch->m_matrix = NULL;
    batch->m_vertexList = NULL;
    batch->m_normalList = NULL;
    batch->unk_000C = 0;
    batch->unk_0010 = 0;
    batch->m_indices = NULL;
    batch->m_faceIndices = NULL;
    batch->m_indexCount = 0;
    batch->m_faceCount = 0;
    batch->m_minVertexIndex = 0;
    batch->m_maxVertexIndex = 0;
    batch->m_def = NULL;
    batch->m_minVertexIndex = 0xFFFF;
    return batch;
}

// OFFSET: 0x0078E5A0
void CMapObjGroup::LinkToAsyncHead(void* param) {
    CMapObjGroup* group = static_cast<CMapObjGroup*>(param);

    if (group->m_asyncObject) {
        AsyncFileReadLinkToPriority(group->m_asyncObject, 1);
        return;
    }

    for (uint32_t i = 0; i < group->m_batchCount; i++) {
        CMapObjMaterial* material = &group->m_parent->m_runtimeMaterials[group->m_batches[i].m_materialId];

        for (uint32_t j = 0; j < 3; j++) {
            CTexture* texture = material->m_textures[j];

            if (texture && !TextureIsLoaded(texture)) {
                TextureIncreasePriority(texture);
            }
        }
    }
}

// OFFSET: 0x0078E7C0
BspQuery_Segment::BspQuery_Segment(SMOPoly* faces, C3Vector* vertexList, uint16_t* indices, const C3Segment& seg, float* hitT, uint16_t faceIgnoreFlags, SMOMaterial* materials) {
    this->m_faces = faces;
    this->m_vertexList = vertexList;
    this->m_indices = indices;
    this->m_overflowFlags = NULL;
    this->m_hitT = hitT;
    this->m_materials = materials;
    this->m_faceIgnoreFlags = faceIgnoreFlags | 0x80;
    this->m_ray.origin = seg.b;
    this->m_ray.dir = C3Vector(seg.t.x - seg.b.x, seg.t.y - seg.b.y, seg.t.z - seg.b.z);

    float length = sqrt(this->m_ray.dir.z * this->m_ray.dir.z + this->m_ray.dir.y * this->m_ray.dir.y + this->m_ray.dir.x * this->m_ray.dir.x);
    this->m_maxT = length * *hitT;
    this->m_seg = seg;
    this->m_origHitT = *hitT;
    this->m_oosegMag = 1.0f / length;
    this->m_ray.dir.x = this->m_ray.dir.x * (1.0f / length);
    this->m_ray.dir.y = this->m_ray.dir.y * (1.0f / length);
    this->m_ray.dir.z = this->m_ray.dir.z * (1.0f / length);
}

// OFFSET: 0x0078EA20
void CMapObjGroup::GetTrisFromQuery(World::TriData* triData, BspQuery* query, const CMapObjDef* mapObjDef, uint32_t statusFlags) {
    if (World::GetEnables() & 0x4000000) {
        for (uint32_t i = 0; i < BspQuery::s_testFaceSub; i++) {
            const uint16_t* indices = &this->m_indices[3 * BspQuery::s_testFaces[i]];
            C3Vector c = this->m_vertices[indices[2]] * mapObjDef->m_mat;
            C3Vector b = this->m_vertices[indices[1]] * mapObjDef->m_mat;
            C3Vector a = this->m_vertices[indices[0]] * mapObjDef->m_mat;
            CFacet facet(a, b, c);
            CMap::TestQueryAdd(facet, CImVector(0x7FFF0000), NULL);
        }

        for (uint32_t i = 0; i < BspQuery::s_hitFaceSub; i++) {
            const uint16_t* indices = &this->m_indices[3 * BspQuery::s_hitFaces[i]];
            C3Vector a = this->m_vertices[indices[0]] * mapObjDef->m_mat;
            C3Vector b = this->m_vertices[indices[1]] * mapObjDef->m_mat;
            C3Vector c = this->m_vertices[indices[2]] * mapObjDef->m_mat;
            CFacet facet(a, b, c);
            CMap::TestQueryAdd(facet, CImVector(0x7F00FF00), NULL);
        }
    }

    World::TriData::s_statusFlags |= statusFlags;

    if (!BspQuery::s_hitFaceSub || BspQuery::s_hitFaceSub > 0x2000) {
        return;
    }

    World::TriData::Batch* batch = World::TriData::AllocBatch(3 * BspQuery::s_hitFaceSub, BspQuery::s_hitFaceSub);

    if (!batch) {
        return;
    }

    batch->m_def = mapObjDef;

    uint16_t* indices;

    if (World::TriData::s_indexCursor + 3 * BspQuery::s_hitFaceSub >= 0xC000) {
        World::TriData::s_statusFlags |= 0x1;
        indices = NULL;
    } else {
        indices = &World::TriData::s_indexPool[World::TriData::s_indexCursor];
        World::TriData::s_indexCursor += 3 * BspQuery::s_hitFaceSub;
    }

    uint16_t* faceIndices;

    if (World::TriData::s_faceIndexCursor + BspQuery::s_hitFaceSub >= 0x4000) {
        World::TriData::s_statusFlags |= 0x1;
        faceIndices = NULL;
    } else {
        faceIndices = &World::TriData::s_faceIndexPool[World::TriData::s_faceIndexCursor];
        World::TriData::s_faceIndexCursor += BspQuery::s_hitFaceSub;
    }

    batch->m_matrix = &mapObjDef->m_mat;
    batch->m_vertexList = this->m_vertices;
    batch->m_normalList = this->m_normals;
    batch->m_faceCount = BspQuery::s_hitFaceSub;
    batch->m_indexCount = 3 * BspQuery::s_hitFaceSub;

    uint16_t* out = indices;

    for (uint32_t i = 0; i < BspQuery::s_hitFaceSub; i++) {
        uint16_t face = BspQuery::s_hitFaces[i];
        faceIndices[i] = face;

        for (uint32_t j = 0; j < 3; j++) {
            uint16_t index = this->m_indices[3 * face + j];
            out[j] = index;

            if (index < batch->m_minVertexIndex) {
                batch->m_minVertexIndex = index;
            }

            if (index > batch->m_maxVertexIndex) {
                batch->m_maxVertexIndex = index;
            }
        }

        out += 3;
    }

    batch->m_indices = indices;
    batch->m_faceIndices = faceIndices;
}

// OFFSET: 0x0078F240/0x140480130
void CMapObjGroup::InitPtrs() {
    this->m_groupName = NULL;
    this->m_polys = NULL;
    this->m_vertices = NULL;
    this->m_normals = NULL;
    this->m_texCoords = NULL;
    this->m_texCoords2 = NULL;
    this->m_indices = NULL;
    this->m_stripIndices = NULL;
    this->m_batches = NULL;
    this->m_shadowBatches = NULL;
    this->m_stripBatches = NULL;
    this->m_lightRefs = NULL;
    this->m_doodadRefs = NULL;
    this->m_colors = NULL;
    this->m_colors2 = NULL;
    this->m_tangentStarts = NULL;
    this->m_tangents = NULL;
    this->m_liquidVertices = NULL;
    this->m_liquidTileFlags = NULL;
    this->m_liquidVertArray = NULL;
    this->m_portalStart = 0;
    this->m_portalCount = 0;
    this->unk_0194 = 0;
    this->m_polyCount = 0;
    this->m_vertexCount = 0;
    this->m_normalCount = 0;
    this->m_texCoordCount = 0;
    this->m_indexCount = 0;
    this->m_stripIndexCount = 0;
    this->m_batchCount = 0;
    this->m_shadowBatchCount = 0;
    this->m_lightRefCount = 0;
    this->m_doodadRefCount = 0;
    this->m_colorCount = 0;
    this->m_liquidVerts.x = 0;
    this->m_liquidVerts.y = 0;
    this->m_liquidTiles.x = 0;
    this->m_liquidTiles.y = 0;
}

// OFFSET: 0x0078F330
int32_t CMapObjGroup::QueryLighting(const C3Vector& pos, uint16_t polyIndex, CImVector& color, bool& exterior) {
    if (polyIndex >= this->m_polyCount) {
        color = this->m_parent->m_ambColor;
        exterior = false;
        return 1;
    }

    static const uint32_t s_axes[3][2] = { { 1, 2 }, { 2, 0 }, { 0, 1 } };

    const uint16_t* tri = &this->m_indices[3 * polyIndex];
    const C3Vector& v0 = this->m_vertices[tri[0]];
    const C3Vector& v1 = this->m_vertices[tri[1]];
    const C3Vector& v2 = this->m_vertices[tri[2]];

    C3Vector normal;
    normal.x = (v1.y - v0.y) * (v2.z - v0.z) - (v1.z - v0.z) * (v2.y - v0.y);
    normal.y = (v1.z - v0.z) * (v2.x - v0.x) - (v2.z - v0.z) * (v1.x - v0.x);
    normal.z = (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);

    const uint32_t* axes = s_axes[normal.MajorAxis()];
    const float* p0 = &v0.x;
    const float* p1 = &v1.x;
    const float* p2 = &v2.x;
    const float* pp = &pos.x;

    float a0 = p0[axes[0]];
    float a1 = p0[axes[1]];
    float e2b = p2[axes[1]] - a1;
    float e1b = p1[axes[1]] - a1;
    float pb = pp[axes[1]] - a1;
    float e2a = p2[axes[0]] - a0;
    float e1a = p1[axes[0]] - a0;
    float pa = pp[axes[0]] - a0;
    float inv = 1.0f / (e2b * e1a - e1b * e2a);

    int32_t w2 = static_cast<int32_t>((pb * e1a - e1b * pa) * inv * 256.0f);
    int32_t w1 = static_cast<int32_t>((e2b * pa - pb * e2a) * inv * 256.0f);
    int32_t w0 = 256 - w2 - w1;
    int32_t w2a = w2;

    if (w1 < 0) {
        int32_t adjust = w1 * w2 / (256 - w1);
        w2 += adjust;
        w0 += w1 - adjust;
        w2a += adjust;
        w1 = 0;
    }

    if (w2 < 0) {
        int32_t adjust = w1 * w2 / (w0 + w1);
        w1 += adjust;
        w0 += w2a - adjust;
        w2 = 0;
        w2a = 0;
    }

    if (w0 < 0) {
        int32_t adjust = w1 * w0 / (w2 + w1);
        w2a += w0 - adjust;
        w1 += adjust;
        w0 = 0;
    }

    const uint8_t* c0 = reinterpret_cast<const uint8_t*>(&this->m_colors[tri[0]]);
    const uint8_t* c1 = reinterpret_cast<const uint8_t*>(&this->m_colors[tri[1]]);
    const uint8_t* c2 = reinterpret_cast<const uint8_t*>(&this->m_colors[tri[2]]);
    uint8_t* out = reinterpret_cast<uint8_t*>(&color);

    out[3] = static_cast<uint16_t>(w2a * c2[3] + w0 * c0[3] + w1 * c1[3]) >> 8;
    out[2] = static_cast<uint16_t>(w2a * c2[2] + w0 * c0[2] + w1 * c1[2]) >> 8;
    out[1] = static_cast<uint16_t>(w2a * c2[1] + w0 * c0[1] + w1 * c1[1]) >> 8;
    out[0] = static_cast<uint8_t>((w0 * c0[0] + w1 * c1[0] + w2a * c2[0]) >> 8);

    uint32_t r = 2 * out[2];
    uint32_t g = 2 * out[1];
    uint32_t b = 2 * out[0];

    if (this->m_parent->m_header->m_flags & 0x2) {
        const uint8_t* amb = reinterpret_cast<const uint8_t*>(&this->m_parent->m_ambColor);
        r += amb[2];
        g += amb[1];
        b += amb[0];
    }

    out[2] = r >= 0xFF ? 0xFF : r;
    out[1] = g >= 0xFF ? 0xFF : g;
    out[0] = b >= 0xFF ? 0xFF : b;
    exterior = this->m_polys[polyIndex].m_flags & 0x1;
    return 1;
}

// OFFSET: 0x0078F6F0
int32_t CMapObjGroup::QueryLiquid(const C3Vector& pos, int32_t requireTiles, uint32_t& liquidType, float& height) {
    uint32_t type = this->m_liquidType;

    if (!type) {
        return 0;
    }

    if (!this->m_liquidTiles.x || !this->m_liquidTiles.y) {
        if (requireTiles) {
            return 0;
        }

        liquidType = type;
        height = this->m_bbox.t.z;
        return 1;
    }

    float fx = s_invLiquidTileSize * (pos.x - this->m_liquidCorner.x);
    float fy = s_invLiquidTileSize * (pos.y - this->m_liquidCorner.y);
    C2iVector tile;
    tile.x = static_cast<int32_t>(floor(fx));
    tile.y = static_cast<int32_t>(floor(fy));

    if (tile.x < 0 || tile.y < 0 || !(tile < this->m_liquidTiles) || (this->m_liquidTileFlags[tile.y * this->m_liquidTiles.x + tile.x] & 0xF) == 0xF) {
        return 0;
    }

    float tx = fx - static_cast<float>(tile.x);
    uint32_t index = tile.x + tile.y * this->m_liquidVerts.x;
    float ty = fy - static_cast<float>(tile.y);
    uint32_t next = index + this->m_liquidVerts.x;
    float h2 = this->m_liquidVertices[next].m_height;
    float h0 = (this->m_liquidVertices[index + 1].m_height - this->m_liquidVertices[index].m_height) * tx + this->m_liquidVertices[index].m_height;
    float h3 = this->m_liquidVertices[next + 1].m_height;
    liquidType = type;
    height = ((h3 - h2) * tx + h2 - h0) * ty + h0;
    return 1;
}

// OFFSET: 0x0078F8A0/0x140480880
void VBBList::Block::Set(uint32_t itemSize, uint32_t itemCount, uint32_t offset) {
    this->m_size = itemCount * itemSize;
    this->m_offset = offset;
    uint32_t aligned = offset;

    if (offset % itemSize) {
        aligned = offset + itemSize - offset % itemSize;
    }

    this->m_buf->m_index = aligned / this->m_buf->m_itemSize;
    g_theGxDevicePtr->BufSizeSet(this->m_buf, itemSize, itemCount - 1);
    this->m_buf->unk_001C = 0;
}

// OFFSET: 0x0078F910
void CMapObjGroup::FillVertexVB(CGxBuf* buf, EGxVertexBufferFormat format) const {
    uint32_t defaultColor = (this->m_parent->m_header->m_flags & 0x2) ? 0xFF000000 : 0xFF7F7F7F;
    const uint32_t* colors = reinterpret_cast<const uint32_t*>(this->m_colors);
    uint32_t colorStride = 1;

    if (!colors) {
        colors = &defaultColor;
        colorStride = 0;
    }

    const uint32_t* colors2 = reinterpret_cast<const uint32_t*>(this->m_colors2);
    uint32_t color2Stride = 1;

    if (!colors2) {
        colors2 = &defaultColor;
        color2Stride = 0;
    }

    C2Vector zero(0.0f, 0.0f);
    const C2Vector* texCoords2 = this->m_texCoords2;
    uint32_t texCoord2Stride = 1;

    if (!texCoords2) {
        texCoords2 = &zero;
        texCoord2Stride = 0;
    }

    if (format != GxVBF_PNCT && format != GxVBF_PNCT2 && format != GxVBF_PNC2T && format != GxVBF_PNC2T2) {
        return;
    }

    uint32_t* dst = reinterpret_cast<uint32_t*>(g_theGxDevicePtr->BufLock(buf));

    for (uint32_t i = 0; i < this->m_vertexCount; i++) {
        const uint32_t* position = reinterpret_cast<const uint32_t*>(&this->m_vertices[i]);
        const uint32_t* normal = reinterpret_cast<const uint32_t*>(&this->m_normals[i]);
        const uint32_t* texCoord = reinterpret_cast<const uint32_t*>(&this->m_texCoords[i]);
        dst[0] = position[0];
        dst[1] = position[1];
        dst[2] = position[2];
        dst[3] = normal[0];
        dst[4] = normal[1];
        dst[5] = normal[2];
        dst[6] = SwapColor(*colors);
        colors += colorStride;
        uint32_t next = 7;

        if (format == GxVBF_PNC2T || format == GxVBF_PNC2T2) {
            dst[next++] = SwapColor(*colors2);
            colors2 += color2Stride;
        }

        dst[next++] = texCoord[0];
        dst[next++] = texCoord[1];

        if (format == GxVBF_PNCT2 || format == GxVBF_PNC2T2) {
            const uint32_t* texCoord2 = reinterpret_cast<const uint32_t*>(texCoords2);
            dst[next++] = texCoord2[0];
            dst[next++] = texCoord2[1];
            texCoords2 += texCoord2Stride;
        }

        dst += next;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x0078FE70
void CMapObjGroup::FillTransparencyVB(CGxBuf* buf) const {
    uint32_t count = this->m_batches[this->m_transBatchCount - 1].m_maxIndex + 1;
    uint32_t* dst = reinterpret_cast<uint32_t*>(g_theGxDevicePtr->BufLock(buf));

    for (uint32_t i = 0; i < count; i++) {
        const uint32_t* position = reinterpret_cast<const uint32_t*>(&this->m_vertices[i]);
        const uint32_t* normal = reinterpret_cast<const uint32_t*>(&this->m_normals[i]);
        const uint32_t* texCoord = reinterpret_cast<const uint32_t*>(&this->m_texCoords[i]);
        dst[0] = position[0];
        dst[1] = position[1];
        dst[2] = position[2];
        dst[3] = normal[0];
        dst[4] = normal[1];
        dst[5] = normal[2];
        dst[6] = 0x007F7F7F | (static_cast<uint32_t>(this->m_colors[i].a) << 24);
        dst[7] = texCoord[0];
        dst[8] = texCoord[1];
        dst += 9;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x0078FF80
void CMapObjGroup::UploadIndexBuffer(CGxBuf* buf) const {
    void* dst = g_theGxDevicePtr->BufLock(buf);
    memcpy(dst, this->m_indices, sizeof(uint16_t) * this->m_indexCount);
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x0078FFE0/0x140480FC0
int32_t CMapObjGroup::GetNumSharedTiles() {
    if (this->m_numSharedTiles > 0) {
        return this->m_numSharedTiles;
    }

    int32_t count = 0;
    const uint8_t* flags = this->m_liquidTileFlags;

    for (int32_t y = 0; y < this->m_liquidTiles.y; y++) {
        for (int32_t x = 0; x < this->m_liquidTiles.x; x++) {
            if ((*flags & 0xF) != 0xF && static_cast<int8_t>(*flags) < 0) {
                count++;
            }

            flags++;
        }
    }

    this->m_numSharedTiles = count;
    return count;
}

// OFFSET: 0x00790050
void CMapObjGroup::GenLiquidVerts() {
    const SMOLiquidVert* src = this->m_liquidVertices;
    C3Vector* dst = this->m_liquidVertArray->m_verts.m_data;
    float x = this->m_liquidCorner.x;
    float y = this->m_liquidCorner.y;
    this->m_liquidMinHeight = src->m_height;
    this->m_liquidMaxHeight = src->m_height;

    for (int32_t row = 0; row < this->m_liquidVerts.y; row++) {
        for (int32_t col = 0; col < this->m_liquidVerts.x; col++) {
            float height = src->m_height;
            dst->x = x;
            dst->y = y;
            dst->z = height;
            dst++;

            if (this->m_liquidMinHeight > height) {
                this->m_liquidMinHeight = height;
            }

            if (this->m_liquidMaxHeight < height) {
                this->m_liquidMaxHeight = height;
            }

            src++;
            x += 4.1666665f;
        }

        x = this->m_liquidCorner.x;
        y += 4.1666665f;
    }
}

// OFFSET: 0x00790170
uint32_t CMapObjGroup::IdentifyLegacyLiquidType() {
    int32_t count = this->m_liquidTiles.x * this->m_liquidTiles.y;

    for (int32_t i = 0; i < count; i++) {
        uint8_t flags = this->m_liquidTileFlags[i];

        if ((flags & 0xF) != 0xF) {
            return (flags & 0xF) + 1;
        }
    }

    return 0;
}

// OFFSET: 0x007901C0
bool CMapObjGroup::VectorIntersectLiquidTiles(const C3Segment& seg, float* t, uint32_t flags, const uint32_t* tileCount, const uint8_t* tiles, World::TriData* triData, const CMapObjDef* mapObjDef) const {
    uint32_t liquidType = this->m_liquidType;

    if (!liquidType || !this->m_liquidTiles.x || !this->m_liquidTiles.y) {
        return false;
    }

    if ((flags & 0x10000) && !(flags & 0x20000) && !(g_liquidTypeDB.GetRecord(liquidType)->m_flags & 0x4)) {
        return false;
    }

    float dx = seg.t.x - seg.b.x;
    float dz = seg.t.z - seg.b.z;
    float dy = seg.t.y - seg.b.y;
    float invLength = 1.0f / sqrt(dx * dx + dz * dz + dy * dy);
    C3Ray ray(seg.b, C3Vector(dx * invLength, dy * invLength, dz * invLength));

    const C3Vector* verts = this->m_liquidVertArray->m_verts.m_data;
    uint32_t tris[6];
    tris[0] = 0;
    tris[1] = this->m_liquidVerts.x + 1;
    tris[2] = this->m_liquidVerts.x;
    tris[3] = 0;
    tris[4] = 1;
    tris[5] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = NULL;
    uint16_t* indices = NULL;
    bool found = false;

    for (uint32_t i = 0; i < *tileCount; i += 2, tiles += 2) {
        uint32_t tileX = tiles[0];
        uint32_t tileY = tiles[1];

        if ((this->m_liquidTileFlags[tileY * this->m_liquidTiles.x + tileX] & 0xF) == 0xF) {
            continue;
        }

        uint32_t base = tileX + tileY * this->m_liquidVerts.x;

        for (uint32_t j = 0; j < 2; j++) {
            uint32_t triIndices[3];
            triIndices[0] = base + tris[3 * j];
            triIndices[1] = base + tris[3 * j + 1];
            triIndices[2] = base + tris[3 * j + 2];

            float hitT = 1.0f;

            if (!NTempest::Intersect(ray, verts, triIndices, &hitT, NULL, 0.0099999998f)) {
                continue;
            }

            hitT *= invLength;

            if (hitT >= *t || hitT < 0.0f) {
                continue;
            }

            *t = hitT;

            if (!triData) {
                found = true;
                continue;
            }

            if (!found) {
                batch = World::TriData::AllocBatch();

                if (batch) {
                    batch->m_def = mapObjDef;
                    batch->m_matrix = &mapObjDef->m_mat;
                    batch->m_vertexList = this->m_liquidVertArray->m_verts.m_data;
                    indices = World::TriData::AllocIndices(3);
                    batch->m_indices = indices;
                }
            }

            found = true;

            if (!batch || !batch->m_indices) {
                continue;
            }

            batch->m_indexCount = 0;

            for (uint32_t k = 0; k < 3; k++) {
                uint16_t index = static_cast<uint16_t>(base + tris[3 * j + k]);
                indices[batch->m_indexCount++] = index;

                if (batch->m_minVertexIndex >= index) {
                    batch->m_minVertexIndex = index;
                }

                if (batch->m_maxVertexIndex <= index) {
                    batch->m_maxVertexIndex = index;
                }
            }

            batch->m_faceCount++;
        }
    }

    return found;
}

// OFFSET: 0x00790530
void CMapObjGroup::VectorIntersectLiquidSX(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) {
    int32_t x = static_cast<int32_t>(start.x);
    int32_t y = static_cast<int32_t>(start.y);
    int32_t endX = static_cast<int32_t>(end.x);
    int32_t step = 2 * (endX >= x) - 1;

    while (x != endX + step) {
        tiles[(*tileCount)++] = static_cast<uint8_t>(x);
        tiles[(*tileCount)++] = static_cast<uint8_t>(y);
        x += step;
    }
}

// OFFSET: 0x00790580
void CMapObjGroup::VectorIntersectLiquidSY(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) {
    int32_t y = static_cast<int32_t>(start.y);
    int32_t x = static_cast<int32_t>(start.x);
    int32_t endY = static_cast<int32_t>(end.y);
    int32_t step = 2 * (endY >= y) - 1;

    while (y != endY + step) {
        tiles[(*tileCount)++] = static_cast<uint8_t>(x);
        tiles[(*tileCount)++] = static_cast<uint8_t>(y);
        y += step;
    }
}

// OFFSET: 0x007905E0
void CMapObjGroup::VectorIntersectLiquidDX(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) const {
    int32_t x = static_cast<int32_t>(start.x);
    int32_t step;
    float frac;

    if (end.x < start.x) {
        step = -1;
        frac = static_cast<float>(x + 1) - start.x;
    } else {
        step = 1;
        frac = start.x - static_cast<float>(x);
    }

    int32_t endX = step + static_cast<int32_t>(end.x);
    float slope = (end.y - start.y) / fabs(end.x - start.x);
    float startY = start.y - slope * frac;
    float offset = 0.0f;
    int32_t prevY = static_cast<int32_t>(start.y);
    int32_t curY = prevY;

    if (x == endX) {
        return;
    }

    do {
        if (curY != prevY) {
            tiles[(*tileCount)++] = static_cast<uint8_t>(x);
            tiles[(*tileCount)++] = static_cast<uint8_t>(prevY);
        }

        tiles[(*tileCount)++] = static_cast<uint8_t>(x);
        tiles[(*tileCount)++] = static_cast<uint8_t>(curY);
        x += step;
        offset += slope;
        prevY = curY;
        curY = static_cast<int32_t>(offset + startY);
    } while (x != endX);

    if (curY != prevY && curY >= 0 && curY < this->m_liquidTiles.y) {
        tiles[(*tileCount)++] = static_cast<uint8_t>(x - step);
        tiles[(*tileCount)++] = static_cast<uint8_t>(curY);
    }
}

// OFFSET: 0x00790710
void CMapObjGroup::VectorIntersectLiquidDY(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) const {
    int32_t x = static_cast<int32_t>(start.x);
    int32_t y = static_cast<int32_t>(start.y);
    int32_t step;
    float frac;

    if (end.y < start.y) {
        step = -1;
        frac = static_cast<float>(y + 1) - start.y;
    } else {
        step = 1;
        frac = start.y - static_cast<float>(y);
    }

    int32_t endY = step + static_cast<int32_t>(end.y);
    float slope = (end.x - start.x) / fabs(end.y - start.y);
    float startX = start.x - slope * frac;
    float offset = 0.0f;
    int32_t prevX = x;
    int32_t curX = x;

    if (y == endY) {
        return;
    }

    do {
        if (curX != prevX) {
            tiles[(*tileCount)++] = static_cast<uint8_t>(prevX);
            tiles[(*tileCount)++] = static_cast<uint8_t>(y);
        }

        tiles[(*tileCount)++] = static_cast<uint8_t>(curX);
        tiles[(*tileCount)++] = static_cast<uint8_t>(y);
        y += step;
        offset += slope;
        prevX = curX;
        curX = static_cast<int32_t>(offset + startX);
    } while (y != endY);

    if (curX != prevX && curX >= 0 && curX < this->m_liquidTiles.x) {
        tiles[(*tileCount)++] = static_cast<uint8_t>(curX);
        tiles[(*tileCount)++] = static_cast<uint8_t>(y - step);
    }
}

// OFFSET: 0x007912B0
uint32_t CMapObjGroup::ComputePassCount(int32_t unified, uint32_t batch) const {
    if (unified) {
        if (batch < this->m_transBatchCount) {
            return 3 - (CShaderEffect::s_enableShaders != 0);
        }

        return 1;
    }

    if (!this->m_colors) {
        return 1;
    }

    return batch < this->m_transBatchCount ? 2 : 1;
}

// OFFSET: 0x007914C0
void CMapObjGroup::Init() {
    this->m_renderFlags &= ~0xF;
    this->m_distSq = 3.402823466e38f;
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_groupFlags = 0;
    this->unk_010C = 0;
    this->unk_0110 = 0;
    this->m_data = NULL;
    this->m_dataSize = 0;
    this->m_parent = NULL;
    this->m_frameStamp = 0;
    this->m_asyncObject = NULL;
    this->m_portalStart = 0;
    this->m_portalCount = 0;
    memset(this->m_fogs, 0, sizeof(this->m_fogs));
    this->m_liquidVerts.y = 0;
    this->m_liquidCorner.x = 0.0f;
    this->m_liquidCorner.y = 0.0f;
    this->m_liquidCorner.z = 0.0f;
    this->m_liquidVerts.x = 0;
    this->m_liquidMaterialId = 0;
    this->m_vertsBlock = NULL;
    this->m_transVertsBlock = NULL;
    this->m_indicesBlock = NULL;
    this->m_unusedFrames = 0;
    this->m_liquidVertsBlock = NULL;
    this->m_liquidIndicesBlock = NULL;
    this->m_liquidVBRefCount = 0;
    this->m_liquidType = 0;
    this->m_numSharedTiles = -1;
    this->InitPtrs();
}

// OFFSET: 0x007915D0
void CMapObjGroup::SetVertexVB() const {
    uint32_t stride = GxVertexSize(this->m_vertexFormat);
    CGxBuf* buf = this->m_vertsBlock ? this->m_vertsBlock->m_buf : g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, stride, this->m_vertexCount);

    if (!buf->unk_001C || !buf->unk_001D) {
        this->FillVertexVB(buf, this->m_vertexFormat);
    }

    g_theGxDevicePtr->PrimVertexPtr(buf, this->m_vertexFormat);
}

// OFFSET: 0x00791640
void CMapObjGroup::SetTransparencyVB() const {
    CGxBuf* buf;

    if (this->m_transVertsBlock) {
        buf = this->m_transVertsBlock->m_buf;
    } else {
        buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 36, this->m_batches[this->m_transBatchCount - 1].m_maxIndex + 1);
    }

    if (!buf->unk_001C || !buf->unk_001D) {
        this->FillTransparencyVB(buf);
    }

    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PNCT);
}

// OFFSET: 0x007916B0
void CMapObjGroup::SetIndexVB() const {
    CGxBuf* buf = this->m_indicesBlock ? this->m_indicesBlock->m_buf : g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, this->m_indexCount);

    if (!buf->unk_001C || !buf->unk_001D) {
        this->UploadIndexBuffer(buf);
    }

    g_theGxDevicePtr->PrimIndexPtr(buf);
}

// OFFSET: 0x00791700
bool CMapObjGroup::VectorIntersectLiquid(const C3Segment& seg, float* t, uint32_t flags, World::TriData* triData, const CMapObjDef* mapObjDef) const {
    float sizeY = this->m_liquidTiles.y * 4.1666665f - 0.1f;

    if (sizeY <= 0.0f) {
        sizeY = 0.0f;
    }

    float sizeX = 0.0f;

    if (this->m_liquidTiles.x * 4.1666665f - 0.1f > 0.0f) {
        sizeX = this->m_liquidTiles.x * 4.1666665f - 0.1f;
    }

    CAaBox box;
    box.b.x = this->m_liquidCorner.x;
    box.b.y = this->m_liquidCorner.y;
    box.b.z = this->m_liquidMinHeight;
    box.t.x = this->m_liquidCorner.x + sizeX;
    box.t.y = this->m_liquidCorner.y + sizeY;
    box.t.z = this->m_liquidMaxHeight;

    C3Vector dir(seg.t.x - seg.b.x, seg.t.y - seg.b.y, seg.t.z - seg.b.z);
    C3Vector start = seg.b;
    C3Vector end = seg.t;
    float lengthSq = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

    if (lengthSq > 2.3841858e-7f) {
        float invLength = 1.0f / sqrt(lengthSq);
        dir.x = dir.x * invLength;
        dir.y = dir.y * invLength;
        dir.z = invLength * dir.z;
    }

    if (start.x < box.b.x || start.y < box.b.y || start.z < box.b.z || box.t.x < start.x || box.t.y < start.y || box.t.z < start.z) {
        C3Ray ray(seg.b, dir);
        float rayT = 1.0f;

        if (!NTempest::Intersect(ray, box, &rayT, &start)) {
            return false;
        }
    }

    if (end.x < box.b.x || end.y < box.b.y || end.z < box.b.z || box.t.x < end.x || box.t.y < end.y || box.t.z < end.z) {
        C3Ray ray(end, C3Vector(-dir.x, -dir.y, -dir.z));
        float rayT = 1.0f;

        if (!NTempest::Intersect(ray, box, &rayT, &end)) {
            return false;
        }
    }

    float cornerX = this->m_liquidCorner.x;
    float cornerY = this->m_liquidCorner.y;
    end.y = s_invLiquidTileSize * (end.y - cornerY);
    start.z = start.z * s_invLiquidTileSize;
    end.x = s_invLiquidTileSize * (end.x - cornerX);
    start.x = s_invLiquidTileSize * (start.x - cornerX);
    start.y = s_invLiquidTileSize * (start.y - cornerY);
    end.z = end.z * s_invLiquidTileSize;

    float deltaY = fabs(end.y - start.y);
    float deltaX = end.x - start.x;
    uint32_t tileCount = 0;
    uint8_t tiles[0x800];

    if (deltaY < 2.3841858e-7f) {
        CMapObjGroup::VectorIntersectLiquidSX(start, end, &tileCount, tiles, sizeof(tiles));
    } else if (fabs(deltaX) < 2.3841858e-7f) {
        CMapObjGroup::VectorIntersectLiquidSY(start, end, &tileCount, tiles, sizeof(tiles));
    } else if (fabs(deltaX) <= deltaY) {
        this->VectorIntersectLiquidDY(start, end, &tileCount, tiles, sizeof(tiles));
    } else {
        this->VectorIntersectLiquidDX(start, end, &tileCount, tiles, sizeof(tiles));
    }

    return this->VectorIntersectLiquidTiles(seg, t, flags, &tileCount, tiles, triData, mapObjDef);
}

// OFFSET: 0x00792680
bool CMapObjGroup::GetTris(const C3Segment& seg, float* dist, uint32_t flags, uint16_t faceIgnoreFlags, World::TriData* triData, const CMapObjDef* mapObjDef) {
    uint32_t batchCount = World::TriData::s_nBatches;

    if (this->m_bsp.m_nNodes) {
        BspQuery_Segment query(this->m_polys, this->m_vertices, this->m_indices, seg, dist, faceIgnoreFlags, this->m_parent->m_materials);
        CAaBsp_Query_Segment<BspQuery_Segment> bspQuery;
        bspQuery.m_aaBsp = &this->m_bsp;
        bspQuery.m_f = &query;
        bspQuery.GetFaceIndices(0, seg, this->m_bsp.m_aaBox);
        this->GetTrisFromQuery(triData, &query, mapObjDef, 0);
    }

    if ((flags & 0x30000) && (this->m_groupFlags & 0x1000)) {
        this->VectorIntersectLiquid(seg, dist, flags, triData, mapObjDef);
    }

    return World::TriData::s_nBatches != batchCount;
}

// OFFSET: 0x007929C0
VBBList::Block* VBBList::AllocBlock() {
    Block* block;

    if (this->m_pooled) {
        block = this->m_freeBlocks.Head();

        if (!block) {
            void* mem = SMemAlloc(sizeof(Block), __FILE__, __LINE__, 0x8);
            block = mem ? new (mem) Block() : NULL;
            this->m_freeBlocks.LinkToTail(block);
            block->m_buf = g_theGxDevicePtr->BufCreate(this->m_pool, 0, 0, 0);
        }

        block->m_link.Unlink();
        block->m_owner = NULL;
        block->m_pool = NULL;
        block->m_lastUsedFrame = 0;
        return block;
    }

    void* mem = SMemAlloc(sizeof(Block), __FILE__, __LINE__, 0x8);
    block = mem ? new (mem) Block() : NULL;
    this->m_blocks.LinkToTail(block);
    block->m_offset = 0;
    block->m_size = 0;
    block->m_owner = NULL;
    block->m_buf = NULL;
    block->m_pool = NULL;
    block->m_lastUsedFrame = 0;
    return block;
}

// OFFSET: 0x00792B40
void CMapObjGroup::AllocVertArray(VertArray*& array, uint32_t count) {
    array = CMapObjGroup::s_freeVertArrays.Head();

    if (!array) {
        void* mem = SMemAlloc(sizeof(VertArray), __FILE__, __LINE__, 0x8);
        VertArray* created = mem ? new (mem) VertArray() : NULL;
        CMapObjGroup::s_freeVertArrays.LinkToTail(created);
        array = created;
    }

    array->m_link.Unlink();
    array->m_verts.SetCount(count);
}

// OFFSET: 0x00792CB0
void CMapObjGroup::PurgeVertArrayList() {
    VertArray* array;

    while ((array = CMapObjGroup::s_freeVertArrays.Head()) != NULL) {
        array->~VertArray();
        SMemFree(array, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x00792DD0
CMapObjGroup::CMapObjGroup() {
    this->m_liquidBatch.m_primType = GxPrim_TriangleFan;
    this->m_liquidBatch.m_minIndex = 0;
    this->m_liquidBatch.m_start = 0;
    this->m_liquidBatch.m_count = 0;
    this->m_liquidBatch.m_maxIndex = 0;
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_liquidVerts.x = 0;
    this->m_liquidVerts.y = 0;
    this->m_liquidTiles.x = 0;
    this->m_liquidTiles.y = 0;
    this->m_liquidCorner.x = 0.0f;
    this->m_liquidCorner.y = 0.0f;
    this->m_liquidCorner.z = 0.0f;
    this->m_liquidMinHeight = 0.0f;
    this->m_liquidMaxHeight = 0.0f;
    this->Init();
    this->m_type |= 0x2000;
}

// OFFSET: 0x00792F70
void VBBList::Destroy() {
    Block* block = this->m_blocks.Head();

    while (block) {
        Block* next = this->m_blocks.Next(block);

        if (block->m_buf && block->m_buf != g_theGxDevicePtr->BufStream(block->m_buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(block->m_buf);
        }

        block->m_buf = NULL;

        if (!this->m_pooled) {
            if (block->m_pool) {
                g_theGxDevicePtr->PoolDestroy(block->m_pool);
            }

            block->m_pool = NULL;
        }

        this->m_blocks.DeleteNode(block);
        block = next;
    }

    if (!this->m_pooled) {
        return;
    }

    block = this->m_freeBlocks.Head();

    while (block) {
        Block* next = this->m_freeBlocks.Next(block);

        if (block->m_buf && block->m_buf != g_theGxDevicePtr->BufStream(block->m_buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(block->m_buf);
        }

        block->m_buf = NULL;
        this->m_freeBlocks.DeleteNode(block);
        block = next;
    }

    if (this->m_pooled && this->m_pool) {
        g_theGxDevicePtr->PoolDestroy(this->m_pool);
    }
}

// OFFSET: 0x00793090/0x140484F30
void VBBList::FreeVBB(Block* block) {
    if (!block) {
        return;
    }

    if (this->m_pooled) {
        Block* prev = this->m_blocks.Prev(block);

        if (prev && prev->m_owner) {
            prev = NULL;
        }

        Block* next = this->m_blocks.Next(block);

        if (next && next->m_owner) {
            next = NULL;
        }

        uint32_t size = block->m_size;
        uint32_t offset = block->m_offset;

        if (prev) {
            size += prev->m_size;
            offset = prev->m_offset;
            prev->m_link.Unlink();
            this->m_freeBlocks.LinkToTail(prev);
        }

        if (next) {
            size += next->m_size;
            next->m_link.Unlink();
            this->m_freeBlocks.LinkToTail(next);
        }

        block->Set(size, 1, offset);
        *block->m_owner = NULL;
        block->m_owner = NULL;
        block->m_lastUsedFrame = 0;
        return;
    }

    if (block->m_buf) {
        GxBufDestroy(block->m_buf);
        block->m_buf = NULL;
    }

    if (block->m_pool) {
        g_theGxDevicePtr->PoolDestroy(block->m_pool);
        block->m_pool = NULL;
    }

    *block->m_owner = NULL;
    this->m_blocks.DeleteNode(block);
}

// OFFSET: 0x007931B0/0x140485090
void VBBList::AllocVBB(Block** owner, uint32_t itemSize, uint32_t itemCount) {
    if (!this->m_pooled) {
        Block* block = this->AllocBlock();
        uint32_t size = itemSize * (itemCount + 1);
        block->m_pool = g_theGxDevicePtr->PoolCreate(this->m_target, this->m_usage, size + 4096, static_cast<EGxPoolHintBits>(3), this->m_target ? "VBBList_idx" : "VBBList_vtx");
        block->m_buf = g_theGxDevicePtr->BufCreate(block->m_pool, itemSize, itemCount, 0);
        block->m_offset = 0;
        block->m_size = size;
        block->m_owner = owner;
        *owner = block;
        return;
    }

    uint32_t needed = itemSize * (itemCount + 1);

    while (true) {
        Block* evict = NULL;

        for (Block* block = this->m_blocks.Head(); block; block = this->m_blocks.Next(block)) {
            if (block->m_owner) {
                if (!evict || block->m_size < evict->m_size) {
                    evict = block;
                }

                continue;
            }

            if (needed < block->m_size) {
                Block* split = this->AllocBlock();
                this->m_blocks.LinkNode(split, 1, block);
                uint32_t splitOffset = needed + block->m_offset;
                split->m_offset = splitOffset;
                split->m_size = block->m_size - needed;
                split->m_buf->m_index = splitOffset / split->m_buf->m_itemSize;
                g_theGxDevicePtr->BufSizeSet(split->m_buf, 1, split->m_size - 1);
                split->m_buf->unk_001C = 0;

                uint32_t offset = block->m_offset;
                block->m_size = needed;

                if (offset % itemSize) {
                    offset = itemSize + offset - offset % itemSize;
                }

                block->m_buf->m_index = offset / block->m_buf->m_itemSize;
                g_theGxDevicePtr->BufSizeSet(block->m_buf, itemSize, itemCount);
                block->m_buf->unk_001C = 0;
                block->m_owner = owner;
                *owner = block;
                return;
            }
        }

        if (needed < evict->m_size) {
            return;
        }

        this->FreeVBB(evict);
    }
}

// OFFSET: 0x00793340
void CMapObjGroup::AllocVB() {
    if (!this->m_vertsBlock) {
        CMapObjGroup::s_vertexList.AllocVBB(&this->m_vertsBlock, GxVertexSize(this->m_vertexFormat), this->m_vertexCount);
    }

    if (!CShaderEffect::s_enableShaders && (this->m_parent->m_header->m_flags & 0x2) && this->m_colors && this->m_transBatchCount && !this->m_transVertsBlock) {
        uint32_t count = this->m_batches[this->m_transBatchCount - 1].m_maxIndex + 1;
        CMapObjGroup::s_vertexList.AllocVBB(&this->m_transVertsBlock, GxVertexSize(GxVBF_PNCT), count);
    }

    if (!this->m_indicesBlock) {
        CMapObjGroup::s_indexList.AllocVBB(&this->m_indicesBlock, 2, this->m_indexCount);
    }
}

// OFFSET: 0x007933F0
void CMapObjGroup::FreeVB() {
    if (this->m_vertsBlock) {
        CMapObjGroup::s_vertexList.FreeVBB(this->m_vertsBlock);
    }

    if (this->m_transVertsBlock) {
        CMapObjGroup::s_vertexList.FreeVBB(this->m_transVertsBlock);
    }

    if (this->m_indicesBlock) {
        CMapObjGroup::s_indexList.FreeVBB(this->m_indicesBlock);
    }

    this->m_vertsBlock = NULL;
    this->m_transVertsBlock = NULL;
    this->m_indicesBlock = NULL;
}

// OFFSET: 0x00793440/0x140485320
void CMapObjGroup::AllocLiquidVB(EGxVertexBufferFormat format, uint32_t vertexCount, uint32_t indexCount, CGxBatch*& batch) {
    if (!this->m_liquidVertsBlock) {
        CMapObjGroup::s_vertexList.AllocVBB(&this->m_liquidVertsBlock, GxVertexSize(format), vertexCount);
        this->m_liquidVertsBlock->m_buf->unk_001C = 0;
    }

    if (!this->m_liquidIndicesBlock) {
        CMapObjGroup::s_indexList.AllocVBB(&this->m_liquidIndicesBlock, 2, indexCount);
        this->m_liquidIndicesBlock->m_buf->unk_001C = 0;
    }

    batch = &this->m_liquidBatch;
    this->m_liquidVBRefCount++;
}

// OFFSET: 0x007934B0/0x1404853C0
void CMapObjGroup::FreeLiquidVB() {
    int32_t refCount = this->m_liquidVBRefCount;

    if (!refCount) {
        return;
    }

    this->m_liquidVBRefCount = refCount - 1;

    if (refCount != 1) {
        return;
    }

    if (this->m_liquidVertsBlock) {
        CMapObjGroup::s_vertexList.FreeVBB(this->m_liquidVertsBlock);
    }

    if (this->m_liquidIndicesBlock) {
        CMapObjGroup::s_indexList.FreeVBB(this->m_liquidIndicesBlock);
    }

    this->m_liquidVertsBlock = NULL;
    this->m_liquidIndicesBlock = NULL;
}

// OFFSET: 0x00793610/0x1404855C0
void CMapObjGroup::Initialize() {
    CMapObjGroup::s_vertexList.m_pooled = 0;
    CMapObjGroup::s_vertexList.m_target = GxPoolTarget_Vertex;
    CMapObjGroup::s_vertexList.m_usage = static_cast<EGxPoolUsage>(World::GetPoolUsage());
    CMapObjGroup::s_vertexList.m_pool = NULL;
    CMapObjGroup::s_indexList.m_pooled = 0;
    CMapObjGroup::s_indexList.m_pool = NULL;
    CMapObjGroup::s_indexList.m_target = GxPoolTarget_Index;
    CMapObjGroup::s_indexList.m_usage = static_cast<EGxPoolUsage>(World::GetPoolUsage());
}

// OFFSET: 0x00793660
void CMapObjGroup::Destroy() {
    CMapObjGroup::s_vertexList.Destroy();
    CMapObjGroup::s_indexList.Destroy();
    CMapObjGroup::PurgeVertArrayList();
}

// OFFSET: 0x00793680/0x140485640
void CMapObjGroup::Clear(int32_t force) {
    if (!force && !(this->m_flags & 0x40)) {
        return;
    }

    int32_t freeData = 1;

    if (this->m_asyncObject) {
        freeData = AsyncFileReadCancel(this->m_asyncObject, CMapObjGroup::AsyncFreeCallback);
        this->m_asyncObject = NULL;
    }

    this->m_bsp.Clear();
    this->FreeVB();

    if (this->m_liquidVertArray) {
        CMapObjGroup::s_freeVertArrays.LinkToTail(this->m_liquidVertArray);
        this->m_liquidVertArray = NULL;
    }

    if (this->m_batches) {
        for (uint32_t i = 0; i < this->m_batchCount; i++) {
            this->m_parent->DestroyMaterial(this->m_batches[i].m_materialId);
        }
    }

    if (this->m_renderFlags & 0x8) {
        if (this->m_tangentStarts) {
            SMemFree(this->m_tangentStarts, "delete[]", -1, 0);
        }

        if (this->m_tangents) {
            SMemFree(this->m_tangents, "delete[]", -1, 0);
        }
    }

    this->InitPtrs();

    if (this->m_data) {
        if (freeData) {
            SMemFree(this->m_data, __FILE__, __LINE__, 0);
        }

        this->m_data = NULL;
    }

    CMapObjOccluder* occluder = this->m_occluders.Head();

    while (occluder) {
        CMapObjOccluder* next = this->m_occluders.Next(occluder);
        CMapObj::FreeOccluder(occluder);
        occluder = next;
    }

    this->m_flags &= ~0x40;
    this->m_renderFlags &= ~0x4;
}

// OFFSET: 0x00793C40
CMapObjGroup::~CMapObjGroup() {
    this->Clear(1);
}
