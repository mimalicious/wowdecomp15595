#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00D38338
CGxShader* CShadowCache::s_shadowMapPixelShader;

// STATIC: 0x00D3833C
CGxShader* CShadowCache::s_shadowMapVertexShader;

// STATIC: %0x00BACE10
float CShadowCache::s_debugAspect = 1.0f;

// STATIC: %0x00BACE14
int32_t CShadowCache::s_recreateTextures = 0;

// STATIC: %0x00BACE18
const float CShadowCache::s_bandRadius[3] = { 40.0f, 160.0f, 640.0f };

// STATIC: %0x00BACE24
const float CShadowCache::s_bandUpdateDistance[3] = { 4.0f, 16.0f, 1024.0f };

// STATIC: %0x00BACE30
const char* CShadowCache::s_shadowModeDescs[NUM_SHADOW_MODES] = {
    "0 - Precomputed terrain shadows, blob shadows.",
    "1 - Precomputed terrain shadows, dynamic shadows near player.",
    "2 - Static environment shadows, dynamic shadows near player.",
    "3 - Full dynamic shadows.",
};

// STATIC: %0x00C3CD88
uint32_t CShadowCache::s_shadowSize;

// STATIC: %0x00C3CD8C
EShadowMode CShadowCache::s_shadowMode;

// STATIC: 0x00D38360
CTexture* CShadowCache::s_intDynShadow;

// STATIC: %0x00C3CD94
int32_t CShadowCache::s_fullClear;

// STATIC: %0x00C3CD98
int32_t CShadowCache::s_fullUpdate;

// STATIC: %0x00C3CD9C
ShadowMatrixCallback CShadowCache::s_matrixCallback;

// STATIC: %0x00C3CDA0
ShadowFrustumCallback CShadowCache::s_frustumCallback;

// STATIC: %0x00C3CDA4
ShadowQueryCallback CShadowCache::s_queryCallback;

// STATIC: %0x00C3CDA8
ShadowRenderCallback CShadowCache::s_renderCallback;

// STATIC: %0x00C3CDAC
int32_t CShadowCache::s_debugLevel;

// STATIC: %0x00C3CDB0
uint8_t CShadowCache::s_frameCounter;

// STATIC: 0x00D38390
CShadowBand CShadowCache::s_extDynShadow;

// STATIC: %0x00C3CDF8
float CShadowCache::s_extDynShadowCullRect[4];

// STATIC: %0x00C3CE08
C3Vector CShadowCache::s_sunDir;

// STATIC: 0x00D383EC
C4Vector CShadowCache::s_sunDirViewSpace;

// STATIC: 0x00D383FC
C4Vector CShadowCache::s_cutoffPlane;

// STATIC: 0x00D3840C
C4Vector CShadowCache::s_cutoffPlaneDefault;

// STATIC: %0x00C3CE44
float CShadowCache::s_textureSize[2];

// STATIC: %0x00C3CE50
float CShadowCache::s_shadowScale;

// STATIC: %0x00C3CE58
C4Vector CShadowCache::s_shadowMatrices[12];

// STATIC: %0x00C3CF18
C4Vector CShadowCache::s_extShadowMatrix[3];

// STATIC: %0x00C3CF48
C4Vector CShadowCache::s_bandParams[6];

// STATIC: %0x00C3CFA8
C4Vector CShadowCache::s_pcfOffsets[8];

// STATIC: 0x00D38600
CShadowBand CShadowCache::s_extShadowBands[3];

// OFFSET: %0x0043F770
const char* CShadowCache::GetShadowModeDesc(uint32_t mode) {
    if (mode >= NUM_SHADOW_MODES) {
        return "Unknown shadow mode";
    }

    return CShadowCache::s_shadowModeDescs[mode];
}

// PROVEN: 1f17a06c IDENTICAL/IDENTICAL
// OFFSET: 0x004352C0/0x14003EA00
uint32_t CShadowCache::ValidateTextureSize(uint32_t size) {
    if (size == 1024 || size == 2048) {
        return 0;
    }

    return 2;
}

// PROVEN: 534e6e2d IDENTICAL/IDENTICAL
// OFFSET: 0x004352E0/0x14003EA20
EShadowMode CShadowCache::GetShadowMode() {
    return CShadowCache::s_shadowMode;
}

// PROVEN: 4a7ae856 DIFFERENT/IDENTICAL
// OFFSET: 0x004352F0/0x14003EA30
int32_t CShadowCache::SetTextureSize(uint32_t size) {
    if (CShadowCache::ValidateTextureSize(size)) {
        return 0;
    }

    CShadowCache::s_shadowSize = size;
    CShadowCache::s_recreateTextures = 1;

    return 1;
}

// PROVEN: ddab60bb IDENTICAL/IDENTICAL
// OFFSET: 0x00435320/0x14003EA60
void CShadowCache::SetMatrixCallback(ShadowMatrixCallback callback) {
    CShadowCache::s_matrixCallback = callback;
}

// PROVEN: e756de25 IDENTICAL/IDENTICAL
// OFFSET: 0x00435330/0x14003EA70
void CShadowCache::SetFrustumCallback(ShadowFrustumCallback callback) {
    CShadowCache::s_frustumCallback = callback;
}

// PROVEN: 8e88f2c1 IDENTICAL/IDENTICAL
// OFFSET: 0x00435340/0x14003EA80
void CShadowCache::SetQueryCallback(ShadowQueryCallback callback) {
    CShadowCache::s_queryCallback = callback;
}

// PROVEN: f1b0ee5f IDENTICAL/IDENTICAL
// OFFSET: 0x00435350/0x14003EA90
void CShadowCache::SetRenderCallback(ShadowRenderCallback callback) {
    CShadowCache::s_renderCallback = callback;
}

// PROVEN: 4ad01d7c IDENTICAL/IDENTICAL
// OFFSET: 0x00435360/0x14003EAA0
void __fastcall CShadowCache::NotifyTexturesRecreate() {
    CShadowCache::s_recreateTextures = 1;
}

