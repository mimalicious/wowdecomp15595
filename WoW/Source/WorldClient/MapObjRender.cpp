#include "WoW/Source/WorldClient/MapObjRender.h"
#include <cstring>
#include <new>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FFX/EffectWaterWindow.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00E89658
CShaderEffect* CMapObjRender::s_unifiedEffects[15];

// STATIC: 0x00E89694
CShaderEffect* CMapObjRender::s_effects[15];

// STATIC: 0x00E896D0
FFX::Effect* CMapObjRender::s_submarineEffect;

// STATIC: 0x00E896D4
FFX::Effect* CMapObjRender::s_waterWindowEffect;

// STATIC: 0x00EA96D8
TSHashTable<CSceneMaterial, HASHKEY_MATERIAL> CMapObjRender::s_materialHash;

static float ColorByte(uint8_t value) {
    return static_cast<float>(value) * 0.0039215689f;
}

static void CreateWaterEffects() {
    void* mem = SMemAlloc(sizeof(EffectWaterWindow), __FILE__, __LINE__, 0);
    CMapObjRender::s_waterWindowEffect = mem ? new (mem) EffectWaterWindow(Liquid::GetWaterDetail()) : NULL;
    mem = SMemAlloc(sizeof(EffectSubmarineWindow), __FILE__, __LINE__, 0);
    CMapObjRender::s_submarineEffect = mem ? new (mem) EffectSubmarineWindow(Liquid::GetWaterDetail()) : NULL;
}

static void DestroyWaterEffects() {
    if (CMapObjRender::s_waterWindowEffect) {
        delete CMapObjRender::s_waterWindowEffect;
    }

    if (CMapObjRender::s_submarineEffect) {
        delete CMapObjRender::s_submarineEffect;
    }
}

// PROVEN: cdaa04ca IDENTICAL/NONE
// OFFSET: 0x007797C0/0x140465500
SMOMaterial* CMapObj::GetGroupMaterial(uint32_t groupNum) {
    CMapObjGroup* group = this->GetGroup(groupNum, 0);

    if (!group) {
        return NULL;
    }

    return &this->m_materials[group->m_liquidMaterialId];
}

// OFFSET: 0x007797F0
void CMapObjRender::WaterResetCallback() {
    DestroyWaterEffects();
    CreateWaterEffects();
}

// PROVEN: 06dc38c7 IDENTICAL/NONE
// OFFSET: 0x00779890
bool HASHKEY_MATERIAL::operator==(const HASHKEY_MATERIAL& key) const {
    return this->m_textures[0] == key.m_textures[0] && this->m_textures[1] == key.m_textures[1] && this->m_textures[2] == key.m_textures[2] && this->m_materialFlags == key.m_materialFlags && this->m_ambColor == key.m_ambColor && this->m_color == key.m_color && this->m_pass == key.m_pass && this->m_shader == key.m_shader && this->m_unified == key.m_unified && this->m_blendMode == key.m_blendMode && this->m_cull == key.m_cull && this->m_lighting == key.m_lighting && this->m_interior == key.m_interior && this->m_fog == key.m_fog && this->m_secondVB == key.m_secondVB && this->m_shadow == key.m_shadow;
}

// OFFSET: 0x00779960
float CMapObj::CalcPortalFarthestDistance(const SMOPortalInfo* portal, const C4Plane& plane) {
    float farthest = 0.0f;
    const C3Vector* verts = &this->m_portalVertices[portal->m_startVertex];

    for (uint32_t i = 0; i < portal->m_count; i++) {
        float distance = verts[i].x * plane.n.x + verts[i].y * plane.n.y + verts[i].z * plane.n.z + plane.d;

        if (distance > farthest) {
            farthest = distance;
        }
    }

    return farthest;
}

// OFFSET: 0x00779B00
void CMapObj::CalcPortalCenterInWorldSpace(const SMOPortalInfo* portal, const C44Matrix& matrix, C3Vector& pos) {
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    const C3Vector* verts = &this->m_portalVertices[portal->m_startVertex];

    for (uint32_t i = 0; i < portal->m_count; i++) {
        pos.x = verts[i].x + pos.x;
        pos.y = verts[i].y + pos.y;
        pos.z = pos.z + verts[i].z;
    }

    float scale = 1.0f / static_cast<float>(portal->m_count);
    float y = pos.y * scale;
    float x = pos.x * scale;
    float z = pos.z * scale;
    pos.x = matrix.c0 * z + matrix.b0 * y + matrix.a0 * x + matrix.d0;
    pos.y = matrix.c1 * z + matrix.b1 * y + matrix.a1 * x + matrix.d1;
    pos.z = matrix.c2 * z + matrix.b2 * y + matrix.a2 * x + matrix.d2;
}

