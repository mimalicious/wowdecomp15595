#include "CGGameObject_C_Type_Door.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"

// OFFSET: 0x005A1C30/0x140211D40
CGGameObject_C_Type_Door::CGGameObject_C_Type_Door(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
    this->m_linkAll = create.unk_0002;
    this->m_useErrorType = 244;
}

// OFFSET: 0x005A1CA0/0x140211DC0
bool CGGameObject_C_Type_Door::CanBeSolid() const {
    if (this->m_owner->m_displayState != 1) {
        return false;
    }

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return this->m_oneShotKit || (this->m_baseKit && !this->m_stateKit);
    }

    return this->m_animState != 4;
}

// PROVEN: 7acb43ae IDENTICAL/NONE
// OFFSET: 0x0059BF70/0x14020AA60
void CGGameObject_C_Type_Door::UpdateAnimState(uint32_t state) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    CGGameObject_C_TypeAnimated::UpdateAnimState(state);
    this->UpdateSolidity();
}

// OFFSET: 0x0059BFA0/0x14020AAA0
void CGGameObject_C_Type_Door::UpdateSolidity() {
    CGGameObject_C* owner = this->m_owner;
    bool solid;

    if (owner->m_displayState != 1) {
        solid = false;
    } else if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        solid = this->m_animState != 4;
    } else if (this->m_oneShotKit) {
        solid = true;
    } else {
        solid = this->m_baseKit && !this->m_stateKit;
    }

    if (solid != (owner->m_hasWorldBounds != 0)) {
        this->m_owner->m_hasWorldBounds = solid;

        if (this->m_owner->m_worldObject && this->m_linkAll) {
            World::ObjectEnablePortals(this->m_owner->m_worldObject, !solid);
        }
    }
}

// OFFSET: 0x0059FC00/0x14020F480
bool CGGameObject_C_Type_Door::CheckPropState() const {
    if (!this->m_owner->GetProperty(3)) {
        return true;
    }

    if (this->m_owner->GetProperty(1)) {
        if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
            return this->m_owner->m_displayState == 1 && this->m_baseKit && !this->m_stateKit;
        }

        return this->m_animState != 1;
    }

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return !this->m_owner->m_displayState && this->m_baseKit && !this->m_stateKit;
    }

    return this->m_animState != 3;
}

// OFFSET: 0x005A1CE0/0x140211E00
bool CGGameObject_C_Type_Door::CanUseNow(GAME_ERROR_TYPE* error, float* distance) const {
    if (this->CheckPropState()) {
        return CGGameObject_C_TypeBase::CanUseNow(error, distance, NULL);
    }

    if (error) {
        *error = static_cast<GAME_ERROR_TYPE>(251);
    }

    return false;
}
