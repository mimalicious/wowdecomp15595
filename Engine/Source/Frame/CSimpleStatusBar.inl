#ifndef ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_INL
#define ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_INL

// INLINED
inline int32_t CSimpleStatusBar::GetObjectType() {
    if (!CSimpleStatusBar::s_objectType) {
        CSimpleStatusBar::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleStatusBar::s_objectType;
}

#endif
