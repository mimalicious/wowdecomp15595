#include "WoW/Source/LiquidSystem/Liquid.h"
#include <cmath>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/CRect.h"

// STATIC: 0x00E4ED00
uint32_t Liquid::CMaterialDebug::s_refCount;

// STATIC: 0x00E4ECFC
CGxShader* Liquid::CMaterialDebug::s_vertexShader;

// STATIC: 0x00E4ECF8
CGxShader* Liquid::CMaterialDebug::s_pixelShader;

// STATIC: 0x00E4ED08
C4Vector Liquid::CMaterialDebug::s_vsConsts[50] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f)
};

// STATIC: 0x00D00730
C4Vector Liquid::CMaterialDebug::s_psConsts[17] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f)
};

// STATIC: 0x00E4F030
uint32_t Liquid::CMaterialMercury::s_refCount;

// STATIC: 0x00E4F02C
CGxShader* Liquid::CMaterialMercury::s_vertexShader;

// STATIC: 0x00E4F028
CGxShader* Liquid::CMaterialMercury::s_pixelShader;

// STATIC: 0x00E4F038
C4Vector Liquid::CMaterialMercury::s_vsConsts[53] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f)
};

// STATIC: 0x00E4F388
C4Vector Liquid::CMaterialMercury::s_psConsts[32] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f)
};

// STATIC: 0x00E4F598
uint32_t Liquid::CMaterialMagma::s_refCount;

// STATIC: 0x00E4F594
CGxShader* Liquid::CMaterialMagma::s_vertexShader;

// STATIC: 0x00E4F590
CGxShader* Liquid::CMaterialMagma::s_pixelShader;

// STATIC: 0x00E4F58C
CGxShader* Liquid::CMaterialMagma::s_procVertexShader;

// STATIC: 0x00E4F588
CGxShader* Liquid::CMaterialMagma::s_procPixelShader;

// STATIC: 0x00E4F5A0
C4Vector Liquid::CMaterialMagma::s_vsConsts[51] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f)
};

// STATIC: 0x00D00850
C4Vector Liquid::CMaterialMagma::s_psConsts[18] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f)
};

// STATIC: 0x00E4F8E0
uint32_t Liquid::CMaterialWater::s_refCount;

// STATIC: 0x00E4F8DC
CGxShader* Liquid::CMaterialWater::s_vertexShader;

// STATIC: 0x00E4F8D8
CGxShader* Liquid::CMaterialWater::s_pixelShader;

// STATIC: 0x00E4F8D4
CGxShader* Liquid::CMaterialWater::s_pixelShaderBelow;

// STATIC: 0x00E4F8D0
CGxShader* Liquid::CMaterialWater::s_pixelShaderAboveFog;

// STATIC: 0x00E4F8E8
C4Vector Liquid::CMaterialWater::s_vsConsts[53] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f)
};

// STATIC: 0x00E4FC38
C4Vector Liquid::CMaterialWater::s_psConsts[32] = {
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(1.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 1.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 1.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 1.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f),
    C4Vector(0.0f, 0.0f, 0.0f, 0.0f)
};

static void ReleaseShader(CGxShader*& shader) {
    if (shader) {
        g_theGxDevicePtr->ShaderRelease(shader);
        shader = NULL;
    }
}

static void DrawInstance(Liquid::CInstance* instance, EGxVertexBufferFormat format, CGxShader* vertexShader, uint32_t permute, int32_t requireCount) {
    CGxBuf* vertexBuf = NULL;
    CGxBuf* indexBuf = NULL;
    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleFan;
    batch.m_start = 0;
    batch.m_count = 0;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 0;
    instance->m_geomFactory->GetBuffers(format, vertexBuf, indexBuf, batch, instance->m_object->m_noReflection == 0);

    if (requireCount && !batch.m_count) {
        return;
    }

    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, format);

    if (vertexShader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &vertexShader->m_permutes[permute]);
    }

    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    g_theGxDevicePtr->Draw(batch);
}

static void SetWorldTransform(const C3Vector& camPos, Liquid::CInstance* instance) {
    C44Matrix transform = instance->m_transform;

    if (Liquid::UseCameraRelativeCoords()) {
        transform.d0 -= camPos.x;
        transform.d1 -= camPos.y;
        transform.d2 -= camPos.z;
    }

    GxXformSet(GxXform_World, transform);
}

static void SetLighting(const C3Vector& camPos, Liquid::CInstance* instance, C4Vector* vsConsts, C4Vector* psConsts) {
    CM2Lighting lighting(instance->m_bounds);
    instance->m_environment->SelectLights(lighting);

    if (Liquid::GetWaterDetail() || Liquid::GetShadersEnabled()) {
        Liquid::SetShaderLighting(vsConsts, psConsts, lighting);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(vsConsts), 14);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(psConsts), 5);
    } else {
        lighting.SetupGxLights(&camPos);
        lighting.SetupGxFog();
    }
}

static void SetDefaultTransform(const C3Vector& camPos, C4Vector* vsConsts, C4Vector* psConsts) {
    if (!Liquid::GetShadersEnabled()) {
        return;
    }

    C44Matrix identity;
    Liquid::SetupTransform(&vsConsts[14], &psConsts[5], camPos, identity);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 14, reinterpret_cast<const float*>(&vsConsts[14]), 12);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 5, reinterpret_cast<const float*>(&psConsts[5]), 12);
}

static void SetInstanceTransform(const C3Vector& camPos, Liquid::CInstance* instance, C4Vector* vsConsts, C4Vector* psConsts) {
    if (instance->m_geomFactory->SetTransform(instance->m_transform)) {
        return;
    }

    Liquid::SetupTransform(&vsConsts[14], &psConsts[5], camPos, instance->m_transform);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 14, reinterpret_cast<const float*>(&vsConsts[14]), 12);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 5, reinterpret_cast<const float*>(&psConsts[5]), 12);
}

