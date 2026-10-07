#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include <cmath>
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/LiquidEnvironment.h"
#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"

// STATIC: 0x00EB3270
C3Vector CChunkLiquid::s_vertXY[81];

// STATIC: 0x00EB3268
int32_t CChunkLiquid::s_vertXYInitialized;

// STATIC: 0x00BC9580
static const uint32_t s_cellTriangles[6] = { 0, 18, 17, 0, 1, 18 };

// STATIC: 0x00EB3A0C
static TSGrowableArray<CChunkLiquid*> s_batchLiquids;

// PROVEN: c063c8fe IDENTICAL/NONE
// OFFSET: 0x0075EDD0/0x14048C530
void CChunkLiquid::Purge() {
    if (this->m_instance) {
        this->m_instance->Release();
        this->m_instance = NULL;
    }
}

// PROVEN: 6e4ed08f IDENTICAL/NONE
// OFFSET: 0x0076CDE0/0x140909D40
float CChunkLiquid::GetVertexHeight(uint32_t index) const {
    return Liquid::GetChunkHeight(this->m_liquidObject, this->m_vertexData, this->m_vertexCount, index);
}

// PROVEN: 6d9f518c IDENTICAL/NONE
// OFFSET: 0x00798930/0x14048C560
void CChunkLiquid::UpdateResourcePurging() {
    if (this->m_instance) {
        this->m_instance->Purge();
    }
}

// PROVEN: 834227f6 IDENTICAL/NONE
// OFFSET: 0x00798940/0x14048C580
bool CChunkLiquid::HasData() const {
    if (this->m_tileData) {
        return true;
    }

    return Liquid::HasData(static_cast<const SMLiquidChunkDesc*>(this->m_owner->m_liquidChunk), static_cast<const SMLiquidInstance*>(this->m_liquidHeader));
}

// PROVEN: b8e632aa IDENTICAL/NONE
// OFFSET: 0x00798960/0x14048C5B0
void CChunkLiquid::CreateVertXY() {
    if (CChunkLiquid::s_vertXYInitialized) {
        return;
    }

    C3Vector* vertex = CChunkLiquid::s_vertXY;
    float x = 0.0f;

    for (uint32_t i = 0; i < 9; i++, x -= 4.1666665f) {
        float y = 0.0f;

        for (uint32_t j = 0; j < 9; j++, y -= 4.1666665f) {
            vertex->y = y;
            vertex->x = x;
            vertex->z = 0.0f;
            vertex++;
        }
    }

    CChunkLiquid::s_vertXYInitialized = 1;
}

// PROVEN: 044bef56 IDENTICAL/NONE
// OFFSET: 0x007989C0/0x14048C630
void CChunkLiquid::CreateVertZ() {
    memset(this->m_heights, 0, sizeof(this->m_heights));
    uint32_t index = 0;

    for (int32_t i = this->m_xOffset; i <= this->m_width; i++) {
        for (int32_t j = this->m_yOffset; j <= this->m_height; j++) {
            this->m_heights[9 * i + j] = Liquid::GetChunkHeight(this->m_liquidObject, this->m_vertexData, this->m_vertexCount, index++) - this->m_owner->m_position.z;
        }
    }
}

// OFFSET: 0x00798A60/0x14048C6F0
bool CChunkLiquid::GetHeight(const C2Vector& frac, const C2iVector& cell, float& height) const {
    if (cell.x < this->m_yOffset || cell.y < this->m_xOffset || cell.x > this->m_height || cell.y > this->m_width) {
        return false;
    }

    int32_t stride = this->m_height - this->m_yOffset + 1;
    int32_t index = cell.x - this->m_yOffset + stride * (cell.y - this->m_xOffset);
    int32_t next = index + stride;
    float h0 = this->GetVertexHeight(index);
    float h1 = this->GetVertexHeight(index + 1);
    float h2 = this->GetVertexHeight(next);
    float h3 = this->GetVertexHeight(next + 1);
    float top = (h1 - h0) * frac.x + h0;
    height = ((h3 - h2) * frac.x + h2 - top) * frac.y + top;
    return true;
}

