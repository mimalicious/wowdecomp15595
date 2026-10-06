#include "CGGameObject_C_Type_FishingHole.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_FishingHole::CGGameObject_C_Type_FishingHole(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// OFFSET: %0x004E0850
bool CGGameObject_C_Type_FishingHole::CanHighlight() const {
    return true;
}

// OFFSET: %0x005A4510
bool CGGameObject_C_Type_FishingHole::CanUse() const {
    return false;
}
