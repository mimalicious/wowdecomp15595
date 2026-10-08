#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Services/FileNotification.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: %0x00C3CBF0
CTexture* CShaderEffect::s_displTex;

// STATIC: %0x00C3CBF4
int32_t CShaderEffect::s_forceAlphaTest;

// STATIC: %0x00C3CBF8
int32_t CShaderEffect::s_clipEnabled;

// STATIC: %0x00C3CBFC
int32_t CShaderEffect::s_useAlphaRef;

// STATIC: 0x00D381B0
float CShaderEffect::s_fogMul;

// STATIC: %0x00C3CC04
uint32_t CShaderEffect::s_usePcfFiltering;

// STATIC: %0x00C3CC08
uint32_t CShaderEffect::s_localLightCount;

// STATIC: %0x00C3CC0C
int32_t CShaderEffect::s_lightEnabled;

// STATIC: %0x00C3CC10
float CShaderEffect::s_terrainDisplFactor;

// STATIC: %0x00C3CC14
float CShaderEffect::s_displFactor;

// STATIC: %0x00C3CC18
float CShaderEffect::s_tesselationDist;

// STATIC: %0x00C3CC1C
float CShaderEffect::s_tesselationFactor;

// STATIC: %0x00C3CC20
int32_t CShaderEffect::s_enableShaders;

// STATIC: %0x00C3CC24
CShaderEffect* CShaderEffect::s_curEffect;

// STATIC: %0x00C3CC28
C3Vector CShaderEffect::s_sunDiffuse;

// STATIC: %0x00C3CC34
C3Vector CShaderEffect::s_sunAmbient;

// STATIC: %0x00C3CC40
C3Vector CShaderEffect::s_sunDir;

// STATIC: %0x00C3CC4C
C3Vector CShaderEffect::s_sunSpec;

// STATIC: %0x00C3CC58
LocalLights CShaderEffect::s_localLights;

// STATIC: 0x00D382B8
C4Vector CShaderEffect::s_fogColorAlphaRef;

// STATIC: 0x00D382C8
C4Vector CShaderEffect::s_fogParams;

// STATIC: %0x00C3CD48
C4Plane CShaderEffect::s_clipPlane;

// STATIC: #0x00D43010
uint32_t CShaderEffect::s_shadowValue;

// OFFSET: 0x00431CB0
uint32_t CShaderEffect::ComputeVertexPermute(int32_t option0, int32_t option1, int32_t lighting, uint32_t lightCount, uint32_t texCoords, uint32_t shadow) {
    uint32_t shadowIndex = shadow > 1 ? 1 : shadow;
    uint32_t texCoordIndex = texCoords > 2 ? 2 : texCoords;
    uint32_t lightIndex = 0;

    if (lighting) {
        lightIndex = (lightCount > 2 ? 2 : lightCount) + 1;
    }

    uint32_t permute = lightIndex + 4 * (texCoordIndex + 3 * shadowIndex);
    permute = (option0 != 0) + 2 * permute;
    return (option1 != 0) + 2 * permute;
}

// PROVEN: 551677b8 IDENTICAL/UNPROVEN
// OFFSET: 0x00431D80/0x14003A550
static void ReloadShadersCallback(const char* fileName) {
    g_theGxDevicePtr->ShaderReloadAll();
}

// PROVEN: 8fdcd3b0 IDENTICAL/IDENTICAL
// OFFSET: 0x004319F0/0x14003A2B0
void CShaderEffect::AddRef() {
    this->m_refCount++;
}

// PROVEN: d4ff236e IDENTICAL/UNPROVEN
// OFFSET: 0x00431E30/0x14003A610
void CShaderEffect::InitEffect(const char* vertexShader, const char* hullShader, const char* domainShader, const char* pixelShader) {
    this->m_vertexShader = NULL;
    this->m_hullShader = NULL;
    this->m_domainShader = NULL;
    this->m_pixelShader = NULL;
    this->m_fixedFuncPassCount = 0;

    if (!CShaderEffect::s_enableShaders || !vertexShader || !pixelShader) {
        return;
    }

    this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, vertexShader);
    this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, pixelShader);

    if (hullShader && *hullShader && domainShader && *domainShader) {
        this->m_hullShader = GxShaderCreate(GxSh_Hull, hullShader);
        this->m_domainShader = GxShaderCreate(GxSh_Domain, domainShader);
    }
}