// OFFSET: 0x00798B40
bool CChunkLiquid::Intersect(int32_t x, int32_t y, C3Ray& ray, float& t) const {
    int32_t index = x + (this->m_height - this->m_yOffset + 1) * (y - this->m_xOffset) - this->m_yOffset;
    int32_t stride = this->m_height - this->m_yOffset + 1;
    uint32_t vertex = y + x + 16 * y;
    float baseZ = this->m_owner->m_position.z;
    CMapChunk::vertexListXY[vertex].z = Liquid::GetChunkHeight(this->m_liquidObject, this->m_vertexData, this->m_vertexCount, index) - baseZ;
    CMapChunk::vertexListXY[vertex + 1].z = Liquid::GetChunkHeight(this->m_liquidObject, this->m_vertexData, this->m_vertexCount, index + 1) - baseZ;
    CMapChunk::vertexListXY[vertex + 17].z = Liquid::GetChunkHeight(this->m_liquidObject, this->m_vertexData, this->m_vertexCount, index + stride) - baseZ;
    CMapChunk::vertexListXY[vertex + 18].z = Liquid::GetChunkHeight(this->m_liquidObject, this->m_vertexData, this->m_vertexCount, index + stride + 1) - baseZ;

    bool hit = false;

    for (uint32_t i = 0; i < 6; i += 3) {
        uint32_t indices[3];
        indices[0] = vertex + s_cellTriangles[i];
        indices[1] = vertex + s_cellTriangles[i + 1];
        indices[2] = vertex + s_cellTriangles[i + 2];
        float hitT = 0.0f;

        if (NTempest::Intersect(ray, CMapChunk::vertexListXY, indices, &hitT, NULL, 0.0099999998f)) {
            hit = true;

            if (t > hitT && hitT >= 0.0f) {
                t = hitT;
            }
        }
    }

    return hit;
}

// OFFSET: 0x00798CB0/0x14048C9F0
bool CChunkLiquid::TileExists(uint32_t x, uint32_t y) const {
    if (this->m_tileData) {
        return (static_cast<const uint8_t*>(this->m_tileData)[8 * y + x] & 0xF) != 15;
    }

    if (x < static_cast<uint32_t>(this->m_yOffset) || y < static_cast<uint32_t>(this->m_xOffset) || x >= static_cast<uint32_t>(this->m_height) || y >= static_cast<uint32_t>(this->m_width)) {
        return false;
    }

    int32_t width = this->m_height - this->m_yOffset;
    const uint8_t* exists = Liquid::GetExistsTable(this->m_owner->m_area->m_liquid, static_cast<const SMLiquidInstance*>(this->m_liquidHeader));
    return CMapChunk::TestBit(exists, x + width * (y - this->m_xOffset) - this->m_yOffset) != 0;
}

// OFFSET: 0x00799460
CChunkLiquid::CChunkLiquid() {
    this->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->unk_0018[0] = 0;
    this->unk_0018[1] = 0;
    this->unk_0018[2] = 0;
    this->m_minHeight = 0.0f;
    this->m_maxHeight = 0.0f;
    this->m_xOffset = 0;
    this->m_yOffset = 0;
    this->m_width = 0;
    this->m_height = 0;
    this->m_liquidType = 0;
    this->m_vertexData = NULL;
    this->m_tileData = NULL;
    this->m_liquidHeader = NULL;
    this->m_instance = NULL;
    this->m_owner = NULL;
    this->m_frameStamp = 0;
    this->unk_0024 = -1.0f;
}

// OFFSET: 0x007994F0
CChunkLiquid::~CChunkLiquid() {
}

