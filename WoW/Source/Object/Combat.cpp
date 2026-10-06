#include "Combat.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"

// OFFSET: %0x00964FE0
void CCombat::SetAttacking(WOWGUID guid) {
    this->m_attacking = guid;
}

// OFFSET: %0x00965000
void CCombatClient::SetAttackSent(WOWGUID guid) {
    this->m_attacking = guid;
    this->m_attackSent = 1;
}

// OFFSET: #0x0098E560
void CCombat::SetClientInitData(const CClientObjCreate& create) {
    this->m_attacking = create.m_attackTarget;
}
