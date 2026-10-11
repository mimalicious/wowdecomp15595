#include "LootFrame.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F44EC0
uint32_t CGLootInfo::m_missingItemsPending;

// STATIC: 0x00F44EC4
uint32_t CGLootInfo::m_itemsPending;

// STATIC: 0x00F44EC8
uint32_t CGLootInfo::m_lootType;

// STATIC: 0x00F44ECC
int32_t CGLootInfo::m_coins;

// STATIC: 0x00F44ED0
LootSlot CGLootInfo::m_slots[24];

// STATIC: 0x00F45230
WOWGUID CGLootInfo::m_lootObj;

// STATIC: 0x00F45238
int32_t CGLootInfo::m_autolooted;

// STATIC: 0x00F4523C
TSGrowableArray<MissingLootItem> CGLootInfo::m_missingItems;

// STATIC: 0x00F45250
static char s_lootSlotTexture[260];

// STATIC: 0x00D12DB0
static int32_t s_bindConfirmSlot = -1;

// STATIC: 0x00DC715C
static uint32_t s_currencyMultiplier[3];

// STATIC: 0x00DC7168
static uint8_t s_lootInitialized;

// STATIC: 0x00CA1364
static const char* s_currencyAmountTokens[3] = { "COPPER_AMOUNT", "SILVER_AMOUNT", "GOLD_AMOUNT" };

// STATIC: 0x00CA1370
static const char* s_currencyTextureTokens[3] = { "COPPER_AMOUNT_TEXTURE", "SILVER_AMOUNT_TEXTURE", "GOLD_AMOUNT_TEXTURE" };

// STATIC: 0x00CA137C
static const char* s_currencySymbolTokens[3] = { "COPPER_AMOUNT_SYMBOL", "SILVER_AMOUNT_SYMBOL", "GOLD_AMOUNT_SYMBOL" };

// OFFSET: 0x004E3990
void CurrencyBreakdown(uint64_t amount, uint64_t* parts) {
    for (int32_t i = 2; i; i--) {
        uint32_t multiplier = s_currencyMultiplier[i];

        if (amount >= multiplier) {
            parts[i] = amount / multiplier;
            amount %= multiplier;
        } else {
            parts[i] = 0;
        }
    }

    parts[0] = amount;
}

// PROVEN: 32af9dfe IDENTICAL/NONE
// OFFSET: 0x004E3A00/0x140124BF0
void LootInitialize() {
    s_currencyMultiplier[0] = 1;
    s_currencyMultiplier[1] = 100;
    s_currencyMultiplier[2] = 10000;
    s_lootInitialized = 1;
}

// PROVEN: 99b26c7d IDENTICAL/NONE
// OFFSET: 0x004E3A30/0x140124C20
void LootDestroy() {
    s_lootInitialized = 0;
}

// OFFSET: 0x004E3A40/0x140124C30
void GetCurrencyIcon(uint64_t amount, char* buffer, uint32_t size) {
    const char* path = ClientDBStringLookup(3);
    SStrPrintf(buffer, size, "%s%s", path, *path ? "\\" : "");

    if (amount < 10) {
        SStrPack(buffer, "INV_Misc_Coin_05", size);
    } else if (amount < 100) {
        SStrPack(buffer, "INV_Misc_Coin_06", size);
    } else if (amount < 1000) {
        SStrPack(buffer, "INV_Misc_Coin_03", size);
    } else if (amount < 10000) {
        SStrPack(buffer, "INV_Misc_Coin_04", size);
    } else {
        SStrPack(buffer, amount >= 100000 ? "INV_Misc_Coin_02" : "INV_Misc_Coin_01", size);
    }
}

// OFFSET: 0x004E3B20/0x140124D40
void GetCurrencyText(int64_t amount, char* buffer, uint32_t size, const char* separator) {
    uint64_t parts[3];
    CurrencyBreakdown(amount, parts);
    *buffer = 0;

    for (int32_t i = 2; i >= 0; i--) {
        uint64_t value = parts[i];

        if (!value) {
            continue;
        }

        if (*buffer) {
            SStrPack(buffer, separator, size);
        }

        char text[1024];
        SStrPrintf(text, sizeof(text), FrameScript_GetText(s_currencyAmountTokens[i], -1, GENDER_NOT_APPLICABLE), value);
        SStrPack(buffer, text, size);
    }
}

