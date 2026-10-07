#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D03D20
static uint16_t s_streamVertexCount = 145;

// STATIC: 0x00D03D40
static uint16_t s_streamIndexCount = 768;

// STATIC: 0x00D03D78
static float s_animSpeeds[8] = { 64.0f, 48.0f, 32.0f, 16.0f, 8.0f, 4.0f, 2.0f, 1.0f };

// STATIC: 0x00E88DA8
C4Vector CMapRenderChunk::s_vertexShaderConsts[38];

static void SetSpecularMask(CMapRenderChunkBatch* batch) {
    float spec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    for (uint32_t i = 0; i < batch->m_layerCount; i++) {
        if (static_cast<int8_t>(batch->m_layers[i].m_flags) < 0) {
            spec[i] = 0.0f;
        }
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, spec, 1);
}

static C3Vector LayerAnimation(uint16_t flags) {
    C4Vector direction = World::texVect[flags & 7];
    float scale = (1.0f / CMapChunk::s_geoToTex) / s_animSpeeds[(flags >> 3) & 7];
    direction.x = direction.x * scale;
    direction.y = direction.y * scale;
    direction.z = direction.z * scale;
    direction.w = direction.w * scale;
    return C3Vector(direction);
}

// OFFSET: 0x00781170
void CMapRenderChunk::InitializeVertexShaderConstants(CWorldView* view) {
    C4Vector* consts = CMapRenderChunk::s_vertexShaderConsts;
    memset(consts, 0, sizeof(CMapRenderChunk::s_vertexShaderConsts));

    C44Matrix translate;
    C3Vector move(-view->m_camPos.x, -view->m_camPos.y, -view->m_camPos.z);
    translate.Translate(move);

    C44Matrix viewMat;
    GxXformView(viewMat);
    *reinterpret_cast<C44Matrix*>(&consts[0]) = translate * viewMat;
    *reinterpret_cast<C44Matrix*>(&consts[8]) = viewMat.Transpose();
    *reinterpret_cast<C44Matrix*>(&consts[4]) = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        consts[6].x = consts[6].x * -1.0f;
        consts[6].y = consts[6].y * -1.0f;
        consts[6].z = consts[6].z * -1.0f;
        consts[6].w = consts[6].w * -1.0f;
    }

    for (uint32_t i = 13; i < 20; i++) {
        consts[i].x = 0.0f;
        consts[i].y = 0.0f;
        consts[i].z = 0.0f;
    }

    C3Vector sunDir(0.0f, 0.0f, 0.0f);

    if (CWorldScene::s_lighting.GetSunLight(sunDir, reinterpret_cast<C3Vector&>(consts[25]), reinterpret_cast<C3Vector&>(consts[26]), reinterpret_cast<C3Vector&>(consts[27]))) {
        C33Matrix rotation(*reinterpret_cast<C44Matrix*>(&consts[0]));
        consts[24].x = rotation.c0 * -sunDir.z + rotation.b0 * -sunDir.y + rotation.a0 * -sunDir.x;
        consts[24].y = rotation.c1 * -sunDir.z + rotation.b1 * -sunDir.y + rotation.a1 * -sunDir.x;
        consts[24].z = rotation.c2 * -sunDir.z + rotation.b2 * -sunDir.y + rotation.a2 * -sunDir.x;
        consts[24].w = 1.0f;
        consts[27].w = 20.0f;
    } else {
        consts[25].x = 1.0f;
        consts[25].y = 1.0f;
        consts[25].z = 1.0f;
        consts[25].w = 1.0f;
        consts[27].x = 0.0f;
        consts[27].y = 0.0f;
        consts[27].z = 0.0f;
        consts[27].w = 0.0f;
    }

    for (uint32_t i = 0; i < 3; i++) {
        C4Vector* light = &consts[28 + 3 * i];
        C3Vector pos(0.0f, 0.0f, 0.0f);

        if (CWorldScene::s_lighting.GetPointLight(i, pos, reinterpret_cast<C3Vector&>(light[1]), reinterpret_cast<C3Vector&>(light[2]))) {
            const C3Vector& camPos = CWorldScene::s_activeWorldView->m_camPos;
            light[0].x = pos.x - camPos.x;
            light[0].y = pos.y - camPos.y;
            light[0].z = pos.z - camPos.z;
            light[0].w = 1.0f;
        } else {
            light[0].x = 0.0f;
            light[0].y = 0.0f;
            light[0].z = 0.0f;
            light[0].w = 0.0f;
            light[1].x = 0.0f;
            light[1].y = 0.0f;
            light[1].z = 0.0f;
            light[1].w = 0.0f;
            light[2].x = 1.0f;
            light[2].y = 0.0f;
            light[2].z = 0.0f;
            light[2].w = 0.0f;
        }
    }

    if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
        const DayNight::FogInfo* fog = DayNight::GetFogInfo();
        float scale = 1.0f / (fog->m_end - fog->m_start);
        consts[12].x = -(scale * CShaderEffect::s_fogMul);
        consts[12].y = scale * fog->m_end;
        consts[12].z = fog->m_density;
        consts[12].w = 0.0f;
    } else {
        consts[12].x = 0.0f;
        consts[12].y = 1.0f;
        consts[12].z = 1.0f;
        consts[12].w = 0.0f;
    }

    consts[12].w = 0.0f;

    if (CShaderEffect::GetClipEnabled() && g_theGxDevicePtr->m_caps.unk_02BC) {
        C4Plane plane = view->m_cullPlane;
        plane.Transform(translate);
        plane.Transform(viewMat);
        consts[37].x = plane.n.x;
        consts[37].y = plane.n.y;
        consts[37].z = plane.n.z;
        consts[37].w = plane.d;
    }
}

