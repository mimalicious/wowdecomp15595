#include "WoW/Source/WorldClient/MapArea.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"

struct LiquidChunkAndDistance {
    /* 0000 */ CMapChunk* m_chunk;
    /* 0004 */ CChunkLiquid* m_liquid;
    /* 0008 */ float m_distSq;

    bool operator<(const LiquidChunkAndDistance& other) const {
        return this->m_distSq < other.m_distSq;
    }
};

// STATIC: 0x00D01BEC
static uint32_t s_batchMask = 3;

// OFFSET: 0x00796F10
static int32_t CanBatchChunks(CMapChunk** chunks, uint32_t mask, int32_t multiPass) {
    if (!CMap::enableChunkBatching) {
        return 0;
    }

    CMapChunk* base = NULL;
    int32_t hasMaterial = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (!(mask & (1 << i))) {
            continue;
        }

        CMapChunk* chunk = chunks[i];

        if (!base) {
            base = chunk;
        } else if (chunk->m_materialId) {
            hasMaterial = 1;

            if (chunk->m_header.m_nLayers > base->m_header.m_nLayers) {
                base = chunk;
            }
        }
    }

    if (multiPass) {
        uint32_t layerCount = base->m_header.m_nLayers;

        for (uint32_t i = 0; i < 4; i++) {
            if (!(mask & (1 << i))) {
                continue;
            }

            CMapChunk* chunk = chunks[i];

            if (chunk == base) {
                continue;
            }

            if (chunk->m_materialId != base->m_materialId) {
                return 0;
            }

            for (uint32_t j = 0; j < chunk->m_header.m_nLayers; j++) {
                const SMLayer& layer = chunk->m_layers[j];

                if (layer.m_flags & 0x4C0) {
                    return 0;
                }

                uint32_t k = 0;

                for (; k < base->m_header.m_nLayers; k++) {
                    const SMLayer& baseLayer = base->m_layers[k];

                    if (baseLayer.m_flags & 0x4C0) {
                        return 0;
                    }

                    if (baseLayer.m_textureId == layer.m_textureId) {
                        break;
                    }
                }

                if (k < base->m_header.m_nLayers) {
                    continue;
                }

                if (hasMaterial) {
                    return 0;
                }

                layerCount++;
            }
        }

        return layerCount <= 4;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (!(mask & (1 << i))) {
            continue;
        }

        CMapChunk* chunk = chunks[i];

        if (chunk == base) {
            continue;
        }

        if (base->m_header.m_nLayers != chunk->m_header.m_nLayers) {
            return 0;
        }

        for (uint32_t j = 0; j < chunk->m_header.m_nLayers; j++) {
            const SMLayer& layer = chunk->m_layers[j];
            const SMLayer& baseLayer = base->m_layers[j];

            if ((layer.m_flags & 0xC0) || (baseLayer.m_flags & 0xC0) || baseLayer.m_textureId != layer.m_textureId) {
                return 0;
            }
        }
    }

    return 1;
}

// PROVEN: 2c619432 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CMapArea::Initialize() {
}

// PROVEN: eb5495bb IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CMapArea::Destroy() {
}

// PROVEN: 452c9edf IDENTICAL/NONE
// OFFSET: 0x00847950
void CMapArea::Enable(int32_t enable) {
}

// OFFSET: 0x007865B0/0x140475720
static void AsyncCancelCallback(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    CMap::FreeRawAreaData(buffer);
}