// OFFSET: 0x004E3C40/0x140124ED0
void GetCurrencyTextureString(int64_t amount, char* buffer, uint32_t size, int32_t height) {
    if (!amount) {
        SStrPrintf(buffer, size, FrameScript_GetText(s_currencyTextureTokens[0], -1, GENDER_NOT_APPLICABLE), 0, height, height);
        return;
    }

    uint64_t parts[3];
    CurrencyBreakdown(amount, parts);
    *buffer = 0;

    for (int32_t i = 2; i >= 0; i--) {
        uint64_t value = parts[i];

        if (!value) {
            continue;
        }

        if (value > 0x7FFFFFFF) {
            SStrPrintf(buffer, size, FrameScript_GetText(s_currencyTextureTokens[0], -1, GENDER_NOT_APPLICABLE), 0, height, height);
            return;
        }

        if (*buffer) {
            SStrPack(buffer, " ", size);
        }

        char text[1024];
        SStrPrintf(text, sizeof(text), FrameScript_GetText(s_currencyTextureTokens[i], -1, GENDER_NOT_APPLICABLE), static_cast<uint32_t>(value), height, height);
        SStrPack(buffer, text, size);
    }
}

// OFFSET: 0x004E3DE0/0x1401250F0
void GetCurrencyShortText(int64_t amount, char* buffer, uint32_t size, const char* separator) {
    if (!amount) {
        SStrPrintf(buffer, size, "%u%s", 0, FrameScript_GetText(s_currencySymbolTokens[0], -1, GENDER_NOT_APPLICABLE));
        return;
    }

    uint64_t parts[3];
    CurrencyBreakdown(amount, parts);
    *buffer = 0;

    for (int32_t i = 2; i >= 0; i--) {
        uint64_t value = parts[i];

        if (!value) {
            continue;
        }

        if (value >> 32) {
            SStrPrintf(buffer, size, "%u%s", 0, FrameScript_GetText(s_currencySymbolTokens[0], -1, GENDER_NOT_APPLICABLE));
            return;
        }

        if (*buffer) {
            SStrPack(buffer, separator, size);
        }

        char text[1024];
        SStrPrintf(text, sizeof(text), "%u%s", static_cast<uint32_t>(value), FrameScript_GetText(s_currencySymbolTokens[i], -1, GENDER_NOT_APPLICABLE));
        SStrPack(buffer, text, size);
    }
}

// OFFSET: 0x00959950/0x1406CBF70
void MasterLootCandidateCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_MASTER_LOOT_LIST, NULL);
    }
}

// PROVEN: 09095f6f IDENTICAL/NONE
// OFFSET: 0x00959970/0x1406CBF90
void CGLootInfo::LeaveWorld() {
    CGGameUI::CloseLoot(true, false, false);
}

// OFFSET: 0x00959980
int32_t CGLootWindow::GetNumLootItems() {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < 24; i++) {
        if (CGLootInfo::m_slots[i].m_itemID > 0 || CGLootInfo::m_slots[i].m_currencyID > 0) {
            count = i + 1;
        }
    }

    if (CGLootInfo::m_coins) {
        count++;
    }

    return count;
}

// OFFSET: 0x00959A20/0x1406CBFA0
int32_t CGLootInfo::GetLootItem(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return 0;
        }

        slot--;
    }

    if (slot >= 24) {
        return 0;
    }

    return CGLootInfo::m_slots[slot].m_itemID;
}

// OFFSET: 0x00959A60/0x1406CBFE0
int32_t CGLootInfo::GetLootCurrency(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return 0;
        }

        slot--;
    }

    if (slot >= 24) {
        return 0;
    }

    return CGLootInfo::m_slots[slot].m_currencyID;
}

