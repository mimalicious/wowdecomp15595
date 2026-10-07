#include "WoW/Source/WorldClient/MapArea.h"
#include <cmath>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/LiquidEnvironment.h"
#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/World.h"

// OFFSET: 0x00401000
void CMapAreaLow::Initialize() {
}

// OFFSET: 0x00401000
void CMapAreaLow::Destroy() {
}

// OFFSET: 0x0077F250
static uint16_t* BuildLowDetailIndices(const uint16_t* holes, int32_t holeMode, uint16_t* indices) {
    uint16_t topLeft = 0;
    uint16_t center = 289;

    for (uint32_t i = 0; i < 16; i++) {
        uint16_t bottomLeft = topLeft + 17;
        uint16_t bottomRight = topLeft + 18;
        uint16_t topRight = topLeft + 1;
        uint32_t bit = 1;

        for (uint32_t j = 0; j < 16; j++) {
            int32_t skip = 0;

            if (holes) {
                if (holeMode) {
                    skip = !(holes[i] & bit);
                } else {
                    skip = (holes[i] & bit) != 0;
                }
            }

            if (!skip) {
                indices[0] = center;
                indices[1] = topRight;
                indices[2] = topLeft;
                indices[3] = center;
                indices[4] = bottomRight;
                indices[5] = topRight;
                indices[6] = center;
                indices[7] = bottomLeft;
                indices[8] = bottomRight;
                indices[9] = center;
                indices[10] = topLeft;
                indices[11] = bottomLeft;
                indices += 12;
            }

            bit = (bit << 1) | (bit >> 31);
            bottomRight++;
            bottomLeft++;
            center++;
            topLeft++;
            topRight++;
        }

        topLeft++;
    }

    return indices;
}