// OFFSET: 0x00779C70/0x140465A00
void CMapObjRender::RenderLiquidBatch(FFX::Effect* effect, SMOBatch* batch, CMapObjDefGroup* defGroup, CMapObjGroup* group, uint32_t batchIndex) {
    CGxBatch gxBatch;
    gxBatch.m_primType = GxPrim_Triangles;
    gxBatch.m_start = batch->m_startIndex;
    gxBatch.m_count = batch->m_count;
    gxBatch.m_minIndex = batch->m_minIndex;
    gxBatch.m_maxIndex = batch->m_maxIndex;
    FFX::AddPortalEffect(effect, defGroup->m_mapObjDef->m_mat, group->m_vertices, group->m_normals, group->m_colors, group->m_colors2, group->m_texCoords, &group->m_tangents[group->m_tangentStarts[batchIndex]], group->m_indices, gxBatch);
}

// OFFSET: 0x00779D10/0x140465AD0
void CMapObjRender::Initialize() {
    CMapObjRender::s_effects[0] = CShaderEffectManager::GetEffect("MapObjDiffuse");
    CMapObjRender::s_effects[1] = CShaderEffectManager::GetEffect("MapObjSpecular");
    CMapObjRender::s_effects[2] = CShaderEffectManager::GetEffect("MapObjMetal");
    CMapObjRender::s_effects[3] = CShaderEffectManager::GetEffect("MapObjEnv");
    CMapObjRender::s_effects[4] = CShaderEffectManager::GetEffect("MapObjOpaque");
    CMapObjRender::s_effects[5] = CShaderEffectManager::GetEffect("MapObjEnvMetal");

    for (uint32_t i = 6; i < 15; i++) {
        CMapObjRender::s_effects[i] = NULL;
    }

    CMapObjRender::s_unifiedEffects[0] = CShaderEffectManager::GetEffect("MapObjUDiffuse");
    CMapObjRender::s_unifiedEffects[1] = CShaderEffectManager::GetEffect("MapObjUSpecular");
    CMapObjRender::s_unifiedEffects[2] = CShaderEffectManager::GetEffect("MapObjUMetal");
    CMapObjRender::s_unifiedEffects[3] = CShaderEffectManager::GetEffect("MapObjUEnv");
    CMapObjRender::s_unifiedEffects[4] = CShaderEffectManager::GetEffect("MapObjUOpaque");
    CMapObjRender::s_unifiedEffects[5] = CShaderEffectManager::GetEffect("MapObjUEnvMetal");
    CMapObjRender::s_unifiedEffects[6] = CShaderEffectManager::GetEffect("MapObjUTwoLayerDiffuse");
    CMapObjRender::s_unifiedEffects[7] = CShaderEffectManager::GetEffect("MapObjUTwoLayerEnvMetal");
    CMapObjRender::s_unifiedEffects[8] = CShaderEffectManager::GetEffect("MapObjUTwoLayerTerrain");
    CMapObjRender::s_unifiedEffects[9] = CShaderEffectManager::GetEffect("MapObjUDiffuseEmissive");
    CMapObjRender::s_unifiedEffects[10] = NULL;
    CMapObjRender::s_unifiedEffects[11] = CShaderEffectManager::GetEffect("MapObjUMaskedEnvMetal");
    CMapObjRender::s_unifiedEffects[12] = CShaderEffectManager::GetEffect("MapObjUEnvMetalEmissive");
    CMapObjRender::s_unifiedEffects[13] = CShaderEffectManager::GetEffect("MapObjUTwoLayerDiffuseOpaque");
    CMapObjRender::s_unifiedEffects[14] = NULL;

    CMapObjRender::s_waterWindowEffect = NULL;
    CMapObjRender::s_submarineEffect = NULL;
    CreateWaterEffects();
    Liquid::AddConfigChangedCallback(CMapObjRender::WaterResetCallback);
}

// OFFSET: 0x00779F10
void CMapObjRender::Destroy() {
    Liquid::RemoveConfigChangedCallback(CMapObjRender::WaterResetCallback);
    DestroyWaterEffects();

    for (uint32_t i = 0; i < 15; i++) {
        CMapObjRender::s_effects[i] = NULL;
        CMapObjRender::s_unifiedEffects[i] = NULL;
    }

    CShaderEffectManager::DestroyAllEffects();
}

// OFFSET: 0x00779FE0
void CMapObjRender::RenderMaterialElements(CWorldView* view, CSceneMaterial* material) {
    TSGrowableArray<RElement>& elements = material->m_elements[view->m_viewType];

    for (uint32_t i = 0; i < elements.m_count; i++) {
        CMapObjDefGroup* defGroup = elements.m_data[i].m_group;
        SMOBatch* batch = elements.m_data[i].m_batch;
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (CShaderEffect::s_enableShaders) {
            C44Matrix modelView = mapObjDef->m_mat * view->m_viewMat;
            C44Matrix transposed = modelView.Transpose();
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 31, &transposed.a0, 3);
        } else {
            C44Matrix camera;
            camera.Translate(C3Vector(-view->m_camPos.x, -view->m_camPos.y, -view->m_camPos.z));
            GxXformSet(GxXform_World, mapObjDef->m_mat * camera);
        }

        group->m_unusedFrames = 0;
        group->AllocVB();
        group->SetIndexVB();

        if (material->m_key.m_secondVB) {
            group->SetTransparencyVB();
        } else {
            group->SetVertexVB();
        }

        CGxBatch gxBatch;
        gxBatch.m_primType = GxPrim_Triangles;
        gxBatch.m_start = batch->m_startIndex;
        gxBatch.m_count = batch->m_count;
        gxBatch.m_minIndex = batch->m_minIndex;
        gxBatch.m_maxIndex = batch->m_maxIndex;
        g_theGxDevicePtr->Draw(gxBatch);
    }
}

