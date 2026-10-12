#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/Object/WGUID.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GroundEffectDoodadRec.h"
#include "WoW/Source/DB/Rec/GroundEffectTextureRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/DB/Rec/TerrainMaterialRec.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D025D0/0x140BB2230
uint32_t CMapChunk::s_vertexCount = 145;

// STATIC: 0x00D025F0/0x140BB2270
uint16_t CMapChunk::s_indexCount = 768;

// STATIC: 0x00BC7650
static uint32_t s_holeMasks[16] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

// STATIC: 0x00BC7690
static uint32_t s_fanHeights[4][2] = { { 17, 0 }, { 0, 1 }, { 18, 17 }, { 1, 18 } };

// STATIC: 0x00BC76B0
static uint32_t s_fanCorners[4][2] = { { 3, 0 }, { 0, 1 }, { 2, 3 }, { 1, 2 } };

// STATIC: 0x00BC76E8
static uint16_t s_lowQualityTexMasks[8] = { 0x3, 0xC, 0x30, 0xC0, 0x300, 0xC00, 0x3000, 0xC000 };

// STATIC: 0x00BC76F8
static uint32_t s_lowQualityTexShifts[8] = { 0, 2, 4, 6, 8, 10, 12, 14 };

// STATIC: 0x00BC7718
static uint16_t s_noEffectDoodadMasks[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };

// STATIC: 0x00BC7728
static uint32_t s_noEffectDoodadShifts[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

// STATIC: 0x00BC92E4/0x1409B760C
static uint32_t s_subchunkTris[4][3] = { { 17, 9, 0 }, { 9, 1, 0 }, { 9, 17, 18 }, { 9, 18, 1 } };

// STATIC: 0x00BC9314/0x1409B7640
static uint32_t s_subchunkVerts[5] = { 0, 9, 17, 1, 18 };

// STATIC: 0x00D03DF8
static uint32_t s_flightBoundsTris[8][3] = { { 3, 0, 4 }, { 0, 1, 4 }, { 1, 2, 4 }, { 2, 5, 4 }, { 5, 8, 4 }, { 8, 7, 4 }, { 7, 6, 4 }, { 6, 3, 4 } };

// STATIC: 0x00D03E58
static C2Vector s_flightBoundsVerts[9] = { C2Vector(0.0f, 0.0f), C2Vector(0.0f, -266.66666f), C2Vector(0.0f, -533.33331f), C2Vector(-266.66666f, 0.0f), C2Vector(-266.66666f, -266.66666f), C2Vector(-266.66666f, -533.33331f), C2Vector(-533.33331f, 0.0f), C2Vector(-533.33331f, -266.66666f), C2Vector(-533.33331f, -533.33331f) };

// STATIC: 0x00D00DE0
static C3Vector s_subchunkCoords[5] = { C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, -4.1666665f, 0.0f), C3Vector(-4.1666665f, -4.1666665f, 0.0f), C3Vector(-4.1666665f, 0.0f, 0.0f), C3Vector(-2.0833333f, -2.0833333f, 0.0f) };

// STATIC: 0x00E74E28/0x140D5EB90
float CMapChunk::s_geoToTex;

// STATIC: 0x00E74E38/0x140D5EBB0
C3Vector CMapChunk::vertexListXY[145];

// STATIC: 0x00E75504/0x140D5F270
static float s_worldPNY[9];

// STATIC: 0x00E75528
static float s_worldPNX[9];

// STATIC: 0x00E7554C/0x140D5F2C0
static float s_worldPNCY[9];

// STATIC: 0x00E75570
static float s_worldPNCX[9];

// STATIC: 0x00E75594/0x140D5F310
static float s_worldPNC2Y[9];

// STATIC: 0x00E755B8
static float s_worldPNC2X[9];

static const float s_cellOffsets[7] = { 4.1666665f, 8.333333f, 12.5f, 16.666666f, 20.833332f, 25.0f, 29.166666f };

static void CalcWorldRows(const CMapChunk* chunk, float* xs, float* ys) {
    float x = 17066.666f - static_cast<float>(chunk->m_chunkY) * 33.333332f;
    float y = 17066.666f - static_cast<float>(chunk->m_chunkX) * 33.333332f;
    float xEnd = static_cast<float>(chunk->m_chunkX + 1) * 33.333332f;
    float yEnd = static_cast<float>(chunk->m_chunkY + 1) * 33.333332f;

    xs[0] = x;
    ys[0] = y;

    for (uint32_t i = 0; i < 7; i++) {
        xs[i + 1] = x - s_cellOffsets[i];
        ys[i + 1] = y - s_cellOffsets[i];
    }

    xs[8] = 17066.666f - yEnd;
    ys[8] = 17066.666f - xEnd;
}

static const float s_outerY[9] = { -0.0f, -4.1666665f, -8.333333f, -12.5f, -16.666666f, -20.833332f, -25.0f, -29.166666f, -33.333332f };
static const float s_innerY[8] = { -2.0833333f, -6.25f, -10.416666f, -14.583333f, -18.75f, -22.916666f, -27.083332f, -31.249998f };

static void CalcLocalSteps(const CMapChunk* chunk, float& stepX, float& stepY) {
    float extY = (17066.666f - static_cast<float>(chunk->m_chunkY) * 33.333332f) - (17066.666f - static_cast<float>(chunk->m_chunkY + 1) * 33.333332f);
    stepY = ((17066.666f - static_cast<float>(chunk->m_chunkX) * 33.333332f) - (17066.666f - static_cast<float>(chunk->m_chunkX + 1) * 33.333332f)) * -0.125f;
    stepX = extY * -0.125f;
}

static uint32_t VertexColor(const uint8_t* color) {
    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        return static_cast<uint32_t>(color[2]) | (static_cast<uint32_t>(color[1]) << 8) | (static_cast<uint32_t>(color[0]) << 16) | (static_cast<uint32_t>(color[3]) << 24);
    }

    return *reinterpret_cast<const uint32_t*>(color);
}

// OFFSET: 0x0073B0B0
void CMapChunk::ProcessIffChunks(uint32_t* data, int32_t size, int32_t isNew) {
    for (int32_t remaining = size; remaining > 0;) {
        uint32_t token = data[0];
        uint8_t* chunkData = reinterpret_cast<uint8_t*>(&data[2]);
        remaining -= 8;

        switch (token) {
            case 'MCSH':
                this->m_shadowMap = chunkData;
                break;

            case 'MCVT':
                this->m_vertices = reinterpret_cast<const float*>(chunkData);
                break;

            case 'MCRW':
                this->m_header.m_nMapObjRefs = data[1] >> 2;
                this->m_mapObjRefs = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MCNR':
                this->m_normals = reinterpret_cast<const int8_t*>(chunkData);
                break;

            case 'MCRD':
                this->m_header.m_nDoodadRefs = data[1] >> 2;
                this->m_doodadRefs = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MCRF':
                this->m_refs = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MCMT':
                this->m_materialId = *reinterpret_cast<uint32_t*>(chunkData);
                break;

            case 'MCLV':
                this->m_vertexLighting = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MCLY':
                this->m_header.m_nLayers = data[1] >> 4;
                this->m_layers = reinterpret_cast<const SMLayer*>(chunkData);
                break;

            case 'MCLQ':
                this->m_mcLiquid = chunkData;
                break;

            case 'MCAL':
                this->m_alphaMaps = chunkData;
                break;

            case 'MCCV':
                this->m_vertexColors = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MCDD':
                if (data[1] == 32) {
                    this->m_chunkFlags |= 0x2;
                }

                this->m_noEffectDoodad = chunkData;
                break;
        }

        remaining -= data[1];
        data = reinterpret_cast<uint32_t*>(&chunkData[data[1]]);
    }
}

// OFFSET: 0x0073B410
int32_t CMapChunk::TestBit(const uint8_t* bits, uint32_t index) {
    return ((1 << (index & 7)) & bits[index >> 3]) != 0;
}

// OFFSET: 0x0073B440
void CMapChunkHeader::Set(const SMChunk& chunk) {
    this->m_flags = chunk.m_flags;
    this->m_areaId = chunk.m_areaId;
    this->m_nDoodadRefs = static_cast<uint16_t>(chunk.m_nDoodadRefs);
    this->m_nMapObjRefs = static_cast<uint16_t>(chunk.m_nMapObjRefs);
    this->m_nLayers = static_cast<uint16_t>(chunk.m_nLayers);
    this->m_nSndEmitters = static_cast<uint16_t>(chunk.m_nSndEmitters);
    memcpy(this->m_lowQualityTexMap, chunk.m_lowQualityTexMap, sizeof(this->m_lowQualityTexMap));
    memcpy(this->m_noEffectDoodad, chunk.m_noEffectDoodad, sizeof(this->m_noEffectDoodad));
}

// OFFSET: 0x0073B4D0/0x140418B60
void CMapChunk::CreateMaterial() {
    if (this->m_shaderTexture) {
        TextureRelease(this->m_shaderTexture);
    }

    TerrainMaterialRec* material = this->m_materialId ? g_terrainMaterialDB.GetRecord(this->m_materialId) : NULL;

    if (material) {
        switch (material->m_shader) {
            case 0:
                this->m_shaderType = 2;
                break;

            case 1:
                this->m_shaderType = 3;
                break;

            case 2:
                this->m_shaderType = 4;
                break;

            case 3:
                this->m_shaderType = 5;
                break;
        }

        const char* envMap = material->m_envMapPath;

        if (*envMap) {
            this->m_shaderTexture = CMap::LoadTerrainTexture(envMap, 1);
        }

        return;
    }

    int32_t hasHighBit = 0;
    int32_t hasEnvMap = 0;
    int32_t hasDynamic = 0;
    uint32_t textureId = 0;

    if (this->m_header.m_nLayers) {
        const SMLayer* layer = this->m_layers;
        textureId = layer->m_textureId;

        for (uint32_t i = 0; i < this->m_header.m_nLayers; i++, layer++) {
            uint32_t flags = layer->m_flags;

            if (static_cast<int8_t>(flags) < 0) {
                hasHighBit = 1;
            }

            if (flags & 0x400) {
                hasEnvMap = 1;
            }

            if (flags & 0x8000) {
                hasDynamic = 1;
            }
        }
    }

    if (this->m_map->m_header.m_flags & 0x4) {
        if (hasEnvMap) {
            if (hasDynamic) {
                this->m_shaderTexture = CMap::GetDynamicTexture(textureId);
            } else {
                this->m_shaderTexture = this->m_area->LoadTexture(textureId);
            }

            this->m_shaderType = 3;
        } else {
            this->m_shaderType = 2;
        }
    } else {
        this->m_shaderType = hasHighBit != 0;
    }
}

// OFFSET: 0x0073B660/0x140418D50
void CMapChunk::CreatePointers(uint8_t* data, int32_t isNew, int32_t root) {
    int32_t size = reinterpret_cast<uint32_t*>(data)[1];
    uint32_t* chunkData = reinterpret_cast<uint32_t*>(&data[8]);

    if (root) {
        const SMChunk* chunk = reinterpret_cast<const SMChunk*>(&data[8]);
        this->m_holes = chunk->m_holes;
        this->m_holesHigh = chunk->m_holesHigh;
        this->m_position = chunk->m_position;
        this->m_header.Set(*chunk);
        chunkData = reinterpret_cast<uint32_t*>(&data[136]);
        size -= 128;
    }

    this->ProcessIffChunks(chunkData, size, isNew);
}

// OFFSET: 0x0073B700/0x140418E30
void CMapChunk::SelectLights(CM2Lighting* lighting) {
    lighting->AddLight(&CWorldMap::s_sunLight);

    const DayNight::FogInfo* fog = DayNight::GetFogInfo();
    C3Vector color(fog->m_color.r * 0.0039215689f, fog->m_color.g * 0.0039215689f, fog->m_color.b * 0.0039215689f);
    lighting->SetFog(color, fog->m_start, fog->m_end, fog->m_density);
}

// OFFSET: 0x0073F710/0x14041D680
void CMapChunk::GetLiquidBox(CAaBox& box) {
    box.t.x = this->m_bbox.t.x;
    box.t.y = this->m_bbox.t.y;
    box.t.z = this->m_bbox.t.z;
    box.b.x = this->m_bbox.b.x;
    box.b.y = this->m_bbox.b.y;
    box.b.z = this->m_bbox.b.z;

    CChunkLiquid* liquid = this->m_liquidList.Head();

    if (!liquid) {
        return;
    }

    box.t.z = liquid->m_maxHeight;
    box.b.z = liquid->m_minHeight;

    for (; liquid; liquid = this->m_liquidList.Next(liquid)) {
        box.t.z = box.t.z <= liquid->m_maxHeight ? liquid->m_maxHeight : box.t.z;
        box.b.z = liquid->m_minHeight <= box.b.z ? liquid->m_minHeight : box.b.z;
    }
}

