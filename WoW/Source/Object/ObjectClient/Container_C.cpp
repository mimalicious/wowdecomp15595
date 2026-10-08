#include "Container_C.h"

// OFFSET: 0x00599050
CGContainer_C::CGContainer_C(uint32_t time, const CClientObjCreate& create) : CGItem_C(time, create) {
    this->m_bag.m_numSlots = this->m_container->m_containerFieldNumSlots;
    this->m_bag.m_slots = reinterpret_cast<WOWGUID*>(this->m_container->m_containerFieldSlot1);
    this->m_bag.m_guid = this->m_obj->m_guid;
    this->m_bag.m_hasBank = 0;
}

// OFFSET: 0x0048E850
CGContainer_C::~CGContainer_C() {
}

// OFFSET: [0x00599100]
const CGBag_C* CGContainer_C::GetBag() const {
    return &this->m_bag;
}

// OFFSET: [0x00599100]
CGBag_C* CGContainer_C::GetBag() {
    return &this->m_bag;
}

// OFFSET: 0x00599010
int32_t CGContainer_C::GetFreeSlots() const {
    if (!this->m_container) {
        return 0;
    }

    int32_t freeSlots = this->m_bag.m_numSlots;

    for (int32_t i = 0; i < this->m_bag.m_numSlots; i++) {
        if (this->m_container->m_containerFieldSlot1[i]) {
            freeSlots--;
        }
    }

    return freeSlots;
}

// OFFSET: 0x00598FD0
void CGContainer_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGItem_C::SetStorage(storage, saved);
    this->m_container = reinterpret_cast<CGContainerData*>(reinterpret_cast<uint8_t*>(storage) + sizeof(CGObjectData) + sizeof(CGItemData));
    this->m_containerMirror = saved + CGItem::TotalFieldsSaved();
}

// PROVEN: 0f5f4e76 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGContainer_C::Initialize() {
}
