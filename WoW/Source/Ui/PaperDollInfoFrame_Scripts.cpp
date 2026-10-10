#include "PaperDollInfoFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include <cstdlib>
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Object/ArenaTeamCache.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Frame/CSimpleTexture.h"

// OFFSET: 0x0097EB00/0x1406F8A50
static int32_t Script_GetInventoryItemTexture(lua_State* L) {
    const char* token;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, token, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(token));

    if (CGCharacterInfo::CanInspect(unit)) {
        int32_t displayId = 0;

        if (slot == -1) {
            if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
                int32_t ammoId = CGCharacterInfo::GetActivePlayerAmmoID(unit);

                if (ammoId && unit->GetBag()->GetItemTypeCount(ammoId, 0)) {
                    WOWGUID requester = unit->m_obj->m_guid;
                    ItemStats_C* stats = g_itemDBCache.GetRecord(ammoId, requester, UnitInventoryChangedCallback, NULL, false);

                    if (stats) {
                        displayId = stats->m_displayId;
                    }
                }
            }
        } else {
            const VisibleItem_C* visible = unit->GetVisibleItem(slot);

            if (visible && visible->m_itemId) {
                displayId = const_cast<VisibleItem_C*>(visible)->GetDisplayID();
            } else {
                const CGItem_C* item = static_cast<const CGBag_C*>(unit->GetBag())->GetItemPointer(slot);

                if (item && !(item->m_modelFlags & 0x10000)) {
                    displayId = item->GetDisplayID();
                }
            }
        }

        if (displayId) {
            char texture[260];
            const char* path = ClientDBStringLookup(3);
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(displayId));
            lua_pushstring(L, texture);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0097DCE0/0x1406F7950
static int32_t Script_UpdateInventoryAlertStatus(lua_State* L) {
    CGCharacterInfo::UpdateInventoryAlerts();
    return 0;
}

// OFFSET: 0x0097B410/0x1406F4A10
static int32_t Script_HasInspectHonorData(lua_State* L) {
    if (CGCharacterInfo::m_hasHonorData) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: #0x005E7700
static int32_t Script_KeyRingButtonIDToInvSlotID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: KeyRingButtonIDToInvSlotID(buttonID)");
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(lua_tonumber(L, 1)) + 86);
    return 1;
}

