#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <typeinfo>
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CFramePoint.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CMath.h"

static const FRAMEPOINT s_centerXPoints[3] = { FRAMEPOINT_TOP, FRAMEPOINT_CENTER, FRAMEPOINT_BOTTOM };

static const FRAMEPOINT s_centerYPoints[3] = { FRAMEPOINT_LEFT, FRAMEPOINT_CENTER, FRAMEPOINT_RIGHT };

static const FRAMEPOINT s_rightPoints[3] = { FRAMEPOINT_TOPRIGHT, FRAMEPOINT_RIGHT, FRAMEPOINT_BOTTOMRIGHT };

static const FRAMEPOINT s_bottomPoints[3] = { FRAMEPOINT_BOTTOMLEFT, FRAMEPOINT_BOTTOM, FRAMEPOINT_BOTTOMRIGHT };

static const FRAMEPOINT s_leftPoints[3] = { FRAMEPOINT_TOPLEFT, FRAMEPOINT_LEFT, FRAMEPOINT_BOTTOMLEFT };

static const FRAMEPOINT s_topPoints[3] = { FRAMEPOINT_TOPLEFT, FRAMEPOINT_TOP, FRAMEPOINT_TOPRIGHT };

// STATIC: 0x00CA2F7C
static TSExplicitList<CLayoutFrame, offsetof(CLayoutFrame, m_resizeLink)> s_resizePendingList;

// STATIC: 0x00DD3938
CDataAllocator CAnimationData::s_AnimationData(sizeof(CAnimationData), 5);

// INLINED
void* CAnimationData::operator new(size_t bytes) {
    return CAnimationData::s_AnimationData.GetData(0, typeid(CAnimationData).raw_name(), SLOG_OBJECT);
}

// INLINED
void CAnimationData::operator delete(void* ptr) {
    CAnimationData::s_AnimationData.PutData(ptr, NULL, 0);
}

// INLINED
CAnimationData::CAnimationData() {
    this->ResetAlphaData();
    this->ResetPositionData();
}

// INLINED
void CAnimationData::ResetAlphaData() {
    this->m_hasAlpha = 0;
    this->m_alpha = 0xFF;
}

// INLINED
void CAnimationData::ResetPositionData() {
    this->m_hasPosition = 0;

    for (int32_t i = 0; i < 4; i++) {
        this->m_position[i].x = 0.0f;
        this->m_position[i].y = 0.0f;
        this->m_position[i].z = 0.0f;
    }
}

// PROVEN: ddb8da15 IDENTICAL/NONE
// OFFSET: 0x005072C0
void CAnimationData::StorePosition(const C3Vector* position) {
    this->m_hasPosition = 1;

    for (int32_t i = 0; i < 4; i++) {
        this->m_position[i] = position[i];
    }
}

// INLINED
static float ComputeRightBoundary(float center, float opposite, float size) {
    if (opposite != CFramePoint::UNDEFINED && size != 0.0f) {
        return size + opposite;
    }

    if (center != CFramePoint::UNDEFINED) {
        if (size != 0.0f) {
            return center + size * 0.5f;
        }

        if (opposite != CFramePoint::UNDEFINED) {
            return center + center - opposite;
        }
    }

    return CFramePoint::UNDEFINED;
}

// INLINED
static float ComputeCenterPosition(float first, float second, float size) {
    if (first != CFramePoint::UNDEFINED) {
        if (second != CFramePoint::UNDEFINED) {
            return (second + first) * 0.5f;
        }

        if (size != 0.0f) {
            return first + size * 0.5f;
        }
    }

    if (second != CFramePoint::UNDEFINED && size != 0.0f) {
        return second - size * 0.5f;
    }

    return CFramePoint::UNDEFINED;
}

// OFFSET: 0x008C3E50
void GetRectFramePoints(const CRect& rect, C2Vector* points) {
    points[FRAMEPOINT_TOPLEFT].x = rect.minX;
    points[FRAMEPOINT_TOPLEFT].y = rect.maxY;
    points[FRAMEPOINT_TOP].x = (rect.maxX - rect.minX) * 0.5f + rect.minX;
    points[FRAMEPOINT_TOP].y = rect.maxY;
    points[FRAMEPOINT_TOPRIGHT].x = rect.maxX;
    points[FRAMEPOINT_TOPRIGHT].y = rect.maxY;
    points[FRAMEPOINT_LEFT].x = rect.minX;
    points[FRAMEPOINT_LEFT].y = (rect.maxY - rect.minY) * 0.5f + rect.minY;
    points[FRAMEPOINT_CENTER].x = (rect.maxX - rect.minX) * 0.5f + rect.minX;
    points[FRAMEPOINT_CENTER].y = (rect.maxY - rect.minY) * 0.5f + rect.minY;
    points[FRAMEPOINT_RIGHT].x = rect.maxX;
    points[FRAMEPOINT_RIGHT].y = (rect.maxY - rect.minY) * 0.5f + rect.minY;
    points[FRAMEPOINT_BOTTOMLEFT].x = rect.minX;
    points[FRAMEPOINT_BOTTOMLEFT].y = rect.minY;
    points[FRAMEPOINT_BOTTOM].x = 0.5f * (rect.maxX - rect.minX) + rect.minX;
    points[FRAMEPOINT_BOTTOM].y = rect.minY;
    points[FRAMEPOINT_BOTTOMRIGHT].x = rect.maxX;
    points[FRAMEPOINT_BOTTOMRIGHT].y = rect.minY;
}

