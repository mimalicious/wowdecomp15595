#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"

// OFFSET: 0x005958C0/0x140202E60
CGVoidStorage_C::CGVoidStorage_C() {
    this->m_state = 0;
    this->m_items = NULL;
}

// OFFSET: 0x00595BB0
CGVoidStorage_C::~CGVoidStorage_C() {
    if (this->m_items) {
        this->m_items->InternalClear(0);
        delete this->m_items;
    }
}

// OFFSET: 0x005958E0/0x140202E80
VoidItem_C* CGVoidStorage_C::GetItemBySlot(uint32_t slot) {
    if (!this->m_items || slot >= 80) {
        return NULL;
    }

    return this->m_items->Ptr(slot, HASHKEY_NONE());
}

// OFFSET: 0x00595910/0x140202EF0
VoidItem_C* CGVoidStorage_C::GetItem(WOWGUID guid) {
    TSHashTable<VoidItem_C, HASHKEY_NONE>* items = this->m_items;

    if (!items) {
        return NULL;
    }

    for (VoidItem_C* item = items->Head(); item; item = items->Next(item)) {
        if (item->m_guid == guid) {
            return item;
        }
    }

    return NULL;
}
