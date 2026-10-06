#include "CGGameObject_C_Type_Generic.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"

// OFFSET: #0x007125F0
CGGameObject_C_Type_Generic::CGGameObject_C_Type_Generic(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// OFFSET: #0x0070FF80
bool CGGameObject_C_Type_Generic::CanHighlight() const {
    uint32_t highlightIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 18);
    GameObjectStats* highlightStats = this->m_owner->m_stats;
    uint32_t highlight = (highlightStats && highlightIndex < 24) ? highlightStats->m_propValue[highlightIndex] : 0;

    return highlight != 0;
}

// PROVEN: fe11bcfe IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_Generic::CanUse() const {
    return false;
}
