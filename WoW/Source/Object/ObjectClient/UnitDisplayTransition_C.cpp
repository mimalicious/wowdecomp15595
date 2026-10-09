#include "UnitDisplayTransition_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/World.h"

// OFFSET: 0x005FDBD0
int32_t CUnitDisplayTransition_C::GetTargetDisplayID() const {
    if (this->m_state <= 4 && this->m_displayInfo) {
        return this->m_displayInfo->m_id;
    }

    return 0;
}

// OFFSET: 0x005FDD40/0x140286440
void CUnitDisplayTransition_C::UpdateWorldObject(const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& position, int32_t a6, int32_t keyBone) {
    if (this->m_worldObject && (this->m_state == 4 || this->m_state == 5)) {
        World::ObjectUpdate(this->m_worldObject, matrix, box, sphere, position, a6, keyBone);
    }
}
