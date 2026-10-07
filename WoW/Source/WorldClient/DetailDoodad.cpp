#include "WoW/Source/WorldClient/DetailDoodad.h"
#include <cmath>
#include <cstring>
#include <new>
#include "Engine/Source/Base/BlzAlgorithm.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GroundEffectDoodadRec.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

struct DetailDoodadVertex {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_normal;
    /* 0018 */ CImVector m_color;
    /* 001C */ CImVector m_light;
    /* 0020 */ C2Vector m_texCoord;
    /* 0028 */ float m_wind;
    /* 002C */ float m_windPhase;
};

struct DetailDoodadInstancedVertex {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C2Vector m_texCoord;
    /* 0014 */ float m_wind[2];
};

struct DetailDoodadInstance {
    /* 0000 */ uint8_t m_normal[4];
    /* 0004 */ CImVector m_color;
    /* 0008 */ CImVector m_light;
    /* 000C */ C3Vector m_position;
    /* 0018 */ uint8_t m_up[3];
    /* 001B */ uint8_t m_rotation;
    /* 001C */ uint8_t m_scale;
    /* 001D */ uint8_t m_windPhase;
    /* 001E */ uint8_t m_pad[2];
};

// STATIC: 0x00E74398
CGxShader* CDetailDoodad::s_pixelShader;

// STATIC: 0x00E7439C
CGxShader* CDetailDoodad::s_vertexShader;

// STATIC: 0x00E743A0
int32_t CDetailDoodad::s_bPoolsDirty;

// STATIC: 0x00E743A4
uint32_t CDetailDoodad::s_idxBufItems;

// STATIC: 0x00E743A8
uint32_t CDetailDoodad::s_vtxBufItems;

// STATIC: 0x00E743AC
uint32_t CDetailDoodad::s_vtxSize;

// STATIC: 0x00E743B0
CGxTex* CDetailDoodad::s_alphaRampTexture;

// STATIC: 0x00E743B4
TSObjectAllocPool<CDetailDoodadInst>* CDetailDoodad::s_instHeap;

// STATIC: 0x00E743B8
TSObjectAllocPool<CDetailDoodadInstBatch>* CDetailDoodad::s_batchHeap;

// STATIC: 0x00E743BC
uint32_t CDetailDoodad::s_currentFrame;

// STATIC: 0x00E743C0
int32_t CDetailDoodad::s_bUseShaders;

// STATIC: 0x00E743C4
int32_t CDetailDoodad::s_bUseInstancing;

// STATIC: 0x00E743C8
TSHashTable<DDMaterial, HASHKEY_NONE> CDetailDoodad::s_materials;

// STATIC: 0x00E743F4
TSGrowableArray<DDMaterial*> CDetailDoodad::s_frameMaterials;

// STATIC: 0x00E74404
TSGrowableArray<CDetailDoodadData*> CDetailDoodad::s_doodadList;

// STATIC: 0x00E74418
CDetailDoodad::SShaderConstants CDetailDoodad::s_shaderConstants;

// STATIC: 0x00E74618
static float s_windTime;

// INLINED
bool DDMaterialSort::operator()(const DDMaterial* a, const DDMaterial* b) const {
    return a->m_texture < b->m_texture;
}

// PROVEN: b0462dbf IDENTICAL/NONE
// OFFSET: 0x00736270
void CDetailDoodad::DestroyPools() {
    CDetailDoodad::s_bPoolsDirty = 1;
}

// PROVEN: 84646dc8 IDENTICAL/NONE
// OFFSET: 0x00736270
void CDetailDoodad::DirtyPools() {
    CDetailDoodad::s_bPoolsDirty = 1;
}

// PROVEN: f70001f8 IDENTICAL/NONE
// OFFSET: 0x00736280/0x140412930
int32_t CDetailDoodad::IsInstanced() {
    return CDetailDoodad::s_bUseInstancing;
}

// PROVEN: 036398a5 IDENTICAL/NONE
// OFFSET: 0x00736290
void CDetailDoodad::ResetVisibility() {
    CDetailDoodad::s_currentFrame++;
}

// PROVEN: 788d40d7 IDENTICAL/NONE
// OFFSET: 0x007362A0
void CDetailDoodad::GetMinMaxDist(float& minDist, float& maxDist) {
    minDist = 0.0f;
    maxDist = CDetailDoodad::s_bUseInstancing ? 500.0f : 140.0f;
}

// PROVEN: ba3241da IDENTICAL/NONE
// OFFSET: 0x007362E0
void CDetailDoodad::GetMinMaxDensity(uint32_t& minDensity, uint32_t& maxDensity) {
    minDensity = 16;
    maxDensity = CDetailDoodad::s_bUseInstancing ? 256 : 64;
}

// PROVEN: a317a4ce IDENTICAL/NONE
// OFFSET: 0x00736710/0x140412C60
void CDetailDoodad::LinkToAsyncHead(uint32_t id) {
    CDetailDoodadData* data = CDetailDoodad::s_doodadList[id];

    if (data && data->m_model) {
        data->m_model->LinkToAsyncHead();
    }
}

// OFFSET: 0x00736740/0x140412C90
void CDetailDoodad::AllocGxBufBatch(uint32_t vertexCount, CGxBuf*& vertexBuf, uint32_t indexCount, CGxBuf*& indexBuf) {
    CGxPool* vertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, static_cast<EGxPoolUsage>(World::worldPoolUsage), CDetailDoodad::s_vtxBufItems * CDetailDoodad::s_vtxSize, static_cast<EGxPoolHintBits>(0), "CDetailDoodad_vtx");
    vertexBuf = g_theGxDevicePtr->BufCreate(vertexPool, CDetailDoodad::s_vtxSize, vertexCount, 0);
    CGxPool* indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, static_cast<EGxPoolUsage>(World::worldPoolUsage), 2 * CDetailDoodad::s_idxBufItems, static_cast<EGxPoolHintBits>(0), "CDetailDoodad_idx");
    indexBuf = g_theGxDevicePtr->BufCreate(indexPool, 2, indexCount, 0);
}