// OFFSET: #0x00873F50
uint32_t CShadowCache::GetMaxShadowMode() {
    return NUM_SHADOW_MODES - 1;
}

// OFFSET: #0x00873FF0
uint32_t CShadowCache::GetShadowValue() {
    return CShadowCache::GetShadowMode();
}

// PROVEN: 1aebd967 IDENTICAL/NONE
// OFFSET: 0x004353E0
void CShadowCache::SetDebugParams(float aspect, int32_t level) {
    CShadowCache::s_debugAspect = aspect;
    CShadowCache::s_debugLevel = level;
}

// PROVEN: cf2b43f4 IDENTICAL/IDENTICAL
// OFFSET: 0x00435400/0x14003EB80
void CShadowCache::SetShadowScale(float scale) {
    CShadowCache::s_shadowScale = scale;
}

// INLINED
CShadowBand::CShadowBand() {
    this->m_lastPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_pos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_dir = C3Vector(1.0f, 0.0f, 0.0f);
}

// OFFSET: %0x0043FAD0
uint32_t CShadowCache::ValidateShadowMode(EShadowMode mode) {
    if (!mode) {
        return 0;
    }

    if (mode >= NUM_SHADOW_MODES) {
        return 2;
    }

    CGxCaps& caps = g_theGxDevicePtr->m_caps;

    if (mode == SHADOW_LOW) {
        if (caps.m_shaderModel < 4) {
            return 64;
        }
    } else if (caps.m_shaderModel < 5) {
        return 128;
    }

    if ((caps.m_pcfFormat[GxTex_D24X8] && caps.m_rttFormat[GxTex_D24X8] && caps.m_texFmt[GxTex_D24X8]) || (caps.m_rttFormat[GxTex_R32F] && caps.m_texFmt[GxTex_R32F])) {
        return g_theGxDevicePtr->DeviceApi() != GxApi_OpenGl ? 0 : 4;
    }

    return 8;
}

// OFFSET: %0x0043FB60
int32_t CShadowCache::SetShadowMode(EShadowMode mode) {
    if (CShadowCache::ValidateShadowMode(mode)) {
        return 0;
    }

    CShadowCache::s_shadowMode = mode;
    CShadowCache::s_recreateTextures = 1;

    return 1;
}

// OFFSET: %0x0043FB90
void CShadowCache::Destroy() {
    if (CShadowCache::s_extDynShadow.m_textures[0]) {
        TextureRelease(CShadowCache::s_extDynShadow.m_textures[0]);
        CShadowCache::s_extDynShadow.m_textures[0] = NULL;
    }

    if (CShadowCache::s_extDynShadow.m_textures[1]) {
        TextureRelease(CShadowCache::s_extDynShadow.m_textures[1]);
        CShadowCache::s_extDynShadow.m_textures[1] = NULL;
    }

    if (CShadowCache::s_intDynShadow) {
        TextureRelease(CShadowCache::s_intDynShadow);
        CShadowCache::s_intDynShadow = NULL;
    }

    for (uint32_t i = 0; i < 3; i++) {
        CShadowBand& band = CShadowCache::s_extShadowBands[i];

        if (band.m_textures[0]) {
            TextureRelease(band.m_textures[0]);
            band.m_textures[0] = NULL;
        }

        if (band.m_textures[1]) {
            TextureRelease(band.m_textures[1]);
            band.m_textures[1] = NULL;
        }
    }

    g_theGxDevicePtr->RemoveTextureRecreationCallback(CShadowCache::NotifyTexturesRecreate);
}

// OFFSET: %0x0043FC50
void CShadowCache::InitPcfOffsets() {
    float size = CShadowCache::s_shadowSize;
    CShadowCache::s_textureSize[0] = size;
    CShadowCache::s_textureSize[1] = size;

    CShadowCache::s_pcfOffsets[0].x = 1.0f / size * 0.8f;
    CShadowCache::s_pcfOffsets[0].y = 1.0f / size * -1.0f;
    CShadowCache::s_pcfOffsets[0].z = 0.0f;
    CShadowCache::s_pcfOffsets[0].w = 0.0f;
    CShadowCache::s_pcfOffsets[1].x = 1.0f / size * -0.2f;
    CShadowCache::s_pcfOffsets[1].y = 1.0f / size * -0.8f;
    CShadowCache::s_pcfOffsets[1].z = 0.0f;
    CShadowCache::s_pcfOffsets[1].w = 0.0f;
    CShadowCache::s_pcfOffsets[2].x = 1.0f / size * 0.2f;
    CShadowCache::s_pcfOffsets[2].y = 1.0f / size * -0.6f;
    CShadowCache::s_pcfOffsets[2].z = 0.0f;
    CShadowCache::s_pcfOffsets[2].w = 0.0f;
    CShadowCache::s_pcfOffsets[3].x = 1.0f / size;
    CShadowCache::s_pcfOffsets[3].y = 1.0f / size * -0.4f;
    CShadowCache::s_pcfOffsets[3].z = 0.0f;
    CShadowCache::s_pcfOffsets[3].w = 0.0f;
    CShadowCache::s_pcfOffsets[4].x = 1.0f / size * -0.6f;
    CShadowCache::s_pcfOffsets[4].y = 1.0f / size * -0.2f;
    CShadowCache::s_pcfOffsets[4].z = 0.0f;
    CShadowCache::s_pcfOffsets[4].w = 0.0f;
    CShadowCache::s_pcfOffsets[5].x = 1.0f / size * 0.6f;
    CShadowCache::s_pcfOffsets[5].y = 1.0f / size * 0.2f;
    CShadowCache::s_pcfOffsets[5].z = 0.0f;
    CShadowCache::s_pcfOffsets[5].w = 0.0f;
    CShadowCache::s_pcfOffsets[6].x = 1.0f / size * -1.0f;
    CShadowCache::s_pcfOffsets[6].y = 1.0f / size * -0.4f;
    CShadowCache::s_pcfOffsets[6].z = 0.0f;
    CShadowCache::s_pcfOffsets[6].w = 0.0f;
    CShadowCache::s_pcfOffsets[7].x = 1.0f / size * -0.4f;
    CShadowCache::s_pcfOffsets[7].y = 1.0f / size * -0.6f;
    CShadowCache::s_pcfOffsets[7].z = 0.0f;
    CShadowCache::s_pcfOffsets[7].w = 0.0f;
}

