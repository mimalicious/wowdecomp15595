#include "CGGameObject_C_Type_Mailbox.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"

// OFFSET: #0x00712750
CGGameObject_C_Type_Mailbox::CGGameObject_C_Type_Mailbox(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// OFFSET: #0x0070C9F0
void CGGameObject_C_Type_Mailbox::StartInteraction() {
    CGGameObject_C* owner = this->m_owner;

    if (owner->IsObjectModelLoaded()) {
        owner->m_typeData->ActivateOneShotAnim(8);
        owner->m_pendingOneShotAnim = 0;
    } else {
        owner->m_pendingOneShotAnim = 2;
    }
}

// PROVEN: 25d830e7 IDENTICAL
// OFFSET: 0x00599640
int32_t CGGameObject_C_Type_Mailbox::CanChangeCursor() const {
    return this->CanUseNow(NULL, NULL, NULL) ? 15 : 41;
}

// OFFSET: %0x005A4460
bool CGGameObject_C_Type_Mailbox::Use() {
    CGPlayer_C::TalkToMailbox(this->m_owner->m_obj->m_guid);

    return true;
}
