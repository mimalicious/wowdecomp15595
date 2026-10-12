#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFONTSTRINGATTRIBUTES_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEFONTSTRINGATTRIBUTES_INL

// INLINED
inline CSimpleFontStringAttributes::CSimpleFontStringAttributes() {
    this->m_fontHeight = 0.0f;
    this->m_spacing = 0.0f;
    this->m_fontFlags = 0;
    this->m_styleFlags = 0x212;
    this->m_color.Set(1.0f, 1.0f, 1.0f, 1.0f);
    this->m_flags = 0;
}

#endif
