#include "CGGameObject_C_Type_Mailbox.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"

// OFFSET: 0x005A2840/0x140212B40
CGGameObject_C_Type_Mailbox::CGGameObject_C_Type_Mailbox(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x0059DC40/0x14020CBF0
void CGGameObject_C_Type_Mailbox::StartInteraction() {
    CGGameObject_C* owner = this->m_owner;

    if (owner->IsObjectModelLoaded()) {
        owner->m_typeData->ActivateOneShotAnim(2);
        owner->m_pendingOneShotAnim = 0;
    } else {
        owner->m_pendingOneShotAnim = 2;
    }
}

// OFFSET: 0x00599640/0x140207A30
int32_t CGGameObject_C_Type_Mailbox::CanChangeCursor() const {
    return this->CanUseNow(NULL, NULL, NULL) ? 15 : 41;
}

// OFFSET: 0x0059AE10/0x140209550
bool CGGameObject_C_Type_Mailbox::Use() {
    CGPlayer_C::TalkToMailbox(this->m_owner->m_obj->m_guid);

    return true;
}