// OFFSET: 0x004F8770
int32_t CLayoutFrame::HasAnyFramepoints() {
    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        if (this->m_framePoints[i] && !(this->m_framePoints[i]->m_flags & 8)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005071E0
bool Anchor::GetAnchor(FRAMEPOINT& point, FRAMEPOINT& relativePoint, float& offsetX, float& offsetY, CStatus* status) {
    if (!(this->m_anchorFlags & 2)) {
        this->m_relativePoint = this->m_point;
    }

    point = static_cast<FRAMEPOINT>(this->m_point);
    relativePoint = static_cast<FRAMEPOINT>(this->m_relativePoint);
    offsetY = 0.0f;
    offsetX = 0.0f;

    linkObject* offset = this->GetChild(XML_OFFSET);

    if (!offset) {
        this->Load(offsetX, offsetY, status);
        return true;
    }

    return static_cast<Dimension*>(offset)->Load(offsetX, offsetY, status);
}

// PROVEN: 8abab33f IDENTICAL/IDENTICAL
// OFFSET: 0x00507260/0x14014E7C0
uint32_t CLayoutFrame::IsResizePending() {
    return this->m_layoutFlags & 4;
}

// PROVEN: ad50406e IDENTICAL/IDENTICAL
// OFFSET: 0x00507270/0x14014E7D0
float CLayoutFrame::GetHeight() {
    return this->m_height;
}

// PROVEN: 78ad4b4f IDENTICAL/IDENTICAL
// OFFSET: 0x00507280/0x14014E7E0
float CLayoutFrame::GetWidth() {
    return this->m_width;
}

// PROVEN: bc6d0e03 IDENTICAL/IDENTICAL
// OFFSET: 0x00507290/0x14014E7F0
void CLayoutFrame::GetClampRectInsets(float& left, float& right, float& top, float& bottom) {
    bottom = 0.0f;
    top = 0.0f;
    right = 0.0f;
    left = 0.0f;
}

// PROVEN: fec1b118 IDENTICAL/NONE
// OFFSET: 0x00507320
CFramePoint* CLayoutFrame::GetFramePoint(FRAMEPOINT point) {
    CFramePoint* framePoint = this->m_framePoints[point];

    if (!framePoint || (framePoint->m_flags & 8)) {
        return NULL;
    }

    return framePoint;
}

// OFFSET: 0x00507340
int32_t CLayoutFrame::GetFramePointX(const FRAMEPOINT* points, int32_t count, float& x) {
    for (int32_t i = 0; i < count; i++) {
        CFramePoint* framePoint = this->m_framePoints[points[i]];

        if (framePoint && !(framePoint->m_flags & 8)) {
            float value = framePoint->X(this->m_layoutScale);
            x = value;

            if (framePoint->m_flags & 4) {
                x = CFramePoint::UNDEFINED;
                return 0;
            }

            if (CFramePoint::UNDEFINED != value) {
                return 1;
            }
        }
    }

    x = CFramePoint::UNDEFINED;
    return 1;
}

// OFFSET: 0x005073E0
int32_t CLayoutFrame::GetFramePointY(const FRAMEPOINT* points, int32_t count, float& y) {
    for (int32_t i = 0; i < count; i++) {
        CFramePoint* framePoint = this->m_framePoints[points[i]];

        if (framePoint && !(framePoint->m_flags & 8)) {
            float value = framePoint->Y(this->m_layoutScale);
            y = value;

            if (framePoint->m_flags & 4) {
                y = CFramePoint::UNDEFINED;
                return 0;
            }

            if (CFramePoint::UNDEFINED != value) {
                return 1;
            }
        }
    }

    y = CFramePoint::UNDEFINED;
    return 1;
}

// OFFSET: 0x00507480
void CLayoutFrame::GetHorizontalFramePoint(const FRAMEPOINT* points, int32_t count, float& x) {
    for (int32_t i = 0; i < 10; i++) {
        if (this->GetFramePointX(points, count, x)) {
            break;
        }
    }
}

// OFFSET: 0x005074C0
void CLayoutFrame::GetVerticalFramePoint(const FRAMEPOINT* points, int32_t count, float& y) {
    for (int32_t i = 0; i < 10; i++) {
        if (this->GetFramePointY(points, count, y)) {
            break;
        }
    }
}

// OFFSET: 0x00507500
float CLayoutFrame::CenterX() {
    if (this->m_guard & 0x10) {
        return CFramePoint::UNDEFINED;
    }

    this->m_guard |= 0x10;

    float x = CFramePoint::UNDEFINED;
    this->GetHorizontalFramePoint(s_centerXPoints, 3, x);

    if (x == CFramePoint::UNDEFINED) {
        float size = this->GetWidth() * this->m_layoutScale;
        x = ComputeCenterPosition(this->Left(), this->Right(), size);
    }

    this->m_guard &= ~0x10u;

    return x;
}

// OFFSET: 0x00507630
float CLayoutFrame::CenterY() {
    if (this->m_guard & 0x20) {
        return CFramePoint::UNDEFINED;
    }

    this->m_guard |= 0x20;

    float y = CFramePoint::UNDEFINED;
    this->GetVerticalFramePoint(s_centerYPoints, 3, y);

    if (y == CFramePoint::UNDEFINED) {
        float size = this->GetHeight() * this->m_layoutScale;
        y = ComputeCenterPosition(this->Bottom(), this->Top(), size);
    }

    this->m_guard &= ~0x20u;

    return y;
}

// OFFSET: 0x00507760
float CLayoutFrame::Right() {
    if (this->m_guard & 4) {
        return CFramePoint::UNDEFINED;
    }

    this->m_guard |= 4;

    float x = CFramePoint::UNDEFINED;
    this->GetHorizontalFramePoint(s_rightPoints, 3, x);

    if (x == CFramePoint::UNDEFINED) {
        float size = this->GetWidth() * this->m_layoutScale;
        x = ComputeRightBoundary(this->CenterX(), this->Left(), size);
    }

    this->m_guard &= ~4u;

    return x;
}

// OFFSET: 0x00507880
float CLayoutFrame::Bottom() {
    if (this->m_guard & 8) {
        return CFramePoint::UNDEFINED;
    }

    this->m_guard |= 8;

    float y = CFramePoint::UNDEFINED;
    this->GetVerticalFramePoint(s_bottomPoints, 3, y);

    if (y == CFramePoint::UNDEFINED) {
        float size = -(this->GetHeight() * this->m_layoutScale);
        y = ComputeRightBoundary(this->CenterY(), this->Top(), size);
    }

    this->m_guard &= ~8u;

    return y;
}

// PROVEN: 89aecdeb IDENTICAL/DIFFERENT
// OFFSET: 0x005079A0/0x14014F7E0
void CLayoutFrame::ProtectFramepoints(uint32_t flag) {
    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        CFramePoint* framePoint = this->m_framePoints[i];

        if (framePoint && !(framePoint->m_flags & 8) && framePoint->m_relative) {
            framePoint->m_relative->SetProtectFlag(flag);
        }
    }
}

// OFFSET: 0x005079E0/0x14014EE80
int32_t CLayoutFrame::IsResizeDependency(CLayoutFrame* frame) {
    CLayoutFrame* checked = NULL;

    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        CFramePoint* framePoint = this->m_framePoints[i];

        if (!framePoint || (framePoint->m_flags & 8)) {
            continue;
        }

        CLayoutFrame* relative = framePoint->m_relative;

        if (!relative || relative == checked) {
            continue;
        }

        if (relative == frame || relative->IsResizeDependency(frame)) {
            return 1;
        }

        checked = relative;
    }

    return 0;
}

