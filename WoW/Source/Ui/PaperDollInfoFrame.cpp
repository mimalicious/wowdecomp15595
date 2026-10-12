#include "PaperDollInfoFrame.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include <cstdlib>

// STATIC: 0x00C00024/0x1409F7F78
static const int32_t s_inventoryAlertSlots[11] = { 0, 2, 4, 5, 6, 7, 8, 9, 15, 16, 17 };

// STATIC: 0x00F4A358/0x140E3E490
TSFixedArray<INV_ALERT_STATUS> CGCharacterInfo::m_invAlertStatus;

// STATIC: 0x00F4A330/0x140E3E468
WOWGUID CGCharacterInfo::m_inspectTarget;

// STATIC: 0x00F4A338/0x140E3E470
int32_t CGCharacterInfo::m_hasHonorData;

// STATIC: 0x00F4A2D8/0x140E3E410
InspectArenaTeam CGCharacterInfo::m_inspectArenaTeams[3];

// STATIC: 0x00F4A320/0x140E3E458
uint32_t CGCharacterInfo::m_inspectLifetimeHonorableKills;

// STATIC: 0x00F4A324/0x140E3E45C
uint16_t CGCharacterInfo::m_inspectYesterdayHonorableKills;

// STATIC: 0x00F4A326/0x140E3E45E
uint8_t CGCharacterInfo::m_inspectLifetimeHighestRank;

// STATIC: 0x00F4A328/0x140E3E460
uint16_t CGCharacterInfo::m_inspectTodayHonorableKills;

// STATIC: 0x00F4A33C/0x140E3E474
int32_t CGCharacterInfo::m_honorRequestPending;

// STATIC: 0x00F4A340/0x140E3E478
int32_t CGCharacterInfo::m_avgItemLevelPending;

// STATIC: 0x00F4A344/0x140E3E47C
float CGCharacterInfo::m_avgItemLevel;

// STATIC: 0x00F4A348/0x140E3E480
float CGCharacterInfo::m_avgItemLevelEquipped;

// STATIC: 0x00F4A34C/0x140E3E484
uint32_t CGCharacterInfo::m_inspectRatedBGPlayed;

// STATIC: 0x00F4A350/0x140E3E488
uint32_t CGCharacterInfo::m_inspectRatedBGRating;

// STATIC: 0x00F4A354/0x140E3E48C
uint32_t CGCharacterInfo::m_inspectRatedBGWon;

// STATIC: 0x00D13668/0x140BCBA80
uint8_t CGCharacterInfo::m_avgItemLevelDirty = 1;

// INLINED
int32_t PlayerEquipmentChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        CGActionBar::UpdateUsable();
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_INVENTORY_CHANGED, "%s", "player");
    }

    return 1;
}

// PROVEN: a101faac IDENTICAL/DIFFERENT
// OFFSET: 0x0097C3C0/0x1406F5B90
int32_t PlayerCharacterPointsUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t oldPoints = *static_cast<const int32_t*>(oldValue);
        uint64_t playerGuid = player->m_obj->m_guid.m_guid;
        int32_t points = playerGuid != ClntObjMgrGetActivePlayer().m_guid ? 0 : player->m_player->m_playerCharacterPoints;
        FrameScript_SignalEvent(SCRIPT_EVENT_CHARACTER_POINTS_CHANGED, "%d", points - oldPoints);
    }

    return 1;
}

// PROVEN: dda228fe IDENTICAL/IDENTICAL
// OFFSET: 0x0097B860/0x1406F4E30
static bool OnInspectHonorStats(ClientInspectHonorStats* msg) {
    if (*reinterpret_cast<WOWGUID*>(&msg->unk_0010) == CGCharacterInfo::m_inspectTarget) {
        CGCharacterInfo::m_inspectLifetimeHighestRank = msg->unk_0020;
        CGCharacterInfo::m_inspectTodayHonorableKills = msg->unk_001C;
        CGCharacterInfo::m_inspectYesterdayHonorableKills = msg->unk_001E;
        CGCharacterInfo::m_inspectLifetimeHonorableKills = msg->unk_0018;
        CGCharacterInfo::m_hasHonorData = 1;
        CGCharacterInfo::m_honorRequestPending = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, NULL);
    }

    return true;
}