// PROVEN: 29f5640c IDENTICAL/NONE
// OFFSET: 0x00435B60
void CShadowCache::SetShadowMapGenericGlobal() {
    if (CShadowCache::s_recreateTextures || CShadowCache::s_shadowMode <= SHADOW_LOWEST) {
        return;
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 224, &CShadowCache::s_extShadowMatrix[0].x, 3);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 6, &CShadowCache::s_sunDirViewSpace.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 7, &CShadowCache::s_bandParams[0].x, 6);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 13, &CShadowCache::s_pcfOffsets[0].x, 8);

    if (CShadowCache::s_shadowMode > SHADOW_LOW) {
        CShadowBand* bands = CShadowCache::s_extShadowBands;
        g_theGxDevicePtr->RsSet(GxRs_Texture5, TextureGetGxTex(bands[0].m_textures[bands[0].m_current]));
        g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(bands[1].m_textures[bands[1].m_current]));
        g_theGxDevicePtr->RsSet(GxRs_Texture7, TextureGetGxTex(bands[2].m_textures[bands[2].m_current]));
    }
}

// PROVEN: 3d779f43 IDENTICAL/NONE
// OFFSET: 0x00435C60
void CShadowCache::SetShadowMapGenericInterior(int32_t interior) {
    if (CShadowCache::s_recreateTextures || CShadowCache::s_shadowMode <= SHADOW_LOWEST) {
        return;
    }

    const C4Vector* cutoffPlane = &CShadowCache::s_cutoffPlane;
    CGxTex* texture = TextureGetGxTex(CShadowCache::s_intDynShadow);

    if (!interior) {
        texture = TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[CShaderEffect::s_usePcfFiltering]);

        if (CShadowCache::s_shadowMode > SHADOW_LOW) {
            cutoffPlane = &CShadowCache::s_cutoffPlaneDefault;
        }
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 5, &cutoffPlane->x, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture4, texture);
}

// PROVEN: 463fda19 IDENTICAL/NONE
// OFFSET: 0x00435CF0
void CShadowCache::SetShadowMapTerrain() {
    if (CShadowCache::s_recreateTextures || CShadowCache::s_shadowMode <= SHADOW_LOWEST) {
        return;
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 38, &CShadowCache::s_extShadowMatrix[0].x, 3);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 7, &CShadowCache::s_bandParams[0].x, 6);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 13, &CShadowCache::s_pcfOffsets[0].x, 8);
    g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[CShaderEffect::s_usePcfFiltering]));

    if (CShadowCache::s_shadowMode > SHADOW_LOW) {
        CShadowBand* bands = CShadowCache::s_extShadowBands;
        g_theGxDevicePtr->RsSet(GxRs_Texture7, TextureGetGxTex(bands[0].m_textures[bands[0].m_current]));
        g_theGxDevicePtr->RsSet(GxRs_Texture8, TextureGetGxTex(bands[1].m_textures[bands[1].m_current]));
        g_theGxDevicePtr->RsSet(GxRs_Texture9, TextureGetGxTex(bands[2].m_textures[bands[2].m_current]));
    }
}

// PROVEN: 5bcef6c4 IDENTICAL/NONE
// OFFSET: 0x00435E00
void CShadowCache::SetShadowMapDetailDoodad() {
    if (CShadowCache::s_recreateTextures || CShadowCache::s_shadowMode <= SHADOW_LOWEST) {
        return;
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 32, &CShadowCache::s_extShadowMatrix[0].x, 3);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 5, &CShadowCache::s_cutoffPlaneDefault.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 6, &CShadowCache::s_sunDirViewSpace.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 7, &CShadowCache::s_bandParams[0].x, 6);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 13, &CShadowCache::s_pcfOffsets[0].x, 8);
    g_theGxDevicePtr->RsSet(GxRs_Texture4, TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[CShaderEffect::s_usePcfFiltering]));

    if (CShadowCache::s_shadowMode > SHADOW_LOW) {
        CShadowBand* bands = CShadowCache::s_extShadowBands;
        g_theGxDevicePtr->RsSet(GxRs_Texture5, TextureGetGxTex(bands[0].m_textures[bands[0].m_current]));
        g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(bands[1].m_textures[bands[1].m_current]));
        g_theGxDevicePtr->RsSet(GxRs_Texture7, TextureGetGxTex(bands[2].m_textures[bands[2].m_current]));
    }
}

