#include "GuildBankFrame.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameScript/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C07014
static const uint32_t s_guildBankTabCosts[6] = { 1000000, 2500000, 5000000, 10000000, 25000000, 50000000 };

// OFFSET: 0x009C03A0/0x14074CC80
static int32_t Script_PickupGuildBankMoney(lua_State* L) {
    uint64_t amount;

    if (!lua_isnumber(L, 1) || !Script_GetMoney(L, 1, &amount)) {
        luaL_error(L, "Usage: PickupGuildBankMoney(amount)");
        return 0;
    }

    if (amount && amount <= CGGuildBankInfo::m_money && amount < 10000000000ull) {
        CGGameUI::SetCursorMoney(amount, 11, SCRIPT_EVENT_GUILDBANK_UPDATE_MONEY);
    }

    return 0;
}

// OFFSET: 0x009C0440/0x14074CD30
static int32_t Script_SetCurrentGuildBankTab(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetCurrentTab(tab)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t tab = tabValue - 1;

    if (tab < 8) {
        CGGuildBankInfo::m_currentTab = static_cast<uint8_t>(tab);
    }

    return 0;
}

// OFFSET: 0x009C0490/0x14074CD90
static int32_t Script_GetCurrentGuildBankTab(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_currentTab + 1);
    return 1;
}

// PROVEN: f227cc47 IDENTICAL/NONE
// OFFSET: 0x009C04C0/0x14074CDC0
static int32_t Script_GetGuildBankMoney(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGuildBankInfo::m_money));
    return 1;
}

// PROVEN: d404ab9d IDENTICAL/NONE
// OFFSET: 0x009C0520/0x14074CDF0
static int32_t Script_GetGuildBankBonusDepositMoney(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGuildBankInfo::m_bonusDepositMoney));
    return 1;
}

// PROVEN: fcc655c5 IDENTICAL/NONE
// OFFSET: 0x009C0580/0x14074CE20
static int32_t Script_GetGuildBankWithdrawMoney(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGuildBankInfo::m_remainingWithdrawMoney));
    return 1;
}

// PROVEN: 5f8cb174 IDENTICAL/NONE
// OFFSET: 0x009C05E0/0x14074CE50
static int32_t Script_GetNumGuildBankTabs(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_numTabs);
    return 1;
}

// PROVEN: 66e896d8 IDENTICAL/NONE
// OFFSET: 0x009C0610/0x14074CE70
static int32_t Script_CloseGuildBankFrame(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    CGGuildBankInfo::SetGuildBanker(none);
    return 0;
}

// OFFSET: 0x009C0630/0x14074CEC0
static int32_t Script_QueryGuildBankLog(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: QueryGuildBankLog(tab)");
        return 0;
    }

    double tab = lua_tonumber(L, 1);

    if (CGGuildBankInfo::m_banker) {
        GlobalGuildBankLogQuery query(static_cast<int32_t>(tab) - 1);
        ClientServices::Send(&query);
    }

    return 0;
}

// OFFSET: 0x009C06B0/0x14074CF30
static int32_t Script_GetNumGuildBankTransactions(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNumGuildBankTransactions(tab)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t tab = tabValue - 1;
    lua_pushnumber(L, tab >= 9 ? 0 : CGGuildBankInfo::m_numLogEntries[tab]);
    return 1;
}

// PROVEN: c04cb8c6 IDENTICAL/NONE
// OFFSET: 0x009C0730/0x14074CFA0
static int32_t Script_GetNumGuildBankMoneyTransactions(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_numLogEntries[8]);
    return 1;
}

// OFFSET: 0x009C0760/0x14074CFC0
static int32_t Script_GetGuildBankText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildBankText(tab)");
        return 0;
    }

    uint32_t tab = static_cast<uint8_t>(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    GuildBankTabInfo* info;

    if (tab >= 8 || static_cast<int32_t>(tab) >= CGGuildBankInfo::m_numTabs || !(info = &CGGuildBankInfo::m_tabInfo[tab]) || !info->m_text) {
        lua_pushnil(L);
        return 1;
    }

    char text[8192];
    SStrCopy(text, info->m_text, sizeof(text));
    lua_pushstring(L, text);
    return 1;
}

// OFFSET: 0x009C1230/0x14074DC80
static int32_t Script_QueryGuildBankTab(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: QueryGuildBankTab(tab)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t tab = tabValue - 1;

    if (tab < 8) {
        CGGuildBankInfo::QueryGuildBankTab(static_cast<uint8_t>(tab));
    }

    return 0;
}