// OFFSET: 0x00959AA0/0x1406CC020
int32_t CGLootInfo::GetLootQuantity(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return 0;
        }

        slot--;
    }

    if (slot >= 24) {
        return 0;
    }

    return CGLootInfo::m_slots[slot].m_quantity;
}

// OFFSET: 0x00959AE0/0x1406CC060
int32_t CGLootInfo::GetLootSeed(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return 0;
        }

        slot--;
    }

    if (slot >= 24) {
        return 0;
    }

    return CGLootInfo::m_slots[slot].m_randomSuffix;
}

// OFFSET: 0x00959B20/0x1406CC0A0
int32_t CGLootInfo::GetLootRandomPropertiesID(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return 0;
        }

        slot--;
    }

    if (slot >= 24) {
        return 0;
    }

    return CGLootInfo::m_slots[slot].m_randomPropertiesID;
}

// OFFSET: 0x00959B60/0x1406CC0E0
uint32_t CGLootInfo::GetLootType() {
    return CGLootInfo::m_lootType;
}

// OFFSET: 0x0095A190
void CGLootInfo::ShutdownGame() {
    ClientCurrencyLootRemoved::s_handler = NULL;
    ClientCurrencyLootRestored::s_handler = NULL;
    ClientLootContents::s_handler = NULL;
}

// OFFSET: 0x0095A1B0
bool CGLootInfo::OnCurrencyLootRestored(ClientCurrencyLootRestored* msg) {
    uint8_t lootListID = msg->m_lootListID;
    uint32_t index = static_cast<uint32_t>(-1);

    for (uint32_t i = 0; i < 24; i++) {
        if (CGLootInfo::m_slots[i].m_currencyID && lootListID == CGLootInfo::m_slots[i].m_lootListID) {
            index = i;
            break;
        }
    }

    if (index >= 24) {
        for (uint32_t i = 0; i < 24; i++) {
            if (!CGLootInfo::m_slots[i].m_itemID && !CGLootInfo::m_slots[i].m_currencyID) {
                index = i;
                break;
            }
        }

        if (index >= 24) {
            return true;
        }
    }

    ::LootSlot& slot = CGLootInfo::m_slots[index];
    slot.m_currencyID = msg->m_currencyID;
    slot.m_quantity = msg->m_quantity;
    slot.m_lootListID = lootListID;

    if (CGLootInfo::m_coins) {
        index++;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CHANGED, "%d", index + 1);
    return true;
}

// OFFSET: 0x0095A260/0x1406CC850
const char* CGLootInfo::GetLootSlotTexture(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return NULL;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            if (CGLootInfo::m_coins >= 0) {
                GetCurrencyIcon(static_cast<int64_t>(CGLootInfo::m_coins), s_lootSlotTexture, sizeof(s_lootSlotTexture));
                return s_lootSlotTexture;
            }

            return NULL;
        }

        slot--;
    }

    if (slot >= 24) {
        return NULL;
    }

    if (CGLootInfo::m_slots[slot].m_itemID) {
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(s_lootSlotTexture, sizeof(s_lootSlotTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(CGLootInfo::m_slots[slot].m_displayID));
        return s_lootSlotTexture;
    }

    if (int32_t currencyID = CGLootInfo::m_slots[slot].m_currencyID) {
        if (CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyID)) {
            const char* path = ClientDBStringLookup(3);
            SStrPrintf(s_lootSlotTexture, sizeof(s_lootSlotTexture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
            return s_lootSlotTexture;
        }
    }

    return NULL;
}

// OFFSET: 0x0095A350/0x1406CC9A0
void CGLootInfo::GetLootSlotText(char* buffer, uint32_t size, uint32_t slot) {
    *buffer = 0;

    if (!CGLootInfo::m_lootObj) {
        return;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            if (CGLootInfo::m_coins > 0) {
                GetCurrencyText(static_cast<int64_t>(CGLootInfo::m_coins), buffer, size, "\n");
            }

            return;
        }

        slot--;
    }

    if (slot >= 24) {
        return;
    }

    if (int32_t itemID = CGLootInfo::m_slots[slot].m_itemID) {
        CGItem_C::BuildItemName(buffer, size, itemID, CGLootInfo::m_slots[slot].m_randomPropertiesID);
    }

    if (int32_t currencyID = CGLootInfo::m_slots[slot].m_currencyID) {
        if (CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyID)) {
            SStrCopy(buffer, currency->m_name, size);
        }
    }
}

