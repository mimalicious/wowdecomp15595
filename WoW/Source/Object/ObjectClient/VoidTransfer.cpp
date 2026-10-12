#include "WoW/Source/Object/ObjectClient/VoidTransfer.h"
#include <cstring>
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00DD8660/0x140CBEDF0
CGVoidTransfer* CGVoidTransfer::s_instance;

// OFFSET: 0x005F9C20/0x14027EAF0
WOWGUID CGVoidTransfer::GetDepositItem(uint32_t slot) {
    if (slot < 9) {
        return this->m_deposits[slot];
    }

    return WOWGUID();
}

// OFFSET: 0x005F9C40/0x14027EB00
WOWGUID CGVoidTransfer::GetWithdrawItem(uint32_t slot) {
    uint32_t state = this->m_storage->m_state;

    if ((state == 2 || state == 4 || state == 3) && slot < 9) {
        return this->m_withdrawals[slot];
    }

    return WOWGUID();
}

// OFFSET: 0x005F9C80/0x14027EB30
bool CGVoidTransfer::RemoveWithdrawItem(uint32_t slot) {
    if (this->m_storage->m_state != 2) {
        return false;
    }

    if (slot > 9) {
        return false;
    }

    WOWGUID guid = this->m_withdrawals[slot];

    if (!guid.m_guid || !this->m_storage->GetItem(guid)) {
        return false;
    }

    this->m_withdrawals[slot] = WOWGUID();
    return true;
}

// OFFSET: 0x005F9CE0/0x14027EB90
bool CGVoidTransfer::IsWithdrawItem(WOWGUID guid) {
    for (uint32_t i = 0; i < 9; i++) {
        if (this->m_withdrawals[i] == guid) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005F9D20/0x14027EBB0
uint64_t CGVoidTransfer::GetDepositCost() {
    uint64_t cost = 0;

    for (uint32_t i = 0; i < 9; i++) {
        if (this->m_deposits[i].m_guid) {
            cost += 250000;
        }
    }

    return cost;
}

// OFFSET: 0x005F9DC0/0x14027EBD0
bool CGVoidTransfer::AddDepositItem(WOWGUID guid, uint32_t& slot, bool& full) {
    if (this->m_storage->m_state != 2) {
        return false;
    }

    if (slot >= 9) {
        uint32_t i;

        for (i = 0; i < 9; i++) {
            if (!this->m_deposits[i].m_guid) {
                slot = i;
                break;
            }
        }

        if (i == 9) {
            full = true;
            return false;
        }
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));
    int32_t error;

    if (!item || !item->CanBeStoredInVoidStorage(error)) {
        return false;
    }

    for (uint32_t i = 0; i < 9; i++) {
        if (this->m_deposits[slot] == guid) {
            return false;
        }
    }

    this->m_deposits[slot] = guid;
    return true;
}

// OFFSET: 0x005F9E70/0x14027ECA0
bool CGVoidTransfer::AddWithdrawItem(WOWGUID guid, uint32_t slot, bool& full) {
    if (this->m_storage->m_state != 2) {
        return false;
    }

    if (slot >= 9) {
        uint32_t i;

        for (i = 0; i < 9; i++) {
            if (!this->m_withdrawals[i].m_guid) {
                slot = i;
                break;
            }
        }

        if (i == 9) {
            full = true;
            return false;
        }
    }

    VoidItem_C* item = this->m_storage->GetItem(guid);

    if (!item || this->IsWithdrawItem(guid)) {
        return false;
    }

    this->m_withdrawals[slot] = item->m_guid;
    return true;
}

// OFFSET: 0x005F9F10/0x14027ED60
bool CGVoidTransfer::RemoveDepositItem(uint32_t slot) {
    if (this->m_storage->m_state != 2) {
        return false;
    }

    if (slot > 9) {
        return false;
    }

    WOWGUID guid = this->m_deposits[slot];

    if (!guid.m_guid || !ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__)) {
        return false;
    }

    this->m_deposits[slot] = WOWGUID();
    return true;
}

// OFFSET: 0x005F9F80/0x14027EDD0
void CGVoidTransfer::Destroy() {
    if (CGVoidTransfer::s_instance) {
        SMemFree(CGVoidTransfer::s_instance, "delete", -1, 0);
    }

    CGVoidTransfer::s_instance = NULL;
}

// OFFSET: 0x005F9FB0/0x14027EE10
CGVoidTransfer::CGVoidTransfer() {
    memset(this->m_deposits, 0, sizeof(this->m_deposits));
    memset(this->m_withdrawals, 0, sizeof(this->m_withdrawals));
    this->m_storage = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->GetVoidStorage();
}

// OFFSET: 0x005FA080/0x14027EF70
CGVoidTransfer* CGVoidTransfer::Get(bool create) {
    if (!CGVoidTransfer::s_instance && create) {
        void* mem = SMemAlloc(sizeof(CGVoidTransfer), __FILE__, __LINE__, 0);
        CGVoidTransfer::s_instance = mem ? new (mem) CGVoidTransfer() : NULL;
    }

    return CGVoidTransfer::s_instance;
}

// OFFSET: 0x005FA0C0/0x14027EFD0
bool CGVoidTransfer::SendTransfer(WOWGUID npc) {
    if (this->m_storage->m_state != 2) {
        return false;
    }

    UserClientVoidStorageTransfer msg;
    memcpy(msg.m_npc, &npc, sizeof(msg.m_npc));

    for (uint32_t i = 0; i < 9; i++) {
        if (this->m_deposits[i]) {
            msg.m_deposits.Add(1, &this->m_deposits[i].m_guid);
        }
    }

    for (uint32_t i = 0; i < 9; i++) {
        if (this->m_withdrawals[i]) {
            msg.m_withdrawals.Add(1, &this->m_withdrawals[i].m_guid);
        }
    }

    if (msg.m_deposits.m_count <= 0 && msg.m_withdrawals.m_count <= 0) {
        return false;
    }

    ClientServices::Send(&msg);
    this->m_storage->m_state = 3;

    return true;
}
