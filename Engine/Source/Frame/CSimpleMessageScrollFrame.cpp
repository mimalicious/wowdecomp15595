#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleMessageScrollFrame.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00DD3A30
CDataAllocator CSimpleMessageScrollFrame::s_allocator(sizeof(CSimpleMessageScrollFrame), 5);

// STATIC: %0x00BB9670
int32_t CSimpleMessageScrollFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3A2C/0x140CB9760
int32_t CSimpleMessageScrollFrame::s_objectType;

// STATIC: %0x00BB8760
const char* CSimpleMessageScrollFrame::s_objectTypeName = "ScrollingMessageFrame";

// INLINED
void CSimpleMessageScrollFrame::operator delete(void* ptr) {
    if (ptr) {
        CSimpleMessageScrollFrame::s_allocator.PutData(ptr, NULL, 0);
    }
}

// INLINED
CSimpleMessageScrollFrameLine::CSimpleMessageScrollFrameLine() {
    this->m_text = NULL;
    this->m_color.b = 0xFF;
    this->m_color.g = 0xFF;
    this->m_color.r = 0xFF;
    this->m_color.a = 0xFF;
    this->m_timeLeft = 0.0f;
    this->m_fadeLeft = 0.0f;
    this->m_isVisible = 0;
    this->m_typeID = 0;
    this->m_accessID = 0;
    this->m_extraData = 0;
}

// INLINED
CSimpleMessageScrollFrameLine::~CSimpleMessageScrollFrameLine() {
    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }
}

// PROVEN: 6eca27b5 IDENTICAL/DIFFERENT
// OFFSET: 0x0051CF20/0x140169750
FrameScript_Object::ScriptIx* CSimpleMessageScrollFrame::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleHyperlinkedFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onmessagescrollchanged", STORM_MAX_STR)) {
        return &this->m_onMessageScrollChanged;
    }

    return NULL;
}

// OFFSET: 0x0051CF80
void CSimpleMessageScrollFrame::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onMessageScrollChanged, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: fc199e7a IDENTICAL/DIFFERENT
// OFFSET: 0x0051CFC0/0x140169820
int32_t CSimpleMessageScrollFrame::GetCurrentScroll() {
    if (!this->m_numMessages || this->m_currentLine == -1) {
        return 0;
    }

    return (this->m_currentLine + this->m_numMessages - this->m_currentScroll) % this->m_numMessages;
}

// OFFSET: 0x0051D200
CSimpleFontStringRecord::CSimpleFontStringRecord(CSimpleFrame* frame, uint32_t drawLayer, bool show) : CSimpleFontString(frame, drawLayer, show) {
}

// INLINED
void CSimpleFontStringRecord::operator delete(void* ptr) {
    if (ptr) {
        SMemFree(ptr, "delete", SLOG_FUNCTION, 0);
    }
}

// OFFSET: [%0x0052CF60, %0x0052CF70, %0x0052CF80, %0x0052D2D0]
CSimpleFontStringRecord::~CSimpleFontStringRecord() {
}

// PROVEN: e0929a7b IDENTICAL/NONE
// OFFSET: 0x0051D290
void CSimpleMessageScrollFrame::SetTimeVisible(float seconds) {
    for (int32_t i = 0; i < this->m_numDisplayed; i++) {
        CSimpleMessageScrollFrameLine* line = this->m_displayNodes[i].m_line;

        if (line->m_isVisible) {
            line->m_timeLeft = seconds;
        }
    }

    this->m_timeVisible = seconds;
}

// PROVEN: 93525626 IDENTICAL/NONE
// OFFSET: 0x0051D2E0
void CSimpleMessageScrollFrame::SetFadeDuration(float duration) {
    for (int32_t i = 0; i < this->m_numDisplayed; i++) {
        CSimpleMessageScrollFrameLine* line = this->m_displayNodes[i].m_line;

        if (line->m_isVisible) {
            line->m_fadeLeft = duration;
        }
    }

    this->m_fadeDuration = duration;
}