// PROVEN: ce4838bb IDENTICAL/IDENTICAL
// OFFSET: 0x00431A00/0x14003A2C0
void CShaderEffect::InitFixedFuncPass(const EGxTexOp* colorOps, const EGxTexOp* alphaOps, uint32_t count) {
    this->m_fixedFuncPassCount = count;

    for (uint32_t i = 0; i < count; i++) {
        this->m_colorOps[i] = colorOps[i];
        this->m_alphaOps[i] = alphaOps[i];
    }
}

// OFFSET: 0x00431EC0
uint32_t CShaderEffect::SelectShadowShader(uint32_t shadow) {
    if (!(CShaderEffect::s_fogColorAlphaRef.w > 0.0f) || (g_theGxDevicePtr->m_caps.unk_02F4 && (CShaderEffect::s_forceAlphaTest || !shadow))) {
        return shadow + 4 * CShaderEffect::s_usePcfFiltering;
    }

    return shadow + 4 * (CShaderEffect::s_usePcfFiltering + 2);
}

// OFFSET: 0x00431F20
void CShaderEffect::SetGlobalLighting(CM2Light* light) {
    CShaderEffect::s_sunDir = light->m_dir;
    CShaderEffect::s_sunSpec = light->m_specColor;

    C44Matrix view;
    GxXformView(view);

    C3Vector sunDir;
    sunDir.x = view.c0 * CShaderEffect::s_sunDir.z + view.b0 * CShaderEffect::s_sunDir.y + view.a0 * CShaderEffect::s_sunDir.x;
    sunDir.y = CShaderEffect::s_sunDir.z * view.c1 + CShaderEffect::s_sunDir.y * view.b1 + CShaderEffect::s_sunDir.x * view.a1;
    sunDir.z = CShaderEffect::s_sunDir.z * view.c2 + CShaderEffect::s_sunDir.y * view.b2 + CShaderEffect::s_sunDir.x * view.a2;
    CShaderEffect::s_sunDir = sunDir;

    if (CShaderEffect::s_enableShaders) {
        C4Vector sunSpec;
        sunSpec.x = CShaderEffect::s_sunSpec.x;
        sunSpec.y = CShaderEffect::s_sunSpec.y;
        sunSpec.z = CShaderEffect::s_sunSpec.z;
        sunSpec.w = 14.0f;

        GxShaderConstantsSet(GxSh_Vertex, 12, &CShaderEffect::s_sunDir.x, 1);
        GxShaderConstantsSet(GxSh_Vertex, 13, &sunSpec.x, 1);
    }
}

// OFFSET: 0x004320D0
void CShaderEffect::ComputeLocalLights(LocalLights* localLights, uint32_t lightCount, CM2Light** lights, const C3Vector* origin) {
    uint32_t i = 0;

    if (lightCount) {
        C44Matrix view;

        if (origin) {
            GxXformView(view);
        }

        for (; i < lightCount; i++) {
            CM2Light* light = lights[i];

            if (light->m_type == M2LIGHT_POINT) {
                localLights->m_color[i].x = light->m_dirColor.x;
                localLights->m_color[i].y = light->m_dirColor.y;
                localLights->m_color[i].z = light->m_dirColor.z;
                localLights->m_color[i].w = 1.0f;

                C3Vector position;

                if (origin) {
                    position = (light->m_pos - *origin) * view;
                } else {
                    position = light->m_viewPos;
                }

                localLights->m_position[i].x = position.x;
                localLights->m_position[i].y = position.y;
                localLights->m_position[i].z = position.z;
                localLights->m_position[i].w = 1.0f;

                localLights->m_constantAttenuation[i] = light->m_constantAttenuation;
                localLights->m_linearAttenuation[i] = light->m_linearAttenuation;
                localLights->m_quadraticAttenuation[i] = light->m_quadraticAttenuation;
            } else {
                localLights->m_color[i].x = 0.0f;
                localLights->m_color[i].y = 0.0f;
                localLights->m_color[i].z = 0.0f;
                localLights->m_color[i].w = 0.0f;
            }
        }
    }

    for (; i < 4; i++) {
        localLights->m_color[i].x = 0.0f;
        localLights->m_color[i].y = 0.0f;
        localLights->m_color[i].z = 0.0f;
        localLights->m_color[i].w = 0.0f;
        localLights->m_constantAttenuation[i] = 1.0f;
    }
}

