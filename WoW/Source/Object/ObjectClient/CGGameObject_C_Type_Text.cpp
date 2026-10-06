#include "CGGameObject_C_Type_Text.h"
#include "WoW/Source/Ui/ItemTextFrame.h"

// OFFSET: #0x007126D0
CGGameObject_C_Type_Text::CGGameObject_C_Type_Text(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
    this->m_interactDistance = 5.5555553f;
}

// OFFSET: #0x0070B570
void CGGameObject_C_Type_Text::CloseInteraction() {
    this->UpdateAnimState(4);
}

// OFFSET: #0x0070B560
void CGGameObject_C_Type_Text::StartInteraction() {
    this->UpdateAnimState(2);
}

// PROVEN: e7bc6bee IDENTICAL/NONE
// OFFSET: 0x005994D0
int32_t CGGameObject_C_Type_Text::CanChangeCursor() const {
    return this->CanUseNow(NULL, NULL, NULL) ? 7 : 33;
}

// OFFSET: %0x005A4170
bool CGGameObject_C_Type_Text::Use() {
    WOWGUID guid = this->m_owner->m_obj->m_guid;
    CGItemText::SetItem(guid, 0);

    return true;
}

// OFFSET: %0x005A41A0
void CGGameObject_C_Type_Text::PostInitWithStats() {
    if (CGItemText::m_itemGUID == this->m_owner->m_obj->m_guid) {
        WOWGUID guid = this->m_owner->m_obj->m_guid;
        CGItemText::SetItem(guid, 1);
    }
}