// PROVEN: e2c20946 IDENTICAL/IDENTICAL
// OFFSET: 0x00507AF0/0x14014F020
int32_t CLayoutFrame::GetRect(CRect* rect) {
    if (!(this->m_layoutFlags & 0x1)) {
        return 0;
    }

    *rect = this->m_rect;

    return 1;
}

// OFFSET: 0x00507B30/0x14014F060
int32_t CLayoutFrame::PtInFrameRect(const C2Vector& point) {
    return (this->m_layoutFlags & 0x1) && this->m_rect.IsPointInside(point);
}

// OFFSET: 0x00507B80
float CLayoutFrame::Left() {
    if (this->m_guard & 1) {
        return CFramePoint::UNDEFINED;
    }

    this->m_guard |= 1;

    float x = CFramePoint::UNDEFINED;
    this->GetHorizontalFramePoint(s_leftPoints, 3, x);

    if (x == CFramePoint::UNDEFINED) {
        float size = -(this->GetWidth() * this->m_layoutScale);
        x = ComputeRightBoundary(this->CenterX(), this->Right(), size);
    }

    this->m_guard &= ~1u;

    return x;
}

// OFFSET: 0x00507CA0
float CLayoutFrame::Top() {
    if (this->m_guard & 2) {
        return CFramePoint::UNDEFINED;
    }

    this->m_guard |= 2;

    float y = CFramePoint::UNDEFINED;
    this->GetVerticalFramePoint(s_topPoints, 3, y);

    if (y == CFramePoint::UNDEFINED) {
        float size = this->GetHeight() * this->m_layoutScale;
        y = ComputeRightBoundary(this->CenterY(), this->Bottom(), size);
    }

    this->m_guard &= ~2u;

    return y;
}

// OFFSET: 0x00507DC0
int32_t CLayoutFrame::CalculateRect(CRect* rect) {
    rect->minX = this->Left();

    if (CFramePoint::UNDEFINED == rect->minX) {
        return 0;
    }

    rect->minY = this->Bottom();

    if (CFramePoint::UNDEFINED == rect->minY) {
        return 0;
    }

    rect->maxX = this->Right();

    if (CFramePoint::UNDEFINED == rect->maxX) {
        return 0;
    }

    rect->maxY = this->Top();

    if (CFramePoint::UNDEFINED == rect->maxY) {
        return 0;
    }

    if (!(this->m_layoutFlags & 0x10)) {
        return 1;
    }

    float left = 0.0f;
    float right = 0.0f;
    float top = 0.0f;
    float bottom = 0.0f;
    this->GetClampRectInsets(left, right, top, bottom);

    float minX = -(this->m_layoutScale * left);
    float maxX = NDCToDDCWidth(1.0f) - this->m_layoutScale * right;
    float minY = -(this->m_layoutScale * bottom);
    float maxY = NDCToDDCHeight(1.0f) - this->m_layoutScale * top;

    if (minX > rect->minX) {
        rect->maxX = rect->maxX - (rect->minX - minX);
        rect->minX = minX;
    }

    if (minY > rect->minY) {
        rect->maxY = rect->maxY - (rect->minY - minY);
        rect->minY = minY;
    }

    if (maxX < rect->maxX) {
        rect->minX = rect->minX - (rect->maxX - maxX);
        rect->maxX = maxX;
    }

    if (maxY < rect->maxY) {
        rect->minY = rect->minY - (rect->maxY - maxY);
        rect->maxY = maxY;
    }

    return 1;
}

// OFFSET: 0x00507FB0/0x14014F600
int32_t CLayoutFrame::OnFrameResize() {
    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (!this->CalculateRect(&rect)) {
        this->m_layoutFlags |= 0x8;
        this->m_layoutFlags &= ~0x1u;
        return 0;
    }

    this->m_layoutFlags &= ~0x4u;
    this->m_layoutFlags &= ~0x8u;

    if ((this->m_layoutFlags & 0x1) && fabs(rect.minX - this->m_rect.minX) < 0.0000099999997f && fabs(rect.maxX - this->m_rect.maxX) < 0.0000099999997f && fequalz(rect.minY, this->m_rect.minY, 0.0000099999997f) && fequalz(rect.maxY, this->m_rect.maxY, 0.0000099999997f)) {
        return 1;
    }

    CRect oldRect = this->m_rect;
    this->m_rect = rect;
    this->m_layoutFlags |= 0x1;
    this->OnFrameSizeChanged(oldRect);

    return this->m_layoutFlags & 1;
}

// PROVEN: 528c53c0 IDENTICAL/UNPROVEN
// OFFSET: 0x00508100/0x14014F760
void CLayoutFrame::SetProtected() {
    this->m_layoutFlags &= ~0x600u;
    this->m_layoutFlags |= 0x100;
    this->PropagateProtectFlagToParent(0x200);
    this->ProtectFramepoints(0x200);
}

// PROVEN: 83d7fb2f IDENTICAL/DIFFERENT
// OFFSET: 0x00508140/0x14014F7E0
void CLayoutFrame::SetProtectFlag(uint32_t flag) {
    if (this->m_layoutFlags & 0x900) {
        return;
    }

    this->m_layoutFlags |= 0x800;
    this->m_layoutFlags &= ~0x200u;
    this->m_layoutFlags |= flag;
    this->PropagateProtectFlagToParent(flag);
    this->ProtectFramepoints(flag);
    this->m_layoutFlags &= ~0x800u;
}