// PROVEN: 99d80a2d IDENTICAL/NONE
// OFFSET: 0x0077A320
HASHKEY_MATERIAL& HASHKEY_MATERIAL::operator=(const HASHKEY_MATERIAL& key) {
    this->m_textures[0] = key.m_textures[0];
    this->m_textures[1] = key.m_textures[1];
    this->m_textures[2] = key.m_textures[2];
    this->m_materialFlags = key.m_materialFlags;
    this->m_ambColor = key.m_ambColor;
    this->m_color = key.m_color;
    this->m_pass = key.m_pass;
    this->m_shader = key.m_shader;
    this->m_unified = key.m_unified;
    this->m_blendMode = key.m_blendMode;
    this->m_cull = key.m_cull;
    this->m_lighting = key.m_lighting;
    this->m_interior = key.m_interior;
    this->m_fog = key.m_fog;
    this->m_secondVB = key.m_secondVB;
    this->m_shadow = key.m_shadow;
    return *this;
}

// INLINED
void HASHKEY_MATERIAL::SetState(uint32_t state) {
    this->m_pass = state & 0x7;
    this->m_shader = (state >> 3) & 0xF;
    this->m_unified = (state >> 7) & 0x1;
    this->m_blendMode = (state >> 8) & 0xF;
    this->m_cull = (state >> 12) & 0x3;
    this->m_lighting = (state >> 14) & 0x3;
    this->m_interior = (state >> 16) & 0x1;
    this->m_fog = (state >> 17) & 0x7;
    this->m_secondVB = (state >> 20) & 0x1;
    this->m_shadow = (state >> 21) & 0x3;
    this->m_pad = state >> 23;
}

// OFFSET: 0x0077A4E0
void CMapObj::UpdateMaterials() {
    float scalar = DayNight::GetSelfIlluminatedScalar();

    for (uint32_t i = 0; i < this->m_materialCount; i++) {
        SMOMaterial* material = &this->m_materials[i];

        if (!(material->m_flags & 0x10)) {
            continue;
        }

        material->m_frameSidnColor = material->m_sidnColor;
        uint32_t factor = static_cast<uint32_t>(static_cast<int64_t>(scalar * 255.0f));
        uint8_t* color = reinterpret_cast<uint8_t*>(&material->m_frameSidnColor);
        uint32_t b = static_cast<uint8_t>(static_cast<uint16_t>(factor * color[0]) >> 8);
        uint32_t g = static_cast<uint8_t>(static_cast<uint16_t>(factor * color[1]) >> 8);
        uint32_t r = static_cast<uint8_t>(static_cast<uint16_t>(factor * color[2]) >> 8);
        *reinterpret_cast<uint32_t*>(&material->m_frameSidnColor) = b | ((g | (r << 8)) << 8);
    }
}

// OFFSET: 0x0077A5A0
void CMapObjRender::SceneSetup(CWorldView* view) {
    const CM2Light& sun = CWorldMap::s_sunLight;
    C4Vector specular(sun.m_specColor.x, sun.m_specColor.y, sun.m_specColor.z, 14.0f);
    const C44Matrix& viewMat = view->m_viewMat;
    C4Vector direction;
    direction.x = viewMat.b0 * sun.m_dir.y + viewMat.c0 * sun.m_dir.z + sun.m_dir.x * viewMat.a0;
    direction.y = viewMat.a1 * sun.m_dir.x + viewMat.c1 * sun.m_dir.z + viewMat.b1 * sun.m_dir.y;
    direction.z = viewMat.a2 * sun.m_dir.x + viewMat.c2 * sun.m_dir.z + viewMat.b2 * sun.m_dir.y;
    direction.w = 0.0f;

    CShaderEffect::SetGlobalLighting(&CWorldMap::s_sunLight);
    CShaderEffect::SetLocalLighting(&CWorldScene::s_lighting, 1, &view->m_camPos);
    CShaderEffect::UpdateProjMatrix();
    CShaderEffect::SetTexMtx_Identity(0);
    CShaderEffect::SetTexMtx_Identity(1);

    if (CShaderEffect::s_enableShaders) {
        C4Vector half(0.5f, 0.5f, 0.5f, 1.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 12, &direction.x, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 13, &specular.x, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 28, &half.x, 1);

        if (CShaderEffect::GetClipEnabled() && g_theGxDevicePtr->m_caps.unk_02BC) {
            C4Plane plane = view->m_cullPlane;
            plane.Transform(viewMat);
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 34, &plane.n.x, 1);
        }
    } else {
        GxRsSet(GxRs_MatDiffuse, static_cast<int32_t>(0xFF7F7F7F));
    }

    GxRsSet(GxRs_DepthWrite, 1);
}

