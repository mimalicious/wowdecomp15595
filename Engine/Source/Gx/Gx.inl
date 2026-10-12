#ifndef ENGINE_SOURCE_GX_GX_INL
#define ENGINE_SOURCE_GX_GX_INL

#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"

// INLINED
inline void GxXformScale(EGxXform xf, const C3Vector& scale) {
    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level].Scale(scale);
}

// INLINED
inline void GxXformPush(EGxXform xf, const C44Matrix& matrix) {
    g_theGxDevicePtr->XformPushAndSet(xf, matrix);
}

// PROVEN: 1d547952 IDENTICAL/DIFFERENT
// OFFSET: 0x0040B030/0x14000DA10
inline void GxRsSet(EGxRenderState which, int32_t value) {
    if (!g_theGxDevicePtr->m_context) {
        return;
    }

    CGxAppRenderState& state = g_theGxDevicePtr->m_appRenderStates[which];

    if (state.m_value.m_data.p != reinterpret_cast<void*>(static_cast<intptr_t>(value))) {
        g_theGxDevicePtr->IRsDirty(which);
        state.m_value.m_data.p = reinterpret_cast<void*>(static_cast<intptr_t>(value));
    }
}

#endif
