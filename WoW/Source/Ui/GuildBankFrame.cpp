#include "GuildBankFrame.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F4DE58
int32_t CGGuildBankInfo::m_textDirty[8];

// STATIC: 0x00F4DE78
GuildBankLogEntry CGGuildBankInfo::m_log[9][25];

// STATIC: 0x00F501A0
GuildBankTabInfo CGGuildBankInfo::m_tabInfo[8];

// STATIC: 0x00F60BC0
int32_t CGGuildBankInfo::m_numLogEntries[9];

// STATIC: 0x00F60BE4
int32_t CGGuildBankInfo::m_numTabs;

// STATIC: 0x00F60BE8
WOWGUID CGGuildBankInfo::m_banker;

// STATIC: 0x00F60BF0
WOWGUID CGGuildBankInfo::m_pendingBanker;

// STATIC: 0x00F60BF8
int32_t CGGuildBankInfo::m_currentTab;

// STATIC: 0x00F60BFC
int32_t CGGuildBankInfo::m_pendingItemCallbacks;

// STATIC: 0x00F60C00
uint64_t CGGuildBankInfo::m_money;

// STATIC: 0x00F60C08
uint64_t CGGuildBankInfo::m_bonusDepositMoney;

// STATIC: 0x00F60C10
int32_t CGGuildBankInfo::m_pendingLogCallbacks;

// STATIC: 0x00F60C14
int32_t CGGuildBankInfo::m_remainingWithdrawals;

// STATIC: 0x00F60C18
uint64_t CGGuildBankInfo::m_remainingWithdrawMoney;

// STATIC: 0x00F60C20
TSGrowableArray<GuildBankItem> CGGuildBankInfo::m_tabSlots[8];

// STATIC: 0x00D15C68
uint32_t CGGuildBankInfo::m_logTab = 255;

// OFFSET: 0x009C02D0/0x14074CBE0
void CGGuildBankInfo::SetGuildBanker(const WOWGUID& banker) {
    if (!banker) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKFRAME_CLOSED, NULL);
        CGGameUI::ClearInteractTarget(CGGuildBankInfo::m_banker, 0);
        CGGuildBankInfo::m_banker = WOWGUID::Make(0);
        CGGuildBankInfo::m_pendingBanker = WOWGUID::Make(0);
        return;
    }

    CGGameUI::SetInteractTarget(banker);
    CGGuildBankInfo::m_banker = banker;
    CGGossipInfo::ClearGossip();
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKFRAME_OPENED, NULL);
}

// OFFSET: 0x009C0350
void CGGuildBankInfo::GetTabAndSlotFromVirtual(uint32_t virtualSlot, uint32_t& tab, uint32_t& slot) {
    tab = virtualSlot / 98;
    slot = virtualSlot % 98;
}

// OFFSET: 0x009C0380
void CGGuildBankInfo::SetTextUpdate(uint32_t tab, int32_t update) {
    CGGuildBankInfo::m_textDirty[tab] = update;
}

// PROVEN: 33bfb5e2 IDENTICAL/NONE
// OFFSET: 0x009C0940
void CGGuildBankInfo::GuildBankItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!CGGuildBankInfo::m_pendingItemCallbacks || !--CGGuildBankInfo::m_pendingItemCallbacks) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, NULL);
    }
}

// PROVEN: 3aa43c58 IDENTICAL/NONE
// OFFSET: 0x009C0970
void CGGuildBankInfo::GuildBankLogCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!CGGuildBankInfo::m_pendingLogCallbacks || !--CGGuildBankInfo::m_pendingLogCallbacks) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKLOG_UPDATE, NULL);
    }
}

// PROVEN: c115d3ca IDENTICAL/NONE
// OFFSET: 0x009C09A0
bool CGGuildBankInfo::ClientGuildBankMoneyWithdrawnHandler(ClientGuildBankMoneyWithdrawn* msg) {
    CGGuildBankInfo::m_remainingWithdrawMoney = msg->m_remainingWithdrawMoney;
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_WITHDRAWMONEY, NULL);
    return true;
}