// OFFSET: 0x0077A800
void CSceneMaterial::UpdateSceneMaterial(int32_t shadow) {
    CShaderEffect* effect = this->m_key.m_unified ? CMapObjRender::s_unifiedEffects[this->m_key.m_shader] : CMapObjRender::s_effects[this->m_key.m_shader];
    this->m_effect = effect;

    if (!CShaderEffect::s_enableShaders) {
        this->m_permuteHash = 0;
        return;
    }

    int32_t shadowValue = shadow;

    if (shadow && this->m_key.m_interior) {
        shadowValue = 1;
    }

    int32_t alphaTest = this->m_key.m_blendMode == 1 && (!g_theGxDevicePtr->m_caps.unk_02F4 || (!CShaderEffect::s_forceAlphaTest && shadowValue));
    uint32_t vertexPermute = CShaderEffect::ComputeVertexPermute(0, 0, this->m_key.m_lighting != 0, CWorldScene::s_lighting.m_lightCount, 0, shadowValue);
    uint32_t pixelPermute = shadowValue + 4 * (CShaderEffect::s_usePcfFiltering + 2 * alphaTest);
    this->m_vertexPermute = vertexPermute;
    this->m_pixelPermute = pixelPermute;

    uintptr_t vs = reinterpret_cast<uintptr_t>(effect->GetVertexPermute(vertexPermute));
    uintptr_t ps = reinterpret_cast<uintptr_t>(effect->GetPixelPermute(pixelPermute));
    this->m_permuteHash = static_cast<uint32_t>(vs ^ ((vs >> 2) + ps + (vs << 6) - 0x61C88647));
}

// OFFSET: 0x0077A920
int32_t CMapObjRender::ApplyMaterial(CWorldView* view, CSceneMaterial* material) {
    const HASHKEY_MATERIAL& key = material->m_key;
    material->m_effect->SetCurrent();

    if (CShaderEffect::s_enableShaders) {
        int32_t clip = CShaderEffect::GetClipEnabled() && g_theGxDevicePtr->m_caps.unk_02BC;
        CShaderEffect::SetShaders(clip + material->m_vertexPermute, -1, -1, material->m_pixelPermute);
    } else {
        GxRsSet(GxRs_ColorMaterial, static_cast<int32_t>(key.m_shadow));
    }

    if (key.m_shader == 8) {
        C44Matrix texGen;
        texGen.d0 = view->m_camPos.x;
        texGen.d1 = view->m_camPos.y;
        texGen.d2 = view->m_camPos.z;
        CShaderEffect::SetTexMtx_TexGen(static_cast<EGxTexGen>(2), texGen, 1);
    } else if (key.m_shader == 11) {
        static C2Vector s_scale(1.0f, 1.0f);
        C2Vector offset(static_cast<float>(static_cast<double>(FrameTime::GetCurTimeMs() % 10000) * 0.001 * 0.1), 0.0f);
        CShaderEffect::SetTexMtx(s_scale, offset, 1);
    } else {
        CShaderEffect::SetTexMtx_Identity(1);
    }

    for (uint32_t i = 0; i < 3; i++) {
        CGxTex* texture = NULL;

        if (key.m_textures[i]) {
            texture = TextureGetGxTex(key.m_textures[i]);

            if (!texture) {
                return 0;
            }
        }

        g_theGxDevicePtr->RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), texture);
    }

    const DayNight::LightInfo* lightInfo = DayNight::GetLightInfo();
    float lights[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float* ambient = &lights[0];
    float* direct = &lights[3];

    switch (key.m_lighting) {
        case 1:
            direct[0] = ColorByte(lightInfo->m_directColor.r);
            direct[1] = ColorByte(lightInfo->m_directColor.g);
            direct[2] = ColorByte(lightInfo->m_directColor.b);
            ambient[0] = ColorByte(lightInfo->m_ambientColor.r);
            ambient[1] = ColorByte(lightInfo->m_ambientColor.g);
            ambient[2] = ColorByte(lightInfo->m_ambientColor.b);
            break;

        case 2:
            direct[0] = ColorByte(lightInfo->m_midColor0.r);
            direct[1] = ColorByte(lightInfo->m_midColor0.g);
            direct[2] = ColorByte(lightInfo->m_midColor0.b);
            ambient[0] = ColorByte(lightInfo->m_midColor1.r);
            ambient[1] = ColorByte(lightInfo->m_midColor1.g);
            ambient[2] = ColorByte(lightInfo->m_midColor1.b);
            break;

        case 3: {
            const uint8_t* color = reinterpret_cast<const uint8_t*>(&key.m_ambColor);
            ambient[0] = ColorByte(color[2]);
            ambient[1] = ColorByte(color[1]);
            ambient[2] = ColorByte(color[0]);
            break;
        }
    }

    if (CShaderEffect::s_enableShaders) {
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 10, ambient, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, direct, 1);
    } else if (key.m_lighting) {
        GxRsSet(GxRs_Lighting, 1);
        CM2Light& sun = CWorldMap::s_sunLight;
        C3Vector savedDir = sun.m_dirColor;
        C3Vector savedAmb = sun.m_ambColor;
        sun.m_dirColor = C3Vector(direct[0], direct[1], direct[2]);
        sun.m_ambColor = C3Vector(ambient[0], ambient[1], ambient[2]);
        sun.GxLightSet(0);
        sun.m_dirColor = savedDir;
        sun.m_ambColor = savedAmb;
    } else {
        GxRsSet(GxRs_Lighting, 0);
    }

    CShadowCache::SetShadowMapGenericInterior(key.m_interior);

    if (key.m_fog) {
        const DayNight::FogInfo* fog = (key.m_fog & 0x2) ? DayNight::GetFogInfo() : DayNight::GetIntFogInfo();
        CImVector color = (key.m_fog & 0x4) ? CImVector(0xFF000000) : fog->m_color;
        CShaderEffect::SetFogParams(fog->m_start, fog->m_end, fog->m_density, color, 0.0f);
        CShaderEffect::SetFogEnabled(1);
    } else {
        CShaderEffect::SetFogEnabled(0);
    }

    if (CShaderEffect::s_enableShaders) {
        C4Vector emissive(*reinterpret_cast<const CImVector*>(&key.m_color));
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 29, &emissive.x, 1);
    } else {
        GxRsSet(GxRs_MatEmissive, static_cast<int32_t>(key.m_color));
    }

    EGxTexWrapMode wrapU = static_cast<EGxTexWrapMode>((key.m_materialFlags & 0x40) == 0);
    EGxTexWrapMode wrapV = static_cast<EGxTexWrapMode>((key.m_materialFlags & 0x80) == 0);
    CGxTex* texture = TextureGetGxTex(key.m_textures[0]);

    if (texture) {
        GxTexSetWrap(texture, wrapU, wrapV);
    }

    if (key.m_textures[1]) {
        texture = TextureGetGxTex(key.m_textures[1]);

        if (texture) {
            GxTexSetWrap(texture, wrapU, wrapV);
        }
    }

    if (key.m_textures[2]) {
        texture = TextureGetGxTex(key.m_textures[2]);

        if (texture) {
            GxTexSetWrap(texture, wrapU, wrapV);
        }
    }

    int32_t cullMode = (view->m_flags & 0x8) ? 2 : 1;
    GxRsSet(GxRs_Culling, key.m_cull ? cullMode : 0);
    GxRsSet(GxRs_BlendingMode, static_cast<int32_t>(key.m_blendMode));
    CShaderEffect::SetAlphaRefDefault();
    return 1;
}