// PROVEN: cb621f54 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B8D0/0x1406F4EA0
static bool OnInspectRatedBgStats(ClientInspectRatedBgStats* msg) {
    if (*reinterpret_cast<WOWGUID*>(&msg->unk_0010) == CGCharacterInfo::m_inspectTarget) {
        CGCharacterInfo::m_inspectRatedBGRating = msg->unk_0020;
        CGCharacterInfo::m_inspectRatedBGPlayed = msg->unk_0018;
        CGCharacterInfo::m_inspectRatedBGWon = msg->unk_001C;
        FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x0097C440/0x1406F5C20
void CGCharacterInfo::InitializeGame() {
    ClientInspectHonorStats::s_handler = JamHandlerEncode(OnInspectHonorStats, g_jamHandlerKey_CFE310);
    ClientInspectRatedBgStats::s_handler = JamHandlerEncode(OnInspectRatedBgStats, g_jamHandlerKey_CFDB84);
    ClientServices::SetMessageHandler(MSG_INSPECT_ARENA_TEAMS, Packet_MSG_INSPECT_ARENA_TEAMS, NULL);
}

// INLINED
void CGCharacterInfo::RegisterMirrorHandlers(WOWGUID guid) {
    if (!guid) {
        return;
    }

    ClntObjMgrSetObjMirrorHandler(guid, ID_PLAYER, 0xD98, 4, PlayerCharacterPointsUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// PROVEN: 32b8e1f9 IDENTICAL/DIFFERENT
// OFFSET: 0x0097CF60/0x1406F69A0
void CGCharacterInfo::UpdateInventoryAlerts() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    bool changed = false;

    for (uint32_t i = 0; i < 11; i++) {
        INV_ALERT_STATUS status = CGCharacterInfo::CalculateInventoryAlertStatus(player, i);

        if (status != CGCharacterInfo::m_invAlertStatus.m_data[i]) {
            CGCharacterInfo::m_invAlertStatus.m_data[i] = status;
            changed = true;
        }
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_INVENTORY_ALERTS, NULL);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_INVENTORY_DURABILITY, NULL);
}

// PROVEN: 855cbcde IDENTICAL/UNPROVEN
// OFFSET: 0x0097DF80/0x1406F7C50
void CGCharacterInfo::EnterWorld() {
    CGCharacterInfo::RegisterMirrorHandlers(ClntObjMgrGetActivePlayer());
    CGCharacterInfo::m_invAlertStatus.SetCount(11);
    CGCharacterInfo::UpdateInventoryAlerts();
    CGCharacterInfo::m_avgItemLevelDirty = 1;
}

// INLINED
void UnitInventoryChangedCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_INVENTORY_CHANGED, "%s", "player");
    }
}

// PROVEN: 5e37b4b7 DIFFERENT/IDENTICAL
// OFFSET: 0x0097B920/0x1406F4EE0
bool CGCharacterInfo::CanInspect(const CGPlayer_C* unit) {
    if (unit) {
        uint64_t guid = unit->m_obj->m_guid.m_guid;

        if (guid == ClntObjMgrGetActivePlayer().m_guid || unit->m_obj->m_guid == CGCharacterInfo::m_inspectTarget) {
            return true;
        }
    }

    return false;
}

// INLINED
int32_t CGCharacterInfo::GetActivePlayerAmmoID(const CGPlayer_C* player) {
    return 0;
}

// PROVEN: d1e78a85 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B210/0x1406F4810
int32_t Packet_MSG_INSPECT_ARENA_TEAMS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (guid != CGCharacterInfo::m_inspectTarget) {
        msg->m_read = msg->m_size;
        return 1;
    }

    uint8_t slot;
    msg->Get(slot);

    if (slot <= 3) {
        msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_teamId);
        msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_rating);
        msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_weekGames);
        msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_weekWins);
        msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_seasonGames);
        msg->Get(CGCharacterInfo::m_inspectArenaTeams[slot].m_personalRating);
        FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, NULL);
    } else {
        msg->m_read = msg->m_size;
    }

    return 1;
}

// PROVEN: 7d01de5a IDENTICAL/IDENTICAL
// OFFSET: 0x0097B310/0x1406F4920
void CGCharacterInfo::SetInspectTarget(const WOWGUID& guid) {
    WOWGUID target = guid;

    if (target == CGCharacterInfo::m_inspectTarget) {
        return;
    }

    CGCharacterInfo::m_inspectTarget = target;
    CGCharacterInfo::m_hasHonorData = 0;
    CGCharacterInfo::m_honorRequestPending = 0;

    for (uint32_t i = 0; i < 3; i++) {
        CGCharacterInfo::m_inspectArenaTeams[i].m_teamId = 0;
    }

    CGTalentInfo_Inspect::SetInspectTarget(guid);
}