// OFFSET: 0x007818A0
void CMapRenderChunk::RenderPrepBufs(CGxBuf* vertexBuf, CGxBuf* indexBuf) {
    int32_t vertexValid = vertexBuf->unk_001C && vertexBuf->unk_001D;
    int32_t indexValid = indexBuf->unk_001C && indexBuf->unk_001D;

    if (!vertexValid) {
        char* vertices = g_theGxDevicePtr->BufLock(vertexBuf);
        C3Vector origin(0.0f, 0.0f, 0.0f);
        int32_t index = 0;

        for (uint32_t i = 0; i < 4; i++) {
            CMapChunk* chunk = this->m_chunks[i];
            float baseZ = this->m_chunks[0]->m_position.z;
            origin.x = chunk->m_position.x - baseZ;
            origin.y = chunk->m_position.y - baseZ;
            origin.z = chunk->m_position.z - baseZ;
            chunk->CreateVertices(vertices, index, origin);
            index += 145;
        }

        g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
        vertexBuf->unk_001C = 1;
    }

    if (indexValid) {
        return;
    }

    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    uint32_t starts[4];
    uint32_t counts[4];
    uint16_t minIndices[4];
    uint16_t maxIndices[4];
    uint32_t indexOffset = 0;
    uint16_t vertexBase = 0;

    for (uint32_t i = 0; i < 4; i++) {
        starts[i] = indexOffset;
        uint32_t count;
        this->m_chunks[i]->CreateIndices(&indices[indexOffset], vertexBase, count, this->m_flags & 0x20);
        indexOffset += count;
        counts[i] = count;
        minIndices[i] = vertexBase;
        vertexBase += 145;
        maxIndices[i] = vertexBase - 1;
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;

    for (uint32_t i = 0; i < this->m_batchCount; i++) {
        CMapRenderChunkBatch& batch = this->m_batches[i];
        batch.m_batch.m_primType = GxPrim_Triangles;
        batch.m_batch.m_start = 0xFFFF;
        batch.m_batch.m_count = 0;
        batch.m_batch.m_minIndex = 0xFFFF;
        batch.m_batch.m_maxIndex = 0;

        uint8_t mask = batch.m_chunkMask;

        for (uint32_t j = 0; j < 4; j++) {
            if (!(mask & (1 << j))) {
                continue;
            }

            if (batch.m_batch.m_start >= starts[j]) {
                batch.m_batch.m_start = starts[j];
            }

            batch.m_batch.m_count += counts[j];

            if (batch.m_batch.m_minIndex >= minIndices[j]) {
                batch.m_batch.m_minIndex = minIndices[j];
            }

            if (batch.m_batch.m_maxIndex <= maxIndices[j]) {
                batch.m_batch.m_maxIndex = maxIndices[j];
            }
        }
    }
}

// OFFSET: 0x00781BD0
void CMapRenderChunk::UseStreamingBufs() {
    uint32_t vertexSize = GxVertexSize(static_cast<EGxVertexBufferFormat>(CMapRenderChunk::s_vertexFormat));
    uint32_t indexCount = 4 * s_streamIndexCount;
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, vertexSize, 4 * s_streamVertexCount);
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, indexCount);
    this->RenderPrepBufs(vertexBuf, indexBuf);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, static_cast<EGxVertexBufferFormat>(CMapRenderChunk::s_vertexFormat));
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
}