// OFFSET: 0x007367D0
void CDetailDoodad::FreeGxBufBatch(CGxBuf*& vertexBuf, CGxBuf*& indexBuf) {
    CGxPool* vertexPool = vertexBuf->m_pool;
    GxBufDestroy(vertexBuf);

    if (vertexPool) {
        g_theGxDevicePtr->PoolDestroy(vertexPool);
    }

    vertexBuf = NULL;
    CGxPool* indexPool = indexBuf->m_pool;
    GxBufDestroy(indexBuf);

    if (indexPool) {
        g_theGxDevicePtr->PoolDestroy(indexPool);
    }

    indexBuf = NULL;
}

// OFFSET: 0x00736880
void CDetailDoodad::FreeGxBuf(CGxBuf*& buf) {
    CGxPool* pool = buf->m_pool;
    GxBufDestroy(buf);

    if (pool) {
        g_theGxDevicePtr->PoolDestroy(pool);
    }

    buf = NULL;
}

// OFFSET: 0x007368E0/0x140412EE0
void CDetailDoodad::AllocGxBufMat(DDMaterial* mat) {
    CGxPool* vertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, static_cast<EGxPoolUsage>(0), CDetailDoodad::s_vtxSize * mat->m_nVertices, static_cast<EGxPoolHintBits>(0), "DetailDoodad_matVtx");
    mat->m_vertexBuf = g_theGxDevicePtr->BufCreate(vertexPool, CDetailDoodad::s_vtxSize, mat->m_nVertices, 0);
    CGxPool* indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, static_cast<EGxPoolUsage>(0), 2 * mat->m_nIndices, static_cast<EGxPoolHintBits>(0), "DetailDoodad_matIdx");
    mat->m_indexBuf = g_theGxDevicePtr->BufCreate(indexPool, 2, mat->m_nIndices, 0);
}

// OFFSET: 0x00736960
void CDetailDoodad::FreeGxBufMat(DDMaterial* mat) {
    if (mat->m_vertexBuf) {
        CGxPool* pool = mat->m_vertexBuf->m_pool;
        GxBufDestroy(mat->m_vertexBuf);

        if (pool) {
            g_theGxDevicePtr->PoolDestroy(pool);
        }

        mat->m_vertexBuf = NULL;
    }

    if (mat->m_indexBuf) {
        CGxPool* pool = mat->m_indexBuf->m_pool;
        GxBufDestroy(mat->m_indexBuf);

        if (pool) {
            g_theGxDevicePtr->PoolDestroy(pool);
        }

        mat->m_indexBuf = NULL;
    }
}

// PROVEN: 3fa6d4de IDENTICAL/NONE
// OFFSET: 0x00736A10/0x140413080
void CDetailDoodad::SetInstanced(int32_t instanced) {
    CDetailDoodad::s_bUseInstancing = g_theGxDevicePtr->m_caps.unk_0304 && instanced ? 1 : 0;
    CDetailDoodad::s_bPoolsDirty = 1;
}

// OFFSET: 0x00736A50
void CDetailDoodad::UpdateWind() {
    s_windTime = static_cast<float>(fmod(FrameTime::GetTickTimeMs() * 0.001 + s_windTime, 31.415927410125732));
    CDetailDoodad::s_shaderConstants.m_wind.x = s_windTime;
    CDetailDoodad::s_shaderConstants.m_wind.y = 0.059999999f;
    CDetailDoodad::s_shaderConstants.m_wind.w = 0.0f;
    CDetailDoodad::s_shaderConstants.m_wind.z = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 31, &CDetailDoodad::s_shaderConstants.m_wind.x, 1);
}

// OFFSET: 0x00736AF0/0x140413170
void CDetailDoodad::CreateAlphaRampTexture(const void*& texels) {
    static CImVector s_texels[8][64];

    for (uint32_t i = 0; i < 64; i++) {
        CImVector color(0xFF, 0xFF, 0xFF, static_cast<uint8_t>(i * 4));

        for (uint32_t y = 0; y < 8; y++) {
            s_texels[y][63 - i] = color;
        }
    }

    texels = s_texels;
}

// OFFSET: 0x00736BB0/0x140413260
void CDetailDoodad::UpdateAlphaRampTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    switch (command) {
        case GxTex_Lock:
            break;

        case GxTex_Latch:
            CDetailDoodad::CreateAlphaRampTexture(texels);
            texelStrideInBytes = 4 * width;
            break;

        case GxTex_Unlock:
            break;

        case GxTex_3:
            break;
    }
}

// OFFSET: 0x00736BF0/0x1404132A0
void CDetailDoodadInstBatch::FillInstanceBuf() {
    DetailDoodadInstance* instance = reinterpret_cast<DetailDoodadInstance*>(GxBufLock(this->m_instanceBuf));
    CDetailDoodadData* data = CDetailDoodad::s_doodadList[this->m_id];
    const C3Vector& origin = this->m_chunk->m_position;

    for (int32_t i = 0; i < static_cast<int32_t>(this->m_adds.Count()); i++) {
        const CDetailDoodadInstAdd& add = this->m_adds[i];
        CImVector color = add.m_color;
        GxFormatColor(color);
        CImVector light = add.m_light;
        GxFormatColor(light);
        uint8_t nx = static_cast<uint8_t>(static_cast<int32_t>((add.m_normal.x + 1.0f) * 127.5f));
        uint8_t ny = static_cast<uint8_t>(static_cast<int32_t>((add.m_normal.y + 1.0f) * 127.5f));
        uint8_t nz = static_cast<uint8_t>(static_cast<int32_t>((add.m_normal.z + 1.0f) * 127.5f));
        instance->m_normal[0] = nx;
        instance->m_normal[1] = ny;
        instance->m_normal[2] = nz;
        instance->m_normal[3] = 0;
        instance->m_color = color;
        instance->m_light = light;
        instance->m_position.y = add.m_position.y + origin.y;
        instance->m_position.z = add.m_position.z + origin.z;
        instance->m_position.x = add.m_position.x + origin.x;

        if (data->m_record->m_flags & 0x1) {
            instance->m_up[0] = nx;
            instance->m_up[1] = ny;
            instance->m_up[2] = nz;
        } else {
            instance->m_up[0] = 127;
            instance->m_up[1] = 127;
            instance->m_up[2] = 255;
        }

        instance->m_rotation = static_cast<uint8_t>(static_cast<int32_t>(add.m_rotation * 40.584511f));
        instance->m_scale = static_cast<uint8_t>(static_cast<int32_t>(add.m_scale * 127.5f));
        instance->m_windPhase = static_cast<uint8_t>(static_cast<int32_t>(add.m_windPhase * 40.584511f));
        instance->m_pad[1] = 0;
        instance->m_pad[0] = 0;
        instance++;
    }

    GxBufUnlock(this->m_instanceBuf, 0);
}