// OFFSET: %0x00440280
void CShadowCache::GatherBands(CShadowCacheQueryData& data, const C3Vector& pos, CFrustum& frustum, int32_t useCullFrustum) {
    uint32_t frame = 0;

    if (CShadowCache::s_shadowMode < SHADOW_HIGH) {
        C44Matrix identity;

        for (uint32_t i = 0; i < 3; i++) {
            CShadowBand& band = CShadowCache::s_extShadowBands[i];

            data.m_textureIndex[i] = band.m_current == 0;
            data.m_active[i] = 0;

            if (!band.m_updateCount) {
                float dx = band.m_lastPos.x - pos.x;
                float dy = band.m_lastPos.y - pos.y;
                float dz = band.m_lastPos.z - pos.z;

                if (dz * dz + dy * dy + dx * dx <= band.m_updateDistance) {
                    continue;
                }
            }

            data.m_queryFlags[i] = 8;
            data.m_radius[i] = band.m_radius;

            if (!band.m_updateCount) {
                band.m_pos = pos;
            }

            data.m_pos[i] = band.m_pos;

            float step;
            uint32_t row;

            if (i >= 2) {
                step = data.m_radius[i] * 0.4f;
                data.m_bounds[i][0] = (band.m_updateCount % 5) * step - data.m_radius[i];
                data.m_bounds[i][1] = data.m_bounds[i][0] + step;
                row = band.m_updateCount / 5;
            } else {
                step = data.m_radius[i] * 0.6666667f;
                data.m_bounds[i][0] = (band.m_updateCount % 3) * step - data.m_radius[i];
                data.m_bounds[i][1] = data.m_bounds[i][0] + step;
                row = band.m_updateCount / 3;
            }

            data.m_bounds[i][2] = step * static_cast<int32_t>(row) - data.m_radius[i];
            data.m_bounds[i][3] = data.m_bounds[i][2] + step;

            data.m_bounds[i][1] = data.m_radius[i] * 2.0f * (ceil(CShadowCache::s_shadowSize / (data.m_radius[i] + data.m_radius[i]) * data.m_bounds[i][1]) / CShadowCache::s_shadowSize);
            data.m_bounds[i][3] = ceil(CShadowCache::s_shadowSize / (2.0f * data.m_radius[i]) * data.m_bounds[i][3]) / CShadowCache::s_shadowSize * (data.m_radius[i] + data.m_radius[i]);

            float scale = 0.5f / data.m_radius[i];
            data.m_texBounds[i][0] = data.m_bounds[i][0] * scale + 0.5f;
            data.m_texBounds[i][1] = data.m_bounds[i][1] * scale + 0.5f;
            data.m_texBounds[i][3] = 0.5f - data.m_bounds[i][2] * scale;
            data.m_texBounds[i][2] = 0.5f - data.m_bounds[i][3] * scale;

            CShadowCache::s_frustumCallback(data.m_frustums[i], data.m_matrices[i], band.m_dir, data, i);
            data.m_frustums[i].Extents(data.m_extents[i]);

            if (i && data.m_active[i - 1]) {
                data.m_cullFrustums[i] = frustum;
                data.m_useCullFrustum[i] = 1;
            } else {
                data.m_useCullFrustum[i] = 0;
            }

            float bounds[4];
            bounds[0] = data.m_bounds[i][0];
            bounds[1] = data.m_bounds[i][1];
            bounds[2] = data.m_bounds[i][2];
            bounds[3] = data.m_bounds[i][3];

            data.m_bounds[i][0] = -data.m_radius[i];
            data.m_bounds[i][1] = data.m_radius[i];
            data.m_bounds[i][2] = -data.m_radius[i];
            data.m_bounds[i][3] = data.m_radius[i];

            CShadowCache::s_frustumCallback(frustum, identity, band.m_dir, data, i);
            frustum.InvertPlanes();

            data.m_bounds[i][0] = bounds[0];
            data.m_bounds[i][1] = bounds[1];
            data.m_bounds[i][2] = bounds[2];
            data.m_bounds[i][3] = bounds[3];

            band.m_updateCount++;

            if ((i < 2 && band.m_updateCount >= 9) || (i == 2 && band.m_updateCount >= 25)) {
                band.m_lastPos = band.m_pos;
                band.m_updateCount = 0;
                band.m_current = band.m_current == 0;
            }

            data.m_active[i] = 1;
            data.m_lastBand = i;
        }
    } else {
        frame = CShadowCache::s_frameCounter;

        for (uint32_t i = 0; i < 3; i++) {
            CShadowBand& band = CShadowCache::s_extShadowBands[i];

            data.m_textureIndex[i] = 0;
            data.m_active[i] = 0;
            data.m_queryFlags[i] = 30;
            data.m_radius[i] = band.m_radius;
            band.m_pos = pos;

            float cell;

            if (i == 0) {
                cell = 2.0f;
            } else if (i == 1) {
                cell = 4.0f;
            } else {
                cell = 16.0f;
            }

            float invCell = 1.0f / cell;
            band.m_pos.x = floor(invCell * band.m_pos.x) * cell;
            band.m_pos.y = floor(band.m_pos.y * invCell) * cell;
            band.m_pos.z = floor(band.m_pos.z * invCell) * cell;

            data.m_pos[i] = band.m_pos;

            data.m_bounds[i][0] = -data.m_radius[i];
            data.m_bounds[i][1] = data.m_radius[i];
            data.m_bounds[i][2] = -data.m_radius[i];
            data.m_bounds[i][3] = data.m_radius[i];

            data.m_texBounds[i][0] = 0.0f;
            data.m_texBounds[i][1] = 1.0f;
            data.m_texBounds[i][2] = 0.0f;
            data.m_texBounds[i][3] = 1.0f;

            CShadowCache::s_frustumCallback(data.m_frustums[i], data.m_matrices[i], band.m_dir, data, i);
            data.m_frustums[i].Extents(data.m_extents[i]);

            data.m_useCullFrustum[i] = useCullFrustum;

            if (useCullFrustum) {
                data.m_cullFrustums[i] = frustum;
            }

            frustum = data.m_frustums[i];
            frustum.InvertPlanes();

            band.m_lastPos = band.m_pos;
            data.m_active[i] = 1;
            data.m_lastBand = i;
            useCullFrustum = 1;
        }
    }

    CShadowCache::s_queryCallback(data, frame);
}

// OFFSET: %0x00440A70
void CShadowCache::RenderBands(CShadowCacheQueryData& data) {
    for (int32_t i = 0; i <= data.m_lastBand; i++) {
        if (!data.m_active[i]) {
            continue;
        }

        CShadowBand& band = CShadowCache::s_extShadowBands[i];
        CGxTex* colorTarget = NULL;
        CGxTex* target;

        if (CShaderEffect::s_usePcfFiltering) {
            if (CShadowCache::s_shadowMode < SHADOW_HIGH) {
                colorTarget = TextureGetGxTex(band.m_textures[data.m_textureIndex[i]]);
            } else {
                colorTarget = TextureGetGxTex(band.m_textures[0]);
            }

            target = TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[0]);
        } else if (CShadowCache::s_shadowMode >= SHADOW_HIGH) {
            target = TextureGetGxTex(band.m_textures[0]);
        } else {
            target = TextureGetGxTex(band.m_textures[data.m_textureIndex[i]]);
        }

        CShadowCache::s_renderCallback(data, i, target, colorTarget, band.m_dir);
    }
}

