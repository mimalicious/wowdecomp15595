#include "WoW/Source/WorldClient/WorldParam.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D02680/0x140BB2310
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

// STATIC: 0x00D02700/0x140BB2410
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

// STATIC: 0x00D02788/0x140BB2520
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

// STATIC: 0x00D02818/0x140BB2640
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

// STATIC: 0x00D028A8/0x140BB2760
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

// STATIC: 0x00D02938/0x140BB2880
const CWorldParam::DefaultSetting* CWorldParam::s_defaults[5] = { s_defaultsLow, s_defaultsFair, s_defaultsGood, s_defaultsHigh, s_defaultsUltra };

// STATIC: 0x00D0294C/0x140BB28A8
const EGxTexFilter CWorldParam::s_textureFilteringModes[6] = { GxTex_LinearMipNearest, GxTex_LinearMipLinear, GxTex_Anisotropic, GxTex_Anisotropic, GxTex_Anisotropic, GxTex_Anisotropic };

// STATIC: 0x00D02964/0x140BB28C0
const uint32_t CWorldParam::s_textureFilteringAnisotropy[6] = { 1, 1, 2, 4, 8, 16 };

// PROVEN: cd3eaae2 IDENTICAL/IDENTICAL
// OFFSET: 0x0076B5A0/0x140453660
const CWorldParam::DefaultSetting* CWorldParam::GetDefaultSettings(uint32_t level) {
    return CWorldParam::s_defaults[level];
}

// PROVEN: e09f161b IDENTICAL/IDENTICAL
// OFFSET: 0x0076B5B0/0x140453670
const CWorldParam::DefaultSetting* CWorldParam::GetCurrentDefaultSettings() {
    return CWorldParam::s_defaults[ConsoleDeviceGetDefaultSettings()->m_graphicsQuality];
}

// PROVEN: 69f993cf IDENTICAL/IDENTICAL
// OFFSET: 0x0076B5C0/0x140453690
void CWorldParam::SetDefaults(OPTION_PAGE page) {
    if (page != OPTION_PAGE_0) {
        return;
    }

    const DefaultSetting* setting = CWorldParam::s_defaults[ConsoleDeviceGetDefaultSettings()->m_graphicsQuality];

    for (; setting->m_name; setting++) {
        CVar* var = CVar::Lookup(setting->m_name);
        var->Set(setting->m_value, true, false, false, true);
    }
}

// PROVEN: cb6b2cea IDENTICAL/UNPROVEN
// OFFSET: 0x0076B610/0x140453700
void CWorldParam::Destroy() {
    RemoveConsoleDeviceDefaultCallback(CWorldParam::SetDefaults);
}

// PROVEN: 1a40b545 IDENTICAL/IDENTICAL
// OFFSET: 0x0076C130/0x140454240
uint32_t CWorldParam::ValidateTextureFilteringMode(uint32_t mode) {
    if (mode <= 1) {
        return 0;
    }

    if (mode > 6) {
        return 2;
    }

    return CWorldParam::s_textureFilteringAnisotropy[mode] > g_theGxDevicePtr->m_caps.m_maxTexAnisotropy ? 8 : 0;
}
