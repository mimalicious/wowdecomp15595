#include "CGGameObject_C_Type_BarberChair.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"

// OFFSET: #0x0070C7F0
CGGameObject_C_Type_BarberChair::CGGameObject_C_Type_BarberChair(CGGameObject_C* owner) : CGGameObject_C_Type_Chair(owner) {
}

// PROVEN: 5da0a073 IDENTICAL
// OFFSET: 0x009FCE20
uint32_t CGGameObject_C_Type_BarberChair::GetNumSlots() const {
    return 1;
}

// OFFSET: #0x00711EB0
bool CGGameObject_C_Type_BarberChair::CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const {
    if (!CGGameObject_C_Type_Chair::CanUseNow(error, distance, errorText)) {
        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player->m_unit->m_unitFieldDisplayid == player->m_unit->m_unitFieldNativedisplayid && !player->IsShapeShifted()) {
        return true;
    }

    if (error) {
        *error = static_cast<GAME_ERROR_TYPE>(ERR_NOT_WHILE_SHAPESHIFTED);
    }

    return false;
}
