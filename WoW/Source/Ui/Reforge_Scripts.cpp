#include "WoW/Source/Ui/Reforge.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"
#include "WoW/Source/DB/Rec/ItemReforgeRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScalingStats.h"

// STATIC: 0x00C016C0
static const int32_t s_reforgeStatOrder[33] = { 4, 3, 7, 5, 6, 13, 14, 15, 28, 29, 30, 31, 32, 35, 36, 37, 38, 39, 42, 41, 43, 44, 45, 46, 47, 49, 50, 51, 52, 53, 54, 55, 56 };

// OFFSET: 0x0098B870/0x140709720
static int32_t Script_ReforgeItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ReforgeItem(index)");
        return 0;
    } else {
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (index < -1 || index >= static_cast<int32_t>(CGReforge::GetNumOptions())) {
            luaL_error(L, "ReforgeItem(index): index out of bounds");
            return 0;
        }

        CGReforge::ReforgeItem(index);
    }

    return 0;
}

// OFFSET: 0x0098B8F0/0x1407097A0
static int32_t Script_GetNumReforgeOptions(lua_State* L) {
    lua_pushnumber(L, CGReforge::GetNumOptions());

    return 1;
}

// OFFSET: 0x0098D900
static bool GetGlobalText(const char* key, char* buffer, uint32_t size) {
    const char* text = FrameScript_GetText(key, -1, GENDER_NOT_APPLICABLE);
    SStrCopy(buffer, text, size);
    return *text != '\0';
}

// OFFSET: 0x0098B930/0x1407097D0
static int32_t Script_GetReforgeOptionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetReforgeOptionInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGReforge::GetNumOptions())) {
        luaL_error(L, "GetReforgeOptionInfo(index): index out of bounds");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGReforge::m_reforgeItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!player || !item) {
        return 0;
    }

    const ItemReforgeRec* reforge = CGReforge::GetOption(index);
    ItemRecSparse* stats = item->GetStats();

    if (!reforge || !stats) {
        return 0;
    }

    int32_t stat = reforge->m_sourceStat;
    int32_t amount;
    uint32_t slot = 0;

    for (; slot < 10; slot++) {
        if (stats->m_statModifierBonusStat[slot] == stat) {
            break;
        }
    }

    if (slot >= 10) {
        amount = CalculateItemBonusFromRandomEnchant(stat, item->m_item->m_itemFieldRandomPropertiesId, item->m_item->m_itemFieldPropertySeed);
    } else {
        amount = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, stats->m_itemLevel, stats->m_statModifierBonusAmount[slot], stats->m_statPercentEditor[slot], stats->m_statPercentageOfSocket[slot], stats->m_quality, stats->m_inventoryType);
    }

    int32_t sourceAmount = static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier);
    int32_t targetAmount = static_cast<int32_t>(static_cast<float>(sourceAmount) * reforge->m_targetMultiplier);
    char text[1024];
    SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[reforge->m_sourceStat]);
    GetGlobalText(text, text, sizeof(text));
    lua_pushstring(L, text);
    lua_pushnumber(L, reforge->m_sourceStat);
    lua_pushnumber(L, sourceAmount);
    SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[reforge->m_targetStat]);
    GetGlobalText(text, text, sizeof(text));
    lua_pushstring(L, text);
    lua_pushnumber(L, reforge->m_targetStat);
    lua_pushnumber(L, targetAmount);

    return 6;
}

// OFFSET: 0x0098BBA0/0x140709B20
static int32_t Script_GetReforgeItemInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGReforge::m_reforgeItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!player || !item) {
        return 0;
    }

    lua_pushnumber(L, CGReforge::FindOption(item->m_item->m_itemFieldEnchantment[8].m_id) + 1);
    const char* path = ClientDBStringLookup(3);
    char text[1024];
    SStrPrintf(text, sizeof(text), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
    lua_pushstring(L, text);
    CGItem_C::BuildItemName(text, sizeof(text), item->m_obj->m_entryID, item->m_item->m_itemFieldRandomPropertiesId);
    lua_pushstring(L, text);
    ItemRecSparse* stats = item->GetStats();

    if (stats) {
        lua_pushnumber(L, stats->m_quality);

        if (item->IsBound()) {
            SStrCopy(text, FrameScript_GetText("ITEM_SOULBOUND", -1, GENDER_NOT_APPLICABLE), sizeof(text));
            lua_pushstring(L, text);
        } else {
            lua_pushstring(L, "");
        }

        int32_t cost = stats->GetSellPrice();

        if (cost) {
            if (cost <= 10000) {
                cost = 10000;
            }
        } else {
            cost = 100000;
        }

        lua_pushnumber(L, cost);

        return 6;
    }

    return 2;
}

