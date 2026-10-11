#include "CGGameObject_C_Type_SpellFocus.h"

// INLINED
CGGameObject_C_Type_SpellFocus::CGGameObject_C_Type_SpellFocus(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// PROVEN: 891d08f9 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_Type_SpellFocus::CanHighlight() const {
    return true;
}

// PROVEN: 1c111db4 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_SpellFocus::CanUse() const {
    return false;
}