// OFFSET: 0x009C1280/0x14074DCE0
static int32_t Script_WithdrawGuildBankMoney(lua_State* L) {
    uint64_t amount;

    if (!lua_isnumber(L, 1) || !Script_GetMoney(L, 1, &amount)) {
        luaL_error(L, "Usage: WithdrawGuildBankMoney(money)");
        return 0;
    }

    if (amount > 9999999999ull) {
        CGGameUI::DisplayError(ERR_TOO_MUCH_GOLD);
        return 0;
    }

    if (amount) {
        CGGuildBankInfo::WithdrawMoney(amount);
    }

    return 0;
}

// OFFSET: 0x009C1310/0x14074DD70
static int32_t Script_GetGuildBankMoneyTransaction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildBankMoneyTransaction(index)");
        return 0;
    }

    int32_t indexValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint8_t index = indexValue - 1;
    GuildBankLogEntry* entry = index >= 25 ? NULL : &CGGuildBankInfo::m_log[8][index];

    switch (entry->m_type) {
        case 4: {
            lua_pushstring(L, "deposit");
            break;
        }

        case 5: {
            lua_pushstring(L, "withdraw");
            break;
        }

        case 8: {
            lua_pushstring(L, "withdrawForTab");
            break;
        }

        case 9: {
            lua_pushstring(L, "buyTab");
            break;
        }

        case 6: {
            lua_pushstring(L, "repair");
            break;
        }

        case 10: {
            lua_pushstring(L, "depositSummary");
            break;
        }

        default: {
            return 0;
        }
    }

    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(entry->m_playerGUID, requester, NULL, NULL, false);

    if (name) {
        lua_pushstring(L, name->m_name);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<double>(entry->m_money));

    int32_t elapsed = _time32(NULL) - entry->m_time;
    int32_t years = elapsed / 3600 / 8760;
    int32_t hours = elapsed / 3600 % 8760;
    int32_t months = hours / 720;
    hours %= 720;
    int32_t days = hours / 24;
    lua_pushnumber(L, years);
    lua_pushnumber(L, months);
    lua_pushnumber(L, days);
    lua_pushnumber(L, hours % 24);
    return 7;
}

// OFFSET: 0x009C1520/0x14074DFC0
static int32_t Script_QueryGuildBankText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: QueryGuildBankLog(tab)");
        return 0;
    }

    CGGuildBankInfo::QueryGuildBankText(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x009C1570/0x14074E010
static int32_t Script_SetGuildBankText(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetGuildBankText(tab, text)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint8_t tab = tabValue - 1;
    const char* text = lua_tolstring(L, 2, NULL);
    char scrubbed[8192];
    scrubbed[0] = 0;

    if (text && *text) {
        ScrubString(text, scrubbed, 2048, sizeof(scrubbed), 0);
    }

    CGGuildBankInfo::SetGuildBankText(tab, scrubbed);
    return 0;
}

// OFFSET: 0x009C1640/0x14074E0E0
static int32_t Script_BuyGuildBankTab(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !CGGameUI::m_hasControl) {
        return 0;
    }

    int32_t tab = CGGuildBankInfo::m_numTabs;
    uint64_t money = CGGuildBankInfo::m_money;

    if (money + player->GetCoinage() < s_guildBankTabCosts[tab]) {
        CGGameUI::DisplayError(ERR_BANKSLOT_INSUFFICIENT_FUNDS);
        return 0;
    }

    CGGuildBankInfo::BuyGuildBankTab();
    return 0;
}

// OFFSET: 0x009C16C0/0x14074E190
static int32_t Script_DepositGuildBankMoney(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DepositGuildBankMoney(money)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint64_t amount;

    if (!Script_GetMoney(L, 1, &amount)) {
        luaL_error(L, "Usage: DepositGuildBankMoney(money)");
        return 0;
    }

    if (player && amount) {
        if (player->GetCoinage() >= amount && amount < 10000000000ull) {
            CGGuildBankInfo::DepositMoney(amount);
            return 0;
        }

        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
    }

    return 0;
}

