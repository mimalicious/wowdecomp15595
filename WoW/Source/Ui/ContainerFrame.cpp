#include "ContainerFrame.h"
#include <cstddef>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/Object/Container.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/CGQuestObjectiveCache.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00F4DDE0/0x140E42790
WOWGUID CGContainerInfo::m_containers[11];

// STATIC: 0x00F4DE38/0x140E427E8
char* CGContainerInfo::m_searchText;

// STATIC: 0x00F4DE3C
char* CGContainerInfo::m_searchTextParsed;

// PROVEN: 63fe3347 IDENTICAL/IDENTICAL
// OFFSET: 0x009BB1E0/0x140746A00
void CGContainerInfo::UpdateCooldowns() {
    FrameScript_SignalEvent(SCRIPT_EVENT_BAG_UPDATE_COOLDOWN, NULL);
}

// PROVEN: b0e3f888 IDENTICAL/IDENTICAL
// OFFSET: 0x009BB1F0/0x140746A10
WOWGUID CGContainerInfo::GetContainer(int32_t index) {
    if (index >= 0 && static_cast<uint32_t>(index) < 11 && (index < 4 || CGBankInfo::m_banker)) {
        return CGContainerInfo::m_containers[index];
    }

    return WOWGUID::Make(0);
}

// PROVEN: 08475d68 IDENTICAL/DIFFERENT
// OFFSET: 0x009BB230/0x140746A40
void CGContainerInfo::SetItemSearch(const char* search) {
    uint32_t length = SStrLenUTF8(search);

    if (CGContainerInfo::m_searchText && !SStrCmpUTF8I(CGContainerInfo::m_searchText, search, STORM_MAX_STR)) {
        return;
    }

    if (!CGContainerInfo::m_searchText && !length) {
        return;
    }

    if (CGContainerInfo::m_searchText) {
        SMemFree(CGContainerInfo::m_searchText, __FILE__, __LINE__, 0);
    }

    if (CGContainerInfo::m_searchTextParsed) {
        SMemFree(CGContainerInfo::m_searchTextParsed, __FILE__, __LINE__, 0);
    }

    CGContainerInfo::m_searchText = NULL;
    CGContainerInfo::m_searchTextParsed = NULL;

    if (SStrLenUTF8(search) > 0) {
        CGContainerInfo::m_searchText = SStrDupA(search, __FILE__, __LINE__);
        CGContainerInfo::m_searchTextParsed = SStrDupA(search, __FILE__, __LINE__);
        CaseAccentInsensitiveParse(CGContainerInfo::m_searchTextParsed, length, s_locale);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_INVENTORY_SEARCH_UPDATE, NULL);
}

// PROVEN: bc95a493 IDENTICAL/DIFFERENT
// OFFSET: 0x009BB4B0/0x140746CF0
void CGContainerInfo::UpdateContents(WOWGUID guid) {
    CGActionBar::UpdateUsable();
    CGQuestObjectiveCache::s_dirty = 1;
    CGCharacterInfo::AverageItemLevelDirty();

    if (guid == ClntObjMgrGetActivePlayer()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_BAG_UPDATE, "%d", 0);
        return;
    }

    for (uint32_t i = 0; i < 11; i++) {
        if (CGContainerInfo::m_containers[i] == guid) {
            FrameScript_SignalEvent(SCRIPT_EVENT_BAG_UPDATE, "%d", i + 1);
            return;
        }
    }
}

// OFFSET: 0x009BB520/0x140746D70
void CGContainerInfo::OpenContainer(WOWGUID guid) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_BAG_OPEN, "%d", 0);
        return;
    }

    for (uint32_t i = 0; i < 11; i++) {
        if (CGContainerInfo::m_containers[i] == guid) {
            FrameScript_SignalEvent(SCRIPT_EVENT_BAG_OPEN, "%d", i + 1);
        }
    }
}

// OFFSET: 0x009BB6B0/0x140746F40
void CGContainerInfo::UpdateItem(WOWGUID guid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGContainerInfo::UpdateContents(item->m_item->m_itemFieldContained);
    }
}

// OFFSET: 0x009BB710/0x140746FA0
bool CGContainerInfo::GetLuaBagAndSlot(lua_State* L, CGBag_C*& bag, int32_t& slot, int32_t& bank) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    bag = NULL;
    bank = 0;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (index >= 0) {
        if (index < 11) {
            if (CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(index), TYPEMASK_CONTAINER, __FILE__, __LINE__))) {
                bag = container->GetBag();
            }

            if (index >= 4) {
                bank = 1;
            }
        }
    } else {
        bag = &player->m_bag;

        if (index == -2) {
            slot += 39;

            if (slot > 66) {
                return false;
            }

            bank = 1;
        } else if (index == -1) {
            slot += 23;

            if (slot > 38) {
                return false;
            }
        }
    }

    return bag && slot >= 0 && slot < bag->m_numSlots;
}