static void SetFog(C4Vector* fog) {
    float start = 0.0f;
    float end = 0.0f;
    float density = 0.0f;
    CImVector color(0);
    Liquid::GetUnderWaterFog(start, end, density, color);

    if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
        float scale = 1.0f / (end - start);
        fog[0].x = -(CShaderEffect::s_fogMul * scale);
        fog[0].y = scale * end;
        fog[0].z = density;
    } else {
        fog[0].x = 0.0f;
        fog[0].y = 1.0f;
        fog[0].z = 1.0f;
    }

    fog[0].w = 0.0f;
    fog[1] = C4Vector(color);

    Liquid::GetAboveWaterFog(start, end, density, color);

    if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
        float scale = 1.0f / (end - start);
        fog[2].z = density;
        fog[2].x = -(CShaderEffect::s_fogMul * scale);
        fog[2].y = scale * end;
    } else {
        fog[2].y = 1.0f;
        fog[2].x = 0.0f;
        fog[2].z = 1.0f;
    }

    fog[2].w = 0.0f;
    fog[3] = C4Vector(color);
}

static void SetDisplacementConstants(const C3Vector& camPos, C4Vector* vsDispl, C4Vector* psDispl, C4Vector* interpolation) {
    if (Liquid::HasDynamicDisplMap()) {
        C3Vector pos(0.0f, 0.0f, 0.0f);
        C2Vector size(0.0f, 0.0f);

        for (uint32_t i = 0; i < Liquid::GetDynamicDisplMapCnt(); i++) {
            Liquid::GetDisplMapInfoRaw(i, pos, size);

            if (Liquid::UseCameraRelativeCoords()) {
                pos.x -= camPos.x;
                pos.y -= camPos.y;
                pos.z -= camPos.z;
            }

            vsDispl[i].y = pos.y;
            vsDispl[i].x = pos.x;
            vsDispl[i].z = 1.0f / size.x;
            vsDispl[i].w = 1.0f / size.y;
            C2iVector texSize = Liquid::GetDisplMapTexSize(i);
            float invX = 1.0f / static_cast<float>(texSize.x);
            float invY = 1.0f / static_cast<float>(texSize.y);
            psDispl[i].x = invX;
            psDispl[i].y = invY;
            psDispl[i].z = invX * 0.5f;
            psDispl[i].w = invY * 0.5f;
        }

        float first = Liquid::GetDynamicDisplMapInterpolation(0);
        float second = Liquid::GetDynamicDisplMapInterpolation(1);
        interpolation->x = first;
        interpolation->y = second;
    } else {
        for (uint32_t i = 0; i < Liquid::GetDynamicDisplMapCnt(); i++) {
            vsDispl[i] = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
            psDispl[i] = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
        }

        interpolation->x = 0.0f;
        interpolation->y = 0.0f;
    }

    interpolation->w = 0.0f;
    interpolation->z = 0.0f;
}

static uint32_t AbovePermute(uint32_t reflection) {
    uint32_t depthMode = DepthCache::GetDepthMode() - 1;
    uint32_t displCount = Liquid::GetDynamicDisplMapCnt();
    uint32_t hardwareDepth = 1;

    if (DepthCache::GetDepthMode() == 3 && TextureGetGxTex(DepthCache::GetDepthTex())->m_format == 23) {
        hardwareDepth = 0;
    }

    uint32_t permute = hardwareDepth + 2 * (depthMode + displCount + 2 * depthMode + reflection + 2 * (depthMode + displCount + 2 * depthMode));

    if (Liquid::GetWaterDetail() == 1) {
        permute = 0;
    }

    return permute;
}

// PROVEN: 314be33d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Liquid::IMaterial::Reset() {
}

// PROVEN: 4d164714 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Liquid::IMaterial::OnAddToScene() {
}

// PROVEN: 2e7e7d46 IDENTICAL/NONE
// OFFSET: 0x005EB640
int32_t Liquid::IMaterial::SetupDefaultTransform(const C3Vector& camPos) {
    return 1;
}

// PROVEN: 203466c5 IDENTICAL/NONE
// OFFSET: 0x005EB640
int32_t Liquid::CMaterialDebug::IsAllLoaded(CMaterialSettings* settings) {
    return 1;
}

// OFFSET: 0x0071B4C0/0x1403EFB40
const C44Matrix* Liquid::GetTexScrollMtx(uint32_t time, const C2Vector& speed) {
    float x = 0.0f;
    float y = 0.0f;

    if (speed.x != 0.0f) {
        uint32_t period = static_cast<uint32_t>(static_cast<int64_t>(1000.0f / speed.x));
        x = static_cast<float>(static_cast<double>(time % period) / static_cast<double>(period));
    }

    if (speed.y != 0.0f) {
        uint32_t period = static_cast<uint32_t>(static_cast<int64_t>(1000.0f / speed.y));
        y = static_cast<float>(static_cast<double>(time % period) / static_cast<double>(period));
    }

    static C44Matrix s_matrix;
    s_matrix.d0 = x;
    s_matrix.d1 = y;
    s_matrix.d2 = 0.0f;
    return &s_matrix;
}

// PROVEN: 2a3bc847 IDENTICAL/NONE
// OFFSET: 0x0071B660
void Liquid::SetMatrixConstant(C4Vector* constants, uint32_t index, const C44Matrix& matrix) {
    *reinterpret_cast<C44Matrix*>(&constants[index * 4]) = matrix;
}

// OFFSET: 0x0071B680
void Liquid::SetupTransform(C4Vector* vsConstants, C4Vector* psConstants, const C3Vector& camPos, const C44Matrix& transform) {
    C44Matrix model = transform;

    if (Liquid::UseCameraRelativeCoords()) {
        model.d0 -= camPos.x;
        model.d1 -= camPos.y;
        model.d2 -= camPos.z;
    }

    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix view;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    C44Matrix projection;
    projection = device->m_projNative;

    if (!device->DeviceApi()) {
        projection.c0 *= -1.0f;
        projection.c1 *= -1.0f;
        projection.c2 *= -1.0f;
        projection.c3 *= -1.0f;
    }

    C44Matrix product = model * view;
    *reinterpret_cast<C44Matrix*>(&vsConstants[0]) = model;
    *reinterpret_cast<C44Matrix*>(&vsConstants[4]) = product;
    *reinterpret_cast<C44Matrix*>(&vsConstants[8]) = projection;
    product = view * projection;
    *reinterpret_cast<C44Matrix*>(&psConstants[0]) = view;
    *reinterpret_cast<C44Matrix*>(&psConstants[4]) = projection;
    *reinterpret_cast<C44Matrix*>(&psConstants[8]) = product;
}