// OFFSET: 0x0051D470/0x140169D80
int32_t CSimpleMessageScrollFrame::GetNumMessages(int32_t accessID) {
    if (!accessID) {
        return this->m_numMessages;
    }

    int32_t count = 0;
    int32_t index = this->m_currentLine - this->m_numMessages + 1;

    for (int32_t i = 0; i < this->m_numMessages; i++, index++) {
        int32_t line = index < 0 ? index + this->m_maxMessages : index;

        if (this->m_lines[line].m_accessID == accessID) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x0051D4E0/0x140169DF0
void CSimpleMessageScrollFrame::UpdateNode(CSimpleMessageScrollFrameDisplayNode* node, CSimpleMessageScrollFrameLine* line, int32_t reset) {
    if (reset || !this->m_atBottom) {
        line->m_color.a = 0xFF;
        line->m_isVisible = 1;
        line->m_timeLeft = this->m_timeVisible;
        line->m_fadeLeft = this->m_fadeDuration;
    }

    float height = node->m_string->GetStringHeight();

    if (line->m_text) {
        node->m_line = line;
        node->m_string->SetText(line->m_text, 1);
        node->m_string->SetVertexColor(line->m_color);
    } else {
        node->m_string->SetText("", 1);
    }

    if (fabs(node->m_string->GetStringHeight() - height) >= 0.001) {
        node->m_string->Resize(1);
    }
}

// OFFSET: 0x0051D580/0x140169EC0
void CSimpleMessageScrollFrame::RefreshMessages() {
    for (int32_t i = this->m_numDisplayed; i; i--) {
        CSimpleMessageScrollFrameDisplayNode& node = this->m_displayNodes[i - 1];
        CSimpleMessageScrollFrameLine* line = node.m_line;
        line->m_color.a = 0xFF;
        line->m_isVisible = 1;
        line->m_timeLeft = this->m_timeVisible;
        line->m_fadeLeft = this->m_fadeDuration;
        node.m_string->SetVertexColor(line->m_color);

        CSimpleFontStringRecord* string = node.m_string;
        string->m_shown = 1;
        string->ShowThis(false);
    }

    this->m_hyperlinksDirty = 1;
}

// OFFSET: 0x0051D7A0/0x14016A1D0
CSimpleMessageScrollFrameLine* CSimpleMessageScrollFrame::GetMessageInfoByAccessID(uint32_t index, int32_t accessID) {
    if (accessID) {
        int32_t count = this->m_numMessages;

        if (count <= 0) {
            return NULL;
        }

        int32_t checked = 0;

        for (int32_t i = this->m_currentLine - count + 1;; i++) {
            int32_t lineIndex = i < 0 ? i + this->m_maxMessages : i;
            CSimpleMessageScrollFrameLine* line = &this->m_lines[lineIndex];

            if (line->m_accessID == accessID && !index--) {
                return line;
            }

            if (++checked >= count) {
                return NULL;
            }
        }
    }

    if (index >= static_cast<uint32_t>(this->m_numMessages)) {
        return NULL;
    }

    int32_t lineIndex = this->m_currentLine - this->m_numMessages + index + 1;

    if (lineIndex < 0) {
        lineIndex += this->m_maxMessages;
    }

    return &this->m_lines[lineIndex];
}

// OFFSET: 0x0051D850
CSimpleMessageScrollFrameDisplayNode::CSimpleMessageScrollFrameDisplayNode() {
    this->m_line = NULL;
    this->m_string = new (SMemAlloc(sizeof(CSimpleFontStringRecord), __FILE__, __LINE__, 0)) CSimpleFontStringRecord(NULL, 1, true);
    this->m_string->m_refcnt++;

    CSimpleFontStringRecord* string = this->m_string;
    uint32_t styleFlags = string->m_styleFlags;
    string->m_settableStyleFlags &= ~0x4000u;

    if ((styleFlags | 0x4000) != styleFlags) {
        string->m_styleFlags = styleFlags | 0x4000;

        if (string->m_string) {
            string->UpdateString();
        }
    }
}

// INLINED
CSimpleMessageScrollFrameDisplayNode::CSimpleMessageScrollFrameDisplayNode(const CSimpleMessageScrollFrameDisplayNode& source) {
    this->m_line = source.m_line;
    this->m_string = source.m_string;
    this->m_string->m_refcnt++;
}

// OFFSET: 0x0051D660
CSimpleMessageScrollFrameDisplayNode::~CSimpleMessageScrollFrameDisplayNode() {
    if (--this->m_string->m_refcnt == 0) {
        this->m_string->DeleteSelf();
    }
}

// OFFSET: 0x0051DD50/0x14016A960
void CSimpleMessageScrollFrame::SetMaxLines(int32_t maxLines) {
    this->m_lines.Clear();
    this->m_displayNodes.Clear();
    this->m_maxMessages = maxLines;
    this->m_numMessages = 0;
    this->m_numDisplayed = 0;
    this->m_atBottom = 1;
    this->m_atTop = 0;
    this->m_lines.SetCount(maxLines);
    this->m_currentLine = -1;
    this->m_currentScroll = 0;
}

// PROVEN: a526c1a4 IDENTICAL/NONE
// OFFSET: 0x0051DDD0/0x14016AA30
void CSimpleMessageScrollFrame::Clear() {
    this->m_displayNodes.Clear();
    this->m_numDisplayed = 0;
    this->m_numMessages = 0;
    this->m_hyperlinksDirty = 1;
    this->m_atBottom = 1;
    this->m_atTop = 0;
    this->m_currentLine = -1;
    this->m_currentScroll = 0;
}

// OFFSET: 0x0051D610
void CSimpleMessageScrollFrame::RefreshHyperlinks() {
    this->ClearHyperlinks();

    if (!this->m_hyperlinksEnabled) {
        return;
    }

    for (int32_t i = this->m_numDisplayed - 1; i != -1; i--) {
        CSimpleMessageScrollFrameDisplayNode& node = this->m_displayNodes[i];

        if (node.m_line->m_isVisible) {
            this->AddHyperlinks(node.m_string);
        }
    }
}

// OFFSET: [0x0051DCD0, 0x0051E1B0]
CSimpleMessageScrollFrame::~CSimpleMessageScrollFrame() {
    this->m_lines.Clear();
    this->m_displayNodes.Clear();
    this->m_numDisplayed = 0;
    delete this->m_font;

    CSimpleHyperlinkButton* button = this->m_hyperlinks.Head();

    while (!TSList<CSimpleHyperlinkButton, TSGetExplicitLink<CSimpleHyperlinkButton> >::IsEnd(button)) {
        CSimpleHyperlinkButton* next = this->m_hyperlinks.RawNext(button);
        this->m_hyperlinks.UnlinkNode(button);
        delete button;
        button = next;
    }
}

// PROVEN: cdf93c03 IDENTICAL/NONE
// OFFSET: 0x0051DC20
const char* CSimpleMessageScrollFrame::GetObjectTypeName() {
    return CSimpleMessageScrollFrame::s_objectTypeName;
}

// PROVEN: cef11aed IDENTICAL/IDENTICAL
// OFFSET: 0x0051DC30/0x14016A7D0
int32_t CSimpleMessageScrollFrame::GetScriptMetaTable() {
    return CSimpleMessageScrollFrame::s_metatable;
}

// PROVEN: 0071eba8 IDENTICAL/IDENTICAL
// OFFSET: 0x0051DC50/0x14016A7F0
bool CSimpleMessageScrollFrame::IsA(int32_t type) {
    return type == CSimpleMessageScrollFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 94984024 IDENTICAL/DIFFERENT
// OFFSET: 0x0051DC90/0x14016A7E0
bool CSimpleMessageScrollFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleMessageScrollFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x0051D330
void CSimpleMessageScrollFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (this->m_fading && this->m_atBottom) {
        for (int32_t i = 0; i < this->m_numDisplayed; i++) {
            CSimpleMessageScrollFrameDisplayNode& node = this->m_displayNodes[i];
            CSimpleMessageScrollFrameLine* line = node.m_line;

            if (!line->m_isVisible) {
                continue;
            }

            bool hide = false;

            if (line->m_timeLeft != 0.0f) {
                line->m_timeLeft -= elapsedSec;

                if (line->m_timeLeft < 0.0f) {
                    if (line->m_fadeLeft == 0.0f) {
                        hide = true;
                    } else {
                        line->m_timeLeft = 0.0f;
                    }
                }
            } else if (line->m_fadeLeft != 0.0f) {
                float fadeLeft = line->m_fadeLeft - elapsedSec;

                if (fadeLeft < 0.0f) {
                    hide = true;
                } else {
                    line->m_fadeLeft = fadeLeft;
                    line->m_color.a = static_cast<uint8_t>(fadeLeft / this->m_fadeDuration * 255.0f);
                    node.m_string->SetVertexColor(line->m_color);
                }
            }

            if (hide) {
                line->m_isVisible = 0;
                CSimpleFontStringRecord* string = node.m_string;
                string->m_shown = 0;
                string->HideThis();
                this->m_hyperlinksDirty = 1;
            }
        }
    }

    if (this->m_hyperlinksDirty) {
        this->RefreshHyperlinks();
        this->m_hyperlinksDirty = 0;
    }
}

// OFFSET: 0x0051DE30/0x14016AAB0
void CSimpleMessageScrollFrame::ScrollMessages(int32_t scroll) {
    if (this->m_currentLine == -1) {
        return;
    }

    bool changed = scroll != this->m_currentScroll;
    this->m_currentScroll = scroll;
    this->m_numDisplayed = 0;

    if (this->m_visible && !this->IsResizePending()) {
        int32_t reset = 0;

        if (this->m_currentScroll == this->m_currentLine) {
            if (!this->m_atBottom) {
                reset = 1;
            }

            this->m_atBottom = 1;
        } else {
            this->m_atBottom = 0;
        }

        float height = (this->m_messageFrameArea.maxY - this->m_messageFrameArea.minY) / this->m_layoutScale;
        int32_t count = this->m_numMessages;

        if (this->m_currentScroll != this->m_currentLine) {
            int32_t current = this->m_currentScroll;

            if (current >= this->m_currentLine) {
                current -= this->m_maxMessages;
            }

            count += current - this->m_currentLine;
        }

        int32_t index = scroll;
        int32_t i = 0;

        if (count > 0) {
            while (true) {
                bool created = false;

                if (i == this->m_displayNodes.Count()) {
                    created = true;
                    this->m_displayNodes.SetCount(i + 1);
                }

                CSimpleMessageScrollFrameLine* line = &this->m_lines[index];
                this->UpdateNode(&this->m_displayNodes[i], line, reset);
                CSimpleFontStringRecord* string = this->m_displayNodes[i].m_string;

                if (created) {
                    string->SetFrame(this, 2, 0, true);
                    string->m_fontableFlags &= ~0x4u;
                    string->SetFontObject(this->m_font);
                    string->SetWidth((this->m_messageFrameArea.maxX - this->m_messageFrameArea.minX) / this->m_layoutScale);

                    if (this->m_insertMode == 1) {
                        if (i == 0) {
                            string->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT, this->m_messageFrameInset.minX, this->m_messageFrameInset.minY, 1);
                        } else {
                            string->SetPoint(FRAMEPOINT_BOTTOMLEFT, this->m_displayNodes[i - 1].m_string, FRAMEPOINT_TOPLEFT, 0.0f, CSimpleTop::RoundToPixelHeight(string->m_spacing), 1);
                        }
                    } else {
                        if (i == 0) {
                            string->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT, this->m_messageFrameInset.minX, this->m_messageFrameInset.minY, 1);
                        } else {
                            string->SetPoint(FRAMEPOINT_TOPLEFT, this->m_displayNodes[i - 1].m_string, FRAMEPOINT_BOTTOMLEFT, 0.0f, -CSimpleTop::RoundToPixelHeight(string->m_spacing), 1);
                        }
                    }

                    string->SetTextLength(this->m_textMaxSize);
                }

                float spacing = CSimpleTop::RoundToPixelHeight(string->m_spacing);
                float lineHeight = string->GetStringHeight() + spacing;

                if (lineHeight > height && i > 0) {
                    break;
                }

                if (line->m_isVisible) {
                    string->m_shown = 1;
                    string->ShowThis(false);
                } else {
                    string->m_shown = 0;
                    string->HideThis();
                }

                height = height - lineHeight;

                if (--index < 0) {
                    index += this->m_maxMessages;
                }

                this->m_numDisplayed++;

                if (++i >= count) {
                    break;
                }
            }
        }

        this->m_atTop = i == count;

        for (uint32_t j = i; j < this->m_displayNodes.Count(); j++) {
            CSimpleFontStringRecord* string = this->m_displayNodes[j].m_string;
            string->m_shown = 0;
            string->HideThis();
        }
    }

    this->m_hyperlinksDirty = 1;

    if (changed && this->HasScript(this->m_onMessageScrollChanged)) {
        this->RunScript(this->m_onMessageScrollChanged, 0, NULL);
    }
}

