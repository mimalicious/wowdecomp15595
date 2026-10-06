#ifndef ENGINE_SOURCE_MODEL2_M2SHADER_H
#define ENGINE_SOURCE_MODEL2_M2SHADER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/ShaderEffect/Types.h"

struct M2ShaderEffect {
    /* 0000 */ uint32_t m_pixelShader;
    /* 0004 */ uint32_t m_vertexShader;
    /* 0008 */ uint32_t m_hullShader;
    /* 000C */ uint32_t m_domainShader;
    /* 0010 */ EGxTexOp m_colorOp;
    /* 0014 */ EGxTexOp m_alphaOp;
};

void M2GetCombinerOps(uint16_t shader, uint32_t textureCount, EGxTexOp* colorOps, EGxTexOp* alphaOps);
const char* M2GetDomainShaderName(uint32_t textureCount, uint16_t shader);
void M2GetFixedFunctionFallback(uint16_t shader, EGxTexOp* colorOp, EGxTexOp* alphaOp);
const char* M2GetHullShaderName(uint32_t textureCount, uint16_t shader);
uint32_t M2GetPixelShaderID(uint32_t textureCount, uint16_t shader);
const char* M2GetPixelShaderName(uint32_t textureCount, uint16_t shader);
uint32_t M2GetVertexShaderID(uint32_t textureCount, uint16_t shader);
const char* M2GetVertexShaderName(uint32_t textureCount, uint16_t shader);

#endif
