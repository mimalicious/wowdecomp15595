#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include <cstring>
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// STATIC: 0x00D02638
static CGxBatch s_chunkBatch;

// STATIC: 0x00E78C08
uint8_t CMapRenderChunk::s_alphaRowBuffer[256];

// STATIC: 0x00E78D08
uint8_t CMapRenderChunk::s_emptyShadow[64];

// STATIC: 0x00E78D48
uint8_t CMapRenderChunk::s_emptyAlpha[64];

// STATIC: 0x00E78D88
uint32_t CMapRenderChunk::s_vertexFormat;

// STATIC: 0x00E78D8C
int32_t CMapRenderChunk::s_pixelShaderIds[4];

// STATIC: 0x00E78DA0
uint8_t CMapRenderChunk::s_texelBuffer[0x10000];

// STATIC: 0x00E88DA0
void (*CMapRenderChunk::s_renderMultiPass)(CWorldView* view, CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch);

// STATIC: 0x00E88DA4
uint32_t CMapRenderChunk::s_savedFogColor;

static uint32_t AlphaSize(const CMapChunk* chunk) {
    const SMAreaHeader* header = chunk->m_area->m_header;
    return header ? 64 >> header->m_mamp : 64;
}

// PROVEN: 0dcac47e IDENTICAL/NONE
// OFFSET: 0x00765E90/0x14044BB30
void CMapRenderChunk::Initialize() {
    CMapRenderChunk::s_vertexFormat = GxVBF_PN;
    s_chunkBatch.m_primType = GxPrim_Triangles;
    s_chunkBatch.m_count = CMapChunk::s_indexCount;
    s_chunkBatch.m_start = 0;
    s_chunkBatch.m_minIndex = 0;
    s_chunkBatch.m_maxIndex = CMapChunk::s_vertexCount - 1;
    memset(CMapRenderChunk::s_emptyAlpha, 0, sizeof(CMapRenderChunk::s_emptyAlpha));
    memset(CMapRenderChunk::s_emptyShadow, 0, sizeof(CMapRenderChunk::s_emptyShadow));
}

// PROVEN: 3190c7c0 IDENTICAL/NONE
// OFFSET: 0x00765EF0/0x14044BBF0
void CMapRenderChunk::SetVertexFormat(uint32_t format) {
    if (CMapRenderChunk::s_vertexFormat != format) {
        CMapRenderChunk::s_vertexFormat = format;
        CWorldMap::ClearAllChunkBufs();
    }
}

// PROVEN: 9bf5b422 IDENTICAL/NONE
// OFFSET: 0x00765F10/0x14044BC10
void CMapRenderChunk::FreeBuf() {
    if (this->m_chunkBuf) {
        CMapChunkBuf::Free(this->m_chunkBuf);
        this->m_chunkBuf = NULL;
    }
}

// OFFSET: 0x00765F40/0x14044BC40
void CMapRenderChunk::AddChunks(CMapChunk** chunks) {
    for (uint32_t i = 0; i < 4; i++) {
        CMapChunk* chunk = chunks[i];
        this->m_chunks[i] = chunk;
        chunk->m_chunkFlags |= 0x1;
    }
}

// PROVEN: 80e1e3d8 IDENTICAL/NONE
// OFFSET: 0x00765F80
void CMapRenderChunk::DestroyBufs() {
    for (uint32_t i = 0; i < this->m_batchCount; i++) {
        CMapRenderChunkBatch& batch = this->m_batches[i];

        for (uint32_t j = 0; j < batch.m_layerCount; j++) {
            CMapRenderChunkLayer& layer = batch.m_layers[j];

            if (layer.m_texture) {
                TextureRelease(layer.m_texture);
                layer.m_texture = NULL;
            }

            layer.m_flags = 0;
            layer.m_textureId = 0xFFFFFFFF;

            if (layer.m_alphaTexture) {
                CTexture* texture = layer.m_alphaTexture;
                TextureRelease(texture);
                layer.m_alphaTexture = NULL;
            }
        }

        CTexture* shadowTexture = batch.m_shadowTexture;
        batch.m_layerCount = 0;

        if (shadowTexture) {
            CTexture* texture = shadowTexture;
            TextureRelease(texture);
            batch.m_shadowTexture = NULL;
        }

        if (batch.m_shaderTexture) {
            CTexture* texture = batch.m_shaderTexture;
            TextureRelease(texture);
            batch.m_shaderTexture = NULL;
        }
    }
}