// OFFSET: 0x0073F7B0/0x14041D720
int32_t CMapChunk::GetLiquidHeight(const C2Vector& pos, float& height) {
    float x = pos.x;
    float y = pos.y;
    C2Vector frac(x - static_cast<float>(static_cast<int32_t>(x)), y - static_cast<float>(static_cast<int32_t>(y)));
    C2iVector tile;
    tile.x = static_cast<int32_t>(x) & 0x7;
    tile.y = static_cast<int32_t>(y) & 0x7;

    for (CChunkLiquid* liquid = this->m_liquidList.Head(); liquid; liquid = this->m_liquidList.Next(liquid)) {
        if (liquid->TileExists(tile.x, tile.y) && liquid->GetHeight(frac, tile, height)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0073F870/0x14041D800
void CMapChunk::CreateLiquids(int32_t isNew) {
    for (CChunkLiquid* liquid = this->m_liquidList.Head(), * next; liquid; liquid = next) {
        next = this->m_liquidList.Next(liquid);
        liquid->Purge();
        liquid->m_chunkLink.Unlink();
        CMap::FreeChunkLiquid(liquid);
    }

    const uint32_t* mcLiquid = static_cast<const uint32_t*>(this->m_mcLiquid);

    if (mcLiquid) {
        uint32_t bit = 4;

        for (uint32_t i = 0; i < 4; i++, bit <<= 1) {
            if (!(bit & this->m_header.m_flags)) {
                continue;
            }

            uint32_t liquidType = i + 1;

            if (this->m_map->m_mapId == 530 && i == 2) {
                liquidType = 15;
            }

            CChunkLiquid* liquid = CMap::AllocChunkLiquid();
            this->m_liquidList.LinkToTail(liquid);
            liquid->m_liquidType = liquidType;
            liquid->m_owner = this;

            switch (i) {
                case 0:
                case 1:
                    liquid->m_liquidObject = 4;
                    break;

                case 2:
                case 3:
                    liquid->m_liquidObject = 5;
                    break;
            }

            liquid->m_position = this->m_position;
            liquid->m_minHeight = *reinterpret_cast<const float*>(&mcLiquid[0]);
            liquid->m_maxHeight = *reinterpret_cast<const float*>(&mcLiquid[1]);
            liquid->m_xOffset = 0;
            liquid->m_yOffset = 0;
            liquid->m_width = 8;
            liquid->m_height = 8;
            liquid->m_vertexData = &mcLiquid[2];
            liquid->m_tileData = &mcLiquid[164];
            liquid->m_vertexCount = 81;
            mcLiquid = &mcLiquid[201];
            liquid->CreateVertZ();
        }
    }

    const void* liquidData = this->m_area->m_liquid;

    if (liquidData) {
        this->m_liquidChunk = const_cast<SMLiquidChunkDesc*>(Liquid::GetChunkDesc(liquidData, this->m_indexX, this->m_indexY));
        const SMLiquidChunkDesc* desc = static_cast<const SMLiquidChunkDesc*>(this->m_liquidChunk);

        for (uint32_t i = 0; i < desc->m_layerCount; i++) {
            const SMLiquidInstance* instance = Liquid::GetInstanceDesc(liquidData, desc, i);
            CChunkLiquid* liquid = CMap::AllocChunkLiquid();
            this->m_liquidList.LinkToTail(liquid);
            liquid->m_owner = this;
            liquid->m_liquidHeader = instance;
            liquid->m_liquidType = instance->m_liquidType;
            liquid->m_liquidObject = Liquid::GetVertexFormat(instance->m_liquidType, instance->m_vertexFormat);
            liquid->m_position = this->m_position;
            liquid->m_minHeight = instance->m_minHeight;
            liquid->m_maxHeight = instance->m_maxHeight;
            liquid->m_xOffset = 0;
            liquid->m_yOffset = 0;
            liquid->m_width = 8;
            liquid->m_height = 8;

            int32_t width = 8;
            int32_t height = 8;

            if (instance->m_vertexFormat < 42) {
                liquid->m_xOffset = instance->m_yOffset;
                liquid->m_yOffset = instance->m_xOffset;
                liquid->m_width = instance->m_yOffset + instance->m_height;
                liquid->m_height = instance->m_xOffset + instance->m_width;
                width = instance->m_width;
                height = instance->m_height;
            }

            liquid->m_vertexData = Liquid::GetInstanceVertices(liquidData, instance);

            if (!liquid->m_vertexData && liquid->m_liquidType != 2) {
                liquid->m_liquidObject = 2;
            }

            liquid->m_vertexCount = (width + 1) * (height + 1);
            liquid->CreateVertZ();
        }
    }

    CChunkLiquid::CreateVertXY();
}

// OFFSET: 0x0073FC30/0x14041DC00
void CMapChunk::CreateBounds() {
    this->m_bbox.b.x = 3.402823466e38f;
    this->m_bbox.b.y = 3.402823466e38f;
    this->m_bbox.b.z = 3.402823466e38f;
    this->m_bbox.t.x = -3.402823466e38f;
    this->m_bbox.t.y = -3.402823466e38f;
    this->m_bbox.t.z = -3.402823466e38f;

    float chunkX = static_cast<float>(this->m_chunkX);
    float chunkX1 = static_cast<float>(this->m_chunkX + 1);
    this->m_bbox.b.x = 17066.666f - static_cast<float>(this->m_chunkY + 1) * 33.333332f;
    this->m_bbox.b.y = 17066.666f - chunkX1 * 33.333332f;
    this->m_bbox.t.x = 17066.666f - static_cast<float>(this->m_chunkY) * 33.333332f;
    this->m_bbox.t.y = 17066.666f - chunkX * 33.333332f;

    const float* vertices = this->m_vertices;

    for (uint32_t i = 0; i < 145; i++) {
        if (this->m_bbox.b.z > vertices[i]) {
            this->m_bbox.b.z = vertices[i];
        }

        if (vertices[i] > this->m_bbox.t.z) {
            this->m_bbox.t.z = vertices[i];
        }
    }

    float top = this->m_position.z + this->m_bbox.t.z;
    this->m_bbox.b.z = this->m_position.z + this->m_bbox.b.z;
    this->m_bbox.t.z = top;
    this->m_terrainBox = this->m_bbox;

    if (!this->m_liquidList.Head()) {
        this->m_liquidBox.b.x = 3.402823466e38f;
        this->m_liquidBox.b.y = 3.402823466e38f;
        this->m_liquidBox.t.x = -3.402823466e38f;
        this->m_liquidBox.b.z = 3.402823466e38f;
        this->m_liquidBox.t.y = -3.402823466e38f;
        this->m_liquidBox.t.z = -3.402823466e38f;
    } else {
        this->GetLiquidBox(this->m_liquidBox);
        this->m_bbox.t.z = this->m_bbox.t.z <= this->m_liquidBox.t.z ? this->m_liquidBox.t.z : this->m_bbox.t.z;
        this->m_bbox.b.z = this->m_liquidBox.b.z <= this->m_bbox.b.z ? this->m_liquidBox.b.z : this->m_bbox.b.z;
    }

    CMapArea* area = this->m_area;
    area->m_bbox.t.z = this->m_bbox.t.z <= area->m_bbox.t.z ? area->m_bbox.t.z : this->m_bbox.t.z;
    area->m_bbox.b.z = area->m_bbox.b.z <= this->m_bbox.b.z ? area->m_bbox.b.z : this->m_bbox.b.z;
    this->m_area->m_chunkBounds[this->m_indexX + 16 * this->m_indexY] = this->m_bbox;
}

// OFFSET: 0x0073FF40/0x14041DFA0
void CMapChunk::CreateRefs(CMapArea* area) {
    C3Vector offset(17066.666f, 17066.666f, 0.0f);
    const uint32_t* doodadRefs;
    const uint32_t* mapObjRefs;

    if (this->m_refs) {
        doodadRefs = this->m_refs;
        mapObjRefs = &this->m_refs[this->m_header.m_nDoodadRefs];
    } else {
        doodadRefs = this->m_doodadRefs;
        mapObjRefs = this->m_mapObjRefs;
    }

    for (uint32_t i = 0; i < this->m_header.m_nMapObjRefs; i++) {
        SMMapObjDef& def = area->m_mapObjDefs[mapObjRefs[i]];

        if (def.m_flags & 0x1) {
            continue;
        }

        CMapObjDef* mapObjDef = this->m_worldMap->CreateMapObjDef(&area->m_mapObjNames[area->m_mapObjNameOffsets[def.m_nameId]], def, offset, 1);
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDef, this);
        mapObjDef->m_parentLinkList.LinkToTail(link);
        this->m_mapObjDefLinkList.LinkToTail(link);
    }

    for (uint32_t i = 0; i < this->m_header.m_nDoodadRefs; i++) {
        SMDoodadDef& def = area->m_doodadDefs[doodadRefs[i]];
        C3Vector doodadOffset(17066.666f, 17066.666f, 0.0f);
        CMapDoodadDef* doodadDef = this->m_worldMap->CreateDoodadDef(&area->m_doodadNames[area->m_doodadNameOffsets[def.m_nameId]], def, doodadOffset);
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(doodadDef, this);
        doodadDef->m_parentLinkList.LinkToTail(link);
        this->m_doodadDefLinkList.LinkToTail(link);
        doodadDef->m_flags |= 0x1;

        if (doodadDef->m_flags & 0x40) {
            doodadDef->LinkToChunk(this);
        }
    }
}

// OFFSET: [0x00740180/0x14041E330, 0x007405F0/0x14041E8C0]
CMapChunk::~CMapChunk() {
    if (this->m_chunkFlags & 0x8) {
        SMemFree(const_cast<SMLayer*>(this->m_layers), __FILE__, __LINE__, 0);
    }

    if (this->m_chunkFlags & 0x10) {
        SMemFree(const_cast<uint8_t*>(this->m_alphaMaps), __FILE__, __LINE__, 0);
    }

    if (this->m_chunkFlags & 0x20) {
        SMemFree(const_cast<uint32_t*>(this->m_vertexLighting), __FILE__, __LINE__, 0);
    }

    if (this->m_chunkFlags & 0x40) {
        SMemFree(const_cast<uint32_t*>(this->m_vertexColors), __FILE__, __LINE__, 0);
    }

    if (this->m_shaderTexture) {
        TextureRelease(this->m_shaderTexture);
    }
}

// OFFSET: 0x00740350/0x14041E5F0
void CMapChunk::Create(int32_t isNew) {
    float chunkX = static_cast<float>(this->m_chunkX) * 33.333332f;
    this->m_position.x = 17066.666f - static_cast<float>(this->m_chunkY) * 33.333332f;
    this->m_position.y = 17066.666f - chunkX;
    this->m_lowQualityTexMap = this->m_header.m_lowQualityTexMap;
    this->m_noEffectDoodad = this->m_header.m_noEffectDoodad;

    if (this->m_header.m_flags & 0x2) {
        this->m_flags |= 0x20;
    }

    this->CreateMaterial();
    this->CreateLiquids(isNew);
    this->CreateBounds();
    this->CreateRefs(this->m_area);
    this->m_area->m_chunks[this->m_indexX + 16 * this->m_indexY] = this;
    this->m_flags |= 0x40;
}

// OFFSET: 0x00740400/0x14041E6C0
CMapChunk::CMapChunk() {
    this->m_indexX = 0;
    this->m_indexY = 0;
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_chunkX = 0;
    this->m_chunkY = 0;
    this->m_header.m_flags = 0;
    this->m_header.m_areaId = 0;
    this->m_header.m_nLayers = 0;
    this->m_header.m_nMapObjRefs = 0;
    this->m_header.m_nDoodadRefs = 0;
    memset(this->m_header.m_lowQualityTexMap, 0, sizeof(this->m_header.m_lowQualityTexMap));
    memset(this->m_header.m_noEffectDoodad, 0, sizeof(this->m_header.m_noEffectDoodad));
    this->m_header.m_nSndEmitters = 0;
    this->m_terrainBox.b.x = 0.0f;
    this->m_terrainBox.b.y = 0.0f;
    this->m_terrainBox.b.z = 0.0f;
    this->m_terrainBox.t.x = 0.0f;
    this->m_terrainBox.t.y = 0.0f;
    this->m_terrainBox.t.z = 0.0f;
    this->m_liquidBox.b.x = 0.0f;
    this->m_liquidBox.b.y = 0.0f;
    this->m_liquidBox.b.z = 0.0f;
    this->m_liquidBox.t.x = 0.0f;
    this->m_liquidBox.t.y = 0.0f;
    this->m_liquidBox.t.z = 0.0f;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_map = NULL;
    this->m_area = NULL;
    this->m_worldMap = NULL;
    this->m_detailDoodadInst = NULL;
    this->m_renderChunk = NULL;
    this->m_lowQualityTexMap = NULL;
    this->m_noEffectDoodad = NULL;
    this->m_vertices = NULL;
    this->m_vertexColors = NULL;
    this->m_chunkFlags &= ~0x7Fu;
    this->m_type |= 0x4;
    this->m_vertexLighting = NULL;
    this->m_normals = NULL;
    this->m_shadowMap = NULL;
    this->m_layers = NULL;
    this->m_alphaMaps = NULL;
    this->m_mcLiquid = NULL;
    this->m_liquidChunk = NULL;
    this->m_refs = NULL;
    this->m_doodadRefs = NULL;
    this->m_mapObjRefs = NULL;
    this->m_materialId = 0;
    this->m_shaderTexture = NULL;
    this->m_holes = 0;
    this->m_holesHigh = 0;
}

struct DetailDoodadCell {
    /* 0000 */ uint32_t x;
    /* 0004 */ uint32_t y;

    DetailDoodadCell() : x(0), y(0) {
    }
};

// OFFSET: 0x00783980/0x140471E70
void CMapChunk::CreateDetailDoodads() {
    CRndSeed seed(0);
    seed.SetSeed(this->m_chunkX | (this->m_chunkY << 16));

    if (World::detailDoodadTest) {
        return;
    }

    static C4Plane s_planes[256];
    static DetailDoodadCell s_cells[256];
    static CImVector s_colors[64];
    static CImVector s_lights[64];
    uint8_t cellMask[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    uint32_t count = World::detailDoodadDensity;

    for (uint32_t i = 0; i < World::detailDoodadDensity; i++) {
        uint32_t x = CRandom::uint32_(seed) & 0x7;
        uint32_t y = CRandom::uint32_(seed) & 0x7;
        s_cells[i].x = x;
        s_cells[i].y = y;

        if (cellMask[y] & (1 << x)) {
            continue;
        }

        uint32_t cell = x + 8 * y;
        uint32_t base = x + 17 * y;
        cellMask[y] |= 1 << x;
        const float* heights = &this->m_vertices[base];
        float offsetX = static_cast<float>(static_cast<int32_t>(y)) * -4.1666665f;
        float offsetY = static_cast<float>(static_cast<int32_t>(x)) * -4.1666665f;
        float centerX = offsetX + s_subchunkCoords[4].x;
        float centerY = offsetY + s_subchunkCoords[4].y;
        float centerZ = heights[9];
        C4Plane* plane = &s_planes[4 * cell];

        for (uint32_t k = 0; k < 4; k++) {
            const C3Vector& a = s_subchunkCoords[s_fanCorners[k][0]];
            const C3Vector& b = s_subchunkCoords[s_fanCorners[k][1]];
            float ax = offsetX + a.x - centerX;
            float ay = a.y + offsetY - centerY;
            float bx = b.x + offsetX - centerX;
            float by = b.y + offsetY - centerY;
            float ah = heights[s_fanHeights[k][0]] - centerZ;
            float bh = heights[s_fanHeights[k][1]] - centerZ;
            plane->n.x = by * ah - bh * ay;
            plane->n.y = bh * ax - ah * bx;
            plane->n.z = ay * bx - by * ax;
            float scale = 1.0f / sqrt(plane->n.x * plane->n.x + plane->n.y * plane->n.y + plane->n.z * plane->n.z);
            plane->n.x = plane->n.x * scale;
            plane->n.y = plane->n.y * scale;
            plane->n.z = plane->n.z * scale;
            plane->d = -(plane->n.z * centerZ + centerY * plane->n.y + plane->n.x * centerX);
            plane++;
        }

        if (this->m_vertexColors) {
            const uint8_t* colors = reinterpret_cast<const uint8_t*>(&this->m_vertexColors[base]);
            int32_t r = ((colors[2] + colors[6] + colors[70] + colors[74]) >> 2) * 2;
            int32_t g = ((colors[1] + colors[5] + colors[69] + colors[73]) >> 2) * 2;
            int32_t b = ((colors[0] + colors[4] + colors[68] + colors[72]) >> 2) * 2;
            s_colors[cell] = CImVector(b > 255 ? 255 : b, g > 255 ? 255 : g, r > 255 ? 255 : r, 0xFF);
        }

        if (this->m_vertexLighting) {
            const uint8_t* lights = reinterpret_cast<const uint8_t*>(&this->m_vertexLighting[base]);
            int32_t r = (lights[2] + lights[6] + lights[70] + lights[74]) >> 2;
            int32_t g = (lights[1] + lights[5] + lights[69] + lights[73]) >> 2;
            int32_t b = (lights[0] + lights[4] + lights[68] + lights[72]) >> 2;
            s_lights[cell] = CImVector(b > 255 ? 255 : b, g > 255 ? 255 : g, r > 255 ? 255 : r, 0xFF);
        }
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    CImVector color(0);
    CImVector light(0);

    for (uint32_t i = 0; i < count; i++) {
        uint32_t x = s_cells[i].x;
        uint32_t y = s_cells[i].y;

        if ((s_noEffectDoodadMasks[x] & this->m_noEffectDoodad[y]) >> s_noEffectDoodadShifts[x]) {
            continue;
        }

        if (this->m_holes & s_holeMasks[4 * (y >> 1) + (x >> 1)]) {
            continue;
        }

        uint32_t layer = (s_lowQualityTexMasks[x] & reinterpret_cast<const uint16_t*>(this->m_lowQualityTexMap)[y]) >> s_lowQualityTexShifts[x];
        GroundEffectTextureRec* rec = g_groundEffectTextureDB.GetRecord(this->m_layers[layer].m_effectId);

        if (!rec) {
            continue;
        }

        uint32_t density = rec->m_density;

        if (!density) {
            density = 8;
        }

        uint32_t ids[16];
        uint32_t slot = 0;
        uint32_t total = 0;

        for (uint32_t k = 0; k < 4; k++) {
            uint32_t weight = rec->m_doodadWeight[k];
            uint32_t id = rec->m_doodadID[k];

            if (weight) {
                total += weight;

                do {
                    ids[slot & 0xF] = id;
                    slot += 13;
                } while (--weight);
            }
        }

        for (; total < 16; total++) {
            ids[slot & 0xF] = rec->m_doodadID[total & 0x3];
            slot += 13;
        }

        float cellX = static_cast<float>(x) * 4.1666665f;
        float cellY = static_cast<float>(y) * 4.1666665f;
        uint32_t cellIndex = x + 8 * y;

        for (uint32_t j = 0; j < density; j++) {
            float rx = CRandom::sfloat_(seed);
            float ry = CRandom::sfloat_(seed);
            uint32_t id = ids[(j + i) & 0xF];
            float u = rx * 2.0833333f + 2.0833333f;
            float v = ry * 2.0833333f + 2.0833333f;

            if (!id) {
                continue;
            }

            CDetailDoodadData* data = CDetailDoodad::s_doodadList[id];
            float px = -u;
            float py = -v;
            uint32_t tri = 0;

            if (px - py < 0.0f) {
                tri = 1;
            }

            if ((-px - 4.1666665f) - py > 0.0f) {
                tri += 2;
            }

            uint32_t planeIndex = 4 * cellIndex + tri;
            const C4Plane& plane = s_planes[planeIndex];
            float nz = plane.n.z;

            if (nz < 0.40000001f) {
                continue;
            }

            light = this->m_vertexLighting ? s_lights[cellIndex] : CImVector(0xFF000000);
            color = (data->m_record->m_flags & 0x2) || !this->m_vertexColors ? CImVector(0xFFFFFFFF) : s_colors[cellIndex];

            if (this->m_shadowMap) {
                const SMAreaHeader* header = this->m_area->m_header;
                int32_t sx = static_cast<int32_t>((u + cellX) * 1.92f);
                int32_t sy = static_cast<int32_t>((v + cellY) * 1.92f);

                if (!header || !header->m_mamp) {
                    if (this->m_shadowMap[(sx >> 3) + 8 * sy] & (1 << (sx & 0x7))) {
                        color.a = 0;
                    }
                } else if (header->m_mamp == 1) {
                    if (this->m_shadowMap[(sx >> 4) + 4 * (sy >> 1)] & (1 << ((sx >> 1) & 0x7))) {
                        color.a = 0;
                    }
                }
            }

            position.x = py - cellY;
            position.y = px - cellX;
            position.z = -((plane.n.y * position.y + plane.n.x * position.x + plane.d) / nz);
            float rotation = (CRandom::sfloat_(seed) + 1.0f) * 3.1415927f;
            float scale = CRandom::sfloat_(seed) * 0.33000001f + 1.0f;
            float windPhase = CRandom::sfloat_(seed) * 6.2831855f;
            this->m_detailDoodadInst->AddDoodad(id, position, rotation, scale, plane, static_cast<uint16_t>(planeIndex), color, light, windPhase);
        }
    }
}

// OFFSET: 0x007848B0/0x1404734B0
void CMapChunk::CreateDetailDoodadInst() {
    if (!this->m_header.m_nLayers || !this->LoadDetailDoodadModels()) {
        return;
    }

    this->m_detailDoodadInst = CDetailDoodad::AllocInst();
    this->m_detailDoodadInst->m_chunk = this;
    this->CreateDetailDoodads();
}

// OFFSET: 0x00786690/0x1404758D0
void CMapChunk::Purge() {
    if (this->m_renderChunk) {
        CMap::FreeRenderChunk(this->m_renderChunk);
        this->m_renderChunk = NULL;
    }

    if (this->m_detailDoodadInst) {
        CDetailDoodad::FreeInst(this->m_detailDoodadInst);
        this->m_detailDoodadInst = NULL;
    }

    for (CChunkLiquid* liquid = this->m_liquidList.Head(), * next; liquid; liquid = next) {
        next = this->m_liquidList.Next(liquid);
        liquid->m_chunkLink.Unlink();
        liquid->Purge();
        CMap::FreeChunkLiquid(liquid);
    }

    for (CMapBaseObjLink* link = this->m_doodadDefLinkList.Head(), * next; link; link = next) {
        next = this->m_doodadDefLinkList.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        doodadDef->Purge();
    }

    for (CMapBaseObjLink* link = this->m_mapObjDefLinkList.Head(), * next; link; link = next) {
        next = this->m_mapObjDefLinkList.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        mapObjDef->Purge(1);
    }

    for (CMapBaseObjLink* link = this->m_entityLinkList.Head(), * next; link; link = next) {
        next = this->m_entityLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
    }
}

// OFFSET: 0x00788080/0x140477970
int32_t CMap::GetHeightTerrain(const CMapChunk* chunk, const C3Vector& pos, int32_t subX, int32_t subY, int32_t a5, float& height) {
    if (a5 && (chunk->m_holes & s_holeMasks[4 * (subY >> 1) + (subX >> 1)])) {
        return 0;
    }

    float originX = subY * -4.1666665f;
    float originY = subX * -4.1666665f;
    float dx = pos.x - chunk->m_position.x;
    float centerX = originX + s_subchunkCoords[4].x;
    float centerY = originY + s_subchunkCoords[4].y;
    float dy = pos.y - chunk->m_position.y;
    uint32_t fan = 0;

    if (((originX + s_subchunkCoords[2].x) - (originX + s_subchunkCoords[0].x)) * ((originY + s_subchunkCoords[2].y) - dy) - ((originY + s_subchunkCoords[2].y) - (originY + s_subchunkCoords[0].y)) * ((originX + s_subchunkCoords[2].x) - dx) <= 0.0f) {
        fan = 1;
    }

    if (((originX + s_subchunkCoords[3].x) - (originX + s_subchunkCoords[1].x)) * ((originY + s_subchunkCoords[3].y) - dy) - ((originY + s_subchunkCoords[3].y) - (originY + s_subchunkCoords[1].y)) * ((originX + s_subchunkCoords[3].x) - dx) <= 0.0f) {
        fan += 2;
    }

    const float* heights = &chunk->m_vertices[subY + subX + 16 * subY];
    float centerZ = heights[9];
    float ax = (s_subchunkCoords[s_fanCorners[fan][0]].x + originX) - centerX;
    float ay = (s_subchunkCoords[s_fanCorners[fan][0]].y + originY) - centerY;
    float az = heights[s_fanHeights[fan][0]] - centerZ;
    float bx = (s_subchunkCoords[s_fanCorners[fan][1]].x + originX) - centerX;
    float by = (s_subchunkCoords[s_fanCorners[fan][1]].y + originY) - centerY;
    float bz = heights[s_fanHeights[fan][1]] - centerZ;

    float nx = bz * ay - az * by;
    float ny = az * bx - bz * ax;
    float nz = by * ax - bx * ay;
    float inv = 1.0f / sqrt(nz * nz + ny * ny + nx * nx);
    nx = inv * nx;
    ny = inv * ny;
    nz = inv * nz;

    height = chunk->m_position.z - ((ny * dy + nx * dx) - (centerY * ny + centerX * nx + nz * centerZ)) / nz;
    return 1;
}

static uint32_t FloatSign(float value) {
    return *reinterpret_cast<uint32_t*>(&value) >> 31;
}

static uint8_t BoxClipCode(const C3Vector& v, const CAaBox& box) {
    return static_cast<uint8_t>(FloatSign(v.x - box.b.x + 0.019444443f) | (FloatSign(v.y - box.b.y + 0.019444443f) << 1) | (FloatSign(v.z - box.b.z + 0.019444443f) << 2) | (FloatSign(box.t.x - v.x + 0.019444443f) << 3) | (FloatSign(box.t.y - v.y + 0.019444443f) << 4) | (FloatSign(box.t.z - v.z + 0.019444443f) << 5));
}

// OFFSET: 0x007883E0/0x140477D40
bool CMap::GetHeightDoodadDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, const C3Vector& start, const C3Vector& end) {
    for (CMapBaseObjLink* link = list.Head(), * next; link; link = next) {
        next = list.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->unk_0020 == CWorldMap::s_queryTag) {
            continue;
        }

        doodadDef->unk_0020 = CWorldMap::s_queryTag;

        if (!(doodadDef->m_flags & 0x40)) {
            return false;
        }

        if (start.x > doodadDef->m_bbox.t.x || doodadDef->m_bbox.b.x > start.x) {
            continue;
        }

        if (start.y > doodadDef->m_bbox.t.y || doodadDef->m_bbox.b.y > start.y || end.z > doodadDef->m_bbox.t.z || doodadDef->m_bbox.b.z > start.z) {
            continue;
        }

        CM2Model* model = doodadDef->m_model;

        if (!(model->m_fFlags & 0x1)) {
            continue;
        }

        if (!model->m_hitTestPrev) {
            CM2Model** head = &model->m_scene->m_hitTestList;
            model->m_hitTestPrev = head;
            model->m_hitTestNext = *head;
            *head = model;

            if (model->m_hitTestNext) {
                model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
            }
        }

        model->m_hitTestMode = 3;
        model->m_hitTestGroup = 0;
        model->m_hitTestOwner = doodadDef;
    }

    return true;
}

// OFFSET: 0x00788500/0x140477E80
bool CMap::GetHeightMapObjDef(CMapObjDef* mapObjDef, const C3Vector& start, const C3Vector& end, float& t) {
    if (!(mapObjDef->m_mapObj->m_flags & 0x40)) {
        return false;
    }

    for (CMapObjDefGroup* group = mapObjDef->m_defGroupList.Head(), * next; group; group = next) {
        next = mapObjDef->m_defGroupList.Next(group);

        if (start.x > group->m_bbox.t.x || group->m_bbox.b.x > start.x) {
            continue;
        }

        if (start.y > group->m_bbox.t.y || group->m_bbox.b.y > start.y || end.z > group->m_bbox.t.z || group->m_bbox.b.z > start.z) {
            continue;
        }

        if (!CMap::GetHeightDoodadDefs(group->m_doodadLinks, start, end)) {
            return false;
        }
    }

    C3Vector localStart = start * mapObjDef->m_invMat;
    C3Vector localEnd = end * mapObjDef->m_invMat;

    if (mapObjDef->m_mapObj->TestBounds(localStart, localEnd)) {
        float hit = t;

        if (mapObjDef->m_mapObj->VectorIntersect(localStart, localEnd, 0x220122, 0, &hit, NULL, mapObjDef, NULL, NULL) && t > hit) {
            t = hit;
        }
    }

    return true;
}

// OFFSET: 0x0077FDC0/0x14046D0D0
void CMap::RenderFlightBounds(const C3Vector& pos) {
    if (this->m_mapFlags & Flag_GlobalMapObj) {
        return;
    }

    CWorldView* view = CWorldScene::s_defaultWorldView;

    if (!view) {
        return;
    }

    int32_t ay = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.001875f);
    int32_t ax = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.001875f);
    CImVector color = g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba ? CImVector(0x80FFFF80) : CImVector(0x8080FFFF);
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), 882);
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 2352);
    CGxVertexPCT* vertexStart = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(vertexBuf));
    uint16_t* indexStart = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    CGxVertexPCT* vertex = vertexStart;
    uint16_t* index = indexStart;

    for (int32_t x = ax - 3; x <= ax + 3; x++) {
        for (int32_t y = ay - 3; y <= ay + 3; y++) {
            CMapAreaTableEntry* entry = this->m_areaTable.Get(y, x);

            if (!entry || !(entry->m_flags & 0x2)) {
                continue;
            }

            CMapArea* area = entry->m_area;

            if (!area || !area->m_flightBounds) {
                continue;
            }

            uint16_t base = static_cast<uint16_t>(vertex - vertexStart);
            static C2Vector s_texCoords[9] = { C2Vector(0.0f, 0.0f), C2Vector(1.0f, 0.0f), C2Vector(2.0f, 0.0f), C2Vector(0.0f, 1.0f), C2Vector(1.0f, 1.0f), C2Vector(2.0f, 1.0f), C2Vector(0.0f, 2.0f), C2Vector(1.0f, 2.0f), C2Vector(2.0f, 2.0f) };
            const int16_t* heights = static_cast<const int16_t*>(area->m_flightBounds);

            for (uint32_t layer = 0; layer < 2; layer++) {
                for (uint32_t i = 0; i < 9; i++) {
                    vertex->p.x = s_flightBoundsVerts[i].x + area->m_posX;
                    vertex->p.y = s_flightBoundsVerts[i].y + area->m_posY;
                    vertex->p.z = static_cast<float>(heights[9 * layer + i]);
                    vertex->c = color;
                    vertex->tc = s_texCoords[i];
                    vertex++;
                }
            }

            for (uint32_t layer = 0; layer < 2; layer++) {
                for (uint32_t t = 0; t < 8; t++) {
                    for (uint32_t k = 0; k < 3; k++) {
                        *index++ = static_cast<uint16_t>(base + s_flightBoundsTris[t][k] + 9 * layer);
                    }
                }
            }
        }
    }

    uint32_t vertexCount = static_cast<uint32_t>(vertex - vertexStart);
    uint32_t indexCount = static_cast<uint32_t>(index - indexStart);
    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;
    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;

    if (!indexCount) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    C44Matrix world;
    world.Translate(C3Vector(-view->m_camPos.x, -view->m_camPos.y, -view->m_camPos.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_Culling, 0);
    CStatus status;
    CGxTexFlags flags;
    flags.m_wrapU = 1;
    flags.m_wrapV = 1;
    CTexture* texture = TextureCreate("XTextures\\FX\\LBTBarrier", flags, &status, 0, NULL, 2);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(texture));
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, static_cast<EGxVertexBufferFormat>(9));
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    DefaultShaders::SetFogEnabled(0);
    DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), -1);
    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(vertexCount - 1);
    g_theGxDevicePtr->Draw(batch);
    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
    TextureRelease(texture);
}