// PROVEN: 90b0ea75 IDENTICAL/NONE
// OFFSET: 0x009C09D0
bool CGGuildBankInfo::ClientGuildBankTextQueryResultHandler(ClientGuildBankTextQueryResult* msg) {
    ScrubString(msg->m_text, CGGuildBankInfo::m_tabInfo[msg->m_tab].m_text, 2048, sizeof(CGGuildBankInfo::m_tabInfo[msg->m_tab].m_text), 0);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_TEXT, "%d", msg->m_tab + 1);
    return true;
}

// OFFSET: 0x009C0A20
void CGGuildBankInfo::ClearGuildTabList(uint32_t tab) {
    if (tab >= 8) {
        return;
    }

    for (uint32_t i = 0; i < CGGuildBankInfo::m_tabSlots[tab].Count(); i++) {
        GuildBankItem& item = CGGuildBankInfo::m_tabSlots[tab][i];
        item.m_itemID = 0;
        item.m_randomPropertiesID = 0;
        item.m_randomPropertiesSeed = 0;
        item.m_count = 0;
        item.m_enchantmentID = 0;
        item.m_charges = 0;
        item.m_socketEnchants[0] = 0;
        item.m_socketEnchants[1] = 0;
        item.m_socketEnchants[2] = 0;
        item.m_locked = 0;
        item.m_flags = 0;
    }
}

// OFFSET: 0x009C0AD0/0x14074D410
void CGGuildBankInfo::BuyGuildBankTab() {
    uint8_t tab = CGGuildBankInfo::m_numTabs;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_BUY_TAB));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(tab);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009C0B60
void CGGuildBankInfo::UpdateTabInfo(uint32_t tab, char* name, char* icon) {
    ScrubString(name, CGGuildBankInfo::m_tabInfo[tab].m_name, 16, sizeof(CGGuildBankInfo::m_tabInfo[tab].m_name), 1);
    SStrCopy(CGGuildBankInfo::m_tabInfo[tab].m_icon, icon, sizeof(CGGuildBankInfo::m_tabInfo[tab].m_icon));
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_TABS, NULL);
}

// OFFSET: 0x009C0BC0/0x14074D570
GuildBankItem* CGGuildBankInfo::GetGuildBankItem(uint32_t tab, uint32_t slot) {
    if (tab >= 8 || slot >= CGGuildBankInfo::m_tabSlots[tab].Count()) {
        return NULL;
    }

    GuildBankItem* item = &CGGuildBankInfo::m_tabSlots[tab][slot];

    if (item->m_itemID) {
        return item;
    }

    return NULL;
}

// OFFSET: 0x009C0BF0
int32_t CGGuildBankInfo::IsLocked(int32_t itemID, uint32_t virtualSlot) {
    uint32_t tab = virtualSlot / 98;
    uint32_t slot = virtualSlot % 98;

    if (tab >= 8 || slot >= CGGuildBankInfo::m_tabSlots[tab].Count()) {
        return 0;
    }

    GuildBankItem* item = &CGGuildBankInfo::m_tabSlots[tab][slot];

    if (itemID != item->m_itemID || !item->m_locked) {
        return 0;
    }

    return item->m_locked;
}

// OFFSET: 0x009C0C40/0x14074D5F0
void CGGuildBankInfo::Lock(int32_t itemID, uint32_t virtualSlot) {
    uint32_t tab = virtualSlot / 98;
    uint32_t slot = virtualSlot % 98;

    if (tab < 8 && slot < CGGuildBankInfo::m_tabSlots[tab].Count()) {
        GuildBankItem* item = &CGGuildBankInfo::m_tabSlots[tab][slot];

        if (itemID == item->m_itemID) {
            item->m_locked = 1;
        }
    }
}

