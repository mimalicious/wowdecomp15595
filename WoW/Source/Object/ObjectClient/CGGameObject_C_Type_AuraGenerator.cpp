#include "CGGameObject_C_Type_AuraGenerator.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_AuraGenerator::CGGameObject_C_Type_AuraGenerator(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// OFFSET: %0x004E0850
bool CGGameObject_C_Type_AuraGenerator::CanHighlight() const {
    return true;
}

// OFFSET: %0x005A4510
bool CGGameObject_C_Type_AuraGenerator::CanUse() const {
    return false;
}