// OFFSET: 0x0071B8B0
void Liquid::SetShaderLighting(C4Vector* vsConstants, C4Vector* psConstants, const CM2Lighting& lighting) {
    C44Matrix view;
    view = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    C3Vector dir = lighting.m_sunDir * C33Matrix(view);
    vsConstants[1] = C4Vector(dir.x, dir.y, dir.z, 1.0f);
    vsConstants[2] = C4Vector(lighting.m_sunAmbient.x, lighting.m_sunAmbient.y, lighting.m_sunAmbient.z, 1.0f);
    vsConstants[3] = C4Vector(lighting.m_sunDiffuse.x, lighting.m_sunDiffuse.y, lighting.m_sunDiffuse.z, 1.0f);
    vsConstants[4] = C4Vector(lighting.m_sunSpecular.x, lighting.m_sunSpecular.y, lighting.m_sunSpecular.z, 6.0f);
    psConstants[1] = C4Vector(-lighting.m_sunDir.x, -lighting.m_sunDir.y, -lighting.m_sunDir.z, -1.0f);
    psConstants[2] = C4Vector(lighting.m_sunAmbient.x, lighting.m_sunAmbient.y, lighting.m_sunAmbient.z, 1.0f);
    psConstants[3] = C4Vector(lighting.m_sunDiffuse.x, lighting.m_sunDiffuse.y, lighting.m_sunDiffuse.z, 1.0f);
    psConstants[4] = C4Vector(lighting.m_sunSpecular.x, lighting.m_sunSpecular.y, lighting.m_sunSpecular.z, 50.0f);

    if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog) && fabsf(lighting.m_fogScale) >= 0.00000023841858f) {
        float scale = 1.0f / (lighting.m_fogEnd - lighting.m_fogStart);
        vsConstants[0] = C4Vector(-(scale * CShaderEffect::s_fogMul), scale * lighting.m_fogEnd, lighting.m_fogDensity, 0.0f);
    } else {
        vsConstants[0] = C4Vector(0.0f, 1.0f, 1.0f, 0.0f);
    }

    psConstants[0] = C4Vector(lighting.m_fogColor.x, lighting.m_fogColor.y, lighting.m_fogColor.z, 1.0f);
}

// OFFSET: 0x0071BCE0/0x1403EFC90
void Liquid::SetScreenConstants(C4Vector* constants) {
    CRect rect;
    rect.minY = 0.0f;
    rect.minX = 0.0f;
    rect.maxY = 0.0f;
    rect.maxX = 0.0f;
    g_theGxDevicePtr->CapsWindowSize(rect);
    float invWidth = 1.0f / rect.maxX;
    float invHeight = 1.0f / rect.maxY;
    float halfX = invWidth * 0.5f;
    float halfY = invHeight * 0.5f;
    constants[0] = C4Vector(rect.maxX, rect.maxY, invWidth, invHeight);
    constants[1] = C4Vector(halfX, halfY, 1.0f / halfX, 1.0f / halfY);
    constants[2] = ViewSettings::CalculateHomToScreen(halfX, halfY);
}

// PROVEN: 01980596 IDENTICAL/NONE
// OFFSET: 0x0071D9D0
int32_t Liquid::IMaterial::SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection) {
    return 1;
}

// OFFSET: 0x0071D9E0/0x1403F3070
Liquid::IMaterial::~IMaterial() {
}

// OFFSET: 0x0071DDF0/0x1403F31B0
void Liquid::CMaterialDebug::Render(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    C44Matrix identity;
    const C44Matrix* transform = instance->m_geomFactory->SetTransform(instance->m_transform) ? &identity : &instance->m_transform;
    Liquid::SetupTransform(&s_vsConsts[14], &s_psConsts[5], camPos, *transform);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 50);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 17);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(9), NULL, 0, 0);
}

// OFFSET: 0x0071E0C0/0x1403F3480
Liquid::CMaterialDebug::CMaterialDebug() {
    if (!s_refCount++) {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "LiquidDebug");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "LiquidDebug");
    }
}

// OFFSET: [0x0071E110/0x1403F34F0, 0x0071E340/0x1403F37C0]
Liquid::CMaterialDebug::~CMaterialDebug() {
    if (s_refCount-- == 1) {
        ReleaseShader(s_vertexShader);
        ReleaseShader(s_pixelShader);
    }
}

// PROVEN: fa32a1fe IDENTICAL/NONE
// OFFSET: 0x0071E170/0x1403F3580
void Liquid::CMaterialDebug::Reset() {
    ReleaseShader(s_vertexShader);
    ReleaseShader(s_pixelShader);
    s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "LiquidDebug");
    s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "LiquidDebug");
}

// OFFSET: 0x0071E1E0/0x1403F3610
int32_t Liquid::CMaterialDebug::SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection) {
    GxRsSet(GxRs_BlendingMode, 2);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 1);
    C44Matrix scale;
    scale.Scale(Liquid::WorldCoordScale());
    Liquid::SetMatrixConstant(&s_vsConsts[26], 0, scale);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_vertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[0]);
    this->m_render = CMaterialDebug::Render;
    return 1;
}

// PROVEN: f8069bd4 IDENTICAL/NONE
// OFFSET: 0x00873190
int32_t Liquid::IMaterial::SetupLighting(const C3Vector& camPos, CInstance* instance) {
    return 1;
}

// PROVEN: ac6cc6a4 IDENTICAL/NONE
// OFFSET: 0x00904350
void Liquid::IMaterial::Release() {
    delete this;
}

// PROVEN: cda9a1fe IDENTICAL/NONE
// OFFSET: 0x00847950
void Liquid::CMaterialDebug::LinkToAsyncHead(CMaterialSettings* settings) {
}