// OFFSET: 0x0051DA80
CSimpleMessageScrollFrame::CSimpleMessageScrollFrame(CSimpleFrame* parent, int32_t maxLines) : CSimpleHyperlinkedFrame(parent) {
    this->m_messageFrameArea.minY = 0.0f;
    this->m_messageFrameArea.minX = 0.0f;
    this->m_messageFrameArea.maxY = 0.0f;
    this->m_messageFrameArea.maxX = 0.0f;
    this->m_messageFrameInset.minY = 0.0f;
    this->m_messageFrameInset.minX = 0.0f;
    this->m_messageFrameInset.maxY = 0.0f;
    this->m_messageFrameInset.maxX = 0.0f;
    this->m_numMessages = 0;
    this->m_maxMessages = maxLines;
    this->m_numDisplayed = 0;
    this->m_atBottom = 1;
    this->m_atTop = 0;
    this->m_textMaxSize = 0;
    this->m_font = new (SMemAlloc(sizeof(CSimpleFontedFrameFont), __FILE__, __LINE__, 0)) CSimpleFontedFrameFont(this);
    this->m_timeVisible = 10.0f;
    this->m_fadeDuration = 3.0f;
    this->m_fading = 1;
    this->m_lines.SetCount(this->m_maxMessages);
    this->m_currentScroll = 0;
    this->m_hyperlinksDirty = 0;
    this->m_currentLine = -1;
    this->m_hyperlinksEnabled = true;
    this->m_insertMode = 1;
}

