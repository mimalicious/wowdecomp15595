#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEFRAME_INL

// INLINED
inline int32_t CSimpleFrame::GetObjectType() {
    if (!CSimpleFrame::s_objectType) {
        CSimpleFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleFrame::s_objectType;
}

#endif