// OFFSET: 0x0071E370/0x1403F3810
void Liquid::CMaterialMercury::OnAddToScene() {
    if (Liquid::GetWaterDetail()) {
        Liquid::SetNeedBackBuffer(1);

        if (Liquid::HasDynamicDisplMap()) {
            Liquid::SetNeedDynDisplMap(1);
        }
    }
}

// OFFSET: 0x0071E3A0/0x1403F3840
int32_t Liquid::CMaterialMercury::IsAllLoaded(CMaterialSettings* settings) {
    if (Liquid::GetWaterDetail()) {
        return settings->IsAllLoaded(2) ? 1 : 0;
    }

    return settings->IsAllLoaded(0) ? 1 : 0;
}

// OFFSET: 0x0071E3E0/0x1403F3890
void Liquid::CMaterialMercury::LinkToAsyncHead(CMaterialSettings* settings) {
    settings->LinkToAsyncHead(Liquid::GetWaterDetail() ? 2 : 0);
}

// OFFSET: 0x0071E540
void Liquid::CMaterialMercury::SetFogConstants() {
    SetFog(&s_psConsts[18]);
}

// OFFSET: 0x0071E740/0x1403F3BC0
void Liquid::CMaterialMercury::InitResources() {
    if (!Liquid::GetShadersEnabled()) {
        return;
    }

    uint32_t detail = Liquid::GetWaterDetail();
    const char* pixelName;

    if (detail) {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ProcMercury");
        pixelName = detail == 1 ? "MediumMercury" : "ProcMercury";
    } else {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "Mercury");
        pixelName = "Mercury";
    }

    s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, pixelName);
}

// OFFSET: 0x0071E7B0/0x1403F3C40
int32_t Liquid::CMaterialMercury::SetupDefaultTransform(const C3Vector& camPos) {
    SetDefaultTransform(camPos, s_vsConsts, s_psConsts);
    return 1;
}

// OFFSET: 0x0071E880/0x1403F3D40
int32_t Liquid::CMaterialMercury::SetupLighting(const C3Vector& camPos, CInstance* instance) {
    SetLighting(camPos, instance, s_vsConsts, s_psConsts);
    return 1;
}

// OFFSET: 0x0071EC00/0x1403F3FF0
Liquid::CMaterialMercury::CMaterialMercury() {
    if (!s_refCount++) {
        CMaterialMercury::InitResources();
    }
}

// OFFSET: [0x0071EC30/0x1403F4030, 0x0071F020/0x1403F4610]
Liquid::CMaterialMercury::~CMaterialMercury() {
    if (s_refCount-- == 1) {
        ReleaseShader(s_vertexShader);
        ReleaseShader(s_pixelShader);
    }
}

// PROVEN: fe1fcd7b IDENTICAL/NONE
// OFFSET: 0x0071EC90/0x1403F40C0
void Liquid::CMaterialMercury::Reset() {
    ReleaseShader(s_vertexShader);
    ReleaseShader(s_pixelShader);
    CMaterialMercury::InitResources();
}

// OFFSET: 0x0071ECE0
void Liquid::CMaterialMercury::RenderShader(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetInstanceTransform(camPos, instance, s_vsConsts, s_psConsts);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(7), s_vertexShader, 0, 1);
}

// OFFSET: 0x0071EDF0
void Liquid::CMaterialMercury::RenderFixedFunction(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetWorldTransform(camPos, instance);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(7), NULL, 0, 1);
}

// OFFSET: 0x0071EF00
void Liquid::CMaterialMercury::RenderProcedural(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetInstanceTransform(camPos, instance, s_vsConsts, s_psConsts);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(7), s_vertexShader, 4 * Liquid::GetDynamicDisplMapCnt(), 1);
}

