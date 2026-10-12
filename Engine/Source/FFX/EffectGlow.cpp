#include "Engine/Source/FFX/EffectGlow.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FFX/PassBox4.h"
#include "Engine/Source/FFX/PassGauss4.h"
#include "Engine/Source/FFX/PassGlow.h"
#include "Engine/Source/FFX/PassGlowWave.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"

CVar* EffectGlow::s_cvarGlow;

// PROVEN: 9a7d537c IDENTICAL/IDENTICAL
// OFFSET: 0x0089DAE0/0x1405DE440
bool EffectGlow::Valid() {
    bool valid = this->m_cvar->m_intValue != 0;

    if (this->m_mode) {
        for (uint32_t i = 0; i < this->m_altPasses.Count(); i++) {
            valid &= this->m_altPasses[i]->Valid();
        }
    } else {
        for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
            valid &= this->m_passes[i]->Valid();
        }
    }

    return valid;
}

// PROVEN: 6705acc2 IDENTICAL/IDENTICAL
// OFFSET: 0x0089DB40/0x1405DE4E0
void EffectGlow::Execute() {
    if (this->m_mode) {
        for (uint32_t i = 0; i < this->m_altPasses.Count(); i++) {
            this->m_altPasses[i]->Execute();
        }
    } else {
        for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
            this->m_passes[i]->Execute();
        }
    }
}

// OFFSET: 0x0089DB90/0x1405DE560
void EffectGlow::SetParameters(int32_t count, const int32_t* params) {
    this->m_mode = params[0];

    static_cast<PassGlowBase*>(this->m_altPasses[2])->m_color = CImVector(params[2], params[2], params[2], params[1]);
    static_cast<PassGlowBase*>(this->m_passes[2])->m_color = CImVector(params[2], params[2], params[2], params[1]);
}

// INLINED
void EffectGlow::InitCVars() {
    EffectGlow::s_cvarGlow = CVar::Register("ffxGlow", "full screen glow effect", 1, FFX::s_defaultEnabled ? "1" : "0", FFX::CVarCallback, 1, false, NULL, false);
}

// OFFSET: 0x0089DC30/0x1405DE650
EffectGlow::EffectGlow() {
    this->m_cvar = EffectGlow::s_cvarGlow;

    FFX::Tex* fullInputs[] = { &FFX::s_full[0] };
    FFX::Tex* quartInputs[] = { &FFX::s_quart[0] };
    FFX::Tex* glowInputs[] = { &FFX::s_full[0], &FFX::s_quart[1] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassBox4(fullInputs, 1, &FFX::s_quart[0], GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassGauss4(quartInputs, 1, &FFX::s_quart[1], GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassGlow(glowInputs, 2, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassBox4(fullInputs, 1, &FFX::s_quart[0], GxBlend_Opaque);
    *this->m_altPasses.New() = pass;

    pass = new (__FILE__, __LINE__) PassGauss4(quartInputs, 1, &FFX::s_quart[1], GxBlend_Opaque);
    *this->m_altPasses.New() = pass;

    if (FFX::s_useRECT && g_theGxDevicePtr->m_caps.m_texNonPow2) {
        pass = new (__FILE__, __LINE__) PassGlow(glowInputs, 2, &FFX::s_screen, GxBlend_Opaque);
        *this->m_altPasses.New() = pass;
    } else {
        pass = new (__FILE__, __LINE__) PassGlowWave(glowInputs, 2, &FFX::s_screen, GxBlend_Opaque);
        *this->m_altPasses.New() = pass;
    }

    this->m_mode = 0;
}

// OFFSET: 0x0089DF20
EffectGlow::~EffectGlow() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        delete this->m_passes[i];
    }

    for (uint32_t i = 0; i < this->m_altPasses.Count(); i++) {
        delete this->m_altPasses[i];
    }
}
