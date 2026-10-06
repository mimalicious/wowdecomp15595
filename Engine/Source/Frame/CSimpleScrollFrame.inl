#ifndef ENGINE_SOURCE_FRAME_CSIMPLESCROLLFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLESCROLLFRAME_INL

// INLINED
inline int32_t CSimpleScrollFrame::GetObjectType() {
    if (!CSimpleScrollFrame::s_objectType) {
        CSimpleScrollFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleScrollFrame::s_objectType;
}

#endif