// OFFSET: 0x0077F530
void CMapAreaLow::CreateVertices(CGxBuf* buf) {
    float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(buf));
    uint32_t index = 0;
    float x = this->m_posX;

    for (uint32_t i = 0; i < 17; i++, x -= 33.333332f) {
        float y = this->m_posY;

        for (uint32_t j = 0; j < 17; j++, y -= 33.333332f, index++) {
            vertices[1] = y;
            vertices[0] = x;
            vertices[2] = static_cast<float>(this->m_heights[index]);
            vertices += 3;
        }
    }

    x = this->m_posX - 16.666666f;

    for (uint32_t i = 0; i < 16; i++, x -= 33.333332f) {
        float y = this->m_posY - 16.666666f;

        for (uint32_t j = 0; j < 16; j++, y -= 33.333332f, index++) {
            vertices[1] = y;
            vertices[0] = x;
            vertices[2] = static_cast<float>(this->m_heights[index]);
            vertices += 3;
        }
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x0077F840
void CMapAreaLow::Render() {
    if (static_cast<int32_t>(this->m_indexCount) <= 0) {
        return;
    }

    if (!this->m_buf) {
        this->m_buf = CMapChunkBuf::Alloc(545, 3072, GxVBF_P);
    }

    CGxBuf* vertexBuf = this->m_buf->m_vertexBuf;

    if (!vertexBuf->unk_001C || !vertexBuf->unk_001D) {
        this->CreateVertices(vertexBuf);
    }

    g_theGxDevicePtr->PrimVertexPtr(this->m_buf->m_vertexBuf, GxVBF_P);
    CGxBuf* indexBuf = this->m_buf->m_indexBuf;
    CGxBatch batch;

    if (this->m_indexCount == 3072) {
        if (!indexBuf->unk_001C || !indexBuf->unk_001D) {
            uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
            BuildLowDetailIndices(NULL, 0, indices);
            CGxBuf* buf = this->m_buf->m_indexBuf;
            g_theGxDevicePtr->BufUnlock(buf, 0);
            buf->unk_001C = 1;
        }

        g_theGxDevicePtr->PrimIndexPtr(this->m_buf->m_indexBuf);
        GxRsSet(GxRs_Culling, 0);
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = 3072;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 544;
        g_theGxDevicePtr->Draw(batch);
    } else {
        if (!indexBuf->unk_001C || !indexBuf->unk_001D) {
            uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
            BuildLowDetailIndices(this->m_holes, 0, indices);
            BuildLowDetailIndices(this->m_holes, 1, &indices[this->m_indexCount]);
            CGxBuf* buf = this->m_buf->m_indexBuf;
            g_theGxDevicePtr->BufUnlock(buf, 0);
            buf->unk_001C = 1;
        }

        g_theGxDevicePtr->PrimIndexPtr(this->m_buf->m_indexBuf);
        GxRsSet(GxRs_Culling, 0);
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = this->m_indexCount;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 544;
        g_theGxDevicePtr->Draw(batch);
        GxRsSet(GxRs_Culling, 1);
        batch.m_start = this->m_indexCount;
        batch.m_count = 3072 - this->m_indexCount;
        batch.m_primType = GxPrim_Triangles;
        g_theGxDevicePtr->Draw(batch);
    }

    this->m_lastUsedFrame = World::GetFrameCnt();
    CMap::s_mapAreaLowUpdateList.LinkToTail(this);
}

// OFFSET: 0x00796880
void CMapAreaLow::LiquidReset() {
    if (this->m_oceanInstance) {
        this->m_oceanInstance->Release();
        this->m_oceanInstance = NULL;
    }
}

// OFFSET: 0x00796B60
void CMapAreaLow::Create(int32_t x, int32_t y, const int16_t* heights, const uint16_t* holes) {
    this->m_heights = heights;
    this->m_holes = holes;

    if (holes) {
        this->m_indexCount = 0;
        const uint16_t* hole = holes;

        for (uint32_t row = 0; row < 16; row++) {
            uint32_t bit = 1;

            for (uint32_t col = 0; col < 16; col++) {
                if (!(*hole & static_cast<uint16_t>(bit))) {
                    this->m_indexCount += 12;
                }

                bit = (bit << 1) | (bit >> 31);
            }

            hole++;
        }
    } else {
        this->m_indexCount = 3072;
    }

    int32_t maxHeight = 0;
    int32_t minHeight = 0;

    if (heights) {
        minHeight = 100000;
        maxHeight = -100000;

        for (uint32_t i = 0; i < 545; i++) {
            int32_t height = this->m_heights[i];

            if (height > maxHeight) {
                maxHeight = height;
            }

            if (height < minHeight) {
                minHeight = height;
            }
        }
    }

    this->m_indexY = y;
    this->m_indexX = x;
    this->m_posY = static_cast<float>(16 * y) * 33.333332f;
    this->m_posX = 17066.666f - this->m_posY;
    this->m_bbox.t.x = this->m_posX;
    this->m_posY = static_cast<float>(16 * x) * -33.333332f + 17066.666f;
    this->m_bbox.t.y = this->m_posY;
    this->m_bbox.b.x = this->m_posX - 533.33331f;
    this->m_bbox.b.y = this->m_posY - 533.33331f;
    this->m_bbox.b.z = static_cast<float>(minHeight) - 1.0f;
    this->m_bbox.t.z = static_cast<float>(maxHeight) + 1.0f;
    this->m_center.x = (this->m_bbox.t.x + this->m_bbox.b.x) * 0.5f;
    this->m_center.y = (this->m_bbox.b.y + this->m_bbox.t.y) * 0.5f;
    this->m_center.z = (this->m_bbox.b.z + this->m_bbox.t.z) * 0.5f;

    float dz = this->m_bbox.t.z - this->m_center.z;
    float dy = this->m_bbox.t.y - this->m_center.y;
    float dx = this->m_bbox.t.x - this->m_center.x;
    this->m_radius = sqrt(dx * dx + (dy * dy + dz * dz));
}

// OFFSET: 0x00796D70
void CMapAreaLow::CreateOceanInstance() {
    CAreaGeomFactory* factory = CAreaGeomFactory::Create(this);
    C44Matrix transform;
    transform.Translate(C3Vector(this->m_posX, this->m_posY, this->m_oceanHeight));
    this->m_oceanInstance = Liquid::CreateInstance(42, 2, factory);
    this->m_oceanInstance->m_environment = CClientLiquidEnvironment::Create(1, 0);
    this->m_oceanInstance->m_transform = transform;
    this->m_oceanInstance->m_bounds.c = this->m_center;
    this->m_oceanInstance->m_bounds.r = this->m_radius;
    this->m_oceanInstance->m_isMesh = factory->GetWorldSpace() == 0;
}

// OFFSET: 0x00796E60
void CMapAreaLow::RenderPrep() {
    if (!this->m_oceanInstance) {
        this->CreateOceanInstance();
    }
}

// OFFSET: 0x00797360
void CMapAreaLow::FreeBuf() {
    if (this->m_buf) {
        CMapChunkBuf::Free(this->m_buf);
        this->m_buf = NULL;
    }

    if (this->m_oceanInstance) {
        this->m_oceanInstance->Purge();
    }

    this->m_renderLink.Unlink();
}

// OFFSET: 0x007975F0
CMapAreaLow::~CMapAreaLow() {
    for (CMapBaseObjLink* link = this->m_linkList.Head(), * next; link; link = next) {
        next = this->m_linkList.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        mapObjDef->Purge(0);
    }

    if (this->m_oceanInstance) {
        this->m_oceanInstance->Release();
        this->m_oceanInstance = NULL;
    }
}

// OFFSET: 0x007978D0
CMapAreaLow::CMapAreaLow() {
    this->m_radius = 0.0f;
    this->m_posX = 0.0f;
    this->m_posY = 0.0f;
    this->m_oceanHeight = 0.0f;
    this->m_indexX = 0;
    this->m_indexY = 0;
    this->m_type |= 0x800;
    this->unk_0010 = 0;
    this->unk_0014 = 0;
    this->m_buf = NULL;
    this->m_lastUsedFrame = 0;
    this->m_indexCount = 0;
    this->m_oceanInstance = NULL;
}