// OFFSET: 0x0095A430/0x1406CCAE0
int32_t CGLootInfo::HasLoot() {
    if (!ClntObjMgrObjectPtr(CGLootInfo::m_lootObj, TYPEMASK_OBJECT, __FILE__, __LINE__)) {
        return 0;
    }

    if (CGLootInfo::m_coins > 0) {
        return 1;
    }

    for (uint32_t i = 0; i < 24; i++) {
        if (CGLootInfo::m_slots[i].m_itemID || CGLootInfo::m_slots[i].m_currencyID) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0095A490
static void MissingLootItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    for (uint32_t i = 0; i < CGLootInfo::m_missingItems.m_count; i++) {
        MissingLootItem& item = CGLootInfo::m_missingItems.m_data[i];

        if (item.m_pending && item.m_itemId == id) {
            item.m_pending = 0;
        }
    }

    if (!--CGLootInfo::m_missingItemsPending) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MISSING_OUT_ON_LOOT, NULL);
    }
}

// OFFSET: 0x0095A500
bool CGLootInfo::IsPlayerIneligible(int32_t itemId) {
    for (uint32_t i = 0; i < CGLootInfo::m_missingItems.m_count; i++) {
        if (CGLootInfo::m_missingItems.m_data[i].m_itemId == itemId) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0095A600
bool CGLootInfo::OnCurrencyLootRemoved(ClientCurrencyLootRemoved* msg) {
    uint32_t index;

    for (index = 0; index < 24; index++) {
        ::LootSlot& slot = CGLootInfo::m_slots[index];

        if (!slot.m_itemID && slot.m_currencyID && msg->m_lootListID == slot.m_lootListID) {
            break;
        }
    }

    if (index >= 24) {
        return true;
    }

    memset(&CGLootInfo::m_slots[index], 0, sizeof(CGLootInfo::m_slots[index]));

    if (CGLootInfo::m_coins) {
        index++;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CLEARED, "%d", index + 1);

    if (!CGLootInfo::HasLoot()) {
        CGGameUI::CloseLoot(true, false, false);
    }

    return true;
}

// OFFSET: 0x0095A6A0
void CGLootInfo::ClearRollPending(WOWGUID guid, uint32_t slot) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) || CGLootInfo::m_lootObj != guid) {
        return;
    }

    for (uint32_t i = 0; i < 24; i++) {
        if (CGLootInfo::m_slots[i].m_lootListID == slot && !CGLootInfo::m_slots[i].m_currencyID) {
            CGLootInfo::m_slots[slot].m_slotType = 0;
            return;
        }
    }
}

// PROVEN: f72cc58e IDENTICAL/NONE
// OFFSET: 0x0095A710
void CGLootInfo::CoinsCleared() {
    if (CGLootInfo::m_coins > 0) {
        CGLootInfo::m_coins = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CLEARED, "%d", 1);

        if (!CGLootInfo::HasLoot()) {
            CGGameUI::CloseLoot(true, false, false);
        }
    }
}