// OFFSET: 0x009C0C90/0x14074D640
void CGGuildBankInfo::Unlock(int32_t itemID, uint32_t virtualSlot) {
    uint32_t tab = virtualSlot / 98;
    uint32_t slot = virtualSlot % 98;

    if (tab < 8 && slot < CGGuildBankInfo::m_tabSlots[tab].Count()) {
        GuildBankItem* item = &CGGuildBankInfo::m_tabSlots[tab][slot];

        if (itemID == item->m_itemID) {
            item->m_locked = 0;
        }
    }
}

// OFFSET: 0x009C0CE0/0x14074D690
void CGGuildBankInfo::EnterWorld() {
    for (uint32_t i = 0; i < 9; i++) {
        CGGuildBankInfo::m_numLogEntries[i] = 0;
    }

    CGGuildBankInfo::m_numTabs = 0;
    CGGuildBankInfo::m_banker = WOWGUID::Make(0);
    CGGuildBankInfo::m_pendingBanker = WOWGUID::Make(0);
    CGGuildBankInfo::m_logTab = 255;

    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < CGGuildBankInfo::m_tabSlots[i].Count(); j++) {
            CGGuildBankInfo::m_tabSlots[i][j].m_itemID = 0;
        }

        CGGuildBankInfo::m_tabInfo[i].m_contentsDirty = 1;
        CGGuildBankInfo::m_textDirty[i] = 1;
    }

    GlobalGuildBankRemainingWithdrawMoneyQuery query;
    ClientServices::Send(&query);
}

// OFFSET: 0x009C0DC0
void CGGuildBankInfo::QueryGuildBankTab(uint32_t tab) {
    if (!CGGuildBankInfo::m_banker) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_QUERY_TAB));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(static_cast<uint8_t>(tab));
    msg.Put(static_cast<uint8_t>(CGGuildBankInfo::m_tabInfo[tab].m_contentsDirty != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009C0E80
void CGGuildBankInfo::QueryGuildBankText(uint32_t tab) {
    if (!CGGuildBankInfo::m_banker || !CGGuildBankInfo::m_textDirty[tab]) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_TEXT, "%d", tab + 1);
        return;
    }

    GlobalGuildBankTextQuery query(tab);
    ClientServices::Send(&query);
    CGGuildBankInfo::m_textDirty[tab] = 0;
}

// OFFSET: 0x009C0EF0
void CGGuildBankInfo::SetTabInfo(uint32_t tab, const char* name, const char* icon) {
    char scrubbed[64];
    ScrubString(name, scrubbed, 16, sizeof(scrubbed), 1);

    int32_t error = ValidateGuildBankTabName(s_locale, scrubbed);

    if (error) {
        CGGameUI::DisplayNameError(error);
        return;
    }

    if (!CGGuildBankInfo::m_banker || static_cast<int32_t>(tab) >= CGGuildBankInfo::m_numTabs) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_UPDATE_TAB));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(static_cast<uint8_t>(tab));
    msg.PutString(scrubbed);
    msg.PutString(icon);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009C1000/0x14074DA20
void CGGuildBankInfo::DepositMoney(int64_t amount) {
    if (!CGGuildBankInfo::m_banker || amount >= 10000000000ll) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_DEPOSIT_MONEY));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(static_cast<uint64_t>(amount));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009C10D0/0x14074DB00
void CGGuildBankInfo::WithdrawMoney(int64_t amount) {
    if (!CGGuildBankInfo::m_banker || amount >= 10000000000ll) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_WITHDRAW_MONEY));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(static_cast<uint64_t>(amount));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009C11A0/0x14074DBE0
void CGGuildBankInfo::SetGuildBankText(uint32_t tab, const char* text) {
    if (!CGGuildBankInfo::m_banker || static_cast<int32_t>(tab) >= CGGuildBankInfo::m_numTabs) {
        return;
    }

    GlobalGuildBankSetTabText request;
    request.m_tab = tab;
    SStrCopy(request.m_tabText, text, sizeof(request.m_tabText));
    ClientServices::Send(&request);
}