// OFFSET: %0x00440B50
void CShadowCache::PostUpdate() {
    if (!CShadowCache::s_matrixCallback || !CShadowCache::s_frustumCallback || !CShadowCache::s_queryCallback || !CShadowCache::s_renderCallback) {
        return;
    }

    C44Matrix scale;
    C44Matrix translate;

    scale.Scale(C3Vector(1.0f, -1.0f, CShadowCache::s_shadowScale));

    C3Vector offset;
    offset.x = 0.5f / CShadowCache::s_textureSize[0];
    offset.y = 0.5f / CShadowCache::s_textureSize[0];
    offset.z = 0.0f;
    translate.Translate(offset);

    C44Matrix matrix;
    CShadowCache::s_matrixCallback(CShadowCache::s_extDynShadow.m_lastPos, CShadowCache::s_extDynShadow.m_radius, matrix, CShadowCache::s_extDynShadow.m_dir, -1);
    matrix = matrix * scale * translate;

    CShadowCache::s_shadowMatrices[0].x = matrix.a0;
    CShadowCache::s_shadowMatrices[0].y = matrix.b0;
    CShadowCache::s_shadowMatrices[0].z = matrix.c0;
    CShadowCache::s_shadowMatrices[0].w = matrix.d0;
    CShadowCache::s_shadowMatrices[1].x = matrix.a1;
    CShadowCache::s_shadowMatrices[1].y = matrix.b1;
    CShadowCache::s_shadowMatrices[1].z = matrix.c1;
    CShadowCache::s_shadowMatrices[1].w = matrix.d1;
    CShadowCache::s_shadowMatrices[2].x = matrix.a2;
    CShadowCache::s_shadowMatrices[2].y = matrix.b2;
    CShadowCache::s_shadowMatrices[2].z = matrix.c2;
    CShadowCache::s_shadowMatrices[2].w = matrix.d2;

    for (int32_t i = 0; i < 3; i++) {
        CShadowBand& band = CShadowCache::s_extShadowBands[i];

        CShadowCache::s_matrixCallback(band.m_lastPos, band.m_radius, matrix, band.m_dir, i);
        matrix = matrix * scale * translate;

        CShadowCache::s_shadowMatrices[3 + 3 * i].x = matrix.a0;
        CShadowCache::s_shadowMatrices[3 + 3 * i].y = matrix.b0;
        CShadowCache::s_shadowMatrices[3 + 3 * i].z = matrix.c0;
        CShadowCache::s_shadowMatrices[3 + 3 * i].w = matrix.d0;
        CShadowCache::s_shadowMatrices[4 + 3 * i].x = matrix.a1;
        CShadowCache::s_shadowMatrices[4 + 3 * i].y = matrix.b1;
        CShadowCache::s_shadowMatrices[4 + 3 * i].z = matrix.c1;
        CShadowCache::s_shadowMatrices[4 + 3 * i].w = matrix.d1;
        CShadowCache::s_shadowMatrices[5 + 3 * i].x = matrix.a2;
        CShadowCache::s_shadowMatrices[5 + 3 * i].y = matrix.b2;
        CShadowCache::s_shadowMatrices[5 + 3 * i].z = matrix.c2;
        CShadowCache::s_shadowMatrices[5 + 3 * i].w = matrix.d2;
    }

    CShadowCache::s_extShadowMatrix[0] = CShadowCache::s_shadowMatrices[0];
    CShadowCache::s_extShadowMatrix[1] = CShadowCache::s_shadowMatrices[1];
    CShadowCache::s_extShadowMatrix[2] = CShadowCache::s_shadowMatrices[2];

    const C4Vector* ext = CShadowCache::s_shadowMatrices;

    for (int32_t i = 0; i < 3; i++) {
        const C4Vector* band = &CShadowCache::s_shadowMatrices[3 + 3 * i];

        float scaleX = ext[0].x == 0.0f ? band[0].y / ext[0].y : band[0].x / ext[0].x;
        float scaleY = ext[1].y == 0.0f ? band[1].x / ext[1].x : band[1].y / ext[1].y;

        CShadowCache::s_bandParams[2 * i].x = scaleX;
        CShadowCache::s_bandParams[2 * i].y = scaleY;
        CShadowCache::s_bandParams[2 * i].z = 1.0f;
        CShadowCache::s_bandParams[2 * i].w = 0.0f;
        CShadowCache::s_bandParams[2 * i + 1].x = band[0].w - scaleX * ext[0].w;
        CShadowCache::s_bandParams[2 * i + 1].y = band[1].w - ext[1].w * scaleY;
        CShadowCache::s_bandParams[2 * i + 1].z = band[2].w - ext[2].w;
        CShadowCache::s_bandParams[2 * i + 1].w = 0.0f;
    }
}

// OFFSET: %0x004414E0
CShadowCacheQueryData::CShadowCacheQueryData() {
    for (uint32_t i = 0; i < 3; i++) {
        this->m_extents[i].b = C3Vector(0.0f, 0.0f, 0.0f);
        this->m_extents[i].t = C3Vector(0.0f, 0.0f, 0.0f);
    }

    for (uint32_t i = 0; i < 3; i++) {
        this->m_cullRects[i][0] = 0.0f;
        this->m_cullRects[i][1] = 0.0f;
        this->m_cullRects[i][2] = 0.0f;
        this->m_cullRects[i][3] = 0.0f;
    }

    for (uint32_t i = 0; i < 3; i++) {
        this->m_pos[i] = C3Vector(0.0f, 0.0f, 0.0f);
    }

    for (uint32_t i = 0; i < 3; i++) {
        this->m_queryFlags[i] = 0;
        this->m_active[i] = 0;
        this->m_useCullFrustum[i] = 0;
        this->m_textureIndex[i] = 0;
        this->m_radius[i] = 0.0f;
        this->m_bounds[i][0] = 0.0f;
        this->m_bounds[i][1] = 0.0f;
        this->m_bounds[i][2] = 0.0f;
        this->m_bounds[i][3] = 0.0f;
        this->m_texBounds[i][0] = 0.0f;
        this->m_texBounds[i][1] = 0.0f;
        this->m_texBounds[i][2] = 0.0f;
        this->m_texBounds[i][3] = 0.0f;
    }

    this->m_useFrustum = 0;
    this->m_lastBand = -1;
}