// PROVEN: f2b44fbb IDENTICAL/IDENTICAL
// OFFSET: 0x00431A40/0x14003A300
void CShaderEffect::SetUsePcfFiltering(uint32_t usePcfFiltering) {
    if (usePcfFiltering) {
        CShaderEffect::s_usePcfFiltering = 1;

        if (CShaderEffect::s_enableShaders) {
            return;
        }
    }

    CShaderEffect::s_usePcfFiltering = 0;
}

// PROVEN: 3dda5674 IDENTICAL/UNPROVEN
// OFFSET: 0x00431A70/0x14003A330
void CShaderEffect::GetClipPlane(C4Plane& plane) {
    plane = CShaderEffect::s_clipPlane;
}

// PROVEN: b8b6f07e IDENTICAL/DIFFERENT
// OFFSET: 0x00432960/0x14003B380
void CShaderEffect::SetClipPlane(const C4Plane& plane) {
    CShaderEffect::s_clipPlane = plane;

    if (CShaderEffect::s_enableShaders && g_theGxDevicePtr->m_caps.unk_02BC) {
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 14, &plane.n.x, 1);
    } else {
        g_theGxDevicePtr->ClipPlaneSet(0, plane);
    }
}

// PROVEN: fc27580e IDENTICAL/NONE
// OFFSET: 0x00431AA0/0x14003A360
int32_t CShaderEffect::GetClipEnabled() {
    return CShaderEffect::s_clipEnabled;
}

// PROVEN: 9554c981 IDENTICAL/DIFFERENT
// OFFSET: 0x004329C0/0x14003B3F0
void CShaderEffect::FlushFogParams() {
    if (!g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog) || !CShaderEffect::s_enableShaders) {
        return;
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 30, &CShaderEffect::s_fogParams.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, &CShaderEffect::s_fogColorAlphaRef.x, 1);
}

// PROVEN: d0bf0f63 IDENTICAL/IDENTICAL
// OFFSET: 0x00431AB0/0x14003A370
int32_t CShaderEffect::TesselationEnabled() {
    return CShaderEffect::s_enableShaders && CShaderEffect::s_tesselationFactor != 0.0f;
}

// PROVEN: 4bd2c4b5 IDENTICAL/IDENTICAL
// OFFSET: 0x00431AE0/0x14003A3A0
int32_t CShaderEffect::DisplacementEnabled() {
    return CShaderEffect::s_enableShaders && CShaderEffect::s_displFactor != 0.0f;
}

// PROVEN: d251fdb9 IDENTICAL/NONE
// OFFSET: 0x00431B10
int32_t CShaderEffect::TerrainDisplEnabled() {
    return CShaderEffect::s_enableShaders && CShaderEffect::s_terrainDisplFactor != 0.0f;
}

// OFFSET: 0x00431D90/0x14003A560
void CShaderEffect::InitShaderSystem(int32_t enableShaders, int32_t usePcfFiltering) {
    CShaderEffect::s_enableShaders = enableShaders;
    CShaderEffect::SetUsePcfFiltering(usePcfFiltering);
    CShaderEffect::s_fogMul = 1.0f;
    CShaderEffect::s_forceAlphaTest = 0;
    CShaderEffect::s_useAlphaRef = g_theGxDevicePtr->m_caps.unk_02F4;

    if (g_theGxDevicePtr->m_caps.m_vendorId == 0x8086) {
        CShaderEffect::s_forceAlphaTest = 1;
        GxLog("Forcing AlphaTest on Intel GPUs instead of using texkill!");
    }

    if (!g_theGxDevicePtr->m_caps.unk_02F4) {
        CShaderEffect::s_forceAlphaTest = 0;
    }

    FileNotification::RegisterSingleFile("bls", ReloadShadersCallback);
}