// OFFSET: 0x00781C50
void CMapRenderChunk::RenderSetup(CMapRenderChunkBatch* batch, int32_t vertexShader) {
    if (!vertexShader) {
        C44Matrix world;
        world.d0 = batch->m_position.x - CWorldScene::s_activeWorldView->m_camPos.x;
        world.d1 = batch->m_position.y - CWorldScene::s_activeWorldView->m_camPos.y;
        world.d2 = batch->m_position.z - CWorldScene::s_activeWorldView->m_camPos.z;
        GxXformSet(GxXform_World, world);
    }

    if (!this->m_chunkBuf) {
        this->UseStreamingBufs();
        return;
    }

    g_theGxDevicePtr->PrimVertexPtr(this->m_chunkBuf->m_vertexBuf, static_cast<EGxVertexBufferFormat>(CMapRenderChunk::s_vertexFormat));
    g_theGxDevicePtr->PrimIndexPtr(this->m_chunkBuf->m_indexBuf);
}

// OFFSET: 0x00781D80
int32_t CMapChunk::LoadDetailDoodadModels() {
    int32_t loaded = 1;

    for (uint32_t i = 0; i < this->m_header.m_nLayers; i++) {
        uint32_t effectId = this->m_layers[i].m_effectId;

        if (!effectId) {
            continue;
        }

        GroundEffectTextureRec* rec = g_groundEffectTextureDB.GetRecord(effectId);

        if (!rec) {
            continue;
        }

        for (uint32_t j = 0; j < 4; j++) {
            if (rec->m_doodadID[j] && !CDetailDoodad::IsModelLoaded(rec->m_doodadID[j])) {
                loaded = 0;
            }
        }
    }

    return loaded;
}

// OFFSET: 0x00781E30
static void CalcTexMatrices(C44Matrix& texMat, C44Matrix& shadowMat, const C3Vector& offset, float scale) {
    texMat.Scale(scale);
    C44Matrix swapped = texMat;
    texMat.a0 = swapped.b0;
    texMat.a1 = swapped.b1;
    texMat.a2 = swapped.b2;
    texMat.a3 = swapped.b3;
    texMat.b0 = swapped.a0;
    texMat.b1 = swapped.a1;
    texMat.b2 = swapped.a2;
    texMat.b3 = swapped.a3;
    texMat.Translate(offset);

    shadowMat.Scale(scale * 0.125f);
    swapped = shadowMat;
    shadowMat.a0 = swapped.b0;
    shadowMat.a1 = swapped.b1;
    shadowMat.a2 = swapped.b2;
    shadowMat.a3 = swapped.b3;
    shadowMat.b0 = swapped.a0;
    shadowMat.b1 = swapped.a1;
    shadowMat.b2 = swapped.a2;
    shadowMat.b3 = swapped.a3;
    shadowMat.Translate(offset);
}

static C3Vector BatchOffset(const CMapRenderChunkBatch* batch) {
    const C3Vector& camPos = CWorldScene::s_activeWorldView->m_camPos;
    return C3Vector(camPos.x - batch->m_position.x, camPos.y - batch->m_position.y, camPos.z - batch->m_position.z);
}

