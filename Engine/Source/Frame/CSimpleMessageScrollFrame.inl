#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGESCROLLFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGESCROLLFRAME_INL

// INLINED
inline int32_t CSimpleMessageScrollFrame::GetObjectType() {
    if (!CSimpleMessageScrollFrame::s_objectType) {
        CSimpleMessageScrollFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMessageScrollFrame::s_objectType;
}

// INLINED
inline CSimpleMessageScrollFrameDisplayNode::CSimpleMessageScrollFrameDisplayNode(const CSimpleMessageScrollFrameDisplayNode& source) {
    this->m_line = source.m_line;
    this->m_string = source.m_string;
    this->m_string->m_refcnt++;
}

#endif
