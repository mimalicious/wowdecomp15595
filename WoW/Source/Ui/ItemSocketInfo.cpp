#include "ItemSocketInfo.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F4A290/0x140E3E3C0
uint8_t CGItemSocketInfo::m_gemContainerSlot[3];

// STATIC: 0x00F4A298/0x140E3E3C8
WOWGUID CGItemSocketInfo::m_gemItemContainer[3];

// STATIC: 0x00F4A2B0/0x140E3E3E0
WOWGUID CGItemSocketInfo::m_gemItems[3];

// STATIC: 0x00F4A2C8/0x140E3E3F8
WOWGUID CGItemSocketInfo::m_socketItem;

// STATIC: 0x00F4A2D0/0x140E3E400
ItemRecSparse* CGItemSocketInfo::m_socketItemRec;

// OFFSET: 0x0097A150
void CGItemSocketInfo::GetGemItemForSlot(uint32_t slot, WOWGUID& item, WOWGUID& container, uint8_t& containerSlot) {
    if (slot >= 3) {
        item = WOWGUID();
        container = WOWGUID();
        containerSlot = 0;
    } else {
        item = CGItemSocketInfo::m_gemItems[slot];
        container = CGItemSocketInfo::m_gemItemContainer[slot];
        containerSlot = CGItemSocketInfo::m_gemContainerSlot[slot];
    }
}

// PROVEN: 4331ca5e IDENTICAL/IDENTICAL
// OFFSET: 0x0097A1B0/0x1406F3730
void EnchantItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_UPDATE, NULL);
    }
}

// PROVEN: 0571ab78 IDENTICAL/UNPROVEN
// OFFSET: 0x0097A1D0/0x1406F3750
void CGItemSocketInfo::EnterWorld() {
    CGItemSocketInfo::m_socketItem = WOWGUID::Make(0);
    CGItemSocketInfo::m_socketItemRec = NULL;
    CGItemSocketInfo::m_gemItems[0] = WOWGUID::Make(0);
    CGItemSocketInfo::m_gemItems[1] = WOWGUID::Make(0);
    CGItemSocketInfo::m_gemItems[2] = WOWGUID::Make(0);
    CGItemSocketInfo::m_gemItemContainer[0] = WOWGUID::Make(0);
    CGItemSocketInfo::m_gemItemContainer[1] = WOWGUID::Make(0);
    CGItemSocketInfo::m_gemItemContainer[2] = WOWGUID::Make(0);
    memset(CGItemSocketInfo::m_gemContainerSlot, 0, sizeof(((CGItemSocketInfo*)0)->m_gemContainerSlot));
}

// PROVEN: 7d5af11b IDENTICAL/UNPROVEN
// OFFSET: 0x0097A310/0x1406F38C0
int32_t CGItemSocketInfo::GemMatchesSocket(const ItemRecSparse* item, const ItemRecSparse* gem, int32_t socket) {
    if (!item || !gem) {
        return 0;
    }

    int32_t color = item->m_socketColor[socket];

    if (!color) {
        return 1;
    }

    GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(gem->m_gemProperties);

    if (properties) {
        return (color & properties->m_type) != 0;
    }

    return 0;
}

// PROVEN: 0dbfd0f7 IDENTICAL/DIFFERENT
// OFFSET: 0x0097A370/0x1406F3B60
bool CGItemSocketInfo::IsSocketItemBoundTradeable() {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        return item->IsBound() && !item->IsBoundForTrade();
    }

    return false;
}

// PROVEN: e2c3abdf IDENTICAL/DIFFERENT
// OFFSET: 0x0097A660/0x1406F3BF0
void CGItemSocketInfo::UpdateItem(WOWGUID item) {
    if (CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        player->CountEquippedGems();
    }

    if (item == CGItemSocketInfo::m_socketItem) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_UPDATE, NULL);
    }
}

// OFFSET: 0x0097A6B0/0x1406F3C40
void CGItemSocketInfo::Accept() {
    if (!CGItemSocketInfo::m_socketItem) {
        return;
    }

    int32_t count = 0;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SOCKET_GEMS));
    msg.Put(CGItemSocketInfo::m_socketItem.m_guid);

    for (uint32_t i = 0; i < 3; i++) {
        if (CGItemSocketInfo::m_gemItems[i]) {
            count++;
        }

        msg.Put(CGItemSocketInfo::m_gemItems[i].m_guid);
    }

    if (count) {
        msg.Finalize();
        ClientServices::Send2(&msg);
        FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_ACCEPT, NULL);
    }
}

// PROVEN: 1059532b IDENTICAL/DIFFERENT
// OFFSET: 0x0097A7B0/0x1406F3D50
bool CGItemSocketInfo::IsSocketItemRefundable() {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        CGItem_C::ItemPurchaseData* purchase = item->m_itemPurchaseData;

        if (purchase) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
            return static_cast<int32_t>(purchase->m_refundTimeLeft - player->GetPlayedTime() + 7200) > 0;
        }
    }

    return false;
}

