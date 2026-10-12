#include <cmath>
#include <cstdint>
#include "Engine/Source/Model2/CM2SceneRender.h"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shader.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Services/RibbonEmitter.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/NTempest.h"

const uint32_t s_fogModes[7] = { 1, 1, 1, 2, 2, 3, 4 };

const uint32_t s_noLiquidFog[7] = { 0, 0, 0, 1, 1, 0, 0 };

// STATIC: 0x00D07F20/0x140BB91B0
static C44Matrix s_identity;

// OFFSET: 0x00828980/0x1405469E0
CM2SceneRender::CM2SceneRender(CM2Scene* scene) {
    this->m_scene = scene;
    this->m_cache = scene->m_cache;
    this->m_defaultMaterial.m_flags = 0;
    this->m_defaultMaterial.m_blendMode = 0;
    this->m_sceneFlag8 = 0;
    this->m_particleEffect = CShaderEffectManager::GetEffect("Particle");
    this->m_particleUnlitEffect = CShaderEffectManager::GetEffect("Particle_Unlit");
    this->m_projectedModModEffect = CShaderEffectManager::GetEffect("Projected_ModMod");
    this->m_projectedModModUnlitEffect = CShaderEffectManager::GetEffect("Projected_ModMod_Unlit");
    this->m_projectedModAddEffect = CShaderEffectManager::GetEffect("Projected_ModAdd");
    this->m_projectedModAddUnlitEffect = CShaderEffectManager::GetEffect("Projected_ModAdd_Unlit");
}

// OFFSET: 0x0082AA70
void CM2SceneRender::SetTextureTransform(uint32_t index, uint32_t stage) {
    uint32_t transform = this->m_data->m_textureTransformCombos[index + this->m_curBatch->m_textureTransformComboIndex];

    if (transform >= this->m_data->m_textureTransforms.m_count) {
        CShaderEffect::SetTexMtx_Identity(stage);
    } else {
        CShaderEffect::SetTexMtx(this->m_curModel->m_textureMatrices[transform], stage);
    }
}

// OFFSET: 0x0082AAC0
void CM2SceneRender::SetTextureTransform(uint32_t stage) {
    uint32_t transform = this->m_data->m_textureTransformCombos[stage + this->m_curBatch->m_textureTransformComboIndex];

    if (transform >= this->m_data->m_textureTransforms.m_count) {
        CShaderEffect::SetTexMtx_Identity(stage);
    } else {
        CShaderEffect::SetTexMtx(this->m_curModel->m_textureMatrices[transform], stage);
    }
}

// OFFSET: 0x0082AB10/0x140549850
void CM2SceneRender::SetupTextureTransforms() {
    if (this->m_curType > 2) {
        CShaderEffect::SetTexMtx_Identity(0);
        CShaderEffect::SetTexMtx_Identity(1);
        return;
    }

    uint32_t shaderId = M2GetVertexShaderID(this->m_curBatch->m_textureCount, this->m_curBatch->m_shader);

    if (CShaderEffect::s_enableShaders) {
        switch (shaderId) {
            case 0:
            case 3:
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 14:
                this->SetTextureTransform(0);
                break;
            case 2:
            case 12:
                this->SetTextureTransform(1);
                this->SetTextureTransform(0);
                break;
            case 10:
                this->SetTextureTransform(0, 1);
                break;
            case 11:
                this->SetTextureTransform(2, 1);
                this->SetTextureTransform(0);
                break;
            case 13:
                this->SetTextureTransform(3, 1);
                this->SetTextureTransform(0);
                break;
            case 15:
                this->SetTextureTransform(2, 0);
                this->SetTextureTransform(1);
                this->SetTextureTransform(0);
                break;
            default:
                break;
        }
    } else {
        switch (shaderId) {
            case 0:
            case 9:
            case 10:
                this->SetTextureTransform(0);
                break;
            case 1:
            case 14:
                CShaderEffect::SetTexMtx_SphereMap(0);
                break;
            case 2:
            case 7:
            case 8:
            case 12:
            case 13:
            case 15:
                this->SetTextureTransform(1);
                this->SetTextureTransform(0);
                break;
            case 3:
            case 6:
            case 11:
                this->SetTextureTransform(0);
                CShaderEffect::SetTexMtx_SphereMap(1);
                break;
            case 4:
                CShaderEffect::SetTexMtx_SphereMap(0);
                this->SetTextureTransform(1);
                break;
            case 5:
                CShaderEffect::SetTexMtx_SphereMap(1);
                CShaderEffect::SetTexMtx_SphereMap(0);
                break;
            default:
                break;
        }
    }

    M2Batch* batch = this->m_curBatch;
    float weights[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    if (batch->m_flags & 0x40) {
        uint32_t count = batch->m_textureCount;

        if (count > 4) {
            count = 4;
        }

        for (uint32_t i = 0; i < count; i++) {
            weights[i] = this->m_curModel->m_textureWeights[this->m_data->m_textureWeightCombos[i + batch->m_textureWeightComboIndex]].m_currentValue;
        }
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 4, weights, 1);
}

// OFFSET: 0x0082ACF0/0x140549C20
void CM2SceneRender::SetupTextures() {
    this->SetupTextureTransforms();

    if (this->m_curType > 2) {
        for (uint32_t i = 0; i < 4; i++) {
            GxRsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), static_cast<void*>(NULL));
        }

        return;
    }

    uint32_t count = this->m_curBatch->m_textureCount;

    if (!CShaderEffect::s_enableShaders && (this->m_curBatch->m_shader & 0x8000)) {
        count = 1;
    }

    uint32_t i;

    if (this->m_curElement->m_flags & 8) {
        CTexture* litSphere = this->m_scene->m_litSphereTexture;

        if (litSphere) {
            CGxTex* gxTex = TextureGetGxTex(litSphere);

            if (gxTex) {
                GxTexSetWrap(gxTex, GxTex_Clamp, GxTex_Clamp);
                g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
            }
        }

        i = 1;
    } else {
        i = 0;

        if (count) {
            do {
                uint32_t combo = this->m_data->m_textureCombos[i + this->m_curBatch->m_textureComboIndex];
                CGxTex* gxTex = NULL;
                CTexture* texture;

                if (combo < this->m_data->m_textures.m_count && (texture = this->m_curModel->m_textures[combo]) != NULL) {
                    gxTex = TextureGetGxTex(texture);

                    if (gxTex) {
                        uint16_t flags = this->m_data->m_textures[combo].m_flags;
                        GxTexSetWrap(gxTex, static_cast<EGxTexWrapMode>(flags & 1), static_cast<EGxTexWrapMode>((flags >> 1) & 1));
                    }
                }

                g_theGxDevicePtr->RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), gxTex);
                i++;
            } while (i < count);

            if (i >= 4) {
                return;
            }
        }
    }

    do {
        GxRsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), static_cast<void*>(NULL));
        i++;
    } while (i < 4);
}