// OFFSET: 0x0071F050/0x1403F4660
int32_t Liquid::CMaterialMercury::SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection) {
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Fog, 1);

    float detailScale = Liquid::WorldCoordScale() * settings->GetFloat(2);
    C44Matrix detail;
    detail.b1 = detailScale;

    if (Liquid::GetWaterDetail()) {
        GxRsSet(GxRs_DepthWrite, 0);
        CGxTex* surface = settings->GetTexture(2, 0);

        if (!surface) {
            return 0;
        }

        CGxTex* backBuffer = TextureGetGxTex(Liquid::GetBackBuffer());

        if (!backBuffer) {
            return 0;
        }

        CGxTex* reflectionTex = TextureGetGxTex(reflection == 1 ? Liquid::GetBlurredReflectionBuffer() : Liquid::GetBlurredBackBuffer());

        if (!reflectionTex) {
            return 0;
        }

        CGxTex* fakeTex = TextureGetGxTex(reflection == 1 ? Liquid::GetRealReflectionBuffer() : Liquid::GetFakeReflectionBuffer());

        if (!fakeTex) {
            return 0;
        }

        float scales[2];

        for (uint32_t i = 0; i < 2; i++) {
            scales[i] = Liquid::WorldCoordScale() * settings->GetFloat(i + 3);
        }

        float rotation = settings->GetFloat(5) * 0.017453292f;

        for (uint32_t i = 0; i < 2; i++) {
            C44Matrix matrix = C44Matrix::RotationAroundZ(rotation);
            matrix.Scale(scales[i]);
            Liquid::SetMatrixConstant(&s_vsConsts[26], i, matrix);
        }

        float f6 = settings->GetFloat(6);
        float f7 = settings->GetFloat(7);
        float f8 = settings->GetFloat(8);
        float f12 = settings->GetFloat(12);
        float f11 = settings->GetFloat(11);
        float f9 = settings->GetFloat(9);
        float f10 = settings->GetFloat(10);
        s_psConsts[27] = C4Vector(f12, f11, f9, f10);
        float f13 = settings->GetFloat(13);
        float f14 = settings->GetFloat(14);
        float f15 = settings->GetFloat(15);
        s_psConsts[23] = C4Vector(f13, f14, f15, f6);

        g_theGxDevicePtr->RsSet(GxRs_Texture0, backBuffer);
        g_theGxDevicePtr->RsSet(GxRs_Texture1, fakeTex);
        g_theGxDevicePtr->RsSet(GxRs_Texture3, reflectionTex);
        g_theGxDevicePtr->RsSet(GxRs_Texture5, surface);

        if (Liquid::GetWaterDetail() > 1) {
            g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(DepthCache::GetDepthSR()));

            if (Liquid::HasDynamicDisplMap()) {
                g_theGxDevicePtr->RsSet(GxRs_Texture4, Liquid::GetDynamicDisplMap(0));

                if (Liquid::GetDynamicDisplMapCnt() > 1) {
                    g_theGxDevicePtr->RsSet(GxRs_Texture8, Liquid::GetDynamicDisplMap(1));
                }
            }
        }

        s_psConsts[22] = ViewSettings::CalculateParams();
        s_vsConsts[50] = C4Vector(f6, f7, 0.0f, f8);
        s_psConsts[28] = C4Vector(f6, f7, 0.0f, 1.0f);
        SetDisplacementConstants(camPos, &s_vsConsts[51], &s_psConsts[29], &s_psConsts[31]);

        Liquid::SetMatrixConstant(&s_vsConsts[26], 2, detail);
        C44Matrix scroll = *Liquid::GetTexScrollMtx(time, C2Vector(0.0099999998f, 0.0f));
        Liquid::SetMatrixConstant(&s_vsConsts[26], 3, scroll);
        s_psConsts[17] = C4Vector(camPos.x, camPos.y, camPos.z, 1.0f);
        Liquid::SetScreenConstants(&s_psConsts[24]);
        CMaterialMercury::SetFogConstants();
        GxRsSet(GxRs_Culling, 0);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[AbovePermute(reflection)]);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 53);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 32);
        this->m_render = CMaterialMercury::RenderProcedural;
        return 1;
    }

    if (g_theGxDevicePtr->m_caps.m_maxClipPlanes || g_theGxDevicePtr->m_caps.unk_02BC) {
        GxRsSet(GxRs_DepthWrite, 0);
    } else {
        GxRsSet(GxRs_DepthWrite, 1);
    }

    CGxTex* texture = settings->GetTexture(0, settings->GetInt(1));

    if (!texture) {
        return 0;
    }

    float scale = Liquid::WorldCoordScale() * settings->GetFloat(0);
    float rotation = settings->GetFloat(1) * 0.017453292f;
    GxRsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, texture);
    C44Matrix matrix = C44Matrix::RotationAroundZ(rotation);
    matrix.Scale(scale);

    if (Liquid::GetShadersEnabled()) {
        Liquid::SetMatrixConstant(&s_vsConsts[26], 1, detail);
        Liquid::SetMatrixConstant(&s_vsConsts[26], 0, matrix);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[0]);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 53);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 32);
        this->m_render = CMaterialMercury::RenderShader;
    } else {
        GxRsSet(GxRs_TextureCoord0, 1);
        GxXformSet(GxXform_Tex0, detail);
        GxRsSet(GxRs_TextureCoord1, 0);
        GxXformSet(GxXform_Tex1, matrix);
        GxRsSet(GxRs_TextureShader1, 1);
        GxRsSet(GxRs_ColorOp1, 2);
        GxRsSet(GxRs_AlphaOp1, 3);
        this->m_render = CMaterialMercury::RenderFixedFunction;
    }

    return 1;
}

// OFFSET: 0x0071F9F0/0x1403F54D0
int32_t Liquid::CMaterialMagma::IsAllLoaded(CMaterialSettings* settings) {
    if (!Liquid::GetWaterDetail()) {
        return settings->IsAllLoaded(0) != 0;
    }

    for (uint32_t i = 1; i <= 3; i++) {
        if (!settings->IsAllLoaded(i)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x0071FA40/0x1403F5530
void Liquid::CMaterialMagma::LinkToAsyncHead(CMaterialSettings* settings) {
    if (!Liquid::GetWaterDetail()) {
        settings->LinkToAsyncHead(0);
        return;
    }

    for (uint32_t i = 1; i <= 3; i++) {
        settings->LinkToAsyncHead(i);
    }
}

// OFFSET: 0x0071FA80/0x1403F5580
void Liquid::CMaterialMagma::InitResources() {
    if (Liquid::GetWaterDetail()) {
        s_procVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ProcMagma");
        s_procPixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "ProcMagma");
    } else {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "Magma");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "Magma");
    }
}

// PROVEN: 68e5887e IDENTICAL/NONE
// OFFSET: 0x0071FAE0/0x1403F5610
void Liquid::CMaterialMagma::DestroyResources() {
    ReleaseShader(s_vertexShader);
    ReleaseShader(s_pixelShader);
    ReleaseShader(s_procVertexShader);
    ReleaseShader(s_procPixelShader);
}

// OFFSET: 0x0071FB60/0x1403F56A0
int32_t Liquid::CMaterialMagma::SetupDefaultTransform(const C3Vector& camPos) {
    SetDefaultTransform(camPos, s_vsConsts, s_psConsts);
    return 1;
}

// OFFSET: 0x0071FC30/0x1403F57A0
int32_t Liquid::CMaterialMagma::SetupLighting(const C3Vector& camPos, CInstance* instance) {
    SetLighting(camPos, instance, s_vsConsts, s_psConsts);
    return 1;
}

// OFFSET: 0x0071FD50/0x1403F5880
Liquid::CMaterialMagma::CMaterialMagma() {
    if (!s_refCount++) {
        CMaterialMagma::InitResources();
    }
}

// PROVEN: 73a7b757 IDENTICAL/NONE
// OFFSET: 0x0071FD80/0x1403F58C0
void Liquid::CMaterialMagma::Reset() {
    CMaterialMagma::DestroyResources();
    CMaterialMagma::InitResources();
}

// OFFSET: 0x0071FD90
void Liquid::CMaterialMagma::RenderShader(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetInstanceTransform(camPos, instance, s_vsConsts, s_psConsts);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(9), NULL, 0, 0);
}