// OFFSET: 0x0097C480/0x1406F5C60
void CGCharacterInfo::UpdateItem(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));
    int32_t slot = player->m_bag.FindItemSlot(guid);

    if (slot == -1) {
        if (!item) {
            return;
        }

        uint64_t container = item->m_item->m_itemFieldContained.m_guid;

        if (container == ClntObjMgrGetActivePlayer().m_guid) {
            return;
        }

        slot = player->m_bag.FindItemSlot(item->m_item->m_itemFieldContained);
    }

    if (static_cast<uint32_t>(slot) <= 22) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_INVENTORY_CHANGED, "%s", "player");
    } else if ((slot >= 39 && slot <= 66) || static_cast<uint32_t>(slot - 67) <= 6) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYERBANKSLOTS_CHANGED, "%d", slot - 38);
    }
}

// INLINED
static void BuyItemInBag(WOWGUID merchant, const VendorItem* item, uint32_t count, WOWGUID bag) {
    CGPlayer_C::BuyItemInSlot(merchant, item, count, bag, 0xFF);
}

// OFFSET: 0x0097C570/0x1406F5DC0
void CGCharacterInfo::PickupInventoryItem(int32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (!CGGameUI::m_hasControl && !(player->m_unit->m_unitFieldFlags & 0x100000))) {
        return;
    }

    if (((slot >= 39 && slot <= 66) || static_cast<uint32_t>(slot - 67) <= 6) && !CGBankInfo::m_banker) {
        return;
    }

    WOWGUID cursorItem;
    WOWGUID cursorContainer;
    uint32_t cursorSlot;
    CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);
    int32_t virtualId;
    int32_t virtualSlot;
    CGGameUI::GetCursorVirtualItem(virtualId, virtualSlot);
    WOWGUID itemGuid = WOWGUID::Make(static_cast<uint32_t>(slot) >= static_cast<uint32_t>(player->m_bag.m_numSlots) ? 0 : player->m_bag.m_slots[slot].m_guid);

    if (!cursorItem) {
        if (virtualId) {
            if (CGGameUI::m_cursorItemType == 5 && CGMerchantInfo::m_merchant) {
                VendorItem* merchantItem = CGMerchantInfo::GetItem(virtualSlot);

                if (merchantItem) {
                    CGPlayer_C::BuyItemInSlot(CGMerchantInfo::m_merchant, merchantItem, merchantItem->m_buyCount, player->m_obj->m_guid, slot);
                    CGGameUI::ClearCursor(1, 1);
                }
            }

            return;
        }

        if (!itemGuid) {
            return;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && (item->m_itemFlags & 1)) {
            return;
        }

        if (Spell_C_IsTargeting() && Spell_C_CanTargetItems()) {
            CGGameUI::CastItemSpell(itemGuid, 0);
        } else if (item && CursorGetResetMode() == 17 && CGMerchantInfo::CanRepair()) {
            uint64_t cost = static_cast<uint32_t>(CGMerchantInfo::GetRepairCost(item));

            if (cost > player->GetCoinage()) {
                CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            } else {
                SE3SoundKitProperties properties;
                SE3::PlaySoundKit("ITEM_REPAIR", NULL, &properties, NULL, __FILE__, __LINE__, "CGCharacterInfo::PickupItem");
                WDataStore msg;
                msg.Put(static_cast<uint32_t>(CMSG_REPAIR_ITEM));
                msg.Put(CGMerchantInfo::m_merchant.m_guid);
                msg.Put(itemGuid.m_guid);
                msg.Put(static_cast<uint8_t>(0));
                msg.Finalize();
                ClientServices::Send2(&msg);
            }
        } else {
            CGGameUI::SetCursorItem(itemGuid, player->m_obj->m_guid, slot, 1, 0, 1);
            CGGameUI::LockItem(itemGuid);
        }
        return;
    }

    if (itemGuid != cursorItem) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && (item->m_itemFlags & 1)) {
            return;
        }

        if (player->ValidateSlot(slot, cursorItem)) {
            player->SwapItems(cursorItem, cursorContainer, cursorSlot, player->m_obj->m_guid, slot, 0);
            CGGameUI::LockItem(itemGuid);
            return;
        }

        if (cursorContainer != player->m_obj->m_guid || cursorSlot >= 19) {
            player->AutoEquipCursorItem(0);
            return;
        }
    }

    CGGameUI::ClearCursor(1, 1);
}