// OFFSET: 0x00766060/0x14044BD80
void CMapRenderChunk::UpdateLoaded() {
    for (uint32_t i = 0; i < this->m_batchCount; i++) {
        CMapRenderChunkBatch& batch = this->m_batches[i];

        for (int32_t j = 0; j < static_cast<int32_t>(batch.m_layerCount); j++) {
            if (!TextureGetGxTex(batch.m_layers[j].m_texture)) {
                return;
            }
        }

        if (batch.m_materialTexture && !TextureGetGxTex(batch.m_materialTexture)) {
            return;
        }
    }

    this->m_flags |= 0x2;
}

// OFFSET: 0x00766700
CMapChunk* CMapRenderChunk::GetBatchChunk(uint32_t chunkMask) {
    uint32_t bit = 1;

    for (int32_t i = 0; i < 4; i++, bit *= 2) {
        if (bit & chunkMask) {
            return this->m_chunks[i];
        }
    }

    return NULL;
}

// PROVEN: fdec8e70 IDENTICAL/NONE
// OFFSET: 0x00766730/0x14044C320
void CMapRenderChunk::AllocBuf() {
    this->m_chunkBuf = CMapChunkBuf::Alloc(580, 3072, static_cast<EGxVertexBufferFormat>(CMapRenderChunk::s_vertexFormat));
}

// OFFSET: 0x00766760
void CMapRenderChunk::AddBatch(uint32_t chunkMask, uint8_t flags) {
    CMapRenderChunkBatch& batch = this->m_batches[this->m_batchCount++];
    int32_t first = 1;
    uint32_t bit = 1;

    for (uint32_t i = 0; i < 4; i++, bit *= 2) {
        if (!(chunkMask & bit)) {
            continue;
        }

        CMapChunk* chunk = this->m_chunks[i];
        chunk->m_renderChunk = this;

        if (first) {
            batch.m_renderChunk = this;
            batch.m_position = chunk->m_position;
            batch.m_flags = flags;
            batch.m_chunkMask = static_cast<uint8_t>(chunkMask);
            this->m_bbox = chunk->m_terrainBox;
            first = 0;
        } else {
            this->m_bbox |= chunk->m_terrainBox;
        }
    }

    float dz = this->m_bbox.t.z - this->m_bbox.b.z;
    float dy = this->m_bbox.t.y - this->m_bbox.b.y;
    float dx = this->m_bbox.t.x - this->m_bbox.b.x;
    this->m_radius = sqrt(dx * dx + (dy * dy + dz * dz)) * 0.5f;
    this->m_center.x = (this->m_bbox.b.x + this->m_bbox.t.x) * 0.5f;
    this->m_center.y = (this->m_bbox.t.y + this->m_bbox.b.y) * 0.5f;
    this->m_center.z = (this->m_bbox.t.z + this->m_bbox.b.z) * 0.5f;
}

// OFFSET: 0x007668B0/0x14044C550
void CMapRenderChunk::CreateLayer(CMapArea* area, CMapRenderChunkBatch& batch, const SMLayer* layer, int32_t merge) {
    if (merge) {
        for (int32_t i = 0; i < static_cast<int32_t>(batch.m_layerCount); i++) {
            if (batch.m_layers[i].m_textureId == layer->m_textureId) {
                return;
            }
        }
    }

    CMapRenderChunkLayer& renderLayer = batch.m_layers[batch.m_layerCount];
    uint16_t flags = static_cast<uint16_t>(layer->m_flags);
    renderLayer.m_flags = flags;
    renderLayer.m_index = batch.m_layerCount;

    if (flags & 0x400) {
        TextureAddRef(CMap::s_blackTexture);
        renderLayer.m_texture = CMap::s_blackTexture;
    } else if (flags & 0x8000) {
        renderLayer.m_texture = CMap::GetDynamicTexture(layer->m_textureId);
    } else {
        renderLayer.m_texture = area->LoadTexture(layer->m_textureId);
    }

    renderLayer.m_textureId = layer->m_textureId;
    renderLayer.m_alphaTexture = NULL;
    renderLayer.m_batch = &batch;
    batch.m_layerCount++;
}

// OFFSET: 0x00767CA0/0x14044EBB0
void CMapRenderChunk::UpdateShadowGxTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command != GxTex_Latch) {
        return;
    }

    CMapRenderChunkBatch* batch = static_cast<CMapRenderChunkBatch*>(userArg);
    batch->m_renderChunk->UnpackShadowBits(batch);
    texelStrideInBytes = 2 * width;
    texels = CMapRenderChunk::s_texelBuffer;
}