// OFFSET: 0x009C17A0/0x14074E280
static int32_t Script_CanWithdrawGuildBankMoney(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t rights = CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank);

    if (CGGuildInfo::GetWithdrawGoldLimitByRankID(player->m_player->m_playerGuildrank) && (rights & 0x80000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009C1850/0x14074E360
static int32_t Script_CanEditGuildBankTabInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x400000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009C18E0/0x14074E3F0
static int32_t Script_AutoStoreGuildBankItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: AutoStoreGuildBankItem(tab, slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    WOWGUID banker = CGGuildBankInfo::m_banker;

    if (!banker) {
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint8_t tab = tabValue - 1;
    int32_t slotValue = static_cast<int32_t>(lua_tonumber(L, 2));
    uint8_t slot = slotValue - 1;

    if (tab >= 8 || slot >= 98) {
        return 0;
    }

    CGGameUI::ClearCursor(1, 1);

    GuildBankItem* item = CGGuildBankInfo::GetGuildBankItem(tab, slot);

    if (!item) {
        return 0;
    }

    if (Spell_C_IsTargeting()) {
        Spell_C_StopTargeting();
    }

    if (!CGGuildInfo::GetNumGuildBankTabItemWithdrawalsByRankID(tab, player->m_player->m_playerGuildrank)) {
        CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
        return 0;
    }

    CGGameUI::m_autoStoreItem = WOWGUID::Make(0);
    CGGameUI::m_autoStoreItemID = item->m_itemID;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_SWAP_ITEMS));
    msg.Put(banker.m_guid);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(tab);
    msg.Put(slot);
    msg.Put(static_cast<uint32_t>(item->m_itemID));
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(static_cast<uint32_t>(item->m_count));
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(static_cast<uint32_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, NULL);
    return 0;
}

// PROVEN: cacfab8a IDENTICAL/NONE
// OFFSET: 0x009C1AE0/0x14074E650
static int32_t Script_SetGuildBankTabInfo(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
        luaL_error(L, "Usage: SetGuildBankTabInfo(tab, name, iconFileName)");
        return 0;
    }

    int32_t tab = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    const char* name = lua_tolstring(L, 2, NULL);
    const char* icon = lua_tolstring(L, 3, NULL);

    if (icon && *icon) {
        CGGuildBankInfo::SetTabInfo(tab, name, icon);
    }

    return 0;
}

// PROVEN: d55a8d1b IDENTICAL/NONE
// OFFSET: 0x009C1B90/0x14074E740
static int32_t Script_GetGuildBankTabCost(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    uint32_t cost;

    if (static_cast<uint32_t>(CGGuildBankInfo::m_numTabs) < 8 && (cost = s_guildBankTabCosts[CGGuildBankInfo::m_numTabs])) {
        lua_pushnumber(L, cost);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009C1C10/0x14074E7C0
static int32_t Script_GetGuildTabardFileNames(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;
    GuildGetGuildTabard(player->m_obj->m_guid, player->GetGuildGUID(), NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background);

    if (emblemStyle == -1 || borderStyle == -1 || background == -1) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 6;
    }

    char file[260];
    CCharacterComponent::GetTabardBackgroundFileName(1, SECTION_TORSO_UPPER, background, file, sizeof(file));
    lua_pushstring(L, file);
    CCharacterComponent::GetTabardBackgroundFileName(1, SECTION_TORSO_LOWER, background, file, sizeof(file));
    lua_pushstring(L, file);
    CCharacterComponent::GetTabardEmblemFileName(SECTION_TORSO_UPPER, emblemStyle, emblemColor, file, sizeof(file));
    lua_pushstring(L, file);
    CCharacterComponent::GetTabardEmblemFileName(SECTION_TORSO_LOWER, emblemStyle, emblemColor, file, sizeof(file));
    lua_pushstring(L, file);
    CCharacterComponent::GetTabardBorderFileName(1, SECTION_TORSO_UPPER, borderStyle, borderColor, file, sizeof(file));
    lua_pushstring(L, file);
    CCharacterComponent::GetTabardBorderFileName(1, SECTION_TORSO_LOWER, borderStyle, borderColor, file, sizeof(file));
    lua_pushstring(L, file);
    return 6;
}

// OFFSET: 0x009C1DE0/0x14074EA00
static int32_t Script_GetGuildLogoInfo(lua_State* L) {
    CGUnit_C* unit;

    if (lua_isstring(L, 1)) {
        unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

        if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return 0;
        }
    } else {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    }

    if (!unit) {
        return 0;
    }

    CImVector background(0);
    CImVector border(0);
    CImVector emblem(0);
    char emblemFile[260];

    if (!CGObject_C::GetGuildLogoInfo(unit->GetGuildGUID().m_guid, background, border, emblem, emblemFile, sizeof(emblemFile))) {
        return 0;
    }

    lua_pushnumber(L, background.r);
    lua_pushnumber(L, background.g);
    lua_pushnumber(L, background.b);
    lua_pushnumber(L, border.r);
    lua_pushnumber(L, border.g);
    lua_pushnumber(L, border.b);
    lua_pushnumber(L, emblem.r);
    lua_pushnumber(L, emblem.g);
    lua_pushnumber(L, emblem.b);
    lua_pushstring(L, emblemFile);
    return 10;
}