// OFFSET: 0x0097C8C0/0x1406F6210
void CGCharacterInfo::UseItem(int32_t slot, WOWGUID* target) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);
    CGItem_C* item = player->m_bag.GetItemPointer(slot);

    if (!item) {
        return;
    }

    if (Spell_C_IsTargeting() && !Spell_C_CanTargetItems()) {
        Spell_C_StopTargeting();
    }

    if (Spell_C_IsTargeting()) {
        CGGameUI::CastItemSpell(item->m_obj->m_guid, 0);
    } else if (CursorGetResetMode() == 17 && CGMerchantInfo::CanRepair()) {
        uint64_t cost = static_cast<uint32_t>(CGMerchantInfo::GetRepairCost(item));

        if (cost > player->GetCoinage()) {
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        } else {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit("ITEM_REPAIR", NULL, &properties, NULL, __FILE__, __LINE__, "CGCharacterInfo::UseItem");
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_REPAIR_ITEM));
            msg.Put(CGMerchantInfo::m_merchant.m_guid);
            msg.Put(item->m_obj->m_guid.m_guid);
            msg.Put(static_cast<uint8_t>(0));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    } else {
        item->UseItem(target, 0);
    }
}

// PROVEN: 8877c011 IDENTICAL/DIFFERENT
// OFFSET: 0x0097CA50/0x1406F6460
void CGCharacterInfo::SocketInventoryItem(int32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGItem_C* item = player->m_bag.GetItemPointer(slot);

    if (item && !(item->m_itemFlags & 0x1) && item->GetNumSockets()) {
        CGItemSocketInfo::SetItem(item->m_obj->m_guid);
    }
}

// OFFSET: 0x0097CAC0/0x1406F64D0
void CGCharacterInfo::PickupBag(uint32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !CGGameUI::m_hasControl || (slot - 67 <= 6 && !CGBankInfo::m_banker)) {
        return;
    }

    WOWGUID cursorItem;
    WOWGUID cursorContainer;
    uint32_t cursorSlot;
    CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);

    if (cursorItem) {
        CGGameUI::ClearCursor(1, 1);
    }

    WOWGUID bag;

    if (slot >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !(bag = player->m_bag.m_slots[slot])) {
        CGCharacterInfo::PickupInventoryItem(slot);
        return;
    }

    CGItem_C* container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(bag, TYPEMASK_CONTAINER, __FILE__, __LINE__));

    if (container && !(container->m_itemFlags & 0x1)) {
        CGGameUI::SetCursorItem(bag, player->m_obj->m_guid, slot, 1, 0, 1);
        CGGameUI::LockItem(bag);
    }
}

// OFFSET: 0x0097B970/0x1406F4F30
void CGCharacterInfo::RequestInspectHonorData() {
    if (!CGCharacterInfo::m_hasHonorData && !CGCharacterInfo::m_honorRequestPending && CGCharacterInfo::m_inspectTarget) {
        UserClientOpcode791E honor;
        *reinterpret_cast<uint64_t*>(honor.unk_0010) = CGCharacterInfo::m_inspectTarget.m_guid;
        ClientServices::Send(&honor);
        CDataStore arena;
        arena.Put(static_cast<uint32_t>(MSG_INSPECT_ARENA_TEAMS));
        arena.Put(CGCharacterInfo::m_inspectTarget.m_guid);
        arena.Finalize();
        ClientServices::Send2(&arena);
        CGCharacterInfo::m_honorRequestPending = 1;
        UserClientOpcode3010 rated;
        *reinterpret_cast<uint64_t*>(rated.unk_0010) = CGCharacterInfo::m_inspectTarget.m_guid;
        ClientServices::Send(&rated);
    }
}

