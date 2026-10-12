#include "Engine/Source/FFX/EffectWaterWindow.h"
#include "Engine/Source/FFX/PassWaterWindow.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0089D9C0
EffectWaterWindow::EffectWaterWindow(uint32_t type) {
    FFX::Tex* inputs[] = { &FFX::s_full[0] };
    FFX::Pass* pass = new (__FILE__, __LINE__) PassWaterWindow(type, 0, inputs, 1, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);
}

// OFFSET: [0x00B49CA0/0x1405DE230, 0x00B49D60/0x1405DE380]
EffectWaterWindow::~EffectWaterWindow() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        delete this->m_passes[i];
    }
}

// OFFSET: 0x0089DA30
EffectSubmarineWindow::EffectSubmarineWindow(uint32_t type) {
    FFX::Tex* inputs[] = { &FFX::s_full[0] };
    FFX::Pass* pass = new (__FILE__, __LINE__) PassWaterWindow(type, 1, inputs, 1, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);
}

// INLINED
EffectSubmarineWindow::~EffectSubmarineWindow() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        delete this->m_passes[i];
    }
}
