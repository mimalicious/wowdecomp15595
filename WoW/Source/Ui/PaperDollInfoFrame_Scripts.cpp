#include "PaperDollInfoFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
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
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Game/ItemTransmogrify.h"
#include "WoW/Source/Ui/Transmogrify.h"

// OFFSET: 0x0097BA80
static int32_t ResolveInventorySlotArg(lua_State* L, int32_t index, int32_t& slot) {
    if (lua_isnumber(L, index)) {
        slot = static_cast<int32_t>(lua_tonumber(L, index)) - 1;
    } else if (lua_isstring(L, index)) {
        const char* name = lua_tolstring(L, index, NULL);
        int32_t count = g_paperDollItemFrameDB.m_numRecords;

        for (int32_t i = 0; i < count; i++) {
            PaperDollItemFrameRec* rec = g_paperDollItemFrameDB.GetRecordByIndex(i);

            if (!SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                slot = rec->m_slotNumber;
                break;
            }
        }
    }

    return (slot >= 0 && slot <= 22) || (slot >= 39 && slot <= 66) || (slot >= 67 && slot <= 73);
}

// OFFSET: 0x0097BB60
static int32_t GetUnitAndSlotFromLua(lua_State* L, const char*& unit, int32_t& slot) {
    if (!lua_isstring(L, 1)) {
        char buffer[1024];
        luaL_error(L, "Usage: %s(unit, slot)", FrameScript_GetCurrentFunction(L, buffer, sizeof(buffer)));
        return 0;
    }

    unit = lua_tolstring(L, 1, NULL);
    return ResolveInventorySlotArg(L, 2, slot);
}

