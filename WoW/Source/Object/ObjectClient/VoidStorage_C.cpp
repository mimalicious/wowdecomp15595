#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VoidTransfer.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/VoidStorageInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x005958C0/0x140202E60
CGVoidStorage_C::CGVoidStorage_C() {
    this->m_state = 0;
    this->m_items = NULL;
}

// PROVEN: 2795fbcd IDENTICAL/DIFFERENT
// OFFSET: 0x00595BB0/0x140203210
CGVoidStorage_C::~CGVoidStorage_C() {
    if (this->m_items) {
        this->m_items->InternalClear(0);
        delete this->m_items;
    }
}

// OFFSET: 0x005958E0/0x140202E80
VoidItem_C* CGVoidStorage_C::GetItemBySlot(uint32_t slot) {
    if (!this->m_items || slot >= 80) {
        return NULL;
    }

    return this->m_items->Ptr(slot, HASHKEY_NONE());
}

// OFFSET: 0x00595910/0x140202EF0
VoidItem_C* CGVoidStorage_C::GetItem(WOWGUID guid) {
    TSHashTable<VoidItem_C, HASHKEY_NONE>* items = this->m_items;

    if (!items) {
        return NULL;
    }

    for (VoidItem_C* item = items->Head(); item; item = items->Next(item)) {
        if (item->m_guid == guid) {
            return item;
        }
    }

    return NULL;
}

// PROVEN: c4706133 IDENTICAL/IDENTICAL
// OFFSET: 0x00595630/0x140202B40
void CGVoidStorage_C::Query() {
    if (this->m_state) {
        return;
    }

    UserClientVoidStorageQuery msg;
    memcpy(msg.m_npc, &CGVoidStorageInfo::m_npc, sizeof(msg.m_npc));
    ClientServices::Send(&msg);
    this->m_state = 1;
}

// OFFSET: 0x00595680/0x140202B90
void CGVoidStorage_C::OnTransferResult(uint32_t result) {
    if (this->m_state == 3) {
        this->m_state = 2;
    } else {
        this->m_state = 0;
        CGVoidStorageInfo::SetNPC(WOWGUID());
    }

    switch (result) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 8:
            CGGameUI::DisplayError(ERR_VOID_TRANSFER_INTERNAL_ERROR);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_VOID_TRANSFER_STORAGE_FULL);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_VOID_TRANSFER_INV_FULL);
            break;

        case 9:
            ConsoleWrite("VOID_TRANSFER_UNKNOWN", DEFAULT_COLOR);
            break;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_VOID_TRANSFER_DONE, NULL);
}

// OFFSET: 0x005957B0/0x140202CF0
static bool OnVoidStorageFailed(ClientVoidStorageFailed* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        CGVoidStorage_C* storage = player->GetVoidStorage();
        CGGameUI::DisplayError(ERR_VOID_STORAGE_LOAD_FAILED);
        storage->m_state = 0;
        CGVoidStorageInfo::SetNPC(WOWGUID());
    }

    return true;
}

// OFFSET: 0x00595810/0x140202D60
static bool OnVoidTransferResult(ClientVoidTransferResult* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->GetVoidStorage()->OnTransferResult(msg->m_result);
    }

    return true;
}

// PROVEN: 4d78b5a8 IDENTICAL/UNPROVEN
// OFFSET: 0x00595750/0x140202C90
void VoidStorage_C_Shutdown() {
    ClientVoidStorageContents::s_handler = NULL;
    ClientVoidStorageFailed::s_handler = NULL;
    ClientVoidStorageTransferChanges::s_handler = NULL;
    ClientVoidTransferResult::s_handler = NULL;
    ClientVoidItemSwapResponse::s_handler = NULL;
    CGVoidTransfer::Destroy();
}