// OFFSET: 0x00736F60
void CDetailDoodad::DestroyModels() {
    for (uint32_t i = 0; i < CDetailDoodad::s_doodadList.Count(); i++) {
        CDetailDoodadData* data = CDetailDoodad::s_doodadList[i];

        if (data) {
            if (data->m_texture) {
                TextureRelease(data->m_texture);
                data->m_texture = NULL;
            }

            if (data->m_model) {
                data->m_model->Release();
                data->m_model = NULL;
            }

            data->m_record = NULL;
            delete data;
        }

        CDetailDoodad::s_doodadList[i] = NULL;
    }
}

// OFFSET: 0x00736FE0
void CDetailDoodad::SetVertexShader(int32_t pointLights, uint32_t shadowMode) {
    uint32_t shadows = shadowMode > 1 ? 1 : shadowMode;
    int32_t lights = pointLights > 1 ? 1 : pointLights;
    GxRsSet(GxRs_VertexShader, CDetailDoodad::s_vertexShader, (CDetailDoodad::s_bUseInstancing != 0) + 2 * (shadows + 2 * lights));
}

// INLINED
void CDetailDoodad::SetPixelShader(uint32_t shadowMode) {
    GxRsSet(GxRs_PixelShader, CDetailDoodad::s_pixelShader, shadowMode + 4 * ((World::GetEnables() >> 16) & 0x1));
}

// OFFSET: 0x007373E0
void CDetailDoodad::InitializeShaderConstants(CWorldView* view) {
    SShaderConstants& constants = CDetailDoodad::s_shaderConstants;
    memset(&constants, 0, sizeof(constants));
    constants.m_world.d3 = 1.0f;
    constants.m_world.c2 = 1.0f;
    constants.m_world.b1 = 1.0f;
    constants.m_world.a0 = 1.0f;
    constants.m_world.d2 = 0.0f;
    constants.m_world.d1 = 0.0f;
    constants.m_world.d0 = 0.0f;
    constants.m_world.c3 = 0.0f;
    constants.m_world.c1 = 0.0f;
    constants.m_world.c0 = 0.0f;
    constants.m_world.b3 = 0.0f;
    constants.m_world.b2 = 0.0f;
    constants.m_world.b0 = 0.0f;
    constants.m_world.a3 = 0.0f;
    constants.m_world.a2 = 0.0f;
    constants.m_world.a1 = 0.0f;
    constants.m_worldView = view->m_viewMat;
    constants.m_view = view->m_viewMat;
    constants.m_projection = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        constants.m_projection.c0 = constants.m_projection.c0 * -1.0f;
        constants.m_projection.c1 = constants.m_projection.c1 * -1.0f;
        constants.m_projection.c2 = constants.m_projection.c2 * -1.0f;
        constants.m_projection.c3 = constants.m_projection.c3 * -1.0f;
    }

    C3Vector sunDir(0.0f, 0.0f, 0.0f);

    if (CWorldScene::s_lighting.GetSunLight(sunDir, constants.m_sunAmbient, constants.m_sunDiffuse, constants.m_sunSpecular)) {
        C3Vector dir = -sunDir * C33Matrix(view->m_viewMat);
        constants.m_sunDir = C4Vector(dir.x, dir.y, dir.z, 1.0f);
    } else {
        constants.m_sunDir = C4Vector(0.0f, 0.0f, 0.0f, 1.0f);
        constants.m_sunDiffuse = C3Vector(0.0f, 0.0f, 0.0f);
        constants.m_sunDiffuseW = 0.0f;
        constants.m_sunAmbient = C3Vector(1.0f, 1.0f, 1.0f);
        constants.m_sunAmbientW = 1.0f;
        constants.m_sunSpecular = C3Vector(0.0f, 0.0f, 0.0f);
    }

    constants.m_sunSpecularW = 0.0f;
    C3Vector lightPos(0.0f, 0.0f, 0.0f);

    for (uint32_t i = 0; i < 3; i++) {
        SShaderConstants::SPointLight& light = constants.m_pointLights[i];

        if (CWorldScene::s_lighting.GetPointLight(i, lightPos, light.m_color, light.m_attenuation)) {
            C3Vector pos = lightPos - view->m_camPos;
            light.m_position = C4Vector(pos.x, pos.y, pos.z, 1.0f);
        } else {
            light.m_position = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
            light.m_color = C3Vector(0.0f, 0.0f, 0.0f);
            light.m_colorW = 0.0f;
            light.m_attenuation = C3Vector(1.0f, 0.0f, 0.0f);
            light.m_attenuationW = 0.0f;
        }
    }

    if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
        const DayNight::FogInfo* fog = DayNight::GetFogInfo();
        CImVector fogColor = fog->m_color;
        float scale = 1.0f / (fog->m_end - fog->m_start);
        constants.m_fog = C4Vector(-(scale * CShaderEffect::s_fogMul), scale * fog->m_end, fog->m_density, 0.0f);

        if (g_theGxDevicePtr->m_caps.unk_02F8) {
            GxRsSet(GxRs_FogColor, *reinterpret_cast<const int32_t*>(&fogColor));
        } else {
            C4Vector color(fogColor);
            color.w = static_cast<float>(World::detailDoodadAlphaRef) * 0.0039215689f;
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, &color.x, 1);
        }

        if (g_theGxDevicePtr->m_caps.unk_02FC) {
            GxRsSet(GxRs_Fog, 1);
        }
    } else {
        constants.m_fog = C4Vector(0.0f, 1.0f, 1.0f, 0.0f);

        if (g_theGxDevicePtr->m_caps.unk_02FC) {
            GxRsSet(GxRs_Fog, 0);
        }
    }

    C4Vector params = ViewSettings::CalculateParams();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 22, &params.x, 1);
    float fadeScale = 1.0f / (World::detailDoodadDist - World::detailDoodadDist * 0.85000002f);
    constants.m_fade.y = fadeScale * World::detailDoodadDist;
    constants.m_fade.x = fadeScale * -1.0f;
    constants.m_fade.z = 0.0f;
    constants.m_fade.w = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &constants.m_projection.a0, 32);
}