// OFFSET: 0x0071FE90
void Liquid::CMaterialMagma::RenderFixedFunction(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetWorldTransform(camPos, instance);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(9), NULL, 0, 0);
}

// OFFSET: 0x0071FFA0
void Liquid::CMaterialMagma::RenderProcedural(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetInstanceTransform(camPos, instance, s_vsConsts, s_psConsts);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(13), s_procVertexShader, 0, 0);
}

// OFFSET: 0x007200B0/0x1403F5D90
Liquid::CMaterialMagma::~CMaterialMagma() {
    if (s_refCount-- == 1) {
        CMaterialMagma::DestroyResources();
    }
}

// OFFSET: 0x007200F0/0x1403F5DF0
int32_t Liquid::CMaterialMagma::SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection) {
    if (Liquid::GetWaterDetail()) {
        CTexture* noise = TextureCacheGetTexture("proceduralNoiseVolTex");

        if (!noise) {
            return 0;
        }

        CGxTex* noiseTex = TextureGetGxTex(noise);
        TextureRelease(noise);

        if (!noiseTex) {
            return 0;
        }

        CGxTex* textures[3];

        for (uint32_t i = 0; i < 3; i++) {
            textures[i] = settings->GetTexture(i + 1, 0);

            if (!textures[i]) {
                return 0;
            }
        }

        float scale1 = Liquid::WorldCoordScale() * settings->GetFloat(2) * 0.61699998f;
        float scale2 = Liquid::WorldCoordScale() * settings->GetFloat(2) * 0.47400001f;
        float rotation = settings->GetFloat(3) * 0.017453292f;
        float scales[3];
        float rotations[3];

        for (uint32_t i = 0; i < 3; i++) {
            scales[i] = settings->GetFloat(i + 4);
            rotations[i] = settings->GetFloat(i + 7) * 0.017453292f;
        }

        float f10 = settings->GetFloat(10);
        float speed = settings->GetFloat(11);
        uint32_t color = static_cast<uint32_t>(settings->GetInt(2));

        GxRsSet(GxRs_BlendingMode, 0);
        GxRsSet(GxRs_DepthWrite, 1);
        GxUpdateAlphaRef(GxRs_AlphaRef);
        GxRsSet(GxRs_Fog, 1);
        GxRsSet(GxRs_Culling, 0);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, noiseTex);

        for (uint32_t i = 0; i < 3; i++) {
            g_theGxDevicePtr->RsSet(static_cast<EGxRenderState>(GxRs_Texture1 + i), textures[i]);
        }

        C44Matrix matrix0;
        matrix0.Scale(0.050000001f);
        matrix0 *= *Liquid::GetTexScrollMtx(time, C2Vector(speed, 0.0f));
        Liquid::SetMatrixConstant(&s_vsConsts[26], 0, matrix0);

        C44Matrix matrix1 = C44Matrix::RotationAroundZ(rotation);
        matrix1.Scale(scale1);
        matrix1 *= *Liquid::GetTexScrollMtx(time, C2Vector(speed * 0.5f, 0.0f));
        Liquid::SetMatrixConstant(&s_vsConsts[26], 1, matrix1);

        C44Matrix matrix2 = C44Matrix::RotationAroundZ(rotation);
        matrix2.Scale(scale2);
        matrix2 *= *Liquid::GetTexScrollMtx(time, C2Vector(speed * 0.5f, 0.0f));
        Liquid::SetMatrixConstant(&s_vsConsts[26], 2, matrix2);

        for (uint32_t i = 0; i < 3; i++) {
            C44Matrix matrix = C44Matrix::RotationAroundZ(rotations[i]);
            matrix.Scale(scales[i]);
            matrix *= *Liquid::GetTexScrollMtx(time, C2Vector(speed, 0.0f));
            Liquid::SetMatrixConstant(&s_vsConsts[26], i + 3, matrix);
        }

        float phase = static_cast<float>(static_cast<double>(time % 600000) * 0.0000016666667);
        s_vsConsts[50] = C4Vector(f10, 0.0f, 0.0f, phase);
        s_psConsts[17].w = phase;
        s_psConsts[17].x = static_cast<float>((color >> 16) & 0xFF) * 0.0039215689f;
        s_psConsts[17].y = static_cast<float>((color >> 8) & 0xFF) * 0.0039215689f;
        s_psConsts[17].z = static_cast<float>(color & 0xFF) * 0.0039215689f;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 51);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 18);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_procPixelShader->m_permutes[0]);
        this->m_render = CMaterialMagma::RenderProcedural;
        return 1;
    }

    CGxTex* texture = settings->GetTexture(0, 1250);

    if (!texture) {
        return 0;
    }

    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 1);
    float speedX = settings->GetFloat(0);
    float speedY = settings->GetFloat(1);
    C44Matrix matrix = *Liquid::GetTexScrollMtx(time, C2Vector(speedX, speedY));
    matrix.Scale(Liquid::WorldCoordScale());
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);

    if (Liquid::GetShadersEnabled()) {
        Liquid::SetMatrixConstant(&s_vsConsts[26], 0, matrix);
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_vertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[0]);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 51);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 18);
        this->m_render = CMaterialMagma::RenderShader;
    } else {
        GxRsSet(GxRs_Lighting, 0);
        GxXformSet(GxXform_Tex0, matrix);
        GxRsSet(GxRs_TextureShader0, 1);
        this->m_render = CMaterialMagma::RenderFixedFunction;
    }

    return 1;
}

// OFFSET: 0x0071E370/0x1403F3810
void Liquid::CMaterialWater::OnAddToScene() {
    if (Liquid::GetWaterDetail()) {
        Liquid::SetNeedBackBuffer(1);

        if (Liquid::HasDynamicDisplMap()) {
            Liquid::SetNeedDynDisplMap(1);
        }
    }
}

