#include "CMovementGlobals.h"
#include "WoW/Source/Object/Movement.h"

// OFFSET: 0x00579A90/0x1401DF120
CMovementGlobals::~CMovementGlobals() {
    if (this->m_movementLog) {
        fclose(this->m_movementLog);
    }

    this->m_movementLog = NULL;

    if (this->m_fallingLog) {
        fclose(this->m_fallingLog);
    }

    this->m_fallingLog = NULL;
}