// OFFSET: 0x00781F00
void CMapRenderChunk::RenderMultiPassAdditive(CWorldView* view, CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch) {
    C44Matrix texMat;
    C44Matrix shadowMat;
    C3Vector offset = BatchOffset(batch);
    CalcTexMatrices(texMat, shadowMat, offset, -CMapChunk::s_geoToTex);
    GxXformSet(GxXform_Tex0, texMat);
    GxXformSet(GxXform_Tex1, shadowMat);
    GxRsSet(GxRs_BlendingMode, 0);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    for (uint32_t i = 0; i < batch->m_layerCount; i++) {
        CMapRenderChunkLayer& layer = batch->m_layers[i];
        CGxTex* texture = TextureGetGxTex(layer.m_texture);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
        GxTexSetWrap(texture, GxTex_Wrap, GxTex_Wrap);

        if (static_cast<int8_t>(layer.m_flags) < 0) {
            GxRsSet(GxRs_Lighting, 0);
        }

        if (layer.m_flags & 0x40) {
            GxXformTranslate(GxXform_Tex0, LayerAnimation(layer.m_flags));
        }

        if (i == 1) {
            GxRsSet(GxRs_BlendingMode, 10);
            GxUpdateAlphaRef(GxRs_AlphaRef);
            GxRsSet(GxRs_FogColor, 0);
        }

        if (layer.m_alphaTexture) {
            g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(layer.m_alphaTexture));
        } else {
            GxRsSet(GxRs_Texture1, 0);
        }

        g_theGxDevicePtr->Draw(batch->m_batch);

        if (static_cast<int8_t>(layer.m_flags) < 0) {
            GxRsSet(GxRs_Lighting, 1);
        }

        if (layer.m_flags & 0x40) {
            GxXformSet(GxXform_Tex0, texMat);
        }
    }

    GxRsSet(GxRs_FogColor, static_cast<int32_t>(CMapRenderChunk::s_savedFogColor));
    GxRsSet(GxRs_Texture0, 0);
    GxRsSet(GxRs_Texture1, 0);
}

// OFFSET: 0x007824F0
void CMapRenderChunk::RenderMultiPassAlpha(CWorldView* view, CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch) {
    C44Matrix texMat;
    C44Matrix shadowMat;
    C3Vector offset = BatchOffset(batch);
    CalcTexMatrices(texMat, shadowMat, offset, -CMapChunk::s_geoToTex);
    GxXformSet(GxXform_Tex0, texMat);
    GxXformSet(GxXform_Tex1, shadowMat);
    GxRsSet(GxRs_BlendingMode, 0);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    for (uint32_t i = 0; i < batch->m_layerCount; i++) {
        CMapRenderChunkLayer& layer = batch->m_layers[i];
        CGxTex* texture = TextureGetGxTex(layer.m_texture);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
        GxTexSetWrap(texture, GxTex_Wrap, GxTex_Wrap);

        if (static_cast<int8_t>(layer.m_flags) < 0) {
            GxRsSet(GxRs_Lighting, 0);
        }

        if (layer.m_flags & 0x40) {
            GxXformTranslate(GxXform_Tex0, LayerAnimation(layer.m_flags));
        }

        if (i == 1) {
            GxRsSet(GxRs_BlendingMode, 2);
            GxUpdateAlphaRef(GxRs_AlphaRef);
        }

        if (layer.m_alphaTexture) {
            g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(layer.m_alphaTexture));
        } else {
            GxRsSet(GxRs_Texture1, 0);
        }

        g_theGxDevicePtr->Draw(batch->m_batch);

        if (static_cast<int8_t>(layer.m_flags) < 0) {
            GxRsSet(GxRs_Lighting, 1);
        }

        if (layer.m_flags & 0x40) {
            GxXformSet(GxXform_Tex0, texMat);
        }
    }

    if (batch->m_shadowTexture && (World::GetEnables() & 0x10)) {
        GxRsSet(GxRs_BlendingMode, 2);
        GxUpdateAlphaRef(GxRs_AlphaRef);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(World::shadowModTexture));
        g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(batch->m_shadowTexture));
        g_theGxDevicePtr->Draw(batch->m_batch);
    }

    GxRsSet(GxRs_Texture0, 0);
    GxRsSet(GxRs_Texture1, 0);
}