// OFFSET: 0x00720800/0x1403F67D0
int32_t Liquid::CMaterialWater::IsAllLoaded(CMaterialSettings* settings) {
    if (!settings->IsAllLoaded(1)) {
        return 0;
    }

    if (!Liquid::GetWaterDetail()) {
        return settings->IsAllLoaded(0) != 0;
    }

    if (!settings->IsAllLoaded(2)) {
        return 0;
    }

    return settings->IsAllLoaded(3) ? 1 : 0;
}

// PROVEN: 65e0d082 IDENTICAL/NONE
// OFFSET: 0x00720860/0x1403F6840
void Liquid::CMaterialWater::LinkToAsyncHead(CMaterialSettings* settings) {
    settings->LinkToAsyncHead(1);

    if (!Liquid::GetWaterDetail()) {
        settings->LinkToAsyncHead(0);
        return;
    }

    settings->LinkToAsyncHead(2);
    settings->LinkToAsyncHead(3);
}

// OFFSET: 0x007208A0
void Liquid::CMaterialWater::SetFogConstants() {
    SetFog(&s_psConsts[18]);
}

// OFFSET: 0x00720AA0/0x1403F6A70
void Liquid::CMaterialWater::InitResources() {
    if (!Liquid::GetShadersEnabled()) {
        return;
    }

    uint32_t detail = Liquid::GetWaterDetail();

    if (!detail) {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "Water");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "Water");
    } else if (detail == 1) {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ProcWater");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "MediumWater");
        s_pixelShaderBelow = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "MediumWaterBelow");
    } else {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ProcWater");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "ProcWaterAbove");
        s_pixelShaderBelow = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "ProcWaterBelow");
        s_pixelShaderAboveFog = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "ProcWaterAboveFog");
    }
}

// PROVEN: 4d272018 IDENTICAL/NONE
// OFFSET: 0x00720B80/0x1403F6BA0
void Liquid::CMaterialWater::DestroyResources() {
    ReleaseShader(s_vertexShader);
    ReleaseShader(s_pixelShader);
    ReleaseShader(s_pixelShaderBelow);
    ReleaseShader(s_pixelShaderAboveFog);
}

// OFFSET: 0x00720C00/0x1403F6C30
int32_t Liquid::CMaterialWater::SetupDefaultTransform(const C3Vector& camPos) {
    SetDefaultTransform(camPos, s_vsConsts, s_psConsts);
    return 1;
}

// OFFSET: 0x00720CD0/0x1403F6D30
int32_t Liquid::CMaterialWater::SetupLighting(const C3Vector& camPos, CInstance* instance) {
    SetLighting(camPos, instance, s_vsConsts, s_psConsts);
    return 1;
}

// OFFSET: 0x00720E80/0x1403F6E10
Liquid::CMaterialWater::CMaterialWater() {
    if (!s_refCount++) {
        CMaterialWater::InitResources();
    }
}

// PROVEN: 63b580a4 IDENTICAL/NONE
// OFFSET: 0x00720EB0/0x1403F6E50
void Liquid::CMaterialWater::Reset() {
    CMaterialWater::DestroyResources();
    CMaterialWater::InitResources();
}

// OFFSET: 0x00720EC0/0x1403F6FE0
void Liquid::CMaterialWater::SetPixelShaderAboveFog() {
    GxRsSet(GxRs_Culling, 0);
    uint32_t depthMode = DepthCache::GetDepthMode() - 1;
    uint32_t displCount = Liquid::GetDynamicDisplMapCnt();
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShaderAboveFog->m_permutes[3 * depthMode + displCount]);
}

// OFFSET: 0x00720F20
void Liquid::CMaterialWater::SetPixelShaderAbove(uint32_t reflection) {
    GxRsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[AbovePermute(reflection)]);
}

// OFFSET: 0x00720FD0
void Liquid::CMaterialWater::SetPixelShaderBelow(uint32_t reflection) {
    GxRsSet(GxRs_Culling, 0);
    uint32_t depthMode = DepthCache::GetDepthMode() - 1;
    uint32_t displCount = Liquid::GetDynamicDisplMapCnt();
    uint32_t permute = 3 * depthMode + displCount + reflection + 2 * (3 * depthMode + displCount);

    if (Liquid::GetWaterDetail() == 1) {
        permute = 0;
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShaderBelow->m_permutes[permute]);
}

// OFFSET: 0x00721050
void Liquid::CMaterialWater::RenderShader(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetInstanceTransform(camPos, instance, s_vsConsts, s_psConsts);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(7), s_vertexShader, 0, 1);
}

// OFFSET: 0x00721160
void Liquid::CMaterialWater::RenderProcedural(uint32_t pass, const C3Vector& camPos, CInstance* instance) {
    SetInstanceTransform(camPos, instance, s_vsConsts, s_psConsts);
    DrawInstance(instance, static_cast<EGxVertexBufferFormat>(7), s_vertexShader, 4 * Liquid::GetDynamicDisplMapCnt(), 1);
}

// OFFSET: 0x00721280/0x1403F7360
Liquid::CMaterialWater::~CMaterialWater() {
    if (s_refCount-- == 1) {
        CMaterialWater::DestroyResources();
    }
}

