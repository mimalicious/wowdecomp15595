#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00CD2830
CDataAllocator CSimpleHyperlinkButton::s_allocator(sizeof(CSimpleHyperlinkButton), 10);

// STATIC: %0x00CD282C
int32_t CSimpleHyperlinkButton::s_objectType;

// STATIC: %0x00BB94EC
const char* CSimpleHyperlinkButton::s_objectTypeName = "HyperLinkButton";

// OFFSET: %0x00546890
bool CSimpleHyperlinkButton::IsA(int32_t type) {
    return type == CSimpleHyperlinkButton::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: %0x005468D0
bool CSimpleHyperlinkButton::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleHyperlinkButton::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: %0x00546870
const char* CSimpleHyperlinkButton::GetObjectTypeName() {
    return CSimpleHyperlinkButton::s_objectTypeName;
}

// INLINED
void CSimpleHyperlinkButton::operator delete(void* ptr) {
    if (ptr) {
        CSimpleHyperlinkButton::s_allocator.PutData(ptr, NULL, 0);
    }
}

// OFFSET: %0x00546200
void CSimpleHyperlinkButton::OnLayerCursorEnter(int32_t a2) {
    static_cast<CSimpleHyperlinkedFrame*>(this->m_parent)->OnHyperlinkEnter(this->m_linkData, this->m_linkText);
}

// OFFSET: %0x00546230
void CSimpleHyperlinkButton::OnLayerCursorExit(int32_t a2, int32_t a3) {
    static_cast<CSimpleHyperlinkedFrame*>(this->m_parent)->OnHyperlinkLeave(this->m_linkData, this->m_linkText);
}

// OFFSET: %0x00546260
void CSimpleHyperlinkButton::OnClick(const char* btn, int32_t down) {
    static_cast<CSimpleHyperlinkedFrame*>(this->m_parent)->OnHyperlinkClick(this->m_linkData, this->m_linkText, btn);
}

// OFFSET: %0x00546560
void CSimpleHyperlinkButton::SetHyperlink(CSimpleFontString* fontString, const GXUFONTHYPERLINKINFO* info) {
    if (!fontString || !info) {
        this->Hide();
        return;
    }

    this->m_linkData = static_cast<char*>(SMemReAlloc(this->m_linkData, info->m_linkDataLength + 1, __FILE__, __LINE__, 0));
    SStrCopy(this->m_linkData, info->m_linkData, info->m_linkDataLength + 1);

    this->m_linkText = static_cast<char*>(SMemReAlloc(this->m_linkText, info->m_linkTextLength + 1, __FILE__, __LINE__, 0));
    SStrCopy(this->m_linkText, info->m_linkText, info->m_linkTextLength + 1);

    float bottom = 0.0f;
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    NDCToDDC(info->m_indentation, info->m_baseLineHeight, &left, &bottom);
    NDCToDDC(info->m_right, info->m_lineHeight, &right, &top);

    float x = 0.0f;
    FRAMEPOINT point = FRAMEPOINT_TOPLEFT;

    switch (fontString->m_styleFlags & 0x7) {
        case 1:
            x = left;
            break;
        case 2:
            point = FRAMEPOINT_TOP;
            x = left + (right - left) * 0.5f;
            break;
        case 4:
            point = FRAMEPOINT_TOPRIGHT;
            x = right;
            break;
    }

    this->MarkFramePointsUnused();

    float scale = 1.0f / this->m_layoutScale;
    this->SetPoint(point, fontString, point, x * scale, top * scale, 1);
    this->SetWidth((right - left) / this->m_layoutScale);
    this->SetHeight((top - bottom) / this->m_layoutScale);
    this->Show();
}

// OFFSET: %0x00546820
CSimpleHyperlinkButton::CSimpleHyperlinkButton(CSimpleHyperlinkedFrame* parent) : CSimpleButton(parent) {
    this->SetClickAction(0xFFFFFFFF80000000ULL);
    this->m_linkData = NULL;
    this->m_linkText = NULL;
}

// OFFSET: [%0x00546880, %0x00546910, %0x005469A0]
CSimpleHyperlinkButton::~CSimpleHyperlinkButton() {
    if (this->m_linkData) {
        SMemFree(this->m_linkData, __FILE__, __LINE__, 0);
    }

    if (this->m_linkText) {
        SMemFree(this->m_linkText, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x00546430
void CSimpleHyperlinkedFrame::RunOnHyperlinkEnterScript(const char* link, const char* text) {
    this->m_inHyperlinkScript = 1;

    if (this->HasScript(this->m_onHyperlinkEnter)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, link);
        lua_pushstring(L, text);
        this->RunScript(this->m_onHyperlinkEnter, 2, NULL);
    }

    this->m_inHyperlinkScript = 0;
}

// OFFSET: %0x00546490
void CSimpleHyperlinkedFrame::RunOnHyperlinkLeaveScript(const char* link, const char* text) {
    this->m_inHyperlinkScript = 1;

    if (this->HasScript(this->m_onHyperlinkLeave)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, link);
        lua_pushstring(L, text);
        this->RunScript(this->m_onHyperlinkLeave, 2, NULL);
    }

    this->m_inHyperlinkScript = 0;
}

// OFFSET: %0x005464F0
void CSimpleHyperlinkedFrame::RunOnHyperlinkClickScript(const char* link, const char* text, const char* btn) {
    this->m_inHyperlinkScript = 1;

    if (this->HasScript(this->m_onHyperlinkClick)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, link);
        lua_pushstring(L, text);
        lua_pushstring(L, btn);
        this->RunScript(this->m_onHyperlinkClick, 3, NULL);
    }

    this->m_inHyperlinkScript = 0;
}

// OFFSET: [%0x00546290, %0x00546420, %0x005467B0]
CSimpleHyperlinkedFrame::~CSimpleHyperlinkedFrame() {
}

// OFFSET: %0x005462B0
FrameScript_Object::ScriptIx* CSimpleHyperlinkedFrame::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onhyperlinkenter", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,link,text) %s end";
        return &this->m_onHyperlinkEnter;
    }

    if (!SStrCmp(name, "onhyperlinkleave", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,link,text) %s end";
        return &this->m_onHyperlinkLeave;
    }

    if (!SStrCmp(name, "onhyperlinkclick", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,link,text,button) %s end";
        return &this->m_onHyperlinkClick;
    }

    return NULL;
}

// OFFSET: %0x005463B0
void CSimpleHyperlinkedFrame::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onHyperlinkEnter, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onHyperlinkLeave, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onHyperlinkClick, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// OFFSET: %0x005467F0
void CSimpleHyperlinkedFrame::OnHyperlinkEnter(const char* link, const char* text) {
    this->RunOnHyperlinkEnterScript(link, text);
}

// OFFSET: %0x00546800
void CSimpleHyperlinkedFrame::OnHyperlinkLeave(const char* link, const char* text) {
    this->RunOnHyperlinkLeaveScript(link, text);
}

// OFFSET: %0x00546810
void CSimpleHyperlinkedFrame::OnHyperlinkClick(const char* link, const char* text, const char* btn) {
    this->RunOnHyperlinkClickScript(link, text, btn);
}

// OFFSET: %0x00546770
CSimpleHyperlinkedFrame::CSimpleHyperlinkedFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_inHyperlinkScript = 0;
}
