#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFont.h"
#include <cmath>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: %0x00CD27CC
static TSHashTable<SIMPLEFONT, HASHKEY_STRI> s_fontList;

// STATIC: %0x00CD27C4
static int32_t s_fontCount;

// STATIC: %0x00BB9228
int32_t CSimpleFont::s_metatable = LUA_NOREF;

// STATIC: %0x00CD27C8
int32_t CSimpleFont::s_objectType;

// STATIC: %0x00BB8B4C
const char* CSimpleFont::s_objectTypeName = "Font";

// INLINED
SIMPLEFONT::SIMPLEFONT() {
    this->m_font = new (SMemAlloc(sizeof(CSimpleFont), __FILE__, __LINE__, 0)) CSimpleFont;
}

// INLINED
SIMPLEFONT::~SIMPLEFONT() {
    delete this->m_font;
}

// PROVEN: 0d737466 IDENTICAL/IDENTICAL
// OFFSET: 0x007ABA80/0x1404A1AD0
const char* CSimpleFont::GetName() {
    return this->m_name;
}

// PROVEN: cd230c82 IDENTICAL/IDENTICAL
// OFFSET: 0x00517000/0x140161610
bool CSimpleFont::IsA(int32_t type) {
    return type == CSimpleFont::s_objectType;
}

// PROVEN: f451c3fd IDENTICAL/IDENTICAL
// OFFSET: 0x00517020/0x140161620
bool CSimpleFont::IsA(const char* typeName) {
    return SStrCmpI(typeName, CSimpleFont::s_objectTypeName, STORM_MAX_STR) == 0;
}

// PROVEN: e129b122 IDENTICAL/IDENTICAL
// OFFSET: 0x00517040/0x140161640
const char* CSimpleFont::GetObjectTypeName() {
    return CSimpleFont::s_objectTypeName;
}

// PROVEN: 369a3955 IDENTICAL/IDENTICAL
// OFFSET: 0x00517050/0x140161650
int32_t CSimpleFont::GetScriptMetaTable() {
    return CSimpleFont::s_metatable;
}

// OFFSET: %0x005367B0
CSimpleFont* CSimpleFont::GetFont(const char* name, int32_t create) {
    SIMPLEFONT* node = s_fontList.Ptr(name);

    if (!node) {
        if (!create) {
            return NULL;
        }

        node = s_fontList.New(name, 0, 0);

        const char* fontName = node->m_key.m_str;
        CSimpleFont* font = node->m_font;

        if (font->m_name) {
            font->UnregisterScriptObject(font->m_name);
        }

        font->m_name = fontName;

        if (fontName) {
            font->RegisterScriptObject(fontName);
        }
    }

    return node->m_font;
}

// OFFSET: %0x00536070
CSimpleFont::CSimpleFont() {
    this->m_name = NULL;
    this->unk_005C = 0;
    s_fontCount++;
}

// OFFSET: [%0x00535D70, %0x00535E60, %0x00536110]
CSimpleFont::~CSimpleFont() {
    if (this->m_name) {
        this->UnregisterScriptObject(this->m_name);
    }

    this->m_name = NULL;

    CSimpleFontable* fontable;

    while ((fontable = this->m_fontableList.Head()) != NULL) {
        fontable->SetFontObject(NULL);
    }

    s_fontCount--;
}

// OFFSET: %0x00535D60
void CSimpleFont::Destroy() {
    s_fontList.Clear();
}

// OFFSET: %0x0050B4D0
void CSimpleFont::RemoveFontable(CSimpleFontable* fontable) {
    this->m_fontableList.UnlinkNode(fontable);
}

// PROVEN: b83da24c DIFFERENT/IDENTICAL
// OFFSET: 0x00525D20/0x140161640
void CSimpleFont::UpdateObjects() {
    if (!this->m_attributes.m_flags) {
        return;
    }

    for (CSimpleFontable* fontable = this->m_fontableList.Head(); !TSList<CSimpleFontable, TSGetExplicitLink<CSimpleFontable> >::IsEnd(fontable); fontable = this->m_fontableList.RawNext(fontable)) {
        fontable->FontObjectUpdated(this->m_attributes);
    }

    this->m_attributes.m_flags &= ~0x1Fu;
}

// OFFSET: %0x00535CE0
CSimpleFont& CSimpleFont::operator=(const CSimpleFont& source) {
    this->unk_005C = source.unk_005C;
    this->m_attributes = source.m_attributes;
    this->m_attributes.m_flags &= this->unk_005C;
    this->m_fontableFlags = source.m_fontableFlags;

    CSimpleFont* font = source.m_fontObject;

    for (CSimpleFont* parent = font; parent; parent = parent->m_fontObject) {
        if (parent == this) {
            font = NULL;
            break;
        }
    }

    if (font != this->m_fontObject) {
        if (this->m_fontObject) {
            this->m_fontObject->RemoveFontable(this);
        }

        this->m_fontObject = font;

        if (font) {
            font->m_fontableList.LinkToTail(this);
        }
    }

    this->UpdateObjects();

    return *this;
}

// OFFSET: %0x00526D70
void CSimpleFont::FontObjectUpdated(CSimpleFontStringAttributes& attributes) {
    attributes.Update(this->m_attributes, this->m_fontableFlags);
    this->UpdateObjects();
}