// OFFSET: 0x00786870/0x140475B40
void CMapArea::PurgeXXX() {
    CMap::CMapAreaTableEntry* entry = this->m_map->m_areaTable.Get(this->m_indexX, this->m_indexY);
    entry->m_flags &= ~0x2u;
    entry->m_area = NULL;

    for (uint32_t i = 0; i < 3; i++) {
        if (this->m_asyncObjects[i]) {
            if (!AsyncFileReadCancel(this->m_asyncObjects[i], AsyncCancelCallback)) {
                this->m_buffers[i] = NULL;
                this->m_bufferSizes[i] = 0;
            }

            this->m_asyncObjects[i] = NULL;
            this->m_files[i] = NULL;
        }
    }

    for (CMapChunk* chunk = this->m_chunkList.Head(), * next; chunk; chunk = next) {
        next = this->m_chunkList.Next(chunk);
        this->m_chunks[16 * chunk->m_indexY + chunk->m_indexX] = NULL;
        chunk->Purge();
        CMap::FreeChunk(chunk);
    }

    for (CMapBaseObjLink* link = this->m_linkList.Head(), * next; link; link = next) {
        next = this->m_linkList.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        mapObjDef->Purge(1);
    }

    for (uint32_t i = 0; i < this->m_textures.m_count; i++) {
        if (this->m_textures.m_data[i].m_texture) {
            TextureRelease(this->m_textures.m_data[i].m_texture);
            this->m_textures.m_data[i].m_texture = NULL;
        }
    }

    this->m_textures.SetCount(0);
}

// PROVEN: 85663c4a IDENTICAL/NONE
// OFFSET: 0x007869A0/0x140475CC0
void CMapArea::PurgeChunk(CMapChunk* chunk) {
    this->m_chunks[16 * chunk->m_indexY + chunk->m_indexX] = NULL;
    chunk->Purge();
    CMap::FreeChunk(chunk);
}

// OFFSET: 0x00796920/0x140489AB0
int32_t CMapArea::IsLoading() {
    for (int32_t i = 0; i < 3; i++) {
        if (this->m_asyncObjects[i]) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00796940/0x140489AE0
void CMapArea::ProcessIffChunks(int32_t fileIndex, uint32_t* data, int32_t size) {
    uint32_t chunkOffset = 0;

    for (int32_t remaining = size; remaining > 0;) {
        uint32_t token = data[0];
        uint8_t* chunkData = reinterpret_cast<uint8_t*>(&data[2]);
        remaining -= 8;

        switch (token) {
            case 'MCNK':
                if (!this->m_chunkInfo[fileIndex]) {
                    this->m_chunkInfo[fileIndex] = CMap::AllocChunkInfo();
                }

                this->m_chunkInfo[fileIndex]->m_chunks[chunkOffset / 16].m_offset = static_cast<uint32_t>(&chunkData[-8] - this->m_buffers[fileIndex]);
                this->m_chunkInfo[fileIndex]->m_chunks[chunkOffset / 16].m_size = 0;
                this->m_chunkInfo[fileIndex]->m_chunks[chunkOffset / 16].m_flags = 0;
                chunkOffset += 16;
                break;

            case 'MHDR':
                this->m_header = reinterpret_cast<SMAreaHeader*>(chunkData);
                break;

            case 'MAMP':
                this->m_header->m_mamp = *chunkData;
                break;

            case 'MTEX':
                this->m_textureNames = reinterpret_cast<const char*>(chunkData);
                this->m_textureNamesSize = data[1];
                break;

            case 'MMDX':
                this->m_doodadNames = reinterpret_cast<const char*>(chunkData);
                break;

            case 'MMID':
                this->m_doodadNameOffsets = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MWMO':
                this->m_mapObjNames = reinterpret_cast<const char*>(chunkData);
                break;

            case 'MWID':
                this->m_mapObjNameOffsets = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MDDF':
                this->m_doodadDefs = reinterpret_cast<SMDoodadDef*>(chunkData);
                this->m_doodadDefCount = data[1] / 36;
                break;

            case 'MODF':
                this->m_mapObjDefs = reinterpret_cast<SMMapObjDef*>(chunkData);
                this->m_mapObjDefCount = data[1] >> 6;
                break;

            case 'MFBO':
                this->m_flightBounds = chunkData;
                break;

            case 'MTXF':
                this->m_textureFlags = reinterpret_cast<const uint32_t*>(chunkData);
                break;

            case 'MH2O':
                this->m_liquid = chunkData;
                break;
        }

        remaining -= data[1];
        data = reinterpret_cast<uint32_t*>(&chunkData[data[1]]);
    }
}

// OFFSET: 0x00796AF0/0x140489D10
void CMapArea::AsyncFailedCallback(CAsyncObject* object) {
    CMapArea* area = static_cast<CMapArea*>(object->m_userArg);
    int32_t index;

    for (index = 0; index < 3; index++) {
        if (area->m_asyncObjects[index] == object) {
            break;
        }
    }

    if (!object->m_cancelled) {
        AsyncFileReadSetStreamingError();
    }

    AsyncFileReadDestroyObject(area->m_asyncObjects[index]);
    area->m_asyncObjects[index] = NULL;
    area->m_files[index] = NULL;
    World::SetLoadFailed(1, NULL);
}

// OFFSET: 0x00796E70/0x14048A180
void CMapArea::AsyncPriorityCallback(void* param) {
    CMapArea* area = static_cast<CMapArea*>(param);

    for (int32_t i = 2; i >= 0; i--) {
        if (area->m_asyncObjects[i]) {
            AsyncFileReadLinkToPriority(area->m_asyncObjects[i], 1);
        }
    }
}

// OFFSET: 0x00796E80/0x14048A1C0
void CMapArea::PurgeChunks(const CiRect& rect) {
    int32_t maxX = rect.maxX - this->m_chunkOffsetX;
    int32_t minY = rect.minY - this->m_chunkOffsetY;
    int32_t maxY = rect.maxY - this->m_chunkOffsetY;
    int32_t minX = rect.minX - this->m_chunkOffsetX;

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            CMapChunk* chunk = this->m_chunks[16 * y + x];

            if (chunk) {
                this->PurgeChunk(chunk);
            }
        }
    }
}