// OFFSET: 0x00768180/0x14044F020
void CMapRenderChunk::CreateLayers() {
    for (uint32_t i = 0; i < this->m_batchCount; i++) {
        CMapRenderChunkBatch& batch = this->m_batches[i];
        CMapChunk* chunk = NULL;
        uint32_t bit = 1;

        for (int32_t j = 0; j < 4; j++, bit *= 2) {
            if (!(bit & batch.m_chunkMask)) {
                continue;
            }

            chunk = this->m_chunks[j];

            for (uint32_t k = 0; k < chunk->m_header.m_nLayers; k++) {
                this->CreateLayer(chunk->m_area, batch, &chunk->m_layers[k], j != 0);
            }

            if (!(this->m_flags & 0x1)) {
                break;
            }
        }

        batch.m_flags |= 0x8;
        batch.m_shaderType = chunk->m_shaderType;
        batch.m_materialTexture = chunk->m_shaderTexture;
    }

    this->m_flags &= ~0x80u;
}

// OFFSET: 0x00768260/0x14044F1A0
void CMapRenderChunk::AllocShadowTexture(CMapRenderChunkBatch& batch) {
    if (batch.m_shadowTexture) {
        CTexture* texture = batch.m_shadowTexture;
        TextureRelease(texture);
    }

    batch.m_shadowTexture = NULL;

    if (!(World::GetEnables() & 0x10)) {
        return;
    }

    CMapChunk* chunk = this->GetBatchChunk(batch.m_chunkMask);

    if (!chunk->m_shadowMap) {
        return;
    }

    uint32_t size = AlphaSize(chunk);
    uint32_t width = size;
    uint32_t height = size;

    if (this->m_flags & 0x8) {
        width = size / 2;
        height = size / 2;
    }

    CGxTexFlags flags;
    flags.m_filter = GxTex_Linear;
    batch.m_shadowTexture = TextureCreate(GxTex_2d, width, height, 0, static_cast<EGxTexFormat>(3), static_cast<EGxTexFormat>(3), flags, &batch, CMapRenderChunk::UpdateShadowGxTexture, "TerrainBlend", 0);
    GxTexUpdate(TextureGetGxTex(batch.m_shadowTexture), 0, 0, width, height, 1);
}

// OFFSET: 0x00768340/0x14044F310
void CMapRenderChunk::CreateLayerTexture(CMapRenderChunkBatch* batch, CMapRenderChunkLayer* layer) {
    EGxTexFormat format = static_cast<EGxTexFormat>((this->m_flags & 0x100) ? 2 : 3);
    CMapChunk* chunk = this->GetBatchChunk(batch->m_chunkMask);
    const SMLayer& chunkLayer = chunk->m_layers[layer->m_index];

    SLayerAlpha alpha;
    alpha.m_alpha = (chunkLayer.m_flags & 0x100) ? chunk->m_alphaMaps + chunkLayer.m_offsetInMCAL : NULL;
    const uint8_t* shadow = (World::GetEnables() & 0x10) ? chunk->m_shadowMap : NULL;
    alpha.m_flags = chunkLayer.m_flags;

    this->UnpackAlphaBits(chunk, reinterpret_cast<uint32_t*>(CMapRenderChunk::s_texelBuffer), AlphaSize(chunk), &alpha, shadow, format, chunk->m_header.m_flags & 0x8000);
}