// OFFSET: %0x00441850
void CShadowCache::ClearBandTextures() {
    if (CShadowCache::s_shadowMode <= SHADOW_LOW) {
        CShadowCache::s_fullUpdate = 0;
        CShadowCache::s_fullClear = 0;
        return;
    }

    if (!CShadowCache::s_fullClear) {
        CShadowCache::s_extShadowBands[0].m_updateCount = 1;
        CShadowCache::s_extShadowBands[1].m_updateCount = 1;
        CShadowCache::s_extShadowBands[2].m_updateCount = 1;
        CShadowCache::s_fullUpdate = 0;
        CShadowCache::s_fullClear = 0;
        return;
    }

    CGxTex* colorTarget = NULL;
    CGxTex* depthTarget = NULL;
    g_theGxDevicePtr->DeviceGetRenderTarget(GxBuffers_Color, colorTarget);

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    CImVector clearColor(0xFF, 0xFF, 0xFF, 0xFF);

    if (CShaderEffect::s_usePcfFiltering) {
        GxRsSet(GxRs_ColorWrite, 0);
        g_theGxDevicePtr->DeviceGetRenderTarget(GxBuffers_Depth, depthTarget);
    }

    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    if (CShaderEffect::s_usePcfFiltering) {
        if (CShadowCache::s_extDynShadow.m_textures[0]) {
            GxRenderTargetSet(GxBuffers_Color, TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[0]), 0);
        }

        if (CShadowCache::s_extDynShadow.m_textures[1]) {
            GxRenderTargetSet(GxBuffers_Depth, TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[1]), 0);
            GxSceneClear(31, clearColor);
        }

        if (CShadowCache::s_intDynShadow) {
            GxRenderTargetSet(GxBuffers_Depth, TextureGetGxTex(CShadowCache::s_intDynShadow), 0);
            GxSceneClear(31, clearColor);
        }
    } else {
        GxRenderTargetSet(GxBuffers_Depth, NULL, 0);

        if (CShadowCache::s_extDynShadow.m_textures[0]) {
            GxRenderTargetSet(GxBuffers_Color, TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[0]), 0);
            GxSceneClear(31, clearColor);
        }

        if (CShadowCache::s_intDynShadow) {
            GxRenderTargetSet(GxBuffers_Color, TextureGetGxTex(CShadowCache::s_intDynShadow), 0);
            GxSceneClear(31, clearColor);
        }
    }

    EGxBuffer buffer;

    if (CShaderEffect::s_usePcfFiltering) {
        buffer = GxBuffers_Depth;

        if (CShadowCache::s_extDynShadow.m_textures[0]) {
            GxRenderTargetSet(GxBuffers_Color, TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[0]), 0);
        }
    } else {
        buffer = GxBuffers_Color;
        GxRenderTargetSet(GxBuffers_Depth, NULL, 0);
    }

    for (uint32_t i = 0; i < 3; i++) {
        CShadowBand& band = CShadowCache::s_extShadowBands[i];

        if (band.m_textures[0]) {
            GxRenderTargetSet(buffer, TextureGetGxTex(band.m_textures[0]), 0);
            GxSceneClear(31, clearColor);
        }

        if (band.m_textures[1]) {
            GxRenderTargetSet(buffer, TextureGetGxTex(band.m_textures[1]), 0);
            GxSceneClear(31, clearColor);
        }
    }

    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxRenderTargetSet(GxBuffers_Color, colorTarget, 0);

    if (CShaderEffect::s_usePcfFiltering) {
        GxRsSet(GxRs_ColorWrite, 15);
        GxRenderTargetSet(GxBuffers_Depth, depthTarget, 0);
    }

    for (uint32_t i = 0; i < 3; i++) {
        CShadowBand& band = CShadowCache::s_extShadowBands[i];
        band.m_lastPos = C3Vector(0.0f, 0.0f, 0.0f);
        band.m_pos = C3Vector(0.0f, 0.0f, 0.0f);
        band.m_updateCount = 0;
        band.m_current = 0;
    }

    CShadowCache::s_fullUpdate = 0;
    CShadowCache::s_fullClear = 0;
}

