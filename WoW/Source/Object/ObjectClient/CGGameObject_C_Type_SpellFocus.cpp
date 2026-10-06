#include "CGGameObject_C_Type_SpellFocus.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_SpellFocus::CGGameObject_C_Type_SpellFocus(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// PROVEN: 891d08f9 IDENTICAL
// OFFSET: 0x00A3AA30
bool CGGameObject_C_Type_SpellFocus::CanHighlight() const {
    return true;
}

// PROVEN: 1c111db4 IDENTICAL
// OFFSET: 0x00B06D20
bool CGGameObject_C_Type_SpellFocus::CanUse() const {
    return false;
}