// OFFSET: 0x00782B50
void CMapRenderChunk::RenderSinglePassPixelShader(CWorldView* view, CMapRenderChunkBatch* batch) {
    GxRsSet(GxRs_BlendingMode, 0);

    C44Matrix texMat;
    C44Matrix shadowMat;
    C3Vector offset = BatchOffset(batch);
    CalcTexMatrices(texMat, shadowMat, offset, -CMapChunk::s_geoToTex);

    if (batch->m_shaderType == 1) {
        SetSpecularMask(batch);
    }

    for (uint32_t i = 0; i < batch->m_layerCount; i++) {
        CMapRenderChunkLayer& layer = batch->m_layers[i];
        CGxTex* texture = TextureGetGxTex(layer.m_texture);
        g_theGxDevicePtr->RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), texture);
        GxTexSetWrap(texture, GxTex_Wrap, GxTex_Wrap);
        GxXformSet(static_cast<EGxXform>(GxXform_Tex0 + i), texMat);

        if (layer.m_flags & 0x40) {
            GxXformTranslate(static_cast<EGxXform>(GxXform_Tex0 + i), LayerAnimation(layer.m_flags));
        }
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture4, TextureGetGxTex(batch->m_shaderTexture));
    GxXformSet(GxXform_Tex4, shadowMat);

    if (batch->m_materialTexture) {
        g_theGxDevicePtr->RsSet(GxRs_Texture5, TextureGetGxTex(batch->m_materialTexture));
        GxRsSet(GxRs_TexGen5, 4);
        GxRsSet(GxRs_TextureShader5, 0);
    }

    g_theGxDevicePtr->Draw(batch->m_batch);
}

// OFFSET: 0x007830A0
void CMapRenderChunk::RenderSetupFixedFunction(CWorldView* view) {
    CWorldScene::s_lighting.SetupGxLights(&view->m_camPos);
    CWorldScene::s_lighting.SetupGxFog();
    GxRsSet(GxRs_MatDiffuse, static_cast<int32_t>(0xFF7F7F7F));

    for (uint32_t i = 0; i < 5; i++) {
        GxRsSet(static_cast<EGxRenderState>(GxRs_TextureCoord0 + i), static_cast<int32_t>(i));
        GxRsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + i), 2);
        GxRsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + i), 1);
    }
}

// OFFSET: 0x007831B0
void CMapRenderChunk::RenderSetupCombiners() {
    GxRsSet(GxRs_ColorOp0, 1);
    GxRsSet(GxRs_AlphaOp0, 0);
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
}

// OFFSET: 0x00783270
void CMapRenderChunk::RenderSetupPixelShader() {
    float fogColor[4];
    fogColor[0] = CWorldScene::s_lighting.m_fogColor.x;
    fogColor[1] = CWorldScene::s_lighting.m_fogColor.y;
    fogColor[2] = CWorldScene::s_lighting.m_fogColor.z;
    fogColor[3] = 1.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, fogColor, 1);

    C4Vector params = ViewSettings::CalculateParams();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 22, reinterpret_cast<const float*>(&params), 1);

    if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
        if (g_theGxDevicePtr->m_caps.unk_02F8) {
            GxRsSet(GxRs_FogColor, *reinterpret_cast<const int32_t*>(&DayNight::GetFogInfo()->m_color));
        }

        if (g_theGxDevicePtr->m_caps.unk_02FC) {
            GxRsSet(GxRs_Fog, 1);
        }
    } else if (g_theGxDevicePtr->m_caps.unk_02FC) {
        GxRsSet(GxRs_Fog, 0);
    }
}