// OFFSET: 0x0098BD70/0x140709D30
static int32_t Script_GetSourceReforgeStats(lua_State* L) {
    int32_t results = 0;
    int32_t lastStat = -1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGReforge::m_reforgeItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!player || !item) {
        return 0;
    }

    ItemRecSparse* stats = item->GetStats();

    if (!stats) {
        return 0;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(CGReforge::GetNumOptions()); i++) {
        const ItemReforgeRec* reforge = CGReforge::GetOption(i);
        int32_t stat = reforge->m_sourceStat;

        if (stat == lastStat) {
            continue;
        }

        char text[1024];
        SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[stat]);
        SStrCopy(text, FrameScript_GetText(text, -1, GENDER_NOT_APPLICABLE), sizeof(text));
        lua_pushstring(L, text);
        lua_pushnumber(L, reforge->m_sourceStat);
        uint32_t slot = 0;

        for (; slot < 10; slot++) {
            if (stats->m_statModifierBonusStat[slot] == reforge->m_sourceStat) {
                break;
            }
        }

        int32_t amount;

        if (slot != 10) {
            amount = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, stats->m_itemLevel, stats->m_statModifierBonusAmount[slot], stats->m_statPercentEditor[slot], stats->m_statPercentageOfSocket[slot], stats->m_quality, stats->m_inventoryType);
        } else {
            amount = CalculateItemBonusFromRandomEnchant(reforge->m_sourceStat, item->m_item->m_itemFieldRandomPropertiesId, item->m_item->m_itemFieldPropertySeed);
        }

        lua_pushnumber(L, static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier));
        lastStat = reforge->m_sourceStat;
        results += 3;
    }

    return results;
}

// OFFSET: 0x0098BF90/0x140709FD0
static int32_t Script_GetDestinationReforgeStats(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetDestinationReforgeStats(stat, value)");
        return 0;
    }

    int32_t results = 0;
    int32_t stat = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t value = static_cast<int32_t>(lua_tonumber(L, 2));

    for (int32_t i = 0; i < static_cast<int32_t>(CGReforge::GetNumOptions()); i++) {
        const ItemReforgeRec* reforge = CGReforge::GetOption(i);

        if (reforge->m_sourceStat != stat) {
            continue;
        }

        char text[1024];
        SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[reforge->m_targetStat]);
        SStrCopy(text, FrameScript_GetText(text, -1, GENDER_NOT_APPLICABLE), sizeof(text));
        lua_pushstring(L, text);
        lua_pushnumber(L, reforge->m_targetStat);
        lua_pushnumber(L, static_cast<int32_t>(static_cast<float>(value) * reforge->m_targetMultiplier));
        lua_pushnumber(L, i + 1);
        results += 4;
    }

    return results;
}

