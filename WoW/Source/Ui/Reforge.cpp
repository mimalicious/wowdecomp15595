#include "WoW/Source/Ui/Reforge.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemReforgeRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F4B0C8
WOWGUID CGReforge::m_forgeMaster;

// STATIC: 0x00F4B0D0
WOWGUID CGReforge::m_reforgeItem;

// STATIC: 0x00F4B0D8
TSCGrowableArray<REFORGE_OPTION, 16> CGReforge::s_reforgeOptions;

// STATIC: 0x00D14528
static ThrottleObject s_reforgeThrottle(1, 1);

// OFFSET: 0x0098B510
void CGReforge::UpdateEnchantment(WOWGUID item) {
    if (CGReforge::m_forgeMaster && CGReforge::m_reforgeItem && CGReforge::m_reforgeItem == item) {
        FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_ITEM_CHANGED, NULL);
    }
}

// OFFSET: 0x0098B5D0
uint32_t CGReforge::GetNumOptions() {
    return CGReforge::s_reforgeOptions.Count();
}

// OFFSET: 0x0098B5E0/0x140709410
const ItemReforgeRec* CGReforge::GetOption(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGReforge::s_reforgeOptions.Count())) {
        return NULL;
    }

    return CGReforge::s_reforgeOptions[index].m_reforge;
}

// OFFSET: 0x0098B630
void CGReforge::ReforgeItem(int32_t index) {
    WOWGUID itemGuid = CGReforge::m_reforgeItem;

    if (!CGReforge::m_reforgeItem || !CGReforge::m_forgeMaster || index < -1) {
        return;
    }

    const ItemReforgeRec* reforge = NULL;

    if (index != -1) {
        if (index >= static_cast<int32_t>(CGReforge::s_reforgeOptions.Count())) {
            return;
        }

        reforge = CGReforge::s_reforgeOptions[index].m_reforge;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
    ItemRecSparse* stats = item ? item->GetStats() : NULL;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if ((index != -1 && !reforge) || !stats || !item) {
        return;
    }

    int32_t reason;

    if (index != -1 && !item->ReforgeIsValid(reforge, reason)) {
        return;
    }

    if (!player) {
        return;
    }

    uint32_t cost = stats->GetSellPrice();

    if (cost) {
        if (cost <= 10000) {
            cost = 10000;
        }
    } else {
        cost = 100000;
    }

    if (index != -1 && player->GetCoinage() < cost) {
        return;
    }

    UserClientReforgeItem msg;
    msg.m_flags |= 1;
    memcpy(msg.m_forgeMaster, &CGReforge::m_forgeMaster, sizeof(msg.m_forgeMaster));
    msg.m_flags |= 8;
    msg.m_reforgeId = index != -1 ? reforge->m_id : 0;
    msg.m_flags |= 2;
    msg.m_slot = CGPlayer_C::FindItemSlot(item->m_item->m_itemFieldContained, item);
    WOWGUID container = item->m_item->m_itemFieldContained;
    msg.m_flags |= 4;
    msg.m_bag = container == player->m_obj->m_guid ? 0xFF : player->FindSlotIndex(container);

    if (s_reforgeThrottle.IncrementAndCheck()) {
        ClientServices::Send(&msg);
    }
}

// OFFSET: 0x0098B820/0x1407096B0
int32_t CGReforge::FindOption(int32_t reforgeId) {
    for (uint32_t i = 0; i < CGReforge::s_reforgeOptions.Count(); i++) {
        if (reforgeId == CGReforge::s_reforgeOptions[i].m_reforge->m_id) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x0098CBA0
void CGReforge::ShutdownGame() {
    CGReforge::m_forgeMaster = WOWGUID();
    CGReforge::m_reforgeItem = WOWGUID();
    CGReforge::s_reforgeOptions.SetCount(0);
}

// OFFSET: 0x0098CBD0/0x14070AF10
bool CGReforge::UpdateOptions() {
    CGReforge::s_reforgeOptions.SetCount(0);
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGReforge::m_reforgeItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return false;
    }

    if (g_itemSparseDB.GetRecord(item->m_obj->m_entryID, CGReforge::m_reforgeItem, CGReforge::ItemStatsCallback, NULL, false)) {
        for (int32_t i = 0; i < g_itemReforgeDB.m_numRecords; i++) {
            const ItemReforgeRec* reforge = i < 0 || i >= g_itemReforgeDB.m_numRecords ? NULL : g_itemReforgeDB.GetRecordByIndex(i);
            int32_t reason;

            if (item->ReforgeIsValid(reforge, reason)) {
                CGReforge::s_reforgeOptions.New()->m_reforge = reforge;
            }
        }
    }

    return CGReforge::s_reforgeOptions.Count() != 0;
}

// OFFSET: 0x0098CCA0/0x14070B080
void CGReforge::ItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success && guid == CGReforge::m_reforgeItem && CGReforge::UpdateOptions()) {
        CGGameUI::LockItem(CGReforge::m_reforgeItem);
        FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_SET_ITEM, NULL);
    }
}