// OFFSET: 0x007833B0
void CMapRenderChunk::SetVertexShader(CMapRenderChunk* renderChunk, CMapRenderChunkBatch* batch, uint32_t shadowMode, uint32_t layerCount, int32_t hasMaterialTexture) {
    C4Vector* consts = CMapRenderChunk::s_vertexShaderConsts;
    int32_t format = 0;

    if (CMapRenderChunk::s_vertexFormat == GxVBF_PNC) {
        format = 1;
    } else if (CMapRenderChunk::s_vertexFormat == GxVBF_PNC2) {
        format = 2;
    }

    float texScale = CMapChunk::s_geoToTex;
    consts[23].x = batch->m_position.x;
    consts[23].y = batch->m_position.y;
    consts[23].z = batch->m_position.z;
    consts[23].w = 0.0f;

    uint32_t layers = layerCount - 1;

    if (layerCount != 1) {
        consts[18].x = -texScale;
        consts[18].y = -texScale;
        consts[18].z = 0.0f;
        consts[18].w = 0.0f;

        for (uint32_t i = 1; i < layers; i++) {
            consts[18 + i] = consts[18];
        }
    }

    float scaleX = -texScale * 0.125f;
    float scaleY = -texScale * 0.125f;

    if (batch->m_flags & 0x1) {
        scaleX = texScale * -0.0625f;
    }

    if (batch->m_flags & 0x2) {
        scaleY = texScale * -0.0625f;
    }

    consts[22].x = scaleX;
    consts[22].y = scaleY;
    consts[22].z = 0.0f;
    consts[22].w = 0.0f;

    int32_t displ = CShaderEffect::TerrainDisplEnabled();
    int32_t clip = CShaderEffect::GetClipEnabled() && g_theGxDevicePtr->m_caps.unk_02BC;
    uint32_t permute = CMap::GetTerrainVSPerm(displ, CWorldScene::s_lighting.m_lightCount != 0, layers, format, hasMaterialTexture, shadowMode, clip);

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &CMap::vs_terrain->m_permutes[permute]);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(consts), 38);

    if (displ) {
        g_theGxDevicePtr->RsSet(GxRs_HullShader, &CMap::hs_terrain->m_permutes[permute]);
        g_theGxDevicePtr->RsSet(GxRs_DomainShader, &CMap::ds_terrain->m_permutes[permute]);
        CShaderEffect::UpdateTerrainDispl();
    }
}

// OFFSET: 0x007835C0
void CMapRenderChunk::RenderSolid(CMapRenderChunkBatch* batch) {
    C44Matrix identity;
    GxXformSet(GxXform_Tex0, identity);
    GxXformSet(GxXform_Tex1, identity);
    GxRsSet(GxRs_BlendingMode, 0);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    CGxTex* texture = TextureGetGxTex(CWorldScene::s_defaultTexture);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    GxTexSetWrap(texture, GxTex_Wrap, GxTex_Wrap);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(CMap::enableTerrainShaderPixel ? CWorldScene::s_defaultBlendTexture : CWorldScene::s_defaultTexture));
    g_theGxDevicePtr->Draw(batch->m_batch);

    GxRsSet(GxRs_Texture0, 0);
    GxRsSet(GxRs_Texture1, 0);
}

// OFFSET: 0x00783820
void CMapRenderChunk::RenderSolidPixelShader(CMapRenderChunkBatch* batch) {
    CMapRenderChunk::RenderSolid(batch);
}

// OFFSET: 0x00783830
void CMapRenderChunk::RenderSolidVertexPixelShader(CWorldView* view, CMapRenderChunkBatch* batch) {
    GxRsSet(GxRs_BlendingMode, 0);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    CGxTex* texture = TextureGetGxTex(CWorldScene::s_defaultTexture);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    GxTexSetWrap(texture, GxTex_Wrap, GxTex_Wrap);
    g_theGxDevicePtr->RsSet(GxRs_Texture4, TextureGetGxTex(CWorldScene::s_defaultBlendTexture));

    CMapRenderChunk::SetVertexShader(batch->m_renderChunk, batch, view->m_shadowMode, 2, 0);
    g_theGxDevicePtr->Draw(batch->m_batch);

    GxRsSet(GxRs_Texture0, 0);
    GxRsSet(GxRs_Texture1, 0);
}

// OFFSET: 0x007844E0
void CMapRenderChunk::SetShaders(uint32_t materialType, uint32_t shadowMode) {
    CMapRenderChunk::s_pixelShaderIds[0] = -1;
    CMapRenderChunk::s_pixelShaderIds[1] = -1;
    CMapRenderChunk::s_pixelShaderIds[2] = -1;
    CMapRenderChunk::s_pixelShaderIds[3] = -1;

    if (materialType == 0 || materialType == 1) {
        CMapRenderChunk::s_renderMultiPass = CMapRenderChunk::RenderMultiPassAlpha;
    } else {
        CMapRenderChunk::s_renderMultiPass = CMapRenderChunk::RenderMultiPassAdditive;
    }

    if (!CMap::enableTerrainShaderPixel) {
        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t permute = CMap::GetTerrainPSPerm(materialType, i + 1, shadowMode);

        if (CMap::ps_terrain && CMap::ps_terrain->m_permutes[permute].Valid()) {
            CMapRenderChunk::s_pixelShaderIds[i] = permute;
        }
    }
}