// OFFSET: %0x00441D00
int32_t CShadowCache::Update(const C3Vector& pos, int32_t useCullFrustum) {
    if (!CShadowCache::s_matrixCallback || !CShadowCache::s_frustumCallback || !CShadowCache::s_queryCallback || !CShadowCache::s_renderCallback) {
        return 0;
    }

    if (!CShadowCache::s_shadowMode) {
        return 1;
    }

    CShadowCacheQueryData data;
    CShadowCache::s_frameCounter++;

    float texelSize = CShadowCache::s_extDynShadow.m_radius / CShadowCache::s_shadowSize + CShadowCache::s_extDynShadow.m_radius / CShadowCache::s_shadowSize;

    C44Matrix lookAt;
    C3Vector up(1.0f, 0.0f, 0.0f);
    C3Vector eye(0.0f, 0.0f, 0.0f);
    GxuXformCreateLookAtXXX(eye, CShadowCache::s_sunDir, up, lookAt);

    float lightX = 1.0f / texelSize * (pos.z * lookAt.c0 + lookAt.b0 * pos.y + lookAt.a0 * pos.x + lookAt.d0);
    float lightY = 1.0f / texelSize * (pos.z * lookAt.c1 + lookAt.b1 * pos.y + lookAt.a1 * pos.x + lookAt.d1);

    C44Matrix inverse = lookAt.Inverse(lookAt.Determinant());

    float dx = (floor(lightX + 0.5f) - lightX) * texelSize;
    float dy = (floor(lightY + 0.5f) - lightY) * texelSize;

    C3Vector snapped;
    snapped.x = pos.x + (inverse.b0 * dy + dx * inverse.a0 + inverse.c0 * 0.0f + inverse.d0);
    snapped.y = pos.y + (inverse.b1 * dy + inverse.a1 * dx + inverse.c1 * 0.0f + inverse.d1);
    snapped.z = pos.z + (inverse.b2 * dy + inverse.a2 * dx + inverse.c2 * 0.0f + inverse.d2);

    float radius = CShadowCache::s_extDynShadow.m_radius;
    data.m_radius[0] = radius;
    data.m_pos[0] = snapped;
    CShadowCache::s_extDynShadow.m_lastPos = snapped;

    CGxTex* depthTarget = NULL;

    if (CShaderEffect::s_usePcfFiltering) {
        GxRsSet(GxRs_ColorWrite, 0);
        g_theGxDevicePtr->DeviceGetRenderTarget(GxBuffers_Depth, depthTarget);
    }

    data.m_bounds[0][0] = -radius;
    data.m_bounds[0][1] = radius;
    data.m_bounds[0][2] = -radius;
    data.m_bounds[0][3] = radius;
    data.m_texBounds[0][0] = 0.0f;
    data.m_texBounds[0][1] = 1.0f;
    data.m_texBounds[0][2] = 0.0f;
    data.m_texBounds[0][3] = 1.0f;

    CShadowCache::s_frustumCallback(data.m_frustums[0], data.m_matrices[0], CShadowCache::s_extDynShadow.m_dir, data, 0);
    data.m_frustums[0].Extents(data.m_extents[0]);

    CShadowCache::s_extDynShadowCullRect[0] = data.m_cullRects[0][0];
    CShadowCache::s_extDynShadow.m_dir.x = 1.0f;
    CShadowCache::s_extDynShadowCullRect[1] = data.m_cullRects[0][1];
    CShadowCache::s_extDynShadowCullRect[2] = data.m_cullRects[0][2];
    CShadowCache::s_extDynShadowCullRect[3] = data.m_cullRects[0][3];

    uint32_t frame = 0;
    data.m_lastBand = 0;
    CShadowCache::s_extDynShadow.m_dir.y = 0.0f;
    CShadowCache::s_extDynShadow.m_dir.z = 0.0f;

    if (CShadowCache::s_shadowMode >= SHADOW_HIGH) {
        frame = CShadowCache::s_frameCounter;
    }

    data.m_queryFlags[0] = 5;
    data.m_useFrustum = 0;
    CShadowCache::s_queryCallback(data, frame);

    CGxTex* target = TextureGetGxTex(CShadowCache::s_intDynShadow);
    CGxTex* colorTarget = NULL;

    if (CShaderEffect::s_usePcfFiltering) {
        target = TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[0]);
        colorTarget = TextureGetGxTex(CShadowCache::s_intDynShadow);
    }

    CShadowCache::s_renderCallback(data, 0, target, colorTarget, CShadowCache::s_extDynShadow.m_dir);

    if (useCullFrustum) {
        data.m_queryFlags[0] = 4;

        if (CShadowCache::s_shadowMode < SHADOW_HIGH) {
            if (CShadowCache::s_shadowMode >= SHADOW_MEDIUM) {
                data.m_queryFlags[0] = 22;
            }
        } else {
            data.m_queryFlags[0] = 30;
        }

        CShadowCache::s_queryCallback(data, frame);

        target = TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[0]);
        colorTarget = NULL;

        if (CShaderEffect::s_usePcfFiltering) {
            colorTarget = TextureGetGxTex(CShadowCache::s_extDynShadow.m_textures[1]);
        }

        CShadowCache::s_renderCallback(data, 0, target, colorTarget, CShadowCache::s_extDynShadow.m_dir);
    }

    if (CShadowCache::s_shadowMode > SHADOW_LOW) {
        if (CShadowCache::s_fullUpdate) {
            CShadowCache::ClearBandTextures();
        }

        if (useCullFrustum || CShadowCache::s_shadowMode < SHADOW_HIGH) {
            int32_t useBandCull = 0;
            CFrustum frustum;

            if (CShadowCache::s_shadowMode < SHADOW_HIGH) {
                data.m_useFrustum = 0;
            } else {
                useBandCull = 1;
                frustum = data.m_frustums[0];
                frustum.InvertPlanes();
                data.m_useFrustum = 1;
            }

            CShadowCache::GatherBands(data, pos, frustum, useBandCull);
            CShadowCache::RenderBands(data);
        }
    }

    if (CShaderEffect::s_usePcfFiltering) {
        GxRsSet(GxRs_ColorWrite, 15);
        GxRenderTargetSet(GxBuffers_Depth, depthTarget, 0);
    }

    return 1;
}

// OFFSET: %0x004422D0
void CShadowCache::PreUpdate(const C3Vector& sunDir) {
    CShadowCache::s_sunDir = sunDir;

    C44Matrix view;
    GxXformView(view);

    C3Vector& dir = CShadowCache::s_sunDir;
    float y = view.c1 * dir.z + view.b1 * dir.y + dir.x * view.a1;
    float z = view.c2 * dir.z + view.b2 * dir.y + dir.x * view.a2;
    float x = view.c0 * dir.z + dir.y * view.b0 + dir.x * view.a0;
    float scale = 1.0 / sqrt(z * z + y * y + x * x);

    CShadowCache::s_sunDirViewSpace.x = x * scale;
    CShadowCache::s_sunDirViewSpace.y = scale * y;
    CShadowCache::s_sunDirViewSpace.z = scale * z;
    CShadowCache::s_sunDirViewSpace.w = 0.0f;
}

