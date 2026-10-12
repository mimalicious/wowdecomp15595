#include "CGGameObject_C_Type_Text.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/ItemTextFrame.h"

// OFFSET: 0x005A24D0/0x140212720
CGGameObject_C_Type_Text::CGGameObject_C_Type_Text(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
    this->m_interactDistance = 5.5555553f;
}

// OFFSET: 0x005A0950/0x140210460
void CGGameObject_C_Type_Text::CloseInteraction() {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        this->m_owner->SetDisplayState(1);
    } else {
        this->UpdateAnimState(4);
    }
}

// OFFSET: 0x005A08F0/0x1402103F0
void CGGameObject_C_Type_Text::StartInteraction() {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        this->m_owner->SetDisplayState(0);
    } else {
        this->UpdateAnimState(2);
    }
}

// OFFSET: 0x005994D0/0x1402078B0
int32_t CGGameObject_C_Type_Text::CanChangeCursor() const {
    return this->CanUseNow(NULL, NULL, NULL) ? 7 : 33;
}

// OFFSET: 0x0059AB20/0x140209290
bool CGGameObject_C_Type_Text::Use() {
    WOWGUID guid = this->m_owner->m_obj->m_guid;
    CGItemText::SetItem(guid, 0);

    return true;
}

// OFFSET: 0x0059AB50/0x1402092C0
void CGGameObject_C_Type_Text::PostInitWithStats() {
    if (CGItemText::m_itemGUID == this->m_owner->m_obj->m_guid) {
        WOWGUID guid = this->m_owner->m_obj->m_guid;
        CGItemText::SetItem(guid, 1);
    }
}