// OFFSET: 0x0095A750/0x1406CCE90
void CGLootInfo::GiveMasterLoot(uint32_t slot, WOWGUID target) {
    if (!CGLootInfo::m_lootObj) {
        return;
    }

    if (CGLootInfo::m_coins) {
        slot--;
    }

    if (slot >= 24 || !CGLootInfo::m_slots[slot].m_itemID || CGLootInfo::m_slots[slot].m_slotType != 2) {
        return;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LOOT_MASTER_GIVE));
    msg.Put(CGLootInfo::m_lootObj.m_guid);
    msg.Put(CGLootInfo::m_slots[slot].m_lootListID);
    msg.Put(target.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0095A8C0
void CGLootInfo::DoAutoLoot(int32_t keepOpen) {
    if (CGLootInfo::m_itemsPending) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_OPENED, "%d", 1);
        return;
    }

    if (CGLootInfo::m_coins) {
        player->LootMoney();
    }

    bool confirmShown = false;

    for (uint32_t i = 0; i < 24; i++) {
        ::LootSlot& slot = CGLootInfo::m_slots[i];

        if (slot.m_currencyID) {
            PlayerLootCurrency loot;
            loot.m_lootListID = slot.m_lootListID;
            ClientServices::Send(&loot);
            continue;
        }

        if (!slot.m_itemID) {
            continue;
        }

        int32_t type = slot.m_slotType;

        if (type && type != 4) {
            continue;
        }

        ItemRecSparse* rec = g_itemSparseDB.GetRecord(slot.m_itemID, CGLootInfo::m_lootObj, NULL, NULL, false);

        if (!rec) {
            continue;
        }

        ItemRec* item = rec->GetNonSparse();

        if (!item) {
            continue;
        }

        if (rec->m_bonding != 1 || rec->m_quality < 2 || (rec->m_flags[0] & 0x800) || type == 4) {
            SI3::PlayItemSound(0, item->m_displayInfoID);
            CGPlayer_C::AutoStoreLootItem(slot.m_lootListID);
        } else if (!confirmShown) {
            if (CGLootInfo::m_coins) {
                FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_BIND_CONFIRM, "%u", i + 2);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_BIND_CONFIRM, "%u", i + 1);
            }

            s_bindConfirmSlot = i;
            confirmShown = true;
        }
    }

    player->SetAutoLoot(0);
    CGLootInfo::m_autolooted = 1;

    if (CGLootInfo::HasLoot()) {
        if (keepOpen) {
            return;
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_OPENED, "%d", 1);
        return;
    }

    CGGameUI::CloseLoot(true, false, false);
}

// OFFSET: 0x0095AA80/0x1406CD320
int32_t CGLootWindow::GetLootItemRarity(uint32_t slot) {
    if (!CGLootInfo::m_lootObj) {
        return 0;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return 0;
        }

        slot--;
    }

    if (slot >= 24) {
        return 0;
    }

    int32_t itemID = CGLootInfo::m_slots[slot].m_itemID;

    if (itemID) {
        WOWGUID requester = WOWGUID::Make(0);
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemID, requester, NULL, NULL, false);
        return rec ? rec->m_quality : -1;
    }

    return 1;
}

// OFFSET: 0x0095AB00/0x1406CD3A0
char* CGLootInfo::GetLootSlotLink(uint32_t slot, char* buffer, uint32_t size) {
    if (!CGLootInfo::m_lootObj) {
        return NULL;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return NULL;
        }

        slot--;
    }

    if (slot >= 24) {
        return NULL;
    }

    ::LootSlot& entry = CGLootInfo::m_slots[slot];

    if (!entry.m_itemID) {
        return NULL;
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(entry.m_itemID, CGLootInfo::m_lootObj, NULL, NULL, false);

    if (!rec) {
        return NULL;
    }

    const char* link = CGTooltip::GetItemHyperlinkString(entry.m_itemID, rec->m_quality, 0, entry.m_randomPropertiesID, entry.m_randomSuffix, 0);
    SStrCopy(buffer, link, size);
    return buffer;
}