// OFFSET: 0x009C2AE0/0x14074FA10
static int32_t Script_GetGuildBankItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetGuildBankItemInfo(tab, slot)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t tab = tabValue - 1;
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    GuildBankItem* item;
    ItemRec* rec;

    if (tab < 8 && slot < CGGuildBankInfo::m_tabSlots[tab].Count() && (item = &CGGuildBankInfo::m_tabSlots[tab][slot])->m_itemID && item && ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        WOWGUID requester = WOWGUID::Make(0);

        if ((rec = g_itemDB.GetRecord(item->m_itemID, requester, NULL, NULL, false))) {
            const char* path = ClientDBStringLookup(3);
            char texture[260];
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
            lua_pushstring(L, texture);
            lua_pushnumber(L, item->m_count);

            if (CGGameUI::VirtualItemIsLocked(item->m_itemID, slot + 98 * tab, 0)) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnil(L);
            }

            lua_pushboolean(L, ItemRecIsFiltered(rec, item->m_randomPropertiesID));
            return 4;
        }
    }

    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushboolean(L, 0);
    return 4;
}

// OFFSET: 0x009C2CE0/0x14074FC70
static int32_t Script_GetGuildBankItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetGuildBankItemLink(tab, slot)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t tab = tabValue - 1;
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (tab >= 8 || slot >= CGGuildBankInfo::m_tabSlots[tab].Count()) {
        return 0;
    }

    GuildBankItem* item = &CGGuildBankInfo::m_tabSlots[tab][slot];

    if (!item->m_itemID || !item) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(item->m_itemID, requester, NULL, NULL, false);

    if (!sparse) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item->m_itemID, sparse->m_quality, item->m_enchantmentID, item->m_socketEnchants, item->m_randomPropertiesID, item->m_randomPropertiesSeed, item->m_reforgeID));
    return 1;
}

// OFFSET: 0x009C2DF0/0x14074FD90
static int32_t Script_PickupGuildBankItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: PickupGuildBankItem(tab, slot)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint8_t tab = tabValue - 1;
    int32_t slotValue = static_cast<int32_t>(lua_tonumber(L, 2));
    uint8_t slot = slotValue - 1;

    if (tab >= 8 || slot >= 98) {
        return 0;
    }

    int32_t virtualID = 0;
    int32_t virtualSlot = 0;
    WOWGUID item;
    WOWGUID container;
    uint32_t containerSlot;
    CGGameUI::GetCursorItem(item, container, containerSlot);

    if (CGGameUI::m_cursorItemType == 10) {
        CGGameUI::GetCursorVirtualItem(virtualID, virtualSlot);
    }

    if (item) {
        CGGuildBankInfo::SwapItems(tab, slot, item.m_guid, container.m_guid, containerSlot, static_cast<uint8_t>(CGGameUI::m_stackSplit));
        return 0;
    }

    if (virtualID) {
        CGGuildBankInfo::SwapBankItems(tab, slot, virtualID, virtualSlot, CGGameUI::m_stackSplit);
        return 0;
    }

    GuildBankItem* bankItem = CGGuildBankInfo::GetGuildBankItem(tab, slot);

    if (!bankItem) {
        return 0;
    }

    if (Spell_C_IsTargeting()) {
        Spell_C_StopTargeting();
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRec* rec = g_itemDB.GetRecord(bankItem->m_itemID, requester, NULL, NULL, false);

    if (rec) {
        uint32_t bankSlot = slot + 98 * tab;

        if (!CGGameUI::VirtualItemIsLocked(bankItem->m_itemID, bankSlot, 0)) {
            CGGameUI::SetCursorVirtualItem(bankItem->m_itemID, rec->m_displayInfoID, bankSlot, 10, 0);
            CGGameUI::LockVirtualItem(bankItem->m_itemID, bankSlot, 0);
        }
    }

    return 0;
}