// OFFSET: 0x00788780/0x140478150
int32_t CMap::GetFlightBounds(const C3Vector& pos, float& z, int32_t lower) {
    if (this->m_mapFlags & Flag_GlobalMapObj) {
        return 0;
    }

    uint32_t base = 0;

    if (lower == 1) {
        base = 9;
    }

    float y = pos.y;
    int32_t cellY = static_cast<int32_t>(-(y - 17066.666f) * 0.0037499999f);
    float x = pos.x;
    int32_t cellX = static_cast<int32_t>(-(x - 17066.666f) * 0.0037499999f);
    CMapAreaTableEntry* entry = this->m_areaTable.Get(cellY >> 1, cellX >> 1);

    if (!entry || !(entry->m_flags & 0x2)) {
        return 0;
    }

    CMapArea* area = entry->m_area;

    if (!area || !area->m_flightBounds) {
        return 0;
    }

    float dx = x - area->m_posX;
    float dy = y - area->m_posY;
    uint32_t tri;

    if (cellY & 1) {
        if (cellX & 1) {
            tri = dx <= dy ? 5 : 4;
        } else {
            tri = -533.33331f - dx <= dy ? 2 : 3;
        }
    } else if (cellX & 1) {
        tri = -533.33331f - dx <= dy ? 7 : 6;
    } else {
        tri = dx > dy;
    }

    const int16_t* bounds = static_cast<const int16_t*>(area->m_flightBounds);
    uint32_t i0 = s_flightBoundsTris[tri][0];
    uint32_t i1 = s_flightBoundsTris[tri][1];
    uint32_t i2 = s_flightBoundsTris[tri][2];
    float x0 = s_flightBoundsVerts[i0].x;
    float y0 = s_flightBoundsVerts[i0].y;
    float ex = s_flightBoundsVerts[i2].x - x0;
    float fx = s_flightBoundsVerts[i1].x - x0;
    float z0 = static_cast<float>(bounds[base + i0]);
    float fy = s_flightBoundsVerts[i1].y - y0;
    float fz = static_cast<float>(bounds[base + i1]) - z0;
    float ey = s_flightBoundsVerts[i2].y - y0;
    float ez = static_cast<float>(bounds[base + i2]) - z0;
    float nz = ey * fx - fy * ex;
    float ny = fz * ex - ez * fx;
    float nx = fy * ez - fz * ey;
    float inv = 1.0f / sqrt(nz * nz + ny * ny + nx * nx);
    z = ((inv * ny) * dy - (z0 * (inv * nz) + y0 * (inv * ny) + x0 * (inv * nx)) + (inv * nx) * dx) * (-1.0f / (inv * nz));
    return 1;
}