// OFFSET: 0x0097DCF0/0x1406F7960
static int32_t Script_OffhandHasWeapon(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item = player ? player->m_bag.GetItemPointer(16) : NULL;

    if (item && item->GetClassID() == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0097BFF0/0x1406F5770
static int32_t Script_GetInventoryAlertStatus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInventoryAlertStatus(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t status = index >= CGCharacterInfo::m_invAlertStatus.m_count ? 0 : CGCharacterInfo::m_invAlertStatus.m_data[index];
    lua_pushnumber(L, status);
    return 1;
}

// OFFSET: 0x0097DEE0/0x1406F7BB0
static int32_t Script_HasWandEquipped(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && static_cast<uint32_t>(player->m_bag.m_numSlots) > 17 && player->m_bag.m_slots[17]) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(player->m_bag.m_slots[17], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && item->GetSubtypeID() == 19) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0097B450/0x1406F4A50
static int32_t Script_GetInspectHonorData(lua_State* L) {
    lua_pushnumber(L, CGCharacterInfo::m_inspectTodayHonorableKills);
    lua_pushnumber(L, CGCharacterInfo::m_inspectTodayHonor);
    lua_pushnumber(L, CGCharacterInfo::m_inspectYesterdayHonorableKills);
    lua_pushnumber(L, CGCharacterInfo::m_inspectYesterdayHonor);
    lua_pushnumber(L, CGCharacterInfo::m_inspectLifetimeHonorableKills);
    lua_pushnumber(L, CGCharacterInfo::m_inspectLifetimeHighestRank);
    return 6;
}

// OFFSET: 0x0097BBD0/0x1406F5180
static int32_t Script_GetInventorySlotInfo(lua_State* L) {
    if (lua_isstring(L, 1)) {
        const char* name = lua_tolstring(L, 1, NULL);
        int32_t count = g_paperDollItemFrameDB.m_numRecords;

        for (int32_t i = 0; i < count; i++) {
            PaperDollItemFrameRec* rec = g_paperDollItemFrameDB.GetRecordByIndex(i);

            if (rec && !SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                lua_pushnumber(L, rec->m_slotNumber);
                lua_pushstring(L, rec->m_slotIcon);

                if (rec->m_slotNumber == 18) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                return 3;
            }
        }
    }

    luaL_error(L, "Invalid inventory slot in GetInventorySlotInfo");
    return 0;
}

// OFFSET: 0x0097D8C0/0x1406F74D0
static int32_t Script_IsInventoryItemLocked(lua_State* L) {
    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot)) {
        return 0;
    }

    CGPlayer_C* player = slot != -1 ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) : NULL;
    const CGItem_C* item = player ? static_cast<const CGBag_C*>(&player->m_bag)->GetItemPointer(slot) : NULL;

    if (item && (item->m_itemFlags & 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0097BCC0
static int32_t Script_GetInventoryItemCooldown(lua_State* L) {
    const char* unitName;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, unitName, slot)) {
        return 0;
    }

    CGPlayer_C* unit;
    CGItem_C* item;

    if (slot != -1 && CGCharacterInfo::CanInspect(unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(unitName))) && (item = unit->m_bag.GetItemPointer(slot)) && item->GetEnchantmentSpell(0) && (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges())) {
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &duration, &start, &enable);
        lua_pushnumber(L, start * 0.001);
        lua_pushnumber(L, duration * 0.001);
        lua_pushnumber(L, enable);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// OFFSET: 0x0097D340
static int32_t Script_GetInventoryItemBroken(lua_State* L) {
    const char* unitName;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, unitName, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(unitName));
    CGItem_C* item;

    if (!CGCharacterInfo::CanInspect(unit) || slot == -1 || !(item = unit->m_bag.GetItemPointer(slot)) || (item->m_item->m_itemFieldFlags & 0x8) || !item->m_item->m_itemFieldMaxdurability || item->GetDurability()) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x0097D400