// OFFSET: 0x00433260/0x14003B4B0
void CShaderEffect::SetCurrent() {
    CShaderEffect::s_curEffect = this;

    if (CShaderEffect::s_enableShaders || !this->m_fixedFuncPassCount) {
        return;
    }

    for (uint32_t i = 0; i < this->m_fixedFuncPassCount; i++) {
        GxRsSet(static_cast<EGxRenderState>(GxRs_ColorOp0 + i), this->m_colorOps[i]);
        GxRsSet(static_cast<EGxRenderState>(GxRs_AlphaOp0 + i), this->m_alphaOps[i]);
    }
}

// OFFSET: 0x00433330/0x14003B5C0
void CShaderEffect::ResetShaders(uint32_t hullPermute, uint32_t domainPermute) {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    if (hullPermute != 0xFFFFFFFF) {
        GxRsSet(GxRs_HullShader, static_cast<CGxShader*>(NULL), 0);
    }

    if (domainPermute != 0xFFFFFFFF) {
        GxRsSet(GxRs_DomainShader, static_cast<CGxShader*>(NULL), 0);
    }
}

// OFFSET: 0x00433380/0x14003B620
void CShaderEffect::SetShaders(uint32_t vertexPermute, int32_t hullPermute, int32_t domainPermute, uint32_t pixelPermute) {
    int32_t useAlphaRef = 1;

    if (CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_VertexShader, CShaderEffect::s_curEffect->m_vertexShader, vertexPermute);
        GxRsSet(GxRs_PixelShader, CShaderEffect::s_curEffect->m_pixelShader, pixelPermute);

        if (hullPermute != -1) {
            GxRsSet(GxRs_HullShader, CShaderEffect::s_curEffect->m_hullShader, hullPermute);
        }

        if (domainPermute != -1) {
            GxRsSet(GxRs_DomainShader, CShaderEffect::s_curEffect->m_domainShader, domainPermute);
        }

        useAlphaRef = (pixelPermute & 8) == 0;
    }

    if (useAlphaRef == CShaderEffect::s_useAlphaRef) {
        return;
    }

    CShaderEffect::s_useAlphaRef = useAlphaRef;

    if (useAlphaRef) {
        GxRsSet(GxRs_AlphaRef, static_cast<int32_t>(static_cast<uint8_t>(static_cast<int32_t>(CShaderEffect::s_fogColorAlphaRef.w * 255.0f))));
    } else {
        GxShaderConstantsSet(GxSh_Pixel, 2, &CShaderEffect::s_fogColorAlphaRef.x, 1);
        GxRsSet(GxRs_AlphaRef, 0);
    }
}

// OFFSET: 0x004334F0/0x14003B7D0
void CShaderEffect::SetDefaultShaders(uint32_t shadow, uint32_t boneInfluences) {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    uint32_t localLights = 0;
    uint32_t shadowIndex = shadow > 1 ? 1 : shadow;
    uint32_t bones = boneInfluences > 2 ? 2 : boneInfluences;

    if (CShaderEffect::s_lightEnabled) {
        localLights = (CShaderEffect::s_localLightCount > 2 ? 2 : CShaderEffect::s_localLightCount) + 1;
    }

    uint32_t pixelPermute = CShaderEffect::SelectShadowShader(shadow);
    CShaderEffect::SetShaders(4 * (localLights + 4 * (shadowIndex + bones + 2 * shadowIndex)), -1, -1, pixelPermute);
}

// OFFSET: 0x00433570
void CShaderEffect::SetLightingEnabled(int32_t enabled) {
    CShaderEffect::s_lightEnabled = enabled;

    if (!CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_Lighting, enabled);
    }
}