// OFFSET: 0x00829020/0x140547070
void CM2SceneRender::SetupBillboardView(const C3Vector& position) {
    C44Matrix view = this->m_scene->m_view;
    view.Translate(position);
    g_theGxDevicePtr->XformSetView(view);

    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
    world.m_mtx[world.m_level] = s_identity;
}

// OFFSET: 0x008290A0
void CM2SceneRender::EndFixed() {
    g_theGxDevicePtr->RsPop();
    g_theGxDevicePtr->XformSetView(s_identity);

    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
    world.m_mtx[world.m_level] = s_identity;
}

// OFFSET: 0x008280A0/0x140545FD0
void CM2SceneRender::SetBatchVertices(int32_t sharedVertices) {
    if (CShaderEffect::s_enableShaders) {
        if (this->m_curType != this->m_prevType || this->m_curShared != this->m_prevShared) {
            this->m_curShared->SetVertices(0, GxVBF_PBNT2);
        }

        return;
    }

    uint32_t env = this->m_curBatch->m_shader & 0x4000;
    int32_t sameEnv = this->m_prevBatch && env == (this->m_prevBatch->m_shader & 0x4000u);

    if (sharedVertices) {
        if (this->m_curType != this->m_prevType || this->m_curShared != this->m_prevShared || !sameEnv) {
            this->m_curShared->SetVertices(env != 0, GxVBF_PBNT2);
        }
    } else if (this->m_curType != this->m_prevType || this->m_curModel != this->m_prevModel || this->m_curSkinSection != this->m_prevSkinSection || !sameEnv) {
        if (this->m_curBatch->m_flags & 0x80) {
            this->m_curModel->SetVerticesMultiSample(this->m_curElement->m_flags & 4, this->m_curSkinSection);
        } else {
            this->m_curModel->SetVertices(this->m_curElement->m_flags & 4, this->m_curSkinSection, env != 0);
        }
    }
}

// OFFSET: 0x008281B0/0x1405460F0
int32_t CM2SceneRender::LightingColorsMatch(const CM2Lighting* a, const CM2Lighting* b) {
    return fabsf(a->m_sunAmbient.x - b->m_sunAmbient.x) < 0.0039215689f && fabsf(a->m_sunAmbient.y - b->m_sunAmbient.y) < 0.0039215689f && fabsf(a->m_sunAmbient.z - b->m_sunAmbient.z) < 0.0039215689f && fabsf(a->m_sunDiffuse.x - b->m_sunDiffuse.x) < 0.0039215689f && fequalz(a->m_sunDiffuse.y, b->m_sunDiffuse.y, 0.0039215689f) && fequalz(a->m_sunDiffuse.z, b->m_sunDiffuse.z, 0.0039215689f);
}

// OFFSET: 0x00828A70/0x140546AD0
void CM2SceneRender::SetupLighting() {
    M2Material* material = this->m_curMaterial;
    int32_t localLighting = !(material->m_flags & 1) && s_localLighting[material->m_blendMode];

    this->m_curLocalLighting = localLighting;
    CShaderEffect::SetLocalLighting(this->m_curLighting, localLighting, NULL);

    if (this->m_scene->m_liquidTypeId && this->m_pass == M2PASS_3) {
        if ((this->m_curElement->m_flags & 1) || s_noLiquidFog[this->m_curMaterial->m_blendMode]) {
            this->m_curFogMode = 0;
        } else {
            this->m_curFogMode = 6 - (ViewSettings::GetSubmerged() != 0);
        }
    } else if ((this->m_curMaterial->m_flags & 2) || this->m_curLighting->m_fogScale <= 0.0f) {
        this->m_curFogMode = 0;
    } else {
        this->m_curFogMode = s_fogModes[this->m_curMaterial->m_blendMode];
    }

    if (this->m_curFogMode) {
        CM2Lighting* lighting = this->m_curLighting;
        float start = lighting->m_fogStart;
        float end = lighting->m_fogEnd;
        float density = lighting->m_fogDensity;
        float height = 0.0f;
        CImVector color;
        *reinterpret_cast<uint32_t*>(&color) = 0;

        switch (this->m_curFogMode) {
            case 1:
            fogColor: {
                float b = lighting->m_fogColor.z;
                b = b >= 0.0f ? (b >= 1.0f ? 1.0f : b) : 0.0f;
                float g = lighting->m_fogColor.y;
                g = g >= 0.0f ? (g >= 1.0f ? 1.0f : g) : 0.0f;
                float r = lighting->m_fogColor.x;
                r = r >= 0.0f ? (r >= 1.0f ? 1.0f : r) : 0.0f;
                color.b = static_cast<uint8_t>(static_cast<int32_t>(b * 255.0f));
                color.g = static_cast<uint8_t>(static_cast<int32_t>(g * 255.0f));
                color.r = static_cast<uint8_t>(static_cast<int32_t>(r * 255.0f));
                color.a = 255;
                break;
            }
            case 2:
                *reinterpret_cast<uint32_t*>(&color) = 0;
                break;
            case 3:
                *reinterpret_cast<uint32_t*>(&color) = 0xFFFFFF;
                break;
            case 4:
                *reinterpret_cast<uint32_t*>(&color) = 0x808080;
                break;
            case 5:
                ViewSettings::GetFog((lighting->m_flags & 0x80) ? 2 : 3, start, end, density, color);
                break;
            case 6: {
                if (!(lighting->m_flags & 0x40)) {
                    goto fogColor;
                }

                ViewSettings::GetFog(3, start, end, density, color);

                C3Vector position(this->m_curModel->m_matrixF4.d0, this->m_curModel->m_matrixF4.d1, this->m_curModel->m_matrixF4.d2);
                const C4Plane& plane = this->m_curLighting->m_liquidPlane;
                float t = 0.0f;
                C3Vector hit;

                if (plane.n.y * position.y + plane.n.z * position.z + plane.n.x * position.x < 0.0f) {
                    C3Vector origin;

                    if (NTempest::Intersect(C3Ray(origin, position), plane, &t, &hit, 0.0099999998f)) {
                        height = hit.z;
                    }
                }

                break;
            }
            default:
                break;
        }

        CShaderEffect::SetFogParams(start, end, density, color, height);
        CShaderEffect::SetFogEnabled(1);
    } else {
        CShaderEffect::SetFogEnabled(0);
    }

    if (this->m_curType == this->m_prevType && this->m_curModel == this->m_prevModel && this->m_curLighting == this->m_prevLighting && !((this->m_curElement->m_flags ^ this->m_prevElement->m_flags) & 2)) {
        return;
    }

    if (!(this->m_curElement->m_flags & 2)) {
        CShaderEffect::SetClipEnabled(0);
        return;
    }

    C4Vector plane;
    plane.x = this->m_curLighting->m_liquidPlane.n.x;
    plane.y = this->m_curLighting->m_liquidPlane.n.y;
    plane.z = this->m_curLighting->m_liquidPlane.n.z;
    plane.w = this->m_curLighting->m_liquidPlane.d;

    if (g_theGxDevicePtr->DeviceApi() && !g_theGxDevicePtr->m_caps.unk_02BC && CShaderEffect::s_enableShaders) {
        plane = plane * this->m_matrix;
    }

    if (this->m_pass == M2PASS_3) {
        plane.x = -plane.x;
        plane.y = -plane.y;
        plane.z = -plane.z;
        plane.w = -plane.w;
    }

    CShaderEffect::SetClipPlane(*reinterpret_cast<C4Plane*>(&plane));
    CShaderEffect::SetClipEnabled(1);
}