static int32_t Script_GetInventoryItemCount(lua_State* L) {
    const char* unitName;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, unitName, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(unitName));

    if (!CGCharacterInfo::CanInspect(unit)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (slot == -1) {
        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            int32_t ammo = CGCharacterInfo::GetActivePlayerAmmoID(unit);

            if (ammo) {
                lua_pushnumber(L, unit->GetBag()->GetItemTypeCount(ammo, 0));
                return 1;
            }
        }

        lua_pushnumber(L, 1.0);
        return 1;
    }

    CGItem_C* item = unit->m_bag.GetItemPointer(slot);

    if (!item) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (!(item->m_obj->m_type & TYPEMASK_CONTAINER)) {
        lua_pushnumber(L, item->m_item->m_itemFieldStackCount);
        return 1;
    }

    if (slot > 22) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID != static_cast<int32_t>(item->GetClassID()) || rec->m_subClassID != static_cast<int32_t>(item->GetSubtypeID())) {
            continue;
        }

        if (!(rec->m_displayFlags & 0x4)) {
            break;
        }

        if (item->GetBag()->GetItemTypeCount(-1, 0) <= 0) {
            lua_pushnumber(L, 0.0);
        } else {
            lua_pushnumber(L, item->GetBag()->GetItemTypeCount(-1, 0));
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x0097D7D0/0x1406F7400
static int32_t Script_PickupInventoryItem(lua_State* L) {
    int32_t slot = 0;

    if (ResolveInventorySlotArg(L, 1, slot)) {
        CGCharacterInfo::PickupInventoryItem(slot);
    }

    return 0;
}

// PROVEN: 9bfc15f6 IDENTICAL/NONE
// OFFSET: 0x0097D9D0/0x1406F75E0
static int32_t Script_PutItemInBackpack(lua_State* L) {
    if (CGCharacterInfo::PutItemInBackpack(0xFF)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0097DA60/0x1406F7670
static int32_t Script_CursorCanGoInSlot(lua_State* L) {
    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if ((player && player->ValidateSlot(slot, CGGameUI::GetCursorItem())) || (slot == -1 && CGGameUI::m_cursorItemType == 9)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0097DD70/0x1406F79F0
static int32_t Script_GetWeaponEnchantInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 6;
    }

    for (int32_t slot = 15; slot <= 16; slot++) {
        CGItem_C* item = player->m_bag.GetItemPointer(slot);

        if (item && item->GetEnchantmentId(1)) {
            lua_pushnumber(L, 1.0);

            if (item->GetEnchantmentDuration(1)) {
                lua_pushnumber(L, item->GetEnchantmentTimeLeft(1));
            } else {
                lua_pushnil(L);
            }

            lua_pushnumber(L, item->GetEnchantmentCharges(1));
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    return 6;
}

// OFFSET: 0x0097DB90/0x1406F77C0
static int32_t Script_GetGuildInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetGuildInfo(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
        uint64_t guildId = player->GetGuildGUID().m_guid;

        if (guildId) {
            WOWGUID guid = unit->m_obj->m_guid;
            GuildStats_C* guild = g_guildInfoCache.GetRecord(guildId, guid, GuildNameCallback, NULL, false);

            if (guild) {
                char rankName[64];
                SStrCopy(rankName, guild->m_rankNames[player->m_player->m_playerGuildrank], STORM_MAX_STR);
                lua_pushstring(L, guild->m_name);
                lua_pushstring(L, rankName);
                lua_pushnumber(L, player->m_player->m_playerGuildrank);
                return 3;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    return 3;
}

// OFFSET: 0x0097D960/0x1406F7570
static int32_t Script_PutItemInBag(lua_State* L) {
    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot) || slot < 19) {
        return 0;
    }

    if (CGCharacterInfo::PutItemInBackpack(slot)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: #0x005E7640
static void InspectArenaTeamCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, NULL);
    }
}

// OFFSET: 0x0097B4F0/0x1406F4AE0
static int32_t Script_ClearInspectPlayer(lua_State* L) {
    WOWGUID none = WOWGUID();

    if (CGCharacterInfo::m_inspectTarget) {
        CGCharacterInfo::m_inspectTarget = WOWGUID();
        CGCharacterInfo::m_hasHonorData = 0;
        CGCharacterInfo::m_honorRequestPending = 0;
        CGCharacterInfo::m_inspectArenaTeams[0].m_teamId = 0;
        CGCharacterInfo::m_inspectArenaTeams[1].m_teamId = 0;
        CGCharacterInfo::m_inspectArenaTeams[2].m_teamId = 0;
        CGTalentInfo_Inspect::SetInspectTarget(none);
    }

    return 0;
}

// OFFSET: 0x0097C090/0x1406F5800
static int32_t Script_GetInspectArenaTeamData(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInspectArenaTeamData(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 3) {
        return 0;
    }

    InspectArenaTeam* team = &CGCharacterInfo::m_inspectArenaTeams[index];

    if (!team || !team->m_teamId) {
        return 0;
    }

    WOWGUID requester = CGCharacterInfo::m_inspectTarget;
    ArenaTeamCache* cache = g_arenaTeamCache.GetRecord(team->m_teamId, requester, &InspectArenaTeamCacheCallback, NULL, false);

    if (!cache) {
        return 0;
    }

    lua_pushstring(L, cache->m_name);
    lua_pushnumber(L, static_cast<int32_t>(cache->m_type));
    lua_pushnumber(L, static_cast<int32_t>(team->m_rating));
    lua_pushnumber(L, static_cast<int32_t>(team->m_weekGames));
    lua_pushnumber(L, static_cast<int32_t>(team->m_weekWins));
    lua_pushnumber(L, static_cast<int32_t>(team->m_seasonGames));
    lua_pushnumber(L, static_cast<int32_t>(team->m_personalRating));
    uint32_t color = cache->m_backgroundColor;
    lua_pushnumber(L, static_cast<uint8_t>(color >> 16) * 0.0039215689);
    lua_pushnumber(L, static_cast<uint8_t>(color >> 8) * 0.0039215689);
    lua_pushnumber(L, static_cast<uint8_t>(color) * 0.0039215689);
    lua_pushnumber(L, static_cast<int32_t>(cache->m_emblemStyle));
    color = cache->m_emblemColor;
    lua_pushnumber(L, static_cast<uint8_t>(color >> 16) * 0.0039215689);
    lua_pushnumber(L, static_cast<uint8_t>(color >> 8) * 0.0039215689);
    lua_pushnumber(L, static_cast<uint8_t>(color) * 0.0039215689);
    lua_pushnumber(L, static_cast<int32_t>(cache->m_borderStyle));
    color = cache->m_borderColor;
    lua_pushnumber(L, static_cast<uint8_t>(color >> 16) * 0.0039215689);
    lua_pushnumber(L, static_cast<uint8_t>(color >> 8) * 0.0039215689);
    lua_pushnumber(L, static_cast<uint8_t>(color) * 0.0039215689);
    return 18;
}

// OFFSET: 0x0097E6E0/0x1406F8590
static int32_t Script_GetInventoryItemsForSlot(lua_State* L) {
    uint32_t slot = 0;

    if (lua_isnumber(L, 1)) {
        slot = static_cast<int32_t>(lua_tonumber(L, 1) - 1.0);

        if (slot > 18) {
            return 0;
        }
    } else {
        if (lua_isstring(L, 1)) {
            const char* name = lua_tolstring(L, 1, NULL);

            for (int32_t i = 0; i < g_paperDollItemFrameDB.m_numRecords; i++) {
                PaperDollItemFrameRec* rec = g_paperDollItemFrameDB.GetRecordByIndex(i);

                if (rec && !SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                    slot = rec->m_slotNumber;
                }
            }
        }

        if (!slot) {
            luaL_error(L, "Usage: GetInventoryItemsForSlot(slot [, returnTable])");
            return 0;
        }
    }

    if (lua_type(L, 2) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGBag_C* inventory = player->GetBag();
    ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);

    if (!chrClass) {
        return 0;
    }

    int32_t bypass = (chrClass->m_flags & 0x8) != 0;

    for (int32_t i = 0; i <= 18; i++) {
        CGItem_C* item = inventory->GetItemPointer(i);

        if (item && item->CanGoInSlot(slot, bypass) && CGCharacterInfo::CanEquipItem(item, slot)) {
            lua_checkstack(L, 2);
            lua_pushnumber(L, i + 0x100001);
            lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
            lua_settable(L, -3);
        }
    }

    for (int32_t i = 23; i <= 38; i++) {
        CGItem_C* item = inventory->GetItemPointer(i);

        if (item && item->CanGoInSlot(slot, bypass)) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

            if (stats && CGCharacterInfo::CanEquipItemInSlot(stats, slot)) {
                lua_checkstack(L, 2);
                lua_pushnumber(L, i + 0x2FFFEA);
                lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
                lua_settable(L, -3);
            }
        }
    }

    for (uint32_t bagIndex = 0; bagIndex < 4; bagIndex++) {
        CGObject_C* container = ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(bagIndex), TYPEMASK_CONTAINER, __FILE__, __LINE__);

        if (!container) {
            continue;
        }

        CGBag_C* bag = container->GetBag();
        int32_t location = ((bagIndex + 1) << 8) | 0x300000;

        for (int32_t i = 0; i <= bag->m_numSlots; i++) {
            CGItem_C* item = bag->GetItemPointer(i);

            if (item && item->CanGoInSlot(slot, bypass)) {
                WOWGUID none = WOWGUID();
                ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

                if (stats && CGCharacterInfo::CanEquipItemInSlot(stats, slot)) {
                    lua_checkstack(L, 2);
                    lua_pushnumber(L, i + location + 1);
                    lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
                    lua_settable(L, -3);
                }
            }
        }
    }

    if (!CGBankInfo::m_banker) {
        return 1;
    }

    for (int32_t i = 39; i <= 66; i++) {
        CGItem_C* item = inventory->GetItemPointer(i);

        if (item && item->CanGoInSlot(slot, bypass)) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

            if (stats && CGCharacterInfo::CanEquipItemInSlot(stats, slot)) {
                lua_checkstack(L, 2);
                lua_pushnumber(L, i + 0x400001);
                lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
                lua_settable(L, -3);
            }
        }
    }

    for (uint32_t bagIndex = 4; bagIndex < 11; bagIndex++) {
        CGObject_C* container = ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(bagIndex), TYPEMASK_CONTAINER, __FILE__, __LINE__);

        if (!container) {
            continue;
        }

        CGBag_C* bag = container->GetBag();

        for (int32_t i = 0; i <= bag->m_numSlots; i++) {
            CGItem_C* item = bag->GetItemPointer(i);

            if (item && item->CanGoInSlot(slot, bypass)) {
                WOWGUID none = WOWGUID();
                ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

                if (stats && CGCharacterInfo::CanEquipItemInSlot(stats, slot)) {
                    lua_checkstack(L, 2);
                    lua_pushnumber(L, static_cast<int32_t>((((bagIndex - 3) | 0x6000) << 8) + i + 1));
                    lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
                    lua_settable(L, -3);
                }
            }
        }
    }

    return 1;
}

