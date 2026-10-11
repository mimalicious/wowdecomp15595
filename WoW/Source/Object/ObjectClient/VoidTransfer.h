#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDTRANSFER_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDTRANSFER_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGVoidStorage_C;

class CGVoidTransfer {
    public:
    static CGVoidTransfer* s_instance;

    static CGVoidTransfer* Get(bool create);
    static void Destroy();

    /* 0000 */ WOWGUID m_deposits[9];
    /* 0048 */ WOWGUID m_withdrawals[9];
    /* 0090 */ CGVoidStorage_C* m_storage;
    /* 0094 */ uint32_t unk_0094;

    CGVoidTransfer();
    WOWGUID GetDepositItem(uint32_t slot);
    WOWGUID GetWithdrawItem(uint32_t slot);
    bool RemoveWithdrawItem(uint32_t slot);
    bool IsWithdrawItem(WOWGUID guid);
    uint64_t GetDepositCost();
    bool AddDepositItem(WOWGUID guid, uint32_t& slot, bool& full);
    bool AddWithdrawItem(WOWGUID guid, uint32_t slot, bool& full);
    bool RemoveDepositItem(uint32_t slot);
    bool SendTransfer(WOWGUID npc);
};

#endif