// OFFSET: 0x004335C0/0x14003B8F0
void CShaderEffect::SetClipEnabled(int32_t enabled) {
    CShaderEffect::s_clipEnabled = enabled;

    if (CShaderEffect::s_enableShaders && g_theGxDevicePtr->m_caps.unk_02BC) {
        return;
    }

    GxRsSet(GxRs_ClipPlaneMask, enabled ? 1 : 0);
}

// OFFSET: 0x00433650/0x14003B9A0
void CShaderEffect::SetFogParams(float start, float end, float density, const CImVector& color, float extra) {
    if (CShaderEffect::s_enableShaders) {
        CShaderEffect::s_fogColorAlphaRef.x = color.r * 0.0039215689f;
        CShaderEffect::s_fogColorAlphaRef.y = color.g * 0.0039215689f;
        CShaderEffect::s_fogColorAlphaRef.z = color.b * 0.0039215689f;

        float scale = 1.0f / (end - start);
        CShaderEffect::s_fogParams.y = scale * end;
        CShaderEffect::s_fogParams.x = -(CShaderEffect::s_fogMul * scale);
        CShaderEffect::s_fogParams.w = extra;
        CShaderEffect::s_fogParams.z = density;

        if (!g_theGxDevicePtr->m_caps.unk_02F8) {
            GxShaderConstantsSet(GxSh_Pixel, 2, &CShaderEffect::s_fogColorAlphaRef.x, 1);
            return;
        }
    } else {
        GxRsSet(GxRs_FogStart, start);
        GxRsSet(GxRs_FogEnd, end);
    }

    GxRsSet(GxRs_FogColor, *reinterpret_cast<const int32_t*>(&color));
}

// OFFSET: 0x00433800/0x14003BB80
void CShaderEffect::SetFogEnabled(int32_t enabled) {
    if (enabled && g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
        if (CShaderEffect::s_enableShaders) {
            GxShaderConstantsSet(GxSh_Vertex, 30, &CShaderEffect::s_fogParams.x, 1);

            if (!g_theGxDevicePtr->m_caps.unk_02FC) {
                return;
            }
        }

        GxRsSet(GxRs_Fog, 1);
        return;
    }

    if (!CShaderEffect::s_enableShaders || g_theGxDevicePtr->m_caps.unk_02FC) {
        GxRsSet(GxRs_Fog, 0);
    } else {
        float fogParams[4] = { 0.0f, 1.0f, 1.0f, 0.0f };
        GxShaderConstantsSet(GxSh_Vertex, 30, fogParams, 1);
    }
}

// OFFSET: 0x004338E0/0x14003BCC0
void CShaderEffect::SetTexMtx_Identity(uint32_t stage) {
    if (CShaderEffect::s_enableShaders) {
        float matrix[8] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
        GxShaderConstantsSet(GxSh_Vertex, 2 * stage + 6, matrix, 2);
        return;
    }

    GxRsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + stage), 0);
    g_theGxDevicePtr->m_xforms[stage].Identity();
    GxRsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + stage), 0);
}

// OFFSET: 0x004339C0
void CShaderEffect::SetTexMtx_SphereMap(uint32_t stage) {
    if (CShaderEffect::s_enableShaders) {
        float matrix[8] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
        GxShaderConstantsSet(GxSh_Vertex, 2 * stage + 6, matrix, 2);
        return;
    }

    GxRsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + stage), 0);
    g_theGxDevicePtr->m_xforms[stage].Identity();
    GxRsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + stage), 6);
}

// OFFSET: 0x00433AA0
void CShaderEffect::SetTexMtx(const C2Vector& scale, const C2Vector& offset, uint32_t stage) {
    if (CShaderEffect::s_enableShaders) {
        float constants[8] = { scale.x, 0.0f, 0.0f, offset.x, 0.0f, scale.y, 0.0f, offset.y };
        GxShaderConstantsSet(GxSh_Vertex, 2 * stage + 6, constants, 2);
        return;
    }

    C44Matrix matrix;
    matrix.Scale(C3Vector(scale.x, scale.y, 1.0f));
    matrix.Translate(C3Vector(offset.x, offset.y, 0.0f));

    GxRsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + stage), 1);

    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[stage];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;

    GxRsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + stage), 0);
}