// OFFSET: 0x00737A80
static C33Matrix BuildInstanceMatrix(const C3Vector& normal, float angle) {
    float ty = normal.z;
    float tz = -normal.y;
    float scale = 1.0f / sqrt(ty * ty + tz * tz);
    C3Vector tangent(scale * 0.0f, ty * scale, tz * scale);
    C3Vector binormal(normal.y * tangent.z - normal.z * tangent.y, normal.z * tangent.x - normal.x * tangent.z, normal.x * tangent.y - normal.y * tangent.x);
    float c = cos(angle);
    float s = sin(angle);
    return C33Matrix(binormal.x * c + tangent.x * s, binormal.y * c + tangent.y * s, binormal.z * c + tangent.z * s, -s * binormal.x + tangent.x * c, binormal.y * -s + tangent.y * c, binormal.z * -s + tangent.z * c, 0.0f, 0.0f, normal.z);
}

// OFFSET: 0x00737C30
void CDetailDoodadInstBatch::FillIndices() {
    uint16_t* indices = reinterpret_cast<uint16_t*>(GxBufLock(this->m_indexBuf));
    uint32_t index = 0;
    uint16_t base = 0;

    for (int32_t i = 0; i < static_cast<int32_t>(this->m_adds.Count()); i++) {
        CM2Model* model = CDetailDoodad::s_doodadList[this->m_adds[i].m_doodadId]->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        M2SkinProfile* skin = model->m_shared->m_skinData;

        for (uint32_t j = 0; j < skin->m_indices.m_count; j++) {
            indices[index++] = base + skin->m_indices[j];
        }

        base += skin->m_vertices.m_count;
    }

    GxBufUnlock(this->m_indexBuf, 0);
}

// OFFSET: 0x00737CF0
void CDetailDoodad::FillIndicesInstanced(DDMaterial* mat) {
    uint16_t* indices = reinterpret_cast<uint16_t*>(GxBufLock(mat->m_indexBuf));
    CM2Model* model = CDetailDoodad::s_doodadList[mat->m_id]->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    M2SkinProfile* skin = model->m_shared->m_skinData;

    for (uint32_t i = 0; i < skin->m_indices.m_count; i++) {
        *indices++ = skin->m_indices[i];
    }

    GxBufUnlock(mat->m_indexBuf, 0);
}

// OFFSET: 0x00737FF0
void CDetailDoodad::InitializeRenderState(CWorldView* view) {
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxRsSet(GxRs_AlphaRef, static_cast<uint8_t>(World::detailDoodadAlphaRef));
    GxRsSet(GxRs_DepthWrite, 1);

    if (CDetailDoodad::s_bUseShaders) {
        CDetailDoodad::SetVertexShader(CWorldScene::s_lighting.m_lightCount != 0, view->m_shadowMode);
        CDetailDoodad::SetPixelShader(view->m_shadowMode);
        CDetailDoodad::InitializeShaderConstants(view);
        CShadowCache::SetShadowMapDetailDoodad();
        return;
    }

    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<const int32_t*>(&white));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, CDetailDoodad::s_alphaRampTexture);
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
    GxRsSet(GxRs_TexGen1, 3);
    GxRsSet(GxRs_TextureShader1, 1);
    C44Matrix texXform;
    texXform.Scale(1.0f / (World::detailDoodadDist * 0.15000001f));
    texXform.RotateAroundY(1.5707964f);
    texXform.Translate(C3Vector(0.0f, 0.0f, World::detailDoodadDist * -0.85000002f));
    GxXformSet(GxXform_Tex1, texXform);
    CWorldScene::s_lighting.SetupGxLights(&view->m_camPos);
    CWorldScene::s_lighting.SetupGxFog();
}

// PROVEN: 8d2bd5d2 IDENTICAL/NONE
// OFFSET: 0x00738390/0x140415130
void CDetailDoodadData::CheckGeometry() {
    CM2Model* model = this->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    M2Data* data = model->m_shared->m_data;
    this->m_texture = CMap::LoadTexture(data->m_textures[0].m_filename.Data());
    model = this->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    M2SkinProfile* skin = model->m_shared->m_skinData;
    this->m_windScale = 0.0f;

    for (uint32_t i = 0; i < skin->m_vertices.m_count; i++) {
        if (data->m_vertices[skin->m_vertices[i]].m_position.z > 0.5f) {
            this->m_windScale = 1.0f;
            return;
        }
    }
}

// OFFSET: 0x00738440
void CDetailDoodadInstBatch::Free() {
    this->m_id = 0xFFFF;
    this->m_texture = NULL;
    this->m_count = 0;
    this->m_vertexCount = 0;
    this->m_indexCount = 0;
    this->m_adds.SetCount(0);
    this->m_adds.m_chunk = 64;

    if (this->m_vertexBuf) {
        CDetailDoodad::FreeGxBufBatch(this->m_vertexBuf, this->m_indexBuf);
    }

    if (this->m_instanceBuf) {
        CDetailDoodad::FreeGxBuf(this->m_instanceBuf);
    }

    this->m_materialLink.Unlink();
    this->m_instLink.Unlink();
}