// PROVEN: 3f4cf561 IDENTICAL/NONE
// OFFSET: 0x009C1F80
void CGGuildBankInfo::ClearContents() {
    for (uint32_t i = 0; i < 8; i++) {
        CGGuildBankInfo::m_tabSlots[i].Clear();
        CGGuildBankInfo::m_tabInfo[i].m_contentsDirty = 1;
        CGGuildBankInfo::m_textDirty[i] = 1;
    }

    CGGuildBankInfo::m_numTabs = 0;
    CGGuildBankInfo::m_currentTab = 0;
}

// PROVEN: a5bb2f62 IDENTICAL/NONE
// OFFSET: 0x009C2000
void CGGuildBankInfo::ShutdownGame() {
    ClientGuildBankQueryResults::s_handler = NULL;
    ClientGuildBankLogQueryResults::s_handler = NULL;
    ClientGuildBankMoneyWithdrawn::s_handler = NULL;
    ClientGuildBankTextQueryResult::s_handler = NULL;
    CGGuildBankInfo::ClearContents();
}

// OFFSET: 0x009C2020
bool CGGuildBankInfo::ClientGuildBankQueryResultsHandler(ClientGuildBankQueryResults* msg) {
    CGGuildBankInfo::m_remainingWithdrawals = msg->m_withdrawalsRemaining;

    if (CGGuildBankInfo::m_pendingItemCallbacks) {
        CGGuildBankInfo::m_pendingItemCallbacks = 0;

        for (uint32_t i = 0; i < CGGuildBankInfo::m_tabSlots[msg->m_tab].Count(); i++) {
            g_nameDBCache.CancelCallback(static_cast<int64_t>(CGGuildBankInfo::m_tabSlots[msg->m_tab][i].m_itemID), &CGGuildBankInfo::GuildBankItemStatsCallback, NULL);
        }
    }

    CGGuildBankInfo::m_money = msg->m_money >= 0x7FFFFFFFFFFFFull ? 0x7FFFFFFFFFFFFull : msg->m_money;

    if (!CGGuildBankInfo::m_tabSlots[msg->m_tab].Count()) {
        CGGuildBankInfo::m_tabSlots[msg->m_tab].SetCount(98);
        CGGuildBankInfo::ClearGuildTabList(msg->m_tab);
    }

    if (msg->m_tabInfo.Count()) {
        CGGuildBankInfo::m_numTabs = msg->m_tabInfo.Count();

        for (int32_t i = 0; i < CGGuildBankInfo::m_numTabs; i++) {
            CGGuildBankInfo::UpdateTabInfo(msg->m_tabInfo[i].m_tabIndex, msg->m_tabInfo[i].m_name, msg->m_tabInfo[i].m_icon);
        }

        CGGuildBankInfo::ClearGuildTabList(msg->m_tab);
    }

    int32_t cursorID = 0;
    uint32_t cursorSlot = 0;

    if (CGGameUI::m_cursorItemType == 10) {
        CGGameUI::GetCursorVirtualItem(cursorID, reinterpret_cast<int32_t&>(cursorSlot));
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return true;
    }

    WOWGUID requester = player->m_obj->m_guid;

    for (uint32_t i = 0; i < msg->m_itemInfo.Count(); i++) {
        JamGuildBankItemInfo& info = msg->m_itemInfo[i];
        uint32_t slot = info.m_slot;

        if (cursorID && msg->m_tab == cursorSlot / 98 && slot == cursorSlot % 98) {
            CGGameUI::ClearCursor(1, 1);
            CGGameUI::UnlockVirtualItem(cursorID, cursorSlot, 0);
        }

        GuildBankItem& item = CGGuildBankInfo::m_tabSlots[msg->m_tab][slot];
        item.m_itemID = info.m_itemID;

        if (!item.m_itemID) {
            continue;
        }

        item.m_flags = info.m_flags;
        item.m_randomPropertiesID = info.m_randomPropertiesID;
        item.m_randomPropertiesSeed = info.m_randomPropertiesSeed;
        item.m_count = info.m_count;
        item.m_enchantmentID = info.m_enchantmentID;
        item.m_charges = info.m_charges;
        item.m_reforgeID = info.m_reforgeID;
        item.m_onUseEnchantmentID = info.m_onUseEnchantmentID;
        item.m_locked = 0;
        item.m_socketEnchants[0] = 0;
        item.m_socketEnchants[1] = 0;
        item.m_socketEnchants[2] = 0;

        for (uint32_t j = 0; j < info.m_socketEnchant.Count(); j++) {
            item.m_socketEnchants[info.m_socketEnchant[j].m_socketIndex] = info.m_socketEnchant[j].m_socketEnchantID;
        }

        if (!g_itemSparseDB.GetRecord(item.m_itemID, requester, &CGGuildBankInfo::GuildBankItemStatsCallback, NULL, false)) {
            CGGuildBankInfo::m_pendingItemCallbacks++;
        }
    }

    CGGuildBankInfo::m_tabInfo[msg->m_tab].m_contentsDirty = 0;

    if (!CGGuildBankInfo::m_banker) {
        CGGuildBankInfo::SetGuildBanker(CGGuildBankInfo::m_pendingBanker);
    } else if (!CGGuildBankInfo::m_pendingItemCallbacks) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, NULL);
    }

    return true;
}

