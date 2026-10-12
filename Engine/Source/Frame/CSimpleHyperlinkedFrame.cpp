#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00DD3CE8
CDataAllocator CSimpleHyperlinkButton::s_allocator(sizeof(CSimpleHyperlinkButton), 10);

// STATIC: 0x00DD3CE4/0x140CB9A90
int32_t CSimpleHyperlinkButton::s_objectType;

// STATIC: 0x00CA3E00
const char* CSimpleHyperlinkButton::s_objectTypeName = "HyperLinkButton";

// STATIC: 0x00CA3E14/0x140B415F0
int32_t CSimpleHyperlinkedFrame::s_metatable;

// PROVEN: 0aac88eb IDENTICAL/IDENTICAL
// OFFSET: 0x005331A0/0x1401855B0
bool CSimpleHyperlinkButton::IsA(int32_t type) {
    return type == CSimpleHyperlinkButton::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: f719991e IDENTICAL/NONE
// OFFSET: 0x005331E0
bool CSimpleHyperlinkButton::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleHyperlinkButton::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// PROVEN: 8a20ce3b IDENTICAL/NONE
// OFFSET: 0x00533180
const char* CSimpleHyperlinkButton::GetObjectTypeName() {
    return CSimpleHyperlinkButton::s_objectTypeName;
}

// INLINED
void CSimpleHyperlinkButton::operator delete(void* ptr) {
    if (ptr) {
        CSimpleHyperlinkButton::s_allocator.PutData(ptr, NULL, 0);
    }
}

// PROVEN: 94262c3f IDENTICAL/IDENTICAL
// OFFSET: 0x00532B00/0x140184CF0
void CSimpleHyperlinkButton::OnLayerCursorEnter(int32_t a2) {
    static_cast<CSimpleHyperlinkedFrame*>(this->m_parent)->OnHyperlinkEnter(this->m_linkData, this->m_linkText);
}

// PROVEN: c313c095 IDENTICAL/IDENTICAL
// OFFSET: 0x00532B30/0x140184D20
void CSimpleHyperlinkButton::OnLayerCursorExit(int32_t a2, int32_t a3) {
    static_cast<CSimpleHyperlinkedFrame*>(this->m_parent)->OnHyperlinkLeave(this->m_linkData, this->m_linkText);
}

// PROVEN: 717f7f3c IDENTICAL/IDENTICAL
// OFFSET: 0x00532B60/0x140184D50
void CSimpleHyperlinkButton::OnClick(const char* btn, int32_t down) {
    static_cast<CSimpleHyperlinkedFrame*>(this->m_parent)->OnHyperlinkClick(this->m_linkData, this->m_linkText, btn);
}

// OFFSET: 0x00532E70/0x140185200
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

// OFFSET: [0x00533220/0x140185650, 0x00533430/0x140185970]
CSimpleHyperlinkButton::~CSimpleHyperlinkButton() {
    if (this->m_linkData) {
        SMemFree(this->m_linkData, __FILE__, __LINE__, 0);
    }

    if (this->m_linkText) {
        SMemFree(this->m_linkText, __FILE__, __LINE__, 0);
    }
}

// PROVEN: 6db8490c IDENTICAL/NONE
// PROVEN: 6db8490c IDENTICAL/IDENTICAL
// OFFSET: 0x00532D20/0x140184FF0
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

// PROVEN: 196e62f7 IDENTICAL/IDENTICAL
// OFFSET: 0x00532D90/0x1401850A0
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

// PROVEN: bfcce0da IDENTICAL/IDENTICAL
// OFFSET: 0x00532E00/0x140185150
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

// OFFSET: [0x00533460/0x1401859C0, 0x00533560/0x140185AF0]
CSimpleHyperlinkedFrame::~CSimpleHyperlinkedFrame() {
}

// PROVEN: 9450b56b IDENTICAL/IDENTICAL
// OFFSET: 0x00532B90/0x140184D80
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

// PROVEN: e1d14464 IDENTICAL/IDENTICAL
// OFFSET: 0x00532C90/0x140184E40
void CSimpleHyperlinkedFrame::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onHyperlinkEnter, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onHyperlinkLeave, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onHyperlinkClick, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: f342770c IDENTICAL/UNPROVEN
// OFFSET: 0x00533080/0x140185470
void CSimpleHyperlinkedFrame::OnHyperlinkEnter(const char* link, const char* text) {
    this->RunOnHyperlinkEnterScript(link, text);
}

// PROVEN: db7c1134 IDENTICAL/UNPROVEN
// OFFSET: 0x00533090/0x140185480
void CSimpleHyperlinkedFrame::OnHyperlinkLeave(const char* link, const char* text) {
    this->RunOnHyperlinkLeaveScript(link, text);
}

// PROVEN: 4493a059 IDENTICAL/UNPROVEN
// OFFSET: 0x005330A0/0x140185490
void CSimpleHyperlinkedFrame::OnHyperlinkClick(const char* link, const char* text, const char* btn) {
    this->RunOnHyperlinkClickScript(link, text, btn);
}

// PROVEN: 9a157691 IDENTICAL/IDENTICAL
// OFFSET: 0x005334D0/0x140185A60
int32_t CSimpleHyperlinkedFrame::GetScriptMetaTable() {
    return CSimpleHyperlinkedFrame::s_metatable;
}

// OFFSET: 0x005334F0/0x140185A80
CSimpleHyperlinkedFrame::CSimpleHyperlinkedFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_inHyperlinkScript = 0;
    this->m_hyperlinksEnabled = 0;
    this->m_hyperlinksDirty = 0;
}

// PROVEN: 862da1bf IDENTICAL/DIFFERENT
// OFFSET: 0x005332B0/0x140185720
void CSimpleHyperlinkedFrame::ClearHyperlinks() {
    CSimpleHyperlinkButton* button = this->m_hyperlinks.Head();

    while (!TSList<CSimpleHyperlinkButton, TSGetExplicitLink<CSimpleHyperlinkButton> >::IsEnd(button)) {
        CSimpleHyperlinkButton* next = this->m_hyperlinks.RawNext(button);
        this->m_hyperlinks.UnlinkNode(button);
        delete button;
        button = next;
    }
}

// OFFSET: 0x00533340/0x1401857D0
void CSimpleHyperlinkedFrame::AddHyperlinks(CSimpleFontString* string) {
    if (!string->m_string) {
        return;
    }

    CGxString* text = TextBlockGetStringPtr(string->m_string);

    if (!text) {
        return;
    }

    const GXUFONTHYPERLINKINFO* info;
    uint32_t count = GxuFontStringHyperLinkInfo(text, info);

    for (uint32_t i = 0; i < count; i++) {
        void* data = CSimpleHyperlinkButton::s_allocator.GetData(0, typeid(CSimpleHyperlinkButton).raw_name(), SLOG_OBJECT);
        CSimpleHyperlinkButton* link = data ? new (data) CSimpleHyperlinkButton(this) : NULL;
        this->m_hyperlinks.LinkToTail(link);
        link->SetHyperlink(string, &info[i]);
    }
}