// OFFSET: 0x0097A870/0x1406F3E20
void CGItemSocketInfo::SetItem(WOWGUID item) {
    CGGameUI::UnlockItem(CGItemSocketInfo::m_socketItem);

    for (uint32_t i = 0; i < 3; i++) {
        if (CGItemSocketInfo::m_gemItems[i]) {
            CGGameUI::UnlockItem(CGItemSocketInfo::m_gemItems[i]);
        }
    }

    CGItemSocketInfo::m_gemItems[0].m_guid = 0;
    CGItemSocketInfo::m_gemItems[1].m_guid = 0;
    CGItemSocketInfo::m_gemItems[2].m_guid = 0;
    CGItemSocketInfo::m_socketItemRec = NULL;
    CGItemSocketInfo::m_socketItem = item;

    if (CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__))) {
        WOWGUID requester = WOWGUID::Make(0);
        CGItemSocketInfo::m_socketItemRec = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, requester, NULL, NULL, false);
    }

    if (!CGItemSocketInfo::m_socketItemRec || !item) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_CLOSE, NULL);
        return;
    }

    CGGameUI::LockItem(item);
    FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_UPDATE, NULL);
}

// OFFSET: 0x0097A970/0x1406F3F20
int32_t CGItemSocketInfo::SetGem(uint32_t slot, WOWGUID item, WOWGUID container, uint8_t containerSlot) {
    if (slot >= 3 || !CGItemSocketInfo::m_socketItem || !CGItemSocketInfo::m_socketItemRec) {
        return 0;
    }

    if (!item) {
        CGItemSocketInfo::m_gemItems[slot].m_guid = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_UPDATE, NULL);
        return 1;
    }

    CGItem_C* gem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!gem) {
        return 0;
    }

    WOWGUID requester = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(gem->m_obj->m_entryID, requester, NULL, NULL, false);

    if (!rec || !rec->m_gemProperties) {
        return 0;
    }

    if (GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(rec->m_gemProperties)) {
        int32_t color = CGItemSocketInfo::m_socketItemRec->m_socketColor[slot];
        int32_t type = properties->m_type;

        if ((color & 0x1) != (type & 0x1)) {
            CGGameUI::DisplayError((color & 0x1) ? ERR_SOCKETING_REQUIRES_META_GEM : ERR_SOCKETING_META_GEM_ONLY_IN_METASLOT);
            return 0;
        }

        if ((color & 0x10) != (type & 0x10)) {
            CGGameUI::DisplayError((color & 0x10) ? ERR_SOCKETING_REQUIRES_HYDRAULIC_GEM : ERR_SOCKETING_HYDRAULIC_GEM_ONLY_IN_HYDRAULICSLOT);
            return 0;
        }

        if ((color & 0x20) != (type & 0x20)) {
            CGGameUI::DisplayError((color & 0x20) ? ERR_SOCKETING_REQUIRES_COGWHEEL_GEM : ERR_SOCKETING_COGWHEEL_GEM_ONLY_IN_COGWHEELSLOT);
            return 0;
        }

        if (properties->m_minItemLevel && CGItemSocketInfo::m_socketItemRec->m_itemLevel < properties->m_minItemLevel) {
            CGGameUI::DisplayError(ERR_SOCKETING_ITEM_TOO_LOW_LEVEL);
            return 0;
        }
    }

    CGItemSocketInfo::m_gemItems[slot] = item;
    CGItemSocketInfo::m_gemItemContainer[slot] = container;
    CGItemSocketInfo::m_gemContainerSlot[slot] = containerSlot;
    FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_UPDATE, NULL);
    return 1;
}

// OFFSET: 0x0097AB60/0x1406F40E0
void CGItemSocketInfo::RemoveItem(WOWGUID item) {
    if (item == CGItemSocketInfo::m_socketItem) {
        CGItemSocketInfo::SetItem(WOWGUID::Make(0));
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (item == CGItemSocketInfo::m_gemItems[i]) {
            if (CGItemSocketInfo::m_socketItem && CGItemSocketInfo::m_socketItemRec) {
                CGItemSocketInfo::m_gemItems[i] = WOWGUID::Make(0);
                FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_UPDATE, NULL);
            }

            break;
        }
    }

    if (CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        player->CountEquippedGems();
    }
}

// PROVEN: c24c11d0 DIFFERENT/IDENTICAL
// OFFSET: 0x0097B1E0/0x1406F47E0
void CGItemSocketInfo::LeaveWorld() {
    CGItemSocketInfo::SetItem(WOWGUID::Make(0));
}