// OFFSET: 0x0082AEE0/0x140549E50
void CM2SceneRender::SetupMaterial() {
    if (this->m_curType != this->m_prevType || this->m_curMaterial != this->m_prevMaterial || ((this->m_prevElement->m_flags ^ this->m_curElement->m_flags) & 1) || ((this->m_prevElement->m_flags ^ this->m_curElement->m_flags) & 8) || this->m_curElement->m_alpha != this->m_prevElement->m_alpha) {
        int32_t blend;

        if (this->m_curElement->m_flags & 1) {
            blend = this->m_curMaterial->m_blendMode == 1;
        } else if (this->m_curElement->m_flags & 8) {
            if (!(this->m_curMaterial->m_flags & 0x10)) {
                GxRsSet(GxRs_DepthFunc, 1);
            }

            blend = 3;
        } else {
            blend = s_blendModes[this->m_pass][this->m_curMaterial->m_blendMode];
        }

        GxRsSet(GxRs_ColorWrite, (this->m_curElement->m_flags & 1) ? 0 : 15);
        GxRsSet(GxRs_BlendingMode, blend);

        float alphaRef;

        if (!this->m_curMaterial->m_blendMode) {
            alphaRef = 0.0f;
        } else if (this->m_curMaterial->m_blendMode == 1) {
            alphaRef = static_cast<float>(CGxDevice::s_alphaRef[GxBlend_AlphaKey]) * 0.0039215689f * this->m_curElement->m_alpha;
        } else {
            alphaRef = 0.0039215689f;
        }

        CShaderEffect::SetAlphaRef(alphaRef);
    }

    if (this->m_curType != this->m_prevType || this->m_curMaterial != this->m_prevMaterial) {
        GxRsSet(GxRs_Culling, !(this->m_curMaterial->m_flags & 4) ? (this->m_sceneFlag8 != 0) + 1 : 0);
        GxRsSet(GxRs_DepthWrite, !(this->m_curMaterial->m_flags & 0x10));
    }

    if (!CShaderEffect::s_enableShaders && this->m_curType != 2) {
        if (this->m_curType > 2) {
            const C44Matrix& matrix = this->m_curModel->m_matrixF4;
            GxRsSet(GxRs_NormalizeNormals, fabsf(matrix.a2 * matrix.a2 + matrix.a1 * matrix.a1 + matrix.a0 * matrix.a0 - 1.0f) > 0.001f);
        } else if (this->m_curSkinSection->m_boneCount != 1) {
            GxRsSet(GxRs_NormalizeNormals, 1);
        } else {
            const C44Matrix& bone = this->m_curModel->m_boneMatrices[this->m_data->m_boneCombos[this->m_curSkinSection->m_boneComboIndex]];
            GxRsSet(GxRs_NormalizeNormals, fabsf(bone.a2 * bone.a2 + bone.a1 * bone.a1 + bone.a0 * bone.a0 - 1.0f) > 0.001f);
        }
    }

    M2Element* element = this->m_curElement;

    if (static_cast<int8_t>(element->m_typeKey >> 29) > 2) {
        if (this->m_curType != this->m_prevType || element->m_alpha != this->m_prevElement->m_alpha) {
            C4Vector diffuse;
            diffuse.x = 1.0f;
            diffuse.y = 1.0f;
            diffuse.z = 1.0f;
            diffuse.w = element->m_alpha;
            CShaderEffect::SetDiffuse(diffuse);

            C4Vector emissive;
            emissive.x = 0.0f;
            emissive.y = 0.0f;
            emissive.z = 0.0f;
            emissive.w = 0.0f;
            CShaderEffect::SetEmissive(emissive);
        }

        return;
    }

    if (this->m_curType == this->m_prevType && this->m_curShared == this->m_prevShared && this->m_curLocalLighting == this->m_prevLocalLighting && this->m_curMaterial->m_blendMode == this->m_prevMaterial->m_blendMode && this->m_prevBatch && this->m_curBatch->m_colorIndex == this->m_prevBatch->m_colorIndex && element->m_alpha == this->m_prevElement->m_alpha) {
        if (!(this->m_curModel->m_diffuseColor != this->m_prevModel->m_diffuseColor) && !(this->m_curModel->m_emissiveColor != this->m_prevModel->m_emissiveColor)) {
            return;
        }

        element = this->m_curElement;
    }

    C3Vector diffuse = this->m_curModel->m_diffuseColor;
    C3Vector emissive = this->m_curModel->m_emissiveColor;
    uint16_t blend = this->m_curMaterial->m_blendMode;

    if (blend == 5 || blend == 6) {
        float scale = blend == 5 ? 1.0f : 0.5f;
        diffuse.x = 0.0f;
        diffuse.y = 0.0f;
        diffuse.z = 0.0f;
        emissive.x = scale;
        emissive.y = scale;
        emissive.z = scale;
    } else {
        uint32_t colorIndex = this->m_curBatch->m_colorIndex;

        if (colorIndex < this->m_data->m_colors.m_count) {
            const C3Vector& color = this->m_curModel->m_colors[colorIndex].m_colorTrack.m_currentValue;
            diffuse.x = color.x * diffuse.x;
            diffuse.y = color.y * diffuse.y;
            diffuse.z = color.z * diffuse.z;
        }

        if (!this->m_curLocalLighting) {
            emissive.x = emissive.x + diffuse.x;
            emissive.y = emissive.y + diffuse.y;
            emissive.z = emissive.z + diffuse.z;
            diffuse.x = 0.0f;
            diffuse.y = 0.0f;
            diffuse.z = 0.0f;
        }
    }

    C4Vector color;
    color.x = diffuse.x;
    color.y = diffuse.y;
    color.z = diffuse.z;
    color.w = element->m_alpha;
    CShaderEffect::SetDiffuse(color);

    color.x = emissive.x;
    color.y = emissive.y;
    color.z = emissive.z;
    color.w = 0.0f;
    CShaderEffect::SetEmissive(color);
}

