#include "Combat.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"

// PROVEN: 97f53222 IDENTICAL
// OFFSET: 0x00A29150
void CCombat::SetAttacking(WOWGUID guid) {
    this->m_attacking = guid;
}

// PROVEN: 6a129a29 IDENTICAL
// OFFSET: 0x00A29170
void CCombatClient::SetAttackSent(WOWGUID guid) {
    this->m_attacking = guid;
    this->m_attackSent = 1;
}

// OFFSET: #0x0098E560
void CCombat::SetClientInitData(const CClientObjCreate& create) {
    this->m_attacking = create.m_attackTarget;
}