// OFFSET: 0x0095ABC0/0x1406CD4B0
void CGLootInfo::LootSlot(uint32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            if (CGLootInfo::m_coins > 0) {
                player->LootMoney();
                CGLootInfo::m_coins = -1;
                FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CLEARED, "%d", 1);

                if (!CGLootInfo::HasLoot()) {
                    CGGameUI::CloseLoot(true, false, false);
                }
            }

            return;
        }

        slot--;
    }

    if (slot >= 24) {
        return;
    }

    ::LootSlot& entry = CGLootInfo::m_slots[slot];

    if (entry.m_itemID) {
        int32_t type = entry.m_slotType;

        switch (type) {
            case 1:
                CGGameUI::DisplayError(ERR_LOOT_ROLL_PENDING);
                return;
            case 3:
                CGGameUI::DisplayError(ERR_LOOT_CANT_LOOT_THAT);
                return;
            case 2:
                FrameScript_SignalEvent(SCRIPT_EVENT_OPEN_MASTER_LOOT_LIST, NULL);
                return;
        }

        ItemRecSparse* rec = g_itemSparseDB.GetRecord(entry.m_itemID, CGLootInfo::m_lootObj, NULL, NULL, false);

        if (rec) {
            if (ItemRec* item = rec->GetNonSparse()) {
                if (rec->m_bonding != 1 || rec->m_quality < 2 || (rec->m_flags[0] & 0x800) || type == 4) {
                    SI3::PlayItemSound(0, item->m_displayInfoID);
                    CGPlayer_C::AutoStoreLootItem(entry.m_lootListID);
                } else {
                    if (!player->GetBag()->ItemCountValid(entry.m_itemID, rec, entry.m_quantity, true)) {
                        return;
                    }

                    if (CGLootInfo::m_coins) {
                        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_BIND_CONFIRM, "%u", slot + 2);
                    } else {
                        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_BIND_CONFIRM, "%u", slot + 1);
                    }

                    s_bindConfirmSlot = slot;
                }
            }
        }
    }

    if (entry.m_currencyID) {
        PlayerLootCurrency loot;
        loot.m_lootListID = entry.m_lootListID;
        ClientServices::Send(&loot);
    }
}

// OFFSET: 0x0095ADC0/0x1406CD710
void CGLootInfo::ConfirmLootSlot(uint32_t slot) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return;
    }

    if (CGLootInfo::m_coins) {
        if (!slot) {
            return;
        }

        slot--;
    }

    if (slot != static_cast<uint32_t>(s_bindConfirmSlot)) {
        return;
    }

    ::LootSlot& entry = CGLootInfo::m_slots[slot];

    if (!entry.m_itemID) {
        return;
    }

    ItemRec* item = g_itemDB.GetRecord(entry.m_itemID, CGLootInfo::m_lootObj, NULL, NULL, false);
    SI3::PlayItemSound(0, item->m_displayInfoID);
    CGPlayer_C::AutoStoreLootItem(entry.m_lootListID);
    s_bindConfirmSlot = -1;
}

// OFFSET: 0x0095AE60
void CGLootInfo::LootButtonItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    for (uint32_t i = 0; i < 24; i++) {
        ::LootSlot& slot = CGLootInfo::m_slots[i];
        uint32_t pendingEvent = slot.m_pendingEvent;

        if (pendingEvent && slot.m_itemID == id) {
            if (pendingEvent == SCRIPT_EVENT_LOOT_SLOT_CHANGED) {
                FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CHANGED, "%d", (CGLootInfo::m_coins != 0) + i + 1);
            }

            slot.m_pendingEvent = 0;
        }
    }

    if (!--CGLootInfo::m_itemsPending) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->m_autoLoot) {
            CGLootInfo::DoAutoLoot(0);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_OPENED, "%d", 0);
        }
    }
}

// OFFSET: 0x0095B8C0
bool CGLootInfo::OnLootContents(ClientLootContents* msg) {
    if (CGLootInfo::m_missingItemsPending) {
        for (uint32_t i = 0; i < CGLootInfo::m_missingItems.m_count; i++) {
            if (CGLootInfo::m_missingItems.m_data[i].m_pending) {
                g_itemSparseDB.CancelCallback(CGLootInfo::m_missingItems.m_data[i].m_itemId, MissingLootItemCallback, NULL);
            }
        }
    }

    CGLootInfo::m_missingItems.m_count = 0;
    CGLootInfo::m_missingItemsPending = 0;

    for (uint32_t i = 0; i < msg->m_items.m_count; i++) {
        JamLootItem& source = msg->m_items.m_data[i];
        MissingLootItem* item = CGLootInfo::m_missingItems.New();
        item->m_itemId = source.m_itemID;
        item->m_displayId = source.m_displayID;
        item->m_quantity = source.m_quantity;
        item->m_seed = source.m_randomSuffix;
        item->m_randomPropertiesId = source.m_randomPropertiesID;
        WOWGUID requester = WOWGUID::Make(0);

        if (g_itemSparseDB.GetRecord(source.m_itemID, requester, MissingLootItemCallback, NULL, false)) {
            item->m_pending = 0;
        } else {
            item->m_pending = 1;
            CGLootInfo::m_missingItemsPending++;
        }
    }

    if (!CGLootInfo::m_missingItemsPending) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MISSING_OUT_ON_LOOT, NULL);
    }

    return true;
}