// OFFSET: 0x0082B4B0/0x14054A5C0
void CM2SceneRender::DrawBatch() {
    DebugLog("Batch: model=%s index=%d", this->m_curModel->m_shared->m_fileNameWithoutPath, this->m_curElement->m_index);

    M2Element* element = this->m_curElement;
    this->m_curBatch = element->m_batch;
    this->m_curSkinSection = element->m_skinSection;
    this->m_curMaterial = &this->m_data->m_materials[this->m_curBatch->m_materialIndex];
    element->m_effect->SetCurrent();

    if (CShaderEffect::s_enableShaders && this->m_curModel != this->m_prevModel && (this->m_curModel->m_fFlags & 0x8000000)) {
        C4Vector color0(this->m_curModel->m_guildColors[0]);
        C4Vector color1(this->m_curModel->m_guildColors[1]);
        C4Vector color2(this->m_curModel->m_guildColors[2]);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &color0.x, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, &color1.x, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 3, &color2.x, 1);
    }

    this->SetupLighting();
    this->SetupMaterial();
    this->SetupTextures();

    if (CShaderEffect::s_enableShaders && (this->m_curType != this->m_prevType || this->m_curModel != this->m_prevModel || this->m_curSkinSection->m_boneComboIndex != this->m_prevSkinSection->m_boneComboIndex)) {
        float* constants = g_theGxDevicePtr->ShaderConstantsLock(GxSh_Vertex) + 31 * 4;

        for (uint32_t i = 0; i < this->m_curSkinSection->m_boneCount; i++) {
            const C44Matrix& bone = this->m_curModel->m_boneMatrices[this->m_data->m_boneCombos[i + this->m_curSkinSection->m_boneComboIndex]];
            constants[0] = bone.a0;
            constants[1] = bone.b0;
            constants[2] = bone.c0;
            constants[3] = bone.d0;
            constants[4] = bone.a1;
            constants[5] = bone.b1;
            constants[6] = bone.c1;
            constants[7] = bone.d1;
            constants[8] = bone.a2;
            constants[9] = bone.b2;
            constants[10] = bone.c2;
            constants[11] = bone.d2;
            constants += 12;
        }

        g_theGxDevicePtr->ShaderConstantsUnlock(GxSh_Vertex, 31, 3 * this->m_curSkinSection->m_boneCount);
    }

    if (this->m_curElement->m_flags & 4) {
        if (this->m_curType != this->m_prevType || this->m_curModel != this->m_prevModel) {
            this->m_curModel->SetIndices();
        }
    } else if (this->m_curType != this->m_prevType || this->m_curShared != this->m_prevShared || (this->m_prevElement->m_flags & 4)) {
        this->m_curShared->SetIndices();
    }

    int32_t sharedVertices = this->m_curModel->m_shared->m_data->m_bones.m_count == 1 && (this->m_cache->m_flags & 0x20);
    this->SetBatchVertices(sharedVertices);
    CShaderEffect::SetShaders(this->m_curElement->m_vertexPermute, this->m_curElement->m_hullPermute, this->m_curElement->m_domainPermute, this->m_curElement->m_pixelPermute);

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = this->m_curSkinSection->m_indexStart;
    batch.m_count = this->m_curSkinSection->m_indexCount;

    if (CShaderEffect::s_enableShaders) {
        batch.m_minIndex = this->m_curSkinSection->m_vertexStart;
        batch.m_maxIndex = this->m_curSkinSection->m_vertexCount + this->m_curSkinSection->m_vertexStart - 1;
        g_theGxDevicePtr->Draw(batch);
    } else if (sharedVertices) {
        CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
        world.m_dirty = 1;
        world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
        world.m_mtx[world.m_level] = this->m_curModel->m_boneMatrices[0];

        batch.m_minIndex = this->m_curSkinSection->m_vertexStart;
        batch.m_maxIndex = this->m_curSkinSection->m_vertexCount + this->m_curSkinSection->m_vertexStart - 1;
        g_theGxDevicePtr->Draw(batch);

        CGxMatrixStack& restore = g_theGxDevicePtr->m_xforms[GxXform_World];
        restore.m_dirty = 1;
        restore.m_flags[restore.m_level] &= ~CGxMatrixStack::F_Identity;
        restore.m_mtx[restore.m_level] = s_identity;
    } else {
        batch.m_minIndex = 0;
        batch.m_maxIndex = this->m_curSkinSection->m_vertexCount - 1;
        g_theGxDevicePtr->Draw(batch);
    }

    CShaderEffect::ResetShaders(this->m_curElement->m_hullPermute, this->m_curElement->m_domainPermute);
}

