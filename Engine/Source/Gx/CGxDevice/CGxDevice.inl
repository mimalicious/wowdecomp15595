#ifndef ENGINE_SOURCE_GX_CGXDEVICE_CGXDEVICE_INL
#define ENGINE_SOURCE_GX_CGXDEVICE_CGXDEVICE_INL

// INLINED
inline void CGxDevice::RsSet(EGxRenderState which, int32_t value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (state.m_value.m_data.p != reinterpret_cast<void*>(static_cast<intptr_t>(value))) {
        this->IRsDirty(which);
        state.m_value.m_data.p = reinterpret_cast<void*>(static_cast<intptr_t>(value));
    }
}

// INLINED
inline void CGxDevice::RsSet(EGxRenderState which, float value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (value != state.m_value.m_data.f[0]) {
        this->IRsDirty(which);
        state.m_value.m_data.f[0] = value;
    }
}

// INLINED
inline void CGxDevice::RsSet(EGxRenderState which, const C3Vector& value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (value.x != state.m_value.m_data.f[0] || value.y != state.m_value.m_data.f[1] || value.z != state.m_value.m_data.f[2]) {
        this->IRsDirty(which);
        state.m_value.m_data.f[0] = value.x;
        state.m_value.m_data.f[1] = value.y;
        state.m_value.m_data.f[2] = value.z;
    }
}

// INLINED
inline CGxMonitorMode::CGxMonitorMode() {
    this->m_size.x = 0;
    this->m_size.y = 0;
}

#endif
