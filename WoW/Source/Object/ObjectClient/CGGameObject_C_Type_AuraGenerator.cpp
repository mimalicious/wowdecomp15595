#include "CGGameObject_C_Type_AuraGenerator.h"

// OFFSET: 0x005A2530/0x140212800
CGGameObject_C_Type_AuraGenerator::CGGameObject_C_Type_AuraGenerator(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_Type_AuraGenerator::CanHighlight() const {
    return true;
}

// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_AuraGenerator::CanUse() const {
    return false;
}