// OFFSET: 0x00433C70
void CShaderEffect::SetTexMtx(const C44Matrix& matrix, uint32_t stage) {
    if (CShaderEffect::s_enableShaders) {
        float constants[8] = { matrix.a0, matrix.b0, matrix.c0, matrix.d0, matrix.a1, matrix.b1, matrix.c1, matrix.d1 };
        GxShaderConstantsSet(GxSh_Vertex, 2 * stage + 6, constants, 2);
        return;
    }

    GxRsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + stage), 1);

    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[stage];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;

    GxRsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + stage), 0);
}

// OFFSET: 0x00433DA0
void CShaderEffect::SetTexMtx_TexGen(EGxTexGen texGen, const C44Matrix& matrix, uint32_t stage) {
    if (CShaderEffect::s_enableShaders) {
        C44Matrix view;
        GxXformView(view);
        view.AffineInvertInPlace();

        C44Matrix texMatrix = view * matrix;
        float constants[8] = { texMatrix.a0, texMatrix.b0, texMatrix.c0, texMatrix.d0, texMatrix.a1, texMatrix.b1, texMatrix.c1, texMatrix.d1 };
        GxShaderConstantsSet(GxSh_Vertex, 2 * stage + 6, constants, 2);
        return;
    }

    GxRsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + stage), 1);

    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[stage];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;

    GxRsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + stage), 2);
}

// OFFSET: 0x00432A20/0x14003C670
void CShaderEffect::UpdateWorldViewMatrix() {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    C44Matrix view;
    C44Matrix world;
    GxXformView(view);

    CGxMatrixStack& worldStack = g_theGxDevicePtr->m_xforms[GxXform_World];
    world = worldStack.m_mtx[worldStack.m_level];

    C44Matrix worldView = world * view;
    worldView = worldView.Transpose();

    GxShaderConstantsSet(GxSh_Vertex, 31, reinterpret_cast<const float*>(&worldView), 4);
}

// OFFSET: 0x00432C00/0x14003C960
void CShaderEffect::UpdateProjMatrix() {
    if (!CShaderEffect::s_enableShaders) {
        return;
    }

    C44Matrix projection;
    projection = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        projection.c0 = projection.c0 * -1.0f;
        projection.c1 = projection.c1 * -1.0f;
        projection.c2 = projection.c2 * -1.0f;
        projection.c3 = -1.0f * projection.c3;
    }

    projection = projection.Transpose();

    GxShaderConstantsSet(GxSh_Vertex, 2, reinterpret_cast<const float*>(&projection), 4);
}

// OFFSET: 0x00432D50/0x14003CAA0
void CShaderEffect::UpdateTesselation() {
    float params[4] = { CShaderEffect::s_tesselationFactor, CShaderEffect::s_tesselationDist, 0.0f, 0.0f };
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Hull, 0, params, 1);

    C44Matrix projection;
    projection = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        projection.c0 = projection.c0 * -1.0f;
        projection.c1 = projection.c1 * -1.0f;
        projection.c2 = projection.c2 * -1.0f;
        projection.c3 = -1.0f * projection.c3;
    }

    projection = projection.Transpose();

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Domain, 0, reinterpret_cast<const float*>(&projection), 4);
}

// OFFSET: 0x00432EE0
void CShaderEffect::UpdateDisplacement() {
    C44Matrix projection;
    projection = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        projection.c0 = projection.c0 * -1.0f;
        projection.c1 = projection.c1 * -1.0f;
        projection.c2 = projection.c2 * -1.0f;
        projection.c3 = -1.0f * projection.c3;
    }

    projection = projection.Transpose();

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Domain, 0, reinterpret_cast<const float*>(&projection), 4);

    float params[4] = { CShaderEffect::s_displFactor, CShaderEffect::s_tesselationDist, 0.0f, 0.0f };
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Domain, 4, params, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Hull, 0, params, 1);

    if (CShaderEffect::s_displTex) {
        g_theGxDevicePtr->RsSet(GxRs_Unk41, TextureGetGxTex(CShaderEffect::s_displTex));
    }
}