// OFFSET: 0x009C2400/0x14074F190
void CGGuildBankInfo::SwapItems(uint32_t tab, uint32_t slot, uint64_t item, uint64_t container, uint32_t containerSlot, int32_t split) {
    if (!CGGuildBankInfo::m_banker || !item) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(item), TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!object) {
        return;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, requester, NULL, NULL, false);

    if (!sparse) {
        return;
    }

    if (object->IsBound()) {
        if (sparse->m_bonding == 4) {
            CGGameUI::DisplayError(ERR_GUILD_BANK_QUEST_ITEM);
        } else {
            CGGameUI::DisplayError(ERR_GUILD_BANK_BOUND_ITEM);
        }

        return;
    }

    if ((object->GetItemStaticFlags(0) & 0x2) || ((object->GetItemStaticFlags(0) & 0x10000) && sparse->m_duration > 0) || object->m_item->m_itemFieldDuration || object->IsZoneBound() || object->IsInstanceBound()) {
        CGGameUI::DisplayError(ERR_GUILD_BANK_CONJURED_ITEM);
        return;
    }

    if (object->m_item->m_itemFieldFlags & 0x8) {
        CGGameUI::DisplayError(ERR_GUILD_BANK_WRAPPED_ITEM);
        return;
    }

    if (container == player->m_obj->m_guid.m_guid && containerSlot < 23) {
        CGGameUI::DisplayError(ERR_GUILD_BANK_EQUIPPED_ITEM);
        return;
    }

    CDataStore msg;
    int32_t bankItemID = 0;
    GuildBankItem* bankItem = CGGuildBankInfo::GetGuildBankItem(tab, slot);

    if (bankItem) {
        bankItemID = bankItem->m_itemID;
    }

    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_SWAP_ITEMS));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(static_cast<uint8_t>(tab));
    msg.Put(static_cast<uint8_t>(slot));
    msg.Put(static_cast<uint32_t>(bankItemID));

    if (!(CGGuildInfo::GetGuildBankTabPermissionsByRankID(tab, player->m_player->m_playerGuildrank) & 0x2)) {
        CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
        return;
    }

    uint8_t bag;
    uint8_t bagSlot;
    player->GetPackAndSlot(object, &bag, &bagSlot);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(bag);
    msg.Put(bagSlot);

    int32_t count = 0;

    if (object->GetStats() && object->GetStats()->m_stackable) {
        if (!bankItemID) {
            count = split;
        } else if (bankItemID == object->m_obj->m_entryID) {
            count = split;

            if (split) {
                if (split >= object->GetStats()->m_stackable - bankItem->m_count) {
                    count = object->GetStats()->m_stackable - bankItem->m_count;
                }
            } else if (object->m_item->m_itemFieldStackCount < object->GetStats()->m_stackable - bankItem->m_count) {
                count = object->m_item->m_itemFieldStackCount;
            } else {
                count = object->GetStats()->m_stackable - bankItem->m_count;
            }
        }
    }

    msg.Put(static_cast<uint8_t>(0));
    msg.Put(static_cast<uint32_t>(count));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::ClearCursor(1, 1);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, NULL);
}