// OFFSET: 0x0097EB00/0x1406F8A50
static int32_t Script_GetInventoryItemTexture(lua_State* L) {
    const char* token;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, token, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(token));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER) && CGCharacterInfo::CanInspect(unit)) {
        int32_t displayId = 0;
        uint64_t guid = unit->m_obj->m_guid.m_guid;

        if (guid == ClntObjMgrGetActivePlayer().m_guid) {
            const CGItem_C* item = unit->m_bag.GetItemPointer(slot);

            if (item && !(item->m_modelFlags & 0x10000)) {
                displayId = item->GetDisplayID();
            }
        } else {
            InspectItem* inspected = unit->GetInspectedItem(slot);

            if (inspected) {
                int32_t id = inspected->m_itemId;
                WOWGUID none;
                none.m_guid = 0;
                ItemRec* rec = g_itemDB.GetRecord(id, none, NULL, NULL, false);

                if (rec) {
                    displayId = rec->m_displayInfoID;
                }
            } else {
                const VisibleItem_C* visible = unit->GetVisibleItem(slot);

                if (visible && abs(visible->m_itemId)) {
                    displayId = const_cast<VisibleItem_C*>(visible)->GetDisplayID();
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

// OFFSET: 0x0097DCF0/0x1406F7960
static int32_t Script_OffhandHasWeapon(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGBag_C* bag = player ? &player->m_bag : NULL;
    CGItem_C* item;

    if (player && bag && (item = bag->GetItemPointer(16)) && item->GetClassID() == 2) {
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
    int32_t status = index < CGCharacterInfo::m_invAlertStatus.m_count ? CGCharacterInfo::m_invAlertStatus.m_data[index] : 0;
    lua_pushnumber(L, status);
    return 1;
}

// OFFSET: 0x0097DEE0/0x1406F7BB0
static int32_t Script_HasWandEquipped(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && static_cast<uint32_t>(player->m_bag.m_numSlots) > 17) {
        uint64_t guid = player->m_bag.m_slots[17].m_guid;

        if (guid) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(guid), TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && item->GetSubtypeID() == 19) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0097B450/0x1406F4A50
static int32_t Script_GetInspectHonorData(lua_State* L) {
    lua_pushnumber(L, CGCharacterInfo::m_inspectTodayHonorableKills);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, CGCharacterInfo::m_inspectYesterdayHonorableKills);
    lua_pushnumber(L, 0.0);
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

                if (rec->m_slotNumber - 1 == 17) {
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

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item;

    if (player && (item = player->m_bag.GetItemPointer(slot)) && (item->m_itemFlags & 1)) {
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

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(unitName));
    CGItem_C* item;

    if (CGCharacterInfo::CanInspect(unit) && (item = unit->m_bag.GetItemPointer(slot)) && item->GetEnchantmentSpell(0) && (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges())) {
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

    if (CGCharacterInfo::CanInspect(unit) && (item = unit->m_bag.GetItemPointer(slot)) && item->GetMaxDurability() && !item->GetDurability()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
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
    CGItem_C* item;

    if (!CGCharacterInfo::CanInspect(unit) || !(item = unit->m_bag.GetItemPointer(slot))) {
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

    uint32_t count = g_itemSubClassDB.m_numRecords;

    for (uint32_t i = 0; i < count; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID != static_cast<int32_t>(item->GetClassID()) || rec->m_subClassID != static_cast<int32_t>(item->GetSubtypeID())) {
            continue;
        }

        if (!(rec->m_displayFlags & 0x4)) {
            break;
        }

        if (item->GetBag()->GetItemTypeCount(-1, 0) <= 0) {
            lua_pushnumber(L, 0);
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

    if (player && player->ValidateSlot(slot, CGGameUI::GetCursorItem())) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0097DD70/0x1406F79F0
static int32_t Script_GetWeaponEnchantInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t slots[3] = { 15, 16, 17 };

    if (!player) {
        return 0;
    }

    CGBag_C* bag = &player->m_bag;

    for (uint32_t i = 0; i < 3; i++) {
        CGItem_C* item = bag->GetItemPointer(slots[i]);

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

    return 9;
}

// OFFSET: 0x0097DB90/0x1406F77C0
static int32_t Script_GetGuildInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetGuildInfo(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    uint64_t guildId;
    GuildStats_C* guild;

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER) && (guildId = unit->GetGuildGUID().m_guid) != 0) {
        WOWGUID guid = unit->m_obj->m_guid;
        guild = g_guildInfoCache.GetRecord2(guildId, guid, GuildNameCallback, NULL, false);

        if (guild) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
            int32_t rank = -1;

            for (int32_t i = 0; i < guild->m_numRanks; i++) {
                if (guild->m_rankIds[i] == static_cast<int32_t>(player->m_player->m_playerGuildrank)) {
                    rank = i;
                    break;
                }
            }

            if (rank < 0) {
                return 0;
            }

            char rankName[64];
            SStrCopy(rankName, guild->m_rankNames[rank], sizeof(rankName));
            lua_pushstring(L, guild->m_name);
            lua_pushstring(L, rankName);
            lua_pushnumber(L, guild->m_rankOrders[rank]);
            return 3;
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

// OFFSET: 0x0097B1F0
static void InspectArenaTeamCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_HONOR_UPDATE, NULL);
    }
}

// OFFSET: 0x0097B4F0/0x1406F4AE0
static int32_t Script_ClearInspectPlayer(lua_State* L) {
    WOWGUID none;
    none.m_guid = 0;

    if (none != CGCharacterInfo::m_inspectTarget) {
        CGCharacterInfo::m_inspectTarget.m_guid = 0;
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

// OFFSET: 0x0097E5A0
static void AddInventoryItemForSlot(CGItem_C* item, lua_State* L, int32_t location, uint32_t slot, uint8_t bypass, bool transmogrify, CGItem_C* target, const ItemRecInterface* targetRec) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!item || !player) {
        return;
    }

    int32_t tableIndex;

    if (transmogrify) {
        ItemRecSparse* stats = item->GetStats();

        if (!stats || !item->CanGoInSlot(slot, bypass)) {
            return;
        }

        int32_t error;

        if (!player->CanUseItem(stats, error)) {
            return;
        }

        ClientItemRec rec(item->GetStats());
        int32_t reason;

        if (!CanTransmogrifyItemWithItem(&rec, targetRec, target->m_item->m_itemFieldFlags, item->m_item->m_itemFieldRandomPropertiesId, target->m_item->m_itemFieldRandomPropertiesId, &reason)) {
            return;
        }

        tableIndex = -4;
    } else {
        if (!item->CanGoInSlot(slot, bypass) || !CGCharacterInfo::CanEquipItem(item, slot)) {
            return;
        }

        tableIndex = -3;
    }

    lua_checkstack(L, 2);
    lua_pushnumber(L, static_cast<uint32_t>(location + 1));
    lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
    lua_settable(L, tableIndex);
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
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetInventoryItemsForSlot(slot, returnTable, [\"transmogrify\"])");
            return 0;
        }

        const char* name = lua_tolstring(L, 1, NULL);
        int32_t count = g_paperDollItemFrameDB.m_numRecords;

        for (int32_t i = 0; i < count; i++) {
            PaperDollItemFrameRec* rec = g_paperDollItemFrameDB.GetRecordByIndex(i);

            if (rec && !SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                slot = rec->m_slotNumber;
            }
        }

        if (!slot) {
            luaL_error(L, "Usage: GetInventoryItemsForSlot(slot, returnTable, [\"transmogrify\"])");
            return 0;
        }
    }

    if (lua_type(L, 2) != LUA_TTABLE) {
        luaL_error(L, "Usage: GetInventoryItemsForSlot(slot, returnTable, [\"transmogrify\"])");
        return 0;
    }

    bool transmogrify = false;

    if (lua_isstring(L, 3) && !SStrCmpI(lua_tolstring(L, 3, NULL), "transmogrify", STORM_MAX_STR)) {
        transmogrify = true;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    const CGBag_C* inventory = static_cast<const CGPlayer_C*>(player)->GetBag();
    ItemRecSparse* targetStats = NULL;
    CGItem_C* target = NULL;
    uint8_t bypass = 0;

    if (transmogrify) {
        target = const_cast<CGBag_C*>(inventory)->GetItemPointer(slot);

        if (!target) {
            return 0;
        }

        targetStats = target->GetStats();
    } else {
        ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);

        if (!chrClass) {
            return 0;
        }

        bypass = (chrClass->m_flags & 0x8) != 0;
    }

    ClientItemRec targetRec(targetStats);

    for (int32_t i = 0; i <= 18; i++) {
        CGItem_C* item = const_cast<CGBag_C*>(inventory)->GetItemPointer(i);

        if (item) {
            AddInventoryItemForSlot(item, L, i + 0x100000, slot, bypass, transmogrify, target, &targetRec);
        }
    }

    for (int32_t i = 23; i <= 38; i++) {
        CGItem_C* item = const_cast<CGBag_C*>(inventory)->GetItemPointer(i);

        if (item) {
            AddInventoryItemForSlot(item, L, i + 0x2FFFE9, slot, bypass, transmogrify, target, &targetRec);
        }
    }

    for (uint32_t bagIndex = 0; bagIndex < 4; bagIndex++) {
        CGObject_C* container = ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(bagIndex), TYPEMASK_CONTAINER, __FILE__, __LINE__);

        if (!container) {
            continue;
        }

        int32_t location = ((bagIndex + 1) << 8) | 0x300000;
        CGBag_C* bag = container->GetBag();

        for (int32_t i = 0; i <= bag->m_numSlots; i++) {
            CGItem_C* item = bag->GetItemPointer(i);

            if (item) {
                AddInventoryItemForSlot(item, L, i + location, slot, bypass, transmogrify, target, &targetRec);
            }
        }
    }

    if (CGBankInfo::m_banker && !transmogrify) {
        for (int32_t i = 39; i <= 66; i++) {
            CGItem_C* item = const_cast<CGBag_C*>(inventory)->GetItemPointer(i);

            if (item) {
                AddInventoryItemForSlot(item, L, i + 0x400000, slot, bypass, false, target, &targetRec);
            }
        }

        for (uint32_t bagIndex = 4; bagIndex < 11; bagIndex++) {
            CGObject_C* container = ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(bagIndex), TYPEMASK_CONTAINER, __FILE__, __LINE__);

            if (!container) {
                continue;
            }

            int32_t location = ((bagIndex - 3) | 0x6000) << 8;
            CGBag_C* bag = container->GetBag();

            for (int32_t i = 0; i <= bag->m_numSlots; i++) {
                CGItem_C* item = bag->GetItemPointer(i);

                if (item) {
                    AddInventoryItemForSlot(item, L, i + location, slot, bypass, false, target, &targetRec);
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

    if (CGCharacterInfo::CanInspect(unit)) {
        CGItem_C* item = unit->m_bag.GetItemPointer(slot);

        if (item) {
            int32_t id = item->m_obj->m_entryID;
            WOWGUID none;
            none.m_guid = 0;
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);

            if (stats) {
                lua_pushnumber(L, stats->m_inventoryType ? stats->m_quality : -1);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0097D5A0/0x1406F71B0
static int32_t Script_GetInventoryItemDurability(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInventoryItemDurability(slot)");
        return 0;
    }

    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGItem_C* item = player->m_bag.GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    uint32_t maxDurability = item->GetMaxDurability();
    uint32_t durability = item->GetDurability();

    if (!maxDurability) {
        return 0;
    }

    lua_pushnumber(L, durability);
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

    uint64_t guid = unit->m_obj->m_guid.m_guid;

    if (guid == ClntObjMgrGetActivePlayer().m_guid) {
        CGItem_C* item = unit->m_bag.GetItemPointer(slot);

        if (item) {
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
            return 1;
        }
    } else {
        InspectItem* inspected = unit->GetInspectedItem(slot);

        if (inspected && inspected->m_itemId) {
            int32_t id = inspected->m_itemId;
            WOWGUID none;
            none.m_guid = 0;
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);

            if (stats) {
                lua_pushstring(L, CGTooltip::GetItemHyperlinkString(inspected->m_itemId, stats->m_quality, inspected->m_enchants[0], &inspected->m_enchants[2], static_cast<uint16_t>(inspected->m_randomPropertiesId), inspected->m_propertySeed, inspected->m_enchants[8]));
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

    uint64_t guid = unit->m_obj->m_guid.m_guid;

    if (guid == ClntObjMgrGetActivePlayer().m_guid) {
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
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInventoryItemGems(slot)");
        return 0;
    }

    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot)) {
        luaL_error(L, "Usage: GetInventoryItemGems(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

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

    if (ResolveInventorySlotArg(L, 1, slot)) {
        WOWGUID target;

        if (Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true, false)) {
            CGCharacterInfo::UseItem(slot, &target);
        }
    }

    return 0;
}

// OFFSET: 0x0097D880/0x1406F74A0
static int32_t Script_SocketInventoryItem(lua_State* L) {
    int32_t slot = 0;

    if (ResolveInventorySlotArg(L, 1, slot)) {
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
    if (!Spell_C_IsTargeting() && CursorGetResetMode() == 1) {
        int32_t slot = 0;

        if (ResolveInventorySlotArg(L, 1, slot)) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                CGItem_C* item = player->GetBag()->GetItemPointer(slot);

                if (item && !(item->m_itemFlags & 0x1)) {
                    CursorSetMode(3);
                }
            }
        }
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

    if (ResolveInventorySlotArg(L, 3, slot)) {
        CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 2, NULL));

        if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            CGItem_C* item = static_cast<CGPlayer_C*>(unit)->m_bag.GetItemPointer(slot);

            if (item) {
                const char* path = ClientDBStringLookup(3);
                char buffer[260];
                SStrPrintf(buffer, sizeof(buffer), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
                SetPortraitTexture(texture, buffer);
            }
        }
    }

    return 0;
}

// OFFSET: 0x0097C080/0x1406F57F0
static int32_t Script_RequestInspectHonorData(lua_State* L) {
    CGCharacterInfo::RequestInspectHonorData();
    return 0;
}

// OFFSET: 0x0097C2F0/0x1406F5AC0
static int32_t Script_GetInspectGuildInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
    lua_pushnumber(L, static_cast<int32_t>(player->GetInspectGuildGUID()));
    lua_pushnumber(L, static_cast<int32_t>(player->GetInspectGuildLevel()));
    lua_pushnumber(L, static_cast<int32_t>(player->GetInspectGuildXP()));
    lua_pushnumber(L, static_cast<int32_t>(player->GetInspectGuildNumMembers()));
    return 4;
}

// OFFSET: 0x0097EE60
static int32_t Script_GetAverageItemLevel(lua_State* L) {
    if (CGCharacterInfo::m_avgItemLevelDirty) {
        float total = CGCharacterInfo::CalculateAverageItemLevel(false);
        float equipped = CGCharacterInfo::CalculateAverageItemLevel(true);

        if (!CGCharacterInfo::m_avgItemLevelPending) {
            CGCharacterInfo::m_avgItemLevel = total;
            CGCharacterInfo::m_avgItemLevelEquipped = equipped;
        }

        CGCharacterInfo::m_avgItemLevelDirty = 0;
    }

    lua_pushnumber(L, CGCharacterInfo::m_avgItemLevel);

    if (CGCharacterInfo::m_avgItemLevelDirty) {
        float total = CGCharacterInfo::CalculateAverageItemLevel(false);
        float equipped = CGCharacterInfo::CalculateAverageItemLevel(true);

        if (!CGCharacterInfo::m_avgItemLevelPending) {
            CGCharacterInfo::m_avgItemLevel = total;
            CGCharacterInfo::m_avgItemLevelEquipped = equipped;
        }

        CGCharacterInfo::m_avgItemLevelDirty = 0;
    }

    lua_pushnumber(L, CGCharacterInfo::m_avgItemLevelEquipped);
    return 2;
}

// OFFSET: 0x0097B550
static int32_t Script_GetInspectRatedBGData(lua_State* L) {
    lua_pushnumber(L, CGCharacterInfo::m_inspectRatedBGRating);
    lua_pushnumber(L, CGCharacterInfo::m_inspectRatedBGPlayed);
    lua_pushnumber(L, CGCharacterInfo::m_inspectRatedBGWon);
    return 3;
}

// STATIC: 0x00D13670
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
    { "GetInspectGuildInfo", &Script_GetInspectGuildInfo },
    { "GetAverageItemLevel", &Script_GetAverageItemLevel },
    { "GetInspectRatedBGData", &Script_GetInspectRatedBGData },
};

// OFFSET: 0x0097B5D0
void CharacterInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0097B600
void CharacterInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
