#ifndef ENGINE_SOURCE_FRAME_CSIMPLEHYPERLINKEDFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEHYPERLINKEDFRAME_INL

// INLINED
inline int32_t CSimpleHyperlinkButton::GetObjectType() {
    if (!CSimpleHyperlinkButton::s_objectType) {
        CSimpleHyperlinkButton::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleHyperlinkButton::s_objectType;
}

// INLINED
inline CSimpleHyperlinkButton::CSimpleHyperlinkButton(CSimpleHyperlinkedFrame* parent) : CSimpleButton(parent) {
    this->SetClickAction(0xFFFFFFFF80000000ULL);
    this->m_linkData = NULL;
    this->m_linkText = NULL;
}

#endif