// OFFSET: 0x00799570
void CChunkLiquid::Batch() {
    s_batchLiquids.Clear();
    s_batchLiquids.Reserve(32, 0);

    if (CMap::enableChunkBatching) {
        const CMapChunk* owner = this->m_owner;
        CMapChunk* const* row = &owner->m_area->m_chunks[(owner->m_indexX & ~1) + 16 * (owner->m_indexY & ~1)];

        for (uint32_t y = 0; y < 2; y++, row += 16) {
            for (uint32_t x = 0; x < 2; x++) {
                CMapChunk* chunk = row[x];

                for (CChunkLiquid* liquid = chunk->m_liquidList.Head(); liquid; liquid = chunk->m_liquidList.Next(liquid)) {
                    if (liquid->m_liquidType == this->m_liquidType) {
                        s_batchLiquids.Add(1, &liquid);
                    }
                }
            }
        }
    } else {
        CChunkLiquid* liquid = this;
        s_batchLiquids.Add(1, &liquid);
    }

    CChunkGeomFactory* factory = CChunkGeomFactory::Create(s_batchLiquids);
    CAaBox bounds;

    for (uint32_t i = 0; i < s_batchLiquids.Count(); i++) {
        const CChunkLiquid* liquid = s_batchLiquids.m_data[i];
        float minX = liquid->m_position.x + static_cast<float>(liquid->m_yOffset) * 4.1666665f;
        float minY = liquid->m_position.y + static_cast<float>(liquid->m_xOffset) * 4.1666665f;
        float maxX = liquid->m_position.x + static_cast<float>(liquid->m_height) * 4.1666665f;
        float maxY = liquid->m_position.y + static_cast<float>(liquid->m_width) * 4.1666665f;

        if (i) {
            bounds.b.x = minX <= bounds.b.x ? minX : bounds.b.x;
            bounds.b.y = minY <= bounds.b.y ? minY : bounds.b.y;
            bounds.b.z = liquid->m_minHeight <= bounds.b.z ? liquid->m_minHeight : bounds.b.z;
            bounds.t.x = bounds.t.x > maxX ? bounds.t.x : maxX;
            bounds.t.y = bounds.t.y > maxY ? bounds.t.y : maxY;
            bounds.t.z = bounds.t.z > liquid->m_maxHeight ? bounds.t.z : liquid->m_maxHeight;
        } else {
            bounds.b = C3Vector(minX, minY, liquid->m_minHeight);
            bounds.t = C3Vector(maxX, maxY, liquid->m_maxHeight);
        }
    }

    C44Matrix transform;
    transform.Translate(s_batchLiquids.m_data[0]->m_position);

    const SMLiquidInstance* header = static_cast<const SMLiquidInstance*>(this->m_liquidHeader);
    Liquid::CInstance* instance = Liquid::CreateInstance(header ? header->m_vertexFormat : 0, this->m_liquidType, factory);
    instance->m_environment = CClientLiquidEnvironment::Create(1, 0);
    instance->m_transform = transform;
    instance->m_bounds.c = C3Vector((bounds.t.x + bounds.b.x) * 0.5f, (bounds.t.y + bounds.b.y) * 0.5f, (bounds.t.z + bounds.b.z) * 0.5f);
    C3Vector extent(bounds.t.x - bounds.b.x, bounds.t.y - bounds.b.y, bounds.t.z - bounds.b.z);
    instance->m_bounds.r = sqrtf(extent.x * extent.x + extent.z * extent.z + extent.y * extent.y) * 0.5f;
    instance->m_isMesh = factory->GetWorldSpace() == 0;
    instance->m_object->SetReflectionPlane(bounds.b.z);

    for (uint32_t i = 0; i < s_batchLiquids.Count(); i++) {
        CChunkLiquid* liquid = s_batchLiquids.m_data[i];
        liquid->m_instance = instance;
        instance->AddRef();
        CMapChunk* owner = liquid->m_owner;
        owner->m_chunkFlags ^= (owner->m_chunkFlags ^ (4 * (instance->m_object->m_reflectionType == 1))) & 0x4;
    }

    instance->Release();
}

// PROVEN: 0b03f0c6 IDENTICAL/NONE
// OFFSET: 0x00799BC0
void CChunkLiquid::RenderPrep() {
    if (!this->m_instance) {
        this->Batch();
    }
}
