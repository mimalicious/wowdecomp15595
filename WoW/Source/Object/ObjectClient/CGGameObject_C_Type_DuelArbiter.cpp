#include "CGGameObject_C_Type_DuelArbiter.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_DuelArbiter::CGGameObject_C_Type_DuelArbiter(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// OFFSET: %0x004E0850
bool CGGameObject_C_Type_DuelArbiter::CanHighlight() const {
    return true;
}

// OFFSET: %0x005A4510
bool CGGameObject_C_Type_DuelArbiter::CanUse() const {
    return false;
}