// PROVEN: 2ca01559 IDENTICAL/DIFFERENT
// OFFSET: 0x0051E1E0/0x14016B000
void CSimpleMessageScrollFrame::FontUpdated(CSimpleFontedFrameFont* font, int32_t flags) {
    if (flags & 0x11) {
        for (uint32_t i = 0; i < this->m_displayNodes.Count(); i++) {
            this->m_displayNodes[i].m_string->SetFontObject(this->m_font);
        }

        this->ScrollMessages(this->m_currentScroll);
    }
}

// OFFSET: 0x0051E240/0x14016B080
void CSimpleMessageScrollFrame::RemoveLines(const int32_t* accessID, const int32_t* extraData) {
    int32_t start = this->m_currentLine - this->m_numMessages + 1;

    if (start < 0) {
        start += this->m_maxMessages;
    }

    int32_t src = start;
    int32_t dst = start;
    int32_t kept = 0;

    for (int32_t i = 0; i < this->m_numMessages; i++) {
        if ((accessID && this->m_lines[src].m_accessID != *accessID) || (extraData && this->m_lines[src].m_extraData != *extraData)) {
            if (src != dst) {
                if (this->m_lines[dst].m_text) {
                    SMemFree(this->m_lines[dst].m_text, __FILE__, __LINE__, 0);
                }

                this->m_lines[dst] = this->m_lines[src];
                this->m_lines[src].m_text = NULL;
            }

            kept++;
            dst = (dst + 1) % this->m_maxMessages;
        }

        src = (src + 1) % this->m_maxMessages;
    }

    if (kept) {
        this->m_currentLine += kept - this->m_numMessages;

        if (this->m_currentLine < 0) {
            this->m_currentLine += this->m_maxMessages;
        }
    } else {
        this->m_currentLine = -1;
    }

    this->m_numMessages = kept;

    if (kept) {
        this->ScrollMessages(this->m_currentLine);
        this->RefreshMessages();
    } else {
        this->Clear();
    }
}