// OFFSET: 0x0097CFD0/0x1406F6A40
int32_t CGCharacterInfo::CanEquipItemInSlot(const ItemRecSparse* stats, uint32_t slot) {
    if (!stats) {
        return 0;
    }

    ItemRec* rec = stats->GetNonSparse();
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!rec) {
        return 0;
    }

    if (stats->m_requiredLevel > 1 && stats->m_requiredLevel > static_cast<int32_t>(player->m_unit->m_unitFieldLevel)) {
        return 0;
    }

    ChrClassesRec* playerClass;

    if (!stats->m_scalingStatDistribution || rec->m_classID != 4 || (rec->m_subclassID != 4 && rec->m_subclassID != 3) || !(playerClass = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1])) || ((rec->m_subclassID != 4 || !(playerClass->m_flags & 0x20)) && (rec->m_subclassID != 3 || !(playerClass->m_flags & 0x10)))) {
        uint32_t proficiency = CGPlayer_C::GetProficiency(static_cast<uint8_t>(rec->m_classID));

        if (proficiency && !((1 << rec->m_subclassID) & proficiency)) {
            return 0;
        }
    }

    if (slot == 16 && (stats->m_inventoryType == 22 || stats->m_inventoryType == 13) && !CGPlayer_C::CanDualWield()) {
        return 0;
    }

    int32_t allowed = 1;

    if (!((stats->m_allowableRace - 1) & stats->m_allowableRace) && !((stats->m_allowableClass - 1) & stats->m_allowableClass)) {
        for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
            ChrRacesRec* race = g_chrRacesDB.GetRecordByIndex(i);

            if (race && !(race->m_flags & 0x1) && ((1 << (race->m_id - 1)) & stats->m_allowableRace) && player->m_unit->m_unitFieldBytes0[0] != race->m_id) {
                return 0;
            }
        }

        for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
            ChrClassesRec* chrClass = g_chrClassesDB.GetRecordByIndex(i);

            if (chrClass && ((1 << (chrClass->m_id - 1)) & stats->m_allowableClass) && player->m_unit->m_unitFieldBytes0[1] != chrClass->m_id) {
                return 0;
            }
        }

        return !stats->m_requiredReputationFaction || GetRepListRepValue(stats->m_requiredReputationFaction) >= CGReputationInfo::m_reactionThresholds[stats->m_requiredReputationRank];
    }

    int32_t allRaces = 1;
    int32_t allClasses = 1;

    for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
        ChrRacesRec* race = g_chrRacesDB.GetRecordByIndex(i);

        if (race && !(race->m_flags & 0x1) && !((1 << (race->m_id - 1)) & stats->m_allowableRace)) {
            allRaces = 0;
            break;
        }
    }

    for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
        ChrClassesRec* chrClass = g_chrClassesDB.GetRecordByIndex(i);

        if (chrClass && !((1 << (chrClass->m_id - 1)) & stats->m_allowableClass)) {
            allClasses = 0;
            break;
        }
    }

    if (!allRaces) {
        allowed = 0;

        for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
            ChrRacesRec* race = g_chrRacesDB.GetRecordByIndex(i);

            if (race && !(race->m_flags & 0x1) && ((1 << (race->m_id - 1)) & stats->m_allowableRace) && player->m_unit->m_unitFieldBytes0[0] == race->m_id) {
                allowed = 1;
            }
        }

        if (!allowed) {
            return 0;
        }
    }

    if (!allClasses) {
        allowed = 0;

        for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
            ChrClassesRec* chrClass = g_chrClassesDB.GetRecordByIndex(i);

            if (chrClass && ((1 << (chrClass->m_id - 1)) & stats->m_allowableClass) && player->m_unit->m_unitFieldBytes0[1] == chrClass->m_id) {
                allowed = 1;
            }
        }
    }

    if (!allowed) {
        return 0;
    }

    return !stats->m_requiredReputationFaction || GetRepListRepValue(stats->m_requiredReputationFaction) >= CGReputationInfo::m_reactionThresholds[stats->m_requiredReputationRank];
}

// PROVEN: 54f87507 IDENTICAL/DIFFERENT
// OFFSET: 0x0097E0A0/0x1406F7DA0
int32_t CGCharacterInfo::CanEquipItem(CGItem_C* item, uint32_t slot) {
    int32_t id = item->m_obj->m_entryID;
    WOWGUID none;
    none.m_guid = 0;
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    return CGCharacterInfo::CanEquipItemInSlot(stats, slot);
}

