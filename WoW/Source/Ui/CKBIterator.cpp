#include "CKBIterator.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: #0x005509A0
void CKBIterator::GoToFirst() {
    if (!this->m_node) {
        return;
    }

    XMLNode* parent = this->m_node->m_parent;
    this->m_node = parent ? parent->m_child : NULL;
    this->m_index = 0;
}

// PROVEN: e5f77eaf IDENTICAL/NONE
// OFFSET: 0x009FAF70/0x140798720
void CKBIterator::GoToNext() {
    if (!this->m_node) {
        return;
    }

    XMLNode* next = this->m_node->m_next;
    this->m_index++;
    this->m_node = next;
}

// OFFSET: #0x005509E0
void CKBIterator::GoTo(int32_t index) {
    XMLNode* node = this->m_node;

    if (!node) {
        return;
    }

    if (this->m_index == index - 1) {
        this->m_index++;
        this->m_node = node->m_next;
        return;
    }

    if (this->m_index == index) {
        return;
    }

    if (index < 0) {
        this->m_node = NULL;
        this->m_index = 0;
        return;
    }

    if (this->m_index > index) {
        this->GoToFirst();
        node = this->m_node;
    }

    while (this->m_index < index) {
        if (!node) {
            break;
        }

        node = node->m_next;
        this->m_index++;
    }

    this->m_node = node;
}

// PROVEN: eacca35d IDENTICAL/NONE
// OFFSET: 0x009FAFF0/0x1407987B0
void CKBIterator::GoToFirstChild() {
    if (!this->m_node) {
        return;
    }

    this->m_node = this->m_node->m_child;
    this->m_index = 0;
}

// PROVEN: e2a535a9 IDENTICAL/NONE
// OFFSET: 0x009FC5F0
void CKBIterator::GoToChild(const char* name) {
    if (!this->m_node) {
        return;
    }

    XMLNode* child = this->m_node->m_child;

    for (this->m_index = 0; child; this->m_index++) {
        if (!SStrCmpI(child->m_name.m_str, name, STORM_MAX_STR)) {
            break;
        }

        child = child->m_next;
    }

    this->m_node = child;
}