// OFFSET: 0x004330A0
void CShaderEffect::UpdateTerrainDispl() {
    C44Matrix projection;
    projection = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        projection.c0 = projection.c0 * -1.0f;
        projection.c1 = projection.c1 * -1.0f;
        projection.c2 = projection.c2 * -1.0f;
        projection.c3 = -1.0f * projection.c3;
    }

    projection = projection.Transpose();

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Domain, 0, reinterpret_cast<const float*>(&projection), 4);

    float params[4] = { CShaderEffect::s_terrainDisplFactor, CShaderEffect::s_tesselationDist, 0.0f, 0.0f };
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Domain, 4, params, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Hull, 0, params, 1);

    if (CShaderEffect::s_displTex) {
        g_theGxDevicePtr->RsSet(GxRs_Unk41, TextureGetGxTex(CShaderEffect::s_displTex));
    }
}

// OFFSET: 0x00433FB0
void CShaderEffect::SetDiffuse(const C4Vector& color) {
    if (CShaderEffect::s_enableShaders) {
        GxShaderConstantsSet(GxSh_Vertex, 28, &color.x, 1);
        return;
    }

    float r = color.x < 0.0f ? 0.0f : (color.x >= 1.0f ? 1.0f : color.x);
    float g = color.y < 0.0f ? 0.0f : (color.y >= 1.0f ? 1.0f : color.y);
    float b = color.z < 0.0f ? 0.0f : (color.z >= 1.0f ? 1.0f : color.z);
    float a = color.w < 0.0f ? 0.0f : (color.w >= 1.0f ? 1.0f : color.w);

    CImVector diffuse;
    diffuse.r = static_cast<uint8_t>(r * 255.0f);
    diffuse.g = static_cast<uint8_t>(g * 255.0f);
    diffuse.b = static_cast<uint8_t>(b * 255.0f);
    diffuse.a = static_cast<uint8_t>(a * 255.0f);

    GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<int32_t*>(&diffuse));
}

// OFFSET: 0x00434100
void CShaderEffect::SetEmissive(const C4Vector& color) {
    if (CShaderEffect::s_enableShaders) {
        GxShaderConstantsSet(GxSh_Vertex, 29, &color.x, 1);
        return;
    }

    float r = color.x < 0.0f ? 0.0f : (color.x >= 1.0f ? 1.0f : color.x);
    float g = color.y < 0.0f ? 0.0f : (color.y >= 1.0f ? 1.0f : color.y);
    float b = color.z < 0.0f ? 0.0f : (color.z >= 1.0f ? 1.0f : color.z);
    float a = color.w < 0.0f ? 0.0f : (color.w >= 1.0f ? 1.0f : color.w);

    CImVector emissive;
    emissive.r = static_cast<uint8_t>(r * 255.0f);
    emissive.g = static_cast<uint8_t>(g * 255.0f);
    emissive.b = static_cast<uint8_t>(b * 255.0f);
    emissive.a = static_cast<uint8_t>(a * 255.0f);

    GxRsSet(GxRs_MatEmissive, *reinterpret_cast<int32_t*>(&emissive));
}

// OFFSET: 0x00434250/0x14003D2A0
void CShaderEffect::SetAlphaRef(float alphaRef) {
    CShaderEffect::s_fogColorAlphaRef.w = alphaRef;

    if (!CShaderEffect::s_useAlphaRef) {
        GxShaderConstantsSet(GxSh_Pixel, 2, &CShaderEffect::s_fogColorAlphaRef.x, 1);
        return;
    }

    GxRsSet(GxRs_AlphaRef, static_cast<int32_t>(static_cast<uint8_t>(static_cast<int32_t>(alphaRef * 255.0f))));
}

