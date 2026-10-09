#include "CGGameObject_C_Type_FishingHole.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_FishingHole::CGGameObject_C_Type_FishingHole(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// PROVEN: fd34c74a IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_Type_FishingHole::CanHighlight() const {
    return true;
}

// PROVEN: be3e38e8 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_FishingHole::CanUse() const {
    return false;
}