// OFFSET: %0x00442460
void CShadowCache::Create() {
    CShadowCache::InitPcfOffsets();

    CShadowCache::s_extDynShadow.m_textures[0] = NULL;
    CShadowCache::s_extDynShadow.m_textures[1] = NULL;
    CShadowCache::s_intDynShadow = NULL;

    for (uint32_t i = 0; i < 3; i++) {
        CShadowCache::s_extShadowBands[i].m_textures[0] = NULL;
        CShadowCache::s_extShadowBands[i].m_textures[1] = NULL;
        CShadowCache::s_extShadowBands[i].m_current = 0;
    }

    CGxTexFlags flags;
    flags.m_filter = GxTex_Nearest;
    flags.m_wrapU = 0;
    flags.m_wrapV = 0;
    flags.m_renderTarget = 1;

    CGxTexFlags colorFlags = flags;
    CGxTexFlags depthFlags;
    EGxTexFormat depthFormat = GxTex_Unknown;

    if (CShaderEffect::s_usePcfFiltering) {
        if (g_theGxDevicePtr->DeviceApi() == GxApi_GLL && g_theGxDevicePtr->m_caps.m_pcfFormat[GxTex_D16] && g_theGxDevicePtr->m_caps.m_rttFormat[GxTex_D16]) {
            depthFlags = flags;
            depthFlags.m_filter = GxTex_Linear;
            depthFlags.m_bit3 = 1;
            depthFormat = GxTex_D16;
            colorFlags.m_filter = GxTex_Linear;
        } else {
            if (g_theGxDevicePtr->m_caps.m_pcfFormat[GxTex_D24X8] && g_theGxDevicePtr->m_caps.m_rttFormat[GxTex_D24X8]) {
                depthFormat = GxTex_D24X8;
            }

            depthFlags = flags;
            depthFlags.m_filter = GxTex_Linear;
            depthFlags.m_bit3 = 1;
            colorFlags.m_filter = GxTex_Linear;
        }
    } else {
        depthFormat = GxTex_R32F;
        depthFlags = flags;
        depthFlags.m_filter = GxTex_Nearest;
    }

    EGxTexFormat colorFormat = g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Null] ? GxTex_Null : GxTex_Argb8888;
    uint32_t size = CShadowCache::s_shadowSize;

    if (CShadowCache::s_shadowMode > SHADOW_LOWEST) {
        if (CShaderEffect::s_usePcfFiltering) {
            CShadowCache::s_extDynShadow.m_textures[0] = TextureCreate(GxTex_2d, size, size, 0, colorFormat, colorFormat, colorFlags, NULL, GxTexNullUserFunc, "ShadowCacheShadowColor", 0);
            CShadowCache::s_extDynShadow.m_textures[1] = TextureCreate(GxTex_2d, size, size, 0, depthFormat, depthFormat, depthFlags, NULL, GxTexNullUserFunc, "ShadowCacheExteriorDynamicShadow", 0);
        } else {
            CShadowCache::s_extDynShadow.m_textures[0] = TextureCreate(GxTex_2d, size, size, 0, depthFormat, depthFormat, depthFlags, NULL, GxTexNullUserFunc, "ShadowCacheExteriorDynamicShadow", 0);
            CShadowCache::s_extDynShadow.m_textures[1] = NULL;
        }

        CShadowCache::s_intDynShadow = TextureCreate(GxTex_2d, size, size, 0, depthFormat, depthFormat, depthFlags, NULL, GxTexNullUserFunc, "ShadowCacheInteriorDynamicShadow", 0);
        CShadowCache::s_extDynShadow.m_radius = 20.0f;

        if (CShadowCache::s_shadowMode > SHADOW_LOW) {
            for (uint32_t i = 0; i < 3; i++) {
                CShadowBand& band = CShadowCache::s_extShadowBands[i];
                band.m_textures[0] = TextureCreate(GxTex_2d, size, size, 0, depthFormat, depthFormat, depthFlags, NULL, GxTexNullUserFunc, "ShadowCacheShadowBand0", 0);

                if (CShadowCache::s_shadowMode < SHADOW_HIGH) {
                    band.m_textures[1] = TextureCreate(GxTex_2d, size, size, 0, depthFormat, depthFormat, depthFlags, NULL, GxTexNullUserFunc, "ShadowCacheShadowBand1", 0);
                }

                band.m_radius = CShadowCache::s_bandRadius[i];
                band.m_updateDistance = CShadowCache::s_bandUpdateDistance[i];
            }
        }
    }

    CShadowCache::ClearBandTextures();
    g_theGxDevicePtr->AddTextureRecreationCallback(CShadowCache::NotifyTexturesRecreate);

    CShadowCache::s_cutoffPlaneDefault.w = 0.0f;
    CShadowCache::s_cutoffPlane.w = 0.0f;
    CShadowCache::s_fullUpdate = 0;
    CShadowCache::s_fullClear = 0;
    CShadowCache::s_cutoffPlaneDefault.x = 0.0f;
    CShadowCache::s_cutoffPlaneDefault.y = 0.0f;
    CShadowCache::s_cutoffPlaneDefault.z = 0.0f;
    CShadowCache::s_cutoffPlane.x = 0.0f;
    CShadowCache::s_cutoffPlane.y = 0.0f;
    CShadowCache::s_cutoffPlane.z = 0.0f;
}

// OFFSET: %0x00442750
int32_t CShadowCache::Init() {
    CShadowCache::Create();

    return 1;
}

// OFFSET: %0x00442730
void CShadowCache::CreateResources() {
    if (!CShadowCache::s_recreateTextures) {
        return;
    }

    CShadowCache::Destroy();
    CShadowCache::Create();
    CShadowCache::s_recreateTextures = 0;
}

// OFFSET: 0x004380A0
void CShadowCache::SetShadowMapAlphaTest(int32_t enable) {
    if (CShaderEffect::s_forceAlphaTest) {
        GxRsSet(GxRs_AlphaRef, enable ? 0x80 : 0);
        return;
    }

    float constants[4] = { 0.0f, 0.0f, 0.0f, enable ? 0.50196081f : 0.0f };
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, constants, 1);
}

// OFFSET: 0x00438630
void CShadowCache::SetShadowMapModel(uint32_t boneInfluences, int32_t untextured) {
    uint32_t vertexPermute = boneInfluences > 2 ? 2 : boneInfluences;
    uint32_t pixelPermute = untextured ? 0 : (CShaderEffect::s_forceAlphaTest != 0) + 1;

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &CShadowCache::s_shadowMapVertexShader->m_permutes[vertexPermute]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &CShadowCache::s_shadowMapPixelShader->m_permutes[pixelPermute]);
    CShadowCache::SetShadowMapAlphaTest(!untextured);
}
