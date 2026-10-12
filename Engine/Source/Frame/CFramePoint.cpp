#include <cstddef>
#include <cstdint>
#include <limits>
#include <typeinfo>
#include "Engine/Source/Frame/CFramePoint.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"

// STATIC: 0x00CA3B70/0x140B410A0
float CFramePoint::UNDEFINED = std::numeric_limits<float>::infinity();

// STATIC: 0x00DD3CD0
CDataAllocator CFramePoint::s_framePointHeap(sizeof(CFramePoint), 0x1000);

// INLINED
void* CFramePoint::operator new(size_t bytes) {
    return CFramePoint::s_framePointHeap.GetData(0, typeid(CFramePoint).raw_name(), SLOG_OBJECT);
}

// INLINED
void CFramePoint::operator delete(void* ptr) {
    CFramePoint::s_framePointHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x0052EAD0/0x14017F9E0
void CFramePoint::SetRelative(CLayoutFrame* relative, FRAMEPOINT framePoint, float x, float y) {
    this->m_x = x;
    this->m_framePoint = framePoint;
    this->m_y = y;
    this->m_relative = relative;

    uint32_t busy = this->m_flags & 2;
    this->m_flags = 0;

    if (busy) {
        this->m_flags |= 6;
    }
}

// OFFSET: 0x0052EB10/0x14017FA10
void CFramePoint::MarkUnused() {
    this->m_framePoint = FRAMEPOINT_NUMPOINTS;
    this->m_x = 0.0f;
    this->m_y = 0.0f;
    this->m_relative = NULL;

    uint32_t busy = this->m_flags & 2;
    this->m_flags = 8;

    if (busy) {
        this->m_flags |= 6;
    }
}

// OFFSET: 0x0052EB50/0x14017FA40
int32_t CFramePoint::GetRelativeRect(CRect& rect) {
    uint32_t busy = this->m_flags & 2;
    int32_t result = 0;

    if (!busy) {
        this->m_flags |= 2;
    }

    this->m_flags &= ~4u;

    if (!this->m_relative->IsResizePending() || (this->m_relative->Resize(1), !(this->m_flags & 4))) {
        if (!(this->m_relative->m_layoutFlags & 0x2) || (this->m_relative->SetDeferredResize(0), !(this->m_flags & 4))) {
            if (this->m_relative->GetRect(&rect)) {
                if (this->m_relative->IsAttachmentOrigin()) {
                    float offsetX = -rect.minX;
                    float offsetY = -rect.minY;
                    rect.minY += offsetY;
                    rect.minX += offsetX;
                    rect.maxY += offsetY;
                    rect.maxX += offsetX;
                }

                result = 1;
            }
        }
    }

    if (!busy) {
        this->m_flags &= ~2u;
    }

    return result;
}

// OFFSET: 0x0052EC40
float CFramePoint::X(float scale) {
    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);

    if (!this->GetRelativeRect(rect)) {
        return CFramePoint::UNDEFINED;
    }

    switch (this->m_framePoint) {
        case FRAMEPOINT_TOPLEFT:
        case FRAMEPOINT_LEFT:
        case FRAMEPOINT_BOTTOMLEFT:
            return this->m_x * scale + rect.minX;

        case FRAMEPOINT_TOP:
        case FRAMEPOINT_CENTER:
        case FRAMEPOINT_BOTTOM:
            return (rect.maxX + rect.minX) * 0.5f + this->m_x * scale;

        case FRAMEPOINT_TOPRIGHT:
        case FRAMEPOINT_RIGHT:
        case FRAMEPOINT_BOTTOMRIGHT:
            return this->m_x * scale + rect.maxX;

        default:
            return CFramePoint::UNDEFINED;
    }
}

// OFFSET: 0x0052ED30/0x14017FB50
float CFramePoint::Y(float scale) {
    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);

    if (!this->GetRelativeRect(rect)) {
        return CFramePoint::UNDEFINED;
    }

    switch (this->m_framePoint) {
        case FRAMEPOINT_TOPLEFT:
        case FRAMEPOINT_TOP:
        case FRAMEPOINT_TOPRIGHT:
            return this->m_y * scale + rect.maxY;

        case FRAMEPOINT_LEFT:
        case FRAMEPOINT_CENTER:
        case FRAMEPOINT_RIGHT:
            return (rect.minY + rect.maxY) * 0.5f + this->m_y * scale;

        case FRAMEPOINT_BOTTOMLEFT:
        case FRAMEPOINT_BOTTOM:
        case FRAMEPOINT_BOTTOMRIGHT:
            return this->m_y * scale + rect.minY;

        default:
            return CFramePoint::UNDEFINED;
    }
}

// OFFSET: 0x0052EE20/0x14017FD70
CFramePoint::CFramePoint(CLayoutFrame* relative, FRAMEPOINT framePoint, float x, float y) {
    this->m_x = x;
    this->m_framePoint = framePoint;
    this->m_y = y;
    this->m_relative = relative;

    uint32_t busy = this->m_flags & 2;
    this->m_flags = 0;

    if (busy) {
        this->m_flags |= 6;
    }
}