// OFFSET: 0x007972F0/0x14048A6F0
CTexture* CMapArea::LoadTexture(uint32_t index) {
    CMapAreaTexture* texture = &this->m_textures.m_data[index];

    if (!texture->m_texture) {
        uint32_t flags = 0;

        if (this->m_textureFlags) {
            flags = this->m_textureFlags[index];
        }

        texture->m_texture = CMap::LoadTerrainTexture(texture->m_name, flags);
    }

    TextureAddRef(texture->m_texture);
    return texture->m_texture;
}

// OFFSET: 0x00797340/0x14048A740
CTexture* CMapArea::GetTexture(uint32_t index) {
    return this->m_textures.m_data[index].m_texture;
}

// OFFSET: 0x007973C0
void CMapArea::PrepareChunk(int32_t x, int32_t y) {
    CMapChunk* chunk = CMap::AllocChunk();
    this->m_chunkList.LinkToTail(chunk);
    chunk->m_map = this->m_map;
    chunk->m_worldMap = this->m_worldMap;
    chunk->m_indexX = x;
    chunk->m_indexY = y;
    chunk->m_area = this;
    chunk->m_chunkX = x + this->m_chunkOffsetX;
    chunk->m_chunkY = y + this->m_chunkOffsetY;

    uint32_t index = x + 16 * y;
    int32_t isNew = 1;

    if (this->m_chunkInfo[0]->m_chunks[index].m_flags & 0x1) {
        isNew = 0;
    }

    this->m_chunkInfo[0]->m_chunks[index].m_flags |= 0x1;

    for (int32_t i = 0; i < 3; i++) {
        if (this->m_buffers[i]) {
            chunk->CreatePointers(this->m_buffers[i] + this->m_chunkInfo[i]->m_chunks[index].m_offset, isNew, i == 0);
        }
    }

    chunk->Create(isNew);
}

// OFFSET: 0x00797490/0x14048A930
void CMapArea::Update(const CiRect& rect) {
    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        int32_t localY = y & 0xF;

        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            int32_t localX = x & 0xF;

            if (x >= World::gbChunkRect.minX && x <= World::gbChunkRect.maxX && y >= World::gbChunkRect.minY && y <= World::gbChunkRect.maxY) {
                if (!this->m_chunks[16 * localY + localX]) {
                    this->PrepareChunk(localX, localY);
                }
            } else if (this->m_chunks[localX + 16 * localY]) {
                this->PurgeChunk(this->m_chunks[localX + 16 * localY]);
            }
        }
    }
}