// OFFSET: 0x009C2740
void CGGuildBankInfo::SwapBankItems(uint32_t tab, uint32_t slot, uint32_t itemID, uint32_t virtualSlot, int32_t split) {
    if (!CGGuildBankInfo::m_banker || !itemID) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (!(CGGuildInfo::GetGuildBankTabPermissionsByRankID(tab, player->m_player->m_playerGuildrank) & 0x2)) {
        CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
        return;
    }

    int32_t count = 0;
    CDataStore msg;
    int32_t bankItemID = 0;
    GuildBankItem* bankItem = CGGuildBankInfo::GetGuildBankItem(tab, slot);

    if (bankItem) {
        bankItemID = bankItem->m_itemID;
    }

    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_SWAP_ITEMS));
    msg.Put(CGGuildBankInfo::m_banker.m_guid);
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(static_cast<uint8_t>(tab));
    msg.Put(static_cast<uint8_t>(slot));
    msg.Put(static_cast<uint32_t>(bankItemID));

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(itemID, requester, NULL, NULL, false);
    int32_t stackable;

    if (!sparse || !(stackable = sparse->m_stackable)) {
        if (itemID == static_cast<uint32_t>(bankItemID)) {
            CGGameUI::ClearCursor(1, 1);
            CGGameUI::UnlockVirtualItem(bankItemID, slot + 98 * tab, 0);
            return;
        }
    } else if (!bankItemID) {
        count = split;
    } else if (itemID == static_cast<uint32_t>(bankItemID)) {
        count = split;

        if (split) {
            if (split >= stackable - bankItem->m_count) {
                count = stackable - bankItem->m_count;
            }
        } else {
            count = stackable - bankItem->m_count;

            if (CGGameUI::GetVirtualItemCount(itemID, virtualSlot, 0) < count) {
                count = CGGameUI::GetVirtualItemCount(itemID, virtualSlot, 0);
            }
        }
    }

    uint32_t otherTab = virtualSlot / 98;
    uint32_t otherSlot = virtualSlot % 98;

    if (otherTab != tab && ((bankItemID && !CGGuildInfo::GetNumGuildBankTabItemWithdrawalsByRankID(tab, player->m_player->m_playerGuildrank)) || !CGGuildInfo::GetNumGuildBankTabItemWithdrawalsByRankID(static_cast<uint8_t>(otherTab), player->m_player->m_playerGuildrank))) {
        CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
        return;
    }

    msg.Put(static_cast<uint8_t>(otherTab));
    msg.Put(static_cast<uint8_t>(otherSlot));
    msg.Put(itemID);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(static_cast<uint32_t>(count));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::ClearCursor(1, 1);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, NULL);
}

// OFFSET: 0x009C29E0
void CGGuildBankInfo::AutoStoreItem(uint64_t item, uint64_t container, uint32_t containerSlot) {
    if (CGGuildBankInfo::m_banker) {
        CGGuildBankInfo::SwapItems(CGGuildBankInfo::m_currentTab, 0xFF, item, container, containerSlot, 0);
    }
}

