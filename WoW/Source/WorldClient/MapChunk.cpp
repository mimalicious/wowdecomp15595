#include "WoW/Source/WorldClient/MapChunk.h"
#include <cmath>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/TerrainMaterialRec.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// STATIC: 0x00D025D0
uint32_t CMapChunk::s_vertexCount = 145;

// STATIC: 0x00D025F0
uint16_t CMapChunk::s_indexCount = 768;

// STATIC: 0x00BC7650
static uint32_t s_holeMasks[16] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

// STATIC: 0x00BC7690
static uint32_t s_fanHeights[4][2] = { { 17, 0 }, { 0, 1 }, { 18, 17 }, { 1, 18 } };

// STATIC: 0x00BC76B0
static uint32_t s_fanCorners[4][2] = { { 3, 0 }, { 0, 1 }, { 2, 3 }, { 1, 2 } };

// STATIC: 0x00D00DE0
static C3Vector s_subchunkCoords[5] = { C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, -4.1666665f, 0.0f), C3Vector(-4.1666665f, -4.1666665f, 0.0f), C3Vector(-4.1666665f, 0.0f, 0.0f), C3Vector(-2.0833333f, -2.0833333f, 0.0f) };

// STATIC: 0x00E74E28
float CMapChunk::s_geoToTex;

// STATIC: 0x00E74E38
C3Vector CMapChunk::vertexListXY[145];

// STATIC: 0x00E75504
static float s_worldPNY[9];

// STATIC: 0x00E75528
static float s_worldPNX[9];

// STATIC: 0x00E7554C
static float s_worldPNCY[9];

// STATIC: 0x00E75570
static float s_worldPNCX[9];

// STATIC: 0x00E75594
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

// OFFSET: 0x0073B4D0
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

// OFFSET: 0x0073B660
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

// OFFSET: 0x0073B700
void CMapChunk::SelectLights(CM2Lighting* lighting) {
    lighting->AddLight(&CWorldMap::s_sunLight);

    const DayNight::FogInfo* fog = DayNight::GetFogInfo();
    C3Vector color(fog->m_color.r * 0.0039215689f, fog->m_color.g * 0.0039215689f, fog->m_color.b * 0.0039215689f);
    lighting->SetFog(color, fog->m_start, fog->m_end, fog->m_density);
}

// OFFSET: 0x0073F710
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

// OFFSET: 0x0073F870
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

// OFFSET: 0x0073FC30
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

// OFFSET: 0x0073FF40
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

// OFFSET: 0x00740180
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

// OFFSET: 0x00740350
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

// OFFSET: 0x00740400
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

// OFFSET: 0x00786690
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

// OFFSET: 0x00788080
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

// OFFSET: 0x00871FE0
void CMapChunk::Destroy() {
}

// OFFSET: 0x0073B1F0
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

// OFFSET: 0x0073B360
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

// OFFSET: 0x0073B4A0
void CMapChunk::Initialize() {
    CMapRenderChunk::Initialize();
    CMapChunk::InitVertexListXY();
    CMapChunk::s_geoToTex = -1.0f / CMapChunk::vertexListXY[1].y;
}

// OFFSET: 0x0073B6D0
void CMapChunk::Batch() {
    if (this->m_chunkFlags & 0x1) {
        return;
    }

    C2iVector index;
    index.x = this->m_indexX & ~1;
    index.y = this->m_indexY & ~1;
    this->m_area->BatchChunks(index);
}

// OFFSET: 0x0073B780
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

// OFFSET: 0x0073C050
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

// OFFSET: 0x0073CD90
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

// OFFSET: 0x0073DF60
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

// OFFSET: 0x0073E660
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

// OFFSET: 0x0073F190
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

// OFFSET: 0x0073F520
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

// OFFSET: 0x0073F5C0
void CMapChunk::CreateIndices(uint16_t* indices, uint16_t base, uint32_t& count, int32_t lowRes) {
    count = this->CreateIndices(indices, base, lowRes);
}

// OFFSET: 0x0073F5E0
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