// OFFSET: 0x00788A40/0x1404784E0
bool CMap::GetHeightMapObjDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, const C3Vector& start, const C3Vector& end, float& t) {
    for (CMapBaseObjLink* link = list.Head(), * next; link; link = next) {
        next = list.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->unk_004C == CWorldMap::s_queryTag) {
            continue;
        }

        mapObjDef->unk_004C = CWorldMap::s_queryTag;

        if (mapObjDef->TestAABox(start, end) && !CMap::GetHeightMapObjDef(mapObjDef, start, end, t)) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x00788AC0/0x1404785A0
bool CMap::QueryLiquidMapObjDef(CMapObjDef* mapObjDef, const C3Vector& pos, int32_t nearest, uint32_t& liquidType, float& height) {
    if (pos.x < mapObjDef->m_bbox.b.x || pos.y < mapObjDef->m_bbox.b.y || pos.z < mapObjDef->m_bbox.b.z) {
        return false;
    }

    if (mapObjDef->m_bbox.t.x < pos.x || mapObjDef->m_bbox.t.y < pos.y || mapObjDef->m_bbox.t.z < pos.z) {
        return false;
    }

    C3Vector local = pos * mapObjDef->m_invMat;
    uint32_t type;
    float localHeight;

    if (!mapObjDef->m_mapObj->TestBounds(local) || !mapObjDef->m_mapObj->QueryLiquid(0, nearest, local, type, localHeight)) {
        return false;
    }

    if (localHeight == FLT_MAX) {
        height = mapObjDef->m_bbox.t.z;
        liquidType = type;
    } else {
        height = mapObjDef->m_mat.a2 * local.x + mapObjDef->m_mat.b2 * local.y + mapObjDef->m_mat.c2 * localHeight + mapObjDef->m_mat.d2;
        liquidType = type;
    }

    return true;
}

// OFFSET: 0x00788C90
void CMap::QueryLightingColorChunk(const CMapChunk* chunk, const C3Vector& pos, int32_t cellY, int32_t cellX, CImVector& color) {
    const uint8_t* lighting = reinterpret_cast<const uint8_t*>(chunk->m_vertexLighting);

    if (!lighting) {
        color = CImVector(0);
        return;
    }

    int32_t fx = static_cast<int32_t>((((pos.x - chunk->m_position.x) - static_cast<float>(cellX) * -4.1666665f) * -0.23999999f) * 255.0f);
    int32_t fy = static_cast<int32_t>((((pos.y - chunk->m_position.y) - static_cast<float>(cellY) * -4.1666665f) * -0.23999999f) * 255.0f);
    const uint8_t* v = lighting + 4 * (cellX + cellY + 16 * cellX);
    int32_t b0 = (v[0] << 8) + ((fx * ((v[4] << 8) - (v[0] << 8))) >> 8);
    int32_t g0 = (v[1] << 8) + ((fx * ((v[5] << 8) - (v[1] << 8))) >> 8);
    int32_t r0 = (v[2] << 8) + ((fx * ((v[6] << 8) - (v[2] << 8))) >> 8);
    int32_t b1 = (v[36] << 8) + ((fx * ((v[40] << 8) - (v[36] << 8))) >> 8);
    int32_t g1 = (v[37] << 8) + ((fx * ((v[41] << 8) - (v[37] << 8))) >> 8);
    int32_t r1 = (v[38] << 8) + ((fx * ((v[42] << 8) - (v[38] << 8))) >> 8);
    color.b = static_cast<uint8_t>((b0 + ((fy * (b1 - b0)) >> 8)) >> 8);
    color.g = static_cast<uint8_t>((g0 + ((fy * (g1 - g0)) >> 8)) >> 8);
    color.r = static_cast<uint8_t>(static_cast<uint16_t>(r0 + ((fy * (r1 - r0)) >> 8)) >> 8);
    color.a = 0;
}

// OFFSET: 0x00788E50
bool CMap::QueryLightingColorTerrain(CMapChunk* chunk, const C3Vector& pos, CImVector& color) {
    int32_t cellY = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.23999999f) & 7;
    int32_t cellX = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.23999999f) & 7;
    CMap::QueryLightingColorChunk(chunk, pos, cellY, cellX, color);
    return true;
}

// OFFSET: 0x00788EC0/0x14040C2A0
uint32_t CMap::QueryAreaIdTerrain(const C3Vector& pos) {
    int32_t y = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.029999999f);
    int32_t x = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.029999999f);
    CMapAreaTableEntry* entry = this->m_areaTable.Get((y >> 4) & 0x3F, (x >> 4) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        return 0;
    }

    CMapChunk* chunk = entry->m_area->m_chunks[(y & 0xF) + 16 * (x & 0xF)];

    if (!chunk) {
        return 0;
    }

    return chunk->m_header.m_areaId;
}

// OFFSET: 0x00788F60
bool CMap::QueryGroundTypeTerrain(const C3Vector& pos, uint32_t& groundType) {
    float fy = -(pos.y - 17066.666f);
    float fx = -(pos.x - 17066.666f);

    if (fy < 0.0f || fx < 0.0f || fy >= 34133.332f || fx >= 34133.332f) {
        return false;
    }

    int32_t x = static_cast<int32_t>(fx * 0.23999999f);
    int32_t y = static_cast<int32_t>(fy * 0.23999999f);
    CMapAreaTableEntry* entry = this->m_areaTable.Get((y >> 7) & 0x3F, (x >> 7) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        return false;
    }

    CMapChunk* chunk = entry->m_area->m_chunks[((y >> 3) & 0xF) + 16 * ((x >> 3) & 0xF)];

    if (!chunk) {
        return false;
    }

    int32_t subY = static_cast<int32_t>(fy * 0.23999999f) & 7;
    int32_t subX = static_cast<int32_t>(fx * 0.23999999f) & 7;

    if (chunk->m_holes & s_holeMasks[4 * (subX >> 1) + (subY >> 1)]) {
        return false;
    }

    const SMLayer* layers = chunk->m_layers;

    if (!layers || !chunk->m_header.m_nLayers) {
        return false;
    }

    const uint16_t* texMap = reinterpret_cast<const uint16_t*>(chunk->m_lowQualityTexMap);
    uint32_t layer = static_cast<uint16_t>(s_lowQualityTexMasks[subY] & texMap[subX]) >> s_lowQualityTexShifts[subY];
    GroundEffectTextureRec* rec = g_groundEffectTextureDB.GetRecord(layers[layer].m_effectId);

    if (!rec) {
        return false;
    }

    groundType = rec->m_sound;
    return true;
}

// OFFSET: 0x007890B0/0x140478CF0
bool CMap::QueryShadowTerrain(const C3Vector& pos) {
    float fy = -(pos.y - 17066.666f);
    float fx = -(pos.x - 17066.666f);

    if (fy < 0.0f || fx < 0.0f || fy >= 34133.332f || fx >= 34133.332f) {
        return false;
    }

    CMapAreaTableEntry* entry = this->m_areaTable.Get((static_cast<int32_t>(fy * 0.029999999f) >> 4) & 0x3F, (static_cast<int32_t>(fx * 0.029999999f) >> 4) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2)) {
        return false;
    }

    CMapArea* area = entry->m_area;

    if (!area) {
        return false;
    }

    CMapChunk* chunk = area->m_chunks[(static_cast<int32_t>(fy * 0.029999999f) & 0xF) + 16 * (static_cast<int32_t>(fx * 0.029999999f) & 0xF)];

    if (!chunk) {
        return false;
    }

    const uint8_t* shadow = chunk->m_shadowMap;

    if (!shadow) {
        return false;
    }

    SMAreaHeader* header = area->m_header;
    int32_t sx = static_cast<int32_t>(fy * 1.92f) & 0x3F;
    int32_t sy = static_cast<int32_t>(fx * 1.92f) & 0x3F;

    if (header) {
        uint32_t shift = header->m_mamp;

        if (shift) {
            sx >>= shift;
            sy >>= shift;
        }
    }

    return (static_cast<uint8_t>(1 << (sx & 7)) & shadow[(sx >> 3) + 8 * sy]) != 0;
}

// OFFSET: 0x007891F0/0x140478E90
int32_t CMap::QueryLiquidTerrain(const C3Vector& pos, uint32_t& liquidType, float& height) {
    float fx = -(pos.x - 17066.666f) * 0.23999999f;
    float fy = -(pos.y - 17066.666f) * 0.23999999f;
    int32_t x = static_cast<int32_t>(fx);
    int32_t y = static_cast<int32_t>(fy);
    CMapAreaTableEntry* entry = this->m_areaTable.Get((y >> 7) & 0x3F, (x >> 7) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        return 0;
    }

    CMapChunk* chunk = entry->m_area->m_chunks[((y >> 3) & 0xF) + 16 * ((x >> 3) & 0xF)];

    if (!chunk) {
        return 0;
    }

    uint32_t objType = 0;
    float objHeight = -FLT_MAX;
    int32_t found = 0;

    for (CMapBaseObjLink* link = chunk->m_mapObjDefLinkList.Head(), * next; link; link = next) {
        next = chunk->m_mapObjDefLinkList.Next(link);

        if (CMap::QueryLiquidMapObjDef(static_cast<CMapObjDef*>(link->m_owner), pos, 0, objType, objHeight)) {
            found = 1;
            break;
        }
    }

    float liquidHeight = -FLT_MAX;
    C2Vector frac(fy - static_cast<float>(y), fx - static_cast<float>(x));
    C2iVector cell = { static_cast<int32_t>(fy) & 7, static_cast<int32_t>(fx) & 7 };

    for (CChunkLiquid* liquid = chunk->m_liquidList.Head(); liquid; liquid = chunk->m_liquidList.Next(liquid)) {
        if (liquid->TileExists(cell.x, cell.y) && liquid->GetHeight(frac, cell, liquidHeight)) {
            if (found) {
                if (objHeight <= liquidHeight) {
                    liquidType = liquid->m_liquidType;
                    height = liquidHeight;
                } else {
                    liquidType = objType;
                    height = objHeight;
                }

                return 1;
            }

            liquidType = liquid->m_liquidType;
            height = liquidHeight;
            return 1;
        }
    }

    if (!found) {
        return 0;
    }

    liquidType = objType;
    height = objHeight;
    return 1;
}