// OFFSET: 0x009C2FF0/0x14074FFD0
static int32_t Script_SplitGuildBankItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: SplitGuildBankItem(tab, slot, amount)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (!CGGameUI::m_hasControl && !(player->m_unit->m_unitFieldFlags & 0x100000))) {
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint8_t tab = tabValue - 1;
    int32_t slotValue = static_cast<int32_t>(lua_tonumber(L, 2));
    uint8_t slot = slotValue - 1;

    if (tab >= 8 || slot >= 98) {
        return 0;
    }

    int32_t virtualID = 0;
    int32_t virtualSlot = 0;
    uint32_t bankSlot = slot + 98 * tab;

    if (CGGameUI::m_cursorItemType == 10) {
        CGGameUI::GetCursorVirtualItem(virtualID, virtualSlot);
    }

    GuildBankItem* item = CGGuildBankInfo::GetGuildBankItem(tab, slot);

    if (!item || item->m_locked) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(item->m_itemID, requester, NULL, NULL, false);

    if (!sparse) {
        return 0;
    }

    ItemRec* rec = sparse->GetNonSparse();

    if (!rec) {
        return 0;
    }

    int32_t amount = static_cast<int32_t>(lua_tonumber(L, 3));

    if (amount < 1 || amount > sparse->m_stackable) {
        return 0;
    }

    CGGameUI::ClearCursor(1, 1);
    Spell_C_StopTargeting();

    if (amount == sparse->m_stackable) {
        CGGameUI::SetCursorVirtualItem(item->m_itemID, rec->m_displayInfoID, bankSlot, 10, 0);
        CGGameUI::LockVirtualItem(item->m_itemID, bankSlot, 0);
    }

    CGGameUI::SetCursorVirtualItem(item->m_itemID, rec->m_displayInfoID, bankSlot, 10, amount);
    CGGameUI::LockVirtualItem(item->m_itemID, bankSlot, 0);
    return 0;
}

// OFFSET: 0x009C3200/0x140750210
static int32_t Script_GetGuildBankTabInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildBankTabInfo(tab)");
        return 0;
    }

    int32_t tabByteValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint8_t tabByte = tabByteValue - 1;
    uint32_t tab = tabByte;
    GuildBankTabInfo* info;

    if (tab >= 8 || tabByte >= CGGuildBankInfo::m_numTabs || !(info = &CGGuildBankInfo::m_tabInfo[tabByte])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushboolean(L, 0);
        return 7;
    }

    char name[64];

    if (info->m_name[0]) {
        CVar* filter = CVar::Lookup("profanityFilter");

        if (!filter || filter->m_intValue) {
            SStrCopy(name, info->m_name, sizeof(name));
            ProfanityFilter(name, false, false);
            lua_pushstring(L, name);
        } else {
            lua_pushstring(L, info->m_name);
        }
    } else {
        SStrPrintf(name, sizeof(name), FrameScript_GetText("GUILDBANK_TAB_NUMBER", -1, GENDER_NOT_APPLICABLE), tab + 1);
        lua_pushstring(L, name);
    }

    char* icon = info->m_icon;

    if (!strlen(icon)) {
        SStrCopy(icon, "INV_Misc_QuestionMark", 256);
    }

    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", icon);
    lua_pushstring(L, texture);

    uint32_t rank = player->m_player->m_playerGuildrank;
    uint8_t rights = CGGuildInfo::GetGuildBankTabPermissionsByRankID(tab, rank);

    if (rights & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (rights & 0x2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, CGGuildInfo::GetNumGuildBankTabItemWithdrawalsByRankID(tab, rank));
    lua_pushnumber(L, CGGuildBankInfo::m_remainingWithdrawals);
    lua_pushboolean(L, CGGuildBankInfo::IsTabFiltered(tab));
    return 7;
}

