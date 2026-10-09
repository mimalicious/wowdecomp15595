#include "CGGameObject_C_Type_AreaDamage.h"

// OFFSET: 0x005A1DE0/0x140211ED0
CGGameObject_C_Type_AreaDamage::CGGameObject_C_Type_AreaDamage(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
    this->m_interactDistance = 0.0f;
}