// OFFSET: 0x0051E430/0x14016B330
void CSimpleMessageScrollFrame::UpdateColorByID(int32_t typeID, const CImVector& color) {
    if (!typeID) {
        return;
    }

    for (int32_t i = 0; i < this->m_numMessages; i++) {
        CSimpleMessageScrollFrameLine& line = this->m_lines[i];

        if (line.m_typeID == typeID) {
            line.m_color = color;
        }
    }

    this->ScrollMessages(this->m_currentScroll);
}

// PROVEN: 29507efb IDENTICAL/NONE
// OFFSET: 0x0051E490
void CSimpleMessageScrollFrame::OnScreenSizeChanged() {
    CSimpleFrame::OnScreenSizeChanged();
    this->ScrollMessages(this->m_currentScroll);
}

// OFFSET: 0x0051E4B0/0x14016B3C0
void CSimpleMessageScrollFrame::UpdateRect() {
    if (!(this->m_layoutFlags & 0x1)) {
        return;
    }

    float oldWidth = this->m_messageFrameArea.maxX - this->m_messageFrameArea.minX;
    float oldHeight = this->m_messageFrameArea.maxY - this->m_messageFrameArea.minY;

    this->m_messageFrameArea.minX = this->m_messageFrameInset.minX * this->m_layoutScale + this->m_rect.minX;
    this->m_messageFrameArea.maxX = this->m_rect.maxX - this->m_messageFrameInset.maxX * this->m_layoutScale;
    this->m_messageFrameArea.minY = this->m_messageFrameInset.minY * this->m_layoutScale + this->m_rect.minY;
    this->m_messageFrameArea.maxY = this->m_rect.maxY - this->m_messageFrameInset.maxY * this->m_layoutScale;

    if (fabs(oldWidth - (this->m_messageFrameArea.maxX - this->m_messageFrameArea.minX)) >= 0.0000099999997f || fabs(oldHeight - (this->m_messageFrameArea.maxY - this->m_messageFrameArea.minY)) >= 0.0000099999997f) {
        float width = (this->m_messageFrameArea.maxX - this->m_messageFrameArea.minX) / this->m_layoutScale;

        for (uint32_t i = 0; i < this->m_displayNodes.Count(); i++) {
            this->m_displayNodes[i].m_string->SetWidth(width);
        }

        this->ScrollMessages(this->m_currentScroll);
    }
}

