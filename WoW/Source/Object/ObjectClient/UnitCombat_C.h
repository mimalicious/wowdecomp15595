#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITCOMBAT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITCOMBAT_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;

class ATTACKROUNDINFO {
    public:
    /* 0000 */ WOWGUID m_attacker;
    /* 0008 */ WOWGUID m_target;
    /* 0010 */ uint32_t m_hitInfo;
    /* 0014 */ int32_t m_damage;
    /* 0018 */ int32_t m_overkill;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t m_schoolMask [2];
    /* 0028 */ float unk_0028 [2];
    /* 0030 */ float unk_0030 [2];
    /* 0038 */ float m_subDamage [2];
    /* 0040 */ uint32_t m_subDamageInt [2];
    /* 0048 */ uint32_t m_absorbed [2];
    /* 0050 */ uint32_t m_resisted [2];
    /* 0058 */ uint32_t m_victimState;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t m_meleeSpellId;
    /* 0064 */ uint32_t m_blocked;
    /* 0068 */ uint32_t m_rageGained;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ float unk_0070 [8];
    /* 0090 */ uint32_t unk_0090 [5];

    ATTACKROUNDINFO();
    void Unpack(CDataStore& msg);
};

void TargetAttacker(const WOWGUID& attacker);
int32_t Packet_Group_29(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

void UnitCombatClientInitialize();
void UnitCombatClientShutdown();

#endif
