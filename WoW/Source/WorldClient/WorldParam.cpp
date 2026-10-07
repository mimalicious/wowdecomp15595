#include "WoW/Source/WorldClient/WorldParam.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D02680
static const CWorldParam::DefaultSetting s_defaultsLow[] = {
    { "farClip", 185 },
    { "baseMip", 1 },
    { "terrainMipLevel", 1 },
    { "componentTextureLevel", 8 },
    { "particleDensity", 10 },
    { "weatherDensity", 0 },
    { "environmentDetail", 50 },
    { "groundEffectDist", 70 },
    { "groundEffectDensity", 16 },
    { "shadowMode", 0 },
    { "shadowTextureSize", 1024 },
    { "textureFilteringMode", 0 },
    { "waterDetail", 0 },
    { "sunshafts", 0 },
    { "projectedTextures", 0 },
    { NULL, 0 },
};

// STATIC: 0x00D02700
static const CWorldParam::DefaultSetting s_defaultsFair[] = {
    { "farClip", 507 },
    { "baseMip", 0 },
    { "terrainMipLevel", 1 },
    { "componentTextureLevel", 8 },
    { "particleDensity", 40 },
    { "weatherDensity", 1 },
    { "environmentDetail", 75 },
    { "groundEffectDist", 110 },
    { "groundEffectDensity", 40 },
    { "shadowMode", 1 },
    { "shadowTextureSize", 1024 },
    { "textureFilteringMode", 1 },
    { "waterDetail", 1 },
    { "reflectionMode", 0 },
    { "sunshafts", 0 },
    { "projectedTextures", 0 },
    { NULL, 0 },
};

// STATIC: 0x00D02788
static const CWorldParam::DefaultSetting s_defaultsGood[] = {
    { "farClip", 727 },
    { "baseMip", 0 },
    { "terrainMipLevel", 0 },
    { "componentTextureLevel", 9 },
    { "particleDensity", 60 },
    { "weatherDensity", 1 },
    { "environmentDetail", 100 },
    { "groundEffectDist", 160 },
    { "groundEffectDensity", 64 },
    { "shadowMode", 1 },
    { "shadowTextureSize", 2048 },
    { "textureFilteringMode", 3 },
    { "waterDetail", 2 },
    { "rippleDetail", 1 },
    { "reflectionMode", 0 },
    { "sunshafts", 1 },
    { "projectedTextures", 1 },
    { NULL, 0 },
};

// STATIC: 0x00D02818
static const CWorldParam::DefaultSetting s_defaultsHigh[] = {
    { "farClip", 1057 },
    { "baseMip", 0 },
    { "terrainMipLevel", 0 },
    { "componentTextureLevel", 9 },
    { "particleDensity", 80 },
    { "weatherDensity", 2 },
    { "environmentDetail", 125 },
    { "groundEffectDist", 200 },
    { "groundEffectDensity", 96 },
    { "shadowMode", 2 },
    { "shadowTextureSize", 2048 },
    { "textureFilteringMode", 4 },
    { "waterDetail", 2 },
    { "rippleDetail", 1 },
    { "reflectionMode", 0 },
    { "sunshafts", 2 },
    { "projectedTextures", 1 },
    { NULL, 0 },
};

// STATIC: 0x00D028A8
static const CWorldParam::DefaultSetting s_defaultsUltra[] = {
    { "farClip", 1250 },
    { "baseMip", 0 },
    { "terrainMipLevel", 0 },
    { "componentTextureLevel", 9 },
    { "particleDensity", 100 },
    { "weatherDensity", 3 },
    { "environmentDetail", 150 },
    { "groundEffectDist", 260 },
    { "groundEffectDensity", 128 },
    { "shadowMode", 3 },
    { "shadowTextureSize", 2048 },
    { "textureFilteringMode", 5 },
    { "waterDetail", 3 },
    { "rippleDetail", 2 },
    { "reflectionMode", 3 },
    { "sunshafts", 2 },
    { "projectedTextures", 1 },
    { NULL, 0 },
};

// STATIC: 0x00D02938
const CWorldParam::DefaultSetting* CWorldParam::s_defaults[5] = { s_defaultsLow, s_defaultsFair, s_defaultsGood, s_defaultsHigh, s_defaultsUltra };

// STATIC: 0x00D0294C
const EGxTexFilter CWorldParam::s_textureFilteringModes[6] = { GxTex_LinearMipNearest, GxTex_LinearMipLinear, GxTex_Anisotropic, GxTex_Anisotropic, GxTex_Anisotropic, GxTex_Anisotropic };

// STATIC: 0x00D02964
const uint32_t CWorldParam::s_textureFilteringAnisotropy[6] = { 1, 1, 2, 4, 8, 16 };

// OFFSET: 0x0076B5C0
void CWorldParam::SetDefaults(OPTION_PAGE page) {
    if (page != OPTION_PAGE_0) {
        return;
    }

    const DefaultSetting* setting = CWorldParam::s_defaults[*reinterpret_cast<uint32_t*>(ConsoleDeviceGetDefaultSettings())];

    for (; setting->m_name; setting++) {
        CVar::Lookup(setting->m_name)->Set(setting->m_value, true, false, false, true);
    }
}

// OFFSET: 0x0076B610
void CWorldParam::Destroy() {
    RemoveConsoleDeviceDefaultCallback(CWorldParam::SetDefaults);
}

// OFFSET: 0x0076C130
uint32_t CWorldParam::ValidateTextureFilteringMode(uint32_t mode) {
    if (mode <= 1) {
        return 0;
    }

    if (mode <= 6) {
        return g_theGxDevicePtr->m_caps.m_maxTexAnisotropy < CWorldParam::s_textureFilteringAnisotropy[mode] ? 8 : 0;
    }

    return 2;
}