// OFFSET: 0x0095BA40/0x1406CD090
void CGLootInfo::SetObject(CGObject_C* object, CDataStore* msg, uint32_t lootType) {
    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        CGGameUI::Target(object->m_obj->m_guid);
    }

    if (CGLootInfo::m_lootObj) {
        if (CGObject_C* previous = ClntObjMgrObjectPtr(CGLootInfo::m_lootObj, TYPEMASK_OBJECT, __FILE__, __LINE__)) {
            uint16_t type = previous->m_obj->m_type;

            if (previous->m_obj->m_type & TYPEMASK_UNIT) {
                if (!CGLootInfo::HasLoot()) {
                    static_cast<CGUnit_C*>(previous)->RemoveLootEffect();
                }
            } else if (type & TYPEMASK_GAMEOBJECT) {
                static_cast<CGGameObject_C*>(previous)->SetDisplayState(1);
            } else if (type & TYPEMASK_ITEM) {
                CGGameUI::UnlockItem(CGLootInfo::m_lootObj);
            }
        }

        for (uint32_t i = 0; i < 24; i++) {
            if (CGLootInfo::m_slots[i].m_pendingEvent) {
                g_itemSparseDB.CancelCallback(CGLootInfo::m_slots[i].m_itemID, CGLootInfo::LootButtonItemStatsCallback, NULL);
            }
        }

        uint64_t sameObject = object && object->m_obj->m_guid.m_guid;

        if (CGLootInfo::m_lootObj.m_guid != sameObject) {
            CGLootInfo::m_autolooted = 0;
        }

        CGLootInfo::m_lootObj.m_guid = 0;
        memset(CGLootInfo::m_slots, 0, sizeof(CGLootInfo::m_slots));
        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_CLOSED, NULL);
    }

    if (!object) {
        return;
    }

    CGLootInfo::m_lootObj = object->m_obj->m_guid;
    CGLootInfo::m_itemsPending = 0;
    CGLootInfo::m_lootType = lootType;
    s_bindConfirmSlot = -1;
    msg->Get(reinterpret_cast<uint32_t&>(CGLootInfo::m_coins));
    uint8_t itemCount;
    msg->Get(itemCount);
    uint8_t currencyCount;
    msg->Get(currencyCount);

    if (itemCount > 18) {
        itemCount = 18;
    }

    if (currencyCount > 6) {
        currencyCount = 6;
    }

    uint32_t index = 0;
    memset(CGLootInfo::m_slots, 0, sizeof(CGLootInfo::m_slots));

    for (uint32_t i = 0; i < itemCount; i++) {
        ::LootSlot& slot = CGLootInfo::m_slots[i];
        msg->Get(slot.m_lootListID);
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_itemID));
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_quantity));
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_displayID));
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_randomSuffix));
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_randomPropertiesID));
        uint8_t slotType;
        msg->Get(slotType);
        slot.m_slotType = slotType;
        slot.m_currencyID = 0;

        if (g_itemSparseDB.GetRecord(slot.m_itemID, CGLootInfo::m_lootObj, CGLootInfo::LootButtonItemStatsCallback, NULL, false)) {
            slot.m_pendingEvent = 0;
        } else {
            CGLootInfo::m_itemsPending++;
            slot.m_pendingEvent = 1;
        }

        index = i + 1;
    }

    for (uint32_t i = 0; i < currencyCount; i++) {
        ::LootSlot& slot = CGLootInfo::m_slots[index + i];
        msg->Get(slot.m_lootListID);
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_currencyID));
        msg->Get(reinterpret_cast<uint32_t&>(slot.m_quantity));
    }

    if (!CGLootInfo::m_itemsPending) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->m_autoLoot) {
            CGLootInfo::DoAutoLoot(0);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_OPENED, "%d", 0);
        }
    }
}

