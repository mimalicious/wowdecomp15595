#include "WorldParam.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Types.h"

// OFFSET: #0x0078DD40
void CWorldParam::SetDefaultParticleDensity() {
    switch (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel]) {
        case GxShProfile_None:
        case GxShProfile_Ps11:
        case GxShProfile_Fp10:
        case GxShProfile_Fp20:
        case GxShProfile_Fp25: {
            CWorldParam::cvar_particleDensity->Set("0.1", true, false, false, true);
            break;
        }

        case GxShProfile_Ps14: {
            CWorldParam::cvar_particleDensity->Set("0.4", true, false, false, true);
            break;
        }

        case GxShProfile_Ps20: {
            CWorldParam::cvar_particleDensity->Set("0.7", true, false, false, true);
            break;
        }

        default: {
            CWorldParam::cvar_particleDensity->Set("1.0", true, false, false, true);
            break;
        }
    }
}

// OFFSET: #0x0078DDF0
void CWorldParam::SetDefaultProjectedTextures() {
    switch (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel]) {
        case GxShProfile_None:
        case GxShProfile_Ps11:
        case GxShProfile_Ps14:
        case GxShProfile_Fp10:
        case GxShProfile_Fp20:
        case GxShProfile_Fp25: {
            CWorldParam::cvar_projectedTextures->Set("0", true, false, false, true);
            break;
        }

        default: {
            CWorldParam::cvar_projectedTextures->Set("1", true, false, false, true);
            break;
        }
    }
}

// OFFSET: #0x0078E950
void CWorldParam::Destroy() {
    RemoveConsoleDeviceDefaultCallback(CWorldParam::SetDefaults);
}

// OFFSET: #0x0078E1A0
void CWorldParam::SetDefaults(OPTION_PAGE page) {
    DefaultSettings* defaults = ConsoleDeviceGetDefaultSettings();

    if (page != OPTION_PAGE_1) {
        return;
    }

    char value[32];

    if (CWorldParam::cvar_farClip) {
        SStrPrintf(value, sizeof(value), "%f", defaults->m_farClip);
        CWorldParam::cvar_farClip->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_shadowLevel) {
        SStrPrintf(value, sizeof(value), "%d", defaults->m_terrainShadowLOD);
        CWorldParam::cvar_shadowLevel->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_maxLights) {
        SStrPrintf(value, sizeof(value), "%d", defaults->m_numLights);
        CWorldParam::cvar_maxLights->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_specular) {
        SStrPrintf(value, sizeof(value), defaults->unk_001A ? "1" : "0");
        CWorldParam::cvar_specular->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_waterLOD) {
        SStrPrintf(value, sizeof(value), "%d", defaults->m_waterLOD);
        CWorldParam::cvar_waterLOD->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_particleDensity) {
        SStrPrintf(value, sizeof(value), "%f", defaults->m_particleDensity);
        CWorldParam::cvar_particleDensity->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_baseMip) {
        SStrPrintf(value, sizeof(value), "%d", defaults->m_baseMipLevel);
        CWorldParam::cvar_baseMip->Set(value, true, false, false, true);
    }

    if (CWorldParam::cvar_groundEffectDensity) {
        SStrPrintf(value, sizeof(value), "%d", defaults->m_detailDoodadDensity);
        CWorldParam::cvar_groundEffectDensity->Set(value, true, false, false, true);
    }

    CWorldParam::cvar_environmentDetail->Default();
    CWorldParam::cvar_groundEffectDist->Set("70.0", true, false, false, true);

    CVar* var = CVar::Lookup("weatherDensity");

    if (var) {
        var->Default();
    }

    var = CVar::Lookup("extShadowQuality");

    if (var) {
        var->Default();
    }

    var = CVar::Lookup("ffxDeath");

    if (var) {
        var->Default();
    }

    var = CVar::Lookup("ffxGlow");

    if (var) {
        var->Default();
    }

    CWorldParam::SetDefaultParticleDensity();
    CWorldParam::SetDefaultProjectedTextures();
}