// OFFSET: 0x0077B0A0
void CMapObjRender::BuildFrameMaterials(CWorldView* view) {
    for (uint32_t i = 0; i < 5; i++) {
        this->m_materials[i].m_count = 0;
    }

    for (CSceneMaterial* material = CMapObjRender::s_materialHash.Head(); material; material = CMapObjRender::s_materialHash.Next(material)) {
        if (!material->m_elements[view->m_viewType].m_count) {
            continue;
        }

        material->UpdateSceneMaterial(view->m_shadowMode);
        *this->m_materials[material->m_key.m_pass].New() = material;
    }
}

// OFFSET: 0x0077B1C0
void CMapObjRender::RenderMaterialList(CWorldView* view, TSGrowableArray<CSceneMaterial*>& list) {
    for (uint32_t i = 0; i < list.m_count; i++) {
        CSceneMaterial* material = list.m_data[i];

        if (CMapObjRender::ApplyMaterial(view, material)) {
            CMapObjRender::RenderMaterialElements(view, material);
        }

        material->m_elements[view->m_viewType].m_count = 0;
    }
}

// OFFSET: 0x0077B220
void CMapObj::RenderMapObjGroupsShadowMap(const CWorldQueryElement* groups, uint32_t count, const C44Matrix& world, const CFrustum& frustum) {
    CAaBox bounds;
    bounds.b.x = 0.0f;
    bounds.b.y = 0.0f;
    bounds.b.z = 0.0f;
    bounds.t.x = 0.0f;
    bounds.t.y = 0.0f;
    bounds.t.z = 0.0f;
    g_theGxDevicePtr->RsPush();

    for (uint32_t i = 0; i < count; i++) {
        CMapObjGroup* group = groups[i].m_group;
        const C44Matrix* matrix = groups[i].m_matrix;
        CMapObj* mapObj = group->m_parent;
        GxXformSet(GxXform_World, *matrix * world);
        CWorldMath::TransformAABox(*matrix, group->m_bbox, bounds);

        if (!group->m_batchCount || !frustum.Cull(bounds)) {
            continue;
        }

        group->m_unusedFrames = 0;
        group->AllocVB();
        group->SetIndexVB();
        group->SetVertexVB();
        CShaderEffect::UpdateWorldViewMatrix();

        if (group->m_renderFlags & 0x2) {
            CShadowCache::SetShadowMapModel(0, 1);
            GxRsSet(GxRs_Texture0, static_cast<void*>(NULL));
            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = group->m_indexStart;
            batch.m_count = group->m_indexEnd - group->m_indexStart + 1;
            batch.m_minIndex = group->m_minVertex;
            batch.m_maxIndex = group->m_maxVertex;
            g_theGxDevicePtr->Draw(batch);
            continue;
        }

        CShadowCache::SetShadowMapModel(0, 0);
        uint32_t batchCount = group->m_shadowBatchCount;
        SMOBatch* batches = group->m_shadowBatches;

        if (!batchCount) {
            batchCount = group->m_batchCount;
            batches = group->m_batches;
        }

        for (uint32_t j = 0; j < batchCount; j++) {
            const SMOBatch* batch = &batches[j];
            const SMOMaterial* material = &mapObj->m_materials[batch->m_materialId];

            if (material->m_flags & 0x100) {
                continue;
            }

            CGxTex* texture = TextureGetGxTex(mapObj->m_runtimeMaterials[batch->m_materialId].m_textures[0]);

            if (!texture) {
                continue;
            }

            g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
            CShadowCache::SetShadowMapAlphaTest(material->m_blendMode == 1);
            CGxBatch gxBatch;
            gxBatch.m_primType = GxPrim_Triangles;
            gxBatch.m_start = batch->m_startIndex;
            gxBatch.m_count = batch->m_count;
            gxBatch.m_minIndex = batch->m_minIndex;
            gxBatch.m_maxIndex = batch->m_maxIndex;
            g_theGxDevicePtr->Draw(gxBatch);
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0077B580
void CMapObj::RenderMapObjGroupsColor(const CWorldQueryElement* groups, uint32_t count, const C44Matrix& world, const CImVector& color) {
    uint32_t vertexColor = *reinterpret_cast<const uint32_t*>(&color);

    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        vertexColor = (*reinterpret_cast<const uint32_t*>(&color) & 0xFF00FF00) | ((*reinterpret_cast<const uint32_t*>(&color) >> 16) & 0xFF) | ((*reinterpret_cast<const uint32_t*>(&color) & 0xFF) << 16);
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 0);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_VertexShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_PixelShader, static_cast<void*>(NULL));

    for (uint32_t i = 0; i < count; i++) {
        CMapObjGroup* group = groups[i].m_group;
        CMapObj* mapObj = group->m_parent;
        GxXformSet(GxXform_World, *groups[i].m_matrix * world);

        if (!group->m_batchCount) {
            continue;
        }

        CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 28, group->m_vertexCount);
        uint32_t* dst = reinterpret_cast<uint32_t*>(g_theGxDevicePtr->BufLock(vertexBuf));

        for (uint32_t j = 0; j < group->m_vertexCount; j++) {
            const uint32_t* position = reinterpret_cast<const uint32_t*>(&group->m_vertices[j]);
            const uint32_t* normal = reinterpret_cast<const uint32_t*>(&group->m_normals[j]);
            dst[0] = position[0];
            dst[1] = position[1];
            dst[2] = position[2];
            dst[3] = normal[0];
            dst[4] = normal[1];
            dst[5] = normal[2];
            dst[6] = vertexColor;
            dst += 7;
        }

        g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
        vertexBuf->unk_001C = 1;
        g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PNC);

        CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, group->m_indexCount);
        void* indices = g_theGxDevicePtr->BufLock(indexBuf);
        memcpy(indices, group->m_indices, sizeof(uint16_t) * group->m_indexCount);
        g_theGxDevicePtr->BufUnlock(indexBuf, 0);
        indexBuf->unk_001C = 1;
        g_theGxDevicePtr->PrimIndexPtr(indexBuf);

        if (group->m_renderFlags & 0x2) {
            GxRsSet(GxRs_Texture0, static_cast<void*>(NULL));
            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = group->m_indexStart;
            batch.m_count = group->m_indexEnd - group->m_indexStart + 1;
            batch.m_minIndex = group->m_minVertex;
            batch.m_maxIndex = group->m_maxVertex;
            DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), -1);
            g_theGxDevicePtr->Draw(batch);
            continue;
        }

        for (uint32_t j = 0; j < group->m_batchCount; j++) {
            const SMOBatch* batch = &group->m_batches[j];
            CGxTex* texture = TextureGetGxTex(mapObj->m_runtimeMaterials[batch->m_materialId].m_textures[0]);

            if (!texture) {
                continue;
            }

            g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
            DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), -1);
            CGxBatch gxBatch;
            gxBatch.m_primType = GxPrim_Triangles;
            gxBatch.m_start = batch->m_startIndex;
            gxBatch.m_count = batch->m_count;
            gxBatch.m_minIndex = batch->m_minIndex;
            gxBatch.m_maxIndex = batch->m_maxIndex;
            g_theGxDevicePtr->Draw(gxBatch);
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0077BA60
void CMapObjRender::UnregisterMaterial(CSceneMaterial* material) {
    if (material->m_refCount-- != 1) {
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        CTexture* texture = material->m_key.m_textures[i];

        if (texture) {
            TextureRelease(texture);
        }
    }

    CMapObjRender::s_materialHash.Delete(material);
}

