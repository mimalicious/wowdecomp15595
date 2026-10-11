#include "EquipmentManager.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include <new>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00F4B978
int32_t CGEquipmentManager::m_ignoredSlots[19];

// STATIC: 0x00F4B9C4
uint8_t CGEquipmentManager::m_lastCanUseEquipmentSetsValue;

// STATIC: 0x00F4B9C5
uint8_t CGEquipmentManager::m_loaded;

// STATIC: 0x00F4B9C6
uint8_t CGEquipmentManager::m_swapPending;

// STATIC: 0x00F4B9C8
int32_t CGEquipmentManager::m_pendingSetID;

// STATIC: 0x00F4B9CC
TSObjectAllocPool<EQUIPMENT_SET>* CGEquipmentManager::s_equipmentSetPool;

// STATIC: 0x00D15060
TSList<EQUIPMENT_SET_NODE> CGEquipmentManager::s_equipmentSets;

// INLINED
EQUIPMENT_SET::EQUIPMENT_SET() {
    this->m_guid = WOWGUID::Make(0);
    memset(this->m_items, 0, sizeof(this->m_items));
    this->m_unsaved = 0;
}

// PROVEN: b695294d IDENTICAL/NONE
// OFFSET: 0x009A1280/0x140725A20
void CGEquipmentManager::EnterWorld() {
    CGEquipmentManager::m_swapPending = 0;
    CGEquipmentManager::m_lastCanUseEquipmentSetsValue = 0;
    CGEquipmentManager::m_pendingSetID = 0;
}

// OFFSET: 0x009A12A0/0x140725A40
int32_t CGEquipmentManager::CanUseEquipmentSets(bool updateActionBar) {
    uint8_t canUse = CGEquipmentManager::m_swapPending == 0;

    if (CGEquipmentManager::m_lastCanUseEquipmentSetsValue != canUse) {
        CGEquipmentManager::m_lastCanUseEquipmentSetsValue = canUse;

        if (updateActionBar) {
            CGActionBar::UpdateUsable();
        }
    }

    return canUse;
}

// PROVEN: 243b4596 IDENTICAL/NONE
// OFFSET: 0x009A12D0/0x140725B20
void CGEquipmentManager::ClearIgnoredSlots() {
    for (uint32_t i = 0; i < 19; i++) {
        CGEquipmentManager::m_ignoredSlots[i] = 0;
    }
}

