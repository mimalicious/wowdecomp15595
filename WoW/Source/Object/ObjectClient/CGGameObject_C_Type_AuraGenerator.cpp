#include "CGGameObject_C_Type_AuraGenerator.h"

// OFFSET: #0x00712800
CGGameObject_C_Type_AuraGenerator::CGGameObject_C_Type_AuraGenerator(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// PROVEN: 40e17f90 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_Type_AuraGenerator::CanHighlight() const {
    return true;
}

// PROVEN: 3aeab986 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_AuraGenerator::CanUse() const {
    return false;
}