// PROVEN: 49586a00 IDENTICAL/NONE
// OFFSET: 0x0051E600
void CSimpleMessageScrollFrame::OnLayerShow() {
    CSimpleFrame::OnLayerShow();
    this->ScrollMessages(this->m_currentScroll);
}

// PROVEN: 05db2248 IDENTICAL/DIFFERENT
// OFFSET: 0x0051E620/0x14016B570
int32_t CSimpleMessageScrollFrame::ScrollUp() {
    if (this->m_currentLine == -1) {
        return 0;
    }

    if (this->m_atTop) {
        this->RefreshMessages();
        return 0;
    }

    int32_t scroll = this->m_currentScroll - 1;

    if (scroll < 0) {
        scroll += this->m_maxMessages;
    }

    this->ScrollMessages(scroll);
    return 1;
}

// PROVEN: 0575a781 IDENTICAL/DIFFERENT
// OFFSET: 0x0051E660/0x14016B5C0
int32_t CSimpleMessageScrollFrame::ScrollDown() {
    if (this->m_currentLine == -1) {
        return 0;
    }

    if (this->m_atBottom) {
        this->RefreshMessages();
        return 0;
    }

    int32_t scroll = this->m_currentScroll + 1;

    if (scroll >= this->m_maxMessages) {
        scroll -= this->m_maxMessages;
    }

    this->ScrollMessages(scroll);
    return 1;
}

