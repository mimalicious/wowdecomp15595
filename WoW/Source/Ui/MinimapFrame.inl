#ifndef WOW_SOURCE_UI_MINIMAPFRAME_INL
#define WOW_SOURCE_UI_MINIMAPFRAME_INL

// INLINED
inline MinimapBlobRing::MinimapBlobRing() {
    this->m_start = 0.0f;
    this->m_end = 0.0f;
    this->m_reverse = 1;
}

// INLINED
inline MINIMAPBLOB::MINIMAPBLOB() {
    this->m_insideTexture = NULL;
    this->m_outsideTexture = NULL;
    this->m_ringTexture = NULL;
    this->m_insideAlpha = 0xFF;
    this->m_outsideAlpha = 0xFF;
    this->m_ringAlpha = 0xFF;
}

// INLINED
inline int32_t CGMinimapFrame::GetObjectType() {
    if (!CGMinimapFrame::s_objectType) {
        CGMinimapFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGMinimapFrame::s_objectType;
}

#endif
