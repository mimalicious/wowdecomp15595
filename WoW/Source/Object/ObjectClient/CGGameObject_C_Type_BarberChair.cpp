#include "CGGameObject_C_Type_BarberChair.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"

// OFFSET: 0x0059AAA0
CGGameObject_C_Type_BarberChair::CGGameObject_C_Type_BarberChair(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_Type_Chair(owner, create) {
}

// OFFSET: 0x009FCE20/0x14090FD50
uint32_t CGGameObject_C_Type_BarberChair::GetNumSlots() const {
    return 1;
}

// OFFSET: 0x005A2440/0x140212670
bool CGGameObject_C_Type_BarberChair::CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const {
    if (!CGGameObject_C_Type_Chair::CanUseNow(error, distance, errorText)) {
        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player->m_unit->m_unitFieldDisplayid == player->m_unit->m_unitFieldNativedisplayid && !player->IsShapeShifted() && player->m_unit->m_unitFieldNativedisplayid == player->GetCurrentClientNativeDisplayID()) {
        return true;
    }

    if (error) {
        *error = static_cast<GAME_ERROR_TYPE>(ERR_NOT_WHILE_SHAPESHIFTED);
    }

    return false;
}