// OFFSET: 0x00789440/0x140479190
bool CMap::QueryLiquid(const C3Vector& pos, int32_t nearest, uint32_t& liquidType, float& height) {
    if (this->m_globalMapObjDef) {
        return CMap::QueryLiquidMapObjDef(this->m_globalMapObjDef, pos, nearest, liquidType, height);
    }

    float fx = -(pos.x - 17066.666f) * 0.23999999f;
    int32_t x = static_cast<int32_t>(fx);
    int32_t y = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.23999999f);
    float fy = -(pos.y - 17066.666f) * 0.23999999f;
    CMapAreaTableEntry* entry = this->m_areaTable.Get((y >> 7) & 0x3F, (x >> 7) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        return false;
    }

    CMapChunk* chunk = entry->m_area->m_chunks[((y >> 3) & 0xF) + 16 * ((x >> 3) & 0xF)];

    if (!chunk) {
        return false;
    }

    uint32_t objType = 0;
    float objHeight = -FLT_MAX;
    int32_t found = 0;

    for (CMapBaseObjLink* link = chunk->m_mapObjDefLinkList.Head(), * next; link; link = next) {
        next = chunk->m_mapObjDefLinkList.Next(link);

        if (CMap::QueryLiquidMapObjDef(static_cast<CMapObjDef*>(link->m_owner), pos, nearest, objType, objHeight)) {
            found = 1;
            break;
        }
    }

    float liquidHeight = -FLT_MAX;
    C2Vector frac(fy - static_cast<float>(y), fx - static_cast<float>(x));
    C2iVector cell = { y & 7, x & 7 };

    for (CChunkLiquid* liquid = chunk->m_liquidList.Head(); liquid; liquid = chunk->m_liquidList.Next(liquid)) {
        if (liquid->TileExists(cell.x, cell.y) && liquid->GetHeight(frac, cell, liquidHeight)) {
            if (found) {
                if (nearest) {
                    if (fabs(pos.z - liquidHeight) <= fabs(pos.z - objHeight)) {
                        liquidType = liquid->m_liquidType;
                        height = liquidHeight;
                    } else {
                        liquidType = objType;
                        height = objHeight;
                    }

                    return true;
                }

                if (objHeight <= liquidHeight) {
                    liquidType = liquid->m_liquidType;
                    height = liquidHeight;
                } else {
                    liquidType = objType;
                    height = objHeight;
                }

                return true;
            }

            liquidType = liquid->m_liquidType;
            height = liquidHeight;
            return true;
        }
    }

    if (!found) {
        return false;
    }

    liquidType = objType;
    height = objHeight;
    return true;
}

// OFFSET: 0x00789750/0x140479590
void CMap::GetTrisSubchunk(CMapChunk* chunk, const CiRect& rect, const CAaBox& box, World::TriData& tris) {
    uint32_t start = rect.minY + rect.minX + 16 * rect.minY;
    const float* heights = &chunk->m_vertices[start];
    int32_t cols = rect.maxX - rect.minX + 1;
    World::TriData::Batch* batch = NULL;
    C3Vector* verts = &CMapChunk::vertexListXY[start];
    uint16_t* indices = NULL;
    int32_t started = 0;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++, heights++, verts++) {
            if (chunk->m_holes & s_holeMasks[4 * (y >> 1) + (x >> 1)]) {
                continue;
            }

            uint8_t clip[19];

            for (uint32_t i = 0; i < 5; i++) {
                uint32_t offset = s_subchunkVerts[i];
                verts[offset].z = heights[offset];
                clip[offset] = BoxClipCode(verts[offset], box);
            }

            uint16_t vertex = static_cast<uint16_t>(verts - CMapChunk::vertexListXY);

            for (uint32_t i = 0; i < 4; i++) {
                const uint32_t* tri = s_subchunkTris[i];

                if (clip[tri[2]] & clip[tri[0]] & clip[tri[1]]) {
                    continue;
                }

                if (!started) {
                    batch = World::TriData::AllocBatch();

                    if (batch) {
                        batch->m_def = chunk;
                        C44Matrix* matrix = World::TriData::AllocMatrix();
                        *matrix = C44Matrix();
                        matrix->d0 = chunk->m_position.x;
                        matrix->d1 = chunk->m_position.y;
                        matrix->d2 = chunk->m_position.z;
                        batch->m_matrix = matrix;
                        batch->m_vertexList = CMapChunk::vertexListXY;
                        batch->m_heightList = const_cast<float*>(chunk->m_vertices);
                        batch->m_packedNormals = chunk->m_normals;
                        indices = World::TriData::AllocIndices(12 * (rect.maxX - rect.minX + 1) * (rect.maxY - rect.minY + 1));
                        batch->m_indices = indices;
                    }
                }

                started = 1;

                if (!batch || !batch->m_indices || !batch->m_vertexList) {
                    continue;
                }

                for (uint32_t j = 0; j < 3; j++) {
                    uint16_t index = vertex + static_cast<uint16_t>(tri[j]);
                    indices[j] = index;
                    batch->m_minVertexIndex = batch->m_minVertexIndex >= index ? index : batch->m_minVertexIndex;
                    batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                }

                batch->m_indexCount += 3;
                batch->m_faceCount++;
                indices += 3;
            }
        }

        heights += 17 - cols;
        verts += 17 - cols;
    }
}

// OFFSET: 0x00789F00/0x140479EE0
void CMap::GetTrisChunk(CMapChunk* chunk, const CAaBox& box, const CiRect& rect, World::TriData& tris, uint32_t flags) {
    float nx = -chunk->m_position.x;
    float ny = -chunk->m_position.y;
    float nz = -chunk->m_position.z;
    CAaBox local;
    local.b.x = box.b.x + nx;
    local.b.y = box.b.y + ny;
    local.b.z = box.b.z + nz;
    local.t.x = box.t.x + nx;
    local.t.y = box.t.y + ny;
    local.t.z = box.t.z + nz;

    if (flags & 0x100) {
        CMap::GetTrisSubchunk(chunk, rect, local, tris);
    }

    if (flags & 0x300F0) {
        CAaBox objBox;
        memset(&objBox, 0, sizeof(objBox));

        for (CMapBaseObjLink* link = chunk->m_mapObjDefLinkList.Head(), * next; link; link = next) {
            next = chunk->m_mapObjDefLinkList.Next(link);
            CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

            if (mapObjDef->unk_004C == CWorldMap::s_queryTag) {
                continue;
            }

            mapObjDef->unk_004C = CWorldMap::s_queryTag;

            if (!(mapObjDef->m_flags & 0x40)) {
                continue;
            }

            CWorldMath::TransformAABox(mapObjDef->m_invMat, box, objBox);

            if (mapObjDef->m_mapObj->TestBounds(objBox)) {
                mapObjDef->m_mapObj->GetTris(objBox, flags, tris, mapObjDef);
            }
        }
    }

    if (flags & 0x30000) {
        int32_t requireFlag = (flags & 0x10000) && !(flags & 0x20000) ? 1 : 0;

        for (CChunkLiquid* liquid = chunk->m_liquidList.Head(); liquid; liquid = chunk->m_liquidList.Next(liquid)) {
            if (!requireFlag || (g_liquidTypeDB.GetRecord(liquid->m_liquidType)->m_flags & 0x4)) {
                liquid->GetTris(tris, local, rect);
            }
        }
    }
}

// OFFSET: 0x0078A130
void CMap::GetTrisTerrain(const CAaBox& box, const CiRect& chunkRect, const CiRect& subChunkRect, World::TriData& tris, uint32_t flags) {
    int32_t y = chunkRect.minY;
    CMapAreaTableEntry* entry = this->m_areaTable.Get(chunkRect.minX >> 4, y >> 4);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        return;
    }

    CMapArea* area = entry->m_area;
    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = 0;
    rect.maxX = 0;

    for (; y <= chunkRect.maxY; y++) {
        rect.minY = (8 * y <= subChunkRect.minY ? subChunkRect.minY : 8 * y) - 8 * y;
        rect.maxY = (8 * y + 7 < subChunkRect.maxY ? 8 * y + 7 : subChunkRect.maxY) - 8 * y;

        for (int32_t x = chunkRect.minX; x <= chunkRect.maxX; x++) {
            CMapChunk* chunk = area->m_chunks[16 * (y & 0xF) + (x & 0xF)];

            if (!chunk) {
                continue;
            }

            rect.maxX = (8 * x + 7 < subChunkRect.maxX ? 8 * x + 7 : subChunkRect.maxX) - 8 * x;
            rect.minX = (8 * x > subChunkRect.minX ? 8 * x : subChunkRect.minX) - 8 * x;
            CMap::GetTrisChunk(chunk, box, rect, tris, flags);
        }
    }
}

// OFFSET: 0x0078A250/0x14047A390
bool CMapChunk::Intersect(int32_t subX, int32_t subY, C3Ray& ray, float& t) {
    bool hit = false;

    if (this->m_holes & s_holeMasks[4 * (subY >> 1) + (subX >> 1)]) {
        return false;
    }

    uint32_t vertex = subY + subX + 16 * subY;
    CMapChunk::vertexListXY[vertex].z = this->m_vertices[vertex];
    CMapChunk::vertexListXY[vertex + 1].z = this->m_vertices[vertex + 1];
    CMapChunk::vertexListXY[vertex + 9].z = this->m_vertices[vertex + 9];
    CMapChunk::vertexListXY[vertex + 17].z = this->m_vertices[vertex + 17];
    CMapChunk::vertexListXY[vertex + 18].z = this->m_vertices[vertex + 18];

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t indices[3];
        indices[1] = vertex + s_fanHeights[i][1];
        indices[2] = vertex + s_fanHeights[i][0];
        indices[0] = vertex + 9;
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

// OFFSET: 0x0078A380/0x14047A540
bool CMapChunk::Intersect(int32_t subX, int32_t subY, C3Ray& ray, C4Plane& plane, float& t) {
    bool hit = false;

    if (this->m_holes & s_holeMasks[4 * (subY >> 1) + (subX >> 1)]) {
        return false;
    }

    uint32_t vertex = subY + subX + 16 * subY;

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t indices[3];
        indices[1] = vertex + s_fanHeights[i][0];
        indices[2] = vertex + s_fanHeights[i][1];
        indices[0] = vertex + 9;
        CMapChunk::vertexListXY[vertex + 9].z = this->m_vertices[vertex + 9];
        CMapChunk::vertexListXY[indices[1]].z = this->m_vertices[indices[1]];
        CMapChunk::vertexListXY[indices[2]].z = this->m_vertices[indices[2]];
        float hitT = 0.0f;

        if (NTempest::Intersect(ray, CMapChunk::vertexListXY, indices, &hitT, NULL, 0.0f) && t > hitT && hitT >= 0.0f) {
            t = hitT;
            hit = true;
            plane.From3Pos(CMapChunk::vertexListXY[indices[0]], CMapChunk::vertexListXY[indices[2]], CMapChunk::vertexListXY[indices[1]]);
        }
    }

    return hit;
}

// OFFSET: 0x0078A4D0
void CMapChunk::Intersect(const CiRect& rect, CAaBox& box, World::FacetData& facets) {
    uint32_t start = rect.minY + rect.minX + 16 * rect.minY;
    const float* heights = &this->m_vertices[start];
    C3Vector* verts = &CMapChunk::vertexListXY[start];
    int32_t cols = rect.maxX - rect.minX + 1;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++, heights++, verts++) {
            if (this->m_holes & s_holeMasks[4 * (y >> 1) + (x >> 1)]) {
                continue;
            }

            uint8_t clip[19];

            for (uint32_t i = 0; i < 5; i++) {
                uint32_t offset = s_subchunkVerts[i];
                verts[offset].z = heights[offset];
                clip[offset] = BoxClipCode(verts[offset], box);
            }

            for (uint32_t i = 0; i < 4; i++) {
                const uint32_t* tri = s_subchunkTris[i];

                if (clip[tri[2]] & clip[tri[1]] & clip[tri[0]]) {
                    if (World::GetEnables() & 0x4000000) {
                        CFacet facet(0.0f);
                        C3Vector v0 = verts[tri[0]] + this->m_position;
                        C3Vector v1 = verts[tri[1]] + this->m_position;
                        C3Vector v2 = verts[tri[2]] + this->m_position;
                        facet.v[0] = v0;
                        facet.v[1] = v1;
                        facet.v[2] = v2;
                        float ay = v1.y - v0.y;
                        float bz = v2.z - v0.z;
                        float cz = (v2.y - v0.y) * (v1.x - v0.x) - ay * (v2.x - v0.x);
                        float cy = (v1.z - v0.z) * (v2.x - v0.x) - bz * (v1.x - v0.x);
                        float cx = ay * bz - (v1.z - v0.z) * (v2.y - v0.y);
                        float inv = 1.0f / sqrt(cz * cz + cy * cy + cx * cx);
                        facet.plane.n.y = cy * inv;
                        facet.plane.n.x = cx * inv;
                        facet.plane.n.z = cz * inv;
                        facet.plane.d = -((cy * inv) * v0.y + (cx * inv) * v0.x + (cz * inv) * v0.z);
                        CMap::TestQueryAdd(facet, CImVector(0x81010000), NULL);
                    }

                    continue;
                }

                CFacet* facet = facets.facets.New();
                facet->v[0] = verts[tri[0]] + this->m_position;
                facet->v[1] = verts[tri[1]] + this->m_position;
                facet->v[2] = verts[tri[2]] + this->m_position;
                facet->plane.From3Pos(facet->v[0], facet->v[1], facet->v[2]);
                facets.facetIds.New(0);
                facets.facetFlags.New(2);

                if (World::GetEnables() & 0x4000000) {
                    CMap::TestQueryAdd(*facet, CImVector(0x8000FF00), NULL);
                }
            }
        }

        heights += 17 - cols;
        verts += 17 - cols;
    }
}

