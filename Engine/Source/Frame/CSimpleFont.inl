#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFONT_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEFONT_INL

// INLINED
inline int32_t CSimpleFont::GetObjectType() {
    if (!CSimpleFont::s_objectType) {
        CSimpleFont::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleFont::s_objectType;
}

#endif