// OFFSET: 0x00784580
void CMapRenderChunk::RenderSinglePassVertexPixelShader(CWorldView* view, CMapRenderChunkBatch* batch) {
    C4Vector* consts = CMapRenderChunk::s_vertexShaderConsts;
    GxRsSet(GxRs_BlendingMode, 0);

    if (batch->m_shaderType == 1) {
        SetSpecularMask(batch);
    }

    uint32_t i;

    for (i = 0; i < batch->m_layerCount; i++) {
        CMapRenderChunkLayer& layer = batch->m_layers[i];
        CGxTex* texture = TextureGetGxTex(layer.m_texture);
        g_theGxDevicePtr->RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), texture);
        GxTexSetWrap(texture, GxTex_Wrap, GxTex_Wrap);

        uint16_t flags = layer.m_flags;

        if (flags & 0x40) {
            C4Vector direction = World::texVect[flags & 7];
            float scale = (1.0f / CMapChunk::s_geoToTex) / s_animSpeeds[(flags >> 3) & 7];
            consts[13 + i].x = -((direction.x * scale) * 0.125f);
            consts[13 + i].y = -((direction.y * scale) * 0.125f);
        }
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture4, batch->m_shaderTexture ? TextureGetGxTex(batch->m_shaderTexture) : NULL);

    if (batch->m_materialTexture) {
        g_theGxDevicePtr->RsSet(GxRs_Texture5, TextureGetGxTex(batch->m_materialTexture));
    }

    CMapRenderChunk::SetVertexShader(batch->m_renderChunk, batch, view->m_shadowMode, batch->m_layerCount + 1, batch->m_materialTexture != NULL);
    g_theGxDevicePtr->Draw(batch->m_batch);

    for (uint32_t j = 0; j < batch->m_layerCount; j++) {
        if (batch->m_layers[j].m_flags & 0x40) {
            consts[13 + j].x = -0.0f;
            consts[13 + j].y = -0.0f;
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 13 + j, reinterpret_cast<const float*>(&consts[13 + j]), 1);
        }
    }
}

// OFFSET: 0x00784810
void CMapRenderChunk::RenderPrep() {
    uint32_t flags = this->m_flags;

    if (flags & 0x4) {
        if (World::alphaShadowMipLevel || CWorldView::DistSqrToBox(this->m_bbox, CWorldScene::s_defaultWorldView->m_camPos) > 603729.0f) {
            this->m_flags = flags | 0x10;
        } else {
            this->m_flags = flags & ~0x10u;
        }
    }

    if (!this->m_chunkBuf) {
        this->AllocBuf();
    }

    if (this->m_chunkBuf) {
        this->RenderPrepBufs(this->m_chunkBuf->m_vertexBuf, this->m_chunkBuf->m_indexBuf);
    }

    if (static_cast<int8_t>(this->m_flags) < 0) {
        this->CreateLayers();
    }

    if ((this->m_flags & 0x2) || (this->UpdateLoaded(), (this->m_flags & 0x2))) {
        this->AllocLayerTextures();
    }
}

// OFFSET: 0x007848E0
void CMapChunk::RenderPrep(CWorldView* view, float distSq) {
    if (!(this->m_chunkFlags & 0x1)) {
        this->Batch();
    }

    if (this->m_chunkX < World::detailDoodadChunkRect.minX || this->m_chunkX > World::detailDoodadChunkRect.maxX) {
        return;
    }

    if (this->m_chunkY < World::detailDoodadChunkRect.minY || this->m_chunkY > World::detailDoodadChunkRect.maxY) {
        return;
    }

    uint32_t viewFlags = view->m_flags;

    if (!(viewFlags & 0x1) || !(viewFlags & 0x4000)) {
        return;
    }

    if (!this->m_detailDoodadInst) {
        this->CreateDetailDoodads();
    }

    if (this->m_detailDoodadInst) {
        this->m_detailDoodadInst->Update(sqrt(distSq));
    }
}