// OFFSET: 0x00797540/0x14048AA10
void CMapArea::CreateMapObjDefs() {
    C3Vector offset(17066.666f, 17066.666f, 0.0f);

    for (uint32_t i = 0; i < this->m_mapObjDefCount; i++) {
        SMMapObjDef& def = this->m_mapObjDefs[i];

        if (def.m_flags & 0x1) {
            continue;
        }

        CMapObjDef* mapObjDef = this->m_worldMap->CreateMapObjDef(&this->m_mapObjNames[this->m_mapObjNameOffsets[def.m_nameId]], def, offset, 1);
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDef, this);
        mapObjDef->m_parentLinkList.LinkToTail(link);
        this->m_linkList.LinkToTail(link);
    }
}

// OFFSET: [0x007976F0/0x14048AD80, 0x00797B70/0x14048B380]
CMapArea::~CMapArea() {
    for (uint32_t i = 0; i < 3; i++) {
        if (this->m_buffers[i]) {
            CMap::FreeRawAreaData(this->m_buffers[i]);
        }

        if (this->m_chunkInfo[i]) {
            CMap::FreeChunkInfo(this->m_chunkInfo[i]);
        }
    }

    if (this->m_areaFlags & 0x4) {
        SMemFree(const_cast<void*>(this->m_liquid), __FILE__, __LINE__, 0);
    }

    this->m_doodadDefs = NULL;
    this->m_mapObjDefs = NULL;
    this->m_doodadDefCount = 0;
    this->m_mapObjDefCount = 0;
    this->m_doodadNames = NULL;
    this->m_mapObjNames = NULL;
    this->m_doodadNameOffsets = NULL;
    this->m_mapObjNameOffsets = NULL;
}

// INLINED
CMapAreaTexture::CMapAreaTexture() {
    this->m_name = NULL;
    this->m_texture = NULL;
}

// OFFSET: 0x00797860/0x14048AF80
void CMapArea::LoadTextures() {
    const char* names = this->m_textureNames;
    uint32_t offset = 0;

    if (!names || !this->m_textureNamesSize) {
        return;
    }

    this->m_textures.m_chunk = 32;

    do {
        this->m_textures.SetCount(this->m_textures.m_count + 1);
        this->m_textures.m_data[this->m_textures.m_count - 1].m_name = &names[offset];

        while (names[offset]) {
            offset++;
        }

        offset++;
    } while (offset < this->m_textureNamesSize);
}

// OFFSET: 0x007979C0
CMapArea::CMapArea() {
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_posX = 0.0f;
    this->m_posY = 0.0f;
    this->unk_0050 = 0;
    this->m_indexX = 0;
    this->m_indexY = 0;
    this->m_chunkOffsetX = 0;
    this->m_chunkOffsetY = 0;

    for (uint32_t i = 0; i < 256; i++) {
        this->m_chunkBounds[i].b.x = 0.0f;
        this->m_chunkBounds[i].b.y = 0.0f;
        this->m_chunkBounds[i].b.z = 0.0f;
        this->m_chunkBounds[i].t.x = 0.0f;
        this->m_chunkBounds[i].t.y = 0.0f;
        this->m_chunkBounds[i].t.z = 0.0f;
    }

    this->m_map = NULL;
    this->m_worldMap = NULL;
    this->m_header = NULL;
    this->m_numFiles = 0;

    for (uint32_t i = 0; i < 3; i++) {
        this->m_files[i] = NULL;
        this->m_asyncObjects[i] = NULL;
        this->m_buffers[i] = NULL;
        this->m_bufferSizes[i] = 0;
        this->m_chunkInfo[i] = NULL;
    }

    memset(this->m_chunks, 0, sizeof(this->m_chunks));
    this->m_type |= 0x2;
    this->m_areaFlags = (this->m_areaFlags & ~0x7u) | 0x2;
    this->m_doodadDefs = NULL;
    this->m_mapObjDefs = NULL;
    this->m_doodadDefCount = 0;
    this->m_mapObjDefCount = 0;
    this->m_doodadNames = NULL;
    this->m_mapObjNames = NULL;
    this->m_doodadNameOffsets = NULL;
    this->m_mapObjNameOffsets = NULL;
    this->m_textureNames = NULL;
    this->m_textureNamesSize = 0;
    this->m_flightBounds = NULL;
    this->m_textureFlags = NULL;
    this->m_liquid = NULL;
    this->m_distSq = 0.0f;
}