// PROVEN: 7418f846 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B3D0/0x1406F49D0
void CGCharacterInfo::AverageItemLevelDirty() {
    if (!CGCharacterInfo::m_avgItemLevelDirty) {
        CGCharacterInfo::m_avgItemLevelDirty = 1;
    }
}

// PROVEN: 3860e311 IDENTICAL/NONE
// OFFSET: 0x0097B360
static void UpdateBestItemLevel(int32_t level, int32_t* best, int32_t* second) {
    if (!second) {
        *best = *best > level ? *best : level;
    } else if (level > *best) {
        *second = *best;
        *best = level;
    } else if (level > *second) {
        *second = level;
    }
}

// PROVEN: b22bfa12 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B380/0x1406F4980
static int32_t QSortItemLevels(const void* a, const void* b) {
    return *static_cast<const int32_t*>(b) - *static_cast<const int32_t*>(a);
}

// PROVEN: 18fe7b5e IDENTICAL/IDENTICAL
// OFFSET: 0x0097B390/0x1406F4990
static void AverageItemLevelCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success && CGCharacterInfo::m_avgItemLevelPending > 0) {
        if (!--CGCharacterInfo::m_avgItemLevelPending) {
            CGCharacterInfo::m_avgItemLevelDirty = 1;
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_AVG_ITEM_LEVEL_READY, NULL);
        }
    }
}

// OFFSET: 0x0097E0F0/0x1406F7DF0
float CGCharacterInfo::CalculateAverageItemLevel(bool equippedOnly) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || CGCharacterInfo::m_avgItemLevelPending > 0) {
        return 0.0f;
    }

    int32_t levels[29];
    memset(levels, 0, sizeof(levels));
    int32_t secondFinger = 0;
    int32_t secondTrinket = 0;
    int32_t secondWeapon = 0;
    int32_t secondTwoHand = 0;
    uint32_t lastSlot = equippedOnly ? 18 : 73;

    for (uint32_t slot = 0; slot <= lastSlot; slot++) {
        uint64_t itemGuid = slot >= static_cast<uint32_t>(player->m_bag.m_numSlots) ? 0 : player->m_bag.m_slots[slot].m_guid;
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(itemGuid), TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item) {
            continue;
        }

        WOWGUID requester = player->m_obj->m_guid;
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, AverageItemLevelCallback, NULL, false);

        if (!stats) {
            CGCharacterInfo::m_avgItemLevelPending++;
            continue;
        }

        int32_t error;

        if (!player->CanUseItem(stats, error) || !item->GetInventoryType()) {
            continue;
        }

        if (static_cast<const CGItem_C*>(item)->GetBag()) {
            for (uint32_t i = 0; i < static_cast<uint32_t>(static_cast<const CGItem_C*>(item)->GetBag()->m_numSlots); i++) {
                const CGBag_C* bag = static_cast<const CGItem_C*>(item)->GetBag();
                uint64_t bagItemGuid = i >= static_cast<uint32_t>(bag->m_numSlots) ? 0 : bag->m_slots[i].m_guid;
                CGItem_C* bagItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(bagItemGuid), TYPEMASK_ITEM, __FILE__, __LINE__));

                if (!bagItem) {
                    continue;
                }

                WOWGUID bagRequester = player->m_obj->m_guid;
                ItemRecSparse* bagStats = g_itemSparseDB.GetRecord(bagItem->m_obj->m_entryID, bagRequester, AverageItemLevelCallback, NULL, false);

                if (!bagStats) {
                    CGCharacterInfo::m_avgItemLevelPending++;
                    continue;
                }

                int32_t bagError;

                if (player->CanUseItem(bagStats, bagError) && bagItem->GetInventoryType()) {
                    uint32_t type = bagItem->GetInventoryType();
                    int32_t* second;

                    switch (type) {
                        case 11:
                            second = &secondFinger;
                            break;
                        case 12:
                            second = &secondTrinket;
                            break;
                        case 13:
                            second = &secondWeapon;
                            break;
                        case 17:
                            second = &secondTwoHand;
                            break;
                        default:
                            second = NULL;
                            break;
                    }

                    UpdateBestItemLevel(bagStats->m_itemLevel, &levels[type], second);
                }
            }
        } else {
            uint32_t type = item->GetInventoryType();
            int32_t* second;

            switch (type) {
                case 11:
                    second = &secondFinger;
                    break;
                case 12:
                    second = &secondTrinket;
                    break;
                case 13:
                    second = &secondWeapon;
                    break;
                case 17:
                    second = &secondTwoHand;
                    break;
                default:
                    second = NULL;
                    break;
            }

            UpdateBestItemLevel(stats->m_itemLevel, &levels[type], second);
        }
    }

    if (CGCharacterInfo::m_avgItemLevelPending > 0) {
        return 0.0f;
    }

    int32_t ranged = levels[26] > levels[28] ? levels[26] : levels[28];
    ranged = levels[25] > ranged ? levels[25] : ranged;
    ranged = levels[15] > ranged ? levels[15] : ranged;
    uint32_t classID = player->m_unit->m_unitFieldBytes0[1];
    int32_t twoHand = levels[17];
    int32_t mainHand;
    int32_t offHand;

    if (classID == 1 || classID == 4 || classID == 7 || classID == 3 || classID == 6) {
        int32_t weapons[6];
        weapons[0] = levels[21];
        weapons[1] = levels[14] > levels[22] ? levels[14] : levels[22];
        weapons[1] = levels[23] > weapons[1] ? levels[23] : weapons[1];
        weapons[2] = levels[13];
        weapons[3] = secondWeapon;

        if (classID == 1) {
            weapons[4] = levels[17];
            weapons[5] = secondTwoHand;
        } else {
            weapons[4] = 0;
            weapons[5] = 0;
        }

        qsort(weapons, 6, sizeof(int32_t), QSortItemLevels);
        mainHand = weapons[0];
        offHand = weapons[1];
    } else {
        mainHand = levels[21] > levels[13] ? levels[21] : levels[13];
        offHand = levels[23] > levels[14] ? levels[23] : levels[14];
    }

    levels[0] = 0;
    levels[4] = 0;
    levels[28] = 0;
    levels[17] = 0;
    levels[18] = 0;
    levels[19] = 0;
    levels[13] = 0;
    levels[14] = 0;
    levels[15] = 0;
    levels[21] = 0;
    levels[22] = 0;
    levels[23] = 0;
    levels[24] = 0;
    levels[25] = 0;
    levels[26] = 0;
    levels[5] = levels[5] > levels[20] ? levels[5] : levels[20];
    levels[20] = 0;
    int32_t total = 0;

    for (uint32_t i = 1; i < 29; i++) {
        total += levels[i];
    }

    total += secondFinger + secondTrinket + ranged;
    float oneHanded = static_cast<float>(total + mainHand + offHand) * (1.0f / 17.0f);
    float twoHanded = static_cast<float>(total + twoHand) * (1.0f / 16.0f);
    return oneHanded > twoHanded ? oneHanded : twoHanded;
}