// PROVEN: ca133aa8 IDENTICAL/DIFFERENT
// OFFSET: 0x0051E6A0/0x14016B610
void CSimpleMessageScrollFrame::ScrollToTop() {
    if (this->m_currentLine == -1) {
        return;
    }

    int32_t scroll = 0;

    if (this->m_numMessages == this->m_maxMessages) {
        scroll = this->m_currentLine + 1;

        if (scroll == this->m_maxMessages) {
            scroll = 0;
        }
    }

    this->ScrollMessages(scroll);
}

// PROVEN: a750d269 IDENTICAL/DIFFERENT
// OFFSET: 0x0051E6D0/0x14016B650
void CSimpleMessageScrollFrame::ScrollToBottom() {
    if (this->m_currentLine == -1) {
        return;
    }

    if (this->m_atBottom) {
        this->RefreshMessages();
    } else {
        this->ScrollMessages(this->m_currentLine);
    }
}

// OFFSET: 0x0051E6F0/0x14016B680
void CSimpleMessageScrollFrame::SetScrollOffset(int32_t offset) {
    int32_t count = this->m_numMessages;

    if (!count || this->m_currentLine == -1) {
        return;
    }

    int32_t scroll = (this->m_currentLine + count - offset) % count;

    if (scroll <= 0) {
        scroll = 0;
    }

    if (count < scroll) {
        scroll = count;
    }

    if (scroll == this->m_currentScroll) {
        this->RefreshMessages();
    } else {
        this->ScrollMessages(scroll);
    }
}

// PROVEN: 3adc2a40 IDENTICAL/DIFFERENT
// OFFSET: 0x0051E750/0x14016B6E0
void CSimpleMessageScrollFrame::SetInsertMode(int32_t mode) {
    this->m_insertMode = mode;
    this->UpdateRect();
}

// OFFSET: 0x0051E770
void CSimpleMessageScrollFrame::AddMessage(const char* text, const CImVector& color, int32_t typeID, int32_t addToTop, int32_t accessID, int32_t extraData) {
    if (!text) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    int32_t maxMessages = this->m_maxMessages;

    if (this->m_numMessages < maxMessages) {
        this->m_numMessages++;
    }

    int32_t scrollDown = 0;
    CSimpleMessageScrollFrameLine* line;

    if (addToTop) {
        if (this->m_currentLine == -1) {
            this->m_currentLine = 0;
        }

        int32_t index = this->m_currentLine - this->m_numMessages + 1;

        if (index < 0) {
            index += maxMessages;
        }

        line = &this->m_lines[index];
    } else {
        this->m_currentLine = (this->m_currentLine + 1) % maxMessages;
        int32_t bottom = this->m_currentScroll - this->m_numDisplayed + 1;

        if (bottom < 0) {
            bottom += maxMessages;
        }

        if (this->m_currentLine == bottom) {
            scrollDown = 1;
        }

        line = &this->m_lines[this->m_currentLine];
    }

    if (!line) {
        return;
    }

    if (line->m_text) {
        SMemFree(line->m_text, __FILE__, __LINE__, 0);
    }

    line->m_text = SStrDupA(text, __FILE__, __LINE__);
    line->m_color = color;
    line->m_isVisible = 1;
    line->m_timeLeft = this->m_timeVisible;
    line->m_typeID = typeID;
    line->m_fadeLeft = this->m_fadeDuration;
    line->m_accessID = accessID;
    line->m_extraData = extraData;

    if (this->m_atBottom) {
        this->ScrollMessages(this->m_currentLine);
    } else if (scrollDown) {
        this->ScrollDown();
    }
}

