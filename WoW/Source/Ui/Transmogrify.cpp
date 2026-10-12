#include "WoW/Source/Ui/Transmogrify.h"
#include <cstdlib>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F4DE40/0x140E427F8
WOWGUID CGTransmogrify::m_transmogrifier;

// STATIC: 0x00F4DE48/0x140E427D8
std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> > CGTransmogrify::s_pendingItems;

// STATIC: 0x00C06B68/0x1409FF638
const uint32_t s_transmogrifySlots[12] = { 0, 2, 14, 4, 8, 9, 5, 6, 7, 15, 16, 17 };

// PROVEN: 1e44f72c IDENTICAL/NONE
// OFFSET: 0x0097B6C0
ClientItemRec::ClientItemRec(const ItemRecSparse* sparse) {
    if (sparse) {
        this->m_sparse = sparse;
        this->m_item = sparse->GetNonSparse();
    }
}

// PROVEN: 5a946974 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B6F0/0x1406F4D00
int32_t ClientItemRec::GetID() const {
    return this->m_item->m_ID;
}

// PROVEN: dfe1c3a1 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B700/0x1406F4D10
int32_t ClientItemRec::GetClassID() const {
    return this->m_item->m_classID;
}

// PROVEN: 23c22130 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B710/0x1406F4D20
int32_t ClientItemRec::GetSubclassID() const {
    return this->m_item->m_subclassID;
}

// PROVEN: f5956444 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B720/0x1406F4D30
int32_t ClientItemRec::GetInventoryType() const {
    return this->m_item->m_inventoryType;
}

// PROVEN: 81e9adbb IDENTICAL/IDENTICAL
// OFFSET: 0x0097B730/0x1406F4D40
int32_t ClientItemRec::GetItemLevel() const {
    return this->m_sparse->m_itemLevel;
}

// PROVEN: 754f10ca IDENTICAL/IDENTICAL
// OFFSET: 0x0097B740/0x1406F4D50
int32_t ClientItemRec::GetAllowableClass() const {
    return this->m_sparse->m_allowableClass;
}

// PROVEN: 006101a3 IDENTICAL/DIFFERENT
// OFFSET: 0x0097B750/0x1406F4D60
int32_t ClientItemRec::GetFlags(int32_t index) const {
    return this->m_sparse->m_flags[index];
}

// PROVEN: cb52931c IDENTICAL/IDENTICAL
// OFFSET: 0x0097B770/0x1406F4D70
int32_t ClientItemRec::GetNumStats() const {
    return 10;
}

// PROVEN: a308bc62 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B780/0x1406F4D80
int32_t ClientItemRec::GetStatType(int32_t index) const {
    return this->m_sparse->m_statModifierBonusStat[index];
}

// PROVEN: 4dc8e383 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B7A0/0x1406F4D90
int32_t ClientItemRec::GetStatValue(int32_t index) const {
    return this->m_sparse->m_statModifierBonusAmount[index];
}

// PROVEN: 5cfa7c70 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B7C0/0x1406F4DA0
int32_t ClientItemRec::GetNumSpells() const {
    return 5;
}

// PROVEN: f59d3bd5 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B7D0/0x1406F4DB0
int32_t ClientItemRec::GetSpellID(int32_t index) const {
    return this->m_sparse->m_spellID[index];
}

// PROVEN: 4d58afe3 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B7F0/0x1406F4DC0
int32_t ClientItemRec::GetSpellTrigger(int32_t index) const {
    return this->m_sparse->m_spellTrigger[index];
}

// PROVEN: 2c5bd5c0 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B810/0x1406F4DD0
int32_t ClientItemRec::GetQuality() const {
    return this->m_sparse->m_quality;
}

// PROVEN: 55d12332 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B820/0x1406F4DE0
int32_t ClientItemRec::GetScalingStatDistribution() const {
    return this->m_sparse->m_scalingStatDistribution;
}

// PROVEN: f3a0b21c IDENTICAL/DIFFERENT
// OFFSET: 0x009BEC70/0x14074AE10
void CGTransmogrify::InitializeGame() {
    CGTransmogrify::m_transmogrifier = WOWGUID();
}

// OFFSET: 0x009BEE40/0x14074AF30
void CGTransmogrify::SetTransmogrifier(const WOWGUID& guid) {
    if (guid == CGTransmogrify::m_transmogrifier) {
        return;
    }

    if (!guid) {
        CGGameUI::ClearInteractTarget(CGTransmogrify::m_transmogrifier, 0);
        CGTransmogrify::m_transmogrifier = WOWGUID();
        CGTransmogrify::s_pendingItems.clear();
        FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_CLOSE, NULL);
    } else {
        CGGossipInfo::ClearGossip();

        if (guid != ClntObjMgrGetActivePlayer()) {
            CGGameUI::SetInteractTarget(guid);
        }

        CGTransmogrify::m_transmogrifier = guid;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_OPEN, NULL);
    }
}