// OFFSET: 0x00434370
void CShaderEffect::SetLocalLighting(CM2Lighting* lighting, int32_t enabled, const C3Vector* origin) {
    CShaderEffect::s_lightEnabled = enabled;

    if (!CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_Lighting, enabled);
    }

    CShaderEffect::s_localLightCount = lighting ? lighting->m_lightCount : 0;

    if (!CShaderEffect::s_enableShaders) {
        if (enabled) {
            lighting->SetupGxLights(origin);
        }

        return;
    }

    if (!enabled) {
        return;
    }

    CShaderEffect::s_sunDir = lighting->m_sunDir;

    if (CShaderEffect::s_sunDir.x != 0.0f || CShaderEffect::s_sunDir.y != 0.0f || CShaderEffect::s_sunDir.z != 0.0f) {
        float scale = 1.0 / sqrt(CShaderEffect::s_sunDir.z * CShaderEffect::s_sunDir.z + CShaderEffect::s_sunDir.y * CShaderEffect::s_sunDir.y + CShaderEffect::s_sunDir.x * CShaderEffect::s_sunDir.x);
        CShaderEffect::s_sunDir.x = CShaderEffect::s_sunDir.x * scale;
        CShaderEffect::s_sunDir.y = CShaderEffect::s_sunDir.y * scale;
        CShaderEffect::s_sunDir.z = CShaderEffect::s_sunDir.z * scale;
    }

    CShaderEffect::s_sunAmbient = lighting->m_sunAmbient;

    float r = lighting->m_sunDiffuse.x;
    float g = lighting->m_sunDiffuse.y;
    float b = lighting->m_sunDiffuse.z;
    CShaderEffect::s_sunDiffuse.x = r >= 1.0f ? 1.0f : r;
    CShaderEffect::s_sunDiffuse.y = g >= 1.0f ? 1.0f : g;
    CShaderEffect::s_sunDiffuse.z = b >= 1.0f ? 1.0f : b;

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, &CShaderEffect::s_sunAmbient.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 10, &CShaderEffect::s_sunDiffuse.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 12, &CShaderEffect::s_sunDir.x, 1);

    if (CShaderEffect::s_localLightCount) {
        CShaderEffect::ComputeLocalLights(&CShaderEffect::s_localLights, CShaderEffect::s_localLightCount, lighting->m_lights, origin);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 17, reinterpret_cast<const float*>(&CShaderEffect::s_localLights), sizeof(LocalLights) / sizeof(C4Vector));
    }

    CShadowCache::SetShadowMapGenericInterior(lighting->m_flags & 8);
}

// OFFSET: 0x004345C0/0x14003D6B0
void CShaderEffect::SetAlphaRefDefault() {
    float alphaRef = CGxDevice::s_alphaRef[g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]] * 0.0039215689f;
    CShaderEffect::SetAlphaRef(alphaRef);
}

// OFFSET: 0x004342F0
CShaderEffect::~CShaderEffect() {
    if (CShaderEffect::s_displTex) {
        TextureRelease(CShaderEffect::s_displTex);
    }

    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_vertexShader);
        this->m_vertexShader = NULL;
    }

    if (this->m_hullShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_hullShader);
        this->m_hullShader = NULL;
    }

    if (this->m_domainShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_domainShader);
        this->m_domainShader = NULL;
    }

    if (this->m_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_pixelShader);
        this->m_pixelShader = NULL;
    }
}

// PROVEN: 52e2fe92 IDENTICAL/IDENTICAL
// OFFSET: 0x00434610/0x14003D6F0
CShaderEffect::CShaderEffect() {
    this->m_refCount = 1;
}

// PROVEN: 52c8bafa IDENTICAL/NONE
// OFFSET: 0x0077A4A0
CGxShaderPermute* CShaderEffect::GetVertexPermute(uint32_t permute) {
    return this->m_vertexShader ? &this->m_vertexShader->m_permutes[permute] : NULL;
}

// PROVEN: 89ea3ef0 IDENTICAL/NONE
// OFFSET: 0x0077A4C0
CGxShaderPermute* CShaderEffect::GetPixelPermute(uint32_t permute) {
    return this->m_pixelShader ? &this->m_pixelShader->m_permutes[permute] : NULL;
}