// OFFSET: 0x0082B8D0/0x14054AB40
void CM2SceneRender::DrawBatchProj() {
    if ((this->m_curModel->m_fFlags & 0x10000000) && !(this->m_curModel->m_fFlags & 4)) {
        return;
    }

    DebugLog("BatchProj: model=%s index=%d", this->m_curModel->m_shared->m_fileNameWithoutPath, this->m_curElement->m_index);

    M2Element* element = this->m_curElement;
    this->m_curBatch = element->m_batch;
    this->m_curSkinSection = element->m_skinSection;
    this->m_curMaterial = &this->m_data->m_materials[this->m_curBatch->m_materialIndex];

    CAaBox box;
    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.b.z = 0.0f;
    box.t.x = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;

    C44Matrix texMatrix;

    if (!this->m_curModel->GetRegionBounds(this->m_curSkinSection, box, texMatrix)) {
        return;
    }

    box.t.z = box.t.z + 2.0f;
    box.b.z = box.b.z - 2.0f;

    C44Matrix depthMatrix;
    depthMatrix.a0 = 0.0f;
    depthMatrix.a1 = 0.0f;
    depthMatrix.a2 = 0.0f;
    depthMatrix.a3 = 0.0f;
    depthMatrix.b0 = 0.0f;
    depthMatrix.b1 = 0.0f;
    depthMatrix.b2 = 0.0f;
    depthMatrix.b3 = 0.0f;
    depthMatrix.c1 = 0.0f;
    depthMatrix.c2 = 0.0f;
    depthMatrix.c3 = 0.0f;
    depthMatrix.d2 = 0.0f;
    depthMatrix.c0 = 1.0f / (box.t.z - box.b.z);
    depthMatrix.d0 = (this->m_scene->m_viewInv.d2 - box.b.z) * depthMatrix.c0;
    depthMatrix.d1 = 0.5f;
    depthMatrix.d3 = 1.0f;

    C3Vector color = this->m_curModel->m_diffuseColor;
    uint32_t colorIndex = this->m_curBatch->m_colorIndex;

    if (colorIndex < this->m_data->m_colors.m_count) {
        const C3Vector& modelColor = this->m_curModel->m_colors[colorIndex].m_colorTrack.m_currentValue;
        color.x = modelColor.x * color.x;
        color.y = modelColor.y * color.y;
        color.z = modelColor.z * color.z;
    }

    color.x = color.x + this->m_curModel->m_emissiveColor.x;
    color.y = color.y + this->m_curModel->m_emissiveColor.y;
    color.z = color.z + this->m_curModel->m_emissiveColor.z;

    CImVector imColor;
    imColor.r = static_cast<uint8_t>(static_cast<int32_t>((color.x >= 0.0f ? (color.x >= 1.0f ? 1.0f : color.x) : 0.0f) * 255.0f));
    imColor.g = static_cast<uint8_t>(static_cast<int32_t>((color.y >= 0.0f ? (color.y >= 1.0f ? 1.0f : color.y) : 0.0f) * 255.0f));
    imColor.b = static_cast<uint8_t>(static_cast<int32_t>((color.z >= 0.0f ? (color.z >= 1.0f ? 1.0f : color.z) : 0.0f) * 255.0f));

    float alpha = this->m_curElement->m_alpha;
    imColor.a = static_cast<uint8_t>(static_cast<int32_t>((alpha >= 0.0f ? (alpha >= 1.0f ? 1.0f : alpha) : 0.0f) * 255.0f));

    this->SetupTextures();
    this->SetupLighting();
    this->SetupMaterial();

    CShaderEffect* effect;

    if (g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0] == 4) {
        effect = this->m_curLocalLighting ? this->m_projectedModAddEffect : this->m_projectedModAddUnlitEffect;
    } else {
        effect = this->m_curLocalLighting ? this->m_projectedModModEffect : this->m_projectedModModUnlitEffect;
    }

    effect->SetCurrent();
    this->SetupBillboardView(*reinterpret_cast<const C3Vector*>(&this->m_scene->m_viewInv.d0));
    CShaderEffect::SetTexMtx_TexGen(GxTexGen_EyeLinear, texMatrix, 0);
    CShaderEffect::SetTexMtx_TexGen(GxTexGen_EyeLinear, depthMatrix, 1);
    this->m_scene->m_projectTextureCallback(box, imColor, this->m_curLocalLighting, this->m_scene->m_projectTextureParam, (this->m_curModel->m_fFlags >> 2) & 1);
    g_theGxDevicePtr->XformSetView(s_identity);

    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
    world.m_mtx[world.m_level] = s_identity;
}