// OFFSET: 0x009BEEE0/0x14074AFF0
void CGTransmogrify::NotifySlotChanged(int32_t slot, WOWGUID itemGuid, bool notify) {
    if (notify) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_UPDATE, "%d", slot + 1);
    }

    if (!itemGuid) {
        return;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!item || !player) {
        return;
    }

    bool refundable = false;
    CGItem_C::ItemPurchaseData* purchase = item->m_itemPurchaseData;

    if (purchase) {
        uint32_t timeLeft = purchase->m_refundTimeLeft;

        if (timeLeft >= item->m_item->m_itemFieldCreatePlayedTime) {
            timeLeft = item->m_item->m_itemFieldCreatePlayedTime;
        }

        refundable = static_cast<int32_t>(timeLeft - player->GetPlayedTime()) != -7200;
    }

    bool tradeable;

    if (!item->IsBound() || item->IsPermanentlyBoundForTrade()) {
        tradeable = false;
    } else {
        tradeable = static_cast<int32_t>(item->m_item->m_itemFieldCreatePlayedTime - player->GetPlayedTime()) != -7200;
    }

    if (!item->IsBound() || refundable || tradeable) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_BIND_CONFIRM, "%d%s", slot + 1, CGTooltip::GetItemHyperlinkString(item));
    }
}

// PROVEN: 74f3d3ca IDENTICAL/DIFFERENT
// OFFSET: 0x009BF070/0x14074B170
void CGTransmogrify::ShutdownGame() {
    CGTransmogrify::SetTransmogrifier(WOWGUID());
}

// OFFSET: 0x009BF090/0x14074B190
TransmogrifyPendingItem* CGTransmogrify::GetPendingItem(uint32_t slot) {
    for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
        if (it->m_slot == slot) {
            return &*it;
        }
    }

    return NULL;
}

// OFFSET: 0x009BF0C0/0x14074B1C0
void CGTransmogrify::RemovePendingItem(int32_t slot, bool notify) {
    for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
        if (it->m_slot != static_cast<uint32_t>(slot)) {
            continue;
        }

        if (it->m_item && notify) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(it->m_item, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                SI3::PlayItemSound(0, item);
            }
        }

        CGTransmogrify::s_pendingItems.erase(it);

        if (notify) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_UPDATE, "%d", slot + 1);
        }

        return;
    }
}

// OFFSET: 0x009BF160/0x14074B290
void CGTransmogrify::SetPendingItem(int32_t slot, WOWGUID item, bool notify) {
    for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
        if (it->m_slot == static_cast<uint32_t>(slot)) {
            it->m_item = item;
            CGTransmogrify::NotifySlotChanged(slot, item, notify);
            return;
        }
    }

    TransmogrifyPendingItem pending;
    pending.m_item = item;
    pending.m_slot = slot;
    CGTransmogrify::s_pendingItems.push_back(pending);
    CGTransmogrify::NotifySlotChanged(slot, item, notify);
}