// OFFSET: 0x0097EC80
static int32_t Script_GetInventoryItemQuality(lua_State* L) {
    const char* token;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, token, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(token));

    if (!CGCharacterInfo::CanInspect(unit)) {
        lua_pushnil(L);
        return 1;
    }

    ItemStats_C* stats;

    if (slot != -1) {
        CGItem_C* item = unit->m_bag.GetItemPointer(slot);

        if (!item) {
            lua_pushnil(L);
            return 1;
        }

        WOWGUID none = WOWGUID();
        stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);
    } else {
        if (unit->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
            lua_pushnil(L);
            return 1;
        }

        int32_t ammo = CGCharacterInfo::GetActivePlayerAmmoID(unit);

        if (!ammo) {
            lua_pushnil(L);
            return 1;
        }

        WOWGUID requester = unit->m_obj->m_guid;
        stats = g_itemDBCache.GetRecord(ammo, requester, &UnitInventoryChangedCallback, NULL, false);
    }

    if (!stats) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, stats->m_inventoryType ? static_cast<int32_t>(stats->m_quality) : -1);
    return 1;
}

// OFFSET: 0x0097D5A0/0x1406F71B0
static int32_t Script_GetInventoryItemDurability(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInventoryItemDurability(slot)");
        return 0;
    }

    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot) || slot == -1) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGItem_C* item = player->m_bag.GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    int32_t maxDurability = item->GetMaxDurability();

    if (!maxDurability) {
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(item->GetDurability()));
    lua_pushnumber(L, maxDurability);
    return 2;
}

