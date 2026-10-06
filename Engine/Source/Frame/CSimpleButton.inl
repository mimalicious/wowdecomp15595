#ifndef ENGINE_SOURCE_FRAME_CSIMPLEBUTTON_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEBUTTON_INL

// INLINED
inline int32_t CSimpleButton::GetObjectType() {
    if (!CSimpleButton::s_objectType) {
        CSimpleButton::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleButton::s_objectType;
}

#endif
