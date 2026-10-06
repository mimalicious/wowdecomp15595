#ifndef ENGINE_SOURCE_FRAME_CSIMPLERENDER_INL
#define ENGINE_SOURCE_FRAME_CSIMPLERENDER_INL

// INLINED
inline int32_t CSimpleFontString::GetObjectType() {
    if (!CSimpleFontString::s_objectType) {
        CSimpleFontString::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleFontString::s_objectType;
}

#endif