// OFFSET: 0x009A14C0/0x140725CA0
int32_t CGEquipmentManager::SetContainsLockedItems(const EQUIPMENT_SET* set) {
    if (!set) {
        return 0;
    }

    for (uint32_t i = 0; i <= 18; i++) {
        if (set->m_items[i].m_guid > 1) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(set->m_items[i], TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && (item->m_itemFlags & 0x1) && !CGMerchantInfo::IsBuybackItem(set->m_items[i])) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x009A15D0/0x140725D30
bool CGEquipmentManager::UseEquipmentSet(const EQUIPMENT_SET* set) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    if (!CGEquipmentManager::CanUseEquipmentSets(true) || CGGameUI::GetCursorItem()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", FrameScript_GetText("SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW", -1, GENDER_NOT_APPLICABLE));
        return false;
    }

    if (!CGGameUI::CanPerformAction(22)) {
        return false;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_EQUIPMENT_SET_USE));
    bool inCombat = (player->m_unit->m_unitFieldFlags & 0x80000) != 0;
    bool combatWarned = false;
    bool missingWarned = false;
    char buffer[3000];

    for (uint32_t slot = 0; slot <= 18; slot++) {
        WOWGUID guid = set->m_items[slot];
        SmartGUID itemGuid;
        itemGuid.m_guid = 1;
        uint8_t bag = 0;
        uint8_t bagSlot = 0;

        if ((inCombat && slot != 15 && slot != 16 && slot != 17) || guid.m_guid == 1) {
            if (inCombat && !combatWarned && guid.m_guid != 1 && guid.m_guid != player->m_bag.GetItemGuid(slot)) {
                SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("EQUIPMENT_MANAGER_COMBAT_SWAP", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(set->m_name));
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", buffer);
                combatWarned = true;
            }
        } else if (!guid) {
            itemGuid.m_guid = 0;
        } else {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                bagSlot = CGPlayer_C::FindItemSlot(ClntObjMgrGetActivePlayer(), item);

                if (static_cast<uint8_t>(bagSlot - 74) <= 11) {
                    bagSlot = 0;
                } else if (static_cast<uint8_t>(bagSlot - 39) <= 27 && !CGBankInfo::m_banker) {
                    bagSlot = 0;
                } else if (bagSlot != 0xFF) {
                    bag = 0xFF;
                    itemGuid.m_guid = guid.m_guid;
                } else {
                    for (uint32_t i = 19; i <= 22; i++) {
                        WOWGUID container = CGContainerInfo::GetContainer(i - 19);

                        if (container) {
                            bagSlot = CGPlayer_C::FindItemSlot(container, item);

                            if (bagSlot != 0xFF) {
                                bag = static_cast<uint8_t>(i);
                                itemGuid.m_guid = guid.m_guid;
                                break;
                            }
                        }
                    }

                    if (itemGuid.m_guid == 1 && CGBankInfo::m_banker) {
                        for (uint32_t i = 67; i <= 73; i++) {
                            WOWGUID container = CGContainerInfo::GetContainer(i - 63);

                            if (container) {
                                bagSlot = CGPlayer_C::FindItemSlot(container, item);

                                if (bagSlot != 0xFF) {
                                    bag = static_cast<uint8_t>(i);
                                    itemGuid.m_guid = guid.m_guid;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (itemGuid.m_guid == 1 && !missingWarned) {
                SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("EQUIPMENT_MANAGER_MISSING_ITEM", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(set->m_name));
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", buffer);
                missingWarned = true;
            }
        }

        msg << itemGuid;
        msg.Put(bag);
        msg.Put(bagSlot);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    CGEquipmentManager::m_swapPending = 1;
    CGEquipmentManager::m_pendingSetID = set->m_id;
    FrameScript_SignalEvent(SCRIPT_EVENT_EQUIPMENT_SWAP_PENDING, NULL);
    CGActionBar::UpdateUsable();
    return true;
}

// OFFSET: 0x009A1990
void CGEquipmentManager::SaveSet(EQUIPMENT_SET* set, const char* name, const char* icon) {
    if (set->m_unsaved) {
        return;
    }

    SStrCopy(set->m_name, name, sizeof(set->m_name));
    SStrCopy(set->m_icon, icon, sizeof(set->m_icon));
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    for (uint32_t slot = 0; slot <= 18; slot++) {
        if (CGEquipmentManager::m_ignoredSlots[slot]) {
            set->m_items[slot].m_guid = 1;
        } else {
            CGItem_C* item = player->m_bag.GetItemPointer(slot);
            set->m_items[slot] = item ? item->m_obj->m_guid : WOWGUID::Make(0);
        }
    }

    if (set->m_guid) {
        CGActionBar::UpdateEquipmentSet(set->m_id);
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SAVE_EQUIPMENT_SET));
    SmartGUID setGuid;
    setGuid.m_guid = set->m_guid.m_guid;
    msg << setGuid;
    msg.Put(static_cast<uint32_t>(set->m_id));
    msg.PutString(set->m_name);
    msg.PutString(set->m_icon);

    for (uint32_t slot = 0; slot < 19; slot++) {
        SmartGUID itemGuid;
        itemGuid.m_guid = set->m_items[slot].m_guid;
        msg << itemGuid;
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_EQUIPMENT_SETS_CHANGED, NULL);
}

// OFFSET: 0x009A1BB0
int32_t CGEquipmentManager::SetUsesLockedSlots(const EQUIPMENT_SET* set) {
    if (!set) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    for (int32_t slot = 0; slot <= 18; slot++) {
        if (set->m_items[slot].m_guid == 1) {
            continue;
        }

        CGItem_C* item = player->m_bag.GetItemPointer(slot);

        if (item && (item->m_itemFlags & 0x1)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x009A1C30/0x1407266F0
int32_t CGEquipmentManager::GetSetList(char* buffer, uint32_t size, const WOWGUID& item) {
    uint32_t remaining = size - 1;
    char* out = buffer;
    const char* separator = "";
    const char* delimiter = FrameScript_GetText("PLAYER_LIST_DELIMITER", -1, GENDER_NOT_APPLICABLE);

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        EQUIPMENT_SET* set = node->m_set;

        for (uint32_t i = 0; i <= 18; i++) {
            if (set->m_items[i] != item) {
                continue;
            }

            size_t nameLength = strlen(set->m_name);
            size_t separatorLength = strlen(separator);

            if (separatorLength + nameLength > remaining) {
                break;
            }

            memcpy(out, separator, separatorLength);
            out += separatorLength;
            memcpy(out, set->m_name, nameLength);
            out += nameLength;
            remaining -= nameLength + separatorLength;
            separator = delimiter;
        }
    }

    *out = '\0';
    return buffer != out;
}

// OFFSET: 0x009A1D50
bool CGEquipmentManager::HasSet(const char* name) {
    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (!strcmp(node->m_set->m_name, name)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009A1DC0
bool CGEquipmentManager::HasSet(int32_t id) {
    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (node->m_set->m_id == id) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009A1E00
int32_t CGEquipmentManager::GetNextFreeSetID() {
    int32_t id = 0;
    EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head();

    while (node) {
        if (node->m_set->m_id == id) {
            if (++id >= 10) {
                return -1;
            }

            node = CGEquipmentManager::s_equipmentSets.Head();
        } else {
            node = CGEquipmentManager::s_equipmentSets.Next(node);
        }
    }

    return id;
}

// OFFSET: 0x009A1E50/0x1407268F0
EQUIPMENT_SET* CGEquipmentManager::GetSetByIndex(int32_t index) {
    int32_t i = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (i == index) {
            return node->m_set;
        }

        i++;
    }

    return NULL;
}

// OFFSET: 0x009A1E90/0x140726930
EQUIPMENT_SET* CGEquipmentManager::GetSetByID(int32_t id) {
    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (node->m_set->m_id == id) {
            return node->m_set;
        }
    }

    return NULL;
}

// OFFSET: 0x009A1ED0/0x140726970
EQUIPMENT_SET* CGEquipmentManager::GetSetByName(const char* name) {
    if (!name) {
        return NULL;
    }

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (!strcmp(node->m_set->m_name, name)) {
            return node->m_set;
        }
    }

    return NULL;
}

// OFFSET: 0x009A1F50
void CGEquipmentManager::EquipSet(int32_t id) {
    EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(id);

    if (set) {
        FrameScript_SignalEvent(SCRIPT_EVENT_WEAR_EQUIPMENT_SET, "%s", static_cast<const char*>(set->m_name));
    }
}

// OFFSET: 0x009A1F80/0x140726A50
void CGEquipmentManager::GetFullIconPath(int32_t id, char* buffer, uint32_t size) {
    EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(id);

    if (set) {
        const char* path = ClientDBStringLookup(3);

        if (*set->m_icon) {
            SStrPrintf(buffer, size, "%s%s%s", path, *path ? "\\" : "", static_cast<const char*>(set->m_icon));
            return;
        }
    }

    SStrPrintf(buffer, size, "");
}

// OFFSET: 0x009A2010/0x140726B20
int32_t CGEquipmentManager::GetNumSets() {
    int32_t count = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        count++;
    }

    return count;
}

// OFFSET: 0x009A2A80/0x1407277A0
void CGEquipmentManager::GetSetStatus(int32_t id, int32_t& total, int32_t& equipped, int32_t& inInventory, int32_t& missing, int32_t (&missingSlots)[19], int32_t& ignored) {
    ignored = 0;
    missing = 0;
    inInventory = 0;
    equipped = 0;
    total = 0;
    EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(id);

    if (!set) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    uint32_t flags = CGBankInfo::m_banker ? 14 : 6;

    for (int32_t slot = 0; slot <= 18; slot++) {
        WOWGUID guid = set->m_items[slot];

        if (!guid) {
            continue;
        }

        if (guid.m_guid == 1) {
            ignored++;
        } else if (static_cast<uint32_t>(player->m_bag.FindItemSlot(guid)) <= 18) {
            equipped++;
        } else if (player->m_bag.ContainsItem(guid, true, flags)) {
            inInventory++;
        } else {
            missingSlots[missing] = slot;
            missing++;
        }
    }

    total = missing + inInventory + equipped;
}

// OFFSET: 0x009A2BB0/0x140727940
bool CGEquipmentManager::IsSetEquipped(int32_t id) {
    EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(id);

    if (!set) {
        return false;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    for (int32_t slot = 0; slot <= 18; slot++) {
        if (set->m_items[slot].m_guid == 1) {
            continue;
        }

        WOWGUID guid = static_cast<uint32_t>(slot) < static_cast<uint32_t>(player->m_bag.m_numSlots) ? player->m_bag.m_slots[slot] : WOWGUID::Make(0);

        if (guid != set->m_items[slot]) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x009A2C60
bool CGEquipmentManager::ModifyEquipmentSet(EQUIPMENT_SET* set, const char* name, const char* icon) {
    if (!set) {
        return false;
    }

    if (name) {
        if (strcmp(set->m_name, name) && CGEquipmentManager::HasSet(name)) {
            return false;
        }

        SStrCopy(set->m_name, name, sizeof(set->m_name));
    }

    if (icon) {
        SStrCopy(set->m_icon, icon, sizeof(set->m_icon));
    }

    if (set->m_guid) {
        CGActionBar::UpdateEquipmentSet(set->m_id);
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SAVE_EQUIPMENT_SET));
    SmartGUID setGuid;
    setGuid.m_guid = set->m_guid.m_guid;
    msg << setGuid;
    msg.Put(static_cast<uint32_t>(set->m_id));
    msg.PutString(set->m_name);
    msg.PutString(set->m_icon);

    for (uint32_t slot = 0; slot < 19; slot++) {
        SmartGUID itemGuid;
        itemGuid.m_guid = set->m_items[slot].m_guid;
        msg << itemGuid;
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_EQUIPMENT_SETS_CHANGED, NULL);
    return true;
}

// OFFSET: 0x009A2E50
bool CGEquipmentManager::CanCreateSet() {
    int32_t count = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        count++;
    }

    return count < 10;
}

// OFFSET: 0x009A2E80
int32_t CGEquipmentManager::OnEquipmentSetResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGEquipmentManager::m_swapPending = 0;
    uint8_t result = 0;
    msg->Get(result);
    int32_t code = result;

    if (!code) {
        EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGEquipmentManager::m_pendingSetID);
        FrameScript_SignalEvent(SCRIPT_EVENT_EQUIPMENT_SWAP_FINISHED, "%b%s", 1, set ? static_cast<const char*>(set->m_name) : NULL);
    } else {
        EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGEquipmentManager::m_pendingSetID);
        FrameScript_SignalEvent(SCRIPT_EVENT_EQUIPMENT_SWAP_FINISHED, "%b%s", 0, set ? static_cast<const char*>(set->m_name) : NULL);

        if (code == 4) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", FrameScript_GetText("EQUIPMENT_MANAGER_BAGS_FULL", -1, GENDER_NOT_APPLICABLE));
        }
    }

    CGActionBar::UpdateUsable();
    CGEquipmentManager::m_pendingSetID = 0;
    return 1;
}

// PROVEN: 62cfdeb7 IDENTICAL/NONE
// OFFSET: 0x009A2F50
void CGEquipmentManager::SaveSetByName(const char* name, const char* icon) {
    EQUIPMENT_SET* set = CGEquipmentManager::GetSetByName(name);

    if (set) {
        CGEquipmentManager::SaveSet(set, name, icon);
    }
}

// OFFSET: 0x009A32B0
void CGEquipmentManager::CreateEquipmentSet(const char* name, const char* icon) {
    int32_t count = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        count++;
    }

    if (count >= 10 || CGEquipmentManager::HasSet(name)) {
        return;
    }

    int32_t id = CGEquipmentManager::GetNextFreeSetID();

    if (id < 0) {
        return;
    }

    EQUIPMENT_SET* set = CGEquipmentManager::s_equipmentSetPool->New("", 0);
    set->m_id = id;
    EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.NewNode(2, 0, 8);
    node->m_set = set;
    CGEquipmentManager::SaveSet(set, name, icon);
    set->m_unsaved = 1;
}

// OFFSET: 0x009A36C0/0x140728B00
bool CGEquipmentManager::DeleteEquipmentSet(EQUIPMENT_SET* set) {
    CGActionBar::RemoveEquipmentSet(set->m_id);
    WOWGUID guid = set->m_guid;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (node->m_set == set) {
            CGEquipmentManager::s_equipmentSets.DeleteNode(node);
            break;
        }
    }

    CGEquipmentManager::s_equipmentSetPool->Delete(set);
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_EQUIPMENT_SET_DELETE));
    SmartGUID setGuid;
    setGuid.m_guid = guid.m_guid;
    msg << setGuid;
    msg.Finalize();
    ClientServices::Send2(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_EQUIPMENT_SETS_CHANGED, NULL);
    return true;
}