// OFFSET: 0x0098CCF0/0x14070B0C0
bool CGReforge::CanReforge(WOWGUID guid, int32_t* reasons) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return false;
    }

    if (!g_itemSparseDB.GetRecord(item->m_obj->m_entryID, CGReforge::m_reforgeItem, CGReforge::ItemStatsCallback, NULL, false)) {
        return false;
    }

    for (int32_t i = 0; i < g_itemReforgeDB.m_numRecords; i++) {
        const ItemReforgeRec* reforge = i < 0 || i >= g_itemReforgeDB.m_numRecords ? NULL : g_itemReforgeDB.GetRecordByIndex(i);
        int32_t reason;

        if (item->ReforgeIsValid(reforge, reason)) {
            return true;
        }

        reasons[reason]++;
    }

    return false;
}

// OFFSET: 0x0098CDA0/0x14070B1B0
bool CGReforge::SetItem(WOWGUID guid) {
    if (!CGReforge::m_forgeMaster) {
        return false;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        CGGameUI::UnlockItem(CGReforge::m_reforgeItem);
        CGReforge::m_reforgeItem = WOWGUID();
        FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_SET_ITEM, NULL);
        return false;
    }

    int32_t reasons[5];
    memset(reasons, 0, sizeof(reasons));
    bool valid = true;

    if (!CGReforge::CanReforge(guid, reasons)) {
        for (int32_t i = 0; i < 5; i++) {
            if (!reasons[i]) {
                continue;
            }

            switch (i - 1) {
                case 0:
                    CGGameUI::DisplayError(ERR_REFORGE_SRC_STAT_NOT_FOUND);
                    valid = false;
                    break;

                case 1:
                    CGGameUI::DisplayError(ERR_REFORGE_INSUFFICIENT_SRC_STAT);
                    valid = false;
                    break;

                case 2:
                    CGGameUI::DisplayError(ERR_REFORGE_ALREADY_HAS_DST_STAT);
                    valid = false;
                    break;

                case 3:
                    CGGameUI::DisplayError(ERR_REFORGE_ITEM_TOO_LOW_LEVEL);
                    valid = false;
                    break;
            }
        }

        if (!valid) {
            CGGameUI::UnlockItem(guid);
            return false;
        }
    }

    WOWGUID old = CGReforge::m_reforgeItem;
    CGReforge::m_reforgeItem = guid;
    CGReforge::UpdateOptions();
    CGGameUI::UnlockItem(old);
    CGGameUI::LockItem(CGReforge::m_reforgeItem);
    SI3::PlayItemSound(1, item);
    FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_SET_ITEM, NULL);

    return valid;
}

// OFFSET: 0x0098D030/0x14070B470
void CGReforge::SetForgeMaster(const WOWGUID& guid) {
    if (guid == CGReforge::m_forgeMaster) {
        return;
    }

    if (!guid) {
        FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_CLOSED, NULL);
        CGGameUI::ClearInteractTarget(CGReforge::m_forgeMaster, 0);
        CGReforge::SetItem(WOWGUID());
        CGReforge::m_forgeMaster = WOWGUID();
        CGReforge::m_reforgeItem = WOWGUID();
        CGReforge::s_reforgeOptions.SetCount(0);
    } else {
        CGGossipInfo::ClearGossip();

        if (guid != ClntObjMgrGetActivePlayer()) {
            CGGameUI::SetInteractTarget(guid);
        }

        CGReforge::m_forgeMaster = guid;
        FrameScript_SignalEvent(SCRIPT_EVENT_FORGE_MASTER_OPENED, NULL);
    }
}

// OFFSET: 0x0098D100
void CGReforge::LeaveWorld() {
    CGReforge::SetForgeMaster(WOWGUID::Make(0));
    ClientReforgeResult::s_handler = NULL;
    ClientForgeMasterSet::s_handler = NULL;
}

// OFFSET: 0x0098D130
static bool OnForgeMasterSet(ClientForgeMasterSet* msg) {
    CGReforge::SetForgeMaster(*reinterpret_cast<const WOWGUID*>(msg->m_forgeMaster));
    return true;
}

// OFFSET: 0x00A3AA30/0x1404BDD20
static bool OnReforgeResult(ClientReforgeResult* msg) {
    return true;
}

// OFFSET: 0x0098D150/0x14070B610
void CGReforge::EnterWorld() {
    CGReforge::m_forgeMaster = WOWGUID();
    CGReforge::m_reforgeItem = WOWGUID();
    ClientReforgeResult::s_handler = OnReforgeResult;
    ClientForgeMasterSet::s_handler = OnForgeMasterSet;
}