// OFFSET: 0x00738510
void CDetailDoodadInstBatch::FillVertices() {
    DetailDoodadVertex* vertex = reinterpret_cast<DetailDoodadVertex*>(GxBufLock(this->m_vertexBuf));
    C3Vector pos(0.0f, 0.0f, 0.0f);
    C33Matrix rotations[4];
    uint8_t built = 0;
    uint16_t lastTri = 0xFFFF;

    for (int32_t i = 0; i < static_cast<int32_t>(this->m_adds.Count()); i++) {
        const CDetailDoodadInstAdd& add = this->m_adds[i];
        CDetailDoodadData* doodadData = CDetailDoodad::s_doodadList[add.m_doodadId];
        CM2Model* model = doodadData->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        M2Data* data = model->m_shared->m_data;
        model = doodadData->m_model;

        if (!(model->m_fFlags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        M2SkinProfile* skin = model->m_shared->m_skinData;
        uint32_t count = skin->m_vertices.m_count;
        CImVector color = add.m_color;
        GxFormatColor(color);

        if (!CDetailDoodad::s_bUseShaders) {
            color.r = static_cast<uint8_t>((color.r * (76 * color.a + 45874)) >> 16);
            color.g = static_cast<uint8_t>((color.g * (76 * color.a + 45874)) >> 16);
            color.b = static_cast<uint8_t>((color.b * (76 * color.a + 45874)) >> 16);
            color.a = 0xFF;
        }

        CImVector light = add.m_light;
        GxFormatColor(light);

        if (doodadData->m_record->m_flags & 0x1) {
            uint16_t tri = add.m_triIndex;

            if ((tri & 0xFC) != lastTri) {
                lastTri = tri & 0xFC;
                built = 0;
            }

            uint32_t corner = tri & 0x3;

            if (!(built & (1 << corner))) {
                rotations[corner] = BuildInstanceMatrix(add.m_normal, add.m_rotation);
                built |= 1 << corner;
            }

            const C33Matrix& rotation = rotations[corner];

            for (uint32_t j = 0; j < count; j++) {
                const M2Vertex& source = data->m_vertices[skin->m_vertices[j]];
                pos = source.m_position * rotation;
                vertex->m_position.x = pos.x * add.m_scale + add.m_position.x;
                vertex->m_position.y = pos.y * add.m_scale + add.m_position.y;
                vertex->m_position.z = add.m_position.z + pos.z * add.m_scale;
                vertex->m_normal = add.m_normal;
                vertex->m_color = color;
                vertex->m_light = light;
                vertex->m_texCoord = source.m_texcoord[0];
                float wind = source.m_position.z * 0.66666669f;

                if (wind > 1.0f || wind >= 0.0f) {
                    if (wind > 1.0f) {
                        wind = 1.0f;
                    }
                } else {
                    wind = 0.0f;
                }

                vertex->m_wind = doodadData->m_windScale * wind;
                vertex->m_windPhase = add.m_windPhase;
                vertex++;
            }
        } else {
            float c = cos(add.m_rotation);
            float s = sin(add.m_rotation);

            for (uint32_t j = 0; j < count; j++) {
                const M2Vertex& source = data->m_vertices[skin->m_vertices[j]];
                vertex->m_position.x = (c * source.m_position.x - source.m_position.y * s) * add.m_scale + add.m_position.x;
                vertex->m_position.y = (source.m_position.y * c + s * source.m_position.x) * add.m_scale + add.m_position.y;
                vertex->m_position.z = source.m_position.z * add.m_scale + add.m_position.z;
                vertex->m_normal = add.m_normal;
                vertex->m_color = color;
                vertex->m_light = light;
                vertex->m_texCoord = source.m_texcoord[0];
                float wind = source.m_position.z * 0.66666669f;

                if (wind > 1.0f || wind >= 0.0f) {
                    if (wind > 1.0f) {
                        wind = 1.0f;
                    }
                } else {
                    wind = 0.0f;
                }

                vertex->m_wind = doodadData->m_windScale * wind;
                vertex->m_windPhase = add.m_windPhase;
                vertex++;
            }
        }
    }

    GxBufUnlock(this->m_vertexBuf, 0);
}

// OFFSET: 0x00738F70
void CDetailDoodad::FillVerticesInstanced(DDMaterial* mat) {
    DetailDoodadInstancedVertex* vertex = reinterpret_cast<DetailDoodadInstancedVertex*>(GxBufLock(mat->m_vertexBuf));
    CDetailDoodadData* doodadData = CDetailDoodad::s_doodadList[mat->m_id];
    CM2Model* model = doodadData->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    M2Data* data = model->m_shared->m_data;
    model = doodadData->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    M2SkinProfile* skin = model->m_shared->m_skinData;

    for (uint32_t i = 0; i < skin->m_vertices.m_count; i++) {
        const M2Vertex& source = data->m_vertices[skin->m_vertices[i]];
        vertex->m_position = source.m_position;
        vertex->m_texCoord = source.m_texcoord[0];
        float wind = source.m_position.z * 0.66666669f;

        if (wind > 1.0f || wind >= 0.0f) {
            if (wind > 1.0f) {
                wind = 1.0f;
            }
        } else {
            wind = 0.0f;
        }

        float scaled = doodadData->m_windScale * wind;
        vertex->m_wind[0] = scaled;
        vertex->m_wind[1] = scaled;
        vertex++;
    }

    GxBufUnlock(mat->m_vertexBuf, 0);
}

// OFFSET: 0x007392A0/0x140415C70
void CDetailDoodadInst::FlagVisible(float dist) {
    if (CDetailDoodad::s_bUseInstancing) {
        for (CDetailDoodadInstBatch* batch = this->m_batches.Head(), * next; batch; batch = next) {
            next = this->m_batches.Next(batch);

            if (batch->m_material && dist < batch->m_material->m_fadeDist) {
                batch->m_material->m_batches.LinkToTail(batch);
            }
        }
    } else {
        for (CDetailDoodadInstBatch* batch = this->m_batches.Head(), * next; batch; batch = next) {
            next = this->m_batches.Next(batch);

            if (batch->m_material) {
                batch->m_material->m_batches.LinkToTail(batch);
            }
        }
    }
}

// PROVEN: 43a4b1fd IDENTICAL/NONE
// OFFSET: 0x00739470/0x140415F90
void CDetailDoodadData::ModelLoadedCallback(CM2Model* model, void* arg) {
    static_cast<CDetailDoodadData*>(arg)->CheckGeometry();
}

// OFFSET: 0x00739480
CDetailDoodadInstBatch::~CDetailDoodadInstBatch() {
    this->Free();
}

// OFFSET: 0x007395C0
void CDetailDoodad::UpdatePools() {
    if (!CDetailDoodad::s_bPoolsDirty) {
        return;
    }

    CWorldMap::ClearAllDetailDoodads();

    for (DDMaterial* mat = CDetailDoodad::s_materials.Head(), * next; mat; mat = next) {
        next = CDetailDoodad::s_materials.Next(mat);
        CDetailDoodad::FreeGxBufMat(mat);
    }

    CDetailDoodad::s_materials.Clear();

    if (CDetailDoodad::s_bUseInstancing) {
        CDetailDoodad::s_vtxBufItems = 0;
        CDetailDoodad::s_idxBufItems = 0;
        CDetailDoodad::s_bPoolsDirty = 0;
        CDetailDoodad::s_vtxSize = 28;
    } else {
        CDetailDoodad::s_vtxBufItems = World::detailDoodadDensity << 6;
        CDetailDoodad::s_idxBufItems = World::detailDoodadDensity << 6;
        CDetailDoodad::s_vtxSize = 48;

        if (World::detailDoodadDensity << 6 > 0x1000) {
            CDetailDoodad::s_vtxBufItems = 4096;
        }

        if (World::detailDoodadDensity << 6 > 0x1000) {
            CDetailDoodad::s_idxBufItems = 4096;
        }

        CDetailDoodad::s_bPoolsDirty = 0;
    }
}

// OFFSET: 0x007396E0/0x140416350
int32_t CDetailDoodadData::Load() {
    if (this->m_model) {
        return 1;
    }

    char fileName[260];
    strcpy(fileName, "World\\NoDXT\\Detail\\");
    strcat(fileName, this->m_record->m_doodadpath);
    this->m_model = CWorldScene::s_m2Scene->CreateModel(fileName, 0, 3);

    if (!this->m_model) {
        return 0;
    }

    this->m_model->SetLoadedCallback(CDetailDoodadData::ModelLoadedCallback, this);
    return 1;
}

// OFFSET: 0x00739980/0x1404166B0
void CDetailDoodad::Initialize() {
    CDetailDoodad::s_currentFrame = 0;
    void* instMem = SMemAlloc(sizeof(TSObjectAllocPool<CDetailDoodadInst>), __FILE__, __LINE__, 0);
    CDetailDoodad::s_instHeap = instMem ? new (instMem) TSObjectAllocPool<CDetailDoodadInst>("WDETAILDOODADINST", 32) : NULL;
    void* batchMem = SMemAlloc(sizeof(TSObjectAllocPool<CDetailDoodadInstBatch>), __FILE__, __LINE__, 0);
    CDetailDoodad::s_batchHeap = batchMem ? new (batchMem) TSObjectAllocPool<CDetailDoodadInstBatch>("WDETAILDOODADBATCH", 64) : NULL;
    uint32_t count = g_groundEffectDoodadDB.m_maxID + 1;
    CDetailDoodad::s_doodadList.SetCount(count);

    for (uint32_t i = 0; i < CDetailDoodad::s_doodadList.Count(); i++) {
        CDetailDoodad::s_doodadList[i] = NULL;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        const GroundEffectDoodadRec* rec = g_groundEffectDoodadDB.GetRecord(i);

        if (rec) {
            void* mem = SMemAlloc(sizeof(CDetailDoodadData), __FILE__, __LINE__, 0);
            CDetailDoodadData* data = mem ? new (mem) CDetailDoodadData() : NULL;
            data->m_record = rec;
            CDetailDoodad::s_doodadList[i] = data;
        }
    }

    CGxTexFlags flags;
    GxTexCreate(64, 8, GxTex_Argb8888, flags, NULL, CDetailDoodad::UpdateAlphaRampTexture, CDetailDoodad::s_alphaRampTexture);
    CDetailDoodad::s_vtxSize = 0;
    CDetailDoodad::s_vtxBufItems = 0;
    CDetailDoodad::s_idxBufItems = 0;
    CDetailDoodad::s_bPoolsDirty = 1;
    CDetailDoodad::s_bUseShaders = 0;
    CDetailDoodad::s_vertexShader = NULL;
    CDetailDoodad::s_pixelShader = NULL;

    if (g_theGxDevicePtr->m_caps.m_shaderModel >= 4) {
        CDetailDoodad::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DetailDoodad");
        CDetailDoodad::s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DetailDoodad");
        CDetailDoodad::s_bUseShaders = 1;
    }
}

// OFFSET: 0x00739BD0/0x1404169D0
int32_t CDetailDoodad::LoadModel(uint32_t id) {
    CDetailDoodadData* data = CDetailDoodad::s_doodadList[id];

    if (!data) {
        return 0;
    }

    if (!data->m_model) {
        data->Load();
        return 0;
    }

    if (!data->m_model->IsLoaded()) {
        return 0;
    }

    if (!data->m_model->TexturesAllLoaded()) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x00739CC0
void CDetailDoodad::Destroy() {
    CDetailDoodad::DestroyModels();
    CDetailDoodad::s_bPoolsDirty = 1;

    for (DDMaterial* mat = CDetailDoodad::s_materials.Head(), * next; mat; mat = next) {
        next = CDetailDoodad::s_materials.Next(mat);
        CDetailDoodad::FreeGxBufMat(mat);
    }

    CDetailDoodad::s_materials.Clear();

    if (CDetailDoodad::s_alphaRampTexture) {
        GxTexDestroy(CDetailDoodad::s_alphaRampTexture);
    }

    CDetailDoodad::s_alphaRampTexture = NULL;
    delete CDetailDoodad::s_instHeap;
    CDetailDoodad::s_instHeap = NULL;
    delete CDetailDoodad::s_batchHeap;
    CDetailDoodad::s_batchHeap = NULL;

    if (CDetailDoodad::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(CDetailDoodad::s_vertexShader);
        CDetailDoodad::s_vertexShader = NULL;
    }

    if (CDetailDoodad::s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(CDetailDoodad::s_pixelShader);
        CDetailDoodad::s_pixelShader = NULL;
    }
}

// OFFSET: 0x00739E70
void CDetailDoodadInst::Free() {
    for (CDetailDoodadInstBatch* batch = this->m_batches.Head(), * next; batch; batch = next) {
        next = this->m_batches.Next(batch);
        batch->Free();
        CDetailDoodad::s_batchHeap->Delete(batch, __FILE__, __LINE__);
    }
}

// OFFSET: 0x00739EE0/0x140416D00
void CDetailDoodad::RenderInstanced(CWorldView* view) {
    for (DDMaterial* mat = CDetailDoodad::s_materials.Head(), * next; mat; mat = next) {
        next = CDetailDoodad::s_materials.Next(mat);

        if (mat->m_batches.Head()) {
            *CDetailDoodad::s_frameMaterials.New() = mat;
        }
    }

    if (!CDetailDoodad::s_frameMaterials.Count()) {
        return;
    }

    blz::sort(CDetailDoodad::s_frameMaterials.m_data, CDetailDoodad::s_frameMaterials.m_data + CDetailDoodad::s_frameMaterials.Count(), DDMaterialSort());

    for (uint32_t i = 0; i < CDetailDoodad::s_frameMaterials.Count(); i++) {
        DDMaterial* mat = CDetailDoodad::s_frameMaterials[i];
        CGxTex* tex = TextureGetGxTex(mat->m_texture);

        if (!tex) {
            continue;
        }

        g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);

        if (!mat->m_vertexBuf) {
            CDetailDoodad::AllocGxBufMat(mat);
        }

        if (!mat->m_vertexBuf->unk_001C || !mat->m_vertexBuf->unk_001D) {
            CDetailDoodad::FillVerticesInstanced(mat);
        }

        if (!mat->m_indexBuf->unk_001C || !mat->m_indexBuf->unk_001D) {
            CDetailDoodad::FillIndicesInstanced(mat);
        }

        g_theGxDevicePtr->PrimIndexPtr(mat->m_indexBuf);
        CDetailDoodad::s_shaderConstants.m_fade.x = -mat->m_fadeScale;
        CDetailDoodad::s_shaderConstants.m_fade.y = mat->m_fadeDist * mat->m_fadeScale;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 5, &CDetailDoodad::s_shaderConstants.m_fade.x, 1);

        for (CDetailDoodadInstBatch* batch = mat->m_batches.Head(), * nextBatch; batch; batch = nextBatch) {
            nextBatch = mat->m_batches.Next(batch);
            batch->m_materialLink.Unlink();
            uint32_t count = batch->m_count;

            if (!batch->m_instanceBuf) {
                CGxPool* pool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, static_cast<EGxPoolUsage>(World::worldPoolUsage), 32 * count, static_cast<EGxPoolHintBits>(0), "CDetailDoodad_const");
                batch->m_instanceBuf = g_theGxDevicePtr->BufCreate(pool, 32, count, 0);
            }

            if (!batch->m_instanceBuf->unk_001C || !batch->m_instanceBuf->unk_001D) {
                batch->FillInstanceBuf();
            }

            CGxBuf* bufs[2];
            bufs[0] = mat->m_vertexBuf;
            bufs[1] = batch->m_instanceBuf;
            EGxVertexBufferFormat formats[2];
            formats[0] = static_cast<EGxVertexBufferFormat>(14);
            formats[1] = static_cast<EGxVertexBufferFormat>(20);
            g_theGxDevicePtr->PrimVertexPtrs(bufs, formats, static_cast<EGxVertexBufferFormat>(21));
            g_theGxDevicePtr->SetInstanceCount(count);
            CGxBatch drawBatch;
            drawBatch.m_primType = GxPrim_Triangles;
            drawBatch.m_start = 0;
            drawBatch.m_count = mat->m_nIndices;
            drawBatch.m_minIndex = 0;
            drawBatch.m_maxIndex = mat->m_nVertices - 1;
            g_theGxDevicePtr->DrawInstanced(drawBatch);
        }
    }

    CDetailDoodad::s_frameMaterials.SetCount(0);
    g_theGxDevicePtr->ResetInstancing();
}