// OFFSET: 0x00797BA0/0x14048B3D0
void CMapArea::Create() {
    for (int32_t i = 0; i < 3; i++) {
        uint8_t* buffer = this->m_buffers[i];

        if (buffer) {
            uint32_t versionSize = reinterpret_cast<uint32_t*>(buffer)[1];
            this->ProcessIffChunks(i, reinterpret_cast<uint32_t*>(&buffer[versionSize + 8]), this->m_bufferSizes[i] - versionSize - 8);
        }
    }

    this->LoadTextures();

    CMap::CMapAreaTableEntry* entry = this->m_map->m_areaTable.Get(this->m_indexX, this->m_indexY);

    if (entry) {
        entry->m_flags |= 0x2;
    }

    this->m_flags |= 0x40;
    this->CreateMapObjDefs();
}

// OFFSET: 0x00797C40/0x14048B4C0
void CMapArea::AsyncLoadCallback(CAsyncObject* object) {
    CMapArea* area = static_cast<CMapArea*>(object->m_userArg);
    int32_t index;

    for (index = 0; index < 3; index++) {
        if (area->m_asyncObjects[index] == object) {
            break;
        }
    }

    AsyncFileReadDestroyObject(area->m_asyncObjects[index]);
    area->m_asyncObjects[index] = NULL;
    area->m_files[index] = NULL;

    if (--area->m_numFiles == 0) {
        static_cast<CMapArea*>(object->m_userArg)->Create();
    }
}

// OFFSET: 0x00797CA0/0x14048B550
void CMapArea::Load(const char* fileName) {
    SFileImpl* file = NULL;

    if (CMap::SafeOpen(fileName, &file, 0)) {
        this->m_files[this->m_numFiles] = file;
        this->m_bufferSizes[this->m_numFiles] = SFileGetFileSize(this->m_files[this->m_numFiles], NULL);
        this->m_buffers[this->m_numFiles] = static_cast<uint8_t*>(CMap::AllocRawAreaData(this->m_bufferSizes[this->m_numFiles]));
        AsyncFileReadCreateObject(this->m_files[this->m_numFiles], 3, 0, &this->m_asyncObjects[this->m_numFiles], this->m_buffers[this->m_numFiles], this, CMapArea::AsyncLoadCallback, CMapArea::AsyncFailedCallback, 0, 0, 0);
    } else {
        World::SetLoadFailed(1, fileName);
    }

    this->m_numFiles++;
}

// PROVEN: 4ac5fdf4 IDENTICAL/NONE
// OFFSET: 0x007968D0/0x140489A70
void CMapArea::InitBounds() {
    float posX = this->m_posX;
    float posY = this->m_posY;
    this->m_bbox.t.x = posX;
    this->m_bbox.b.x = posX - 533.33331f;
    this->m_bbox.t.y = posY;
    this->m_bbox.t.z = -100000.0f;
    this->m_bbox.b.y = posY - 533.33331f;
    this->m_bbox.b.z = 100000.0f;
}