// OFFSET: 0x009BCD90/0x1407488B0
static void TriggerMultiCastBarUpdate(const ItemRecSparse* rec) {
    if (!rec) {
        return;
    }

    uint32_t mask = CGActionBar::GetMultiCastSlotMaskForTotemCategory(rec->m_totemCategoryID);

    if (!mask) {
        return;
    }

    if (CGTutorial::m_tutorialShown.m_count && !(CGTutorial::m_tutorialShown.m_bits.m_data[1] & 0x4000)) {
        for (uint32_t i = 0; i < 4; i++) {
            if (((1 << i) & mask) && CGSpellBook::m_totemSpells[i].Count()) {
                CGTutorial::TriggerTutorial(46);
                break;
            }
        }
    }

    CGActionBar::UpdateMultiCastBar();
}

// PROVEN: 6b3b2c19 IDENTICAL/IDENTICAL
// OFFSET: 0x009BCE10/0x140748930
static void MultiCastBarItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        WOWGUID requester = WOWGUID::Make(0);

        if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(id, requester, NULL, NULL, false)) {
            TriggerMultiCastBarUpdate(rec);
        }
    }
}

// OFFSET: 0x009BCE50/0x140748980
int32_t UpdateContainerContents(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CONTAINER, __FILE__, __LINE__));

    if (!container) {
        return 1;
    }

    uint32_t slot = (fieldOffset - offsetof(CGContainerData, m_containerFieldSlot1)) / sizeof(WOWGUID);
    CGBag_C* bag = container->GetBag();
    WOWGUID newGuid = slot < static_cast<uint32_t>(bag->m_numSlots) ? bag->m_slots[slot] : WOWGUID();
    const WOWGUID& oldGuid = *static_cast<const WOWGUID*>(oldValue);
    CGItem_C* oldItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(oldGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
    CGItem_C* newItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(newGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (oldGuid != newGuid) {
        if (oldItem) {
            CGActionBar::UpdateItem(oldItem->m_obj->m_entryID);
        }

        if (newItem) {
            CGActionBar::UpdateItem(newItem->m_obj->m_entryID);
        }

        CGTradeSkillInfo::RefreshList(0);
    }

    CGGameUI::UnlockItem(newGuid);

    if (CGGameUI::m_autoStoreItem == oldGuid || CGGameUI::m_autoStoreItem == newGuid || (!CGGameUI::m_autoStoreItem && CGGameUI::m_autoStoreItemID)) {
        WOWGUID requester = WOWGUID::Make(0);

        if (ItemRec* item = g_itemDB.GetRecord(CGGameUI::m_autoStoreItemID, requester, NULL, NULL, false)) {
            SI3::PlayItemSound(1, item->m_displayInfoID);
        }
    }

    CGGameUI::m_autoStoreItem.m_guid = 0;
    CGGameUI::m_autoStoreItemID = 0;
    CGContainerInfo::UpdateContents(guid);

    if (newItem) {
        WOWGUID requester = WOWGUID::Make(0);

        if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(newItem->m_obj->m_entryID, requester, MultiCastBarItemStatsCallback, NULL, false)) {
            TriggerMultiCastBarUpdate(rec);
        }

        if (CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(newItem->m_item->m_itemFieldOwner, TYPEMASK_UNIT, __FILE__, __LINE__))) {
            if (owner->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
                int32_t itemId = newItem->m_obj->m_entryID;
                ClntObjMgrGetActivePlayerObj()->CheckForQuestItemLoot(itemId);
            }
        }
    }

    return 1;
}

// OFFSET: 0x009BD030/0x140748B90
int32_t UpdateInvContents(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t slot = (fieldOffset - offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead)) / sizeof(WOWGUID);
    WOWGUID newGuid = slot < static_cast<uint32_t>(player->m_bag.m_numSlots) ? player->m_bag.m_slots[slot] : WOWGUID();
    const WOWGUID& oldGuid = *static_cast<const WOWGUID*>(oldValue);
    CGItem_C* oldItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(oldGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
    CGItem_C* newItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(newGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (oldGuid != newGuid) {
        if (oldItem) {
            CGActionBar::UpdateItem(oldItem->m_obj->m_entryID);
        }

        if (newItem) {
            CGActionBar::UpdateItem(newItem->m_obj->m_entryID);
        }

        CGTradeSkillInfo::RefreshList(0);
    }

    CGGameUI::UnlockItem(newGuid);

    if (CGGameUI::m_autoStoreItem == oldGuid || CGGameUI::m_autoStoreItem == newGuid || (!CGGameUI::m_autoStoreItem && CGGameUI::m_autoStoreItemID)) {
        WOWGUID requester = WOWGUID::Make(0);

        if (ItemRec* item = g_itemDB.GetRecord(CGGameUI::m_autoStoreItemID, requester, NULL, NULL, false)) {
            SI3::PlayItemSound(1, item->m_displayInfoID);
        }
    }

    CGGameUI::m_autoStoreItem.m_guid = 0;
    CGGameUI::m_autoStoreItemID = 0;
    CGContainerInfo::UpdateContents(guid);

    if (newItem) {
        WOWGUID requester = WOWGUID::Make(0);

        if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(newItem->m_obj->m_entryID, requester, MultiCastBarItemStatsCallback, NULL, false)) {
            TriggerMultiCastBarUpdate(rec);
        }

        player->CheckForQuestItemLoot(newItem->m_obj->m_entryID);
    }

    return 1;
}

