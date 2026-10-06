#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_INL

// INLINED
inline int32_t CSimpleMessageFrame::GetObjectType() {
    if (!CSimpleMessageFrame::s_objectType) {
        CSimpleMessageFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMessageFrame::s_objectType;
}

#endif
