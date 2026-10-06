#include <cstddef>
#include <cstdint>
#include <cstring>
#include <typeinfo>
#include <expat.h>
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00CC9190
static XML_Parser s_parser;

// STATIC: %0x00CC9194
static XMLNode* s_freeNodes;

// STATIC: %0x00CC9198
CDataAllocator XMLNode::s_allocator(sizeof(XMLNode), 1170);

// PROVEN: 914a8ef0 IDENTICAL
// OFFSET: 0x004EBA10
static void end_element(void* userData, const char* name) {
    XMLTree* tree = static_cast<XMLTree*>(userData);
    tree->m_current = tree->m_current->m_parent;
}

// OFFSET: %0x004FE690
static void handle_body(void* userData, const char* s, int len) {
    XMLNode* node = static_cast<XMLTree*>(userData)->m_current;

    if (!len) {
        return;
    }

    if (!node->m_body) {
        int32_t i = 0;

        while (i < len) {
            char c = s[i];

            if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
                break;
            }

            i++;
        }

        if (i >= len) {
            return;
        }
    }

    node->m_body = static_cast<char*>(SMemReAlloc(node->m_body, node->m_bodyLen + len + 1, __FILE__, __LINE__, 0));
    memcpy(&node->m_body[node->m_bodyLen], s, len);
    node->m_bodyLen += len;
    node->m_body[node->m_bodyLen] = '\0';
}

// OFFSET: %0x004FE710
void XMLNode::Init(XMLNode* parent, const char* name) {
    this->m_parent = parent;
    this->m_child = NULL;
    this->m_name.Copy(name);
    this->m_body = NULL;
    this->m_bodyLen = 0;

    if (this->m_parent && this->m_parent->m_body) {
        this->m_flags &= ~1u;
        this->m_next = NULL;
        this->m_userData = NULL;
        this->m_offset = this->m_parent->m_bodyLen;
    } else {
        this->m_flags &= ~1u;
        this->m_offset = 0;
        this->m_next = NULL;
        this->m_userData = NULL;
    }
}

// PROVEN: 6a443163 IDENTICAL
// OFFSET: 0x004EBB70
XMLNode* XMLNode::GetChildByName(const char* name) {
    for (XMLNode* child = this->m_child; child; child = child->m_next) {
        if (!SStrCmpI(child->m_name.m_str, name, STORM_MAX_STR)) {
            return child;
        }
    }

    return NULL;
}

// PROVEN: c4f5b8f4 IDENTICAL
// OFFSET: 0x004EBBB0
const char* XMLNode::GetAttributeByName(const char* name) {
    uint32_t count = this->m_attributes.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (!SStrCmpI(this->m_attributes[i].m_name.m_str, name, STORM_MAX_STR)) {
            return this->m_attributes[i].m_value.m_str;
        }
    }

    return NULL;
}

// OFFSET: %0x004FE810
void XMLTree_Cleanup() {
    if (s_parser) {
        XML_ParserFree(s_parser);
        s_parser = NULL;
    }

    while (s_freeNodes) {
        XMLNode* node = s_freeNodes;
        s_freeNodes = node->m_next;
        node->m_next = NULL;
        delete node;
    }
}

// OFFSET: %0x004FE9E0
XMLNode::~XMLNode() {
    if (this->m_userData) {
        delete this->m_userData;
    }

    if (this->m_next) {
        delete this->m_next;
    }

    if (this->m_child) {
        delete this->m_child;
    }

    if (this->m_body) {
        SMemFree(this->m_body, __FILE__, __LINE__, 0);
    }
}

// INLINED
void* XMLNode::operator new(size_t bytes) {
    return XMLNode::s_allocator.GetData(0, typeid(XMLNode).raw_name(), SLOG_OBJECT);
}

// INLINED
void XMLNode::operator delete(void* ptr) {
    XMLNode::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: %0x004FEB50
XMLNode::XMLNode(XMLNode* parent, const char* name) {
    this->Init(parent, name);
}

// OFFSET: %0x004FEB90
static void begin_element(void* userData, const char* name, const char** atts) {
    XMLTree* tree = static_cast<XMLTree*>(userData);
    XMLNode* node = s_freeNodes;

    if (node) {
        s_freeNodes = node->m_next;
        node->Init(tree->m_current, name);
    } else {
        node = new XMLNode(tree->m_current, name);
    }

    XMLNode* parent = tree->m_current;

    if (parent) {
        XMLNode* child = parent->m_child;

        if (child) {
            while (child->m_next) {
                child = child->m_next;
            }

            child->m_next = node;
        } else {
            parent->m_child = node;
        }
    } else {
        tree->m_root = node;
    }

    tree->m_current = node;

    uint32_t count = 0;

    for (const char** att = atts; *att; att += 2) {
        count++;
    }

    node->m_attributes.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        node->m_attributes[i].m_name.Copy(atts[2 * i]);
        node->m_attributes[i].m_value.Copy(atts[2 * i + 1]);
    }
}

// OFFSET: %0x004FEC80
void XMLNode::SetAttribute(const char* name, const char* value) {
    uint32_t count = this->m_attributes.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (!SStrCmpI(name, this->m_attributes[i].m_name.m_str, STORM_MAX_STR)) {
            this->m_attributes[i].m_value.Copy(value);
            return;
        }
    }

    this->m_attributes.SetCount(count + 1);
    this->m_attributes[count].m_name.Copy(name);
    this->m_attributes[count].m_value.Copy(value);
}

// OFFSET: %0x004FED20
void XMLNode::Recycle() {
    if (this->m_userData) {
        this->m_userData->Release();
        this->m_userData = NULL;
    }

    if (this->m_next) {
        this->m_next->Recycle();
        this->m_next = NULL;
    }

    if (this->m_child) {
        this->m_child->Recycle();
        this->m_child = NULL;
    }

    if (this->m_body) {
        SMemFree(this->m_body, __FILE__, __LINE__, 0);
        this->m_body = NULL;
    }

    this->m_attributes.Clear();
    this->m_next = s_freeNodes;
    s_freeNodes = this;
}

// PROVEN: a98ce437 IDENTICAL
// OFFSET: 0x004EC1A0
void XMLTree_Free(XMLTree* tree) {
    if (tree->m_root) {
        tree->m_root->Recycle();
    }

    SMemFree(tree, __FILE__, __LINE__, 0);
}

// OFFSET: %0x004FEDD0
XMLTree* XMLTree_Load(const char* data, uint32_t size) {
    XMLTree* tree = static_cast<XMLTree*>(SMemAlloc(sizeof(XMLTree), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    if (s_parser) {
        XML_ParserReset(s_parser, NULL);
    } else {
        s_parser = XML_ParserCreate(NULL);
    }

    XML_SetElementHandler(s_parser, begin_element, end_element);
    XML_SetCharacterDataHandler(s_parser, handle_body);
    XML_SetUserData(s_parser, tree);

    if (XML_Parse(s_parser, data, size, 1)) {
        return tree;
    }

    XMLTree_Free(tree);

    return NULL;
}

// PROVEN: 23d682c3 IDENTICAL
// OFFSET: 0x00B2DD50
XMLNode* XMLTree_GetRoot(XMLTree* tree) {
    return tree->m_root;
}

// OFFSET: %0x0065D090
XMLNodeData::~XMLNodeData() {
}

// PROVEN: 7981ca12 IDENTICAL
// OFFSET: 0x00401000
void XMLNodeData::Release() {
}