// OFFSET: 0x009BD1E0/0x140748D50
void CGContainerInfo::UpdateContainers() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < 11; i++) {
        uint32_t slot = i < 4 ? i + 19 : i + 63;
        WOWGUID guid = slot < static_cast<uint32_t>(player->m_bag.m_numSlots) ? player->m_bag.m_slots[slot] : WOWGUID();

        if (guid == CGContainerInfo::m_containers[i]) {
            continue;
        }

        if (CGContainerInfo::m_containers[i]) {
            CGContainer_C* old = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(CGContainerInfo::m_containers[i], TYPEMASK_CONTAINER, __FILE__, __LINE__));

            if (old && old->GetBag()) {
                int32_t count = old->GetBag()->m_numSlots;

                for (int32_t j = 0; j < count; j++) {
                    ClntObjMgrUnsetObjMirrorHandler(CGContainerInfo::m_containers[i], ID_CONTAINER, offsetof(CGContainerData, m_containerFieldSlot1) + j * sizeof(WOWGUID), UpdateContainerContents, NULL);
                }
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_BAG_CLOSED, "%d", i + 1);
        }

        CGContainerInfo::m_containers[i] = guid;

        if (guid) {
            CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CONTAINER, __FILE__, __LINE__));

            if (container && container->GetBag()) {
                int32_t count = container->GetBag()->m_numSlots;

                for (int32_t j = 0; j < count; j++) {
                    ClntObjMgrSetObjMirrorHandler(guid, ID_CONTAINER, offsetof(CGContainerData, m_containerFieldSlot1) + j * sizeof(WOWGUID), sizeof(WOWGUID), UpdateContainerContents, NULL, static_cast<HANDLER_PRIORITY>(0), 1);
                }
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_BAG_UPDATE, "%d", i + 1);
        }
    }
}

// OFFSET: 0x009BD3C0/0x140748FB0
bool ItemRecIsFiltered(const ItemRec* item, int32_t randomPropertiesId) {
    if (!CGContainerInfo::m_searchText) {
        return false;
    }

    char text[1024];
    CGItem_C::BuildItemName(text, sizeof(text), item->m_ID, randomPropertiesId);
    CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

    if (SStrStrUTF8I(text, CGContainerInfo::m_searchTextParsed)) {
        return false;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_ID, requester, NULL, NULL, false);

    if (!rec) {
        return false;
    }

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* subClass = g_itemSubClassDB.GetRecordByIndex(i);

        if (!subClass || subClass->m_classID != item->m_classID || subClass->m_subClassID != item->m_subclassID) {
            continue;
        }

        if (rec->m_inventoryType != 16 && !(subClass->m_flags & 0x1)) {
            const char* name = subClass->m_displayName;

            if (name && *name) {
                SStrCopy(text, name, sizeof(text));
            }
        }

        break;
    }

    CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

    if (SStrStrUTF8I(text, CGContainerInfo::m_searchTextParsed)) {
        return false;
    }

    const char* type = FrameScript_GetText(s_inventoryTypeTokens[rec->m_inventoryType], -1, GENDER_NOT_APPLICABLE);
    SStrCopy(text, type, sizeof(text));

    if (!*type) {
        text[0] = 0;
    }

    CaseAccentInsensitiveParse(text, sizeof(text), s_locale);
    return SStrStrUTF8I(text, CGContainerInfo::m_searchTextParsed) == NULL;
}