// OFFSET: 0x00508270/0x14014F960
void CLayoutFrame::AddToResizeList() {
    if (s_resizePendingList.IsLinked(this)) {
        return;
    }

    CLayoutFrame* dependent = NULL;

    for (CLayoutFrame* frame = s_resizePendingList.Head(); frame; frame = s_resizePendingList.Next(frame)) {
        for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
            CFramePoint* framePoint = frame->m_framePoints[i];

            if (framePoint && !(framePoint->m_flags & 8) && framePoint->m_relative == this) {
                dependent = frame;
            }
        }

        if (dependent) {
            s_resizePendingList.LinkNode(this, 2, dependent);
            this->m_resizeCounter = 6;
            return;
        }
    }

    s_resizePendingList.LinkToTail(this);
    this->m_resizeCounter = 6;
}

// OFFSET: 0x005084A0/0x14014FB90
void CLayoutFrame::ResizePending() {
    for (uint32_t loops = 0; s_resizePendingList.Head() && loops < 32; loops++) {
        CLayoutFrame* frame = s_resizePendingList.Head();

        while (frame) {
            if (!frame->IsObjectLoaded()) {
                if (!loops) {
                    frame->OnFrameResize();
                }

                frame = frame->m_resizeLink.Next();
                continue;
            }

            int32_t resized = frame->OnFrameResize();
            CLayoutFrame* next = frame->m_resizeLink.Next();

            if (resized || --frame->m_resizeCounter == 0) {
                frame->m_layoutFlags &= ~0x4u;
                s_resizePendingList.UnlinkNode(frame);
            }

            frame = next;
        }
    }
}

// OFFSET: 0x00508580/0x14014FCB0
CLayoutFrame::CLayoutFrame() {
    this->m_animData = NULL;
    this->m_layoutFlags = 0;
    this->m_preload = 0;
    this->m_shown = 0;
    this->m_visible = 0;
    this->m_loadPending = 0;
    this->m_rect.minY = 0.0f;
    this->m_rect.minX = 0.0f;
    this->m_rect.maxY = 0.0f;
    this->m_rect.maxX = 0.0f;
    this->m_width = 0.0f;
    this->m_height = 0.0f;
    this->m_layoutScale = 1.0f;
    this->m_layoutDepth = 1.0f;
    this->m_resizeRect.minY = 0.0f;
    this->m_resizeRect.minX = 0.0f;
    this->m_resizeRect.maxY = 0.0f;
    this->m_resizeRect.maxX = 0.0f;

    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        this->m_framePoints[i] = NULL;
    }

    this->m_guard = 0;
}

// PROVEN: 7b077510 IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CLayoutFrame::AreChildrenProtected(int32_t& deferred) {
    return 0;
}

// PROVEN: 01536278 IDENTICAL/UNPROVEN
// OFFSET: 0x00508620/0x14014FD70
void CLayoutFrame::OnProtectedAttach(CLayoutFrame* frame) {
    if (this->m_layoutFlags & 0x400) {
        frame->SetProtectFlag(0x400);
    }

    if (this->m_layoutFlags & 0x300) {
        frame->SetProtectFlag(0x200);
    }
}

// PROVEN: 4cdd0eda IDENTICAL/UNPROVEN
// OFFSET: 0x00508660/0x14014FDC0
void CLayoutFrame::OnProtectedDetach(CLayoutFrame* frame) {
    if (this->m_layoutFlags & 0x700) {
        frame->SetProtectFlag(0x400);
    }
}

// PROVEN: 067d629b IDENTICAL/NONE
// OFFSET: 0x00508190
int32_t CLayoutFrame::AreDependentsProtected(int32_t& deferred) {
    for (CLayoutFrame::FRAMENODE* node = this->m_resizeList.Head(), * nextNode; !this->m_resizeList.IsEnd(node); node = nextNode) {
        nextNode = this->m_resizeList.RawNext(node);
        if (node->m_frame->m_layoutFlags & 0x800) {
            deferred = 1;
        } else if (node->m_frame->IsProtected()) {
            return 1;
        }
    }

    return 0;
}

// PROVEN: 90001b19 IDENTICAL/DIFFERENT
// OFFSET: 0x005081F0/0x14014F880
uint32_t CLayoutFrame::IsProtected() {
    if (this->m_layoutFlags & 0x400) {
        this->m_layoutFlags |= 0x800;

        int32_t deferred = 0;

        if (this->AreChildrenProtected(deferred) || this->AreDependentsProtected(deferred)) {
            this->m_layoutFlags |= 0x200;
        }

        this->m_layoutFlags &= ~0x800u;

        if (!deferred || (this->m_layoutFlags & 0x200)) {
            this->m_layoutFlags &= ~0x400u;
        }
    }

    return this->m_layoutFlags & 0x300;
}

// OFFSET: 0x00508A50/0x140150560
void CLayoutFrame::RegisterResize(CLayoutFrame* frame, uint32_t dependency) {
    for (CLayoutFrame::FRAMENODE* node = this->m_resizeList.Head(); node; node = this->m_resizeList.Next(node)) {
        if (node->m_frame == frame) {
            node->m_dependency |= dependency;
            return;
        }
    }

    CLayoutFrame::FRAMENODE* node = this->m_resizeList.NewNode(2, 0, 0);
    node->m_frame = frame;
    node->m_dependency = dependency;

    if (frame->m_layoutFlags & 0x400) {
        this->SetProtectFlag(0x400);
    }

    if (frame->m_layoutFlags & 0x300) {
        this->SetProtectFlag(0x200);
    }
}

// OFFSET: 0x00508F80/0x140150BE0
void CLayoutFrame::UnregisterResize(CLayoutFrame* frame, uint32_t dependency) {
    CLayoutFrame::FRAMENODE* node = this->m_resizeList.Head();

    while (node) {
        if (node->m_frame != frame) {
            node = this->m_resizeList.Next(node);
            continue;
        }

        node->m_dependency &= ~dependency;

        if (node->m_dependency) {
            return;
        }

        if (frame->m_layoutFlags & 0x700) {
            this->SetProtectFlag(0x400);
        }

        this->m_resizeList.DeleteNode(node);
        node = NULL;
    }
}

// OFFSET: 0x00508680/0x14014FDF0
void CLayoutFrame::Resize(int32_t force) {
    if (force && !(this->m_layoutFlags & 0x8) && this->OnFrameResize()) {
        if (s_resizePendingList.IsLinked(this)) {
            s_resizePendingList.UnlinkNode(this);
        }

        return;
    }

    if ((this->m_layoutFlags & 0x4) && ((this->m_layoutFlags & 0x2) || s_resizePendingList.IsLinked(this))) {
        this->m_resizeCounter = 6;
        return;
    }

    this->m_layoutFlags |= 0x4;

    if (this->m_layoutFlags & 0x2) {
        CLayoutFrame::FRAMENODE* node = this->m_resizeList.Head();

        while (!this->m_resizeList.IsEnd(node)) {
            CLayoutFrame::FRAMENODE* next = this->m_resizeList.RawNext(node);
            node->m_frame->Resize(0);
            node = next;
        }
    } else {
        this->AddToResizeList();
    }
}