// OFFSET: 0x00797120/0x14048A4C0
void CMapArea::BatchChunks(const C2iVector& index) {
    uint32_t first = 16 * index.y + index.x;
    CMapChunk* chunks[4];
    chunks[0] = this->m_chunks[first];
    chunks[1] = this->m_chunks[first + 1];
    chunks[2] = this->m_chunks[first + 16];
    chunks[3] = this->m_chunks[first + 17];

    for (uint32_t i = 0; i < 4; i++) {
        if (!chunks[i]) {
            return;
        }
    }

    int32_t multiPass = (this->m_map->m_header.m_flags >> 2) & 1;
    CMapRenderChunk* renderChunk = CMap::AllocRenderChunk();

    if (multiPass) {
        renderChunk->m_flags |= 0x101;

        if (World::terrainAlphaBitDepth == 8) {
            renderChunk->m_flags |= 0x200;
        }
    }

    if (!this->m_header || !this->m_header->m_mamp) {
        renderChunk->m_flags |= 0x4;
    }

    if (s_batchMask == 3 && CanBatchChunks(chunks, 15, multiPass)) {
        renderChunk->AddChunks(chunks);
        renderChunk->AddBatch(15, 3);
        return;
    }

    int32_t top = 0;
    int32_t bottom = 0;
    int32_t left = 0;
    int32_t right = 0;

    if (s_batchMask & 1) {
        left = CanBatchChunks(chunks, 5, multiPass);
        right = CanBatchChunks(chunks, 10, multiPass);
    }

    if (s_batchMask & 2) {
        top = CanBatchChunks(chunks, 3, multiPass);
        bottom = CanBatchChunks(chunks, 12, multiPass);
    }

    uint8_t flags = 2;

    if (left + right > bottom + top) {
        CMapChunk* swap = chunks[2];
        chunks[2] = chunks[1];
        chunks[1] = swap;
        top = left;
        bottom = right;
        flags = 1;
    }

    renderChunk->AddChunks(chunks);

    if (top) {
        renderChunk->AddBatch(3, flags);
    } else {
        renderChunk->AddBatch(1, 0);
        renderChunk->AddBatch(2, 0);
    }

    if (bottom) {
        renderChunk->AddBatch(12, flags);
    } else {
        renderChunk->AddBatch(4, 0);
        renderChunk->AddBatch(8, 0);
    }
}