// OFFSET: 0x0078B070/0x14047B4E0
void CMapChunk::Intersect(const CiRect& rect, const CFrustum& frustum, World::FacetData& facets) {
    uint32_t start = rect.minY + rect.minX + 16 * rect.minY;
    const float* heights = &this->m_vertices[start];
    C3Vector* verts = &CMapChunk::vertexListXY[start];
    int32_t cols = rect.maxX - rect.minX + 1;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++, heights++, verts++) {
            if (this->m_holes & s_holeMasks[4 * (y >> 1) + (x >> 1)]) {
                continue;
            }

            uint8_t clip[19];

            for (uint32_t i = 0; i < 5; i++) {
                uint32_t offset = s_subchunkVerts[i];
                verts[offset].z = heights[offset];
                frustum.Cull(verts[offset], clip[offset]);
            }

            for (uint32_t i = 0; i < 4; i++) {
                const uint32_t* tri = s_subchunkTris[i];

                if (clip[tri[0]] & clip[tri[2]] & clip[tri[1]]) {
                    continue;
                }

                CFacet* facet = facets.facets.New();
                facet->v[0] = verts[tri[0]] + this->m_position;
                facet->v[1] = verts[tri[1]] + this->m_position;
                facet->v[2] = verts[tri[2]] + this->m_position;
                facet->plane.From3Pos(facet->v[0], facet->v[1], facet->v[2]);
            }
        }

        heights += 17 - cols;
        verts += 17 - cols;
    }
}

// OFFSET: 0x0078B410
void CMap::GetChunkLiquidFacets(CChunkLiquid* liquid, const CAaBox& box, const CiRect& rect, World::FacetData& facets) {
    World::TriData::s_statusFlags = 0;
    World::TriData::s_nBatches = 0;
    World::TriData::s_faceIndexCursor = 0;
    World::TriData::s_indexCursor = 0;
    World::TriData::s_nMatrices = 0;
    World::TriData tris;

    if (liquid->GetTris(tris, box, rect) && World::TriData::s_nBatches > 0) {
        World::TriDataToFacetData(tris, facets, 0, 6);
    }
}

// OFFSET: 0x0078B470
static void CreateQuadFacets(const C3Vector& edge, const C3Vector& normal, const C3Vector& origin, World::FacetData& facets, const C3Vector& up, uint8_t flags) {
    CFacet* facet = facets.facets.New();
    facet->plane.n = normal;
    facet->plane.d = -(normal.z * origin.z + origin.x * normal.x + normal.y * origin.y);
    facet->v[0] = origin;
    facet->v[1] = origin + edge;
    facet->v[2] = up + (origin + edge);
    facets.facetIds.New(0);
    facets.facetFlags.New(flags);

    facet = facets.facets.New();
    facet->plane.n = normal;
    facet->plane.d = -(normal.z * origin.z + origin.x * normal.x + normal.y * origin.y);
    facet->v[0] = origin;
    facet->v[1] = up + (origin + edge);
    facet->v[2] = origin + up;
    facets.facetIds.New(0);
    facets.facetFlags.New(flags);
}

// OFFSET: 0x0078B6C0/0x14047BC50
static void CollectDoodadDefFacets(CMapDoodadDef* doodadDef, World::FacetData& facets, const CAaBox& box, uint64_t guid) {
    uint32_t oldCount = facets.facets.Count();
    doodadDef->m_model->GetCollisionFacets(box, doodadDef->m_model->m_worldTransform, facets.facets);
    uint32_t count = facets.facets.Count();
    facets.facetIds.SetCount(count);
    facets.facetFlags.SetCount(count);

    for (uint32_t i = oldCount; i < count; i++) {
        facets.facetIds.m_data[i] = guid;
        facets.facetFlags.m_data[i] = 5;
    }
}

// OFFSET: 0x0078B790
static void CollectEntityFacets(World::ObjectCollisionData& data, World::FacetData& facets, const CAaBox& box, uint64_t guid) {
    uint32_t oldCount = facets.facets.Count();
    data.m_model->GetCollisionFacets(box, data.m_matrix, facets.facets);
    uint32_t count = facets.facets.Count();
    facets.facetIds.SetCount(count);
    facets.facetFlags.SetCount(count);

    for (uint32_t i = oldCount; i < count; i++) {
        facets.facetIds.m_data[i] = guid;
        facets.facetFlags.m_data[i] = 4;
    }
}

// OFFSET: 0x0078B850/0x14047BCF0
bool CMap::GetDoodadDefFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags, uint64_t guid) {
    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        next = links.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->unk_0020 == CWorldMap::s_queryTag) {
            continue;
        }

        doodadDef->unk_0020 = CWorldMap::s_queryTag;
        uint32_t doodadFlags = doodadDef->m_flags;

        if (doodadFlags & 0x80) {
            continue;
        }

        if (doodadFlags & 0x40) {
            if (box2.t.x < doodadDef->m_modelBox.b.x || box2.t.y < doodadDef->m_modelBox.b.y || box2.t.z < doodadDef->m_modelBox.b.z || doodadDef->m_modelBox.t.x < box2.b.x || doodadDef->m_modelBox.t.y < box2.b.y || doodadDef->m_modelBox.t.z < box2.b.z) {
                continue;
            }

            CollectDoodadDefFacets(doodadDef, facets, box2, guid);
        } else if (static_cast<int32_t>(flags) >= 0) {
            if (box.t.x > doodadDef->m_modelBox.b.x && box.t.y > doodadDef->m_modelBox.b.y && box.t.z > doodadDef->m_modelBox.b.z && doodadDef->m_modelBox.t.x > box.b.x && doodadDef->m_modelBox.t.y > box.b.y && doodadDef->m_modelBox.t.z > box.b.z) {
                doodadDef->m_entityFlags |= 0x8000;
                facets.m_status |= 0x2;
                return false;
            }
        } else {
            CAaBox blocker;
            blocker.b.x = doodadDef->m_modelBox.b.x;
            blocker.b.y = doodadDef->m_modelBox.b.y;
            blocker.b.z = doodadDef->m_modelBox.b.z - 0.5f;
            blocker.t.x = doodadDef->m_modelBox.t.x;
            blocker.t.y = doodadDef->m_modelBox.t.y;
            blocker.t.z = doodadDef->m_modelBox.t.z + 1.0f;

            if (box2.t.x < blocker.b.x || box2.t.y < blocker.b.y || box2.t.z < blocker.b.z || blocker.t.x < box2.b.x || blocker.t.y < box2.b.y || blocker.t.z < box2.b.z) {
                continue;
            }

            doodadDef->m_entityFlags |= 0x8000;
            World::AddAaBoxFacets(blocker, guid, facets, 1);
        }
    }

    return true;
}

// OFFSET: 0x0078BAB0/0x14047BFC0
bool CMap::GetEntityFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags) {
    if (!World::s_entityRenderCallback) {
        return true;
    }

    CAaBox bounds;
    memset(&bounds, 0, sizeof(bounds));
    World::ObjectCollisionData data;
    memset(&data.m_bounds, 0, sizeof(data.m_bounds));
    data.m_matrix = C44Matrix();

    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        CMapEntity* entity = static_cast<CMapEntity*>(link->m_owner);
        next = links.Next(link);
        uint32_t entityFlags = entity->m_flags;

        if (entityFlags & 0x80) {
            return true;
        }

        if ((entityFlags & 0x10000) || entity->unk_0020 == CWorldMap::s_queryTag) {
            continue;
        }

        entity->unk_0020 = CWorldMap::s_queryTag;

        if (box2.t.x < entity->m_bbox.b.x || box2.t.y < entity->m_bbox.b.y || box2.t.z < entity->m_bbox.b.z || entity->m_bbox.t.x < box2.b.x || entity->m_bbox.t.y < box2.b.y || entity->m_bbox.t.z < box2.b.z) {
            continue;
        }

        WOWGUID guid = WOWGUID::Make(entity->m_param64);

        if (!reinterpret_cast<World::OBJECT_COLLISION_CALLBACK>(World::s_entityRenderCallback)(&guid, entity->m_param32, &data, flags)) {
            continue;
        }

        if (!data.m_model) {
            CM2Model* model = entity->m_model;

            if (model) {
                data.m_model = model;

                if (model->IsLoaded()) {
                    if (!(model->m_shared->m_flags & 0x1)) {
                        model->WaitForLoad(NULL);
                    }

                    bounds = model->m_shared->m_data->m_collisionBounds.m_extent;
                } else {
                    bounds = model->m_shared->m_boundingBox;
                }

                if (!bounds.IsNonDegenerate()) {
                    continue;
                }

                data.m_bounds = CAaBox::Transform(bounds, data.m_matrix);
            }
        }

        if (data.m_model->IsLoaded()) {
            if (box2.Intersects(data.m_bounds)) {
                CollectEntityFacets(data, facets, box2, guid.m_guid);
            }

            continue;
        }

        if (box.IntersectsExclusive(data.m_bounds)) {
            facets.m_status |= 0x2;
            return false;
        }

        if (static_cast<int32_t>(flags) < 0 && box2.Intersects(data.m_bounds)) {
            World::AddAaBoxFacets(data.m_bounds, guid.m_guid, facets, 1);
        }
    }

    return true;
}

// OFFSET: 0x0078BDE0
bool CMap::GetMapObjDefFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags) {
    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        next = links.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->unk_004C == CWorldMap::s_queryTag) {
            continue;
        }

        uint32_t loaded = mapObjDef->m_flags & 0x40;
        mapObjDef->unk_004C = CWorldMap::s_queryTag;

        if (!loaded) {
            CAaBox blocker;
            blocker.b.x = mapObjDef->m_bbox.b.x;
            blocker.b.y = mapObjDef->m_bbox.b.y;
            blocker.b.z = mapObjDef->m_bbox.b.z - 0.5f;
            blocker.t.x = mapObjDef->m_bbox.t.x;
            blocker.t.y = mapObjDef->m_bbox.t.y;
            blocker.t.z = mapObjDef->m_bbox.t.z + 1.0f;

            if (box2.t.x >= blocker.b.x && box2.t.y >= blocker.b.y && box2.t.z >= blocker.b.z && blocker.t.x >= box2.b.x && blocker.t.y >= box2.b.y && blocker.t.z >= box2.b.z) {
                if (box.t.x > mapObjDef->m_bbox.b.x && box.t.y > mapObjDef->m_bbox.b.y && box.t.z > mapObjDef->m_bbox.b.z && mapObjDef->m_bbox.t.x > box.b.x && mapObjDef->m_bbox.t.y > box.b.y && mapObjDef->m_bbox.t.z > box.b.z) {
                    facets.m_status |= 0x2;
                    return false;
                }

                if (static_cast<int32_t>(flags) < 0) {
                    World::AddAaBoxFacets(blocker, 0, facets, 1);
                }
            }

            continue;
        }

        if (box2.t.x < mapObjDef->m_bbox.b.x || box2.t.y < mapObjDef->m_bbox.b.y || box2.t.z < mapObjDef->m_bbox.b.z || mapObjDef->m_bbox.t.x < box2.b.x || mapObjDef->m_bbox.t.y < box2.b.y || mapObjDef->m_bbox.t.z < box2.b.z) {
            continue;
        }

        if (!mapObjDef->GetFacets(box, box2, facets, flags)) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x0078BFD0
void CMap::CreateImpassableFacets(CMapChunk* chunk, const CAaBox& box, World::FacetData& facets, uint32_t flags) {
    C3Vector up(0.0f, 0.0f, 32000.0f);
    C3Vector origin(0.0f, 0.0f, 0.0f);
    C3Vector normal(0.0f, 0.0f, 0.0f);
    C3Vector edge(0.0f, 0.0f, 0.0f);

    if (chunk->m_bbox.b.y > box.b.y) {
        origin = C3Vector(chunk->m_bbox.b.x, chunk->m_bbox.b.y, chunk->m_bbox.b.z);
        edge = C3Vector(chunk->m_bbox.t.x - chunk->m_bbox.b.x, 0.0f, 0.0f);
        normal = C3Vector(0.0f, -1.0f, 0.0f);
        CreateQuadFacets(edge, normal, origin, facets, up, 3);
    }

    if (box.t.y > chunk->m_bbox.t.y) {
        origin = C3Vector(chunk->m_bbox.t.x, chunk->m_bbox.t.y, chunk->m_bbox.b.z);
        edge = C3Vector(chunk->m_bbox.b.x - chunk->m_bbox.t.x, 0.0f, 0.0f);
        normal = C3Vector(0.0f, 1.0f, 0.0f);
        CreateQuadFacets(edge, normal, origin, facets, up, 3);
    }

    if (chunk->m_bbox.b.x > box.b.x) {
        origin = C3Vector(chunk->m_bbox.b.x, chunk->m_bbox.t.y, chunk->m_bbox.b.z);
        edge = C3Vector(0.0f, chunk->m_bbox.b.y - chunk->m_bbox.t.y, 0.0f);
        normal = C3Vector(-1.0f, 0.0f, 0.0f);
        CreateQuadFacets(edge, normal, origin, facets, up, 3);
    }

    if (box.t.x > chunk->m_bbox.t.x) {
        origin = C3Vector(chunk->m_bbox.t.x, chunk->m_bbox.b.y, chunk->m_bbox.b.z);
        edge = C3Vector(0.0f, chunk->m_bbox.t.y - chunk->m_bbox.b.y, 0.0f);
        normal = C3Vector(1.0f, 0.0f, 0.0f);
        CreateQuadFacets(edge, normal, origin, facets, up, 3);
    }
}