// OFFSET: 0x0082BD00/0x14054B140
void CM2SceneRender::DrawBatchDoodad(M2Element* elements, uint32_t count, uint32_t* indices) {
    DebugLog("BatchDoodad: model=%s index=%d", this->m_curModel->m_shared->m_fileNameWithoutPath, this->m_curElement->m_index);

    M2Element* element = this->m_curElement;
    this->m_curBatch = element->m_batch;
    this->m_curSkinSection = element->m_skinSection;
    this->m_curMaterial = &this->m_data->m_materials[this->m_curBatch->m_materialIndex];
    element->m_effect->SetCurrent();

    this->SetupLighting();
    this->SetupMaterial();
    this->SetupTextures();

    uint32_t instances = this->m_curShared->AllocInstances(count, 0);

    if (instances > count) {
        instances = count;
    }

    this->m_curShared->SetIndices();

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = this->m_curSkinSection->m_indexStart;
    batch.m_count = 0;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 0;

    if (CShaderEffect::s_enableShaders) {
        this->m_curShared->SetVertices(0, GxVBF_PBNT2);

        for (uint32_t done = 0; done < count; done += instances) {
            if (done + instances > count) {
                instances = count - done;
            }

            float* constants = g_theGxDevicePtr->ShaderConstantsLock(GxSh_Vertex) + 31 * 4;
            uint32_t registers = 0;

            for (uint32_t n = 0; n < instances; n++) {
                CM2Model* model = elements[indices[done + n]].m_model;

                for (uint32_t i = 0; i < this->m_curSkinSection->m_boneCount; i++) {
                    const C44Matrix& bone = model->m_boneMatrices[this->m_data->m_boneCombos[i + this->m_curSkinSection->m_boneComboIndex]];
                    constants[0] = bone.a0;
                    constants[1] = bone.b0;
                    constants[2] = bone.c0;
                    constants[3] = bone.d0;
                    constants[4] = bone.a1;
                    constants[5] = bone.b1;
                    constants[6] = bone.c1;
                    constants[7] = bone.d1;
                    constants[8] = bone.a2;
                    constants[9] = bone.b2;
                    constants[10] = bone.c2;
                    constants[11] = bone.d2;
                    constants += 12;
                    registers++;
                }
            }

            g_theGxDevicePtr->ShaderConstantsUnlock(GxSh_Vertex, 31, 3 * registers);
            CShaderEffect::SetShaders(this->m_curElement->m_vertexPermute, this->m_curElement->m_hullPermute, this->m_curElement->m_domainPermute, this->m_curElement->m_pixelPermute);

            batch.m_count = instances * this->m_curSkinSection->m_indexCount;
            batch.m_maxIndex = static_cast<uint16_t>(instances * this->m_curShared->m_skinData->m_vertices.m_count - 1);
            g_theGxDevicePtr->Draw(batch);
            CShaderEffect::ResetShaders(this->m_curElement->m_hullPermute, this->m_curElement->m_domainPermute);
        }
    } else {
        for (uint32_t done = 0; done < count; done += instances) {
            if (done + instances > count) {
                instances = count - done;
            }

            CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 32, instances * this->m_curSkinSection->m_vertexCount);
            uint8_t* vertices = reinterpret_cast<uint8_t*>(g_theGxDevicePtr->BufLock(buf));

            if (!vertices) {
                return;
            }

            M2SkinSection* section = this->m_curSkinSection;
            int32_t normalize = section->m_boneCount != 1;

            for (uint32_t n = 0; n < instances; n++) {
                CM2Model* model = elements[indices[done + n]].m_model;
                const M2Vertex* source = &model->m_shared->m_data->m_vertices[section->m_vertexStart];

                if (section->m_boneInfluences > 1) {
                    SIMD::s_functions.SkinVertices(vertices, source, section->m_vertexCount, model->m_boneMatrices, 0);
                } else {
                    SIMD::s_functions.SkinVerticesOneBone(vertices, source, section->m_vertexCount, model->m_boneMatrices, 0);
                }

                section = this->m_curSkinSection;
                vertices += 32 * section->m_vertexCount;

                if (!normalize) {
                    const C44Matrix& bone = model->m_boneMatrices[this->m_data->m_boneCombos[section->m_boneComboIndex]];
                    normalize = fabsf(bone.a2 * bone.a2 + bone.a1 * bone.a1 + bone.a0 * bone.a0 - 1.0f) > 0.001f;
                }
            }

            g_theGxDevicePtr->BufUnlock(buf, 0);
            buf->unk_001C = 1;
            GxPrimVertexPtr(buf, GxVBF_PNT);
            GxRsSet(GxRs_NormalizeNormals, normalize);

            batch.m_count = instances * this->m_curSkinSection->m_indexCount;
            batch.m_maxIndex = static_cast<uint16_t>(instances * this->m_curSkinSection->m_vertexCount - 1);
            g_theGxDevicePtr->Draw(batch);
        }
    }
}

// OFFSET: 0x0082C1B0/0x14054B730
void CM2SceneRender::DrawRibbon() {
    DebugLog("Ribbon: model=%s index=%d", this->m_curModel->m_shared->m_fileNameWithoutPath, this->m_curElement->m_index);

    this->m_curMaterial = &this->m_data->m_materials[this->m_data->m_ribbons[this->m_curElement->m_index].m_materialIndices[0]];
    this->m_particleUnlitEffect->SetCurrent();
    this->SetupTextures();
    this->SetupLighting();
    this->SetupMaterial();
    this->SetupBillboardView(C3Vector(0.0f, 0.0f, 0.0f));
    CShaderEffect::SetTexMtx_Identity(0);
    CRibbonEmitter::s_enableFog = fabsf(this->m_curLighting->m_fogScale) >= 0.00000023841858f;
    this->m_curModel->m_ribbonEmitters[this->m_curElement->m_index]->Render(static_cast<const C44Matrix*>(this->m_curModel->m_frameOfReference));
    g_theGxDevicePtr->XformSetView(s_identity);

    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
    world.m_mtx[world.m_level] = s_identity;
}

// OFFSET: 0x0082C2E0/0x14054B9E0
void CM2SceneRender::RenderBatchedParticle(CParticleEmitter2* emitter, CGxBuf* buf, uint32_t vertexCount, uint32_t indexCount) {
    CShaderEffect* effect = (emitter->m_params.m_materialFlags & 1) ? this->m_particleEffect : this->m_particleUnlitEffect;

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(emitter->m_texture));
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    effect->SetCurrent();
    this->SetupLighting();
    this->SetupMaterial();
    emitter->RenderParticleVertices(buf, vertexCount, indexCount);
}