// OFFSET: 0x007212C0/0x1403F73C0
int32_t Liquid::CMaterialWater::SetupRender(uint32_t time, ERenderPass pass, const C3Vector& camPos, IEnvironment* environment, CMaterialSettings* settings, uint32_t reflection) {
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Fog, 1);

    CGxTex* baseTex = settings->GetTexture(1, 1250);

    if (!baseTex) {
        return 0;
    }

    float detailScale = Liquid::WorldCoordScale() * settings->GetFloat(2);
    C44Matrix detail;
    detail.b1 = detailScale;

    int32_t depthWrite = !Liquid::GetWaterDetail() && !g_theGxDevicePtr->m_caps.m_maxClipPlanes && !g_theGxDevicePtr->m_caps.unk_02BC;

    if (depthWrite | DepthCache::DepthRangeDumpEnabled()) {
        GxRsSet(GxRs_DepthWrite, 1);
    } else {
        GxRsSet(GxRs_DepthWrite, 0);
    }

    if (Liquid::GetWaterDetail()) {
        CGxTex* surface = settings->GetTexture(2, 0);

        if (!surface) {
            return 0;
        }

        CGxTex* backBuffer = TextureGetGxTex(Liquid::GetBackBuffer());

        if (!backBuffer) {
            return 0;
        }

        CGxTex* reflectionTex = TextureGetGxTex(reflection == 1 ? Liquid::GetBlurredReflectionBuffer() : Liquid::GetBlurredBackBuffer());

        if (!reflectionTex) {
            return 0;
        }

        CGxTex* fakeTex = TextureGetGxTex(reflection == 1 ? Liquid::GetRealReflectionBuffer() : Liquid::GetFakeReflectionBuffer());

        if (!fakeTex) {
            return 0;
        }

        float scales[2];

        for (uint32_t i = 0; i < 2; i++) {
            scales[i] = Liquid::WorldCoordScale() * settings->GetFloat(i + 3);
        }

        float rotation = settings->GetFloat(5) * 0.017453292f;

        for (uint32_t i = 0; i < 2; i++) {
            C44Matrix matrix = C44Matrix::RotationAroundZ(rotation);
            matrix.Scale(scales[i]);
            Liquid::SetMatrixConstant(&s_vsConsts[26], i, matrix);
        }

        float f6 = settings->GetFloat(6);
        float wave = static_cast<float>(static_cast<double>(time % 10000) * 0.000099999997);
        float f7 = settings->GetFloat(7) * 0.34999999f;
        float f8 = settings->GetFloat(8);
        float f12 = settings->GetFloat(12);
        float f11 = settings->GetFloat(11);
        float f9 = settings->GetFloat(9);
        float f10 = settings->GetFloat(10);
        s_psConsts[27] = C4Vector(f12, f11, f9, f10);
        float f13 = settings->GetFloat(13);
        float f14 = settings->GetFloat(14);
        float f15 = settings->GetFloat(15);
        s_psConsts[23] = C4Vector(f13, f14, f15, f6);

        g_theGxDevicePtr->RsSet(GxRs_Texture0, backBuffer);
        g_theGxDevicePtr->RsSet(GxRs_Texture1, fakeTex);
        g_theGxDevicePtr->RsSet(GxRs_Texture2, baseTex);
        g_theGxDevicePtr->RsSet(GxRs_Texture3, reflectionTex);
        g_theGxDevicePtr->RsSet(GxRs_Texture5, surface);

        if (Liquid::GetWaterDetail() > 1) {
            CGxTex* foam = settings->GetTexture(3, 0);

            if (!foam) {
                return 0;
            }

            g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(DepthCache::GetDepthSR()));
            g_theGxDevicePtr->RsSet(GxRs_Texture7, foam);

            if (Liquid::HasDynamicDisplMap()) {
                g_theGxDevicePtr->RsSet(GxRs_Texture4, Liquid::GetDynamicDisplMap(0));

                if (Liquid::GetDynamicDisplMapCnt() > 1) {
                    GxRsSet(GxRs_Texture8, static_cast<void*>(Liquid::GetDynamicDisplMap(1)));
                }
            }
        }

        s_psConsts[22] = ViewSettings::CalculateParams();
        float resolution = 1.0f;

        if (Liquid::s_surfaceGen) {
            resolution = static_cast<float>(Liquid::s_surfaceGen->m_resolution);
        }

        s_vsConsts[50] = C4Vector(wave * 3.1415927f, f7, resolution, f8);
        s_psConsts[28] = C4Vector(wave * 3.1415927f, f7, resolution, 1.0f);
        SetDisplacementConstants(camPos, &s_vsConsts[51], &s_psConsts[29], &s_psConsts[31]);

        Liquid::SetMatrixConstant(&s_vsConsts[26], 2, detail);
        C44Matrix scroll = *Liquid::GetTexScrollMtx(time, C2Vector(0.015f, 0.0f));
        Liquid::SetMatrixConstant(&s_vsConsts[26], 3, scroll);
        s_psConsts[17] = C4Vector(camPos.x, camPos.y, camPos.z, 1.0f);
        Liquid::SetScreenConstants(&s_psConsts[24]);
        CMaterialWater::SetFogConstants();

        if (Liquid::GetWaterDetail() == 1) {
            if (Liquid::CameraSubmerged()) {
                CMaterialWater::SetPixelShaderBelow(reflection);
            } else {
                CMaterialWater::SetPixelShaderAbove(reflection);
            }
        } else if (Liquid::CameraSubmerged()) {
            CMaterialWater::SetPixelShaderBelow(reflection);
        } else if (pass == 2) {
            CMaterialWater::SetPixelShaderAboveFog();
        } else {
            CMaterialWater::SetPixelShaderAbove(reflection);
        }

        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 53);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 32);
        this->m_render = CMaterialWater::RenderProcedural;
        return 1;
    }

    CGxTex* texture = settings->GetTexture(0, settings->GetInt(1));

    if (!texture) {
        return 0;
    }

    float scale = Liquid::WorldCoordScale() * settings->GetFloat(0);
    float rotation = settings->GetFloat(1) * 0.017453292f;
    GxRsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, baseTex);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, texture);
    C44Matrix matrix = C44Matrix::RotationAroundZ(rotation);
    matrix.Scale(scale);

    if (Liquid::GetShadersEnabled()) {
        Liquid::SetMatrixConstant(&s_vsConsts[26], 1, detail);
        Liquid::SetMatrixConstant(&s_vsConsts[26], 0, matrix);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[0]);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(s_vsConsts), 53);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(s_psConsts), 32);
        this->m_render = CMaterialWater::RenderShader;
    } else {
        GxRsSet(GxRs_TextureCoord0, 1);
        GxXformSet(GxXform_Tex0, detail);
        GxRsSet(GxRs_TextureCoord1, 0);
        GxXformSet(GxXform_Tex1, matrix);
        GxRsSet(GxRs_TextureShader1, 1);
        GxRsSet(GxRs_ColorOp1, 2);
        GxRsSet(GxRs_AlphaOp1, 3);
        this->m_render = CMaterialMercury::RenderFixedFunction;
    }

    return 1;
}
