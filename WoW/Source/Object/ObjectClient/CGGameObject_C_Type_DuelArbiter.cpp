#include "CGGameObject_C_Type_DuelArbiter.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_DuelArbiter::CGGameObject_C_Type_DuelArbiter(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// PROVEN: 33ae85df IDENTICAL
// OFFSET: 0x00A3AA30
bool CGGameObject_C_Type_DuelArbiter::CanHighlight() const {
    return true;
}

// PROVEN: 82631c17 IDENTICAL
// OFFSET: 0x00B06D20
bool CGGameObject_C_Type_DuelArbiter::CanUse() const {
    return false;
}
