#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"

static bool Region_child(linkObject* node, MemBlock& block, char* name);
static bool Frame_child(linkObject* node, MemBlock& block, char* name);
static bool Region_context(linkObject* node, MemBlock& block, const char* name);

// INLINED
MemBlock::MemBlock() {
    this->m_block = NULL;
    this->m_strings = NULL;
}

// PROVEN: c9862745 IDENTICAL
// OFFSET: 0x004EC920
MemBlock::~MemBlock() {
    for (uint32_t i = 0; i < this->m_allocations.Count(); i++) {
        SMemFree(this->m_allocations[i], __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x00500950
bool MemBlock::Advance() {
    if (!this->m_block || this->m_end < this->m_cur + 0x4000) {
        void* block = SMemAlloc(0x8000, __FILE__, __LINE__, 0);
        this->m_allocations.Add(1, &block);
        this->m_block = static_cast<char*>(block);
        memset(block, 0, 0x8000);
        this->m_cur = this->m_block;
        this->m_end = this->m_block + 0x8000;
    }

    return true;
}

// OFFSET: %0x005008F0
bool MemBlock::Reserve(uint32_t bytes) {
    if (!this->m_strings || bytes > static_cast<uint32_t>(this->m_stringsEnd - this->m_strings)) {
        void* block = SMemAlloc(0x8000, __FILE__, __LINE__, 0);
        this->m_allocations.Add(1, &block);
        this->m_strings = static_cast<char*>(block);
        this->m_stringsEnd = this->m_strings + 0x8000;
    }

    return true;
}

// OFFSET: %0x004FC0E0
bool linkObject::RelocateStrings(char*& dest) {
    for (int32_t offset = this->m_strings; offset; ) {
        XMLString* string = reinterpret_cast<XMLString*>(reinterpret_cast<char*>(this) + offset);
        uint32_t length = strlen(string->m_string);
        memcpy(dest, string->m_string, length + 1);
        string->m_string = dest;
        dest += length + 1;
        offset = string->m_next;
    }

    return true;
}

// OFFSET: %0x004FC140
bool linkObject::Relocate(char*& dest) {
    char* self = dest;
    dest += this->m_size;
    this->RelocateStrings(dest);

    for (linkObject** link = &this->m_child; *link; link = &(*link)->m_next) {
        linkObject* child = *link;
        *link = reinterpret_cast<linkObject*>(dest);
        child->Relocate(dest);
    }

    memcpy(self, this, this->m_size);

    return true;
}

// OFFSET: %0x004FC1A0
void FrameXML_FreeCompiled(linkObject* node, bool force) {
    if (!node) {
        return;
    }

    node->Recycle(force);

    if (node->m_allocation) {
        SMemFree(node->m_allocation, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x004FC630
uint32_t linkObject::CalculateSize() {
    uint32_t strings = 0;

    for (int32_t offset = this->m_strings; offset; ) {
        XMLString* string = reinterpret_cast<XMLString*>(reinterpret_cast<char*>(this) + offset);
        strings += strlen(string->m_string) + 1;
        offset = string->m_next;
    }

    uint32_t children = 0;

    for (linkObject* child = this->m_child; child; child = child->m_next) {
        children += child->CalculateSize();
    }

    return strings + children + this->m_size;
}

// OFFSET: %0x004FC6D0
linkObject* linkObject::Compile(uint32_t& size) {
    size = this->CalculateSize();

    if (!size) {
        return NULL;
    }

    linkObject* result = static_cast<linkObject*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    char* dest = reinterpret_cast<char*>(result);
    this->Relocate(dest);

    if (result) {
        result->Recycle(false);

        if (result->m_allocation) {
            SMemFree(result->m_allocation, __FILE__, __LINE__, 0);
        }
    }

    result->m_allocation = result;
    result->m_next = NULL;

    return result;
}

// PROVEN: 0c3ec88c IDENTICAL
// OFFSET: 0x004EE8A0
linkObject* linkObject::GetChild(uint32_t element) {
    for (linkObject* child = this->m_child; child; child = child->m_next) {
        if (child->m_element == element) {
            return child;
        }
    }

    return NULL;
}

// OFFSET: %0x00500540
void linkObject::Recycle(bool force) {
    if ((this->m_flags & 1) && !force) {
        return;
    }

    if (this->m_flags & 0x20) {
        Function* function = static_cast<Function*>(this);

        if (function->m_ref) {
            if (*function->m_ref > 0) {
                FrameScript_ReleaseFunction(*function->m_ref);
                *function->m_ref = 0;
            }

            SMemFree(function->m_ref, __FILE__, __LINE__, 0);
        }
    } else {
        for (linkObject* child = this->m_child; child; child = child->m_next) {
            child->Recycle(force);
        }
    }
}

// OFFSET: %0x005009C0
bool linkObject::AddString(MemBlock& block, XMLString& field, const char* string) {
    uint32_t length = (strlen(string) + 4) & ~3u;
    block.Reserve(length);
    field.m_string = block.m_strings;
    memcpy(block.m_strings, string, length);
    block.m_strings += length;
    field.m_next = this->m_strings;
    this->m_strings = reinterpret_cast<char*>(&field) - reinterpret_cast<char*>(this);

    return true;
}

// OFFSET: %0x00500A20
bool linkObject::parse(MemBlock& block) {
    this->AddString(block, this->m_type, this->m_xml->m_name.m_str);

    const char* isVirtual = this->m_xml->GetAttributeByName("virtual");

    if (isVirtual) {
        this->m_flags |= 0x800000;
        this->m_flags ^= (this->m_flags ^ ((SStrCmpI(isVirtual, "true", STORM_MAX_STR) == 0) << 26)) & 0x4000000;
    }

    const char* name = this->m_xml->GetAttributeByName("name");

    if (name && *name) {
        this->AddString(block, this->m_name, name);
        this->m_flags |= 0x1000000;
    }

    const char* inherits = this->m_xml->GetAttributeByName("inherits");

    if (inherits && *inherits) {
        this->m_flags |= 0x2000000;
        this->AddString(block, this->m_inherits, inherits);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return true;
}

// OFFSET: %0x00500AE0
bool linkObject::dochildren(XMLChildFunc func, MemBlock& block) {
    linkObject** tail = &this->m_child;

    for (linkObject* child = this->m_child; child; child = child->m_next) {
        tail = &child->m_next;
    }

    XMLNode* xml = this->m_xml->m_child;

    while (xml) {
        XMLNode* next = xml->m_next;

        if (!(xml->m_flags & 1)) {
            block.Advance();
            linkObject* node = reinterpret_cast<linkObject*>(block.m_cur);
            node->m_xml = xml;

            char name[128];
            SStrCopy(name, xml->m_name.m_str, sizeof(name));
            SStrLower(name);

            if (func(node, block, name)) {
                xml->m_flags |= 1;
                *tail = node;
                tail = &node->m_next;
            }
        }

        xml = next;
    }

    *tail = NULL;

    return true;
}

// OFFSET: %0x00500D80
bool Function::parse(MemBlock& block) {
    this->m_flags |= 0x20;

    const char* function = this->m_xml->GetAttributeByName("function");

    if (function && *function) {
        this->m_functionFlags |= 1;
        this->AddString(block, this->m_function, function);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    const char* body = this->m_xml->m_body;

    if (body) {
        this->m_bodyFlags |= 1;
        this->AddString(block, this->m_body, body);
    }

    this->linkObject::parse(block);

    return true;
}

// OFFSET: %0x00500D10
static bool Script_parse(Script* node, MemBlock& block) {
    const char* file = node->m_xml->GetAttributeByName("file");

    if (file) {
        node->m_fileFlags |= 1;
        node->AddString(block, node->m_file, file);
    }

    node->m_size = block.m_cur - reinterpret_cast<char*>(node);
    node->m_size = block.m_cur - reinterpret_cast<char*>(node);

    const char* body = node->m_xml->m_body;

    if (body) {
        node->m_bodyFlags |= 1;
        node->AddString(block, node->m_body, body);
    }

    node->parse(block);

    return true;
}

// OFFSET: %0x00500BC0
bool Attribute::parse(MemBlock& block) {
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    const char* type = this->m_xml->GetAttributeByName("type");

    if (type) {
        this->m_attributeFlags |= 2;
        this->AddString(block, this->m_attributeType, type);
    }

    const char* value = this->m_xml->GetAttributeByName("value");

    if (value) {
        this->m_attributeFlags |= 1;
        this->AddString(block, this->m_value, value);
    }

    this->linkObject::parse(block);

    return true;
}

// OFFSET: %0x00500C30
bool Dimension::parseRelative(MemBlock& block) {
    const char* x = this->m_xml->GetAttributeByName("x");

    if (x && *x) {
        this->m_x = SStrToFloat(x);
        this->m_dimensionFlags |= 1;
    }

    const char* y = this->m_xml->GetAttributeByName("y");

    if (y && *y) {
        this->m_y = SStrToFloat(y);
        this->m_dimensionFlags |= 2;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->linkObject::parse(block);
}

// OFFSET: %0x00500CA0
bool Dimension::parseAbsolute(MemBlock& block) {
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseRelative(block);

    if (this->m_dimensionFlags & 1) {
        this->m_x = NDCToDDCWidth(this->m_x / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (this->m_dimensionFlags & 2) {
        this->m_y = NDCToDDCWidth(this->m_y / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    return true;
}

// OFFSET: %0x00500DF0
bool ControlPoint::parse(MemBlock& block) {
    const char* parentKey = this->m_xml->GetAttributeByName("parentKey");

    if (parentKey && *parentKey) {
        this->m_pointFlags |= 4;
        this->AddString(block, this->m_parentKey, parentKey);
    }

    const char* offsetX = this->m_xml->GetAttributeByName("offsetX");

    if (offsetX) {
        this->m_pointFlags |= 1;
        this->m_offsetX = SStrToFloat(offsetX);
    }

    const char* offsetY = this->m_xml->GetAttributeByName("offsetY");

    if (offsetY) {
        this->m_pointFlags |= 2;
        this->m_offsetY = SStrToFloat(offsetY);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->linkObject::parse(block);
}

// INLINED
static float ClampColorComponent(const char* value) {
    if (SStrToFloat(value) >= 1.0f || SStrToFloat(value) > 0.0f) {
        if (SStrToFloat(value) >= 1.0f) {
            return 1.0f;
        }

        return SStrToFloat(value);
    }

    return 0.0f;
}

// OFFSET: %0x00500E80
bool Color::parse(MemBlock& block) {
    this->m_colorFlags |= 1;
    this->m_r = 0.0f;
    this->m_g = 0.0f;
    this->m_b = 0.0f;
    this->m_a = 1.0f;

    const char* r = this->m_xml->GetAttributeByName("r");

    if (r) {
        this->m_r = ClampColorComponent(r);
    }

    const char* g = this->m_xml->GetAttributeByName("g");

    if (g) {
        this->m_g = ClampColorComponent(g);
    }

    const char* b = this->m_xml->GetAttributeByName("b");

    if (b) {
        this->m_b = ClampColorComponent(b);
    }

    const char* a = this->m_xml->GetAttributeByName("a");

    if (a) {
        this->m_a = ClampColorComponent(a);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return true;
}

// OFFSET: %0x00501080
bool Value::parseAbsolute(MemBlock& block) {
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    const char* val = this->m_xml->GetAttributeByName("val");

    if (val && *val) {
        this->m_value = SStrToFloat(val);
        this->m_valueFlags |= 1;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    if (this->m_valueFlags & 1) {
        this->m_value = NDCToDDCWidth(this->m_value / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    return true;
}

// OFFSET: %0x00501100
bool Rect::parse(MemBlock& block) {
    const char* value = this->m_xml->GetAttributeByName("ULx");

    if (value) {
        this->m_rectFlags |= 1;
        this->m_ulx = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("ULy");

    if (value) {
        this->m_rectFlags |= 2;
        this->m_uly = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("URx");

    if (value) {
        this->m_rectFlags |= 4;
        this->m_urx = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("URy");

    if (value) {
        this->m_rectFlags |= 8;
        this->m_ury = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("LLx");

    if (value) {
        this->m_rectFlags |= 0x10;
        this->m_llx = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("LLy");

    if (value) {
        this->m_rectFlags |= 0x20;
        this->m_lly = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("LRx");

    if (value) {
        this->m_rectFlags |= 0x40;
        this->m_lrx = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("LRy");

    if (value) {
        this->m_rectFlags |= 0x80;
        this->m_lry = SStrToFloat(value);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->linkObject::parse(block);
}

// OFFSET: %0x00501230
bool Insets::parseRelative(MemBlock& block) {
    const char* left = this->m_xml->GetAttributeByName("left");

    if (left && *left) {
        this->m_left = SStrToFloat(left);
        this->m_insetFlags |= 1;
    }

    const char* right = this->m_xml->GetAttributeByName("right");

    if (right && *right) {
        this->m_right = SStrToFloat(right);
        this->m_insetFlags |= 2;
    }

    const char* top = this->m_xml->GetAttributeByName("top");

    if (top && *top) {
        this->m_top = SStrToFloat(top);
        this->m_insetFlags |= 4;
    }

    const char* bottom = this->m_xml->GetAttributeByName("bottom");

    if (bottom && *bottom) {
        this->m_bottom = SStrToFloat(bottom);
        this->m_insetFlags |= 8;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return true;
}

// OFFSET: %0x005012F0
bool Insets::parseAbsolute(MemBlock& block) {
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseRelative(block);

    if (this->m_insetFlags & 1) {
        this->m_left = NDCToDDCWidth(this->m_left / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (this->m_insetFlags & 2) {
        this->m_right = NDCToDDCWidth(this->m_right / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (this->m_insetFlags & 4) {
        this->m_top = NDCToDDCWidth(this->m_top / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (this->m_insetFlags & 8) {
        this->m_bottom = NDCToDDCWidth(this->m_bottom / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    return true;
}

// OFFSET: %0x005013A0
bool BlobFrame::parse(MemBlock& block) {
    this->m_flags |= 0x40000;

    const char* fill = this->m_xml->GetAttributeByName("filltexture");

    if (fill && *fill) {
        this->m_blobFlags |= 1;
        this->AddString(block, this->m_fillTexture, fill);
    }

    const char* border = this->m_xml->GetAttributeByName("bordertexture");

    if (border && *border) {
        this->m_blobFlags |= 2;
        this->AddString(block, this->m_borderTexture, border);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);

    return true;
}

template <uint32_t ELEMENT, uint32_t SIGNATURE>
static bool AdvanceAndParseScript(linkObject* node, MemBlock& block) {
    Function* function = static_cast<Function*>(node);
    function->m_element = ELEMENT;
    block.m_cur = reinterpret_cast<char*>(function) + sizeof(Function);
    function->m_functionFlags |= SIGNATURE;
    function->m_size = block.m_cur - reinterpret_cast<char*>(function);
    function->parse(block);

    return true;
}

template <uint32_t ELEMENT>
static bool AdvanceAndParseScript(linkObject* node, MemBlock& block) {
    Function* function = static_cast<Function*>(node);
    function->m_element = ELEMENT;
    block.m_cur = reinterpret_cast<char*>(function) + sizeof(Function);
    function->m_size = sizeof(Function);

    return function->parse(block);
}

// OFFSET: %0x005015E0
static bool Origin_parse(linkObject* node, MemBlock& block) {
    Origin* origin = static_cast<Origin*>(node);
    origin->m_element = XML_ORIGIN;
    block.m_cur = reinterpret_cast<char*>(origin) + sizeof(Origin);
    origin->m_point = FRAMEPOINT_CENTER;

    const char* point = origin->m_xml->GetAttributeByName("point");

    if (point && StringToFramePoint(point, reinterpret_cast<FRAMEPOINT&>(origin->m_point))) {
        origin->m_originFlags |= 1;
    }

    origin->m_size = block.m_cur - reinterpret_cast<char*>(origin);

    return origin->parse(block);
}

// OFFSET: %0x00501640
static bool RelValue_parse(linkObject* node, MemBlock& block) {
    Value* value = static_cast<Value*>(node);
    value->m_element = XML_RELVALUE;
    block.m_cur = reinterpret_cast<char*>(value) + sizeof(Value);

    const char* val = value->m_xml->GetAttributeByName("val");

    if (val && *val) {
        value->m_value = SStrToFloat(val);
        value->m_valueFlags |= 1;
    }

    value->m_size = block.m_cur - reinterpret_cast<char*>(value);
    value->linkObject::parse(block);

    return true;
}

// OFFSET: %0x005016A0
static bool BackdropColor_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_COLOR;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);

    return static_cast<Color*>(node)->parse(block);
}

// OFFSET: %0x005016C0
static bool Include_parse(linkObject* node, MemBlock& block) {
    Include* include = static_cast<Include*>(node);
    include->m_element = XML_INCLUDE;
    block.m_cur = reinterpret_cast<char*>(include) + sizeof(Include);

    const char* file = include->m_xml->GetAttributeByName("file");

    if (file) {
        include->m_includeFlags |= 1;
        include->AddString(block, include->m_file, file);
    }

    include->m_size = block.m_cur - reinterpret_cast<char*>(include);
    include->parse(block);

    return true;
}

// OFFSET: %0x00501710
static bool BlobFrame_advance(linkObject* node, MemBlock& block) {
    BlobFrame* frame = static_cast<BlobFrame*>(node);
    frame->m_regionType = XML_BLOBFRAME;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(BlobFrame);

    return frame->parse(block);
}

// OFFSET: %0x00501740
static bool QuestPOIFrame_advance(linkObject* node, MemBlock& block) {
    BlobFrame* frame = static_cast<BlobFrame*>(node);
    frame->m_regionType = XML_QUESTPOIFRAME;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(BlobFrame);
    frame->m_flags |= 0x80000;
    frame->m_size = block.m_cur - reinterpret_cast<char*>(frame);
    frame->parse(block);

    return true;
}

// OFFSET: %0x00501780
static bool ArchaeologyDigSiteFrame_advance(linkObject* node, MemBlock& block) {
    BlobFrame* frame = static_cast<BlobFrame*>(node);
    frame->m_regionType = XML_ARCHAEOLOGYDIGSITEFRAME;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(BlobFrame);
    frame->m_flags |= 0x100000;
    frame->m_size = block.m_cur - reinterpret_cast<char*>(frame);
    frame->parse(block);

    return true;
}

// OFFSET: %0x005017C0
static bool Minimap_advance(linkObject* node, MemBlock& block) {
    Minimap* minimap = static_cast<Minimap*>(node);
    minimap->m_regionType = XML_MINIMAP;
    block.m_cur = reinterpret_cast<char*>(minimap) + sizeof(Minimap);

    const char* texture = minimap->m_xml->GetAttributeByName("minimapPlayerTexture");

    if (texture) {
        minimap->m_minimapFlags |= 1;
        minimap->AddString(block, minimap->m_playerTexture, texture);
    }

    minimap->m_size = block.m_cur - reinterpret_cast<char*>(minimap);
    minimap->Frame::parse(block);

    return true;
}

// OFFSET: %0x00501820
bool FontStyle::parse(MemBlock& block) {
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    const char* style = this->m_xml->GetAttributeByName("style");

    if (style && *style) {
        this->m_styleFlags |= 1;
        this->AddString(block, this->m_style, style);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return true;
}

// OFFSET: %0x00501880
static bool Dimensions_child(linkObject* node, MemBlock& block, char* name) {
    Dimension* dimension = static_cast<Dimension*>(node);

    if (!SStrCmp(name, "absdimension", STORM_MAX_STR)) {
        dimension->m_element = XML_ABSDIMENSION;
        block.m_cur = reinterpret_cast<char*>(dimension) + sizeof(Dimension);

        return dimension->parseAbsolute(block);
    }

    if (!SStrCmp(name, "reldimension", STORM_MAX_STR)) {
        dimension->m_element = XML_RELDIMENSION;
        block.m_cur = reinterpret_cast<char*>(dimension) + sizeof(Dimension);

        return dimension->parseRelative(block);
    }

    return false;
}

// OFFSET: %0x00501930
static bool ControlPoints_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "controlpoint", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_CONTROLPOINT;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(ControlPoint);

    return static_cast<ControlPoint*>(node)->parse(block);
}

// OFFSET: %0x00501990
static bool Val_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "absvalue", STORM_MAX_STR)) {
        node->m_element = XML_ABSVALUE;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Value);

        return static_cast<Value*>(node)->parseAbsolute(block);
    }

    if (!SStrCmp(name, "relvalue", STORM_MAX_STR)) {
        return RelValue_parse(node, block);
    }

    return false;
}

// OFFSET: %0x00501A40
static bool TexCoords_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "rect", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_RECT;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Rect);

    return static_cast<Rect*>(node)->parse(block);
}

// OFFSET: %0x00501AA0
static bool Insets_child(linkObject* node, MemBlock& block, char* name) {
    Insets* insets = static_cast<Insets*>(node);

    if (!SStrCmp(name, "absinset", STORM_MAX_STR)) {
        insets->m_element = XML_ABSINSET;
        block.m_cur = reinterpret_cast<char*>(insets) + sizeof(Insets);

        return insets->parseAbsolute(block);
    }

    if (!SStrCmp(name, "relinset", STORM_MAX_STR)) {
        insets->m_element = XML_RELINSET;
        block.m_cur = reinterpret_cast<char*>(insets) + sizeof(Insets);

        return insets->parseRelative(block);
    }

    return false;
}

// OFFSET: %0x00500390
static bool Frames_child(linkObject* node, MemBlock& block, char* name) {
    static_cast<Region*>(node)->m_element = XML_REGION;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Region);

    return Region_context(node, block, name);
}

// OFFSET: %0x00501B50
static bool ScrollFrame_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "scrollchild", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_SCROLLCHILD;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(Frames_child, block);
}

// OFFSET: %0x00501BC0
static bool Attributes_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "attribute", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_ATTRIBUTE;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Attribute);

    return static_cast<Attribute*>(node)->parse(block);
}

// OFFSET: %0x00502400
static bool HighlightColor_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_HIGHLIGHTCOLOR;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
    node->m_size = sizeof(Color);

    return static_cast<Color*>(node)->parse(block);
}

// OFFSET: %0x00502430
static bool DisabledFont_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_DISABLEDFONT;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontStyle);

    return static_cast<FontStyle*>(node)->parse(block);
}

// OFFSET: %0x00502450
static bool NormalFont_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_NORMALFONT;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontStyle);

    return static_cast<FontStyle*>(node)->parse(block);
}

// OFFSET: %0x00502470
static bool HighlightFont_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_HIGHLIGHTFONT;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontStyle);

    return static_cast<FontStyle*>(node)->parse(block);
}

// OFFSET: %0x00502490
static bool NormalColor_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_NORMALCOLOR;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
    node->m_size = sizeof(Color);

    return static_cast<Color*>(node)->parse(block);
}

// OFFSET: %0x005024C0
static bool DisabledColor_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_DISABLEDCOLOR;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
    node->m_size = sizeof(Color);

    return static_cast<Color*>(node)->parse(block);
}

// OFFSET: %0x005024F0
static bool Scripts_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "onclick", STORM_MAX_STR)) {
        node->m_element = XML_ONCLICK;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Function);
        node->m_size = sizeof(Function);

        return static_cast<Function*>(node)->parse(block);
    }

    if (!SStrCmp(name, "preclick", STORM_MAX_STR)) {
        node->m_element = XML_PRECLICK;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Function);
        node->m_size = sizeof(Function);

        return static_cast<Function*>(node)->parse(block);
    }

    if (!SStrCmp(name, "postclick", STORM_MAX_STR)) {
        node->m_element = XML_POSTCLICK;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Function);
        node->m_size = sizeof(Function);

        return static_cast<Function*>(node)->parse(block);
    }

    if (!SStrCmp(name, "ondoubleclick", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONDOUBLECLICK>(node, block);
    }

    if (!SStrCmp(name, "onload", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONLOAD>(node, block);
    }

    if (!SStrCmp(name, "onfinished", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONFINISHED>(node, block);
    }

    if (!SStrCmp(name, "onplay", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONPLAY>(node, block);
    }

    if (!SStrCmp(name, "onpause", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONPAUSE>(node, block);
    }

    if (!SStrCmp(name, "onstop", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONSTOP>(node, block);
    }

    if (!SStrCmp(name, "onsizechanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONSIZECHANGED>(node, block);
    }

    if (!SStrCmp(name, "onupdate", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONUPDATE>(node, block);
    }

    if (!SStrCmp(name, "onshow", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONSHOW>(node, block);
    }

    if (!SStrCmp(name, "onhide", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONHIDE>(node, block);
    }

    if (!SStrCmp(name, "onenter", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONENTER, 8>(node, block);
    }

    if (!SStrCmp(name, "onleave", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONLEAVE, 8>(node, block);
    }

    if (!SStrCmp(name, "onmousedown", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOUSEDOWN, 8>(node, block);
    }

    if (!SStrCmp(name, "onmouseup", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOUSEUP, 8>(node, block);
    }

    if (!SStrCmp(name, "onmousewheel", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOUSEWHEEL, 0x10>(node, block);
    }

    if (!SStrCmp(name, "ondragstart", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONDRAGSTART, 8>(node, block);
    }

    if (!SStrCmp(name, "ondragstop", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONDRAGSTOP>(node, block);
    }

    if (!SStrCmp(name, "onreceivedrag", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONRECEIVEDRAG>(node, block);
    }

    if (!SStrCmp(name, "onchar", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONCHAR, 2>(node, block);
    }

    if (!SStrCmp(name, "onhyperlinkclick", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONHYPERLINKCLICK>(node, block);
    }

    if (!SStrCmp(name, "onevent", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONEVENT>(node, block);
    }

    if (!SStrCmp(name, "onkeydown", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONKEYDOWN, 4>(node, block);
    }

    if (!SStrCmp(name, "onkeyup", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONKEYUP, 4>(node, block);
    }

    if (!SStrCmp(name, "onattributechanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONATTRIBUTECHANGED>(node, block);
    }

    if (!SStrCmp(name, "onenable", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONENABLE>(node, block);
    }

    if (!SStrCmp(name, "ondisable", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONDISABLE>(node, block);
    }

    if (!SStrCmp(name, "onenterpressed", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONENTERPRESSED>(node, block);
    }

    if (!SStrCmp(name, "onescapepressed", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONESCAPEPRESSED>(node, block);
    }

    if (!SStrCmp(name, "onscrollrangechanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONSCROLLRANGECHANGED>(node, block);
    }

    if (!SStrCmp(name, "onverticalscroll", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONVERTICALSCROLL>(node, block);
    }

    if (!SStrCmp(name, "onmousewheel", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOUSEWHEEL, 0x10>(node, block);
    }

    if (!SStrCmp(name, "onvaluechanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONVALUECHANGED>(node, block);
    }

    if (!SStrCmp(name, "ontabpressed", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTABPRESSED>(node, block);
    }

    if (!SStrCmp(name, "oneditfocuslost", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONEDITFOCUSLOST>(node, block);
    }

    if (!SStrCmp(name, "oneditfocusgained", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONEDITFOCUSGAINED>(node, block);
    }

    if (!SStrCmp(name, "ontextchanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTEXTCHANGED>(node, block);
    }

    if (!SStrCmp(name, "onupdatemodel", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONUPDATEMODEL>(node, block);
    }

    if (!SStrCmp(name, "onmovieshowsubtitle", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOVIESHOWSUBTITLE>(node, block);
    }

    if (!SStrCmp(name, "onmoviehidesubtitle", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOVIEHIDESUBTITLE>(node, block);
    }

    if (!SStrCmp(name, "onmoviefinished", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMOVIEFINISHED>(node, block);
    }

    if (!SStrCmp(name, "ontooltipsetdefaultanchor", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTOOLTIPSETDEFAULTANCHOR>(node, block);
    }

    if (!SStrCmp(name, "ontooltipaddmoney", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTOOLTIPADDMONEY>(node, block);
    }

    if (!SStrCmp(name, "ontooltipcleared", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTOOLTIPCLEARED>(node, block);
    }

    if (!SStrCmp(name, "ontooltipsetunit", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTOOLTIPSETUNIT>(node, block);
    }

    if (!SStrCmp(name, "ontooltipsetitem", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTOOLTIPSETITEM>(node, block);
    }

    if (!SStrCmp(name, "onspacepressed", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONSPACEPRESSED>(node, block);
    }

    if (!SStrCmp(name, "ontextset", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONTEXTSET>(node, block);
    }

    if (!SStrCmp(name, "oninputlanguagechanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONINPUTLANGUAGECHANGED>(node, block);
    }

    if (!SStrCmp(name, "onminmaxchanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONMINMAXCHANGED>(node, block);
    }

    if (!SStrCmp(name, "oncursorchanged", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONCURSORCHANGED>(node, block);
    }

    if (!SStrCmp(name, "onanimfinished", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONANIMFINISHED>(node, block);
    }

    if (!SStrCmp(name, "oncolorselect", STORM_MAX_STR)) {
        return AdvanceAndParseScript<XML_ONCOLORSELECT>(node, block);
    }

    return false;
}

// OFFSET: %0x00503410
bool TexCoords::parse(MemBlock& block) {
    const char* flip = this->m_xml->GetAttributeByName("flip");

    if (flip) {
        this->m_texCoordFlags |= 0x40;
        this->m_texCoordFlags ^= (this->m_texCoordFlags ^ (StringToBOOL(flip) << 7)) & 0x80;
    }

    const char* value = this->m_xml->GetAttributeByName("degrees");

    if (value) {
        this->m_texCoordFlags |= 0x10;
        this->m_degrees = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("radians");

    if (value) {
        this->m_texCoordFlags |= 0x20;
        this->m_radians = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("left");

    if (value) {
        this->m_texCoordFlags |= 1;
        this->m_left = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("right");

    if (value) {
        this->m_texCoordFlags |= 2;
        this->m_right = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("top");

    if (value) {
        this->m_texCoordFlags |= 4;
        this->m_top = SStrToFloat(value);
    }

    value = this->m_xml->GetAttributeByName("bottom");

    if (value) {
        this->m_texCoordFlags |= 8;
        this->m_bottom = SStrToFloat(value);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(TexCoords_child, block);
}

// OFFSET: %0x00503530
static bool Gradient_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "mincolor", STORM_MAX_STR)) {
        node->m_element = XML_MINCOLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
        node->m_size = sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    if (!SStrCmp(name, "maxcolor", STORM_MAX_STR)) {
        node->m_element = XML_MAXCOLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
        node->m_size = sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    return false;
}

// OFFSET: %0x005035E0
static bool Model_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "fogcolor", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_FOGCOLOR;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
    node->m_size = sizeof(Color);

    return static_cast<Color*>(node)->parse(block);
}

// OFFSET: %0x00503640
static bool ScrollFrame_advance(linkObject* node, MemBlock& block) {
    Frame* frame = static_cast<Frame*>(node);
    frame->m_regionType = XML_SCROLLFRAME;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(Frame);
    frame->m_size = sizeof(Frame);
    frame->parse(block);

    return frame->dochildren(ScrollFrame_child, block);
}

// OFFSET: %0x00503680
static bool Attributes_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_ATTRIBUTES;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(Attributes_child, block);
}

// OFFSET: %0x005036C0
static bool Path_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "controlpoints", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_CONTROLPOINTS;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(ControlPoints_child, block);
}

// OFFSET: %0x00503730
bool Gradient::parse(MemBlock& block) {
    this->m_orientation = 0;

    const char* orientation = this->m_xml->GetAttributeByName("orientation");

    if (orientation && *orientation && StringToOrientation(orientation, reinterpret_cast<ORIENTATION&>(this->m_orientation))) {
        this->m_gradientFlags |= 1;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(Gradient_child, block);
}

template <uint32_t ELEMENT>
static bool AdvanceAndParseDimension(linkObject* node, MemBlock& block) {
    Dimension* dimension = static_cast<Dimension*>(node);
    dimension->m_element = ELEMENT;
    block.m_cur = reinterpret_cast<char*>(dimension) + sizeof(Dimension);
    dimension->m_size = sizeof(Dimension);
    dimension->m_size = block.m_cur - reinterpret_cast<char*>(dimension);
    dimension->m_size = block.m_cur - reinterpret_cast<char*>(dimension);
    dimension->parseAbsolute(block);

    return dimension->dochildren(Dimensions_child, block);
}

// OFFSET: %0x00503830
static bool Offset_parse(linkObject* node, MemBlock& block) {
    Dimension* dimension = static_cast<Dimension*>(node);
    dimension->m_element = XML_OFFSET;
    block.m_cur = reinterpret_cast<char*>(dimension) + sizeof(Dimension);
    dimension->m_size = sizeof(Dimension);
    dimension->m_size = block.m_cur - reinterpret_cast<char*>(dimension);
    dimension->parseAbsolute(block);

    return dimension->dochildren(Dimensions_child, block);
}

// OFFSET: %0x00503870
static bool Scripts_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_SCRIPTS;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(Scripts_child, block);
}

template <uint32_t ELEMENT>
static bool AdvanceAndParseValue(linkObject* node, MemBlock& block) {
    Value* value = static_cast<Value*>(node);
    value->m_element = ELEMENT;
    block.m_cur = reinterpret_cast<char*>(value) + sizeof(Value);
    value->m_size = sizeof(Value);
    value->m_size = block.m_cur - reinterpret_cast<char*>(value);
    value->parseAbsolute(block);

    return value->dochildren(Val_child, block);
}

template <uint32_t ELEMENT>
static bool AdvanceAndParseInsets(linkObject* node, MemBlock& block) {
    Insets* insets = static_cast<Insets*>(node);
    insets->m_element = ELEMENT;
    block.m_cur = reinterpret_cast<char*>(insets) + sizeof(Insets);
    insets->m_size = sizeof(Insets);
    insets->m_size = block.m_cur - reinterpret_cast<char*>(insets);
    insets->parseAbsolute(block);

    return insets->dochildren(Insets_child, block);
}

// OFFSET: %0x005039F0
static bool ResizeBounds_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "minresize", STORM_MAX_STR)) {
        return AdvanceAndParseDimension<XML_MINRESIZE>(node, block);
    }

    if (!SStrCmp(name, "maxresize", STORM_MAX_STR)) {
        return AdvanceAndParseDimension<XML_MAXRESIZE>(node, block);
    }

    return false;
}

// OFFSET: %0x00503A90
static bool Anchor_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "offset", STORM_MAX_STR)) {
        return false;
    }

    Dimension* dimension = static_cast<Dimension*>(node);
    dimension->m_element = XML_OFFSET;
    block.m_cur = reinterpret_cast<char*>(dimension) + sizeof(Dimension);
    dimension->m_size = sizeof(Dimension);
    dimension->m_size = block.m_cur - reinterpret_cast<char*>(dimension);
    dimension->parseAbsolute(block);

    return dimension->dochildren(Dimensions_child, block);
}

// OFFSET: %0x00503B10
static bool Animation_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "scripts", STORM_MAX_STR)) {
        node->m_element = XML_SCRIPTS;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
        node->m_size = sizeof(linkObject);
        node->parse(block);

        return node->dochildren(Scripts_child, block);
    }

    if (!SStrCmp(name, "origin", STORM_MAX_STR)) {
        return Origin_parse(node, block);
    }

    return false;
}

// OFFSET: %0x00503BD0
static bool Shadow_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "color", STORM_MAX_STR)) {
        node->m_element = XML_COLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    if (!SStrCmp(name, "offset", STORM_MAX_STR)) {
        return Offset_parse(node, block);
    }

    return false;
}

// OFFSET: %0x00503C80
static bool Texture_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "texcoords", STORM_MAX_STR)) {
        node->m_element = XML_TEXCOORDS;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(TexCoords);

        return static_cast<TexCoords*>(node)->parse(block);
    }

    if (!SStrCmp(name, "gradient", STORM_MAX_STR)) {
        node->m_element = XML_GRADIENT;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Gradient);

        return static_cast<Gradient*>(node)->parse(block);
    }

    return false;
}

// OFFSET: %0x00503D30
static bool Backdrop_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "edgesize", STORM_MAX_STR)) {
        Value* value = static_cast<Value*>(node);
        value->m_element = XML_EDGESIZE;
        block.m_cur = reinterpret_cast<char*>(value) + sizeof(Value);
        value->m_size = sizeof(Value);
        value->m_size = block.m_cur - reinterpret_cast<char*>(value);
        value->parseAbsolute(block);

        return value->dochildren(Val_child, block);
    }

    if (!SStrCmp(name, "tilesize", STORM_MAX_STR)) {
        return AdvanceAndParseValue<XML_TILESIZE>(node, block);
    }

    if (!SStrCmp(name, "bordercolor", STORM_MAX_STR)) {
        node->m_element = XML_BORDERCOLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
        node->m_size = sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    if (!SStrCmp(name, "color", STORM_MAX_STR)) {
        return BackdropColor_parse(node, block);
    }

    if (!SStrCmp(name, "backgroundinsets", STORM_MAX_STR)) {
        return AdvanceAndParseInsets<XML_BACKGROUNDINSETS>(node, block);
    }

    return false;
}

// OFFSET: %0x00503ED0
static bool Path_parse(linkObject* node, MemBlock& block) {
    PathAnimation* path = static_cast<PathAnimation*>(node);
    path->m_element = XML_PATH;
    block.m_cur = reinterpret_cast<char*>(path) + sizeof(PathAnimation);

    const char* curve = path->m_xml->GetAttributeByName("curve");

    if (curve && *curve) {
        path->m_pathFlags |= 1;
        path->AddString(block, path->m_curve, curve);
    }

    path->m_size = block.m_cur - reinterpret_cast<char*>(path);

    return path->dochildren(Path_child, block);
}

// OFFSET: %0x00503F30
bool Anchor::parse(MemBlock& block) {
    const char* relativeTo = this->m_xml->GetAttributeByName("relativeTo");

    if (relativeTo) {
        this->m_anchorFlags |= 4;
        this->AddString(block, this->m_relativeTo, relativeTo);
    }

    const char* point = this->m_xml->GetAttributeByName("point");

    if (point && StringToFramePoint(point, reinterpret_cast<FRAMEPOINT&>(this->m_point))) {
        this->m_anchorFlags |= 1;
    }

    const char* relativePoint = this->m_xml->GetAttributeByName("relativePoint");

    if (relativePoint && StringToFramePoint(relativePoint, reinterpret_cast<FRAMEPOINT&>(this->m_relativePoint))) {
        this->m_anchorFlags |= 2;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseAbsolute(block);
    this->dochildren(Dimensions_child, block);

    return this->dochildren(Anchor_child, block);
}

// OFFSET: %0x00503FE0
bool Animation::parse(MemBlock& block) {
    this->m_flags |= 0x2000;

    const char* target = this->m_xml->GetAttributeByName("target");

    if (target) {
        this->m_animFlags2 |= 2;
        this->AddString(block, this->m_target, target);
    }

    const char* parentKey = this->m_xml->GetAttributeByName("parentKey");

    if (parentKey && *parentKey) {
        this->m_animFlags |= 1;
        this->AddString(block, this->m_parentKey, parentKey);
    }

    const char* startDelay = this->m_xml->GetAttributeByName("startDelay");

    if (startDelay && *startDelay) {
        this->m_animFlags |= 2;
        this->m_startDelay = SStrToFloat(startDelay);
    }

    const char* endDelay = this->m_xml->GetAttributeByName("endDelay");

    if (endDelay && *endDelay) {
        this->m_animFlags |= 4;
        this->m_endDelay = SStrToFloat(endDelay);
    }

    const char* duration = this->m_xml->GetAttributeByName("duration");

    if (duration && *duration) {
        this->m_animFlags |= 8;
        this->m_duration = SStrToFloat(duration);
    }

    const char* maxFramerate = this->m_xml->GetAttributeByName("maxFramerate");

    if (maxFramerate && *maxFramerate) {
        this->m_animFlags |= 0x10;
        this->m_maxFramerate = SStrToFloat(maxFramerate);
    }

    const char* order = this->m_xml->GetAttributeByName("order");

    if (order && *order) {
        this->m_animFlags2 |= 1;
        this->m_order = SStrToInt(order);
    }

    const char* smoothing = this->m_xml->GetAttributeByName("smoothing");

    if (smoothing && *smoothing) {
        this->m_hasSmoothing = 1;
        this->AddString(block, this->m_smoothing, smoothing);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(Animation_child, block);
}

// OFFSET: %0x00504150
bool VectorAnimation::parseTranslation(MemBlock& block) {
    const char* x = this->m_xml->GetAttributeByName("offsetX");

    if (x) {
        this->m_vectorFlags |= 1;
        this->m_x = SStrToFloat(x);
    }

    const char* y = this->m_xml->GetAttributeByName("offsetY");

    if (y) {
        this->m_vectorFlags |= 2;
        this->m_y = SStrToFloat(y);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->Animation::parse(block);
}

// OFFSET: %0x005041B0
bool VectorAnimation::parseRotation(MemBlock& block) {
    const char* degrees = this->m_xml->GetAttributeByName("degrees");

    if (degrees) {
        this->m_vectorFlags |= 1;
        this->m_x = SStrToFloat(degrees);
    }

    const char* radians = this->m_xml->GetAttributeByName("radians");

    if (radians) {
        this->m_vectorFlags |= 2;
        this->m_y = SStrToFloat(radians);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->Animation::parse(block);
}

// OFFSET: %0x00504210
bool VectorAnimation::parseScale(MemBlock& block) {
    const char* x = this->m_xml->GetAttributeByName("scaleX");

    if (x) {
        this->m_vectorFlags |= 1;
        this->m_x = SStrToFloat(x);
    }

    const char* y = this->m_xml->GetAttributeByName("scaleY");

    if (y) {
        this->m_vectorFlags |= 2;
        this->m_y = SStrToFloat(y);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->Animation::parse(block);
}

// OFFSET: %0x00504270
bool Backdrop::parse(MemBlock& block) {
    const char* bgFile = this->m_xml->GetAttributeByName("bgFile");

    if (bgFile) {
        this->m_backdropFlags |= 2;
        this->AddString(block, this->m_bgFile, bgFile);
    }

    const char* edgeFile = this->m_xml->GetAttributeByName("edgeFile");

    if (edgeFile) {
        this->m_backdropFlags |= 4;
        this->AddString(block, this->m_edgeFile, edgeFile);
    }

    const char* tile = this->m_xml->GetAttributeByName("tile");

    if (tile && *tile) {
        this->m_backdropFlags |= 8;
        this->m_backdropFlags ^= (static_cast<uint8_t>(StringToBOOL(tile)) ^ static_cast<uint8_t>(this->m_backdropFlags)) & 1;
    }

    this->m_alphaMode = GxBlend_Alpha;

    const char* alphaMode = this->m_xml->GetAttributeByName("alphaMode");

    if (alphaMode) {
        this->m_backdropFlags |= 0x10;

        if (*alphaMode) {
            StringToBlendMode(alphaMode, reinterpret_cast<EGxBlend&>(this->m_alphaMode));
        }
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(Backdrop_child, block);
}

// OFFSET: %0x00504340
static bool Alpha_parse(linkObject* node, MemBlock& block) {
    AlphaAnimation* alpha = static_cast<AlphaAnimation*>(node);
    alpha->m_element = XML_ALPHA;
    block.m_cur = reinterpret_cast<char*>(alpha) + sizeof(AlphaAnimation);

    const char* change = alpha->m_xml->GetAttributeByName("change");

    if (change) {
        alpha->m_alphaFlags |= 1;
        alpha->m_change = SStrToFloat(change);
    }

    alpha->m_size = block.m_cur - reinterpret_cast<char*>(alpha);

    return alpha->Animation::parse(block);
}

// OFFSET: %0x00504390
static bool Translation_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_TRANSLATION;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(VectorAnimation);

    return static_cast<VectorAnimation*>(node)->parseTranslation(block);
}

// OFFSET: %0x005043B0
static bool Rotation_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_ROTATION;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(VectorAnimation);

    return static_cast<VectorAnimation*>(node)->parseRotation(block);
}

// OFFSET: %0x005043D0
static bool Scale_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_SCALE;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(VectorAnimation);

    return static_cast<VectorAnimation*>(node)->parseScale(block);
}

// OFFSET: %0x005043F0
static bool Anchors_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "anchor", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_ANCHOR;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Anchor);

    return static_cast<Anchor*>(node)->parse(block);
}

// OFFSET: %0x00504450
static bool AnimationGroup_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "animation", STORM_MAX_STR)) {
        node->m_element = XML_ANIMATION;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Animation);

        return static_cast<Animation*>(node)->parse(block);
    }

    if (!SStrCmp(name, "scripts", STORM_MAX_STR)) {
        node->m_element = XML_SCRIPTS;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
        node->m_size = sizeof(linkObject);
        node->parse(block);

        return node->dochildren(Scripts_child, block);
    }

    if (!SStrCmp(name, "alpha", STORM_MAX_STR)) {
        return Alpha_parse(node, block);
    }

    if (!SStrCmp(name, "translation", STORM_MAX_STR)) {
        return Translation_parse(node, block);
    }

    if (!SStrCmp(name, "rotation", STORM_MAX_STR)) {
        return Rotation_parse(node, block);
    }

    if (!SStrCmp(name, "scale", STORM_MAX_STR)) {
        return Scale_parse(node, block);
    }

    if (!SStrCmp(name, "path", STORM_MAX_STR)) {
        return Path_parse(node, block);
    }

    return false;
}

// OFFSET: %0x00504670
static bool Font_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "color", STORM_MAX_STR)) {
        node->m_element = XML_COLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    if (!SStrCmp(name, "shadow", STORM_MAX_STR)) {
        node->m_element = XML_SHADOW;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
        node->m_size = sizeof(linkObject);
        node->parse(block);

        return node->dochildren(Shadow_child, block);
    }

    if (!SStrCmp(name, "fontheight", STORM_MAX_STR)) {
        return AdvanceAndParseValue<XML_FONTHEIGHT>(node, block);
    }

    return false;
}

// OFFSET: %0x00504780
bool AnimationGroup::parse(MemBlock& block) {
    const char* parentKey = this->m_xml->GetAttributeByName("parentKey");

    if (parentKey && *parentKey) {
        this->m_groupFlags |= 4;
        this->AddString(block, this->m_parentKey, parentKey);
    }

    const char* looping = this->m_xml->GetAttributeByName("looping");

    if (looping && *looping) {
        this->m_groupFlags |= 8;
        this->AddString(block, this->m_looping, looping);
    }

    const char* x = this->m_xml->GetAttributeByName("initialOffsetX");

    if (x) {
        this->m_groupFlags |= 1;
        this->m_initialOffsetX = SStrToFloat(x);
    }

    const char* y = this->m_xml->GetAttributeByName("initialOffsetY");

    if (y) {
        this->m_groupFlags |= 2;
        this->m_initialOffsetY = SStrToFloat(y);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(AnimationGroup_child, block);
}

// OFFSET: %0x00504840
bool Font::parseFont(MemBlock& block) {
    this->m_flags |= 0x4000;

    const char* font = this->m_xml->GetAttributeByName("font");

    if (font) {
        this->m_fontFlags |= 0x80;
        this->AddString(block, this->m_font, font);
    }

    const char* justifyV = this->m_xml->GetAttributeByName("justifyV");

    if (justifyV) {
        this->m_fontFlags |= 1;
        this->AddString(block, this->m_justifyV, justifyV);
    }

    const char* justifyH = this->m_xml->GetAttributeByName("justifyH");

    if (justifyH) {
        this->m_fontFlags |= 2;
        this->AddString(block, this->m_justifyH, justifyH);
    }

    const char* outline = this->m_xml->GetAttributeByName("outline");

    if (outline) {
        this->m_fontFlags |= 0x100;
        this->AddString(block, this->m_outline, outline);
    }

    const char* monochrome = this->m_xml->GetAttributeByName("monochrome");

    if (monochrome && *monochrome) {
        this->m_fontFlags |= 0x200;
        this->m_fontFlags ^= (this->m_fontFlags ^ (StringToBOOL(monochrome) << 10)) & 0x400;
    }

    const char* indented = this->m_xml->GetAttributeByName("indented");

    if (indented && *indented) {
        this->m_fontFlags |= 8;
        this->m_fontFlags ^= (this->m_fontFlags ^ (16 * StringToBOOL(indented))) & 0x10;
    }

    const char* nonSpaceWrap = this->m_xml->GetAttributeByName("nonspacewrap");

    if (nonSpaceWrap && *nonSpaceWrap) {
        this->m_fontFlags |= 4;
        this->m_fontFlags ^= (this->m_fontFlags ^ (StringToBOOL(nonSpaceWrap) << 6)) & 0x40;
    }

    const char* spacing = this->m_xml->GetAttributeByName("spacing");

    if (spacing && *spacing) {
        this->m_fontFlags |= 0x20;
        this->m_spacing = SStrToFloat(spacing);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(Font_child, block);
}

// OFFSET: %0x005049C0
bool Region::parseLayout(MemBlock& block) {
    const char* preload = this->m_xml->GetAttributeByName("preload");

    if (preload) {
        this->m_layoutFlags |= 4;
        this->m_layoutFlags ^= (this->m_layoutFlags ^ (8 * (SStrCmpI(preload, "true", STORM_MAX_STR) == 0))) & 8;
    }

    const char* setAllPoints = this->m_xml->GetAttributeByName("setAllPoints");

    if (setAllPoints && *setAllPoints) {
        this->m_layoutFlags = this->m_layoutFlags ^ (this->m_layoutFlags ^ (2 * StringToBOOL(setAllPoints))) & 2 | 1;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseFont(block);

    return true;
}

// OFFSET: %0x00504A50
static bool Animations_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "animationgroup", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_ANIMATIONGROUP;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(AnimationGroup);

    return static_cast<AnimationGroup*>(node)->parse(block);
}

// OFFSET: %0x00504AB0
static bool Animations_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_ANIMATIONS;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(Animations_child, block);
}

// OFFSET: %0x00504AF0
static bool Region_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "size", STORM_MAX_STR)) {
        return AdvanceAndParseDimension<XML_SIZE>(node, block);
    }

    if (!SStrCmp(name, "anchors", STORM_MAX_STR)) {
        node->m_element = XML_ANCHORS;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
        node->m_size = sizeof(linkObject);
        node->parse(block);

        return node->dochildren(Anchors_child, block);
    }

    if (!SStrCmp(name, "animations", STORM_MAX_STR)) {
        return Animations_parse(node, block);
    }

    return false;
}

// OFFSET: %0x00504C20
bool Region::parse(MemBlock& block) {
    this->m_flags |= 0x10;

    const char* alpha = this->m_xml->GetAttributeByName("alpha");

    if (alpha) {
        this->m_regionFlags |= 1;
        this->m_alpha = SStrToFloat(alpha);
    }

    const char* hidden = this->m_xml->GetAttributeByName("hidden");

    if (hidden) {
        this->m_regionFlags |= 2;
        this->m_regionFlags ^= (this->m_regionFlags ^ (8 * (SStrCmpI(hidden, "true", STORM_MAX_STR) == 0))) & 8;
    }

    const char* parentKey = this->m_xml->GetAttributeByName("parentKey");

    if (parentKey && *parentKey) {
        this->m_regionFlags |= 4;
        this->AddString(block, this->m_parentKey, parentKey);
    }

    const char* forceAlpha = this->m_xml->GetAttributeByName("forceAlpha");

    if (forceAlpha && *forceAlpha) {
        this->m_regionFlags |= 0x10;
        this->m_regionFlags ^= (this->m_regionFlags ^ (32 * StringToBOOL(forceAlpha))) & 0x20;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseLayout(block);

    return this->dochildren(Region_child, block);
}

// OFFSET: %0x00504D10
bool Texture::parse(MemBlock& block) {
    this->m_textureFlags &= 0xFFFE9FFF;
    this->m_flags |= 0x8000;

    const char* value = this->m_xml->GetAttributeByName("desaturated");

    if (value) {
        this->m_textureFlags |= 0x20000;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 18)) & 0x40000;
    }

    value = this->m_xml->GetAttributeByName("noanimalpha");

    if (value && *value) {
        this->m_textureFlags |= 0x8000;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 16)) & 0x10000;
    }

    value = this->m_xml->GetAttributeByName("nounload");

    if (value && *value) {
        this->m_textureFlags |= 0x800;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 13)) & 0x2000;
    }

    value = this->m_xml->GetAttributeByName("nolazyload");

    if (value && *value) {
        this->m_textureFlags |= 0x1000;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 14)) & 0x4000;
    }

    value = this->m_xml->GetAttributeByName("horizTile");

    if (value && *value) {
        this->m_textureFlags |= 0x80;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 8)) & 0x100;
    }

    value = this->m_xml->GetAttributeByName("vertTile");

    if (value && *value) {
        this->m_textureFlags |= 0x200;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 10)) & 0x400;
    }

    value = this->m_xml->GetAttributeByName("nonBlocking");

    if (value && *value) {
        this->m_textureFlags |= 0x20;
        this->m_textureFlags ^= (this->m_textureFlags ^ (StringToBOOL(value) << 6)) & 0x40;
    }

    value = this->m_xml->GetAttributeByName("hidden");

    if (value && *value) {
        this->m_textureFlags |= 8;
        this->m_textureFlags ^= (this->m_textureFlags ^ (16 * StringToBOOL(value))) & 0x10;
    }

    const char* file = this->m_xml->GetAttributeByName("file");

    if (file) {
        this->m_textureFlags |= 2;
        this->AddString(block, this->m_file, file);
    }

    const char* mask = this->m_xml->GetAttributeByName("mask");

    if (mask) {
        this->m_textureFlags |= 1;
        this->AddString(block, this->m_mask, mask);
    }

    this->m_alphaMode = GxBlend_Alpha;

    const char* alphaMode = this->m_xml->GetAttributeByName("alphaMode");

    if (alphaMode) {
        this->m_textureFlags |= 4;

        if (*alphaMode) {
            StringToBlendMode(alphaMode, reinterpret_cast<EGxBlend&>(this->m_alphaMode));
        }
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Region::parse(block);

    return this->dochildren(Texture_child, block);
}

// OFFSET: %0x00504F60
bool FontString::parse(MemBlock& block) {
    this->m_flags |= 0x10000;

    const char* degrees = this->m_xml->GetAttributeByName("degrees");

    if (degrees && *degrees) {
        this->m_stringFlags |= 0x20;
        this->m_rotation = SStrToFloat(degrees) * 0.017453292f;
    }

    const char* radians = this->m_xml->GetAttributeByName("radians");

    if (radians && *radians) {
        this->m_stringFlags |= 0x20;
        this->m_rotation = SStrToFloat(radians);
    }

    const char* scale = this->m_xml->GetAttributeByName("scale");

    if (scale && *scale) {
        this->m_stringFlags |= 0x40;
        this->m_scale = SStrToFloat(scale);
    }

    const char* wordwrap = this->m_xml->GetAttributeByName("wordwrap");

    if (wordwrap && *wordwrap) {
        this->m_stringFlags |= 8;
        this->m_stringFlags ^= (this->m_stringFlags ^ (16 * StringToBOOL(wordwrap))) & 0x10;
    }

    const char* text = this->m_xml->GetAttributeByName("text");

    if (text && *text) {
        this->m_stringFlags |= 4;
        this->AddString(block, this->m_text, text);
    }

    const char* bytes = this->m_xml->GetAttributeByName("bytes");

    if (bytes) {
        this->m_stringFlags |= 1;
        this->m_bytes = SStrToInt(bytes);
    }

    const char* maxLines = this->m_xml->GetAttributeByName("maxLines");

    if (maxLines && *maxLines) {
        int32_t lines = SStrToInt(maxLines);
        this->m_stringFlags |= 2;
        this->m_maxLines = lines;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->Region::parse(block);
}

// OFFSET: %0x005050B0
static bool LayerFontString_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_FONTSTRING;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontString);

    return static_cast<FontString*>(node)->parse(block);
}

// OFFSET: %0x005050E0
static bool Texture_advance(linkObject* node, MemBlock& block) {
    Texture* texture = static_cast<Texture*>(node);
    texture->m_regionType = XML_TEXTURE;
    block.m_cur = reinterpret_cast<char*>(texture) + sizeof(Texture);

    return texture->parse(block);
}

// OFFSET: %0x00505110
static bool FontString_advance(linkObject* node, MemBlock& block) {
    FontString* string = static_cast<FontString*>(node);
    string->m_regionType = XML_FONTSTRING;
    block.m_cur = reinterpret_cast<char*>(string) + sizeof(FontString);

    return string->parse(block);
}

// OFFSET: %0x00505140
static bool EditBox_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "textinsets", STORM_MAX_STR)) {
        Insets* insets = static_cast<Insets*>(node);
        insets->m_element = XML_TEXTINSETS;
        block.m_cur = reinterpret_cast<char*>(insets) + sizeof(Insets);
        insets->m_size = sizeof(Insets);
        insets->m_size = block.m_cur - reinterpret_cast<char*>(insets);
        insets->parseAbsolute(block);

        return insets->dochildren(Insets_child, block);
    }

    if (!SStrCmp(name, "highlightcolor", STORM_MAX_STR)) {
        node->m_element = XML_HIGHLIGHTCOLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
        node->m_size = sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    if (!SStrCmp(name, "fontstring", STORM_MAX_STR)) {
        node->m_element = XML_FONTSTRING;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontString);

        return static_cast<FontString*>(node)->parse(block);
    }

    return false;
}

// OFFSET: %0x00505260
static bool ColorValueThumbTexture_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_COLORVALUETHUMBTEXTURE;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
    node->m_size = sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x00505290
static bool HighlightTexture_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_HIGHLIGHTTEXTURE;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
    node->m_size = sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x005052C0
static bool ButtonText_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_BUTTONTEXT;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontString);
    node->m_size = sizeof(FontString);

    return static_cast<FontString*>(node)->parse(block);
}

// OFFSET: %0x005052F0
static bool FontStrings_child(linkObject* node, MemBlock& block, char* name) {
    uint32_t element;

    if (!SStrCmp(name, "fontstringheader1", STORM_MAX_STR)) {
        element = XML_FONTSTRINGHEADER1;
    } else if (!SStrCmp(name, "fontstringheader2", STORM_MAX_STR)) {
        element = XML_FONTSTRINGHEADER2;
    } else if (!SStrCmp(name, "fontstringheader3", STORM_MAX_STR)) {
        element = XML_FONTSTRINGHEADER3;
    } else {
        if (!SStrCmp(name, "fontstring", STORM_MAX_STR)) {
            return LayerFontString_parse(node, block);
        }

        return false;
    }

    node->m_element = element;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(FontString);
    node->m_size = sizeof(FontString);

    return static_cast<FontString*>(node)->parse(block);
}

// OFFSET: %0x00505420
static bool Layer_child(linkObject* node, MemBlock& block, char* name) {
    if (FontStrings_child(node, block, name)) {
        return true;
    }

    if (SStrCmp(name, "texture", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_TEXTURE;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x005054A0
static bool ColorSelect_child(linkObject* node, MemBlock& block, char* name) {
    uint32_t element;

    if (!SStrCmp(name, "colorwheeltexture", STORM_MAX_STR)) {
        element = XML_COLORWHEELTEXTURE;
    } else if (!SStrCmp(name, "colorwheelthumbtexture", STORM_MAX_STR)) {
        element = XML_COLORWHEELTHUMBTEXTURE;
    } else if (!SStrCmp(name, "colorvaluetexture", STORM_MAX_STR)) {
        element = XML_COLORVALUETEXTURE;
    } else {
        if (!SStrCmp(name, "colorvaluethumbtexture", STORM_MAX_STR)) {
            return ColorValueThumbTexture_parse(node, block);
        }

        return false;
    }

    node->m_element = element;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
    node->m_size = sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x005055D0
static bool Slider_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "thumbtexture", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_THUMBTEXTURE;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
    node->m_size = sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x00505630
static bool StatusBar_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "bartexture", STORM_MAX_STR)) {
        node->m_element = XML_BARTEXTURE;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
        node->m_size = sizeof(Texture);

        return static_cast<Texture*>(node)->parse(block);
    }

    if (!SStrCmp(name, "barcolor", STORM_MAX_STR)) {
        node->m_element = XML_BARCOLOR;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Color);
        node->m_size = sizeof(Color);

        return static_cast<Color*>(node)->parse(block);
    }

    return false;
}

// OFFSET: %0x005056F0
static bool FontFrame_child(linkObject* node, MemBlock& block, char* name) {
    return FontStrings_child(node, block, name);
}

// OFFSET: %0x00505700
static bool Button_child(linkObject* node, MemBlock& block, char* name) {
    uint32_t element;

    if (!SStrCmp(name, "normaltexture", STORM_MAX_STR)) {
        element = XML_NORMALTEXTURE;
    } else if (!SStrCmp(name, "pushedtexture", STORM_MAX_STR)) {
        element = XML_PUSHEDTEXTURE;
    } else if (!SStrCmp(name, "disabledtexture", STORM_MAX_STR)) {
        element = XML_DISABLEDTEXTURE;
    } else {
        if (!SStrCmp(name, "highlighttexture", STORM_MAX_STR)) {
            return HighlightTexture_parse(node, block);
        }

        if (!SStrCmp(name, "buttontext", STORM_MAX_STR)) {
            return ButtonText_parse(node, block);
        }

        if (!SStrCmp(name, "disabledfont", STORM_MAX_STR)) {
            return DisabledFont_parse(node, block);
        }

        if (!SStrCmp(name, "normalfont", STORM_MAX_STR)) {
            return NormalFont_parse(node, block);
        }

        if (!SStrCmp(name, "highlightfont", STORM_MAX_STR)) {
            return HighlightFont_parse(node, block);
        }

        if (!SStrCmp(name, "normalcolor", STORM_MAX_STR)) {
            return NormalColor_parse(node, block);
        }

        if (!SStrCmp(name, "disabledcolor", STORM_MAX_STR)) {
            return DisabledColor_parse(node, block);
        }

        if (!SStrCmp(name, "highlightcolor", STORM_MAX_STR)) {
            return HighlightColor_parse(node, block);
        }

        if (!SStrCmp(name, "pushedtextoffset", STORM_MAX_STR)) {
            return AdvanceAndParseDimension<XML_PUSHEDTEXTOFFSET>(node, block);
        }

        return false;
    }

    node->m_element = element;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
    node->m_size = sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x00505A50
static bool CheckButton_child(linkObject* node, MemBlock& block, char* name) {
    uint32_t element;

    if (!SStrCmp(name, "checkedtexture", STORM_MAX_STR)) {
        element = XML_CHECKEDTEXTURE;
    } else {
        if (SStrCmp(name, "disabledcheckedtexture", STORM_MAX_STR)) {
            return false;
        }

        element = XML_DISABLEDCHECKEDTEXTURE;
    }

    node->m_element = element;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Texture);
    node->m_size = sizeof(Texture);

    return static_cast<Texture*>(node)->parse(block);
}

// OFFSET: %0x00505AF0
bool Layer::parse(MemBlock& block) {
    this->m_level = 2;
    this->m_subLevel = 0;

    const char* level = this->m_xml->GetAttributeByName("level");

    if (level) {
        this->m_layerFlags |= 1;
        StringToDrawLayer(level, this->m_level);
    }

    const char* subLevel = this->m_xml->GetAttributeByName("textureSubLevel");

    if (subLevel) {
        int32_t value = SStrToInt(subLevel);

        if (value > 7) {
            value = 7;
        } else if (value < -8) {
            value = -8;
        }

        this->m_layerFlags |= 2;
        this->m_subLevel = static_cast<int8_t>(value);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->linkObject::parse(block);

    return this->dochildren(Layer_child, block);
}

// OFFSET: %0x00505B80
bool Slider::parse(MemBlock& block) {
    this->m_flags |= 0x800;

    const char* valueStep = this->m_xml->GetAttributeByName("valueStep");

    if (valueStep && *valueStep) {
        this->m_valueFlags |= 0x10;
        this->m_valueStep = SStrToFloat(valueStep);
    }

    this->m_drawLayer = 3;
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseValues(block);

    return this->dochildren(Slider_child, block);
}

// OFFSET: %0x005003C0
bool StatusBar::parseValues(MemBlock& block) {
    const char* minValue = this->m_xml->GetAttributeByName("minValue");

    if (minValue && *minValue) {
        this->m_valueFlags |= 2;
        this->m_minValue = SStrToFloat(minValue);
    }

    const char* maxValue = this->m_xml->GetAttributeByName("maxValue");

    if (maxValue && *maxValue) {
        this->m_valueFlags |= 4;
        this->m_maxValue = SStrToFloat(maxValue);
    }

    const char* defaultValue = this->m_xml->GetAttributeByName("defaultValue");

    if (defaultValue && *defaultValue) {
        this->m_valueFlags |= 8;
        this->m_defaultValue = SStrToFloat(defaultValue);
    }

    const char* orientation = this->m_xml->GetAttributeByName("orientation");

    if (orientation && *orientation && StringToOrientation(orientation, reinterpret_cast<ORIENTATION&>(this->m_orientation))) {
        this->m_valueFlags |= 1;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->Frame::parse(block);
}

// OFFSET: %0x00505BF0
bool StatusBar::parse(MemBlock& block) {
    this->m_flags |= 0x80;
    this->m_drawLayer = 2;

    const char* rotatesTexture = this->m_xml->GetAttributeByName("rotatesTexture");

    if (rotatesTexture && *rotatesTexture) {
        this->m_valueFlags |= 0x20;
        this->m_valueFlags ^= (this->m_valueFlags ^ (StringToBOOL(rotatesTexture) << 6)) & 0x40;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->parseValues(block);

    return this->dochildren(StatusBar_child, block);
}

// OFFSET: %0x005004A0
bool Cooldown::parse(MemBlock& block) {
    this->m_flags |= 0x400000;

    const char* reverse = this->m_xml->GetAttributeByName("reverse");

    if (reverse && *reverse) {
        this->m_cooldownFlags |= 1;
        this->m_cooldownFlags ^= (this->m_cooldownFlags ^ (2 * StringToBOOL(reverse))) & 2;
    }

    const char* drawEdge = this->m_xml->GetAttributeByName("drawEdge");

    if (drawEdge && *drawEdge) {
        this->m_cooldownFlags |= 4;
        this->m_cooldownFlags ^= (this->m_cooldownFlags ^ (8 * StringToBOOL(drawEdge))) & 8;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);

    return true;
}

// OFFSET: %0x00505C70
bool MessageFrame::parse(MemBlock& block) {
    this->m_flags |= 0x200;

    const char* fade = this->m_xml->GetAttributeByName("fade");

    if (fade && *fade) {
        this->m_messageFlags |= 1;
        this->m_messageFlags ^= (this->m_messageFlags ^ (2 * StringToBOOL(fade))) & 2;
    }

    const char* displayDuration = this->m_xml->GetAttributeByName("displayDuration");

    if (displayDuration && *displayDuration) {
        this->m_messageFlags |= 4;
        this->m_displayDuration = SStrToFloat(displayDuration);
    }

    const char* fadeDuration = this->m_xml->GetAttributeByName("fadeDuration");

    if (fadeDuration && *fadeDuration) {
        this->m_messageFlags |= 8;
        this->m_fadeDuration = SStrToFloat(fadeDuration);
    }

    const char* insertMode = this->m_xml->GetAttributeByName("insertMode");

    if (insertMode && *insertMode) {
        int32_t bottom = SStrCmpI(insertMode, "BOTTOM", STORM_MAX_STR);
        this->m_messageFlags |= 0x10;
        this->m_insertMode = bottom == 0;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);

    return this->dochildren(FontFrame_child, block);
}

// OFFSET: %0x00505D80
bool ScrollingMessageFrame::parse(MemBlock& block) {
    this->m_flags |= 0x400;

    const char* maxLines = this->m_xml->GetAttributeByName("maxLines");

    if (maxLines && *maxLines) {
        int32_t lines = SStrToInt(maxLines);
        this->m_scrollFlags |= 1;
        this->m_maxLines = lines;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);

    return this->MessageFrame::parse(block);
}

// OFFSET: %0x00505DE0
bool SimpleHTML::parse(MemBlock& block) {
    this->m_flags |= 0x1000;

    const char* file = this->m_xml->GetAttributeByName("file");

    if (file) {
        this->m_htmlFlags |= 1;
        this->AddString(block, this->m_file, file);
    }

    const char* hyperlinkFormat = this->m_xml->GetAttributeByName("hyperlinkFormat");

    if (hyperlinkFormat) {
        this->m_htmlFlags |= 2;
        this->AddString(block, this->m_hyperlinkFormat, hyperlinkFormat);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);

    return this->dochildren(FontFrame_child, block);
}

// OFFSET: %0x00505E70
bool EditBox::parse(MemBlock& block) {
    this->m_flags |= 0x40;

    const char* letters = this->m_xml->GetAttributeByName("letters");

    if (letters) {
        this->m_editFlags |= 1;
        this->m_letters = SStrToInt(letters);
    }

    const char* countInvisible = this->m_xml->GetAttributeByName("countInvisibleLetters");

    if (countInvisible && *countInvisible) {
        this->m_editFlags |= 2;
        this->m_editFlags ^= (this->m_editFlags ^ (4 * StringToBOOL(countInvisible))) & 4;
    }

    const char* blinkSpeed = this->m_xml->GetAttributeByName("blinkSpeed");

    if (blinkSpeed && *blinkSpeed) {
        this->m_editFlags |= 8;
        this->m_blinkSpeed = SStrToFloat(blinkSpeed) <= 0.0f ? 0.0f : SStrToFloat(blinkSpeed);
    }

    const char* autoFocus = this->m_xml->GetAttributeByName("autoFocus");

    if (autoFocus && *autoFocus) {
        this->m_editFlags |= 0x10;
        this->m_editFlags ^= (this->m_editFlags ^ (32 * StringToBOOL(autoFocus))) & 0x20;
    }

    const char* multiLine = this->m_xml->GetAttributeByName("multiLine");

    if (multiLine && *multiLine) {
        this->m_editFlags |= 0x40;
        this->m_editFlags ^= (this->m_editFlags ^ (StringToBOOL(multiLine) << 7)) & 0x80;
    }

    const char* numeric = this->m_xml->GetAttributeByName("numeric");

    if (numeric && *numeric) {
        this->m_editFlags |= 0x100;
        this->m_editFlags ^= (this->m_editFlags ^ (StringToBOOL(numeric) << 9)) & 0x200;
    }

    const char* password = this->m_xml->GetAttributeByName("password");

    if (password && *password) {
        this->m_editFlags |= 0x400;
        this->m_editFlags ^= (this->m_editFlags ^ (StringToBOOL(password) << 11)) & 0x800;
    }

    const char* ignoreArrows = this->m_xml->GetAttributeByName("ignoreArrows");

    if (ignoreArrows && *ignoreArrows) {
        this->m_editFlags |= 0x1000;
        this->m_editFlags ^= (this->m_editFlags ^ (StringToBOOL(ignoreArrows) << 13)) & 0x2000;
    }

    const char* historyLines = this->m_xml->GetAttributeByName("historyLines");

    if (historyLines) {
        this->m_editFlags |= 0x4000;
        this->m_historyLines = SStrToInt(historyLines) <= 0 ? 0 : SStrToInt(historyLines);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);
    this->dochildren(FontFrame_child, block);

    return this->dochildren(EditBox_child, block);
}

// OFFSET: %0x005060D0
bool Button::parse(MemBlock& block) {
    this->m_flags |= 4;

    const char* text = this->m_xml->GetAttributeByName("text");

    if (text && *text) {
        this->m_buttonFlags |= 1;
        this->AddString(block, this->m_text, text);
    }

    const char* registerForClicks = this->m_xml->GetAttributeByName("registerForClicks");

    if (registerForClicks && *registerForClicks) {
        this->m_buttonFlags |= 2;
        this->AddString(block, this->m_registerForClicks, registerForClicks);
    }

    const char* motionScripts = this->m_xml->GetAttributeByName("motionScriptsWhileDisabled");

    if (motionScripts) {
        this->m_buttonFlags |= 4;
        this->m_buttonFlags ^= (this->m_buttonFlags ^ (8 * StringToBOOL(motionScripts))) & 8;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);

    return this->dochildren(Button_child, block);
}

// OFFSET: %0x005061A0
bool CheckButton::parse(MemBlock& block) {
    this->m_flags |= 2;

    const char* checked = this->m_xml->GetAttributeByName("checked");

    if (checked && *checked) {
        this->m_checkFlags |= 1;
        this->m_checkFlags ^= (this->m_checkFlags ^ (2 * StringToBOOL(checked))) & 2;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Button::parse(block);

    return this->dochildren(CheckButton_child, block);
}

// OFFSET: %0x00506210
static bool MessageFrame_advance(linkObject* node, MemBlock& block) {
    MessageFrame* frame = static_cast<MessageFrame*>(node);
    frame->m_regionType = XML_MESSAGEFRAME;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(MessageFrame);

    return frame->parse(block);
}

// OFFSET: %0x00506240
static bool ScrollingMessageFrame_advance(linkObject* node, MemBlock& block) {
    ScrollingMessageFrame* frame = static_cast<ScrollingMessageFrame*>(node);
    frame->m_regionType = XML_SCROLLINGMESSAGEFRAME;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(ScrollingMessageFrame);

    return frame->parse(block);
}

// OFFSET: %0x00506270
static bool StatusBar_advance(linkObject* node, MemBlock& block) {
    StatusBar* bar = static_cast<StatusBar*>(node);
    bar->m_regionType = XML_STATUSBAR;
    block.m_cur = reinterpret_cast<char*>(bar) + sizeof(StatusBar);

    return bar->parse(block);
}

// OFFSET: %0x005062A0
static bool EditBox_advance(linkObject* node, MemBlock& block) {
    EditBox* editBox = static_cast<EditBox*>(node);
    editBox->m_regionType = XML_EDITBOX;
    block.m_cur = reinterpret_cast<char*>(editBox) + sizeof(EditBox);

    return editBox->parse(block);
}

// OFFSET: %0x005062D0
static bool ColorSelect_advance(linkObject* node, MemBlock& block) {
    Frame* frame = static_cast<Frame*>(node);
    frame->m_regionType = XML_COLORSELECT;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(Frame);
    frame->m_flags |= 0x200000;
    frame->m_size = block.m_cur - reinterpret_cast<char*>(frame);
    frame->parse(block);

    return frame->dochildren(ColorSelect_child, block);
}

// OFFSET: %0x00506320
static bool SimpleHTML_advance(linkObject* node, MemBlock& block) {
    SimpleHTML* html = static_cast<SimpleHTML*>(node);
    html->m_regionType = XML_SIMPLEHTML;
    block.m_cur = reinterpret_cast<char*>(html) + sizeof(SimpleHTML);

    return html->parse(block);
}

// OFFSET: %0x00506350
static bool Layers_child(linkObject* node, MemBlock& block, char* name) {
    if (SStrCmp(name, "layer", STORM_MAX_STR)) {
        return false;
    }

    node->m_element = XML_LAYER;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Layer);

    return static_cast<Layer*>(node)->parse(block);
}

template <uint32_t TYPE>
static bool AdvanceAndParseFrame(linkObject* node, MemBlock& block) {
    Frame* frame = static_cast<Frame*>(node);
    frame->m_regionType = TYPE;
    block.m_cur = reinterpret_cast<char*>(frame) + sizeof(Frame);
    frame->m_size = sizeof(Frame);

    return frame->parse(block);
}

template <uint32_t TYPE>
static bool AdvanceAndParseModel(linkObject* node, MemBlock& block) {
    Model* model = static_cast<Model*>(node);
    model->m_regionType = TYPE;
    block.m_cur = reinterpret_cast<char*>(model) + sizeof(Model);
    model->m_size = sizeof(Model);
    model->m_size = block.m_cur - reinterpret_cast<char*>(model);

    return model->parse(block);
}

// OFFSET: %0x005005C0
static bool Model_advance(linkObject* node, MemBlock& block) {
    Model* model = static_cast<Model*>(node);
    model->m_regionType = XML_MODEL;
    block.m_cur = reinterpret_cast<char*>(model) + sizeof(Model);

    return model->parse(block);
}

// OFFSET: %0x005005F0
static bool ModelFFX_advance(linkObject* node, MemBlock& block) {
    ModelFFX* model = static_cast<ModelFFX*>(node);
    model->m_regionType = XML_MODELFFX;
    block.m_cur = reinterpret_cast<char*>(model) + sizeof(ModelFFX);
    model->m_flags |= 0x20000;

    const char* glow = model->m_xml->GetAttributeByName("glow");

    if (glow) {
        model->m_ffxFlags |= 1;
        model->m_glow = SStrToFloat(glow);
    }

    model->m_size = block.m_cur - reinterpret_cast<char*>(model);
    model->Model::parse(block);

    return true;
}

// OFFSET: %0x00500650
static bool Cooldown_advance(linkObject* node, MemBlock& block) {
    Cooldown* cooldown = static_cast<Cooldown*>(node);
    cooldown->m_regionType = XML_COOLDOWN;
    block.m_cur = reinterpret_cast<char*>(cooldown) + sizeof(Cooldown);

    return cooldown->parse(block);
}

// OFFSET: %0x005063B0
static bool Region_context(linkObject* node, MemBlock& block, const char* name) {
    if (!SStrCmp(name, "button", STORM_MAX_STR)) {
        Button* button = static_cast<Button*>(node);
        button->m_regionType = XML_BUTTON;
        block.m_cur = reinterpret_cast<char*>(button) + sizeof(Button);

        return button->parse(block);
    }

    if (!SStrCmp(name, "checkbutton", STORM_MAX_STR)) {
        CheckButton* button = static_cast<CheckButton*>(node);
        button->m_regionType = XML_CHECKBUTTON;
        block.m_cur = reinterpret_cast<char*>(button) + sizeof(CheckButton);

        return button->parse(block);
    }

    if (!SStrCmp(name, "slider", STORM_MAX_STR)) {
        Slider* slider = static_cast<Slider*>(node);
        slider->m_regionType = XML_SLIDER;
        block.m_cur = reinterpret_cast<char*>(slider) + sizeof(Slider);

        return slider->parse(block);
    }

    if (!SStrCmp(name, "scrollframe", STORM_MAX_STR)) {
        return ScrollFrame_advance(node, block);
    }

    if (!SStrCmp(name, "texture", STORM_MAX_STR)) {
        return Texture_advance(node, block);
    }

    if (!SStrCmp(name, "model", STORM_MAX_STR)) {
        return Model_advance(node, block);
    }

    if (!SStrCmp(name, "movieframe", STORM_MAX_STR)) {
        return AdvanceAndParseFrame<XML_MOVIEFRAME>(node, block);
    }

    if (!SStrCmp(name, "modelffx", STORM_MAX_STR)) {
        return ModelFFX_advance(node, block);
    }

    if (!SStrCmp(name, "messageframe", STORM_MAX_STR)) {
        return MessageFrame_advance(node, block);
    }

    if (!SStrCmp(name, "scrollingmessageframe", STORM_MAX_STR)) {
        return ScrollingMessageFrame_advance(node, block);
    }

    if (!SStrCmp(name, "worldframe", STORM_MAX_STR)) {
        return AdvanceAndParseFrame<XML_WORLDFRAME>(node, block);
    }

    if (!SStrCmp(name, "statusbar", STORM_MAX_STR)) {
        return StatusBar_advance(node, block);
    }

    if (!SStrCmp(name, "cooldown", STORM_MAX_STR)) {
        return Cooldown_advance(node, block);
    }

    if (!SStrCmp(name, "editbox", STORM_MAX_STR)) {
        return EditBox_advance(node, block);
    }

    if (!SStrCmp(name, "gametooltip", STORM_MAX_STR)) {
        return AdvanceAndParseFrame<XML_GAMETOOLTIP>(node, block);
    }

    if (!SStrCmp(name, "blobframe", STORM_MAX_STR)) {
        return BlobFrame_advance(node, block);
    }

    if (!SStrCmp(name, "questpoiframe", STORM_MAX_STR)) {
        return QuestPOIFrame_advance(node, block);
    }

    if (!SStrCmp(name, "archaeologydigsiteframe", STORM_MAX_STR)) {
        return ArchaeologyDigSiteFrame_advance(node, block);
    }

    if (!SStrCmp(name, "minimap", STORM_MAX_STR)) {
        return Minimap_advance(node, block);
    }

    if (!SStrCmp(name, "playermodel", STORM_MAX_STR)) {
        return AdvanceAndParseModel<XML_PLAYERMODEL>(node, block);
    }

    if (!SStrCmp(name, "dressupmodel", STORM_MAX_STR)) {
        return AdvanceAndParseModel<XML_DRESSUPMODEL>(node, block);
    }

    if (!SStrCmp(name, "tabardmodel", STORM_MAX_STR)) {
        return AdvanceAndParseModel<XML_TABARDMODEL>(node, block);
    }

    if (!SStrCmp(name, "fontstring", STORM_MAX_STR)) {
        return FontString_advance(node, block);
    }

    if (!SStrCmp(name, "colorselect", STORM_MAX_STR)) {
        return ColorSelect_advance(node, block);
    }

    if (!SStrCmp(name, "simplehtml", STORM_MAX_STR)) {
        return SimpleHTML_advance(node, block);
    }

    if (!SStrCmp(name, "frame", STORM_MAX_STR)) {
        Frame* frame = static_cast<Frame*>(node);
        frame->m_regionType = XML_FRAME;
        block.m_cur = reinterpret_cast<char*>(frame) + sizeof(Frame);

        return frame->parse(block);
    }

    return false;
}

// OFFSET: %0x00506AF0
static bool Ui_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "include", STORM_MAX_STR)) {
        return Include_parse(node, block);
    }

    if (!SStrCmp(name, "script", STORM_MAX_STR)) {
        node->m_element = XML_SCRIPT;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Script);

        return Script_parse(static_cast<Script*>(node), block);
    }

    if (!SStrCmp(name, "font", STORM_MAX_STR)) {
        node->m_element = XML_FONT;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Font);

        return static_cast<Font*>(node)->parseFont(block);
    }

    node->m_element = XML_REGION;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(Region);

    return Region_context(node, block, name);
}

// OFFSET: %0x00506BF0
linkObject* FrameXML_CompileXML_Region(XMLNode* node) {
    MemBlock block;
    block.Advance();

    Region* region = reinterpret_cast<Region*>(block.m_cur);
    region->m_xml = node;
    region->m_next = NULL;
    node->m_flags |= 1;

    char name[128];
    SStrCopy(name, node->m_name.m_str, sizeof(name));
    SStrLower(name);

    region->m_regionType = XML_REGION;
    block.m_cur = reinterpret_cast<char*>(region) + sizeof(Region);
    Region_context(region, block, name);

    uint32_t size;

    return region->Compile(size);
}

// OFFSET: %0x00506CB0
static bool Layers_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_LAYERS;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(Layers_child, block);
}

// OFFSET: %0x00506CF0
static bool Frames_parse(linkObject* node, MemBlock& block) {
    node->m_element = XML_FRAMES;
    block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
    node->m_size = sizeof(linkObject);
    node->parse(block);

    return node->dochildren(Frames_child, block);
}

// OFFSET: %0x00506D30
linkObject* FrameXML_CompileXML_Ui(XMLNode* node, MemBlock& block) {
    block.Advance();

    Ui* ui = reinterpret_cast<Ui*>(block.m_cur);
    ui->m_xml = node;
    ui->m_next = NULL;
    node->m_flags |= 1;
    ui->m_element = XML_UI;
    block.m_cur = reinterpret_cast<char*>(ui) + sizeof(Ui);
    ui->m_size = sizeof(Ui);
    ui->parse(block);
    ui->dochildren(Ui_child, block);

    return ui;
}

// OFFSET: %0x00506D80
static bool Frame_child(linkObject* node, MemBlock& block, char* name) {
    if (!SStrCmp(name, "titleregion", STORM_MAX_STR)) {
        Region* region = static_cast<Region*>(node);
        region->m_element = XML_TITLEREGION;
        block.m_cur = reinterpret_cast<char*>(region) + sizeof(Region);
        region->m_size = sizeof(Region);

        return region->parse(block);
    }

    if (!SStrCmp(name, "resizebounds", STORM_MAX_STR)) {
        node->m_element = XML_RESIZEBOUNDS;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(linkObject);
        node->m_size = sizeof(linkObject);
        node->parse(block);

        return node->dochildren(ResizeBounds_child, block);
    }

    if (!SStrCmp(name, "backdrop", STORM_MAX_STR)) {
        node->m_element = XML_BACKDROP;
        block.m_cur = reinterpret_cast<char*>(node) + sizeof(Backdrop);

        return static_cast<Backdrop*>(node)->parse(block);
    }

    if (!SStrCmp(name, "hitrectinsets", STORM_MAX_STR)) {
        return AdvanceAndParseInsets<XML_HITRECTINSETS>(node, block);
    }

    if (!SStrCmp(name, "layers", STORM_MAX_STR)) {
        return Layers_parse(node, block);
    }

    if (!SStrCmp(name, "attributes", STORM_MAX_STR)) {
        return Attributes_parse(node, block);
    }

    if (!SStrCmp(name, "scripts", STORM_MAX_STR)) {
        return Scripts_parse(node, block);
    }

    if (!SStrCmp(name, "frames", STORM_MAX_STR)) {
        return Frames_parse(node, block);
    }

    return false;
}

// OFFSET: %0x00507000
bool Frame::parse(MemBlock& block) {
    this->m_flags |= 8;

    const char* parent = this->m_xml->GetAttributeByName("parent");

    if (parent && *parent) {
        this->m_frameFlags |= 0x400000;
        this->AddString(block, this->m_parent, parent);
    }

    this->m_id = -1;

    const char* id = this->m_xml->GetAttributeByName("id");

    if (id) {
        int32_t value = SStrToInt(id);
        this->m_frameFlags |= 0x800000;
        this->m_id = value;
    }

    const char* drawLayer = this->m_xml->GetAttributeByName("drawLayer");

    if (drawLayer && *drawLayer && StringToDrawLayer(drawLayer, this->m_drawLayer)) {
        this->m_frameFlags |= 0x1000000;
    }

    const char* value = this->m_xml->GetAttributeByName("toplevel");

    if (value && *value) {
        this->m_frameFlags |= 0x400;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 11)) & 0x800;
    }

    value = this->m_xml->GetAttributeByName("useParentLevel");

    if (value && *value) {
        this->m_frameFlags |= 0x2000000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 26)) & 0x4000000;
    }

    value = this->m_xml->GetAttributeByName("movable");

    if (value && *value) {
        this->m_frameFlags |= 0x1000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 13)) & 0x2000;
    }

    value = this->m_xml->GetAttributeByName("resizable");

    if (value && *value) {
        this->m_frameFlags |= 0x4000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 15)) & 0x8000;
    }

    const char* strata = this->m_xml->GetAttributeByName("frameStrata");

    if (strata && StringToFrameStrata(strata, reinterpret_cast<FRAME_STRATA&>(this->m_strata))) {
        this->m_frameFlags |= 0x10000;
    }

    const char* frameLevel = this->m_xml->GetAttributeByName("frameLevel");

    if (frameLevel) {
        this->m_frameFlags |= 1;
        this->m_frameLevel = SStrToInt(frameLevel);
    }

    value = this->m_xml->GetAttributeByName("enableMouse");

    if (value && *value) {
        this->m_frameFlags |= 0x20000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 18)) & 0x40000;
    }

    value = this->m_xml->GetAttributeByName("enableKeyboard");

    if (value && *value) {
        this->m_frameFlags |= 0x80000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 20)) & 0x100000;
    }

    value = this->m_xml->GetAttributeByName("clampedToScreen");

    if (value && *value) {
        this->m_frameFlags |= 2;
        this->m_frameFlags ^= (this->m_frameFlags ^ (4 * StringToBOOL(value))) & 4;
    }

    value = this->m_xml->GetAttributeByName("protected");

    if (value && *value) {
        this->m_frameFlags |= 8;
        this->m_frameFlags ^= (this->m_frameFlags ^ (16 * StringToBOOL(value))) & 0x10;
    }

    value = this->m_xml->GetAttributeByName("jumpNavigateEnabled");

    if (value && *value) {
        this->m_frameFlags |= 0x20;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 6)) & 0x40;
    }

    value = this->m_xml->GetAttributeByName("jumpNavigateStart");

    if (value && *value) {
        this->m_frameFlags |= 0x80;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 8)) & 0x100;
    }

    const char* depth = this->m_xml->GetAttributeByName("depth");

    if (depth) {
        this->m_frameFlags |= 0x200;
        this->m_depth = SStrToFloat(depth);
    }

    value = this->m_xml->GetAttributeByName("dontSavePosition");

    if (value && *value) {
        this->m_frameFlags |= 0x8000000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 28)) & 0x10000000;
    }

    value = this->m_xml->GetAttributeByName("propagateKeyboardInput");

    if (value && *value) {
        this->m_frameFlags |= 0x20000000;
        this->m_frameFlags ^= (this->m_frameFlags ^ (StringToBOOL(value) << 30)) & 0x40000000;
    }

    value = this->m_xml->GetAttributeByName("forceAlpha");

    if (value && *value) {
        this->m_frameFlags |= 0x80000000;
        this->m_frameFlags2 ^= (static_cast<uint8_t>(StringToBOOL(value)) ^ static_cast<uint8_t>(this->m_frameFlags2)) & 1;
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Region::parse(block);

    return this->dochildren(Frame_child, block);
}

// OFFSET: %0x005073D0
bool Model::parse(MemBlock& block) {
    this->m_flags |= 0x100;

    const char* file = this->m_xml->GetAttributeByName("file");

    if (file) {
        this->m_modelFlags |= 1;
        this->AddString(block, this->m_file, file);
    }

    const char* scale = this->m_xml->GetAttributeByName("scale");

    if (scale) {
        this->m_modelFlags |= 2;
        this->m_scale = SStrToFloat(scale);
    }

    const char* fogNear = this->m_xml->GetAttributeByName("fogNear");

    if (fogNear) {
        this->m_modelFlags |= 4;
        this->m_fogNear = SStrToFloat(fogNear);
    }

    const char* fogFar = this->m_xml->GetAttributeByName("fogFar");

    if (fogFar) {
        this->m_modelFlags |= 8;
        this->m_fogFar = SStrToFloat(fogFar);
    }

    this->m_size = block.m_cur - reinterpret_cast<char*>(this);
    this->Frame::parse(block);

    return this->dochildren(Model_child, block);
}
