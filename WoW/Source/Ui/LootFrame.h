#ifndef WOW_SOURCE_UI_LOOTFRAME_H
#define WOW_SOURCE_UI_LOOTFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
class CDataStore;
class CGObject_C;
class ClientCurrencyLootRemoved;
class ClientCurrencyLootRestored;
class ClientLootContents;

void LootDestroy();
void LootInitialize();

struct LootSlot {
    /* 0000 */ uint32_t m_pendingEvent;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_displayID;
    /* 000C */ int32_t m_currencyID;
    /* 0010 */ int32_t m_quantity;
    /* 0014 */ int32_t m_randomSuffix;
    /* 0018 */ int32_t m_randomPropertiesID;
    /* 001C */ uint8_t m_lootListID;
    /* 001D */ uint8_t unk_001D[3];
    /* 0020 */ int32_t m_slotType;
};

struct MissingLootItem {
    /* 0000 */ int32_t m_itemId;
    /* 0004 */ int32_t m_displayId;
    /* 0008 */ int32_t m_quantity;
    /* 000C */ int32_t m_seed;
    /* 0010 */ int32_t m_randomPropertiesId;
    /* 0014 */ uint8_t m_pending;
    /* 0015 */ uint8_t unk_0015[3];
};

class CGLootInfo {
    public:
    static uint32_t m_missingItemsPending;
    static uint32_t m_itemsPending;
    static uint32_t m_lootType;
    static int32_t m_coins;
    static LootSlot m_slots[24];
    static WOWGUID m_lootObj;
    static int32_t m_autolooted;
    static TSGrowableArray<MissingLootItem> m_missingItems;

    static void InitializeGame();
    static void ShutdownGame();
    static void LeaveWorld();
    static int32_t GetLootItem(uint32_t slot);
    static int32_t GetLootCurrency(uint32_t slot);
    static int32_t GetLootQuantity(uint32_t slot);
    static int32_t GetLootSeed(uint32_t slot);
    static int32_t GetLootRandomPropertiesID(uint32_t slot);
    static uint32_t GetLootType();
    static const char* GetLootSlotTexture(uint32_t slot);
    static void GetLootSlotText(char* buffer, uint32_t size, uint32_t slot);
    static int32_t HasLoot();
    static bool IsPlayerIneligible(int32_t itemId);
    static bool OnCurrencyLootRestored(ClientCurrencyLootRestored* msg);
    static bool OnCurrencyLootRemoved(ClientCurrencyLootRemoved* msg);
    static void ClearRollPending(WOWGUID guid, uint32_t slot);
    static void CoinsCleared();
    static void GiveMasterLoot(uint32_t slot, WOWGUID target);
    static void DoAutoLoot(int32_t keepOpen);
    static char* GetLootSlotLink(uint32_t slot, char* buffer, uint32_t size);
    static void LootSlot(uint32_t slot);
    static void ConfirmLootSlot(uint32_t slot);
    static void LootButtonItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static bool OnLootContents(ClientLootContents* msg);
    static void SetObject(CGObject_C* object, CDataStore* msg, uint32_t lootType);
    static void ClearSlot(uint8_t slot);
    static void ChangedSlot(uint8_t slot, CDataStore* msg);
};

class CGLootWindow {
    public:
    static int32_t GetNumLootItems();
    static int32_t GetLootItemRarity(uint32_t slot);
};

void MasterLootCandidateCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void LootInfoRegisterScriptFunctions();
void LootInfoUnregisterScriptFunctions();

void CurrencyBreakdown(uint64_t amount, uint64_t* parts);
void GetCurrencyTextureString(int64_t amount, char* buffer, uint32_t size, int32_t height);
void GetCurrencyText(int64_t amount, char* buffer, uint32_t size, const char* separator);
void GetCurrencyShortText(int64_t amount, char* buffer, uint32_t size, const char* separator);
void GetCurrencyIcon(uint64_t amount, char* buffer, uint32_t size);

#endif