// OFFSET: 0x0097CBC0/0x1406F65F0
int32_t CGCharacterInfo::PutItemInBackpack(uint32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !CGGameUI::m_hasControl || (slot - 67 <= 6 && !CGBankInfo::m_banker)) {
        return 0;
    }

    WOWGUID cursorItem;
    WOWGUID cursorContainer;
    uint32_t cursorSlot;
    CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);
    int32_t virtualId;
    int32_t virtualSlot;
    CGGameUI::GetCursorVirtualItem(virtualId, virtualSlot);
    WOWGUID bag;

    if (slot == 0xFF) {
        bag = player->m_obj->m_guid;
    } else {
        if (slot >= static_cast<uint32_t>(player->m_bag.m_numSlots)) {
            CGCharacterInfo::PickupInventoryItem(slot);
            return 0;
        }

        bag = player->m_bag.m_slots[slot];
    }

    if (!bag) {
        CGCharacterInfo::PickupInventoryItem(slot);
        return 0;
    }

    CGItem_C* bagItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(bag, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!(bagItem && (bagItem->m_itemFlags & 1)) && cursorItem != bag) {
        if (!cursorItem && !virtualId) {
            CGGameUI::ClearCursor(1, 1);
            return 0;
        }

        if (cursorContainer == bag && (cursorContainer != player->m_obj->m_guid || cursorSlot - 23 <= 15)) {
            CGGameUI::ClearCursor(1, 1);
            return 1;
        }

        if (!cursorItem) {
            if (CGGameUI::m_cursorItemType == 5 && CGMerchantInfo::m_merchant) {
                VendorItem* merchantItem = CGMerchantInfo::GetItem(virtualSlot);

                if (merchantItem) {
                    BuyItemInBag(CGMerchantInfo::m_merchant, merchantItem, merchantItem->m_buyCount, bag);
                }
            }

            CGGameUI::ClearCursor(1, 1);
            return 0;
        }

        if (slot != 0xFF) {
            CGItem_C* cursorObject;

            if ((cursorContainer == player->m_obj->m_guid && cursorSlot - 19 <= 3) || ((cursorObject = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__))) && (cursorObject->m_obj->m_type & TYPEMASK_CONTAINER))) {
                CGGameUI::LockItem(bag);
                player->SwapItems(cursorItem, cursorContainer, cursorSlot, player->m_obj->m_guid, slot, 0);
                return 1;
            }
        }

        if (CGGameUI::m_stackSplit) {
            player->SplitItem(cursorItem, cursorContainer, cursorSlot, bag, 0xFF, CGGameUI::m_stackSplit);
        } else {
            player->AutoStoreItemInBag(cursorItem, cursorContainer, cursorSlot, bag, 0);
        }

        CGGameUI::ClearCursor(0, 1);
        return 1;
    }

    CGGameUI::ClearCursor(1, 1);
    return 1;
}

