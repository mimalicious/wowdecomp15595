#ifndef ENGINE_SOURCE_FRAME_CSIMPLECOLORSELECT_INL
#define ENGINE_SOURCE_FRAME_CSIMPLECOLORSELECT_INL

// INLINED
inline int32_t CSimpleColorSelect::GetObjectType() {
    if (!CSimpleColorSelect::s_objectType) {
        CSimpleColorSelect::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleColorSelect::s_objectType;
}

#endif
