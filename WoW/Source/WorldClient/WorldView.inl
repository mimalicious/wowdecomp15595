#ifndef WOW_SOURCE_WORLDCLIENT_WORLDVIEW_INL
#define WOW_SOURCE_WORLDCLIENT_WORLDVIEW_INL

// INLINED
inline CPortalView::CPortalView() {
    this->m_rect.minY = 3.402823466e38f;
    this->m_rect.minX = 3.402823466e38f;
    this->m_rect.maxY = -3.402823466e38f;
    this->m_rect.maxX = -3.402823466e38f;
    this->m_maxViewDepth = -1.0f;
    this->m_vertStart = 0;
    this->m_vertCount = 0;
}

#endif