// OFFSET: 0x0082C390/0x14054BAD0
uint32_t CM2SceneRender::DrawBatchedParticle(uint32_t index, M2Element* elements, uint32_t* indices, uint32_t count) {
    uint32_t* first = &indices[index];
    M2Element* element = &elements[*first];
    CParticleEmitter2* emitter = element->m_emitter;
    EGxVertexBufferFormat format = (emitter->m_params.m_materialFlags & 1) ? GxVBF_PNCT : GxVBF_PCT;
    uint32_t vertexSize = GxVertexSize(format);
    uint32_t materialBlend = emitter->m_params.m_materialBlend;
    CTexture* texture = emitter->m_texture;
    uint32_t live = emitter->m_liveList.m_count;
    uint32_t clip = element->m_flags & 2;
    uint32_t maxVertices = 0x100000 / vertexSize;
    uint32_t materialFlags = emitter->m_params.m_materialFlags;
    float alpha = element->m_alpha;
    CM2Lighting* lighting = element->m_model->m_currentLighting;
    void* frameOfReference = element->m_model->m_frameOfReference;
    uint32_t particles = 0x4000 / emitter->m_vertsPerParticle;

    if (particles >= live) {
        particles = live;
    }

    uint32_t vertices = particles * emitter->m_vertsPerParticle;
    particles = 0x4000 / emitter->m_vertsPerParticle;

    if (particles >= live) {
        particles = live;
    }

    uint32_t indexCount = particles * emitter->m_indicesPerParticle;
    uint32_t batched = 1;

    for (uint32_t i = index + 1; i < count; i++) {
        M2Element* other = &elements[indices[i]];

        if ((other->m_typeKey & 0xE0000000) != 0x80000000) {
            break;
        }

        CParticleEmitter2* otherEmitter = other->m_emitter;
        uint32_t otherLive = otherEmitter->m_liveList.m_count;
        uint32_t otherParticles = 0x4000 / otherEmitter->m_vertsPerParticle;

        if (otherParticles >= otherLive) {
            otherParticles = otherEmitter->m_liveList.m_count;
        }

        uint32_t otherVertices = otherParticles * otherEmitter->m_vertsPerParticle;
        otherParticles = 0x4000 / otherEmitter->m_vertsPerParticle;

        if (otherParticles >= otherLive) {
            otherParticles = otherLive;
        }

        uint32_t otherIndices = otherParticles * otherEmitter->m_indicesPerParticle;

        if (otherEmitter->m_params.m_materialBlend != materialBlend) {
            break;
        }

        uint32_t lit = otherEmitter->m_params.m_materialFlags & 1;

        if (lit != (materialFlags & 1) || ((otherEmitter->m_params.m_materialFlags ^ materialFlags) & 2) || ((otherEmitter->m_params.m_materialFlags ^ materialFlags) & 4) || otherEmitter->m_texture != texture || other->m_model->m_frameOfReference != frameOfReference || other->m_alpha != alpha) {
            break;
        }

        otherVertices += vertices;

        if (otherVertices > maxVertices) {
            break;
        }

        otherIndices += indexCount;

        if (otherIndices > 0x20000 || (lit && !CM2SceneRender::LightingColorsMatch(other->m_model->m_currentLighting, lighting))) {
            break;
        }

        uint32_t otherClip = other->m_flags & 2;

        if (otherClip != clip || (otherClip && lighting->m_liquidPlane != other->m_model->m_currentLighting->m_liquidPlane)) {
            break;
        }

        indexCount = otherIndices;
        batched++;
        vertices = otherVertices;
    }

    if (!vertices || !indexCount) {
        this->m_curType = -1;
        return 0;
    }

    DebugLog("ParticleBatch");

    CGxMatrixStack& viewStack = g_theGxDevicePtr->m_xforms[GxXform_View];
    C44Matrix view = viewStack.m_mtx[viewStack.m_level];
    this->SetupBillboardView(*reinterpret_cast<const C3Vector*>(&this->m_scene->m_viewInv.d0));

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, vertexSize, vertices);
    uint8_t* data = reinterpret_cast<uint8_t*>(g_theGxDevicePtr->BufLock(buf));
    uint32_t renderedVertices = 0;
    uint32_t renderedIndices = 0;

    for (uint32_t i = 0; i < batched; i++) {
        emitter = elements[first[i]].m_emitter;
        emitter->Render(static_cast<const C44Matrix*>(this->m_curModel->m_frameOfReference), data, i == 0);
        renderedVertices += emitter->m_renderedParticles * emitter->m_vertsPerParticle;
        data += vertexSize * emitter->m_renderedParticles * emitter->m_vertsPerParticle;
        renderedIndices += emitter->m_renderedParticles * emitter->m_indicesPerParticle;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;

    if (renderedVertices && renderedIndices) {
        this->RenderBatchedParticle(emitter, buf, renderedVertices, renderedIndices);
    } else {
        this->m_curType = -1;
    }

    g_theGxDevicePtr->XformSetView(view);

    return batched - 1;
}

// OFFSET: 0x0082C850/0x14054C0D0
uint32_t CM2SceneRender::DrawParticle(uint32_t index, M2Element* elements, uint32_t* indices, uint32_t count) {
    CParticleEmitter2* emitter = this->m_curElement->m_emitter;
    uint32_t flags = emitter->m_params.m_materialFlags;
    M2Material material;
    material.m_flags = 4;

    if (!(flags & 1)) {
        material.m_flags = 5;
    }

    if (!(flags & 2)) {
        material.m_flags |= 2;
    }

    if (!(flags & 4)) {
        material.m_flags |= 0x10;
    }

    material.m_blendMode = static_cast<uint16_t>(ParticleBlendToM2Blend(emitter->m_params.m_materialBlend));

    this->m_curMaterial = &material;
    this->m_prevMaterial = NULL;

    if (this->m_cache->m_flags & 0x40) {
        return this->DrawBatchedParticle(index, elements, indices, count);
    }

    CGxTex* gxTex = TextureGetGxTex(emitter->m_texture);

    if (!gxTex) {
        return 0;
    }

    DebugLog("Particle: model=%s", this->m_curModel->m_shared->m_fileNameWithoutPath);

    CShaderEffect* effect = (emitter->m_params.m_materialFlags & 1) ? this->m_particleEffect : this->m_particleUnlitEffect;
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    effect->SetCurrent();
    this->SetupLighting();
    this->SetupMaterial();
    this->SetupBillboardView(*reinterpret_cast<const C3Vector*>(&this->m_scene->m_viewInv.d0));
    emitter->Render(static_cast<const C44Matrix*>(this->m_curModel->m_frameOfReference), NULL, 1);
    g_theGxDevicePtr->XformSetView(s_identity);
    GxXformSet(GxXform_World, s_identity);

    return 0;
}

// OFFSET: 0x0082F200/0x14054F8F0
void CM2SceneRender::BeginFixed(const C3Vector& position) {
    this->SetupTextures();
    this->SetupLighting();
    this->SetupMaterial();
    GxRsSet(GxRs_VertexShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_PixelShader, static_cast<void*>(NULL));
    this->SetupBillboardView(position);
    g_theGxDevicePtr->RsPush();
}

