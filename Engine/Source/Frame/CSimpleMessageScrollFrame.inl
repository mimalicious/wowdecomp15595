#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGESCROLLFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGESCROLLFRAME_INL

// INLINED
inline int32_t CSimpleMessageScrollFrame::GetObjectType() {
    if (!CSimpleMessageScrollFrame::s_objectType) {
        CSimpleMessageScrollFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMessageScrollFrame::s_objectType;
}

#endif
