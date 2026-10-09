#include "CGGameObject_C_Type_Binder.h"

// OFFSET: 0x0059A750
CGGameObject_C_Type_Binder::CGGameObject_C_Type_Binder(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeBase(owner, create) {
    this->m_interactDistance = 10.0f;
}