// OFFSET: 0x0073A240
void CDetailDoodad::Render(CWorldView* view) {
    if (CDetailDoodad::s_bUseInstancing) {
        CDetailDoodad::RenderInstanced(view);
        return;
    }

    for (DDMaterial* mat = CDetailDoodad::s_materials.Head(), * next; mat; mat = next) {
        next = CDetailDoodad::s_materials.Next(mat);

        if (!mat->m_batches.Head()) {
            continue;
        }

        CGxTex* tex = TextureGetGxTex(mat->m_texture);

        if (!tex) {
            continue;
        }

        g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);

        for (CDetailDoodadInstBatch* batch = mat->m_batches.Head(), * nextBatch; batch; batch = nextBatch) {
            nextBatch = mat->m_batches.Next(batch);
            batch->m_materialLink.Unlink();
            C44Matrix world;

            if (CDetailDoodad::s_bUseShaders) {
                world.Translate(batch->m_chunk->m_position);
                CDetailDoodad::s_shaderConstants.m_world = world;
                CDetailDoodad::s_shaderConstants.m_worldView = world * view->m_viewMat;
                g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 19, &CDetailDoodad::s_shaderConstants.m_world.a0, 12);
            } else {
                C3Vector move(batch->m_chunk->m_position.x - view->m_camPos.x, batch->m_chunk->m_position.y - view->m_camPos.y, batch->m_chunk->m_position.z - view->m_camPos.z);
                world.Translate(move);
                GxXformSet(GxXform_World, world);
            }

            if (!batch->m_vertexBuf || batch->m_vertexBuf->m_pool->m_usage == GxPoolUsage_Stream) {
                CDetailDoodad::AllocGxBufBatch(batch->m_vertexCount, batch->m_vertexBuf, batch->m_indexCount, batch->m_indexBuf);
            }

            if (!batch->m_vertexBuf->unk_001C || !batch->m_vertexBuf->unk_001D) {
                batch->FillVertices();
            }

            if (!batch->m_indexBuf->unk_001C || !batch->m_indexBuf->unk_001D) {
                batch->FillIndices();
            }

            g_theGxDevicePtr->PrimVertexPtr(batch->m_vertexBuf, static_cast<EGxVertexBufferFormat>(19));
            g_theGxDevicePtr->PrimIndexPtr(batch->m_indexBuf);
            CGxBatch drawBatch;
            drawBatch.m_primType = GxPrim_Triangles;
            drawBatch.m_start = 0;
            drawBatch.m_count = batch->m_indexCount;
            drawBatch.m_minIndex = 0;
            drawBatch.m_maxIndex = batch->m_vertexCount - 1;
            g_theGxDevicePtr->Draw(drawBatch);
        }
    }
}