// OFFSET: 0x00508720/0x14014FEF0
bool CLayoutFrame::SetLayoutScale(float scale, bool force) {
    if (scale == 0.0f) {
        DebugValidate("scale");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return false;
    }

    if ((force || fabs(scale - this->m_layoutScale) >= 0.00000023841858f) && scale > 0.00000011920929f) {
        this->m_layoutScale = scale;
        this->m_rect.minY = 0.0f;
        this->m_rect.minX = 0.0f;
        this->m_rect.maxY = 0.0f;
        this->m_rect.maxX = 0.0f;
        this->m_layoutFlags &= ~0x1u;
        this->Resize(0);
        return true;
    }

    return false;
}

// PROVEN: 0e34334e IDENTICAL/DIFFERENT
// OFFSET: 0x005087A0/0x14014FF80
bool CLayoutFrame::SetLayoutDepth(float depth, bool force) {
    if (!force && fabs(depth - this->m_layoutDepth) < 0.00000023841858f) {
        return false;
    }

    if (depth < 0.2f) {
        depth = 0.2f;
    }

    this->m_layoutFlags &= ~0x1u;
    this->m_layoutDepth = depth;
    this->Resize(0);

    return true;
}

// PROVEN: 2b3f4d39 IDENTICAL/NONE
// OFFSET: 0x00508800
void CLayoutFrame::SetWidth(float width) {
    this->m_layoutFlags &= ~0x8u;
    this->m_width = width;
    this->Resize(0);
}

// PROVEN: 1fdf5f53 IDENTICAL/NONE
// OFFSET: 0x00508830
void CLayoutFrame::SetHeight(float height) {
    this->m_layoutFlags &= ~0x8u;
    this->m_height = height;
    this->Resize(0);
}

// OFFSET: 0x00508860
void CLayoutFrame::SetSize(float width, float height) {
    this->m_layoutFlags &= ~0x8u;
    this->m_width = width;
    this->m_height = height;
    this->Resize(0);
}

// OFFSET: 0x00508890/0x140150250
void CLayoutFrame::GetSize(float& width, float& height, int32_t ignoreRect) {
    width = this->GetWidth();
    height = this->GetHeight();

    if (ignoreRect || (height != 0.0f && width != 0.0f)) {
        return;
    }

    if (this->IsResizePending()) {
        this->Resize(1);
    }

    if (this->m_layoutFlags & 0x1) {
        width = (this->m_rect.maxX - this->m_rect.minX) / this->m_layoutScale;
        height = (this->m_rect.maxY - this->m_rect.minY) / this->m_layoutScale;
    }
}

// PROVEN: d51564a2 IDENTICAL/NONE
// OFFSET: 0x005089A0
void CLayoutFrame::SetClampedToScreen(int32_t clamped) {
    if (clamped) {
        this->m_layoutFlags |= 0x10;
    } else {
        this->m_layoutFlags &= ~0x10u;
    }

    this->Resize(0);
}

