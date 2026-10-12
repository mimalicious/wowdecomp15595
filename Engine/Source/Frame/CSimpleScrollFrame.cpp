#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00CA4470/0x140B42290
int32_t CSimpleScrollFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3A5C/0x140CB97B0
int32_t CSimpleScrollFrame::s_objectType;

// STATIC: 0x00CA3460/0x140B40620
const char* CSimpleScrollFrame::s_objectTypeName = "ScrollFrame";

// STATIC: 0x00DD3A60/0x140CB97B8
CDataAllocator CSimpleScrollFrame::s_allocator(sizeof(CSimpleScrollFrame), 5);

// INLINED
void CSimpleScrollFrame::operator delete(void* ptr) {
    CSimpleScrollFrame::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: 0x005201B0/0x14016D870
void CSimpleScrollFrame::RunOnHorizontalScrollScript() {
    if (this->HasScript(this->m_onHorizontalScroll)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * this->m_scrollOffset.x));
        this->RunScript(this->m_onHorizontalScroll, 1, NULL);
    }
}

// OFFSET: 0x00520220/0x14016D910
void CSimpleScrollFrame::RunOnVerticalScrollScript() {
    if (this->HasScript(this->m_onVerticalScroll)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * this->m_scrollOffset.y));
        this->RunScript(this->m_onVerticalScroll, 1, NULL);
    }
}

// OFFSET: 0x00520290/0x14016D9B0
void CSimpleScrollFrame::RunOnScrollRangeChangedScript() {
    if (this->HasScript(this->m_onScrollRangeChanged)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * this->m_scrollRange.x));
        lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * this->m_scrollRange.y));
        this->RunScript(this->m_onScrollRangeChanged, 2, NULL);
    }
}

// PROVEN: 620d4f07 IDENTICAL/IDENTICAL
// OFFSET: 0x0051FF60/0x14016DA80
void CSimpleScrollFrame::UnparentFrame(CSimpleFrame* child) {
    if (child == this->m_scrollChild) {
        this->m_scrollChild = NULL;
    }

    CSimpleFrame::UnparentFrame(child);
}

// PROVEN: 25aa2d2c IDENTICAL/IDENTICAL
// OFFSET: 0x0051FF90/0x14016D650
const char* CSimpleScrollFrame::GetObjectTypeName() {
    return CSimpleScrollFrame::s_objectTypeName;
}

// PROVEN: 3759fa8b IDENTICAL/IDENTICAL
// OFFSET: 0x0051FFA0/0x14016D660
int32_t CSimpleScrollFrame::GetScriptMetaTable() {
    return CSimpleScrollFrame::s_metatable;
}

// PROVEN: 8a6fe960 IDENTICAL/IDENTICAL
// OFFSET: 0x0051FFB0/0x14016D670
FrameScript_Object::ScriptIx* CSimpleScrollFrame::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onhorizontalscroll", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,offset) %s end";
        return &this->m_onHorizontalScroll;
    }

    if (!SStrCmp(name, "onverticalscroll", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,offset) %s end";
        return &this->m_onVerticalScroll;
    }

    if (!SStrCmp(name, "onscrollrangechanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,xrange,yrange) %s end";
        return &this->m_onScrollRangeChanged;
    }

    return NULL;
}

// PROVEN: 68923b0e IDENTICAL/IDENTICAL
// OFFSET: 0x005200B0/0x14016D730
void CSimpleScrollFrame::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onHorizontalScroll, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onVerticalScroll, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onScrollRangeChanged, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: 18477c7b DIFFERENT/IDENTICAL
// OFFSET: 0x00520120/0x14016D7D0
void CSimpleScrollFrame::UpdateScrollChild() {
    if (this->m_scrollChild) {
        this->m_scrollChild->MarkFramePointsUnused();
        this->m_scrollChild->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT, -this->m_scrollOffset.x, this->m_scrollOffset.y, 1);
    }
}

// PROVEN: 72c1d12a IDENTICAL/IDENTICAL
// OFFSET: 0x00520170/0x14016D840
void CSimpleScrollFrame::OnFrameSizeChanged(float width, float height) {
    CSimpleFrame::OnFrameSizeChanged(width, height);
    this->m_scrollChildUpdate = 1;
}

// INLINED
CSimpleScrollFrame::~CSimpleScrollFrame() {
}