// OFFSET: 0x0077E730
void CMapArea::QueryLiquidSounds(const C3Vector& pos, float radius, TSGrowableArray<World::LiquidQueryResult>& results) {
    int32_t centerY = static_cast<int32_t>(-(pos.y - this->m_posY) * 0.029999999f);
    float fx = -(pos.x - this->m_posX) * 0.029999999f;
    int32_t extent = static_cast<int32_t>(ceil(radius * 0.030000001f));
    int32_t minX = static_cast<int32_t>(fx) - extent;
    minX = minX <= 0 ? 0 : minX;
    int32_t minY = centerY - extent;
    minY = minY <= 0 ? 0 : minY;
    int32_t maxX = extent + static_cast<int32_t>(fx);

    if (maxX >= 15) {
        maxX = 15;
    }

    int32_t maxY = centerY + extent;

    if (maxY >= 15) {
        maxY = 15;
    }

    TSGrowableArray<LiquidChunkAndDistance> liquids;
    float radiusSq = radius * radius;

    for (int32_t x = minX; x <= maxX; x++) {
        for (int32_t y = minY; y <= maxY; y++) {
            CMapChunk* chunk = this->m_chunks[16 * x + y];

            if (!chunk) {
                continue;
            }

            for (CChunkLiquid* liquid = chunk->m_liquidList.Head(), * next; liquid; liquid = next) {
                float midZ = (liquid->m_maxHeight + liquid->m_minHeight) * 0.5f;
                next = chunk->m_liquidList.Next(liquid);
                LiquidChunkAndDistance* entry = liquids.New();
                entry->m_liquid = liquid;
                entry->m_chunk = chunk;
                float dx = (chunk->m_position.x - 16.666666f) - pos.x;
                float dy = (chunk->m_position.y - 16.666666f) - pos.y;
                entry->m_distSq = (dx * dx + (midZ - pos.z) * (midZ - pos.z) + dy * dy) - 277.77777f;
            }
        }
    }

    if (!liquids.Count()) {
        return;
    }

    LiquidChunkAndDistance* end = &liquids.m_data[liquids.Count()];
    std::sort(liquids.m_data, end);

    for (LiquidChunkAndDistance* entry = liquids.m_data; entry != end; entry++) {
        CChunkLiquid* liquid = entry->m_liquid;
        CMapChunk* chunk = entry->m_chunk;

        if (liquid->m_tileData) {
            float dx = chunk->m_position.x - pos.x;
            float dy = chunk->m_position.y - pos.y;
            float dz = (liquid->m_maxHeight + liquid->m_minHeight) * 0.5f - pos.z;
            const uint8_t* tiles = static_cast<const uint8_t*>(liquid->m_tileData);

            for (uint32_t row = 0; row < 64; row += 8) {
                for (uint32_t col = 0; col < 8; col++) {
                    if ((tiles[row + col] & 0xF) != 15) {
                        if (!(dx * dx + dy * dy + dz * dz <= radiusSq)) {
                            continue;
                        }

                        World::LiquidQueryResult* result = results.New();
                        result->m_liquidType = liquid->m_liquidType;
                        result->m_offset.x = dx;
                        result->m_offset.y = dy;
                        result->m_offset.z = dz;
                    }

                    dy = dy - 4.1666665f;
                }

                dy = dy + 33.333332f;
                dx = dx - 4.1666665f;
            }

            continue;
        }

        float ry = -(pos.y - chunk->m_position.y);
        float rx = -(pos.x - chunk->m_position.x);
        int32_t tileExtent = static_cast<int32_t>(ceil(radius * 0.24000001f));
        int32_t x0 = static_cast<int32_t>(rx * 0.23999999f) - tileExtent;
        int32_t y0 = static_cast<int32_t>(ry * 0.23999999f) - tileExtent;
        int32_t y1 = static_cast<int32_t>(ry * 0.23999999f) + tileExtent + 1;

        if (y1 >= liquid->m_height) {
            y1 = liquid->m_height;
        }

        int32_t x1 = liquid->m_width;

        if (static_cast<int32_t>(rx * 0.23999999f) + tileExtent + 1 < x1) {
            x1 = static_cast<int32_t>(rx * 0.23999999f) + tileExtent + 1;
        }

        int32_t yStart = liquid->m_yOffset;

        if (y0 > yStart) {
            yStart = y0;
        }

        int32_t xStart = liquid->m_xOffset;

        if (x0 > xStart) {
            xStart = x0;
        }

        if (xStart >= x1 || yStart >= y1) {
            continue;
        }

        int32_t xOffset = liquid->m_xOffset;
        float dz = (liquid->m_maxHeight + liquid->m_minHeight) * 0.5f - pos.z;
        float dx = ((chunk->m_position.x - static_cast<float>(xStart) * 4.1666665f) - 2.0833333f) - pos.x;
        float dy = ((chunk->m_position.y - static_cast<float>(yStart) * 4.1666665f) - 2.0833333f) - pos.y;
        int32_t yOffset = liquid->m_yOffset;
        int32_t col0 = yStart - yOffset;
        int32_t colEnd = y1 - yOffset;
        int32_t cols = y1 - yStart;
        int32_t stride = liquid->m_height - yOffset;
        const uint8_t* exists = Liquid::GetExistsTable(this->m_liquid, static_cast<const SMLiquidInstance*>(liquid->m_liquidHeader));
        int32_t rowEnd = x1 - xOffset;
        int32_t row = xStart - xOffset;

        if (row >= rowEnd) {
            continue;
        }

        int32_t bit = col0 + stride * row;
        float rowStep = static_cast<float>(cols) * 4.1666665f;

        for (int32_t rows = rowEnd - row; rows; rows--) {
            if (col0 < colEnd) {
                int32_t index = bit;

                for (int32_t i = cols; i; i--) {
                    if ((static_cast<uint8_t>(1 << (index & 7)) & exists[index >> 3]) && dy * dy + dx * dx + dz * dz <= radiusSq) {
                        World::LiquidQueryResult* result = results.New();
                        result->m_liquidType = liquid->m_liquidType;
                        result->m_offset.x = dx;
                        result->m_offset.y = dy;
                        result->m_offset.z = dz;
                    }

                    index++;
                    dy = dy - 4.1666665f;
                }
            }

            bit += stride;
            dy = rowStep + dy;
            dx = dx - 4.1666665f;
        }
    }
}