// OFFSET: 0x0098C140/0x14070A200
static int32_t Script_GetReforgeItemStats(lua_State* L) {
    int32_t results = 0;
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGReforge::m_reforgeItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return 0;
    }

    ItemRecSparse* stats = item->GetStats();

    if (!stats) {
        return 0;
    }

    int32_t randomProperties = item->m_item->m_itemFieldRandomPropertiesId;
    char text[1024];

    for (uint32_t order = 0; order < 33; order++) {
        for (uint32_t slot = 0; slot < 10; slot++) {
            int32_t stat = stats->m_statModifierBonusStat[slot];

            if (stat <= 0 || s_reforgeStatOrder[order] != stat || stat == 50) {
                continue;
            }

            SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[stat]);
            SStrCopy(text, FrameScript_GetText(text, -1, GENDER_NOT_APPLICABLE), sizeof(text));

            if (text[0]) {
                lua_pushstring(L, text);
                lua_pushnumber(L, stat);
                lua_pushnumber(L, stats->m_statModifierBonusAmount[slot]);
                results += 3;
            }
        }
    }

    if (randomProperties > 0) {
        const ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomProperties);

        if (properties) {
            for (uint32_t i = 0; i < 5; i++) {
                const SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(properties->m_enchantment[i]);

                if (!enchantment) {
                    continue;
                }

                for (uint32_t j = 0; j < 3; j++) {
                    if (enchantment->m_effect[j] != 5) {
                        continue;
                    }

                    int32_t stat = enchantment->m_effectArg[j];
                    SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[stat]);
                    SStrCopy(text, FrameScript_GetText(text, -1, GENDER_NOT_APPLICABLE), sizeof(text));
                    lua_pushstring(L, text);
                    lua_pushnumber(L, stat);
                    lua_pushnumber(L, (enchantment->m_effectPointsMax[j] + enchantment->m_effectPointsMin[j]) / 2);
                    results += 3;
                }
            }
        }
    } else if (randomProperties < 0) {
        const ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomProperties);

        if (suffix) {
            for (uint32_t i = 0; i < 5; i++) {
                const SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(suffix->m_enchantment[i]);

                if (!enchantment) {
                    continue;
                }

                for (uint32_t j = 0; j < 3; j++) {
                    if (enchantment->m_effect[j] != 5) {
                        continue;
                    }

                    int32_t stat = enchantment->m_effectArg[j];
                    SStrPrintf(text, sizeof(text), "%s_SHORT", s_itemModTokens[stat]);
                    SStrCopy(text, FrameScript_GetText(text, -1, GENDER_NOT_APPLICABLE), sizeof(text));
                    lua_pushstring(L, text);
                    lua_pushnumber(L, stat);
                    lua_pushnumber(L, CGItem_C::GetRandomPropertyOverrideAmountEx(i + 10, randomProperties, static_cast<uint16_t>(item->m_item->m_itemFieldPropertySeed)));
                    results += 3;
                }
            }
        }
    }

    return results;
}

// OFFSET: 0x0098CF20/0x14070B340
static int32_t Script_SetReforgeFromCursorItem(lua_State* L) {
    WOWGUID current = CGReforge::m_reforgeItem;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (!CGGameUI::m_hasControl && !(player->m_unit->m_unitFieldFlags & 0x100000))) {
        return 0;
    }

    CGItem_C* cursor = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__));

    if (cursor) {
        CGReforge::SetItem(cursor->m_obj->m_guid);
        CGGameUI::ClearCursor(0, 1);
        return 0;
    }

    if (current) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(current, TYPEMASK_ITEM, __FILE__, __LINE__));
        uint8_t slot = CGPlayer_C::FindItemSlot(item->m_item->m_itemFieldContained, item);
        CGGameUI::SetCursorItem(current, item->m_item->m_itemFieldContained, slot, 0, 0, 1);
        CGReforge::SetItem(WOWGUID());
    }

    return 0;
}

// OFFSET: 0x0098D0E0
static int32_t Script_CloseReforge(lua_State* L) {
    CGReforge::SetForgeMaster(WOWGUID());

    return 0;
}

// STATIC: 0x00D144E0
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetReforgeFromCursorItem", &Script_SetReforgeFromCursorItem },
    { "GetNumReforgeOptions", &Script_GetNumReforgeOptions },
    { "GetReforgeOptionInfo", &Script_GetReforgeOptionInfo },
    { "ReforgeItem", &Script_ReforgeItem },
    { "GetReforgeItemInfo", &Script_GetReforgeItemInfo },
    { "GetSourceReforgeStats", &Script_GetSourceReforgeStats },
    { "GetDestinationReforgeStats", &Script_GetDestinationReforgeStats },
    { "CloseReforge", &Script_CloseReforge },
    { "GetReforgeItemStats", &Script_GetReforgeItemStats },
};

// OFFSET: 0x0098B550
void ReforgeRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0098B580
void ReforgeUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