// OFFSET: 0x00768410
void CMapRenderChunk::CreateShaderTexture(CMapRenderChunkBatch* batch) {
    EGxTexFormat format = static_cast<EGxTexFormat>((this->m_flags & 0x100) ? 2 : 3);
    uint32_t size = AlphaSize(this->m_chunks[0]);
    uint32_t sourceSize = size;
    uint32_t pitch = size;

    if (this->m_flags & 0x8) {
        size >>= 1;
        pitch = size;
    }

    uint32_t offsets[4];
    uint32_t flags = batch->m_flags;

    if ((flags & 0x3) == 0x3) {
        pitch = 2 * size;
        offsets[0] = 0;
        offsets[1] = size;
        offsets[2] = 2 * size * size;
        offsets[3] = size * (2 * size + 1);
    } else {
        if (flags & 0x2) {
            pitch = 2 * size;
            offsets[1] = size;
        } else if (flags & 0x1) {
            offsets[1] = size * size;
        }

        offsets[0] = 0;
    }

    int32_t first = 1;
    uint32_t* offset = offsets;
    uint32_t bit = 1;

    for (uint32_t i = 0; i < 4; i++, bit *= 2) {
        if (!(bit & batch->m_chunkMask)) {
            continue;
        }

        CMapChunk* chunk = this->m_chunks[i];
        int32_t baseLayer = 0;
        SLayerAlpha alphas[4];
        alphas[0].m_alpha = NULL;
        alphas[1].m_alpha = NULL;
        alphas[2].m_alpha = NULL;
        alphas[3].m_alpha = NULL;

        if (first) {
            for (uint32_t j = 0; j < chunk->m_header.m_nLayers; j++) {
                uint32_t layerFlags = chunk->m_layers[j].m_flags;

                if (layerFlags & 0x100) {
                    alphas[j].m_alpha = chunk->m_alphaMaps + chunk->m_layers[j].m_offsetInMCAL;
                }

                alphas[j].m_flags = layerFlags;
            }

            first = 0;
        } else {
            for (uint32_t j = 0; j < chunk->m_header.m_nLayers; j++) {
                const SMLayer& layer = chunk->m_layers[j];
                uint32_t k;

                for (k = 0; k < batch->m_layerCount; k++) {
                    if (batch->m_layers[k].m_textureId == layer.m_textureId) {
                        break;
                    }
                }

                if (k >= batch->m_layerCount) {
                    continue;
                }

                if (layer.m_flags & 0x100) {
                    alphas[k].m_alpha = layer.m_offsetInMCAL + chunk->m_alphaMaps;
                }

                alphas[k].m_flags = layer.m_flags;

                if (!j) {
                    baseLayer = k;
                }
            }
        }

        const uint8_t* shadow = (World::GetEnables() & 0x10) ? chunk->m_shadowMap : NULL;
        this->UnpackAlphaShadowBits(CMapRenderChunk::s_texelBuffer, *offset, pitch, sourceSize, alphas, baseLayer, shadow, format, chunk->m_header.m_flags & 0x8000);
        offset++;
    }
}

// OFFSET: 0x00768630/0x14044F820
void CMapRenderChunk::UpdateLayerGxTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CMapRenderChunkLayer* layer = static_cast<CMapRenderChunkLayer*>(userArg);
    CMapRenderChunk* renderChunk = layer->m_batch->m_renderChunk;

    if (command != GxTex_Latch) {
        return;
    }

    renderChunk->CreateLayerTexture(layer->m_batch, layer);
    texelStrideInBytes = (renderChunk->m_flags & 0x100) ? 4 * width : 2 * width;
    texels = CMapRenderChunk::s_texelBuffer;
}

// OFFSET: 0x00768690/0x14044F880
void CMapRenderChunk::UpdateShaderGxTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CMapRenderChunkBatch* batch = static_cast<CMapRenderChunkBatch*>(userArg);
    CMapRenderChunk* renderChunk = batch->m_renderChunk;

    if (command != GxTex_Latch) {
        return;
    }

    renderChunk->CreateShaderTexture(batch);
    texels = CMapRenderChunk::s_texelBuffer;
    texelStrideInBytes = (renderChunk->m_flags & 0x100) ? 4 * width : 2 * width;
}

// OFFSET: 0x007686F0
CMapRenderChunk::CMapRenderChunk() {
    this->m_bbox.b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_bbox.t = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_center = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_radius = 0.0f;
    this->m_lastUsedFrame = 0;
    this->m_stamp = 0;
    this->m_flags = 0x80;
    this->m_chunks[0] = NULL;
    this->m_chunks[1] = NULL;
    this->m_chunks[2] = NULL;
    this->m_chunks[3] = NULL;
    this->m_batchCount = 0;
    this->m_chunkBuf = NULL;
}

// OFFSET: 0x00768790
CMapRenderChunk::~CMapRenderChunk() {
    if (this->m_chunkBuf) {
        CMapChunkBuf::Free(this->m_chunkBuf);
        this->m_chunkBuf = NULL;
    }

    this->DestroyBufs();

    for (uint32_t i = 0; i < 4; i++) {
        CMapChunk* chunk = this->m_chunks[i];

        if (chunk) {
            chunk->m_renderChunk = NULL;
            chunk->m_chunkFlags &= ~0x1u;
            this->m_chunks[i] = NULL;
        }
    }
}