// OFFSET: 0x00508430/0x14014FB00
int32_t CLayoutFrame::CanBeAnchorFor(CLayoutFrame* frame) {
    if (!frame->IsProtected()) {
        return 1;
    }

    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        CFramePoint* framePoint = this->m_framePoints[i];

        if (framePoint && !(framePoint->m_flags & 8) && framePoint->m_relative && !framePoint->m_relative->CanBeAnchorFor(frame)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x005089D0/0x1401504A0
void CLayoutFrame::OnFrameSizeChanged(const CRect& rect) {
    for (CLayoutFrame::FRAMENODE* node = this->m_resizeList.Head(); node; node = this->m_resizeList.Next(node)) {
        node->m_frame->Resize(0);
    }
}

// OFFSET: [0x00509140/0x140150DB0]
void CLayoutFrame::MarkFramePointsUnused() {
    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        CFramePoint* framePoint = this->m_framePoints[i];

        if (framePoint && !(framePoint->m_flags & 8)) {
            if (framePoint->m_relative) {
                framePoint->m_relative->UnregisterResize(this, 1 << i);
            }

            framePoint->MarkUnused();
        }
    }
}

// OFFSET: 0x00509250/0x140150F30
void CLayoutFrame::SetPoint(FRAMEPOINT point, CLayoutFrame* relative, FRAMEPOINT relativePoint, float offsetX, float offsetY, int32_t doResize) {
    if (!relative) {
        DebugValidate("relative");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (relative == this) {
        DebugValidate("relative != this");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (!relative->CanBeAnchorFor(this)) {
        return;
    }

    CFramePoint* framePoint = this->m_framePoints[point];

    if (!framePoint) {
        this->m_framePoints[point] = new CFramePoint(relative, relativePoint, offsetX, offsetY);
    } else if (framePoint->m_relative && framePoint->m_relative == relative) {
        if (static_cast<int8_t>(framePoint->m_framePoint) == relativePoint && !fnotequal(framePoint->m_x, offsetX) && !fnotequal(framePoint->m_y, offsetY)) {
            return;
        }

        framePoint->SetRelative(relative, relativePoint, offsetX, offsetY);
        this->m_layoutFlags &= ~0x8u;

        if (doResize) {
            this->Resize(0);
        }

        return;
    } else {
        if (framePoint->m_relative) {
            framePoint->m_relative->UnregisterResize(this, 1 << point);
        }

        framePoint->SetRelative(relative, relativePoint, offsetX, offsetY);
    }

    this->m_layoutFlags &= ~0x8u;
    relative->RegisterResize(this, 1 << point);

    if (doResize) {
        this->Resize(0);
    }
}

// OFFSET: 0x005093E0/0x140151140
void CLayoutFrame::SetAllPoints(CLayoutFrame* relative, int32_t doResize) {
    if (!relative) {
        DebugValidate("relative");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (relative == this) {
        DebugValidate("relative != this");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (!relative->CanBeAnchorFor(this)) {
        return;
    }

    this->MarkFramePointsUnused();

    if (this->m_framePoints[FRAMEPOINT_TOPLEFT]) {
        this->m_framePoints[FRAMEPOINT_TOPLEFT]->SetRelative(relative, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f);
    } else {
        this->m_framePoints[FRAMEPOINT_TOPLEFT] = new CFramePoint(relative, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f);
    }

    if (this->m_framePoints[FRAMEPOINT_BOTTOMRIGHT]) {
        this->m_framePoints[FRAMEPOINT_BOTTOMRIGHT]->SetRelative(relative, FRAMEPOINT_BOTTOMRIGHT, 0.0f, 0.0f);
    } else {
        this->m_framePoints[FRAMEPOINT_BOTTOMRIGHT] = new CFramePoint(relative, FRAMEPOINT_BOTTOMRIGHT, 0.0f, 0.0f);
    }

    this->m_layoutFlags &= ~0x8u;
    relative->RegisterResize(this, (1 << FRAMEPOINT_TOPLEFT) | (1 << FRAMEPOINT_BOTTOMRIGHT));

    if (doResize) {
        this->Resize(0);
    }
}

// OFFSET: 0x00508BB0/0x1401506A0
void CLayoutFrame::SetDeferredResize(int32_t deferred) {
    if (deferred) {
        this->m_layoutFlags |= 0x2;

        if (this->m_resizeLink.IsLinked()) {
            s_resizePendingList.UnlinkNode(this);
        }

        return;
    }

    this->m_layoutFlags &= ~0x2u;

    if (this->IsResizePending()) {
        this->Resize(1);
    }
}

// OFFSET: 0x00509530/0x140151300
int32_t CLayoutFrame::FlattenFrame(CLayoutFrame* relative, float width, float height, float offsetX, float offsetY) {
    if (this->IsResizePending()) {
        this->Resize(1);
    }

    if (!(this->m_layoutFlags & 0x1)) {
        return 0;
    }

    CRect rect = this->m_rect;

    if (!(relative->m_layoutFlags & 0x1)) {
        return 0;
    }

    float relativeTop = relative->m_rect.maxY;
    float relativeLeft = relative->m_rect.minX;

    this->MarkFramePointsUnused();

    float invScale = 1.0f / this->m_layoutScale;
    float y = -(relativeTop - (rect.maxY + offsetY)) * invScale;
    float x = invScale * -(relativeLeft - (rect.minX + offsetX));
    this->SetPoint(FRAMEPOINT_TOPLEFT, relative, FRAMEPOINT_TOPLEFT, x, y, 0);

    if (width == 0.0f) {
        width = (rect.maxX - rect.minX) / this->m_layoutScale;
    }

    this->SetWidth(width);

    if (height == 0.0f) {
        height = (rect.maxY - rect.minY) / this->m_layoutScale;
    }

    this->SetHeight(height);
    this->Resize(1);

    return 1;
}

// OFFSET: 0x00509790/0x140151670
int32_t CLayoutFrame::UnflattenFrame(CLayoutFrame* relative) {
    if (this->IsResizePending()) {
        this->Resize(1);
    }

    if (!(this->m_layoutFlags & 0x1)) {
        return 0;
    }

    CRect rect = this->m_rect;

    if (!(relative->m_layoutFlags & 0x1)) {
        return 0;
    }

    CRect relativeRect = relative->m_rect;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    C2Vector relativePoints[FRAMEPOINT_NUMPOINTS];
    C2Vector points[FRAMEPOINT_NUMPOINTS];
    FRAMEPOINT closest = FRAMEPOINT_TOPLEFT;

    GetRectFramePoints(relativeRect, relativePoints);
    GetRectFramePoints(rect, points);

    float closestDistance = std::numeric_limits<float>::max();

    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        float dx = relativePoints[i].x - points[i].x;
        float dy = relativePoints[i].y - points[i].y;
        float distance = dx * dx + dy * dy;

        if (distance < closestDistance) {
            closestDistance = distance;
            closest = static_cast<FRAMEPOINT>(i);
            offsetX = points[i].x - relativePoints[i].x;
            offsetY = points[i].y - relativePoints[i].y;
        }
    }

    this->MarkFramePointsUnused();

    float scale = 1.0f / this->m_layoutScale;
    this->SetPoint(closest, relative, closest, offsetX * scale, offsetY * scale, 0);
    this->Resize(1);

    return 1;
}

// OFFSET: 0x00509BF0
int32_t CLayoutFrame::DragBy(CLayoutFrame* relative, float offsetX, float offsetY, FRAMEPOINT point) {
    float width = this->GetWidth();
    float height = this->GetHeight();
    float newOffsetX;
    float newOffsetY;

    switch (point) {
        case FRAMEPOINT_TOPLEFT: {
            float scale = 1.0f / this->m_layoutScale;
            newOffsetX = offsetX;
            width = width - scale * offsetX;

            if (0.0f != this->m_resizeRect.minX && this->m_resizeRect.minX > width) {
                newOffsetX = newOffsetX - (this->m_resizeRect.minX - width) * this->m_layoutScale;
                width = this->m_resizeRect.minX;
            }

            if (0.0f != this->m_resizeRect.maxX && this->m_resizeRect.maxX < width) {
                newOffsetX = newOffsetX + (width - this->m_resizeRect.maxX) * this->m_layoutScale;
                width = this->m_resizeRect.maxX;
            }

            newOffsetY = offsetY;
            height = height + scale * offsetY;

            if (0.0f != this->m_resizeRect.minY && this->m_resizeRect.minY > height) {
                newOffsetY = newOffsetY + (this->m_resizeRect.minY - height) * this->m_layoutScale;
                height = this->m_resizeRect.minY;
            }

            if (0.0f != this->m_resizeRect.maxY && this->m_resizeRect.maxY < height) {
                newOffsetY = newOffsetY - (height - this->m_resizeRect.maxY) * this->m_layoutScale;
                height = this->m_resizeRect.maxY;
            }

            break;
        }

        case FRAMEPOINT_TOP: {
            newOffsetY = offsetY;
            height = height + offsetY / this->m_layoutScale;

            if (0.0f != this->m_resizeRect.minY && this->m_resizeRect.minY > height) {
                newOffsetY = newOffsetY + (this->m_resizeRect.minY - height) * this->m_layoutScale;
                height = this->m_resizeRect.minY;
            }

            if (0.0f != this->m_resizeRect.maxY && this->m_resizeRect.maxY < height) {
                newOffsetY = newOffsetY - (height - this->m_resizeRect.maxY) * this->m_layoutScale;
                height = this->m_resizeRect.maxY;
            }

            newOffsetX = 0.0f;
            break;
        }

        case FRAMEPOINT_TOPRIGHT: {
            float scale = 1.0f / this->m_layoutScale;
            width = offsetX * scale + width;

            if (0.0f != this->m_resizeRect.minX && this->m_resizeRect.minX > width) {
                width = this->m_resizeRect.minX;
            }

            if (0.0f != this->m_resizeRect.maxX && this->m_resizeRect.maxX < width) {
                width = this->m_resizeRect.maxX;
            }

            newOffsetY = offsetY;
            height = height + scale * offsetY;

            if (0.0f != this->m_resizeRect.minY && this->m_resizeRect.minY > height) {
                newOffsetY = newOffsetY + (this->m_resizeRect.minY - height) * this->m_layoutScale;
                height = this->m_resizeRect.minY;
            }

            if (0.0f != this->m_resizeRect.maxY && this->m_resizeRect.maxY < height) {
                newOffsetY = newOffsetY - (height - this->m_resizeRect.maxY) * this->m_layoutScale;
                height = this->m_resizeRect.maxY;
            }

            newOffsetX = 0.0f;
            break;
        }

        case FRAMEPOINT_LEFT: {
            newOffsetX = offsetX;
            width = width - offsetX / this->m_layoutScale;

            if (0.0f != this->m_resizeRect.minX && this->m_resizeRect.minX > width) {
                newOffsetX = newOffsetX - (this->m_resizeRect.minX - width) * this->m_layoutScale;
                width = this->m_resizeRect.minX;
            }

            if (0.0f != this->m_resizeRect.maxX && this->m_resizeRect.maxX < width) {
                newOffsetX = newOffsetX + (width - this->m_resizeRect.maxX) * this->m_layoutScale;
                width = this->m_resizeRect.maxX;
            }

            newOffsetY = 0.0f;
            break;
        }

        case FRAMEPOINT_CENTER: {
            CFramePoint* framePoint = this->GetFramePoint(FRAMEPOINT_TOPLEFT);

            if (framePoint && framePoint->m_relative) {
                framePoint->m_x = offsetX / this->m_layoutScale + framePoint->m_x;
                framePoint->m_y = offsetY / this->m_layoutScale + framePoint->m_y;
                this->Resize(1);
                return 1;
            }

            newOffsetX = offsetX;
            newOffsetY = offsetY;
            break;
        }

        case FRAMEPOINT_RIGHT: {
            width = offsetX / this->m_layoutScale + width;

            if (0.0f != this->m_resizeRect.minX && this->m_resizeRect.minX > width) {
                width = this->m_resizeRect.minX;
            }

            if (0.0f != this->m_resizeRect.maxX && this->m_resizeRect.maxX < width) {
                width = this->m_resizeRect.maxX;
            }

            newOffsetX = 0.0f;
            newOffsetY = 0.0f;
            break;
        }

        case FRAMEPOINT_BOTTOMLEFT: {
            float scale = 1.0f / this->m_layoutScale;
            newOffsetX = offsetX;
            width = width - scale * offsetX;

            if (0.0f != this->m_resizeRect.minX && this->m_resizeRect.minX > width) {
                newOffsetX = newOffsetX - (this->m_resizeRect.minX - width) * this->m_layoutScale;
                width = this->m_resizeRect.minX;
            }

            if (0.0f != this->m_resizeRect.maxX && this->m_resizeRect.maxX < width) {
                newOffsetX = newOffsetX + (width - this->m_resizeRect.maxX) * this->m_layoutScale;
                width = this->m_resizeRect.maxX;
            }

            newOffsetY = 0.0f;
            height = height - scale * offsetY;

            if (0.0f != this->m_resizeRect.minY && this->m_resizeRect.minY > height) {
                height = this->m_resizeRect.minY;
            }

            if (0.0f != this->m_resizeRect.maxY && this->m_resizeRect.maxY < height) {
                height = this->m_resizeRect.maxY;
            }

            break;
        }

        case FRAMEPOINT_BOTTOM: {
            height = height - offsetY / this->m_layoutScale;
            newOffsetY = 0.0f;

            if (0.0f != this->m_resizeRect.minY && this->m_resizeRect.minY > height) {
                height = this->m_resizeRect.minY;
            }

            if (0.0f != this->m_resizeRect.maxY && this->m_resizeRect.maxY < height) {
                height = this->m_resizeRect.maxY;
            }

            newOffsetX = 0.0f;
            break;
        }

        case FRAMEPOINT_BOTTOMRIGHT: {
            float scale = 1.0f / this->m_layoutScale;
            width = offsetX * scale + width;

            if (0.0f != this->m_resizeRect.minX && this->m_resizeRect.minX > width) {
                width = this->m_resizeRect.minX;
            }

            if (0.0f != this->m_resizeRect.maxX && this->m_resizeRect.maxX < width) {
                width = this->m_resizeRect.maxX;
            }

            newOffsetY = 0.0f;
            height = height - scale * offsetY;

            if (0.0f != this->m_resizeRect.minY && this->m_resizeRect.minY > height) {
                height = this->m_resizeRect.minY;
            }

            if (0.0f != this->m_resizeRect.maxY && this->m_resizeRect.maxY < height) {
                height = this->m_resizeRect.maxY;
            }

            newOffsetX = 0.0f;
            break;
        }

        default: {
            newOffsetX = offsetX;
            newOffsetY = offsetY;
            break;
        }
    }

    return this->FlattenFrame(relative, width, height, newOffsetX, newOffsetY);
}

// OFFSET: 0x00509020/0x140150C50
void CLayoutFrame::DestroyLayout() {
    for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        CFramePoint* framePoint = this->m_framePoints[i];

        if (framePoint) {
            if (!(framePoint->m_flags & 8) && framePoint->m_relative) {
                framePoint->m_relative->UnregisterResize(this, 1 << i);
            }

            delete framePoint;
            this->m_framePoints[i] = NULL;
        }
    }

    for (CLayoutFrame::FRAMENODE* node = this->m_resizeList.Head(); node; node = this->m_resizeList.Next(node)) {
        CLayoutFrame* frame = node->m_frame;

        for (int32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
            CFramePoint* framePoint = frame->m_framePoints[i];

            if (framePoint && framePoint->m_relative == this) {
                framePoint->MarkUnused();
            }
        }

        frame->m_layoutFlags &= ~0x8u;
    }

    this->m_resizeList.Clear();
    s_resizePendingList.UnlinkNode(this);
}

// OFFSET: [0x005091A0/0x140150E40, 0x0050A270]
CLayoutFrame::~CLayoutFrame() {
    if (this->m_animData) {
        delete this->m_animData;
        this->m_animData = NULL;
    }

    this->DestroyLayout();
}

// OFFSET: 0x0050A2A0/0x1401521A0
void CLayoutFrame::LoadXML(linkObject* node, CStatus* status) {
    Region* region = static_cast<Region*>(node);

    if (region->m_layoutFlags & 4) {
        this->m_preload = (region->m_layoutFlags >> 3) & 1;
    }

    linkObject* size = node->GetChild(XML_SIZE);

    if (size) {
        float x;
        float y;

        if (static_cast<Dimension*>(size)->Load(x, y, status)) {
            this->SetWidth(x);
            this->SetHeight(y);
        }
    }

    CLayoutFrame* parent = this->GetLayoutParent();
    int32_t setAllPoints = (region->m_layoutFlags & 1) && (region->m_layoutFlags & 2);
    linkObject* anchors = node->GetChild(XML_ANCHORS);

    if (!anchors) {
        if (setAllPoints) {
            this->SetAllPoints(parent, 1);
        }

        return;
    }

    for (linkObject* child = anchors->m_child; child; child = child->m_next) {
        if (child->m_element != XML_ANCHOR) {
            continue;
        }

        Anchor* anchor = static_cast<Anchor*>(child);
        CLayoutFrame* relative = parent;
        FRAMEPOINT point;
        FRAMEPOINT relativePoint;
        float offsetX = 0.0f;
        float offsetY = 0.0f;

        if (!anchor->GetAnchor(point, relativePoint, offsetX, offsetY, status)) {
            status->Add(STATUS_WARNING, "Invalid anchor point in frame.");
            continue;
        }

        if (anchor->m_anchorFlags & 4) {
            relative = this->GetLayoutFrameByName(anchor->m_relativeTo.m_string);

            if (!relative) {
                status->Add(STATUS_WARNING, "Couldn't find relative frame: %s", anchor->m_relativeTo.m_string);
                continue;
            }

            if (relative == this) {
                status->Add(STATUS_WARNING, "Frame anchored to itself: %s", anchor->m_relativeTo.m_string);
                continue;
            }
        }

        this->SetPoint(point, relative, relativePoint, offsetX, offsetY, 0);
    }

    this->Resize(0);
}

CLayoutFrame* CLayoutFrame::GetLayoutParent() {
    return NULL;
}

void CLayoutFrame::PropagateProtectFlagToParent(uint32_t flag) {
}

int32_t CLayoutFrame::IsAttachmentOrigin() {
    return 0;
}

CLayoutFrame* CLayoutFrame::GetLayoutFrameByName(const char* name) {
    return NULL;
}

// PROVEN: a9b2f1df IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
int32_t CLayoutFrame::IsObjectLoaded() {
    return 1;
}

// PROVEN: cba41c6f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CLayoutFrame::LoadObject() {
}

// PROVEN: 4b3e9b26 IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CLayoutFrame::DumpInfo(int32_t depth, void (*print)(char*, void*), void* param) {
}

// PROVEN: 520a98c3 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CLayoutFrame::HideThis() {
}