// PROVEN: 77024fcf IDENTICAL/IDENTICAL
// OFFSET: 0x00595770/0x140202CC0
bool CGVoidStorage_C::IsNewItem(WOWGUID guid) {
    for (uint32_t i = 0; i < this->m_newItems.m_count; i++) {
        if (this->m_newItems.m_data[i] == guid.m_guid) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00595970/0x140202F50
bool CGVoidStorage_C::SwapItem(WOWGUID guid, uint32_t slot) {
    if (this->m_state != 2 || !this->m_items || slot >= 80 || !this->GetItem(guid)) {
        return false;
    }

    UserClientVoidSwapItem msg;
    memcpy(msg.m_npc, &CGVoidStorageInfo::m_npc, sizeof(msg.m_npc));
    memcpy(msg.m_item, &guid, sizeof(msg.m_item));
    msg.m_slot = slot;
    ClientServices::Send(&msg);
    this->m_state = 4;

    return true;
}

// PROVEN: abf3f12b IDENTICAL/IDENTICAL
// OFFSET: 0x00595A00/0x140202FF0
void CGVoidStorage_C::ClearNewItems() {
    this->m_newItems.Clear();
}

// OFFSET: 0x00595C00
bool CGVoidStorage_C::RemoveItem(WOWGUID guid) {
    TSHashTable<VoidItem_C, HASHKEY_NONE>* items = this->m_items;

    if (!items) {
        return false;
    }

    for (VoidItem_C* item = items->Head(); item; item = items->Next(item)) {
        if (item->m_guid == guid) {
            items->Delete(item);
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005963D0/0x140203DE0
bool CGVoidStorage_C::AddItem(const JamVoidItem& item) {
    if (!this->m_items) {
        return false;
    }

    uint32_t slot = item.m_slot;

    if (slot >= 80 || this->m_items->Ptr(slot, HASHKEY_NONE())) {
        return false;
    }

    VoidItem_C* entry = this->m_items->New(slot, HASHKEY_NONE(), 0, 0);
    memcpy(&entry->m_guid, item.m_guid, sizeof(entry->m_guid));
    entry->m_entryID = item.m_itemID;
    entry->m_randomPropertiesId = item.m_randomPropertiesId;
    entry->m_propertySeed = item.m_propertySeed;
    memcpy(&entry->m_creator, item.m_creator, sizeof(entry->m_creator));

    return true;
}

// OFFSET: 0x00596440
void CGVoidStorage_C::OnSwapResponse(WOWGUID firstItem, uint32_t firstSlot, WOWGUID secondItem, uint32_t secondSlot) {
    if (firstItem) {
        VoidItem_C* first = this->GetItem(firstItem);
        this->m_items->Unlink(first);

        if (secondItem) {
            VoidItem_C* second = this->GetItem(secondItem);
            this->m_items->Unlink(second);
            this->m_items->Insert(second, secondSlot, HASHKEY_NONE());
        }

        this->m_items->Insert(first, firstSlot, HASHKEY_NONE());
    }

    if (this->m_state == 4) {
        this->m_state = 2;
        FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CONTENTS_UPDATE, NULL);
    } else {
        this->m_state = 0;
        CGVoidStorageInfo::SetNPC(WOWGUID());
    }
}

// OFFSET: 0x005964F0
static bool OnVoidItemSwapResponse(ClientVoidItemSwapResponse* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->GetVoidStorage()->OnSwapResponse(WOWGUID::Make(msg->m_firstItem), msg->m_firstSlot, WOWGUID::Make(msg->m_secondItem), msg->m_secondSlot);
    }

    return true;
}

// OFFSET: 0x00596540/0x140204060
void CGVoidStorage_C::SetContents(const TSGrowableArray<JamVoidItem>& items) {
    this->m_state = 2;

    if (this->m_items) {
        this->m_items->InternalClear(0);
    } else {
        void* mem = SMemAlloc(sizeof(TSHashTable<VoidItem_C, HASHKEY_NONE>), __FILE__, __LINE__, 0);
        this->m_items = mem ? new (mem) TSHashTable<VoidItem_C, HASHKEY_NONE>() : NULL;
    }

    for (uint32_t i = 0; i < items.m_count; i++) {
        WOWGUID none = WOWGUID();
        this->AddItem(items.m_data[i]);
        g_itemSparseDB.GetRecord(items.m_data[i].m_itemID, none, NULL, NULL, false);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_UPDATE, NULL);
}

// OFFSET: 0x005965F0
void CGVoidStorage_C::OnTransferChanges(const TSGrowableArray<uint64_t>& removed, const TSGrowableArray<JamVoidItem>& added) {
    for (uint32_t i = 0; i < removed.m_count; i++) {
        this->RemoveItem(WOWGUID::Make(removed.m_data[i]));
    }

    this->m_newItems.Clear();

    for (uint32_t i = 0; i < added.m_count; i++) {
        this->AddItem(added.m_data[i]);
        WOWGUID guid;
        memcpy(&guid, added.m_data[i].m_guid, sizeof(guid));
        *this->m_newItems.New() = guid.m_guid;
    }
}

// OFFSET: 0x00596840/0x140204280
static bool OnVoidStorageContents(ClientVoidStorageContents* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->GetVoidStorage()->SetContents(msg->m_items);
    }

    return true;
}

// OFFSET: 0x00596880
static bool OnVoidStorageTransferChanges(ClientVoidStorageTransferChanges* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->GetVoidStorage()->OnTransferChanges(msg->m_removed, msg->m_added);
    }

    return true;
}

// OFFSET: 0x005968C0/0x140204320
void VoidStorage_C_Initialize() {
    ClientVoidStorageContents::s_handler = JamHandlerEncode(OnVoidStorageContents, g_jamHandlerKey_CFF6D4);
    ClientVoidStorageFailed::s_handler = JamHandlerEncode(OnVoidStorageFailed, g_jamHandlerKey_CFF164);
    ClientVoidStorageTransferChanges::s_handler = JamHandlerEncode(OnVoidStorageTransferChanges, g_jamHandlerKey_CFD540);
    ClientVoidTransferResult::s_handler = JamHandlerEncode(OnVoidTransferResult, g_jamHandlerKey_CFE088);
    ClientVoidItemSwapResponse::s_handler = JamHandlerEncode(OnVoidItemSwapResponse, g_jamHandlerKey_CFE448);
}
