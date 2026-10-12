#include "CGGameObject_C_Type_SpellCaster.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"

// OFFSET: 0x005A28A0/0x140212BB0
CGGameObject_C_Type_SpellCaster::CGGameObject_C_Type_SpellCaster(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x005A2900/0x140212C20
bool CGGameObject_C_Type_SpellCaster::CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        if (!this->m_owner->GetProperty(47)) {
            return CGGameObject_C_TypeBase::CanUseNow(error, distance, errorText);
        }

        WOWGUID creator = this->m_owner->m_gameObject->m_createdBy;

        if ((creator.m_guid & 0xF000000000000000ULL) || !(creator.m_guid & 0xF07FFFFFFFFFFFFFULL) || CGPartyInfo::IsMember(creator) || CGGameUI::IsPartyMember(creator)) {
            return CGGameObject_C_TypeBase::CanUseNow(error, distance, errorText);
        }
    }

    if (error) {
        *error = static_cast<GAME_ERROR_TYPE>(176);
    }

    return false;
}