// OFFSET: 0x009A3800
int32_t CGEquipmentManager::OnEquipmentSet(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count = 0;
    msg->Get(count);
    uint32_t existing = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        existing++;
    }

    if (CGEquipmentManager::m_loaded && count == existing) {
        msg->m_read = msg->m_size;
        return 1;
    }

    if (existing) {
        for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
            CGEquipmentManager::s_equipmentSetPool->Delete(node->m_set);
        }

        CGEquipmentManager::s_equipmentSets.Clear();
    }

    for (int32_t i = 0; i < static_cast<int32_t>(count) && i < 10; i++) {
        EQUIPMENT_SET* set = CGEquipmentManager::s_equipmentSetPool->New("", 0);
        SmartGUID guid;
        *msg >> guid;
        set->m_guid = guid;
        msg->Get(reinterpret_cast<uint32_t&>(set->m_id));
        msg->GetString(set->m_name, sizeof(set->m_name));
        msg->GetString(set->m_icon, sizeof(set->m_icon));

        for (uint32_t slot = 0; slot < 19; slot++) {
            SmartGUID item;
            *msg >> item;
            set->m_items[slot] = item;
        }

        EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.NewNode(2, 0, 8);
        node->m_set = set;
    }

    CGEquipmentManager::m_loaded = 1;
    return 1;
}

