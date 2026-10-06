#include <cmath>
#include <cstddef>
#include <cstdint>
#include <malloc.h>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleHTML.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFontedFrame.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: %0x00BB98B8
int32_t CSimpleHTML::s_metatable = LUA_NOREF;

// STATIC: %0x00CD25E4
int32_t CSimpleHTML::s_objectType;

// STATIC: %0x00BB87F8
const char* CSimpleHTML::s_objectTypeName = "SimpleHTML";

// STATIC: %0x00CD25E8
CDataAllocator CSimpleHTML::s_allocator(sizeof(CSimpleHTML), 5);

// INLINED
void CSimpleHTML::operator delete(void* ptr) {
    if (ptr) {
        CSimpleHTML::s_allocator.PutData(ptr, NULL, 0);
    }
}

// OFFSET: %0x005318B0
void CSimpleHTML::FontUpdated(CSimpleFontedFrameFont* font, int32_t flags) {
    if (flags & 0x11) {
        this->m_hyperlinksDirty = 1;
    }
}

// OFFSET: %0x005318D0
void CSimpleHTML::SetHyperlinkFormat(const char* format) {
    if (this->m_hyperlinkFormat) {
        SMemFree(this->m_hyperlinkFormat, __FILE__, __LINE__, 0);
    }

    this->m_hyperlinkFormat = SStrDupA(format, __FILE__, __LINE__);
}

// OFFSET: %0x00531920
void CSimpleHTML::OnFrameSizeChanged(const CRect& rect) {
    CSimpleFrame::OnFrameSizeChanged(rect);

    if (!(fabsf((rect.maxX - rect.minX) - (this->m_rect.maxX - this->m_rect.minX)) < 0.0000099999997f) || !(fabsf((rect.maxY - rect.minY) - (this->m_rect.maxY - this->m_rect.minY)) < 0.0000099999997f)) {
        this->m_hyperlinksDirty = 1;
    }
}

// OFFSET: %0x00531990
CSimpleHTML::CSimpleHTML(CSimpleFrame* parent) : CSimpleHyperlinkedFrame(parent) {
    for (int32_t i = 0; i < NUM_HTML_TEXT_TYPES; i++) {
        void* data = SMemAlloc(sizeof(CSimpleFontedFrameFont), __FILE__, __LINE__, 0);
        this->m_fonts[i] = data ? new (data) CSimpleFontedFrameFont(this) : NULL;
    }

    this->m_hyperlinkFormat = NULL;
    this->SetHyperlinkFormat("|H%s|h%s|h");
    this->m_hyperlinksDirty = 0;
    this->m_hyperlinksEnabled = true;
}

// PROVEN: 7590d339 IDENTICAL/NONE
// OFFSET: 0x00521C40
const char* CSimpleHTML::GetObjectTypeName() {
    return CSimpleHTML::s_objectTypeName;
}

// PROVEN: 2a773a72 IDENTICAL/NONE
// OFFSET: 0x00521C50/0x14016F850
int32_t CSimpleHTML::GetScriptMetaTable() {
    return CSimpleHTML::s_metatable;
}

