#include "WoW/Source/Ui/VoidStorageInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/Object/ObjectClient/VoidTransfer.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F76858
WOWGUID CGVoidStorageInfo::m_npc;

// OFFSET: 0x009ED390/0x1407866E0
void CGVoidStorageInfo::EnterWorld() {
    CGVoidStorageInfo::m_npc = WOWGUID();
}

// OFFSET: 0x009ED3A0
void CGVoidStorageInfo::ShutdownGame() {
    CGVoidTransfer::Destroy();
}

// OFFSET: 0x009ED650/0x140786A20
bool CGVoidStorageInfo::IsItemPendingWithdraw(WOWGUID guid) {
    if (CGGameUI::m_cursorVoidStorageItem && CGGameUI::m_cursorVoidStorageItem == guid) {
        return true;
    }

    CGVoidTransfer* transfer = CGVoidTransfer::Get(false);

    return transfer && transfer->IsWithdrawItem(guid);
}

// OFFSET: 0x009ED6A0/0x140786A70
void CGVoidStorageInfo::SetNPC(const WOWGUID& guid) {
    if (guid == CGVoidStorageInfo::m_npc) {
        return;
    }

    if (!guid) {
        CGGameUI::ClearInteractTarget(CGVoidStorageInfo::m_npc, 0);
        CGVoidStorageInfo::m_npc = WOWGUID();
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGVoidStorage_C* storage = player->GetVoidStorage();

            if (storage) {
                storage->ClearNewItems();
            }
        }

        CGVoidTransfer* transfer = CGVoidTransfer::Get(false);

        if (transfer) {
            for (uint32_t i = 0; i < 9; i++) {
                WOWGUID item = transfer->GetDepositItem(i);

                if (item) {
                    CGGameUI::UnlockItem(item);
                }
            }

            CGVoidTransfer::Destroy();
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CLOSE, NULL);
        return;
    }

    CGGossipInfo::ClearGossip();

    if (guid != ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetInteractTarget(guid);
    }

    CGVoidStorageInfo::m_npc = guid;
    FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_OPEN, NULL);
}

// OFFSET: 0x009ED930/0x140786CF0
void CGVoidStorageInfo::DepositItem(CGItem_C* item, uint32_t slot, bool playSound) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || storage->m_state != 2) {
        return;
    }

    if (!item) {
        CGGameUI::DisplayError(ERR_ITEM_NOT_FOUND);
        return;
    }

    int32_t error = ERR_MAX_VALUE;

    if (!item->CanBeStoredInVoidStorage(error)) {
        CGGameUI::DisplayError(static_cast<UIErrors>(error));
        return;
    }

    CGVoidTransfer* transfer = CGVoidTransfer::Get(true);
    WOWGUID previous = transfer->GetDepositItem(slot);
    bool full = false;

    if (transfer->AddDepositItem(item->m_obj->m_guid, slot, full)) {
        if (previous) {
            CGItem_C* previousItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(previous, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (previousItem) {
                uint8_t itemSlot = CGPlayer_C::FindItemSlot(previousItem->m_item->m_itemFieldContained, previousItem);
                CGGameUI::SetCursorItem(previous, previousItem->m_item->m_itemFieldContained, itemSlot, 0, 0, 1);
            }
        } else {
            CGGameUI::ClearCursor(0, 1);
        }

        if (playSound) {
            SI3::PlayItemSound(1, item);
        }

        CGGameUI::LockItem(item->m_obj->m_guid);
        FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_DEPOSIT_UPDATE, "%d", slot + 1);
        bool enchanted = item->GetEnchantmentId(0) != 0;
        bool gemmed = item->GetEnchantmentId(2) || item->GetEnchantmentId(3) || item->GetEnchantmentId(4);
        bool prismatic = item->GetEnchantmentId(6) || item->GetEnchantmentId(7);
        bool reforged = item->GetEnchantmentId(8) != 0;
        bool transmogrified = item->GetEnchantmentId(9) != 0;
        bool refundable = false;
        CGItem_C::ItemPurchaseData* purchase = item->m_itemPurchaseData;

        if (purchase) {
            uint32_t timeLeft = purchase->m_refundTimeLeft;

            if (timeLeft >= item->m_item->m_itemFieldCreatePlayedTime) {
                timeLeft = item->m_item->m_itemFieldCreatePlayedTime;
            }

            refundable = static_cast<int32_t>(timeLeft - player->GetPlayedTime()) != -7200;
        }

        bool tradeable = false;

        if (item->IsBound() && !item->IsPermanentlyBoundForTrade()) {
            tradeable = static_cast<int32_t>(item->m_item->m_itemFieldCreatePlayedTime - player->GetPlayedTime()) != -7200;
        }

        if (enchanted || gemmed || prismatic || reforged || transmogrified || refundable || tradeable) {
            FrameScript_SignalEvent(SCRIPT_EVENT_VOID_DEPOSIT_WARNING, "%d%s", slot + 1, CGTooltip::GetItemHyperlinkString(item));
        }
        return;
    }

    if (full) {
        CGGameUI::DisplayError(ERR_VOID_DEPOSIT_FULL);
    }
}

// OFFSET: 0x009EDF20/0x140787410
void CGVoidStorageInfo::LeaveWorld() {
    CGVoidStorageInfo::SetNPC(WOWGUID());
}

// OFFSET: 0x009EDF40/0x140787430
void CGVoidStorageInfo::AutoDepositItem(CGItem_C* item) {
    if (item && !(item->m_itemFlags & 1)) {
        CGVoidStorageInfo::DepositItem(item, 9, true);
    }
}