// PROVEN: 3f7df67e IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLayoutFrame::ShowThis(bool loadPending) {
}

// OFFSET: 0x00508C70/0x1401507C0
void CLayoutFrame::AnimData_Init() {
    this->Resize(1);

    if (!this->m_animData) {
        this->m_animData = new CAnimationData();
    }
}

// OFFSET: 0x00508DB0/0x140150960
void CLayoutFrame::AnimData_ResetPositionData() {
    if (this->m_animData) {
        this->m_animData->ResetPositionData();
    }

    this->Resize(1);
}

// PROVEN: 51a9da86 IDENTICAL/NONE
// OFFSET: 0x00507A40/0x14014EF00
void CLayoutFrame::AnimData_ResetAlphaData() {
    if (this->m_animData) {
        this->m_animData->ResetAlphaData();
    }
}

// PROVEN: e0115494 IDENTICAL/NONE
// OFFSET: 0x00507A50/0x14014EF20
bool CLayoutFrame::AnimData_HasActivePosition() {
    if (this->m_animData) {
        return this->m_animData->m_hasPosition;
    }

    return false;
}

// PROVEN: 0bbb489e IDENTICAL/NONE
// OFFSET: 0x00507A70/0x14014EF40
bool CLayoutFrame::AnimData_HasActiveAlpha() {
    if (this->m_animData) {
        return this->m_animData->m_hasAlpha;
    }

    return false;
}

// PROVEN: 5d34ab29 IDENTICAL/NONE
// OFFSET: 0x00507A90/0x14014EF60
void CLayoutFrame::AnimData_StorePosition(const C3Vector* position) {
    if (this->m_animData) {
        this->m_animData->StorePosition(position);
    }
}

// PROVEN: 27f3b041 IDENTICAL/NONE
// OFFSET: 0x00507AB0/0x14014EFD0
C3Vector* CLayoutFrame::AnimData_GetPositionPtr() {
    return this->m_animData->m_position;
}

// PROVEN: d0a44095 IDENTICAL/NONE
// OFFSET: 0x00507AC0/0x14014EFE0
void CLayoutFrame::AnimData_StoreAlpha(uint8_t alpha) {
    CAnimationData* data = this->m_animData;

    if (data) {
        data->m_hasAlpha = 1;
        data->m_alpha = alpha;
    }
}

// PROVEN: 959e0ddf IDENTICAL/NONE
// OFFSET: 0x00507AE0/0x14014F000
uint8_t CLayoutFrame::AnimData_GetAlpha() {
    if (this->m_animData) {
        return this->m_animData->m_alpha;
    }

    return 0xFF;
}