// OFFSET: 0x0078C210
bool CMap::CreateFlightBoundsFacets(int32_t x, int32_t y, const CAaBox& box, World::FacetData& facets, uint32_t flags) {
    CMapAreaTableEntry* entry = this->m_areaTable.Get(x, y);

    if (!entry || !(entry->m_flags & 0x2)) {
        return false;
    }

    CMapArea* area = entry->m_area;

    if (!area) {
        return false;
    }

    const int16_t* bounds = static_cast<const int16_t*>(area->m_flightBounds);

    if (!bounds) {
        return true;
    }

    uint32_t oldCount = facets.facets.Count();

    if (flags & 0x200) {
        int32_t top = static_cast<int32_t>(box.t.z + 1.0f);
        bool below[9];

        for (uint32_t i = 0; i < 9; i++) {
            below[i] = top < bounds[i];
        }

        for (uint32_t tri = 0; tri < 8; tri++) {
            uint32_t i0 = s_flightBoundsTris[tri][0];
            uint32_t i1 = s_flightBoundsTris[tri][1];
            uint32_t i2 = s_flightBoundsTris[tri][2];

            if (below[i0] && below[i1] && below[i2]) {
                continue;
            }

            const int16_t* heights = static_cast<const int16_t*>(area->m_flightBounds);
            CFacet* facet = facets.facets.New();
            facet->v[0] = C3Vector(s_flightBoundsVerts[i0].x + area->m_posX, s_flightBoundsVerts[i0].y + area->m_posY, static_cast<float>(heights[i0]));
            facet->v[1] = C3Vector(s_flightBoundsVerts[i1].x + area->m_posX, s_flightBoundsVerts[i1].y + area->m_posY, static_cast<float>(heights[i1]));
            facet->v[2] = C3Vector(s_flightBoundsVerts[i2].x + area->m_posX, s_flightBoundsVerts[i2].y + area->m_posY, static_cast<float>(heights[i2]));
            facet->plane.From3Pos(facet->v[0], facet->v[1], facet->v[2]);
            facets.facetFlags.New(8);
        }
    }

    if (flags & 0x400) {
        int32_t bottom = static_cast<int32_t>(box.b.z - 1.0f);
        const int16_t* lower = &static_cast<const int16_t*>(area->m_flightBounds)[9];
        bool above[9];

        for (uint32_t i = 0; i < 9; i++) {
            above[i] = bottom > lower[i];
        }

        for (uint32_t tri = 0; tri < 8; tri++) {
            uint32_t i0 = s_flightBoundsTris[tri][0];
            uint32_t i1 = s_flightBoundsTris[tri][1];
            uint32_t i2 = s_flightBoundsTris[tri][2];

            if (above[i0] && above[i1] && above[i2]) {
                continue;
            }

            const int16_t* heights = &static_cast<const int16_t*>(area->m_flightBounds)[9];
            CFacet* facet = facets.facets.New();
            facet->v[0] = C3Vector(s_flightBoundsVerts[i0].x + area->m_posX, s_flightBoundsVerts[i0].y + area->m_posY, static_cast<float>(heights[i0]));
            facet->v[1] = C3Vector(s_flightBoundsVerts[i2].x + area->m_posX, s_flightBoundsVerts[i2].y + area->m_posY, static_cast<float>(heights[i2]));
            facet->v[2] = C3Vector(s_flightBoundsVerts[i1].x + area->m_posX, s_flightBoundsVerts[i1].y + area->m_posY, static_cast<float>(heights[i1]));
            facet->plane.From3Pos(facet->v[0], facet->v[1], facet->v[2]);
            facets.facetFlags.New(7);
        }
    }

    uint32_t count = facets.facets.Count();
    facets.facetIds.SetCount(count);

    if (count - oldCount) {
        memset(&facets.facetIds.m_data[oldCount], 0, sizeof(uint64_t) * (count - oldCount));
    }

    return true;
}

// OFFSET: 0x0078C940/0x14047D2C0
void CMap::GetMapObjDefFacets(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const CFrustum& frustum, World::FacetData& facets, uint32_t flags) {
    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        next = links.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->unk_004C == CWorldMap::s_queryTag) {
            continue;
        }

        uint32_t skip = mapObjDef->m_flags & 0x90;
        mapObjDef->unk_004C = CWorldMap::s_queryTag;

        if (!skip && frustum.Cull(mapObjDef->m_bbox)) {
            mapObjDef->GetFacets(frustum, facets, flags);
        }
    }
}

// OFFSET: 0x0078CD20/0x14047D750
bool CMap::GetChunkFacets(CMapChunk* chunk, const CAaBox& box, const CAaBox& box2, const CiRect& rect, World::FacetData& facets, uint32_t flags) {
    if (chunk->m_flags & 0x20) {
        CMap::CreateImpassableFacets(chunk, box2, facets, flags);
    }

    float nx = -chunk->m_position.x;
    float ny = -chunk->m_position.y;
    float nz = -chunk->m_position.z;
    CAaBox local;
    local.b.x = box2.b.x + nx;
    local.b.y = box2.b.y + ny;
    local.b.z = box2.b.z + nz;
    local.t.x = box2.t.x + nx;
    local.t.y = box2.t.y + ny;
    local.t.z = box2.t.z + nz;

    if ((flags & 0xF00000) && !CMap::GetEntityFacets(chunk->m_entityLinkList, box, box2, facets, flags)) {
        return false;
    }

    if ((flags & 0x300F0) && !CMap::GetMapObjDefFacets(chunk->m_mapObjDefLinkList, box, box2, facets, flags)) {
        return false;
    }

    if (chunk->m_bbox.t.z >= box2.b.z) {
        if (flags & 0x100) {
            chunk->Intersect(rect, local, facets);
        }

        if ((flags & 0xF) && !CMap::GetDoodadDefFacets(chunk->m_doodadDefLinkList, box, box2, facets, flags, 0)) {
            return false;
        }
    }

    if (chunk->m_liquidBox.t.z >= box2.b.z && (flags & 0x30000)) {
        int32_t requireFlag = (flags & 0x10000) && !(flags & 0x20000) ? 1 : 0;

        for (CChunkLiquid* liquid = chunk->m_liquidList.Head(), * next; liquid; liquid = next) {
            next = chunk->m_liquidList.Next(liquid);

            if (!requireFlag || (g_liquidTypeDB.GetRecord(liquid->m_liquidType)->m_flags & 0x4)) {
                CMap::GetChunkLiquidFacets(liquid, local, rect, facets);
            }
        }
    }

    return true;
}

// OFFSET: 0x0078CF40/0x14047DA30
bool CMap::GetChunkFacets(CMapChunk* chunk, const CFrustum& frustum, const CAaBox& bounds, const CiRect& rect, World::FacetData& facets, uint32_t flags) {
    uint32_t oldCount = facets.facets.Count();
    CFrustum local = frustum;
    local.Translate(C3Vector(-chunk->m_position.x, -chunk->m_position.y, -chunk->m_position.z));

    if (flags & 0x300F0) {
        CMap::GetMapObjDefFacets(chunk->m_mapObjDefLinkList, frustum, facets, flags);
    }

    if (flags & 0x100) {
        chunk->Intersect(rect, local, facets);
    }

    if (flags & 0x30000) {
        bool requireFlag = (flags & 0x10000) && !(flags & 0x20000);

        for (CChunkLiquid* liquid = chunk->m_liquidList.Head(), * next; liquid; liquid = next) {
            next = chunk->m_liquidList.Next(liquid);

            if (requireFlag && !(g_liquidTypeDB.GetRecord(liquid->m_liquidType)->m_flags & 0x4)) {
                continue;
            }

            World::TriData::s_statusFlags = 0;
            World::TriData::s_nBatches = 0;
            World::TriData::s_faceIndexCursor = 0;
            World::TriData::s_indexCursor = 0;
            World::TriData::s_nMatrices = 0;
            World::TriData tris;

            if (liquid->GetTris(tris, local, rect) && World::TriData::s_nBatches) {
                World::TriDataToFacetData(tris, facets, 0, 6);
            }
        }
    }

    if (flags & 0xF) {
        for (CMapBaseObjLink* link = chunk->m_doodadDefLinkList.Head(), * next; link; link = next) {
            next = chunk->m_doodadDefLinkList.Next(link);
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

            if (doodadDef->unk_0020 == CWorldMap::s_queryTag) {
                continue;
            }

            uint32_t loaded = doodadDef->m_flags & 0x40;
            doodadDef->unk_0020 = CWorldMap::s_queryTag;

            if (loaded && bounds.t.x >= doodadDef->m_modelBox.b.x && bounds.t.y >= doodadDef->m_modelBox.b.y && bounds.t.z >= doodadDef->m_modelBox.b.z && doodadDef->m_modelBox.t.x >= bounds.b.x && doodadDef->m_modelBox.t.y >= bounds.b.y && doodadDef->m_modelBox.t.z >= bounds.b.z) {
                doodadDef->m_model->GetCollisionFacets(bounds, doodadDef->m_model->m_worldTransform, facets.facets);
            }
        }
    }

    return oldCount != facets.facets.Count();
}

// OFFSET: 0x0078D1C0/0x14047DD00
bool CMap::GetFacets(const CAaBox& box, const CAaBox& box2, const CiRect& chunkRect, const CiRect& subChunkRect, World::FacetData& facets, uint32_t flags) {
    int32_t areaY = chunkRect.minY >> 4;
    int32_t areaX = chunkRect.minX >> 4;
    CMapAreaTableEntry* entry = this->m_areaTable.Get(areaX, areaY);

    if (entry && (entry->m_flags & 0x2) && entry->m_area) {
        CMapArea* area = entry->m_area;
        CiRect rect;
        rect.minY = 0;
        rect.minX = 0;
        rect.maxY = 0;
        rect.maxX = 0;

        for (int32_t y = chunkRect.minY; y <= chunkRect.maxY; y++) {
            rect.minY = (8 * y > subChunkRect.minY ? 8 * y : subChunkRect.minY) - 8 * y;
            rect.maxY = (8 * y + 7 < subChunkRect.maxY ? 8 * y + 7 : subChunkRect.maxY) - 8 * y;

            for (int32_t x = chunkRect.minX; x <= chunkRect.maxX; x++) {
                CMapChunk* chunk = area->m_chunks[16 * (y & 0xF) + (x & 0xF)];

                if (!chunk) {
                    return false;
                }

                rect.maxX = (8 * x + 7 < subChunkRect.maxX ? 8 * x + 7 : subChunkRect.maxX) - 8 * x;
                rect.minX = (8 * x > subChunkRect.minX ? 8 * x : subChunkRect.minX) - 8 * x;

                if (!this->GetChunkFacets(chunk, box, box2, rect, facets, flags)) {
                    return false;
                }
            }
        }

        return true;
    }

    entry = this->m_areaTable.Get(areaX, areaY);

    if (!entry || !(entry->m_flags & 0x1)) {
        return true;
    }

    if (static_cast<int32_t>(flags) >= 0) {
        return false;
    }

    CAaBox blocker;
    blocker.b.x = (static_cast<float>(chunkRect.maxY) + 1.0f) * -33.333332f + 17066.666f;
    blocker.b.y = (static_cast<float>(chunkRect.maxX) + 1.0f) * -33.333332f + 17066.666f;
    blocker.b.z = box.b.z - 16.666666f;
    blocker.t.x = static_cast<float>(chunkRect.minY) * -33.333332f + 17066.666f;
    blocker.t.y = static_cast<float>(chunkRect.minX) * -33.333332f + 17066.666f;
    blocker.t.z = box.t.z + 16.666666f;

    if (box.t.x > blocker.b.x && box.t.y > blocker.b.y && box.t.z > blocker.b.z && blocker.t.x > box.b.x && blocker.t.y > box.b.y && blocker.t.z > box.b.z) {
        return false;
    }

    World::AddAaBoxFacets(blocker, 0, facets, 1);
    return true;
}

// OFFSET: 0x0078D440/0x14047E090
bool CMap::GetFacets(const CFrustum& frustum, const CAaBox& bounds, const CiRect& chunkRect, const CiRect& subChunkRect, World::FacetData& facets, uint32_t flags) {
    int32_t areaY = chunkRect.minY >> 4;
    int32_t areaX = chunkRect.minX >> 4;
    CMapAreaTableEntry* entry = this->m_areaTable.Get(areaX, areaY);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        entry = this->m_areaTable.Get(areaX, areaY);
        return !entry || !(entry->m_flags & 0x1);
    }

    CMapArea* area = entry->m_area;
    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = 0;
    rect.maxX = 0;

    for (int32_t y = chunkRect.minY; y <= chunkRect.maxY; y++) {
        rect.minY = (8 * y <= subChunkRect.minY ? subChunkRect.minY : 8 * y) - 8 * y;
        rect.maxY = (8 * y + 7 >= subChunkRect.maxY ? subChunkRect.maxY : 8 * y + 7) - 8 * y;

        for (int32_t x = chunkRect.minX; x <= chunkRect.maxX; x++) {
            CMapChunk* chunk = area->m_chunks[16 * (y & 0xF) + (x & 0xF)];

            if (!chunk) {
                return false;
            }

            rect.maxX = (8 * x + 7 < subChunkRect.maxX ? 8 * x + 7 : subChunkRect.maxX) - 8 * x;
            rect.minX = (8 * x > subChunkRect.minX ? 8 * x : subChunkRect.minX) - 8 * x;
            CMap::GetChunkFacets(chunk, frustum, bounds, rect, facets, flags);
        }
    }

    return true;
}

// OFFSET: 0x00871FE0/0x140581DE0
void CMapChunk::Destroy() {
}

// OFFSET: 0x0073B1F0/0x140418950
void CMapChunk::InitVertexListXY() {
    C3Vector* vertex = CMapChunk::vertexListXY;

    for (int32_t i = 0; i < 9; i++) {
        float x = static_cast<float>(i) * -4.1666665f;

        for (uint32_t j = 0; j < 9; j++) {
            vertex->x = x;
            vertex->y = s_outerY[j];
            vertex++;
        }

        if (i < 8) {
            float innerX = x - 2.0833333f;

            for (uint32_t j = 0; j < 8; j++) {
                vertex->x = innerX;
                vertex->y = s_innerY[j];
                vertex++;
            }
        }
    }
}

// OFFSET: 0x0073B360/0x140418A70
int32_t CMapChunk::AreTexturesLoaded() {
    const SMLayer* layer = this->m_layers;

    for (uint32_t i = 0; i < this->m_header.m_nLayers; i++, layer++) {
        if (layer->m_flags & 0x8000) {
            continue;
        }

        CTexture* texture = this->m_area->GetTexture(layer->m_textureId);

        if (!texture) {
            texture = this->m_area->LoadTexture(layer->m_textureId);
            TextureRelease(texture);

            if (!texture) {
                continue;
            }
        }

        if (!TextureIsLoaded(texture)) {
            return 0;
        }
    }

    CTexture* shaderTexture = this->m_shaderTexture;
    return !shaderTexture || TextureIsLoaded(shaderTexture);
}

// OFFSET: 0x0073B4A0/0x140418B30
void CMapChunk::Initialize() {
    CMapRenderChunk::Initialize();
    CMapChunk::InitVertexListXY();
    CMapChunk::s_geoToTex = -1.0f / CMapChunk::vertexListXY[1].y;
}

