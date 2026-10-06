#include <cstddef>
#include <cstdint>
#include "Engine/Source/Model2/M2Shader.h"

// STATIC: %0x00C0D310
static const char* s_modelPixelShaders[33] = {
    "Combiners_Opaque",
    "Combiners_Mod",
    "Combiners_Opaque_Mod",
    "Combiners_Opaque_Mod2x",
    "Combiners_Opaque_Mod2xNA",
    "Combiners_Opaque_Opaque",
    "Combiners_Mod_Mod",
    "Combiners_Mod_Mod2x",
    "Combiners_Mod_Add",
    "Combiners_Mod_Mod2xNA",
    "Combiners_Mod_AddNA",
    "Combiners_Mod_Opaque",
    "Combiners_Opaque_Mod2xNA_Alpha",
    "Combiners_Opaque_AddAlpha",
    "Combiners_Opaque_AddAlpha_Alpha",
    "Combiners_Opaque_Mod2xNA_Alpha_Add",
    "Combiners_Mod_AddAlpha",
    "Combiners_Mod_AddAlpha_Alpha",
    "Combiners_Opaque_Alpha_Alpha",
    "Combiners_Opaque_Mod2xNA_Alpha_3s",
    "Combiners_Opaque_AddAlpha_Wgt",
    "Combiners_Mod_Add_Alpha",
    "Combiners_Opaque_ModNA_Alpha",
    "Combiners_Mod_AddAlpha_Wgt",
    "Combiners_Opaque_Mod_Add_Wgt",
    "Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha",
    "Combiners_Mod_Dual_Crossfade",
    "Combiners_Opaque_Mod2xNA_Alpha_Alpha",
    "Combiners_Mod_Masked_Dual_Crossfade",
    "Combiners_Opaque_Alpha",
    "Guild",
    "Guild_NoBorder",
    "Guild_Opaque",
};

// STATIC: %0x00C0D398
static const char* s_modelVertexShaders[16] = {
    "Diffuse_T1",
    "Diffuse_Env",
    "Diffuse_T1_T2",
    "Diffuse_T1_Env",
    "Diffuse_Env_T1",
    "Diffuse_Env_Env",
    "Diffuse_T1_Env_T1",
    "Diffuse_T1_T1",
    "Diffuse_T1_T1_T1",
    "Diffuse_EdgeFade_T1",
    "Diffuse_T2",
    "Diffuse_T1_Env_T2",
    "Diffuse_EdgeFade_T1_T2",
    "Diffuse_T1_T1_T1_T2",
    "Diffuse_EdgeFade_Env",
    "Diffuse_T1_T2_T1",
};

// STATIC: %0x00C0D3D8
static const char* s_modelHullShaders[4] = {
    "T1",
    "T1_T2",
    "T1_T2_T3",
    "T1_T2_T3_T4",
};

// STATIC: %0x00C0D3E8
static const char* s_modelDomainShaders[4] = {
    "T1",
    "T1_T2",
    "T1_T2_T3",
    "T1_T2_T3_T4",
};

// STATIC: %0x00C0D3F8
static M2ShaderEffect s_shaderEffects[29] = {
    { 12, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 13, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 14, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 15, 6, 2, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 16, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 13, 7, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 16, 7, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 17, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 18, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 19, 6, 2, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 20, 7, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 21, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 22, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 23, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 23, 7, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 20, 2, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 24, 3, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 25, 6, 2, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 26, 8, 2, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 1, 9, 0, 0, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 27, 11, 2, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 6, 12, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 28, 13, 3, 3, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 29, 7, 1, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 25, 11, 2, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3) },
    { 1, 14, 0, 0, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 30, 15, 2, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 31, 2, 1, 2, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
    { 32, 15, 2, 1, static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0) },
};

// STATIC: %0x00C0D6B0
static EGxTexOp s_alphaOps[8] = { static_cast<EGxTexOp>(3), static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(3), static_cast<EGxTexOp>(2), static_cast<EGxTexOp>(1), static_cast<EGxTexOp>(3), static_cast<EGxTexOp>(3), static_cast<EGxTexOp>(3) };

// STATIC: %0x00C0D6D0
static EGxTexOp s_colorOps[8] = { static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(0), static_cast<EGxTexOp>(4), static_cast<EGxTexOp>(2), static_cast<EGxTexOp>(1), static_cast<EGxTexOp>(5), static_cast<EGxTexOp>(1), static_cast<EGxTexOp>(2) };

