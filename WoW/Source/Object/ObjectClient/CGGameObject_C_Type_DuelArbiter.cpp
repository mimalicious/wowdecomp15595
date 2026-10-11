#include "CGGameObject_C_Type_DuelArbiter.h"

// INLINED
CGGameObject_C_Type_DuelArbiter::CGGameObject_C_Type_DuelArbiter(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// PROVEN: 33ae85df IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_Type_DuelArbiter::CanHighlight() const {
    return true;
}

// PROVEN: 82631c17 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_DuelArbiter::CanUse() const {
    return false;
}