// OFFSET: 0x0073B6D0/0x140418DF0
void CMapChunk::Batch() {
    if (this->m_chunkFlags & 0x1) {
        return;
    }

    C2iVector index;
    index.x = this->m_indexX & ~1;
    index.y = this->m_indexY & ~1;
    this->m_area->BatchChunks(index);
}

// OFFSET: 0x0073B780/0x140418EC0
void CMapChunk::CreateVerticesWorldPN(float* vertices, const C3Vector& origin) {
    CalcWorldRows(this, s_worldPNX, s_worldPNY);

    const float* heights = this->m_vertices;
    const int8_t* normals = this->m_normals;
    float* out = vertices;

    for (uint32_t i = 0; i < 9; i++) {
        for (uint32_t j = 0; j < 9; j++) {
            out[0] = s_worldPNX[i];
            out[1] = s_worldPNY[j];
            out[2] = this->m_position.z + heights[j];
            out[3] = normals[3 * j] * 0.0078740157f;
            out[4] = normals[3 * j + 1] * 0.0078740157f;
            out[5] = normals[3 * j + 2] * 0.0078740157f;
            out += 6;
        }

        heights += 9;
        normals += 27;

        if (i < 8) {
            for (uint32_t j = 0; j < 8; j++) {
                out[0] = s_worldPNX[i] - 2.0833333f;
                out[1] = s_worldPNY[j] - 2.0833333f;
                out[2] = this->m_position.z + heights[j];
                out[3] = normals[3 * j] * 0.0078740157f;
                out[4] = normals[3 * j + 1] * 0.0078740157f;
                out[5] = normals[3 * j + 2] * 0.0078740157f;
                out += 6;
            }

            heights += 8;
            normals += 24;
        }
    }
}

// OFFSET: 0x0073C050/0x140419890
void CMapChunk::CreateVerticesWorldPNC(float* vertices, const C3Vector& origin) {
    CalcWorldRows(this, s_worldPNCX, s_worldPNCY);

    const int8_t* normals = this->m_normals;
    const float* heights = this->m_vertices;
    uint8_t defaultColor[4] = { 127, 127, 127, 0xFF };
    const uint8_t* color = reinterpret_cast<const uint8_t*>(this->m_vertexColors);
    uint32_t stride = 4;

    if (!color) {
        color = defaultColor;
        stride = 0;
    }

    float* out = vertices;

    for (uint32_t i = 0; i < 9; i++) {
        for (uint32_t j = 0; j < 9; j++) {
            out[0] = s_worldPNCX[i];
            out[1] = s_worldPNCY[j];
            out[2] = this->m_position.z + heights[j];
            out[3] = normals[3 * j] * 0.0078740157f;
            out[4] = normals[3 * j + 1] * 0.0078740157f;
            out[5] = normals[3 * j + 2] * 0.0078740157f;
            *reinterpret_cast<uint32_t*>(&out[6]) = VertexColor(color);
            color += stride;
            out += 7;
        }

        heights += 9;
        normals += 27;

        if (i < 8) {
            for (uint32_t j = 0; j < 8; j++) {
                out[0] = s_worldPNCX[i] - 2.0833333f;
                out[1] = s_worldPNCY[j] - 2.0833333f;
                out[2] = this->m_position.z + heights[j];
                out[3] = normals[3 * j] * 0.0078740157f;
                out[4] = normals[3 * j + 1] * 0.0078740157f;
                out[5] = normals[3 * j + 2] * 0.0078740157f;
                *reinterpret_cast<uint32_t*>(&out[6]) = VertexColor(color);
                color += stride;
                out += 7;
            }

            heights += 8;
            normals += 24;
        }
    }
}

// OFFSET: 0x0073CD90/0x14041A740
void CMapChunk::CreateVerticesWorldPNC2(float* vertices, const C3Vector& origin) {
    CalcWorldRows(this, s_worldPNC2X, s_worldPNC2Y);

    uint8_t defaultColor[4] = { 127, 127, 127, 0xFF };
    uint8_t defaultLighting[4] = { 0, 0, 0, 0xFF };
    const uint8_t* color = reinterpret_cast<const uint8_t*>(this->m_vertexColors);
    const float* heights = this->m_vertices;
    const int8_t* normals = this->m_normals;
    uint32_t colorStride = 4;

    if (!color) {
        color = defaultColor;
        colorStride = 0;
    }

    const uint8_t* lighting = reinterpret_cast<const uint8_t*>(this->m_vertexLighting);
    uint32_t lightingStride = 4;

    if (!lighting) {
        lighting = defaultLighting;
        lightingStride = 0;
    }

    float* out = vertices;

    for (uint32_t i = 0; i < 9; i++) {
        for (uint32_t j = 0; j < 9; j++) {
            out[0] = s_worldPNC2X[i];
            out[1] = s_worldPNC2Y[j];
            out[2] = this->m_position.z + heights[j];
            out[3] = normals[3 * j] * 0.0078740157f;
            out[4] = normals[3 * j + 1] * 0.0078740157f;
            out[5] = normals[3 * j + 2] * 0.0078740157f;
            *reinterpret_cast<uint32_t*>(&out[6]) = VertexColor(color);
            *reinterpret_cast<uint32_t*>(&out[7]) = VertexColor(lighting);
            color += colorStride;
            lighting += lightingStride;
            out += 8;
        }

        heights += 9;
        normals += 27;

        if (i < 8) {
            for (uint32_t j = 0; j < 8; j++) {
                out[0] = s_worldPNC2X[i] - 2.0833333f;
                out[1] = s_worldPNC2Y[j] - 2.0833333f;
                out[2] = this->m_position.z + heights[j];
                out[3] = normals[3 * j] * 0.0078740157f;
                out[4] = normals[3 * j + 1] * 0.0078740157f;
                out[5] = normals[3 * j + 2] * 0.0078740157f;
                *reinterpret_cast<uint32_t*>(&out[6]) = VertexColor(color);
                *reinterpret_cast<uint32_t*>(&out[7]) = VertexColor(lighting);
                color += colorStride;
                lighting += lightingStride;
                out += 8;
            }

            heights += 8;
            normals += 24;
        }
    }
}

// OFFSET: 0x0073DF60/0x14041BBA0
void CMapChunk::CreateVerticesLocalPN(float* vertices) {
    float stepX;
    float stepY;
    CalcLocalSteps(this, stepX, stepY);

    float halfY = stepY * 0.5f;
    const float* heights = this->m_vertices;
    const int8_t* normals = this->m_normals;
    float* out = vertices;

    for (uint32_t i = 0; i < 9; i++) {
        float x = static_cast<float>(i) * stepX;

        for (uint32_t j = 0; j < 9; j++) {
            out[0] = x;
            out[1] = stepY * static_cast<float>(j);
            out[2] = heights[j];
            out[3] = normals[3 * j] * 0.0078740157f;
            out[4] = normals[3 * j + 1] * 0.0078740157f;
            out[5] = normals[3 * j + 2] * 0.0078740157f;
            out += 6;
        }

        heights += 9;
        normals += 27;

        if (i < 8) {
            float innerX = static_cast<float>(i) * stepY + stepX * 0.5f;

            for (uint32_t j = 0; j < 8; j++) {
                out[0] = innerX;
                out[1] = stepX * static_cast<float>(j) + halfY;
                out[2] = heights[j];
                out[3] = normals[3 * j] * 0.0078740157f;
                out[4] = normals[3 * j + 1] * 0.0078740157f;
                out[5] = normals[3 * j + 2] * 0.0078740157f;
                out += 6;
            }

            heights += 8;
            normals += 24;
        }
    }
}

// OFFSET: 0x0073E660/0x14041C3C0
void CMapChunk::CreateVerticesLocalPNC(float* vertices) {
    float stepX;
    float stepY;
    CalcLocalSteps(this, stepX, stepY);

    float halfY = stepY * 0.5f;
    const int8_t* normals = this->m_normals;
    const float* heights = this->m_vertices;
    uint8_t defaultColor[4] = { 127, 127, 127, 0xFF };
    const uint8_t* color = reinterpret_cast<const uint8_t*>(this->m_vertexColors);
    uint32_t stride = 4;

    if (!color) {
        color = defaultColor;
        stride = 0;
    }

    float* out = vertices;

    for (uint32_t i = 0; i < 9; i++) {
        float x = static_cast<float>(i) * stepX;

        for (uint32_t j = 0; j < 9; j++) {
            out[0] = x;
            out[1] = stepY * static_cast<float>(j);
            out[2] = heights[j];
            out[3] = normals[3 * j] * 0.0078740157f;
            out[4] = normals[3 * j + 1] * 0.0078740157f;
            out[5] = normals[3 * j + 2] * 0.0078740157f;
            *reinterpret_cast<uint32_t*>(&out[6]) = VertexColor(color);
            color += stride;
            out += 7;
        }

        heights += 9;
        normals += 27;

        if (i < 8) {
            float innerX = static_cast<float>(i) * stepY + stepX * 0.5f;

            for (uint32_t j = 0; j < 8; j++) {
                out[0] = innerX;
                out[1] = stepX * static_cast<float>(j) + halfY;
                out[2] = heights[j];
                out[3] = normals[3 * j] * 0.0078740157f;
                out[4] = normals[3 * j + 1] * 0.0078740157f;
                out[5] = normals[3 * j + 2] * 0.0078740157f;
                *reinterpret_cast<uint32_t*>(&out[6]) = VertexColor(color);
                color += stride;
                out += 7;
            }

            heights += 8;
            normals += 24;
        }
    }
}

// OFFSET: 0x0073F190/0x14041D250
uint32_t CMapChunk::CreateIndices(uint16_t* indices, uint16_t base, int32_t lowRes) {
    uint16_t count = 0;
    int32_t flip = this->m_map->m_header.m_flags & 0x20;
    uint16_t* out = indices;

    if (lowRes) {
        uint16_t row = base;

        for (uint32_t i = 0; i < 4; i++, row += 34) {
            for (uint32_t j = 0; j < 4; j++) {
                if (this->m_holes & s_holeMasks[4 * i + j]) {
                    continue;
                }

                uint16_t v = row + 2 * j;
                out[0] = v + 18;
                out[1] = v;
                out[2] = v + 34;
                out[3] = v + 18;
                out[4] = v + 2;
                out[5] = v;
                out[6] = v + 18;
                out[7] = v + 36;
                out[8] = v + 2;
                out[9] = v + 18;
                out[10] = v + 34;
                out[11] = v + 36;
                out += 12;
                count += 12;
            }
        }

        return count;
    }

    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            if (this->m_holes & s_holeMasks[4 * (i >> 1) + (j >> 1)]) {
                continue;
            }

            uint16_t topLeft = base + j;
            uint16_t topRight = base + j + 1;
            uint16_t center = base + j + 9;
            uint16_t bottomLeft = base + j + 17;
            uint16_t bottomRight = base + j + 18;

            if (flip) {
                out[0] = bottomLeft;
                out[1] = topLeft;
                out[2] = center;
                out[3] = topLeft;
                out[4] = topRight;
                out[5] = center;
                out[6] = topRight;
                out[7] = bottomRight;
                out[8] = center;
                out[9] = bottomRight;
                out[10] = bottomLeft;
                out[11] = center;
            } else {
                out[0] = center;
                out[1] = topLeft;
                out[2] = bottomLeft;
                out[3] = center;
                out[4] = topRight;
                out[5] = topLeft;
                out[6] = center;
                out[7] = bottomRight;
                out[8] = topRight;
                out[9] = center;
                out[10] = bottomLeft;
                out[11] = bottomRight;
            }

            out += 12;
            count += 12;
        }

        base += 17;
    }

    return count;
}

// OFFSET: 0x0073F520/0x14041D490
void CMapChunk::CreateVertices(void* vertices, int32_t index, const C3Vector& origin) {
    uint8_t* buffer = static_cast<uint8_t*>(vertices);

    if (CMap::enableTerrainShaderVertex) {
        if (CMapRenderChunk::s_vertexFormat == GxVBF_PN) {
            this->CreateVerticesWorldPN(reinterpret_cast<float*>(buffer + 24 * index), origin);
        } else if (CMapRenderChunk::s_vertexFormat == GxVBF_PNC) {
            this->CreateVerticesWorldPNC(reinterpret_cast<float*>(buffer + 28 * index), origin);
        } else {
            this->CreateVerticesWorldPNC2(reinterpret_cast<float*>(buffer + 32 * index), origin);
        }
    } else if (CMapRenderChunk::s_vertexFormat == GxVBF_PN) {
        this->CreateVerticesLocalPN(reinterpret_cast<float*>(buffer + 24 * index));
    } else {
        this->CreateVerticesLocalPNC(reinterpret_cast<float*>(buffer + 28 * index));
    }
}

// OFFSET: 0x0073F5C0/0x14041D510
void CMapChunk::CreateIndices(uint16_t* indices, uint16_t base, uint32_t& count, int32_t lowRes) {
    count = this->CreateIndices(indices, base, lowRes);
}

// OFFSET: 0x0073F5E0/0x14041D530
void CMapChunk::LinkToAsyncHead(void* param) {
    CMapChunk* chunk = static_cast<CMapChunk*>(param);
    const SMLayer* layer = chunk->m_layers;

    for (uint32_t i = 0; i < chunk->m_header.m_nLayers; i++, layer++) {
        if (!(layer->m_flags & 0x8000)) {
            CTexture* texture = chunk->m_area->GetTexture(layer->m_textureId);

            if (texture && !TextureIsLoaded(texture)) {
                TextureIncreasePriority(texture);
            }
        }

        uint32_t effectId = layer->m_effectId;

        if (effectId) {
            GroundEffectTextureRec* rec = g_groundEffectTextureDB.GetRecord(effectId);

            if (rec) {
                for (uint32_t j = 0; j < 4; j++) {
                    if (rec->m_doodadID[j]) {
                        CDetailDoodad::LinkToAsyncHead(static_cast<uint32_t>(rec->m_doodadID[j]));
                    }
                }
            }
        }
    }

    if (chunk->m_shaderTexture) {
        TextureIncreasePriority(chunk->m_shaderTexture);
    }

    for (CChunkLiquid* liquid = chunk->m_liquidList.Head(); liquid; liquid = chunk->m_liquidList.Next(liquid)) {
        Liquid::LinkToAsyncHead(liquid->m_liquidType);
    }
}