// PROVEN: 5468ca43 IDENTICAL/IDENTICAL
// OFFSET: 0x00520330/0x14016DA80
bool CSimpleScrollFrame::IsA(int32_t type) {
    return type == CSimpleScrollFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 3fabb958 IDENTICAL/DIFFERENT
// OFFSET: 0x00520370/0x14016DAB0
bool CSimpleScrollFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleScrollFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x005203B0/0x14016DB20
CSimpleScrollFrame::CSimpleScrollFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_scrollChildUpdate = 0;
    this->m_scrollChild = NULL;
    this->m_scrollRange.x = 0.0f;
    this->m_scrollRange.y = 0.0f;
    this->m_scrollOffset.x = 0.0f;
    this->m_scrollOffset.y = 0.0f;
}

// PROVEN: dae24b96 DIFFERENT/IDENTICAL
// OFFSET: 0x00520460/0x14016DBF0
void CSimpleScrollFrame::SetScrollChild(CSimpleFrame* child) {
    if (this->m_scrollChild) {
        this->m_scrollChild->SetBeingScrolled(0, 0);
        this->m_scrollChild->SetParent(NULL);
    }

    this->m_scrollChild = child;

    if (child) {
        child->SetParent(this);
        this->m_scrollChild->SetBeingScrolled(1, 1);
    }

    this->UpdateScrollChild();
    this->m_scrollChildUpdate = 1;
}

// PROVEN: b422e887 DIFFERENT/IDENTICAL
// OFFSET: 0x00520510/0x14016DC70
void CSimpleScrollFrame::SetHorizontalScroll(float offset) {
    if (fabsf(offset - this->m_scrollOffset.x) >= 0.00000095367432f) {
        this->m_scrollOffset.x = offset;
        this->UpdateScrollChild();
        this->RunOnHorizontalScrollScript();
    }
}

// PROVEN: 716eae40 DIFFERENT/IDENTICAL
// OFFSET: 0x00520550/0x14016DCB0
void CSimpleScrollFrame::SetVerticalScroll(float offset) {
    if (fabsf(offset - this->m_scrollOffset.y) >= 0.00000095367432f) {
        this->m_scrollOffset.y = offset;
        this->UpdateScrollChild();
        this->RunOnVerticalScrollScript();
    }
}

// OFFSET: 0x00520590/0x14016DCF0
void CSimpleScrollFrame::UpdateScrollChildRect(float width, float height) {
    if (!this->m_scrollChild) {
        return;
    }

    C2Vector range = this->m_scrollRange;
    CRect rect(FLT_MAX, FLT_MAX, 0.0f, 0.0f);
    this->m_scrollChild->GetBoundsRect(rect);

    float rangeX = rect.maxX - rect.minX - width;

    if (!(0.0f < rangeX)) {
        rangeX = 0.0f;
    }

    float scale = 1.0f / this->m_layoutScale;
    this->m_scrollRange.x = rangeX * scale;

    float rangeY = rect.maxY - rect.minY - height;
    this->m_scrollRange.y = (rangeY > 0.0f ? rangeY : 0.0f) * scale;

    if (!(fabsf(this->m_scrollRange.x - range.x) < 0.00000095367432f) || !(fabsf(this->m_scrollRange.y - range.y) < 0.00000095367432f)) {
        this->RunOnScrollRangeChangedScript();
    }
}

// PROVEN: 7fb36aa8 IDENTICAL/IDENTICAL
// OFFSET: 0x00520690/0x14016DE10
void CSimpleScrollFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (this->m_scrollChildUpdate) {
        CRect rect(0.0f, 0.0f, 0.0f, 0.0f);

        if (this->GetRect(&rect)) {
            this->UpdateScrollChildRect(rect.maxX - rect.minX, rect.maxY - rect.minY);
        }

        this->m_scrollChildUpdate = 0;
    }
}