// OFFSET: 0x0082F290/0x14054F9C0
void CM2SceneRender::DrawCallback() {
    DebugLog("Callback");

    this->BeginFixed(C3Vector(0.0f, 0.0f, 0.0f));
    this->m_curModel->m_drawCallback(this->m_curModel, this->m_curModel->m_currentLighting, this->m_curModel->m_drawArg);
    CM2SceneRender::EndFixed();
    CShaderEffect::UpdateProjMatrix();
    CShaderEffect::FlushFogParams();
}

// OFFSET: 0x0082F3C0/0x14054FC40
void CM2SceneRender::Draw(M2PASS pass, M2Element* elements, uint32_t* indices, uint32_t* counts, uint32_t count) {
    if (!count) {
        return;
    }

    this->m_pass = static_cast<M2PASS>(-1);
    this->m_prevElement = NULL;
    this->m_curElement = NULL;
    this->m_prevType = -1;
    this->m_curType = -1;
    this->m_prevModel = NULL;
    this->m_curModel = NULL;
    this->m_prevShared = NULL;
    this->m_curShared = NULL;
    this->m_data = NULL;
    this->m_prevLighting = NULL;
    this->m_curLighting = NULL;
    this->m_prevLocalLighting = 0;
    this->m_curLocalLighting = 0;
    this->m_prevFogMode = 0xFFFFFFFF;
    this->m_curFogMode = 0xFFFFFFFF;
    this->m_prevBatch = NULL;
    this->m_curBatch = NULL;
    this->m_prevSkinSection = NULL;
    this->m_curSkinSection = NULL;
    this->m_prevMaterial = NULL;
    this->m_curMaterial = NULL;

    g_theGxDevicePtr->RsPush();

    CGxMatrixStack& viewStack = g_theGxDevicePtr->m_xforms[GxXform_View];
    C44Matrix view = viewStack.m_mtx[viewStack.m_level];

    for (uint32_t xf = GxXform_Tex0; xf <= GxXform_World; xf++) {
        g_theGxDevicePtr->XformPush(static_cast<EGxXform>(xf));
    }

    g_theGxDevicePtr->XformSetView(s_identity);

    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
    world.m_mtx[world.m_level] = s_identity;

    GxRsSet(GxRs_DepthFunc, 0);
    GxRsSet(GxRs_PolygonOffset, 0);
    GxRsSet(GxRs_Lighting, 0);

    CImVector fogColor(0xFF, 0xFF, 0xFF, 0xFF);
    CShaderEffect::SetFogParams(0.0f, 1000.0f, 1.0f, fogColor, 0.0f);
    GxRsSet(GxRs_Fog, 0);

    if (CShaderEffect::s_enableShaders) {
        C44Matrix projection = g_theGxDevicePtr->m_projNative;

        if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
            projection.c0 = projection.c0 * -1.0f;
            projection.c1 = projection.c1 * -1.0f;
            projection.c2 = projection.c2 * -1.0f;
            projection.c3 = projection.c3 * -1.0f;
        }

        this->m_matrix = projection.Inverse(projection.Determinant());
        this->m_matrix = this->m_matrix.Transpose();
        CShaderEffect::UpdateProjMatrix();
        CShadowCache::SetShadowMapGenericGlobal();

        C4Vector params = ViewSettings::CalculateParams();
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 22, &params.x, 1);

        if (CShaderEffect::TesselationEnabled()) {
            CShaderEffect::UpdateTesselation();
        } else if (CShaderEffect::DisplacementEnabled()) {
            CShaderEffect::UpdateDisplacement();
        }
    }

    this->m_pass = pass;

    for (uint32_t i = 0; i < count; i++) {
        M2Element* element = &elements[indices[i]];

        if (!counts && (element->m_model->m_fFlags & 0x1000) && (element->m_typeKey & 0xE0000000) != 0x80000000) {
            continue;
        }

        this->m_curElement = element;
        this->m_curType = element->m_typeKey >> 29;
        this->m_curModel = element->m_model;
        this->m_curShared = this->m_curModel->m_shared;
        this->m_curLighting = this->m_curModel->m_currentLighting;
        this->m_curBatch = NULL;
        this->m_curSkinSection = NULL;
        this->m_curLocalLighting = 1;
        this->m_curFogMode = 1;
        this->m_curMaterial = &this->m_defaultMaterial;
        this->m_data = this->m_curShared->m_data;
        this->m_cache->LinkToSharedUpdateList(this->m_curShared);

        switch (this->m_curElement->m_typeKey >> 29) {
            case 0:
                this->DrawBatchProj();
                break;
            case 1:
                this->DrawBatch();
                break;
            case 2:
                this->DrawBatchDoodad(elements, counts[i], &indices[i]);
                i += counts[i] - 1;
                break;
            case 3:
                this->DrawRibbon();
                break;
            case 4:
                i += this->DrawParticle(i, elements, indices, count);
                break;
            case 5:
                this->DrawCallback();
                break;
            default:
                continue;
        }

        if (this->m_curElement->m_flags & 8) {
            GxRsSet(GxRs_DepthFunc, 0);
        }

        this->m_prevElement = this->m_curElement;
        this->m_prevType = this->m_curType;
        this->m_prevModel = this->m_curModel;
        this->m_prevShared = this->m_curShared;
        this->m_prevLighting = this->m_curLighting;
        this->m_prevLocalLighting = this->m_curLocalLighting;
        this->m_prevFogMode = this->m_curFogMode;
        this->m_prevBatch = this->m_curBatch;
        this->m_prevSkinSection = this->m_curSkinSection;
        this->m_prevMaterial = this->m_curMaterial;
    }

    for (uint32_t xf = GxXform_Tex0; xf <= GxXform_World; xf++) {
        CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];

        if (stack.m_level) {
            stack.m_level--;
        }

        stack.m_dirty = 1;
    }

    g_theGxDevicePtr->XformSetView(view);
    g_theGxDevicePtr->RsPop();
    GxRsSet(GxRs_Fog, 0);
    CShaderEffect::SetClipEnabled(0);
}