// OFFSET: 0x009C2A20
bool CGGuildBankInfo::IsTabFiltered(int32_t tab) {
    if (!CGContainerInfo::IsSearchActive()) {
        return false;
    }

    if (tab < 0 || tab >= CGGuildBankInfo::m_numTabs) {
        return true;
    }

    for (uint32_t i = 0; i < CGGuildBankInfo::m_tabSlots[tab].Count(); i++) {
        if (static_cast<uint32_t>(tab) >= 8 || i >= CGGuildBankInfo::m_tabSlots[tab].Count()) {
            continue;
        }

        GuildBankItem* item = &CGGuildBankInfo::m_tabSlots[tab][i];

        if (!item->m_itemID || !item) {
            continue;
        }

        WOWGUID requester = WOWGUID::Make(0);
        ItemRec* rec = g_itemDB.GetRecord(item->m_itemID, requester, NULL, NULL, false);

        if (rec && !ItemRecIsFiltered(rec, item->m_randomPropertiesID)) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x009C3850
bool CGGuildBankInfo::ClientGuildBankLogQueryResultsHandler(ClientGuildBankLogQueryResults* msg) {
    int32_t now = _time32(NULL);
    uint32_t tab = msg->m_tab;

    if (CGGuildBankInfo::m_pendingLogCallbacks && CGGuildBankInfo::m_logTab < 9) {
        CGGuildBankInfo::m_pendingLogCallbacks = 0;

        for (uint32_t i = 0; i < 25; i++) {
            g_nameDBCache.CancelCallback(CGGuildBankInfo::m_log[CGGuildBankInfo::m_logTab][i].m_playerGUID, &CGGuildBankInfo::GuildBankLogCallback, NULL);

            int32_t itemID = CGGuildBankInfo::m_log[CGGuildBankInfo::m_logTab][i].m_itemID;

            if (itemID) {
                g_itemSparseDB.CancelCallback(itemID, &CGGuildBankInfo::GuildBankLogCallback, NULL);
            }
        }
    }

    CGGuildBankInfo::m_logTab = tab;

    for (uint32_t i = 0; i < msg->m_entry.Count(); i++) {
        JamGuildBankLogEntry& entry = msg->m_entry[i];
        GuildBankLogEntry& log = CGGuildBankInfo::m_log[tab][i];

        log.m_type = entry.m_entryType;
        log.m_playerGUID = *reinterpret_cast<uint64_t*>(entry.m_playerGUID);
        log.m_time = now - entry.m_timeOffset;
        log.m_money = entry.m_hasMoney ? entry.m_money : 0;

        if (entry.m_hasItemID) {
            log.m_itemID = entry.m_itemID;

            if (!g_itemSparseDB.GetRecord(entry.m_itemID, reinterpret_cast<WOWGUID&>(log.m_playerGUID), &CGGuildBankInfo::GuildBankLogCallback, NULL, false)) {
                CGGuildBankInfo::m_pendingLogCallbacks++;
            }
        } else {
            log.m_itemID = 0;
        }

        log.m_count = entry.m_hasCount ? entry.m_count : 0;
        log.m_otherTab = entry.m_hasOtherTab ? entry.m_otherTab : 0;

        uint64_t guid = log.m_playerGUID;

        if (!(guid & 0xF000000000000000ull) && (guid & 0xF07FFFFFFFFFFFFFull) && !g_nameDBCache.GetRecord(guid, reinterpret_cast<WOWGUID&>(log.m_playerGUID), &CGGuildBankInfo::GuildBankLogCallback, NULL, false)) {
            CGGuildBankInfo::m_pendingLogCallbacks++;
        }
    }

    if (msg->m_hasWeeklyBonusMoney) {
        CGGuildBankInfo::m_bonusDepositMoney = msg->m_weeklyBonusMoney;
    }

    CGGuildBankInfo::m_numLogEntries[tab] = msg->m_entry.Count();

    if (CGGuildBankInfo::m_banker && !CGGuildBankInfo::m_pendingLogCallbacks) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKLOG_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x009C3A70/0x140750DA0
void CGGuildBankInfo::InitializeGame() {
    ClientGuildBankQueryResults::s_handler = &CGGuildBankInfo::ClientGuildBankQueryResultsHandler;
    ClientGuildBankLogQueryResults::s_handler = &CGGuildBankInfo::ClientGuildBankLogQueryResultsHandler;
    ClientGuildBankMoneyWithdrawn::s_handler = &CGGuildBankInfo::ClientGuildBankMoneyWithdrawnHandler;
    ClientGuildBankTextQueryResult::s_handler = &CGGuildBankInfo::ClientGuildBankTextQueryResultHandler;
    CGGuildBankInfo::m_numTabs = 0;
    memset(CGGuildBankInfo::m_tabInfo, 0, sizeof(CGGuildBankInfo::m_tabInfo));
}