// OFFSET: 0x00520710/0x14016DE90
void CSimpleScrollFrame::RenderScrollChild(void* param) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(param);
    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);

    if (!scrollFrame->GetHitRect(rect) || !scrollFrame->m_scrollChild || !scrollFrame->m_scrollChild->m_visible) {
        return;
    }

    C44Matrix projection;
    C44Matrix view;
    CGxDevice* device = g_theGxDevicePtr;
    projection = device->m_projection;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    float left = 0.0f;
    float bottom = 0.0f;
    float right = 0.0f;
    float top = 0.0f;
    NDCToDDC(minX, minY, &left, &bottom);
    NDCToDDC(maxX, maxY, &right, &top);

    if (left > rect.minX) {
        rect.minX = left;
    }

    if (right < rect.maxX) {
        rect.maxX = right;
    }

    if (bottom > rect.minY) {
        rect.minY = bottom;
    }

    if (top < rect.maxY) {
        rect.maxY = top;
    }

    if (rect.minX == rect.maxX || rect.minY == rect.maxY) {
        return;
    }

    CRect screenRect = rect;
    C2Vector offset;
    offset.x = 0.0f;
    offset.y = 0.0f;
    CameraSetupScreenProjection(rect, offset, 0.0f, 0);

    DDCToNDC(rect.minX, rect.minY, &rect.minX, &rect.minY);
    DDCToNDC(rect.maxX, rect.maxY, &rect.maxX, &rect.maxY);

    if (rect.minX < 0.0f) {
        rect.maxX = rect.maxX - rect.minX;
        rect.minX = 0.0f;
    }

    if (rect.maxX > 1.0f) {
        rect.minX = rect.minX - (rect.maxX - 1.0f);
        rect.maxX = 1.0f;
    }

    if (rect.minY < 0.0f) {
        rect.maxY = rect.maxY - rect.minY;
        rect.minY = 0.0f;
    }

    if (rect.maxY > 1.0f) {
        rect.minY = rect.minY - (rect.maxY - 1.0f);
        rect.maxY = 1.0f;
    }

    if (rect.minX < rect.maxX && rect.minY < rect.maxY) {
        GxXformSetViewport(rect.minX, rect.maxX, rect.minY, rect.maxY, 0.0f, 1.0f);
        offset.x = 0.0f;
        offset.y = 0.0f;
        CameraSetupScreenProjection(screenRect, offset, 0.0f, 0);
        scrollFrame->m_scrollChild->OnFrameRender();
    }

    GxXformSetProjection(projection);
    GxXformSetView(view);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x00520B50/0x14016E560
void CSimpleScrollFrame::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    linkObject* scrollChild = node->m_child;

    while (scrollChild && scrollChild->m_element != XML_SCROLLCHILD) {
        scrollChild = scrollChild->m_next;
    }

    if (!scrollChild) {
        return;
    }

    linkObject* childNode = scrollChild->m_child;

    if (!childNode) {
        const char* name = this->GetName();
        status->Add(STATUS_WARNING, "Frame %s: Scroll frame created without scroll child", name ? name : "<unnamed>");
        return;
    }

    if (!(childNode->m_flags & 0x8)) {
        const char* name = this->GetName();
        status->Add(STATUS_WARNING, "Frame %s: Invalid scroll child", name ? name : "<unnamed>");
        return;
    }

    CSimpleFrame* child = FrameXML_CreateFrame(childNode, this, status);

    if (child) {
        this->SetScrollChild(child);
    }
}

// PROVEN: 03eb32d5 IDENTICAL/IDENTICAL
// OFFSET: 0x00520C20/0x14016E640
void CSimpleScrollFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);

    if (layer == 4) {
        batch->QueueCallback(&CSimpleScrollFrame::RenderScrollChild, this);
    }
}

// OFFSET: 0x00520C50/0x14016E690
bool CSimpleScrollFrame::GetBoundsRect(CRect& rect) {
    CRect frameRect(0.0f, 0.0f, 0.0f, 0.0f);

    if (this->IsResizePending()) {
        this->Resize(1);
    }

    if (this->GetRect(&frameRect)) {
        rect.minX = frameRect.minX >= rect.minX ? rect.minX : frameRect.minX;
        rect.maxX = frameRect.maxX <= rect.maxX ? rect.maxX : frameRect.maxX;
        rect.minY = frameRect.minY >= rect.minY ? rect.minY : frameRect.minY;
        rect.maxY = frameRect.maxY <= rect.maxY ? rect.maxY : frameRect.maxY;
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        if (!region->m_shown) {
            continue;
        }

        if (region->IsResizePending()) {
            region->Resize(1);
        }

        if (region->GetRect(&frameRect)) {
            rect.minX = frameRect.minX >= rect.minX ? rect.minX : frameRect.minX;
            rect.maxX = frameRect.maxX <= rect.maxX ? rect.maxX : frameRect.maxX;
            rect.minY = frameRect.minY >= rect.minY ? rect.minY : frameRect.minY;
            rect.maxY = frameRect.maxY <= rect.maxY ? rect.maxY : frameRect.maxY;
        }
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        if (node->m_frame != this->m_scrollChild && node->m_frame->m_shown) {
            node->m_frame->GetBoundsRect(rect);
        }
    }

    return rect.maxY > rect.minY && rect.maxX > rect.minX;
}