// OFFSET: 0x00768860/0x14044FA80
void CMapRenderChunk::AllocLayerTexture(CMapRenderChunkBatch& batch, uint32_t index) {
    CMapRenderChunkLayer& layer = batch.m_layers[index];

    if (layer.m_alphaTexture) {
        CTexture* texture = layer.m_alphaTexture;
        TextureRelease(texture);
    }

    layer.m_alphaTexture = NULL;

    uint32_t flags = this->m_flags;
    int32_t baseOnly = (flags & 0x1) && !layer.m_index;

    if (!(layer.m_flags & 0x100) && !baseOnly) {
        return;
    }

    uint32_t size = AlphaSize(this->m_chunks[0]);
    uint32_t width = size;
    uint32_t height = size;

    if (flags & 0x8) {
        width = size / 2;
        height = size / 2;
    }

    EGxTexFormat format = static_cast<EGxTexFormat>((flags & 0x200) ? 2 : 3);
    EGxTexFormat dataFormat = static_cast<EGxTexFormat>((flags & 0x100) ? 2 : 3);
    CGxTexFlags texFlags;
    texFlags.m_filter = GxTex_Linear;
    layer.m_alphaTexture = TextureCreate(GxTex_2d, width, height, 0, format, dataFormat, texFlags, &layer, CMapRenderChunk::UpdateLayerGxTexture, "TerrainBlend", 0);
    GxTexUpdate(TextureGetGxTex(layer.m_alphaTexture), 0, 0, width, height, 1);
}

// OFFSET: 0x00768970/0x14044FC00
void CMapRenderChunk::AllocShaderTexture(CMapRenderChunkBatch& batch) {
    if (batch.m_shaderTexture) {
        CTexture* texture = batch.m_shaderTexture;
        TextureRelease(texture);
    }

    batch.m_shaderTexture = NULL;

    uint32_t size = AlphaSize(this->m_chunks[0]);
    uint32_t flags = this->m_flags;
    uint32_t width = size;
    uint32_t height = size;

    if (flags & 0x8) {
        width = size >> 1;
        height = size >> 1;
    }

    if (batch.m_flags & 0x1) {
        height *= 2;
    }

    if (batch.m_flags & 0x2) {
        width *= 2;
    }

    EGxTexFormat format = static_cast<EGxTexFormat>((flags & 0x200) ? 2 : 3);
    EGxTexFormat dataFormat = static_cast<EGxTexFormat>((flags & 0x100) ? 2 : 3);
    CGxTexFlags texFlags;
    texFlags.m_filter = GxTex_Linear;
    batch.m_shaderTexture = TextureCreate(GxTex_2d, width, height, 0, format, dataFormat, texFlags, &batch, CMapRenderChunk::UpdateShaderGxTexture, "TerrainBlend", 0);
    GxTexUpdate(TextureGetGxTex(batch.m_shaderTexture), 0, 0, width, height, 1);
}

// OFFSET: 0x00768A60/0x14044FD40
void CMapRenderChunk::AllocLayerTextures() {
    uint32_t flags = this->m_flags;
    int32_t mipChanged = 0;

    if ((flags & 0x10) ? !(flags & 0x8) : (flags & 0x8) != 0) {
        mipChanged = 1;
    }

    this->m_flags = flags & ~0x8u;

    if (this->m_flags & 0x10) {
        this->m_flags |= 0x8;
    }

    for (uint32_t i = 0; i < this->m_batchCount; i++) {
        CMapRenderChunkBatch& batch = this->m_batches[i];

        if (batch.m_layerCount && ((batch.m_flags & 0x8) || mipChanged)) {
            if (CMap::enableTerrainShaderPixel) {
                this->AllocShaderTexture(batch);
            } else {
                for (uint32_t j = 0; j < batch.m_layerCount; j++) {
                    this->AllocLayerTexture(batch, j);
                }

                if (!(this->m_flags & 0x1)) {
                    this->AllocShadowTexture(batch);
                }
            }
        }

        batch.m_flags &= ~0x8;
    }
}

// OFFSET: 0x0079D550
CMapRenderChunkBatch::CMapRenderChunkBatch() {
    for (uint32_t i = 0; i < 4; i++) {
        this->m_layers[i].m_flags = 0;
        this->m_layers[i].m_index = 0;
        this->m_layers[i].m_texture = NULL;
        this->m_layers[i].m_textureId = 0xFFFFFFFF;
        this->m_layers[i].m_alphaTexture = NULL;
        this->m_layers[i].m_batch = NULL;
    }

    this->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_batch.m_primType = GxPrim_TriangleFan;
    this->m_batch.m_start = 0;
    this->m_batch.m_count = 0;
    this->m_batch.m_minIndex = 0;
    this->m_batch.m_maxIndex = 0;
    this->m_flags = 0;
    this->m_layerCount = 0;
    this->m_shadowTexture = NULL;
    this->m_shaderTexture = NULL;
    this->m_materialTexture = NULL;
    this->m_renderChunk = NULL;
}

// PROVEN: ee8ed274 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CMapRenderChunk::DestroyPools() {
}

// PROVEN: 784ea8a8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CMapRenderChunk::DirtyPools() {
}

// PROVEN: dfa0e1fb IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CMapRenderChunk::UpdateBufs() {
}

// PROVEN: f41802ce IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CMapRenderChunk::UpdatePools() {
}