// OFFSET: 0x0097ED50
static int32_t Script_GetInventoryItemLink(lua_State* L) {
    const char* token;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, token, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(token));

    if (!CGCharacterInfo::CanInspect(unit)) {
        return 0;
    }

    WOWGUID guid = unit->m_obj->m_guid;
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (slot == -1) {
        int32_t ammo = guid == activePlayer ? CGCharacterInfo::GetActivePlayerAmmoID(unit) : 0;
        WOWGUID none = WOWGUID();
        ItemStats_C* stats = g_itemDBCache.GetRecord(ammo, none, NULL, NULL, false);

        if (stats) {
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(ammo, stats->m_quality, 0, 0, 0, 0));
            return 1;
        }
    } else if (guid == activePlayer) {
        CGItem_C* item = unit->m_bag.GetItemPointer(slot);

        if (item) {
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
            return 1;
        }
    } else {
        InspectItem* inspected = unit->GetInspectedItem(slot);

        if (inspected && inspected->m_itemId) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(inspected->m_itemId, none, NULL, NULL, false);

            if (stats) {
                lua_pushstring(L, CGTooltip::GetItemHyperlinkString(inspected->m_itemId, stats->m_quality, inspected->m_enchants[0], &inspected->m_enchants[2], static_cast<uint16_t>(inspected->m_randomPropertiesId), inspected->m_propertySeed, 0));
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x0097BDF0
static int32_t Script_GetInventoryItemID(lua_State* L) {
    const char* token;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, token, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(token));

    if (!CGCharacterInfo::CanInspect(unit)) {
        return 0;
    }

    WOWGUID guid = unit->m_obj->m_guid;
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (slot == -1) {
        if (guid == activePlayer) {
            int32_t ammo = CGCharacterInfo::GetActivePlayerAmmoID(unit);

            if (ammo) {
                lua_pushnumber(L, ammo);
                return 1;
            }
        }
    } else if (guid == activePlayer) {
        CGItem_C* item = unit->m_bag.GetItemPointer(slot);

        if (item) {
            lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
            return 1;
        }
    } else {
        const VisibleItem_C* visible = unit->GetVisibleItem(slot);

        if (visible) {
            int32_t itemId = abs(visible->m_itemId);

            if (itemId) {
                lua_pushnumber(L, itemId);
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x0097D6B0/0x1406F72D0
static int32_t Script_GetInventoryItemGems(lua_State* L) {
    int32_t slot;

    if (!lua_isnumber(L, 1) || (slot = 0, !ResolveInventorySlotArg(L, 1, slot))) {
        luaL_error(L, "Usage: GetInventoryItemGems(slot)");
        return 0;
    }

    if (slot == -1) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 3;
    }

    CGItem_C* item = player->m_bag.GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    for (int32_t i = 0; i < 3; i++) {
        SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(item->GetEnchantmentId(i + 2));

        if (enchant && enchant->m_srcItemID) {
            lua_pushnumber(L, enchant->m_srcItemID);
        } else {
            lua_pushnil(L);
        }
    }

    return 3;
}

// OFFSET: 0x0097D810/0x1406F7430
static int32_t Script_UseInventoryItem(lua_State* L) {
    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot) || slot == -1) {
        return 0;
    }

    WOWGUID target;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true)) {
        CGCharacterInfo::UseItem(slot, &target);
    }

    return 0;
}

