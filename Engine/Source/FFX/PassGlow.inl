#ifndef ENGINE_SOURCE_FFX_PASSGLOW_INL
#define ENGINE_SOURCE_FFX_PASSGLOW_INL

// INLINED
inline PassGlowBase::PassGlowBase(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_color = CImVector(0, 0, 0, 0);
}

#endif