// OFFSET: 0x009BE4B0/0x14074A490
int32_t InvUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t slot = (fieldOffset - offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead)) / sizeof(WOWGUID);

    if (slot < static_cast<uint32_t>(player->m_bag.m_numSlots) && player->m_bag.m_slots[slot]) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(player->m_bag.m_slots[slot], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && (item->m_obj->m_type & TYPEMASK_CONTAINER)) {
            if (CGBag_C* bag = item->GetBag()) {
                for (uint32_t i = 0; i < static_cast<uint32_t>(bag->m_numSlots); i++) {
                    if (CGItem_C* contained = bag->GetItemPointer(i)) {
                        CGActionBar::UpdateItem(contained->m_obj->m_entryID);
                    }
                }
            }
        }
    }

    CGContainerInfo::UpdateContents(guid);
    CGContainerInfo::UpdateContainers();
    CGTradeSkillInfo::RefreshList(0);
    return 1;
}

// OFFSET: 0x009BE590/0x14074A580
void CGContainerInfo::EnterWorld() {
    for (int32_t offset = offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 19 * sizeof(WOWGUID); offset < static_cast<int32_t>(offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 23 * sizeof(WOWGUID)); offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), InvUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (int32_t offset = offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 23 * sizeof(WOWGUID); offset < static_cast<int32_t>(offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 39 * sizeof(WOWGUID)); offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), UpdateInvContents, NULL, static_cast<HANDLER_PRIORITY>(0), 1);
    }

    for (int32_t offset = offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 67 * sizeof(WOWGUID); offset < static_cast<int32_t>(offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 74 * sizeof(WOWGUID)); offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), InvUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    memset(CGContainerInfo::m_containers, 0, sizeof(CGContainerInfo::m_containers));
    CGContainerInfo::UpdateContainers();
}

// OFFSET: 0x009BE640/0x14074A6B0
void CGContainerInfo::LeaveWorld() {
    for (uint32_t i = 0; i < 11; i++) {
        if (!CGContainerInfo::m_containers[i]) {
            continue;
        }

        CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(CGContainerInfo::m_containers[i], TYPEMASK_CONTAINER, __FILE__, __LINE__));

        if (!container) {
            continue;
        }

        int32_t count = container->GetBag()->m_numSlots;

        for (int32_t j = 0; j < count; j++) {
            ClntObjMgrUnsetObjMirrorHandler(CGContainerInfo::m_containers[i], ID_CONTAINER, offsetof(CGContainerData, m_containerFieldSlot1) + j * sizeof(WOWGUID), UpdateContainerContents, NULL);
        }
    }

    for (uint32_t offset = offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 23 * sizeof(WOWGUID); offset < offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 39 * sizeof(WOWGUID); offset += sizeof(WOWGUID)) {
        ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, UpdateInvContents, NULL);
    }

    for (uint32_t offset = offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 19 * sizeof(WOWGUID); offset < offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 23 * sizeof(WOWGUID); offset += sizeof(WOWGUID)) {
        ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, InvUpdateHandler, NULL);
    }

    for (uint32_t offset = offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 67 * sizeof(WOWGUID); offset < offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead) + 74 * sizeof(WOWGUID); offset += sizeof(WOWGUID)) {
        ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, InvUpdateHandler, NULL);
    }

    if (CGContainerInfo::m_searchText) {
        SMemFree(CGContainerInfo::m_searchText, __FILE__, __LINE__, 0);
    }

    if (CGContainerInfo::m_searchTextParsed) {
        SMemFree(CGContainerInfo::m_searchTextParsed, __FILE__, __LINE__, 0);
    }

    CGContainerInfo::m_searchTextParsed = NULL;
    CGContainerInfo::m_searchText = NULL;
}

// PROVEN: 7fce8354 IDENTICAL/IDENTICAL
// OFFSET: 0x009BB320/0x140746B50
bool CGContainerInfo::IsSearchActive() {
    return CGContainerInfo::m_searchText != NULL;
}

// OFFSET: 0x009BE790/0x14074A860
bool CGContainerInfo::IsItemFiltered(const CGItem_C* item) {
    if (!CGContainerInfo::m_searchText) {
        return false;
    }

    for (int32_t i = CGEquipmentManager::GetNumSets(); i >= 0; i--) {
        EQUIPMENT_SET* set = CGEquipmentManager::GetSetByIndex(i);

        if (!set) {
            continue;
        }

        char name[1024];
        SStrCopy(name, set->m_name, sizeof(name));
        CaseAccentInsensitiveParse(name, sizeof(name), s_locale);

        if (!SStrStrUTF8I(name, CGContainerInfo::m_searchTextParsed)) {
            continue;
        }

        for (uint32_t j = 0; j <= 18; j++) {
            if (set->m_items[j].m_guid > 1 && ClntObjMgrObjectPtr(set->m_items[j], TYPEMASK_ITEM, __FILE__, __LINE__) == item) {
                return false;
            }
        }
    }

    WOWGUID requester = WOWGUID();
    ItemRec* rec = g_itemDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false);
    return ItemRecIsFiltered(rec, item->m_item->m_itemFieldRandomPropertiesId);
}
