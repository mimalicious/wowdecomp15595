#ifndef WOW_SOURCE_UI_GUILDBANKFRAME_H
#define WOW_SOURCE_UI_GUILDBANKFRAME_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct GuildBankTabInfo {
    /* 0000 */ char m_name[64];
    /* 0040 */ char m_icon[256];
    /* 0140 */ char m_text[8192];
    /* 2140 */ int32_t m_contentsDirty;
};

struct GuildBankLogEntry {
    /* 0000 */ int32_t m_type;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t m_playerGUID;
    /* 0010 */ int32_t m_itemID;
    /* 0014 */ int32_t m_count;
    /* 0018 */ uint64_t m_money;
    /* 0020 */ int32_t m_otherTab;
    /* 0024 */ int32_t m_time;
};

struct GuildBankItem {
    /* 0000 */ int32_t m_itemID;
    /* 0004 */ int32_t m_randomPropertiesSeed;
    /* 0008 */ int32_t m_randomPropertiesID;
    /* 000C */ int32_t m_count;
    /* 0010 */ int32_t m_enchantmentID;
    /* 0014 */ int32_t m_charges;
    /* 0018 */ int32_t m_socketEnchants[3];
    /* 0024 */ int32_t m_reforgeID;
    /* 0028 */ int32_t m_onUseEnchantmentID;
    /* 002C */ int32_t m_locked;
    /* 0030 */ uint32_t m_flags;
};

class CGGuildBankInfo {
    public:
    static int32_t m_textDirty[8];
    static GuildBankLogEntry m_log[9][25];
    static GuildBankTabInfo m_tabInfo[8];
    static int32_t m_numLogEntries[9];
    static int32_t m_numTabs;
    static WOWGUID m_banker;
    static WOWGUID m_pendingBanker;
    static int32_t m_currentTab;
    static int32_t m_pendingItemCallbacks;
    static uint64_t m_money;
    static uint64_t m_bonusDepositMoney;
    static int32_t m_pendingLogCallbacks;
    static int32_t m_remainingWithdrawals;
    static uint64_t m_remainingWithdrawMoney;
    static TSGrowableArray<GuildBankItem> m_tabSlots[8];
    static uint32_t m_logTab;

    static void SetGuildBanker(const WOWGUID& banker);
    static void GetTabAndSlotFromVirtual(uint32_t virtualSlot, uint32_t& tab, uint32_t& slot);
    static void SetTextUpdate(uint32_t tab, int32_t update);
    static void GuildBankItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void GuildBankLogCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static bool ClientGuildBankMoneyWithdrawnHandler(ClientGuildBankMoneyWithdrawn* msg);
    static bool ClientGuildBankTextQueryResultHandler(ClientGuildBankTextQueryResult* msg);
    static void ClearGuildTabList(uint32_t tab);
    static void BuyGuildBankTab();
    static void UpdateTabInfo(uint32_t tab, char* name, char* icon);
    static GuildBankItem* GetGuildBankItem(uint32_t tab, uint32_t slot);
    static int32_t IsLocked(int32_t itemID, uint32_t virtualSlot);
    static void Lock(int32_t itemID, uint32_t virtualSlot);
    static void Unlock(int32_t itemID, uint32_t virtualSlot);
    static void EnterWorld();
    static void QueryGuildBankTab(uint32_t tab);
    static void QueryGuildBankText(uint32_t tab);
    static void SetTabInfo(uint32_t tab, const char* name, const char* icon);
    static void DepositMoney(int64_t amount);
    static void WithdrawMoney(int64_t amount);
    static void SetGuildBankText(uint32_t tab, const char* text);
    static void ClearContents();
    static void ShutdownGame();
    static bool ClientGuildBankQueryResultsHandler(ClientGuildBankQueryResults* msg);
    static void SwapItems(uint32_t tab, uint32_t slot, uint64_t item, uint64_t container, uint32_t containerSlot, int32_t split);
    static void SwapBankItems(uint32_t tab, uint32_t slot, uint32_t itemID, uint32_t virtualSlot, int32_t split);
    static void AutoStoreItem(uint64_t item, uint64_t container, uint32_t containerSlot);
    static bool IsTabFiltered(int32_t tab);
    static bool ClientGuildBankLogQueryResultsHandler(ClientGuildBankLogQueryResults* msg);
    static void InitializeGame();
};

void GuildBankRegisterScriptFunctions();
void GuildBankUnregisterScriptFunctions();

#endif
