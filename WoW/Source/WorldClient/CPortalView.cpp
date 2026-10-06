#include "CPortalView.h"
#include "Engine/Source/Tempest/CRect.h"

// OFFSET: #0x00790570
CPortalView::CPortalView(const CRect& rect, float maxViewDepth) {
    this->m_rect = rect;
    this->m_maxViewDepth = maxViewDepth;
    this->m_verts = NULL;
    this->m_vertCount = 0;
}

// OFFSET: #0x007905B0
void CPortalView::Merge(const CPortalView& other) {
    this->m_rect = this->m_rect | other.m_rect;
    this->m_maxViewDepth = other.m_maxViewDepth >= this->m_maxViewDepth ? other.m_maxViewDepth : this->m_maxViewDepth;
    this->m_vertCount = 0;
}