// OFFSET: 0x0097D880/0x1406F74A0
static int32_t Script_SocketInventoryItem(lua_State* L) {
    int32_t slot = 0;

    if (ResolveInventorySlotArg(L, 1, slot) && slot != -1) {
        CGCharacterInfo::SocketInventoryItem(slot);
    }

    return 0;
}

// OFFSET: 0x0097DA20/0x1406F7630
static int32_t Script_PickupBagFromSlot(lua_State* L) {
    int32_t slot = 0;

    if (ResolveInventorySlotArg(L, 1, slot) && slot >= 19) {
        CGCharacterInfo::PickupBag(slot);
    }

    return 0;
}

// OFFSET: 0x0097DB00/0x1406F7720
static int32_t Script_ShowInventorySellCursor(lua_State* L) {
    if (Spell_C_IsTargeting() || CursorGetResetMode() != 1) {
        return 0;
    }

    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot) || slot == -1) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGItem_C* item = player->GetBag()->GetItemPointer(slot);

    if (item && !(item->m_itemFlags & 0x1)) {
        CursorSetMode(3);
    }

    return 0;
}

// OFFSET: 0x0097BED0/0x1406F5600
static int32_t Script_SetInventoryPortraitTexture(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    texture->SetTexture(static_cast<CTexture*>(NULL));
    int32_t slot = 0;

    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetInventoryPortraitTexture(texture, unit, slot)");
        return 0;
    }

    if (!ResolveInventorySlotArg(L, 3, slot)) {
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 2, NULL));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
    CGItem_C* item = slot == -1 ? player->m_bag.FindItemOfType(CGCharacterInfo::GetActivePlayerAmmoID(player), 0) : player->m_bag.GetItemPointer(slot);

    if (item) {
        const char* path = ClientDBStringLookup(3);
        char buffer[260];
        SStrPrintf(buffer, sizeof(buffer), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
        SetPortraitTexture(texture, buffer);
    }

    return 0;
}