// OFFSET: 0x0097CE70/0x1406F68A0
INV_ALERT_STATUS CGCharacterInfo::CalculateInventoryAlertStatus(CGPlayer_C* player, uint32_t slot) {
    CGItem_C* item = player->m_bag.GetItemPointer(s_inventoryAlertSlots[slot]);

    if (!item) {
        return INV_ALERT_NONE;
    }

    if (item->m_item->m_itemFieldFlags & 0x10) {
        return INV_ALERT_RED;
    }

    if ((item->m_item->m_itemFieldFlags & 0x8)) {
        return INV_ALERT_NONE;
    }

    if (item->m_item->m_itemFieldMaxdurability <= 0) {
        return INV_ALERT_NONE;
    }

    if ((item->m_item->m_itemFieldFlags & 0x8)) {
        CGTutorial::TriggerTutorial(36);
        return INV_ALERT_RED;
    }

    if (!item->m_item->m_itemFieldDurability) {
        CGTutorial::TriggerTutorial(36);
        return INV_ALERT_RED;
    }

    if (static_cast<float>(item->GetDurability()) / static_cast<float>(item->GetMaxDurability()) < 0.2f) {
        CGTutorial::TriggerTutorial(35);
        return INV_ALERT_YELLOW;
    }

    return INV_ALERT_NONE;
}

// PROVEN: faa74318 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B830/0x1406F4DF0
void CGCharacterInfo::ShutdownGame() {
    ClientInspectHonorStats::s_handler = NULL;
    ClientInspectRatedBgStats::s_handler = NULL;
    ClientServices::ClearMessageHandler(MSG_INSPECT_ARENA_TEAMS);
    CGCharacterInfo::m_avgItemLevelDirty = 1;
    CGCharacterInfo::m_avgItemLevelPending = 0;
}

// PROVEN: f7dc4934 IDENTICAL/IDENTICAL
// OFFSET: 0x0097B3F0/0x1406F49F0
void GuildNameCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_PLAYER_GUILD_UPDATE);
    }
}

// PROVEN: b49fd75f IDENTICAL/DIFFERENT
// OFFSET: 0x0097DFE0/0x1406F7CD0
void CGCharacterInfo::LeaveWorld() {
    WOWGUID guid = ClntObjMgrGetActivePlayer();

    if (guid) {
        ClntObjMgrUnsetObjMirrorHandler(guid, ID_PLAYER, 0xD98, PlayerCharacterPointsUpdateHandler, NULL);
    }

    CGCharacterInfo::m_invAlertStatus.Clear();
    WOWGUID none;
    none.m_guid = 0;

    if (none != CGCharacterInfo::m_inspectTarget) {
        CGCharacterInfo::m_inspectTarget.m_guid = 0;
        CGCharacterInfo::m_hasHonorData = 0;
        CGCharacterInfo::m_honorRequestPending = 0;

        for (uint32_t i = 0; i < 3; i++) {
            CGCharacterInfo::m_inspectArenaTeams[i].m_teamId = 0;
        }

        CGTalentInfo_Inspect::SetInspectTarget(none);
    }

    CGCharacterInfo::m_avgItemLevelPending = 0;
    CGCharacterInfo::m_avgItemLevelDirty = 1;
}