// OFFSET: 0x009A3A10
static void OnEquipmentSetSaved(int32_t id, WOWGUID guid) {
    EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(id);

    if (!set) {
        return;
    }

    if (guid.m_guid) {
        set->m_guid = guid;
        set->m_unsaved = 0;
        return;
    }

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (node->m_set->m_id == id) {
            CGEquipmentManager::s_equipmentSets.DeleteNode(node);
            break;
        }
    }

    CGEquipmentManager::s_equipmentSetPool->Delete(set);
}

// OFFSET: 0x009A3B10
void CGEquipmentManager::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_LOAD_EQUIPMENT_SET);
    ClientServices::ClearMessageHandler(SMSG_EQUIPMENT_SET_ID);
    ClientServices::ClearMessageHandler(SMSG_USE_EQUIPMENT_SET_RESULT);
    CGEquipmentManager::m_swapPending = 0;
    CGEquipmentManager::m_pendingSetID = 0;
    CGEquipmentManager::m_loaded = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        CGEquipmentManager::s_equipmentSetPool->Delete(node->m_set);
    }

    CGEquipmentManager::s_equipmentSets.Clear();
    delete CGEquipmentManager::s_equipmentSetPool;
    CGEquipmentManager::s_equipmentSetPool = NULL;
}

// OFFSET: 0x009A3C20
int32_t CGEquipmentManager::OnEquipmentSetID(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t id = 0;
    msg->Get(reinterpret_cast<uint32_t&>(id));
    SmartGUID guid;
    *msg >> guid;
    OnEquipmentSetSaved(id, guid);
    return 1;
}

// OFFSET: 0x009A3C70/0x1407292A0
void CGEquipmentManager::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_LOAD_EQUIPMENT_SET, CGEquipmentManager::OnEquipmentSet, NULL);
    ClientServices::SetMessageHandler(SMSG_EQUIPMENT_SET_ID, CGEquipmentManager::OnEquipmentSetID, NULL);
    ClientServices::SetMessageHandler(SMSG_USE_EQUIPMENT_SET_RESULT, CGEquipmentManager::OnEquipmentSetResult, NULL);

    if (!CGEquipmentManager::s_equipmentSetPool) {
        CGEquipmentManager::s_equipmentSetPool = new (SMemAlloc(sizeof(TSObjectAllocPool<EQUIPMENT_SET>), __FILE__, __LINE__, 0)) TSObjectAllocPool<EQUIPMENT_SET>("EQUIPMENTSET", 128);
    }
}