// OFFSET: 0x009BF200/0x14074B350
void CGTransmogrify::ValidatePendingItems() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin();

    while (it != CGTransmogrify::s_pendingItems.end()) {
        CGItem_C* equipped = player->m_bag.GetItemPointer(it->m_slot);
        bool keep = false;

        if (equipped) {
            if (it->m_item) {
                CGItem_C* source = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(it->m_item, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (source) {
                    ClientItemRec targetRec(equipped->GetStats());
                    ClientItemRec sourceRec(source->GetStats());
                    int32_t reason = 0;
                    keep = CanTransmogrifyItemWithItem(&sourceRec, &targetRec, equipped->m_item->m_itemFieldFlags, source->m_item->m_itemFieldRandomPropertiesId, equipped->m_item->m_itemFieldRandomPropertiesId, &reason);
                }
            } else {
                const VisibleItem_C* visible = player->GetVisibleItem(it->m_slot);
                keep = visible && equipped->m_obj->m_entryID != static_cast<uint32_t>(abs(visible->m_itemId));
            }
        }

        if (keep) {
            ++it;
            continue;
        }

        it = CGTransmogrify::s_pendingItems.erase(it);

        if (it == CGTransmogrify::s_pendingItems.end()) {
            it = CGTransmogrify::s_pendingItems.begin();
        } else {
            ++it;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_UPDATE, NULL);
}

// OFFSET: 0x009BEC80/0x14074AE20
static UIErrors GetTransmogrifyError(int32_t reason) {
    switch (reason) {
        case TRANSMOGRIFY_NO_ITEM:
            return ERR_ITEM_NOT_FOUND;
        case TRANSMOGRIFY_NOT_SOULBOUND:
            return ERR_TRANSMOGRIFY_NOT_SOULBOUND;
        case TRANSMOGRIFY_LEGENDARY:
            return ERR_TRANSMOGRIFY_LEGENDARY;
        case TRANSMOGRIFY_INVALID_TYPE:
            return ERR_TRANSMOGRIFY_INVALID_ITEM_TYPE;
        case TRANSMOGRIFY_INVALID_TARGET:
            return ERR_TRANSMOGRIFY_INVALID_DESTINATION;
        case TRANSMOGRIFY_MISMATCH:
            return ERR_TRANSMOGRIFY_MISMATCH;
        case TRANSMOGRIFY_SAME_ITEM:
            return ERR_TRANSMOGRIFY_SAME_ITEM;
        case TRANSMOGRIFY_INVALID_SOURCE:
        case TRANSMOGRIFY_NO_STATS:
            return ERR_TRANSMOGRIFY_INVALID_SOURCE;
        default:
            return ERR_MAX_VALUE;
    }
}

// OFFSET: 0x009BF3A0/0x14074B5E0
void CGTransmogrify::UseItemForSlot(CGItem_C* item, uint32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !item) {
        return;
    }

    CGItem_C* equipped = player->m_bag.GetItemPointer(slot);

    if (!equipped) {
        CGGameUI::DisplayError(ERR_TRANSMOGRIFY_NOT_EQUIPPED);
        return;
    }

    uint32_t displayId = item->GetDisplayID();
    VisibleItem_C* visible = const_cast<VisibleItem_C*>(player->GetVisibleItem(slot));
    char name[256];

    if (equipped->GetDisplayID() == displayId || (visible && visible->GetDisplayID() == displayId)) {
        CGItem_C::BuildItemName(name, sizeof(name), equipped->m_obj->m_entryID, equipped->m_item->m_itemFieldRandomPropertiesId);
        CGGameUI::DisplayError(ERR_TRANSMOGRIFY_SAME_APPEARANCE, name);
        return;
    }

    ClientItemRec targetRec(equipped->GetStats());
    ItemRecSparse* stats = item->GetStats();
    ClientItemRec sourceRec(stats);
    int32_t error = ERR_MAX_VALUE;

    if (stats && !player->CanUseItem(stats, error)) {
        CGGameUI::DisplayError(ERR_TRANSMOGRIFY_CANT_EQUIP);
        return;
    }

    int32_t reason = 0;

    if (CanTransmogrifyItemWithItem(&sourceRec, &targetRec, equipped->m_item->m_itemFieldFlags, item->m_item->m_itemFieldRandomPropertiesId, equipped->m_item->m_itemFieldRandomPropertiesId, &reason)) {
        CGTransmogrify::SetPendingItem(slot, item->m_obj->m_guid, true);
        SI3::PlayItemSound(1, item);
        CGGameUI::ClearCursor(1, 1);
        return;
    }

    UIErrors result = GetTransmogrifyError(reason);

    if (result == ERR_TRANSMOGRIFY_NOT_SOULBOUND || result == ERR_TRANSMOGRIFY_INVALID_DESTINATION) {
        CGItem_C::BuildItemName(name, sizeof(name), equipped->m_obj->m_entryID, equipped->m_item->m_itemFieldRandomPropertiesId);
        CGGameUI::DisplayError(result, name);
    } else {
        CGGameUI::DisplayError(result);
    }
}

// OFFSET: 0x009BFD90/0x14074C2B0
void CGTransmogrify::OnTransmogrifySuccess(uint32_t slot) {
    for (std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> >::iterator it = CGTransmogrify::s_pendingItems.begin(); it != CGTransmogrify::s_pendingItems.end(); ++it) {
        if (it->m_slot == slot) {
            CGTransmogrify::s_pendingItems.erase(it);
            break;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRANSMOGRIFY_SUCCESS, "%d", slot + 1);
}

// PROVEN: d0cadecf IDENTICAL/DIFFERENT
// OFFSET: 0x009BFDF0/0x14074C320
void CGTransmogrify::AutoAddItem(CGItem_C* item) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !item) {
        return;
    }

    for (uint32_t i = 0; i < 12; i++) {
        uint32_t slot = s_transmogrifySlots[i];

        if (item->CanGoInSlot(slot, 0)) {
            CGTransmogrify::UseItemForSlot(item, slot);
            return;
        }
    }

    CGGameUI::DisplayError(ERR_TRANSMOGRIFY_INVALID_ITEM_TYPE);
}