// OFFSET: 0x0077BAB0
void CMapObjRender::RenderPass(CWorldView* view, uint32_t pass) {
    TSGrowableArray<CSceneMaterial*>& list = this->m_materials[pass];

    if (!list.m_count) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    CMapObjRender::SceneSetup(view);
    CMapObjRender::RenderMaterialList(view, list);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0077C390
CSceneMaterial* CMapObjRender::RegisterMaterial(const HASHKEY_MATERIAL& key) {
    uint32_t seed = (key.m_blendMode << 8) | (key.m_materialFlags << 16) | key.m_shader;
    uint32_t hash = ((seed >> 2) + (seed << 6) + reinterpret_cast<uintptr_t>(key.m_textures[0]) - 0x61C88647) ^ seed;
    hash = ((hash >> 2) + (hash << 6) + reinterpret_cast<uintptr_t>(key.m_textures[1]) - 0x61C88647) ^ hash;
    hash = ((hash >> 2) + (hash << 6) + reinterpret_cast<uintptr_t>(key.m_textures[2]) - 0x61C88647) ^ hash;

    CSceneMaterial* material = CMapObjRender::s_materialHash.Ptr(hash, key);

    if (!material) {
        material = CMapObjRender::s_materialHash.New(hash, key, 0, 0);
        material->m_refCount = 0;

        for (uint32_t i = 0; i < 3; i++) {
            if (key.m_textures[i]) {
                TextureAddRef(key.m_textures[i]);
            }
        }
    }

    material->m_refCount++;
    return material;
}

// OFFSET: 0x0077C4C0
void CMapObj::UpdateSceneMaterials(CMapObjDefGroup* defGroup, uint32_t state) {
    CMapObjGroup* group = this->GetGroup(defGroup->m_groupNum, 0);
    int32_t unified = this->m_header->m_flags & 0x2;
    uint32_t shadow = (state & 0x4) ? 1 : 2;
    CSceneMaterial** slot = defGroup->m_batchMaterials.m_data;
    uint32_t keyState = 0;

    for (uint32_t i = 0; i < group->m_batchCount; i++) {
        const SMOBatch* batch = &group->m_batches[i];
        const SMOMaterial* material = &this->m_materials[batch->m_materialId];

        if (material->m_shader == 10 || material->m_shader == 14) {
            continue;
        }

        const CMapObjMaterial* runtime = &this->m_runtimeMaterials[batch->m_materialId];
        uint32_t flags = material->m_flags;
        HASHKEY_MATERIAL key;
        key.m_textures[0] = runtime->m_textures[0];
        key.m_textures[1] = runtime->m_textures[1];
        key.m_textures[2] = runtime->m_textures[2];
        key.m_materialFlags = flags;
        key.m_ambColor = 0;

        uint32_t base = (keyState & 0xFF800000) | (~(flags << 10) & 0x1000) | (8 * ((material->m_shader & 0xF) | (16 * ((unified != 0) | (2 * (material->m_blendMode & 0xF))))));
        uint32_t color = (flags & 0x10) ? *reinterpret_cast<const uint32_t*>(&material->m_frameSidnColor) : 0;

        if (state & 0x2) {
            uint32_t highlight = *reinterpret_cast<const uint32_t*>(&DayNight::GetLightInfo()->m_ambientColor);
            uint32_t sum = highlight + color;
            uint32_t carry = (color ^ highlight ^ sum) & 0x1010100;
            color = (sum - carry) | (carry - (carry >> 8));
        }

        color = (((color >> 16) & 0xFF) >> 1 << 16) | ((((color >> 8) & 0xFF) >> 1) << 8) | ((color & 0xFF) >> 1);
        key.m_color = color;
        uint32_t lighting = (((flags & 0x20) != 0) + 1) << 14;
        uint32_t fog = ((flags & 0x2) ? 0 : shadow) << 17;

        if (!unified) {
            if (group->m_colors) {
                if (i >= group->m_transBatchCount) {
                    uint32_t value = i >= static_cast<uint32_t>(group->m_transBatchCount + group->m_intBatchCount) ? ((base & ~0xC000) | lighting) : (base & ~0xC000);
                    keyState = (value & ~0xE0000) | fog | 0x10000;
                } else {
                    uint32_t first = (base & 0xFFF139F9) | (~(flags << 17) & 0x40000) | lighting | 0x901;
                    key.SetState(first);
                    CSceneMaterial* previous = *slot;
                    CSceneMaterial* registered = CMapObjRender::RegisterMaterial(key);

                    if (previous) {
                        CMapObjRender::UnregisterMaterial(previous);
                    }

                    *slot++ = registered;
                    keyState = (first & 0xFFF137FA) | fog | 0x10702;
                }
            } else {
                keyState = (base & 0xFFF03FFF) | (~(flags << 14) & 0x4000) | (~(flags << 17) & 0x40000);
            }
        } else {
            key.m_ambColor = *reinterpret_cast<uint32_t*>(&this->m_ambColor);

            if (i < group->m_transBatchCount) {
                uint32_t first = (base & 0xFFFFF0F8) | 0x901;
                first = (flags & 0x1) ? (first & ~0xC000) : ((first & ~0xC000) | lighting);

                if (CShaderEffect::s_enableShaders) {
                    first = (first & 0xFFF1FFFF) | (~(flags << 17) & 0x40000);
                    key.SetState(first);
                    CSceneMaterial* previous = *slot;
                    CSceneMaterial* registered = CMapObjRender::RegisterMaterial(key);

                    if (previous) {
                        CMapObjRender::UnregisterMaterial(previous);
                    }

                    *slot++ = registered;
                    keyState = (first & 0xFFF1F7FA) | fog | 0x1C702;
                } else {
                    first = (first & 0xFFF1FFFF) | (((((flags & 0x2) ? 0 : 6) & 7) | 8) << 17);
                    key.SetState(first);
                    CSceneMaterial* previous = *slot;
                    CSceneMaterial* registered = CMapObjRender::RegisterMaterial(key);

                    if (previous) {
                        CMapObjRender::UnregisterMaterial(previous);
                    }

                    *slot++ = registered;
                    uint32_t second = (first & 0xFFFF30F8) | 0xC702;
                    second = (second & ~0xE0000) | ((((flags & 0x2) ? 0 : (shadow | 4)) & 7) << 17);
                    key.SetState(second);
                    previous = *slot;
                    registered = CMapObjRender::RegisterMaterial(key);

                    if (previous) {
                        CMapObjRender::UnregisterMaterial(previous);
                    }

                    *slot++ = registered;
                    keyState = (second & 0xFFE13AFB) | fog | 0xA03;
                }
            } else if (!(group->m_groupFlags & 0x48)) {
                uint32_t value = (flags & 0x20) ? ((base & 0xFFFF3FFF) | 0x8000) : (base | 0xC000);
                keyState = (((value & ~0xE0000) | (shadow << 17)) & 0xFF9EFFFF) | 0x410000;
            } else {
                uint32_t hash = this->m_hashval;
                uint32_t value = base & 0xFFF53FFF;
                bool special = hash == 0xEA58F58B || hash == 0x9F57D72D || hash == 0x66C8E63E || hash == 0x91A6305E;

                if (special) {
                    value = (value & 0xFFF03FFF) | 0x30000;
                } else {
                    value |= (~(flags << 14) & 0x4000) | 0x40000;
                }

                keyState = (value & 0xFF9FFFFF) | 0x400000;
            }
        }

        key.SetState(keyState);
        CSceneMaterial* previous = *slot;
        CSceneMaterial* registered = CMapObjRender::RegisterMaterial(key);

        if (previous) {
            CMapObjRender::UnregisterMaterial(previous);
        }

        *slot++ = registered;
    }
}

// OFFSET: 0x0077CA20/0x140469410
void CMapObj::GatherGroup(CWorldView* view, CMapObjDefGroup* defGroup) {
    CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
    CMapObjGroup* group = this->GetGroup(defGroup->m_groupNum, 0);

    if (!(group->m_renderFlags & 0x1)) {
        this->AttenTransVerts(group);
    }

    uint32_t state = (view->m_flags & 0x4) ? 5 : 1;

    if (mapObjDef->m_param64 && mapObjDef->m_param64 == World::GetTrackedMapObjGUID()) {
        state |= 0x2;
    }

    if (state != defGroup->m_materialState) {
        this->UpdateSceneMaterials(defGroup, state);
        defGroup->m_materialState = state;
    }

    int32_t unified = this->m_header->m_flags & 0x2;
    CSceneMaterial** slot = defGroup->m_batchMaterials.m_data;

    for (uint32_t i = 0; i < group->m_batchCount; i++) {
        SMOBatch* batch = &group->m_batches[i];
        uint32_t shader = this->m_materials[batch->m_materialId].m_shader;

        if (shader == 10) {
            if (*reinterpret_cast<uint32_t*>(CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex]) == 643) {
                CMapObjRender::RenderLiquidBatch(CMapObjRender::s_waterWindowEffect, batch, defGroup, group, i);
                continue;
            }

            CMapObjRender::RenderLiquidBatch(CMapObjRender::s_submarineEffect, batch, defGroup, group, i);
            continue;
        }

        if (shader == 14) {
            CMapObjRender::RenderLiquidBatch(CMapObjRender::s_submarineEffect, batch, defGroup, group, i);
            continue;
        }

        uint32_t passCount = group->ComputePassCount(unified, i);

        for (uint32_t j = 0; j < passCount; j++) {
            RElement* element = slot[j]->m_elements[view->m_viewType].New();
            element->m_group = defGroup;
            element->m_batch = batch;
        }

        slot += passCount;
    }
}
