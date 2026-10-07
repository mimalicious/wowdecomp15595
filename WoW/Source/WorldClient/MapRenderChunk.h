#ifndef WOW_SOURCE_WORLDCLIENT_MAPRENDERCHUNK_H
#define WOW_SOURCE_WORLDCLIENT_MAPRENDERCHUNK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/MapChunkBuf.h"

struct CGxBuf;
class CGxTex;
class CMapArea;
class CMapChunk;
class CMapRenderChunk;
class CMapRenderChunkBatch;
class CTexture;
class CWorldView;
struct SMLayer;

class CMapRenderChunkLayer {
    public:
    /* 0000 */ uint16_t m_flags;
    /* 0002 */ uint16_t m_index;
    /* 0004 */ CTexture* m_texture;
    /* 0008 */ uint32_t m_textureId;
    /* 000C */ CTexture* m_alphaTexture;
    /* 0010 */ CMapRenderChunkBatch* m_batch;
};

class CMapRenderChunkBatch {
    public:
    /* 0000 */ uint8_t m_flags;
    /* 0001 */ uint8_t m_chunkMask;
    /* 0002 */ uint8_t m_layerCount;
    /* 0003 */ uint8_t m_shaderType;
    /* 0004 */ CMapRenderChunkLayer m_layers[4];
    /* 0054 */ CTexture* m_shaderTexture;
    /* 0058 */ CTexture* m_shadowTexture;
    /* 005C */ CTexture* m_materialTexture;
    /* 0060 */ C3Vector m_position;
    /* 006C */ CGxBatch m_batch;
    /* 007C */ CMapRenderChunk* m_renderChunk;

    CMapRenderChunkBatch();
};

class CMapRenderChunk {
    public:
    struct SLayerAlpha {
        /* 0000 */ uint32_t m_flags;
        /* 0004 */ const uint8_t* m_alpha;
    };

    static uint8_t s_alphaRowBuffer[256];
    static uint8_t s_emptyShadow[64];
    static uint8_t s_emptyAlpha[64];
    static uint32_t s_vertexFormat;
    static uint8_t s_texelBuffer[0x10000];
    static C4Vector s_vertexShaderConsts[38];
    static int32_t s_pixelShaderIds[4];
    static void (*s_renderMultiPass)(CWorldView* view, CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch);
    static uint32_t s_savedFogColor;

    static void DestroyPools();
    static void DirtyPools();
    static void Initialize();
    static void InitializeVertexShaderConstants(CWorldView* view);
    static void RenderMultiPassAdditive(CWorldView* view, CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch);
    static void RenderMultiPassAlpha(CWorldView* view, CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch);
    static void RenderSetupCombiners();
    static void RenderSetupFixedFunction(CWorldView* view);
    static void RenderSetupPixelShader();
    static void RenderSinglePassPixelShader(CWorldView* view, CMapRenderChunkBatch* batch);
    static void RenderSinglePassVertexPixelShader(CWorldView* view, CMapRenderChunkBatch* batch);
    static void RenderSolid(CMapRenderChunkBatch* batch);
    static void RenderSolidPixelShader(CMapRenderChunkBatch* batch);
    static void RenderSolidVertexPixelShader(CWorldView* view, CMapRenderChunkBatch* batch);
    static void SetShaders(uint32_t materialType, uint32_t shadowMode);
    static void SetVertexFormat(uint32_t format);
    static void SetVertexShader(CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch, uint32_t shadowMode, uint32_t layerCount, int32_t hasMaterialTexture);
    static void UpdateBufs();
    static void UpdateLayerGxTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdatePools();
    static void UpdateShaderGxTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdateShadowGxTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ TSLink<CMapRenderChunk> m_link;
    /* 000C */ uint32_t m_flags;
    /* 0010 */ uint32_t m_stamp;
    /* 0014 */ uint32_t m_lastUsedFrame;
    /* 0018 */ CMapChunk* m_chunks[4];
    /* 0028 */ CAaBox m_bbox;
    /* 0040 */ C3Vector m_center;
    /* 004C */ float m_radius;
    /* 0050 */ uint32_t m_batchCount;
    /* 0054 */ CMapRenderChunkBatch m_batches[4];
    /* 0254 */ CMapChunkBuf* m_chunkBuf;

    CMapRenderChunk();
    ~CMapRenderChunk();
    void AddBatch(uint32_t chunkMask, uint8_t flags);
    void AddChunks(CMapChunk** chunks);
    void AllocBuf();
    void AllocLayerTexture(CMapRenderChunkBatch& batch, uint32_t layer);
    void AllocLayerTextures();
    void AllocShaderTexture(CMapRenderChunkBatch& batch);
    void AllocShadowTexture(CMapRenderChunkBatch& batch);
    void CreateLayer(CMapArea* area, CMapRenderChunkBatch& batch, const SMLayer* layer, int32_t merge);
    void CreateLayers();
    void CreateLayerTexture(CMapRenderChunkBatch* batch, CMapRenderChunkLayer* layer);
    void CreateShaderTexture(CMapRenderChunkBatch* batch);
    void DestroyBufs();
    void FreeBuf();
    CMapChunk* GetBatchChunk(uint32_t chunkMask);
    void RenderPrep();
    void RenderPrepBufs(CGxBuf* vertexBuf, CGxBuf* indexBuf);
    void RenderSetup(CMapRenderChunkBatch* batch, int32_t vertexShader);
    void UnpackAlphaBits(CMapChunk* chunk, uint32_t* dest, uint32_t size, SLayerAlpha* alpha, const uint8_t* shadow, EGxTexFormat format, int32_t fixedAlpha);
    void UnpackAlphaShadowBits(void* dest, uint32_t x, uint32_t pitch, uint32_t size, SLayerAlpha* alpha, int32_t layer, const uint8_t* shadow, EGxTexFormat format, int32_t fixedAlpha);
    void UnpackShadowBits(CMapRenderChunkBatch* batch);
    void UpdateLoaded();
    void UseStreamingBufs();
};


#endif