// OFFSET: %0x005368B0
void CSimpleFont::LoadXML(linkObject* node, CStatus* status) {
    if (node->m_flags & 0x2000000) {
        SIMPLEFONT* inherited = s_fontList.Ptr(node->m_inherits.m_string);

        if (inherited) {
            this->SetFontObject(inherited->m_font);
        } else {
            status->Add(STATUS_WARNING, "Couldn't find inherited font: %s", node->m_inherits.m_string);
        }
    }

    if (!(node->m_flags & 0x4000)) {
        return;
    }

    Font* fontNode = static_cast<Font*>(node);

    if (fontNode->m_fontFlags & 0x80) {
        const char* fontName = fontNode->m_font.m_string;
        SIMPLEFONT* existing = s_fontList.Ptr(fontName);
        CSimpleFont* font;

        if (existing && (font = existing->m_font) != NULL) {
            this->SetFontObject(font);
        } else {
            float fontHeight = 0.0f;
            linkObject* heightNode = node->GetChild(XML_FONTHEIGHT);

            if (!heightNode || (LoadXML_Value(heightNode, fontHeight, status), fontHeight <= 0.0f)) {
                const char* name = this->GetName();
                status->Add(STATUS_WARNING, "Font %s: Missing font height in %s element", name ? name : "<unnamed>", node->m_type.m_string);
                return;
            }

            uint32_t fontFlags = 0;

            if (fontNode->m_fontFlags & 0x100) {
                if (!SStrCmpI(fontNode->m_outline.m_string, "NORMAL", STORM_MAX_STR)) {
                    fontFlags = 1;
                } else if (!SStrCmpI(fontNode->m_outline.m_string, "THICK", STORM_MAX_STR)) {
                    fontFlags = 5;
                }
            }

            if ((fontNode->m_fontFlags & 0x200) && (fontNode->m_fontFlags & 0x400)) {
                fontFlags |= 2;
            }

            if (this->m_attributes.SetFont(fontName, fontHeight, fontFlags)) {
                this->m_fontableFlags &= ~0x1u;
            } else {
                status->Add(STATUS_WARNING, "Font %s: Unable to load font file %s", this->GetDisplayName(), fontName);
            }
        }
    }

    if (fontNode->m_fontFlags & 0x20) {
        float spacing = NDCToDDCWidth(fontNode->m_spacing / (CoordinateGetAspectCompensation() * 1024.0f));

        if (fabsf(spacing - this->m_attributes.m_spacing) >= 0.00000023841858f || !(this->m_attributes.m_flags & 0x1000)) {
            this->m_attributes.m_flags |= 0x1010;
            this->m_attributes.m_spacing = spacing;
        }

        this->m_fontableFlags &= ~0x10u;
    }

    uint32_t justify;

    if ((fontNode->m_fontFlags & 0x1) && StringToJustify(fontNode->m_justifyV.m_string, justify)) {
        this->m_attributes.SetJustifyV(justify);
        this->m_fontableFlags &= ~0x2u;
    }

    if ((fontNode->m_fontFlags & 0x2) && StringToJustify(fontNode->m_justifyH.m_string, justify)) {
        this->m_attributes.SetJustifyH(justify);
        this->m_fontableFlags &= ~0x2u;
    }

    if (fontNode->m_fontFlags & 0x4) {
        uint32_t styleFlags = (fontNode->m_fontFlags & 0x40) ? this->m_attributes.m_styleFlags | 0x1000 : this->m_attributes.m_styleFlags & ~0x1000u;

        if (styleFlags != this->m_attributes.m_styleFlags || !(this->m_attributes.m_flags & 0x200)) {
            this->m_attributes.m_flags |= 0x202;
            this->m_attributes.m_styleFlags = styleFlags;
        }
    }

    if (fontNode->m_fontFlags & 0x8) {
        uint32_t styleFlags = (fontNode->m_fontFlags & 0x10) ? this->m_attributes.m_styleFlags | 0x20000 : this->m_attributes.m_styleFlags & ~0x20000u;

        if (styleFlags != this->m_attributes.m_styleFlags || !(this->m_attributes.m_flags & 0x200)) {
            this->m_attributes.m_flags |= 0x202;
            this->m_attributes.m_styleFlags = styleFlags;
        }
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_COLOR) {
            CImVector color(0, 0, 0, 0);
            LoadXML_Color(child, color, status);

            if (*reinterpret_cast<uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&this->m_attributes.m_color) || !(this->m_attributes.m_flags & 0x400)) {
                this->m_attributes.m_color = color;
                this->m_attributes.m_flags |= 0x404;
            }

            this->m_fontableFlags &= ~0x4u;
        } else if (child->m_element == XML_SHADOW) {
            CImVector color(0x00, 0x00, 0x00, 0xFF);
            C2Vector offset;
            offset.x = 0.001f;
            offset.y = -0.001f;

            linkObject* colorNode = child->GetChild(XML_COLOR);

            if (colorNode) {
                LoadXML_Color(colorNode, color, status);
            }

            linkObject* offsetNode = child->GetChild(XML_OFFSET);

            if (offsetNode) {
                LoadXML_Dimensions(offsetNode, offset.x, offset.y, status);
            }

            if (*reinterpret_cast<uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&this->m_attributes.m_shadowColor) || offset.x != this->m_attributes.m_shadowOffset.x || offset.y != this->m_attributes.m_shadowOffset.y || !(this->m_attributes.m_flags & 0x800)) {
                this->m_attributes.m_shadowColor = color;
                this->m_attributes.m_flags |= 0x808;
                this->m_attributes.m_shadowOffset = offset;
            }

            this->m_fontableFlags &= ~0x8u;
        }
    }

    this->UpdateObjects();
}