// OFFSET: 0x0073A940
CDetailDoodadInst* CDetailDoodad::AllocInst() {
    return CDetailDoodad::s_instHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x0073A960
CDetailDoodadInst::~CDetailDoodadInst() {
    this->Free();
}

// OFFSET: 0x0073ACE0/0x140418290
void CDetailDoodad::FreeInst(CDetailDoodadInst* inst) {
    CDetailDoodad::s_instHeap->Delete(inst, __FILE__, __LINE__);
}

// OFFSET: 0x0073AD20
void CDetailDoodadInst::AddDoodad(uint32_t id, const C3Vector& position, float rotation, float scale, const C4Plane& plane, uint16_t triIndex, const CImVector& color, const CImVector& light, float windPhase) {
    CDetailDoodadData* data = CDetailDoodad::s_doodadList[id];
    CM2Model* model = data->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    M2SkinProfile* skin = model->m_shared->m_skinData;
    uint32_t nVertices = skin->m_vertices.m_count;
    uint32_t nIndices = skin->m_indices.m_count;
    uint32_t batchCount = 0;
    CDetailDoodadInstBatch* batch;

    if (CDetailDoodad::s_bUseInstancing) {
        for (batch = this->m_batches.Head(); batch; batch = this->m_batches.Next(batch)) {
            if (batch->m_id == id) {
                break;
            }
        }
    } else {
        for (batch = this->m_batches.Head(); batch; batch = this->m_batches.Next(batch)) {
            batchCount++;

            if (batch->m_texture == data->m_texture && nVertices + batch->m_vertexCount <= CDetailDoodad::s_vtxBufItems && nIndices + batch->m_indexCount <= CDetailDoodad::s_idxBufItems) {
                break;
            }
        }

        if (!batch && batchCount >= 4) {
            return;
        }
    }

    if (!batch) {
        batch = CDetailDoodad::s_batchHeap->New(__FILE__, __LINE__);
        batch->m_id = id;
        batch->m_texture = data->m_texture;
        this->m_batches.LinkToHead(batch);
    }

    if (!batch->m_material) {
        uint32_t key = CDetailDoodad::s_bUseInstancing ? id : reinterpret_cast<uint32_t>(data->m_texture);
        DDMaterial* mat = CDetailDoodad::s_materials.Ptr(key, HASHKEY_NONE());

        if (!mat) {
            mat = CDetailDoodad::s_materials.New(key, HASHKEY_NONE(), 0, 0);
            mat->m_id = id;
            mat->m_texture = data->m_texture;
            mat->m_nVertices = nVertices;
            mat->m_vertexBuf = NULL;
            mat->m_indexBuf = NULL;
            mat->m_nIndices = nIndices;

            if (CDetailDoodad::s_bUseInstancing) {
                CM2Model* doodadModel = CDetailDoodad::s_doodadList[id]->m_model;

                if (!(doodadModel->m_fFlags & 0x1)) {
                    doodadModel->WaitForLoad(NULL);
                }

                float fadeDist = doodadModel->m_shared->m_data->m_bounds.m_radius * World::detailDoodadDensityFade * World::detailDoodadDist;

                if (fadeDist >= 120.0f) {
                    if (World::detailDoodadDist <= fadeDist) {
                        fadeDist = World::detailDoodadDist;
                    }
                } else {
                    fadeDist = 120.0f;
                }

                mat->m_fadeDist = fadeDist;
                mat->m_fadeScale = 1.0f / (fadeDist - fadeDist * 0.69999999f);
            }
        }

        batch->m_material = mat;
    }

    if (!batch->m_chunk) {
        batch->m_chunk = this->m_chunk;
    }

    CDetailDoodadInstAdd* add = batch->m_adds.New();
    add->m_doodadId = id;
    add->m_position = position;
    add->m_rotation = rotation;
    add->m_scale = scale;
    add->m_normal = plane.n;
    add->m_triIndex = triIndex;
    add->m_color = color;
    add->m_light = light;
    add->m_windPhase = windPhase;

    if (!CDetailDoodad::s_bUseInstancing) {
        batch->m_vertexCount += nVertices;
        batch->m_indexCount += nIndices;
    }

    batch->m_count++;
}
