#ifndef ENGINE_SOURCE_FFX_EFFECTWATERWINDOW_INL
#define ENGINE_SOURCE_FFX_EFFECTWATERWINDOW_INL

// INLINED
inline EffectSubmarineWindow::~EffectSubmarineWindow() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        delete this->m_passes[i];
    }
}

#endif
