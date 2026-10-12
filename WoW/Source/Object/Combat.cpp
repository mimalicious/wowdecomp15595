#include "Combat.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"

// PROVEN: 97f53222 IDENTICAL/IDENTICAL
// OFFSET: 0x00A29150/0x1408F6050
void CCombat::SetAttacking(WOWGUID guid) {
    this->m_attacking = guid;
}

// PROVEN: 6a129a29 IDENTICAL/IDENTICAL
// OFFSET: 0x00A29170/0x1407D0E20
void CCombatClient::SetAttackSent(WOWGUID guid) {
    this->m_attacking = guid;
    this->m_attackSent = 1;
}

// OFFSET: 0x00A29190/0x1407D0E30
void CCombat::SetClientInitData(const CClientObjCreate& create) {
    this->m_attacking = create.m_hasTarget ? create.m_target : WOWGUID();
}