// OFFSET: 0x009C34C0/0x140750530
static int32_t Script_GetGuildBankTransaction(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetGuildBankTransaction(tab, index)");
        return 0;
    }

    int32_t tabValue = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t tab = tabValue - 1;
    int32_t indexValue = static_cast<int32_t>(lua_tonumber(L, 2));
    uint8_t index = indexValue - 1;
    GuildBankLogEntry* entry = tab >= 9 || index >= 25 ? NULL : &CGGuildBankInfo::m_log[tab][index];

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    char type[16];

    switch (entry->m_type) {
        case 1: {
            SStrCopy(type, "deposit", sizeof(type));
            break;
        }

        case 2: {
            SStrCopy(type, "withdraw", sizeof(type));
            break;
        }

        case 3:
        case 7: {
            SStrCopy(type, "move", sizeof(type));
            break;
        }

        default: {
            SStrCopy(type, "unknown", sizeof(type));
            break;
        }
    }

    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(entry->m_playerGUID, requester, NULL, NULL, false);
    WOWGUID itemRequester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(entry->m_itemID, itemRequester, NULL, NULL, false);
    lua_pushstring(L, type);

    if (name) {
        lua_pushstring(L, name->m_name);
    } else {
        lua_pushnil(L);
    }

    if (sparse) {
        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(entry->m_itemID, sparse->m_quality, 0, 0, 0, 0));
    } else {
        lua_pushstring(L, "");
    }

    lua_pushnumber(L, entry->m_count);

    if (entry->m_type == 3) {
        lua_pushnumber(L, static_cast<int32_t>(tab + 1));
        lua_pushnumber(L, entry->m_otherTab + 1);
    } else if (entry->m_type == 7) {
        lua_pushnumber(L, entry->m_otherTab + 1);
        lua_pushnumber(L, static_cast<int32_t>(tab + 1));
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    int32_t elapsed = _time32(NULL) - entry->m_time;
    int32_t years = elapsed / 3600 / 8760;
    int32_t hours = elapsed / 3600 % 8760;
    int32_t months = hours / 720;
    hours %= 720;
    int32_t days = hours / 24;
    lua_pushnumber(L, years);
    lua_pushnumber(L, months);
    lua_pushnumber(L, days);
    lua_pushnumber(L, hours % 24);
    return 10;
}

// STATIC: 0x00D15C70
static FrameScript_Method s_ScriptFunctions[] = {
    { "QueryGuildBankTab", &Script_QueryGuildBankTab },
    { "SetCurrentGuildBankTab", &Script_SetCurrentGuildBankTab },
    { "GetCurrentGuildBankTab", &Script_GetCurrentGuildBankTab },
    { "GetGuildBankItemInfo", &Script_GetGuildBankItemInfo },
    { "SetGuildBankTabInfo", &Script_SetGuildBankTabInfo },
    { "GetGuildBankItemLink", &Script_GetGuildBankItemLink },
    { "PickupGuildBankItem", &Script_PickupGuildBankItem },
    { "AutoStoreGuildBankItem", &Script_AutoStoreGuildBankItem },
    { "SplitGuildBankItem", &Script_SplitGuildBankItem },
    { "GetNumGuildBankTabs", &Script_GetNumGuildBankTabs },
    { "GetGuildBankTabInfo", &Script_GetGuildBankTabInfo },
    { "GetGuildBankTabCost", &Script_GetGuildBankTabCost },
    { "BuyGuildBankTab", &Script_BuyGuildBankTab },
    { "DepositGuildBankMoney", &Script_DepositGuildBankMoney },
    { "WithdrawGuildBankMoney", &Script_WithdrawGuildBankMoney },
    { "CanWithdrawGuildBankMoney", &Script_CanWithdrawGuildBankMoney },
    { "PickupGuildBankMoney", &Script_PickupGuildBankMoney },
    { "GetGuildBankMoney", &Script_GetGuildBankMoney },
    { "GetGuildBankBonusDepositMoney", &Script_GetGuildBankBonusDepositMoney },
    { "GetGuildBankWithdrawMoney", &Script_GetGuildBankWithdrawMoney },
    { "CloseGuildBankFrame", &Script_CloseGuildBankFrame },
    { "GetGuildTabardFileNames", &Script_GetGuildTabardFileNames },
    { "GetGuildLogoInfo", &Script_GetGuildLogoInfo },
    { "QueryGuildBankLog", &Script_QueryGuildBankLog },
    { "GetNumGuildBankTransactions", &Script_GetNumGuildBankTransactions },
    { "GetGuildBankTransaction", &Script_GetGuildBankTransaction },
    { "GetNumGuildBankMoneyTransactions", &Script_GetNumGuildBankMoneyTransactions },
    { "GetGuildBankMoneyTransaction", &Script_GetGuildBankMoneyTransaction },
    { "QueryGuildBankText", &Script_QueryGuildBankText },
    { "GetGuildBankText", &Script_GetGuildBankText },
    { "SetGuildBankText", &Script_SetGuildBankText },
    { "CanEditGuildBankTabInfo", &Script_CanEditGuildBankTabInfo },
};

// PROVEN: 5f44d6fc IDENTICAL/NONE
// OFFSET: 0x009C0840
void GuildBankRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: ff128081 IDENTICAL/NONE
// OFFSET: 0x009C0870
void GuildBankUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