// OFFSET: %0x0079FA00
uint32_t M2GetPixelShaderID(uint32_t textureCount, uint16_t shader) {
    if (shader & 0x8000) {
        uint32_t shaderID = shader & 0x7FFF;

        if (shaderID >= 29) {
            return 0;
        }

        return s_shaderEffects[shaderID].m_pixelShader;
    }

    uint32_t op0 = (shader >> 4) & 7;

    if (textureCount == 1) {
        return op0 != 0;
    }

    if (op0) {
        switch (shader & 7) {
            case 0:
                return 11;
            case 3:
                return 8;
            case 4:
                return 7;
            case 6:
                return 9;
            case 7:
                return 10;
            default:
                return 6;
        }
    }

    switch (shader & 7) {
        case 0:
            return 5;
        case 3:
        case 7:
            return 13;
        case 4:
            return 3;
        case 6:
            return 4;
        default:
            return 2;
    }
}

// OFFSET: %0x0079FAF0
uint32_t M2GetVertexShaderID(uint32_t textureCount, uint16_t shader) {
    if (shader & 0x8000) {
        uint32_t shaderID = shader & 0x7FFF;

        if (shaderID >= 29) {
            return 0;
        }

        return s_shaderEffects[shaderID].m_vertexShader;
    }

    uint32_t env0 = (shader >> 4) & 8;

    if (textureCount == 1) {
        if (env0) {
            return 1;
        }

        return (shader & 0x4000) ? 10 : 0;
    }

    if (env0) {
        return ((shader & 8) != 0) + 4;
    }

    if (shader & 8) {
        return 3;
    }

    return (shader & 0x4000) ? 2 : 7;
}

// OFFSET: %0x0079FB80
const char* M2GetPixelShaderName(uint32_t textureCount, uint16_t shader) {
    return s_modelPixelShaders[M2GetPixelShaderID(textureCount, shader)];
}

// OFFSET: %0x0079FBA0
const char* M2GetVertexShaderName(uint32_t textureCount, uint16_t shader) {
    return s_modelVertexShaders[M2GetVertexShaderID(textureCount, shader)];
}

// OFFSET: %0x0079FBC0
const char* M2GetHullShaderName(uint32_t textureCount, uint16_t shader) {
    if (!(shader & 0x8000)) {
        return s_modelHullShaders[textureCount != 1];
    }

    uint32_t shaderID = shader & 0x7FFF;

    if (shaderID >= 29) {
        return s_modelHullShaders[0];
    }

    return s_modelHullShaders[s_shaderEffects[shaderID].m_hullShader];
}

// OFFSET: %0x0079FC10
const char* M2GetDomainShaderName(uint32_t textureCount, uint16_t shader) {
    if (!(shader & 0x8000)) {
        return s_modelDomainShaders[textureCount != 1];
    }

    uint32_t shaderID = shader & 0x7FFF;

    if (shaderID >= 29) {
        return s_modelDomainShaders[0];
    }

    return s_modelDomainShaders[s_shaderEffects[shaderID].m_domainShader];
}

// OFFSET: %0x0079FC60
void M2GetFixedFunctionFallback(uint16_t shader, EGxTexOp* colorOp, EGxTexOp* alphaOp) {
    if (shader & 0x8000) {
        uint32_t shaderID = shader & 0x7FFF;
        *colorOp = s_shaderEffects[shaderID].m_colorOp;
        *alphaOp = s_shaderEffects[shaderID].m_alphaOp;
    } else {
        *colorOp = static_cast<EGxTexOp>(0);
        *alphaOp = static_cast<EGxTexOp>((shader & 0x70) ? 0 : 3);
    }
}

// PROVEN: c04f6b0c IDENTICAL
// OFFSET: 0x00839890
void M2GetCombinerOps(uint16_t shader, uint32_t textureCount, EGxTexOp* colorOps, EGxTexOp* alphaOps) {
    uint32_t ops[2];
    ops[0] = (shader >> 4) & 7;
    ops[1] = shader & 7;

    for (uint32_t i = 0; i < textureCount; i++) {
        colorOps[i] = s_colorOps[ops[i]];
        alphaOps[i] = s_alphaOps[ops[i]];
    }
}