// PROVEN: 6673e8d4 IDENTICAL/NONE
// OFFSET: 0x00521C70/0x14016F870
bool CSimpleHTML::IsA(int32_t type) {
    return type == CSimpleHTML::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 73949285 IDENTICAL/NONE
// OFFSET: 0x00521CB0
bool CSimpleHTML::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleHTML::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: %0x00531B50
void CSimpleHTML::ParseIMG(XMLNode* node, CStatus* status) {
    bool floating = false;
    uint32_t justify = 1;
    const char* align = node->GetAttributeByName("align");

    if (align && *align) {
        StringToJustify(align, justify);

        if (justify == 1 || justify == 4) {
            floating = true;
        }
    }

    float width = 0.0f;
    float height = 0.0f;

    const char* widthAttr = node->GetAttributeByName("width");

    if (widthAttr && *widthAttr) {
        width = NDCToDDCWidth(SStrToFloat(widthAttr) / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    const char* heightAttr = node->GetAttributeByName("height");

    if (heightAttr && *heightAttr) {
        height = NDCToDDCWidth(SStrToFloat(heightAttr) / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(this, 2, 0, true) : NULL;

    texture->SetWidth(width);
    texture->SetHeight(height);

    if (justify == 1) {
        if (this->m_layoutAnchor) {
            texture->SetPoint(FRAMEPOINT_TOPLEFT, this->m_layoutAnchor, FRAMEPOINT_BOTTOMLEFT, 0.0f, this->m_layoutOffset, 1);
        } else {
            texture->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f, 1);
        }
    } else if (justify == 2) {
        if (this->m_layoutAnchor) {
            texture->SetPoint(FRAMEPOINT_TOP, this->m_layoutAnchor, FRAMEPOINT_BOTTOM, 0.0f, this->m_layoutOffset, 1);
        } else {
            texture->SetPoint(FRAMEPOINT_TOP, this, FRAMEPOINT_TOP, 0.0f, 0.0f, 1);
        }
    } else if (justify == 4) {
        if (this->m_layoutAnchor) {
            texture->SetPoint(FRAMEPOINT_TOPRIGHT, this->m_layoutAnchor, FRAMEPOINT_BOTTOMRIGHT, 0.0f, this->m_layoutOffset, 1);
        } else {
            texture->SetPoint(FRAMEPOINT_TOPRIGHT, this, FRAMEPOINT_TOPRIGHT, 0.0f, 0.0f, 1);
        }
    }

    texture->SetTexture(node->GetAttributeByName("src"), 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);

    if (!floating) {
        this->m_layoutOffset -= texture->GetHeight();
    }

    CONTENTNODE* content = this->m_content.NewNode(2, 0, 0);
    content->m_type = 2;
    content->m_region = texture;
}

// OFFSET: %0x00531E20
void CSimpleHTML::AddText(const char* text, HTML_TEXT_TYPE type, uint32_t justify) {
    void* data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
    CSimpleFontString* string = data ? new (data) CSimpleFontString(this, 2, 1) : NULL;

    if (this->m_layoutAnchor) {
        string->SetPoint(FRAMEPOINT_TOPLEFT, this->m_layoutAnchor, FRAMEPOINT_BOTTOMLEFT, 0.0f, this->m_layoutOffset, 1);
    } else {
        string->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f, 1);
    }

    string->SetWidth(this->GetWidth());

    CSimpleFont* font = this->m_fonts[type];

    if (!font->m_attributes.m_font.m_str) {
        font = this->m_fonts[HTML_TEXT_NORMAL];
    }

    string->SetFontObject(font);
    string->SetJustifyH(justify);
    string->SetNonSpaceWrap(1);
    string->m_fontableFlags &= ~0x2u;
    string->SetText(text, 1);

    this->m_layoutAnchor = string;
    this->m_layoutOffset = -CSimpleTop::RoundToPixelHeight(string->m_spacing);

    CONTENTNODE* content = this->m_content.NewNode(2, 0, 0);
    content->m_region = string;
    content->m_type = SStrStr(text, "|H") != NULL;
}

// OFFSET: [%0x00531AC0, %0x00532090, %0x00532930]
CSimpleHTML::~CSimpleHTML() {
    for (int32_t i = 0; i < NUM_HTML_TEXT_TYPES; i++) {
        if (this->m_fonts[i]) {
            delete this->m_fonts[i];
        }
    }

    CONTENTNODE* content = this->m_content.Head();

    while (!TSList<CONTENTNODE, TSGetLink<CONTENTNODE> >::IsEnd(content)) {
        content = this->m_content.DeleteNode(content);
    }

    CSimpleHyperlinkButton* button = this->m_hyperlinks.Head();

    while (!TSList<CSimpleHyperlinkButton, TSGetExplicitLink<CSimpleHyperlinkButton> >::IsEnd(button)) {
        CSimpleHyperlinkButton* next = this->m_hyperlinks.RawNext(button);
        this->m_hyperlinks.UnlinkNode(button);
        delete button;
        button = next;
    }

    SMemFree(this->m_hyperlinkFormat, __FILE__, __LINE__, 0);
}

// OFFSET: %0x00532220
void CSimpleHTML::ClearContent() {
    CONTENTNODE* content = this->m_content.Head();

    while (!TSList<CONTENTNODE, TSGetLink<CONTENTNODE> >::IsEnd(content)) {
        if (content->m_region) {
            delete content->m_region;
        }

        content = this->m_content.DeleteNode(content);
    }

    this->m_layoutAnchor = NULL;
    this->m_layoutOffset = 0.0f;

    CSimpleHyperlinkButton* button = this->m_hyperlinks.Head();

    while (!TSList<CSimpleHyperlinkButton, TSGetExplicitLink<CSimpleHyperlinkButton> >::IsEnd(button)) {
        CSimpleHyperlinkButton* next = this->m_hyperlinks.RawNext(button);
        this->m_hyperlinks.UnlinkNode(button);
        delete button;
        button = next;
    }
}

// OFFSET: %0x00532300
void CSimpleHTML::ParseP(XMLNode* node, HTML_TEXT_TYPE type, CStatus* status) {
    uint32_t justify = 1;
    const char* align = node->GetAttributeByName("align");

    if (align && *align) {
        StringToJustify(align, justify);
    }

    char* text = SStrDupA(node->m_body ? node->m_body : "", __FILE__, __LINE__);
    uint32_t offset = 0;

    for (XMLNode* child = node->m_child; child; child = child->m_next) {
        if (!SStrCmpI(child->m_name.m_str, "BR", STORM_MAX_STR)) {
            char* newText = static_cast<char*>(SMemAlloc(SStrLen(text) + 3, __FILE__, __LINE__, 0));
            SStrCopy(newText, text, offset + child->m_offset + 1);
            SStrPack(newText, "|n", STORM_MAX_STR);
            SStrPack(newText, &text[offset + child->m_offset], STORM_MAX_STR);
            SMemFree(text, __FILE__, __LINE__, 0);
            offset += 2;
            text = newText;
        } else if (!SStrCmpI(child->m_name.m_str, "A", STORM_MAX_STR)) {
            const char* linkText = child->m_body;
            const char* href = child->GetAttributeByName("href");

            if (linkText && *linkText && href && *href) {
                uint32_t size = SStrLen(href) + SStrLen(this->m_hyperlinkFormat) + SStrLen(linkText) + 1;
                char* link = static_cast<char*>(alloca(size));
                SStrPrintf(link, size, this->m_hyperlinkFormat, href, linkText);

                char* newText = static_cast<char*>(SMemAlloc(SStrLen(text) + size, __FILE__, __LINE__, 0));
                SStrCopy(newText, text, offset + child->m_offset + 1);
                SStrPack(newText, link, STORM_MAX_STR);
                SStrPack(newText, &text[offset + child->m_offset], STORM_MAX_STR);
                SMemFree(text, __FILE__, __LINE__, 0);
                text = newText;
                offset += SStrLen(link);
            }
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Unknown element type: %s", name ? name : "<unnamed>", child->m_name.m_str);
        }
    }

    bool space = false;
    bool lineStart = true;
    char* src = text;
    char* dst = text;

    while (*src) {
        switch (*src) {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
                if (!lineStart && !space) {
                    space = true;
                    *dst++ = ' ';
                }

                break;

            case '|':
                if (src[1] == 'n') {
                    if (dst > text && space) {
                        dst--;
                    }

                    *dst++ = '|';
                    *dst++ = 'n';
                    space = false;
                    lineStart = true;
                    src++;
                    break;
                }

            default:
                lineStart = false;
                space = false;
                *dst++ = *src;
                break;
        }

        src++;
    }

    if (dst > text && space) {
        dst--;
    }

    *dst = '\0';

    this->AddText(text, type, justify);
    SMemFree(text, __FILE__, __LINE__, 0);
}

// OFFSET: %0x00532760
void CSimpleHTML::RefreshHyperlinks() {
    if (!(this->m_layoutFlags & 0x1)) {
        return;
    }

    CSimpleHyperlinkButton* button = this->m_hyperlinks.Head();

    while (!TSList<CSimpleHyperlinkButton, TSGetExplicitLink<CSimpleHyperlinkButton> >::IsEnd(button)) {
        CSimpleHyperlinkButton* next = this->m_hyperlinks.RawNext(button);
        this->m_hyperlinks.UnlinkNode(button);
        delete button;
        button = next;
    }

    if (!this->m_hyperlinksEnabled) {
        return;
    }

    CONTENTNODE* content = this->m_content.Head();

    while (!TSList<CONTENTNODE, TSGetLink<CONTENTNODE> >::IsEnd(content)) {
        if (content->m_type == 1) {
            CSimpleFontString* string = static_cast<CSimpleFontString*>(content->m_region);
            const GXUFONTHYPERLINKINFO* info;
            uint32_t count = GxuFontStringHyperLinkInfo(string->m_string ? TextBlockGetStringPtr(string->m_string) : NULL, info);

            for (uint32_t i = 0; i < count; i++) {
                void* data = CSimpleHyperlinkButton::s_allocator.GetData(0, typeid(CSimpleHyperlinkButton).raw_name(), SLOG_OBJECT);
                CSimpleHyperlinkButton* link = data ? new (data) CSimpleHyperlinkButton(this) : NULL;
                this->m_hyperlinks.LinkToTail(link);
                link->SetHyperlink(string, &info[i]);
            }
        }

        content = this->m_content.RawNext(content);
    }
}

// OFFSET: %0x00532960
void CSimpleHTML::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (this->m_hyperlinksDirty) {
        this->RefreshHyperlinks();
        this->m_hyperlinksDirty = 0;
    }
}

// OFFSET: %0x005329A0
void CSimpleHTML::ParseBODY(XMLNode* node, CStatus* status) {
    for (XMLNode* child = node->m_child; child; child = child->m_next) {
        if (!SStrCmpI(child->m_name.m_str, "H1", STORM_MAX_STR)) {
            this->ParseP(child, HTML_TEXT_HEADER1, status);
        } else if (!SStrCmpI(child->m_name.m_str, "H2", STORM_MAX_STR)) {
            this->ParseP(child, HTML_TEXT_HEADER2, status);
        } else if (!SStrCmpI(child->m_name.m_str, "H3", STORM_MAX_STR)) {
            this->ParseP(child, HTML_TEXT_HEADER3, status);
        } else if (!SStrCmpI(child->m_name.m_str, "P", STORM_MAX_STR)) {
            this->ParseP(child, HTML_TEXT_NORMAL, status);
        } else if (!SStrCmpI(child->m_name.m_str, "BR", STORM_MAX_STR)) {
            this->AddText("\n", HTML_TEXT_NORMAL, 1);
        } else if (!SStrCmpI(child->m_name.m_str, "IMG", STORM_MAX_STR)) {
            this->ParseIMG(child, status);
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Unknown element type: %s", name ? name : "<unnamed>", child->m_name.m_str);
        }
    }
}

// OFFSET: %0x00532AE0
bool CSimpleHTML::SetText(const char* text, CStatus* status) {
    static CStatus s_nullStatus;

    if (!status) {
        status = &s_nullStatus;
    }

    if (!text) {
        text = "";
    }

    if (this->m_inHyperlinkScript) {
        return false;
    }

    this->ClearContent();

    bool parsed = false;
    XMLTree* tree = XMLTree_Load(text, SStrLen(text));

    if (tree) {
        XMLNode* root = XMLTree_GetRoot(tree);

        if (SStrCmpI(root->m_name.m_str, "HTML", STORM_MAX_STR)) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Unknown element type: %s (expected HTML)", name ? name : "<unnamed>", root->m_name.m_str);
        } else {
            for (XMLNode* child = root->m_child; child; child = child->m_next) {
                if (!SStrCmpI(child->m_name.m_str, "BODY", STORM_MAX_STR)) {
                    parsed = true;
                    this->ParseBODY(child, status);
                    break;
                }

                const char* name = this->GetName();
                status->Add(STATUS_WARNING, "Frame %s: Unknown element type: %s (expected BODY)", name ? name : "<unnamed>", child->m_name.m_str);
            }
        }

        XMLTree_Free(tree);
    }

    if (!parsed) {
        this->AddText(text, HTML_TEXT_NORMAL, 1);
    }

    this->m_hyperlinksDirty = 1;
    return parsed;
}

// OFFSET: %0x00532C90
void CSimpleHTML::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x1000)) {
        return;
    }

    SimpleHTML* html = static_cast<SimpleHTML*>(node);

    if (html->m_fontFlags & 0x80) {
        CSimpleFont* font = CSimpleFont::GetFont(html->m_font.m_string, 0);

        if (font) {
            for (int32_t i = 0; i < NUM_HTML_TEXT_TYPES; i++) {
                this->m_fonts[i]->SetFontObject(font);
            }
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s %s: Couldn't find font object named %s", this->GetObjectTypeName(), name ? name : "<unnamed>", html->m_font.m_string);
        }
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_FONTSTRING:
                this->m_fonts[HTML_TEXT_NORMAL]->LoadXML(child, status);
                break;

            case XML_FONTSTRINGHEADER1:
                this->m_fonts[HTML_TEXT_HEADER1]->LoadXML(child, status);
                break;

            case XML_FONTSTRINGHEADER2:
                this->m_fonts[HTML_TEXT_HEADER2]->LoadXML(child, status);
                break;

            case XML_FONTSTRINGHEADER3:
                this->m_fonts[HTML_TEXT_HEADER3]->LoadXML(child, status);
                break;
        }
    }

    void* data;

    if ((html->m_htmlFlags & 1) && SFile::ReadFile(NULL, html->m_file.m_string, &data, NULL, 1, 1, NULL)) {
        this->SetText(static_cast<const char*>(data), NULL);
        SFile::Unload(data);
    }

    if (html->m_htmlFlags & 2) {
        this->SetHyperlinkFormat(html->m_hyperlinkFormat.m_string);
    }
}