// PROVEN: f8cb949f IDENTICAL/IDENTICAL
// OFFSET: 0x0051E8B0/0x14016B880
void CSimpleMessageScrollFrame::OnFrameSizeChanged(const CRect& rect) {
    CSimpleFrame::OnFrameSizeChanged(rect);
    this->UpdateRect();
}

// OFFSET: 0x0051E8D0/0x14016B8A0
void CSimpleMessageScrollFrame::PageUp() {
    for (int32_t i = this->m_numDisplayed; i; i--) {
        if (!this->ScrollUp()) {
            return;
        }
    }

    this->ScrollDown();
}

// OFFSET: 0x0051E950/0x14016B940
void CSimpleMessageScrollFrame::PageDown() {
    for (int32_t i = this->m_numDisplayed; i; i--) {
        if (!this->ScrollDown()) {
            return;
        }
    }

    this->ScrollUp();
}

// OFFSET: 0x0051E9D0
void CSimpleMessageScrollFrame::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x400)) {
        return;
    }

    ScrollingMessageFrame* frame = static_cast<ScrollingMessageFrame*>(node);

    if (frame->m_fontFlags & 0x80) {
        CSimpleFont* font = CSimpleFont::GetFont(frame->m_font.m_string, 0);

        if (font) {
            this->m_font->SetFontObject(font);
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s %s: Couldn't find font object named %s", this->GetObjectTypeName(), name ? name : "<unnamed>", frame->m_font.m_string);
        }
    }

    if (frame->m_messageFlags & 1) {
        this->m_fading = (frame->m_messageFlags >> 1) & 1;
    }

    if ((frame->m_messageFlags & 4) && frame->m_displayDuration > 0.0f) {
        this->SetTimeVisible(frame->m_displayDuration);
    }

    if ((frame->m_messageFlags & 8) && frame->m_fadeDuration > 0.0f) {
        this->SetFadeDuration(frame->m_fadeDuration);
    }

    if ((frame->m_scrollFlags & 1) && frame->m_maxLines > 0) {
        this->SetMaxLines(frame->m_maxLines);
    }

    if (frame->m_messageFlags & 0x10) {
        this->m_insertMode = frame->m_insertMode;
        this->UpdateRect();
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_FONTSTRING) {
            if (this->m_font->m_fontObject) {
                CSimpleFont* fontObject = this->m_font->m_fontObject;
                const char* name = this->GetName();
                status->Add(STATUS_WARNING, "%s: FontString element overriding font %s", name ? name : "<unnamed>", fontObject->GetName());
            }

            this->m_font->LoadXML(child, status);

            FontString* fontString = static_cast<FontString*>(child);

            if (fontString->m_stringFlags & 1) {
                this->m_textMaxSize = fontString->m_bytes;
            }
        } else if (child->m_element == XML_TEXTINSETS) {
            float left;
            float right;
            float top;
            float bottom;

            if (LoadXML_Insets(child, left, right, top, bottom, status)) {
                this->m_messageFrameInset.minX = left;
                this->m_messageFrameInset.maxX = right;
                this->m_messageFrameInset.maxY = top;
                this->m_messageFrameInset.minY = bottom;
                this->UpdateRect();
            }
        }
    }
}