// OFFSET: 0x0097C080/0x1406F57F0
static int32_t Script_RequestInspectHonorData(lua_State* L) {
    CGCharacterInfo::RequestInspectHonorData();
    return 0;
}

// STATIC: #0x00AD1270
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetInventorySlotInfo", &Script_GetInventorySlotInfo },
    { "GetInventoryItemsForSlot", &Script_GetInventoryItemsForSlot },
    { "GetInventoryItemTexture", &Script_GetInventoryItemTexture },
    { "GetInventoryItemBroken", &Script_GetInventoryItemBroken },
    { "GetInventoryItemCount", &Script_GetInventoryItemCount },
    { "GetInventoryItemQuality", &Script_GetInventoryItemQuality },
    { "GetInventoryItemCooldown", &Script_GetInventoryItemCooldown },
    { "GetInventoryItemDurability", &Script_GetInventoryItemDurability },
    { "GetInventoryItemLink", &Script_GetInventoryItemLink },
    { "GetInventoryItemID", &Script_GetInventoryItemID },
    { "GetInventoryItemGems", &Script_GetInventoryItemGems },
    { "KeyRingButtonIDToInvSlotID", &Script_KeyRingButtonIDToInvSlotID },
    { "PickupInventoryItem", &Script_PickupInventoryItem },
    { "UseInventoryItem", &Script_UseInventoryItem },
    { "SocketInventoryItem", &Script_SocketInventoryItem },
    { "IsInventoryItemLocked", &Script_IsInventoryItemLocked },
    { "PutItemInBag", &Script_PutItemInBag },
    { "PutItemInBackpack", &Script_PutItemInBackpack },
    { "PickupBagFromSlot", &Script_PickupBagFromSlot },
    { "CursorCanGoInSlot", &Script_CursorCanGoInSlot },
    { "ShowInventorySellCursor", &Script_ShowInventorySellCursor },
    { "SetInventoryPortraitTexture", &Script_SetInventoryPortraitTexture },
    { "GetGuildInfo", &Script_GetGuildInfo },
    { "GetInventoryAlertStatus", &Script_GetInventoryAlertStatus },
    { "UpdateInventoryAlertStatus", &Script_UpdateInventoryAlertStatus },
    { "OffhandHasWeapon", &Script_OffhandHasWeapon },
    { "HasInspectHonorData", &Script_HasInspectHonorData },
    { "RequestInspectHonorData", &Script_RequestInspectHonorData },
    { "GetInspectHonorData", &Script_GetInspectHonorData },
    { "GetInspectArenaTeamData", &Script_GetInspectArenaTeamData },
    { "ClearInspectPlayer", &Script_ClearInspectPlayer },
    { "GetWeaponEnchantInfo", &Script_GetWeaponEnchantInfo },
    { "HasWandEquipped", &Script_HasWandEquipped },
};

// OFFSET: #0x005E78F0
void CharacterInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: #0x005E7920
void CharacterInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