// OFFSET: 0x0095BD50
void CGLootInfo::ClearSlot(uint8_t slot) {
    uint32_t index;

    for (index = 0; index < 24; index++) {
        ::LootSlot& entry = CGLootInfo::m_slots[index];

        if (entry.m_itemID && !entry.m_currencyID && slot == entry.m_lootListID) {
            break;
        }
    }

    if (index >= 24) {
        return;
    }

    if (CGLootInfo::m_slots[index].m_pendingEvent) {
        g_itemSparseDB.CancelCallback(CGLootInfo::m_slots[index].m_itemID, CGLootInfo::LootButtonItemStatsCallback, NULL);
    }

    memset(&CGLootInfo::m_slots[index], 0, sizeof(CGLootInfo::m_slots[index]));

    if (CGLootInfo::m_coins) {
        index++;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CLEARED, "%d", index + 1);

    if (!CGLootInfo::HasLoot()) {
        CGGameUI::CloseLoot(true, false, false);
    }
}

// OFFSET: 0x0095BE30
void CGLootInfo::ChangedSlot(uint8_t slot, CDataStore* msg) {
    uint32_t index = static_cast<uint32_t>(-1);

    for (uint32_t i = 0; i < 24; i++) {
        ::LootSlot& entry = CGLootInfo::m_slots[i];

        if (entry.m_itemID && !entry.m_currencyID && slot == entry.m_lootListID) {
            index = i;
            break;
        }
    }

    if (index >= 24) {
        for (uint32_t i = 0; i < 24; i++) {
            if (!CGLootInfo::m_slots[i].m_itemID && !CGLootInfo::m_slots[i].m_currencyID) {
                index = i;
                break;
            }
        }

        if (index >= 24) {
            return;
        }
    }

    ::LootSlot& entry = CGLootInfo::m_slots[index];
    msg->Get(reinterpret_cast<uint32_t&>(entry.m_itemID));
    msg->Get(reinterpret_cast<uint32_t&>(entry.m_displayID));
    msg->Get(reinterpret_cast<uint32_t&>(entry.m_randomSuffix));
    msg->Get(reinterpret_cast<uint32_t&>(entry.m_randomPropertiesID));
    msg->Get(reinterpret_cast<uint32_t&>(entry.m_quantity));
    entry.m_lootListID = slot;

    if (g_itemSparseDB.GetRecord(entry.m_itemID, CGLootInfo::m_lootObj, CGLootInfo::LootButtonItemStatsCallback, NULL, false)) {
        entry.m_pendingEvent = 0;
    } else {
        CGLootInfo::m_itemsPending++;
        entry.m_pendingEvent = SCRIPT_EVENT_LOOT_SLOT_CHANGED;
    }

    if (CGLootInfo::m_coins) {
        index++;
    }

    if (!CGLootInfo::m_itemsPending) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CHANGED, "%d", index + 1);
    }
}

// OFFSET: 0x0095BF60/0x1406CECA0
void CGLootInfo::InitializeGame() {
    CGLootInfo::m_lootObj = WOWGUID();
    ClientCurrencyLootRemoved::s_handler = CGLootInfo::OnCurrencyLootRemoved;
    ClientCurrencyLootRestored::s_handler = CGLootInfo::OnCurrencyLootRestored;
    ClientLootContents::s_handler = CGLootInfo::OnLootContents;
}

// INLINED
void CGPlayer_C::OnLootClearMoney() {
    if (CGLootInfo::m_coins > 0) {
        CGLootInfo::m_coins = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_SLOT_CLEARED, "%d", 1);

        if (!CGLootInfo::HasLoot()) {
            CGGameUI::CloseLoot(true, false, false);
        }
    }
}
