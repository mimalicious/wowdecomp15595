#include "LFGInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonExpansionRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"

// STATIC: 0x00D137A8
static const char* s_lfgTypeTokens[7] = {
    "LFG_TYPE_NONE",
    "LFG_TYPE_DUNGEON",
    "LFG_TYPE_RAID",
    "LFG_TYPE_QUEST",
    "LFG_TYPE_ZONE",
    "LFG_TYPE_RANDOM_DUNGEON",
};

// STATIC: 0x00F4AD88
static char s_completionRewardTexture[260];

// STATIC: 0x00F4AEB8
static char s_rewardItemTexture[260];

// STATIC: 0x00F4AFC0
static char s_shortageRewardTexture[260];

// OFFSET: 0x00985350/0x140700500
LFGDungeonEntry* CGLookingForGroup::GetDungeonRewardInfo(uint32_t dungeonKey) {
    if (CGLookingForGroup::m_playerLockInfo.m_valid) {
        for (LFGDungeonEntry* entry = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); !CGLookingForGroup::m_playerLockInfo.m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
            if (entry->m_slot == dungeonKey) {
                return entry;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x00985390/0x140700540
LFG_SLOT_INFO_LOOT* CGLookingForGroup::GetDungeonReward(uint32_t dungeonKey, uint32_t index) {
    LFGDungeonEntry* entry = NULL;

    if (CGLookingForGroup::m_playerLockInfo.m_valid) {
        for (LFGDungeonEntry* dungeon = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); !CGLookingForGroup::m_playerLockInfo.m_dungeons.IsEnd(dungeon); dungeon = dungeon->RawNext()) {
            if (dungeon->m_slot == dungeonKey) {
                entry = dungeon;
                break;
            }
        }
    }

    if (!entry || index >= entry->m_rewards.Count()) {
        return NULL;
    }

    return &entry->m_rewards.m_data[index];
}

// OFFSET: 0x00980F40/0x1406FB440
static bool CanPartyLFGBackfill() {
    int32_t members = CGRaidInfo::m_realNumMembers;

    if (!members) {
        members = CGPartyInfo::m_realNumMembers;

        if (!members && !CGPartyInfo::m_onePersonParty) {
            return false;
        }

        members++;
    }

    if (CGPartyInfo::m_lfgSlot && !(CGPartyInfo::m_playerLFGFlags & 0x2) && !CGPartyInfo::m_lfgUnk) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGPartyInfo::m_lfgSlot & 0xFFFFFF);

        if (rec && members < rec->GetGroupSize()) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009861A0/0x1407015B0
static int32_t PushAuraExpiration(lua_State* L, int32_t spellID) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t index = player->GetAuraIndex(spellID);

        if (index != -1) {
            lua_pushnumber(L, static_cast<uint32_t>(player->GetAura(index)->m_endTime) * 0.001);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x009833A0/0x1406FE0D0
static int32_t PushUnitHasAura(lua_State* L, int32_t spellID) {
    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));

    if (guid) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            lua_pushboolean(L, unit->HasAura(spellID, WOWGUID()));
            return 1;
        }

        PartyMemberState* state = CGGameUI::GetPartyMemberState(guid);

        if (state) {
            for (uint32_t i = 0; i < state->m_numAuras; i++) {
                if (state->m_auras.m_data[i].m_spellId == spellID) {
                    lua_pushboolean(L, 1);
                    return 1;
                }
            }

            lua_pushboolean(L, 0);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x009828E0/0x1406FD440
static int32_t PushLockReasonArgs(lua_State* L, LFGDungeonEntry* entry) {
    switch (entry->m_lockReason) {
        case 9: {
            AreaTableRec* area = g_areaTableDB.GetRecord(entry->unk_004C);

            if (area) {
                lua_pushstring(L, area->m_areaName);
                return 1;
            }

            lua_pushstring(L, FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
            return 1;
        }

        case 4:
        case 5:
            lua_pushnumber(L, entry->unk_004C);
            lua_pushnumber(L, entry->unk_0050);
            return 2;

        default:
            return 0;
    }
}

// PROVEN: a5d876ec IDENTICAL/NONE
// OFFSET: 0x00981850/0x1406FC0F0
static int32_t Script_GetLFGRoles(lua_State* L) {
    if (s_cvLfgSelectedRoles) {
        int32_t roles = s_cvLfgSelectedRoles->m_intValue;
        lua_pushboolean(L, roles & 1);
        lua_pushboolean(L, roles & 2);
        lua_pushboolean(L, roles & 4);
        lua_pushboolean(L, roles & 8);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 4;
}

// OFFSET: 0x00986120/0x1407014F0
static int32_t Script_GetLFGRandomCooldownExpiration(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t index = player->GetAuraIndex(71328);

        if (index != -1) {
            lua_pushnumber(L, static_cast<uint32_t>(player->GetAura(index)->m_endTime) * 0.001);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00986220/0x140701680
static int32_t Script_GetLFGDeserterExpiration(lua_State* L) {
    return PushAuraExpiration(L, 71041);
}

// OFFSET: 0x0097F4A0/0x1406F9590
static int32_t Script_SetLFGRoles(lua_State* L) {
    uint8_t roles = 0;

    if (StringToBOOL(L, 1, 0)) {
        roles = 0x1;
    }

    if (StringToBOOL(L, 2, 0)) {
        roles |= 0x2;
    }

    if (StringToBOOL(L, 3, 0)) {
        roles |= 0x4;
    }

    if (StringToBOOL(L, 4, 0)) {
        roles |= 0x8;
    }

    uint8_t classRoles = CGLookingForGroup::GetClassRoles(ClientServices::GetCharacterClass());
    s_cvLfgSelectedRoles->Set(roles & (classRoles | 1), true, false, false, true);
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_UPDATE, NULL);
    return 0;
}

// OFFSET: 0x0097F460/0x1406F9520
static int32_t Script_GetLFGTypes(lua_State* L) {
    lua_checkstack(L, 7);

    for (uint32_t i = 0; i < 7; i++) {
        lua_pushstring(L, FrameScript_GetText(s_lfgTypeTokens[i], -1, GENDER_NOT_APPLICABLE));
    }

    return 7;
}

// OFFSET: 0x00988AB0/0x1407051F0
static void RandomDungeonRewardItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        WOWGUID none;
        none.m_guid = 0;

        if (g_itemSparseDB.GetRecord(id, none, NULL, NULL, false)) {
            FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE_RANDOM_INFO, NULL);
        }
    }
}

// OFFSET: 0x0097F010/0x1406F9160
static int32_t Script_SearchLFGGetJoinedID(lua_State* L) {
    int32_t id = CGLookingForGroup::m_searchJoinedID & 0xFFFFFF;

    if (id) {
        lua_pushnumber(L, id);
        return 1;
    }

    return 0;
}

// OFFSET: 0x0097F5E0/0x1406F96F0
static int32_t Script_GetRandomDungeonBestChoice(lua_State* L) {
    if (CGLookingForGroup::m_randomDungeonBestChoice) {
        lua_pushnumber(L, CGLookingForGroup::m_randomDungeonBestChoice & 0xFFFFFF);
        return 1;
    }

    return 0;
}

// OFFSET: 0x00982B30/0x1406FD710
static int32_t Script_GetNumRandomDungeons(lua_State* L) {
    lua_pushnumber(L, CGLookingForGroup::m_randomDungeons.Count());
    return 1;
}

// OFFSET: 0x009831D0/0x1406FDEC0
static int32_t Script_CanPartyLFGBackfill(lua_State* L) {
    lua_pushboolean(L, CanPartyLFGBackfill());
    return 1;
}

// OFFSET: 0x009831F0/0x1406FDEF0
static int32_t Script_GetPartyLFGBackfillInfo(lua_State* L) {
    if (CanPartyLFGBackfill()) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGPartyInfo::m_lfgSlot & 0xFFFFFF);
        lua_pushstring(L, rec->m_name);
        lua_pushnumber(L, rec->m_id);
        lua_pushnumber(L, rec->m_typeID);
        lua_pushnumber(L, GetLFGDungeonSubtype(rec));
        return 4;
    }

    return 0;
}

// OFFSET: 0x00980FC0/0x1406FB4D0
static int32_t Script_SearchLFGGetNumResults(lua_State* L) {
    double numPlayers;

    if (CGLookingForGroup::m_searchResults.m_lfgID) {
        lua_pushnumber(L, CGLookingForGroup::m_searchResults.m_playerList.m_count + CGLookingForGroup::m_searchResults.m_partyList.m_count);
        numPlayers = CGLookingForGroup::m_searchResults.m_totalParties + CGLookingForGroup::m_searchResults.m_totalPlayers;
    } else {
        lua_pushnumber(L, 0.0);
        numPlayers = 0.0;
    }

    lua_pushnumber(L, numPlayers);
    return 2;
}

// OFFSET: 0x00982660/0x1406FD120
static int32_t Script_GetLFDChoiceOrder(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDChoiceOrder([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    } else {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    }

    uint32_t count = CGLookingForGroup::m_lfdEntries.Count();
    uint32_t key = 1;

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfdEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfdEntries.m_data[i];
            lua_pushnumber(L, key++);
            lua_pushnumber(L, entry.m_id);
            lua_settable(L, -3);
        }
    }

    return 1;
}

// OFFSET: 0x009827A0/0x1406FD2B0
static int32_t Script_GetLFRChoiceOrder(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFRChoiceOrder([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    } else {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    }

    uint32_t count = CGLookingForGroup::m_lfrEntries.Count();
    uint32_t key = 1;

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfrEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfrEntries.m_data[i];
            lua_pushnumber(L, key++);
            lua_pushnumber(L, entry.m_id);
            lua_settable(L, -3);
        }
    }

    return 1;
}

// PROVEN: 768ebc7d IDENTICAL/NONE
// OFFSET: 0x0097F540/0x1406F9650
static int32_t Script_GetLFGInfoLocal(lua_State* L) {
    lua_pushboolean(L, CGLookingForGroup::m_localInfo.m_joined);
    lua_pushboolean(L, CGLookingForGroup::m_localInfo.unk_0004);
    lua_pushboolean(L, CGLookingForGroup::m_localInfo.unk_0005);
    lua_pushstring(L, CGLookingForGroup::m_localInfo.m_comment);
    lua_pushnumber(L, CGLookingForGroup::m_localInfo.m_numDungeons);

    for (uint32_t i = 0; i < 3; i++) {
        lua_pushnumber(L, CGLookingForGroup::m_localInfo.m_roles[i]);
    }

    return 8;
}

// OFFSET: 0x00985440/0x140700590
static int32_t Script_GetLFGInfoServer(lua_State* L) {
    LFGInfo* info = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;
    lua_pushboolean(L, CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers);
    lua_pushboolean(L, info->m_joined);
    lua_pushboolean(L, info->m_queued);
    lua_pushboolean(L, info->unk_0004);
    lua_pushboolean(L, info->unk_0005);
    lua_pushstring(L, info->m_comment);
    lua_pushnumber(L, info->m_numDungeons);
    lua_pushboolean(L, CGLookingForGroup::HasRFDungeon(info));

    for (uint32_t i = 0; i < 3; i++) {
        lua_pushnumber(L, info->m_roles[i]);
    }

    return 11;
}

// OFFSET: 0x00981910/0x1406FC1D0
static int32_t Script_GetLFGQueueStats(lua_State* L) {
    LFGInfo* info = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;

    if (!info->m_joined || !info->m_queued || !CGLookingForGroup::m_queueStatus.m_lfgID) {
        return 0;
    }

    lua_pushboolean(L, 1);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_tanksNeeded);
    lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_healersNeeded);
    lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_damageNeeded);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_queueStatus.m_lfgID);

    if (rec) {
        lua_pushnumber(L, rec->m_countTank);
        lua_pushnumber(L, rec->m_countHealer);
        lua_pushnumber(L, rec->m_countDamage);
        lua_pushnumber(L, rec->m_typeID);
        lua_pushnumber(L, GetLFGDungeonSubtype(rec));
        lua_pushstring(L, rec->m_name);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_tankWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_healerWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_damageWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_myWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_averageWait);
        lua_pushnumber(L, static_cast<uint32_t>(CGLookingForGroup::m_queueStatus.m_queuedTime) * 0.001);
        return 17;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 17;
}

// OFFSET: 0x00985530/0x140700690
static int32_t Script_GetLFGQueuedList(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDQueuedList([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    } else {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    }

    LFGInfo* info = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;

    for (LFGDungeonEntry* entry = info->m_dungeons.Head(); !info->m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
        lua_pushnumber(L, entry->m_slot & 0xFFFFFF);
        lua_pushboolean(L, 1);
        lua_settable(L, -3);
    }

    return 1;
}

// OFFSET: 0x009858F0/0x140700AB0
static int32_t Script_GetLFDLockPlayerCount(lua_State* L) {
    if (!CGLookingForGroup::m_playerLockInfo.m_valid) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    if (!CGLookingForGroup::m_partyLockInfo.m_valid) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    int32_t count = 0;

    for (LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.m_members.Head(); !CGLookingForGroup::m_partyLockInfo.m_members.IsEnd(member); member = member->RawNext()) {
        count++;
    }

    lua_pushnumber(L, count + 1);
    return 1;
}

// OFFSET: 0x00988B00/0x140705240
static int32_t Script_GetLFGDungeonRewardInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetLFGDungeonRewardInfo(dungeonID, lootIndex)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec || (rec->m_typeID != 6 && !(rec->m_flags & 0x4) && !rec->IsRaid())) {
        return 0;
    }

    uint32_t slot = (rec->m_typeID << 24) | (dungeonID & 0xFFFFFF);
    LFG_SLOT_INFO_LOOT* item = CGLookingForGroup::GetDungeonReward(slot, lua_tointeger(L, 2) - 1);

    if (!item) {
        return 0;
    }

    const char* path = ClientDBStringLookup(3);

    if (item->m_isCurrency) {
        CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item->m_id);

        if (!currency) {
            return 0;
        }

        SStrPrintf(s_rewardItemTexture, sizeof(s_rewardItemTexture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
        lua_pushstring(L, currency->m_name);
        lua_pushstring(L, s_rewardItemTexture);

        if (currency->m_flags & 0x8) {
            lua_pushinteger(L, item->m_quantity / 100);
        } else {
            lua_pushinteger(L, item->m_quantity);
        }

        return 3;
    }

    const char* name = "";
    WOWGUID none = WOWGUID();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_id, none, RandomDungeonRewardItemStatsCallback, NULL, false);

    if (stats) {
        name = stats->GetName(0);
    }

    SStrPrintf(s_rewardItemTexture, sizeof(s_rewardItemTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayID));
    lua_pushstring(L, name);
    lua_pushstring(L, s_rewardItemTexture);
    lua_pushnumber(L, item->m_quantity);
    return 3;
}

// OFFSET: 0x00982E80/0x1406FDAE0
static int32_t Script_GetLFGCompletionReward(lua_State* L) {
    if (CGLookingForGroup::m_reward.m_actualSlot) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_reward.m_actualSlot & 0xFFFFFF);

        if (!rec) {
            return 0;
        }

        lua_pushstring(L, rec->m_name);
        lua_pushnumber(L, rec->m_typeID);
        lua_pushnumber(L, GetLFGDungeonSubtype(rec));
        lua_pushstring(L, rec->m_textureFilename);
        lua_pushnumber(L, CGLookingForGroup::m_reward.m_baseMoney);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, CGLookingForGroup::m_reward.m_baseXP);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, CGLookingForGroup::m_completionRewards.Count());
        return 10;
    }

    return 0;
}

// OFFSET: 0x00982FD0/0x1406FDC60
static int32_t Script_GetLFGCompletionRewardItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGCompletionRewardItem(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index < CGLookingForGroup::m_completionRewards.Count()) {
        LFG_SLOT_INFO_LOOT* item = &CGLookingForGroup::m_completionRewards.m_data[index];
        const char* path = ClientDBStringLookup(3);

        if (item->m_isCurrency) {
            CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item->m_id);

            if (currency) {
                SStrPrintf(s_completionRewardTexture, sizeof(s_completionRewardTexture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
                lua_pushstring(L, s_completionRewardTexture);

                if (currency->m_flags & 0x8) {
                    lua_pushinteger(L, item->m_quantity / 100);
                } else {
                    lua_pushinteger(L, item->m_quantity);
                }

                return 2;
            }
        } else {
            SStrPrintf(s_completionRewardTexture, sizeof(s_completionRewardTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayID));
            lua_pushstring(L, s_completionRewardTexture);
            lua_pushnumber(L, item->m_quantity);
            return 2;
        }
    }

    return 0;
}

// OFFSET: 0x009856D0/0x140700860
static int32_t Script_JoinLFG(lua_State* L) {
    if (!CGLookingForGroup::m_localInfo.m_numDungeons) {
        CGGameUI::DisplayError(ERR_LFG_NO_SLOTS_SELECTED);
        return 0;
    }

    if (!s_cvLfgSelectedRoles || !(s_cvLfgSelectedRoles->m_intValue & 0xE)) {
        CGGameUI::DisplayError(ERR_LFG_NO_ROLES_SELECTED);
        return 0;
    }

    CGLookingForGroup::JoinLFG();
    return 0;
}

// OFFSET: 0x00985710/0x1407008B0
static int32_t Script_GetLFDChoiceLockedState(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDChoiceLockedState([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    } else {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    }

    if (CGLookingForGroup::m_partyLockInfo.m_valid) {
        for (LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.m_members.Head(); !CGLookingForGroup::m_partyLockInfo.m_members.IsEnd(member); member = member->RawNext()) {
            for (LFGDungeonEntry* entry = member->m_dungeons.Head(); !member->m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
                if (static_cast<uint8_t>(entry->m_slot >> 24) != 2 && entry->m_locked) {
                    lua_pushnumber(L, entry->m_slot & 0xFFFFFF);
                    lua_pushnumber(L, entry->m_lockReason);
                    lua_settable(L, -3);
                }
            }
        }
    }

    if (CGLookingForGroup::m_playerLockInfo.m_valid) {
        for (LFGDungeonEntry* entry = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); !CGLookingForGroup::m_playerLockInfo.m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
            if (entry->m_locked) {
                lua_pushnumber(L, entry->m_slot & 0xFFFFFF);
                lua_pushnumber(L, entry->m_lockReason);
                lua_settable(L, -3);
            }
        }
    }

    return 1;
}

// OFFSET: 0x00986FB0/0x140702730
static int32_t Script_IsListedInLFR(lua_State* L) {
    LFGInfo* info = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;
    lua_pushboolean(L, CGLookingForGroup::HasRaidDungeon(info));
    return 1;
}

// OFFSET: 0x009820E0
static int32_t Script_GetLFGRoleUpdate(lua_State* L) {
    return CGLookingForGroup::GetLFGRoleUpdate(L);
}

// OFFSET: 0x009821C0/0x1406FCBD0
static int32_t Script_GetLFGRoleUpdateSlot(lua_State* L) {
    return CGLookingForGroup::GetLFGRoleUpdateSlot(L);
}

// OFFSET: 0x00981B50/0x1406FC460
int32_t CGLookingForGroup::GetLFGProposal(lua_State* L) {
    if (!CGLookingForGroup::m_proposal.m_id) {
        return 0;
    }

    LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_proposal.m_id & 0xFFFFFF);

    if (!dungeon) {
        return 0;
    }

    lua_pushboolean(L, 1);
    lua_pushnumber(L, dungeon->m_id);
    lua_pushnumber(L, dungeon->m_typeID);
    lua_pushnumber(L, GetLFGDungeonSubtype(dungeon));
    lua_pushstring(L, dungeon->m_name);
    lua_pushstring(L, dungeon->m_textureFilename);
    uint32_t roles = 0;
    uint32_t accepted = 0;

    for (uint32_t i = 0; i < CGLookingForGroup::m_proposal.m_numMembers; i++) {
        if (CGLookingForGroup::m_proposal.m_members[i].m_isLocal) {
            roles = CGLookingForGroup::m_proposal.m_members[i].m_roles;
            accepted = CGLookingForGroup::m_proposal.m_members[i].m_accepted;
            break;
        }
    }

    lua_pushstring(L, GetRoleTokenFromFlags(roles));
    lua_pushboolean(L, accepted);
    uint32_t total = 0;
    uint32_t value = 0;

    if (dungeon->m_typeID == 6) {
        total = 0;
        value = CGLookingForGroup::m_proposal.m_completedMask != 0;
    } else if (dungeon->m_flags & 0x4) {
        total = 1;
        value = CGLookingForGroup::m_proposal.m_completedMask != 0;
    } else {
        DungeonEncounterCountMatchingRows(dungeon->m_mapID, dungeon->m_difficulty, CGLookingForGroup::m_proposal.m_completedMask, &total, &value);
    }

    lua_pushnumber(L, total);
    lua_pushnumber(L, value);
    lua_pushnumber(L, CGLookingForGroup::m_proposal.m_numMembers);
    lua_pushboolean(L, roles & 0x1);
    lua_pushboolean(L, (static_cast<uint32_t>(dungeon->m_flags) >> 2) & 0x1);
    return 13;
}

// OFFSET: 0x00981D60/0x1406FC710
static int32_t Script_GetLFGProposal(lua_State* L) {
    return CGLookingForGroup::GetLFGProposal(L);
}

// OFFSET: 0x00982B70/0x1406FD740
static int32_t PushDungeonInfo(lua_State* L, int32_t dungeonId) {
    if (dungeonId < 0) {
        LfgDungeonGroupRec* group = g_lfgDungeonGroupDB.GetRecord(-dungeonId);

        if (group) {
            lua_pushstring(L, group->m_name);
            lua_pushnumber(L, group->m_typeID);
            return 2;
        }
    } else {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonId);

        if (rec) {
        LfgDungeonExpansionRec* expansion = GetLfgDungeonExpansionRec(rec->m_id, ClientServices::Connection()->m_accountExpansion);
        MapDifficultyRec* difficulty = GetMapDifficultyRec(rec->m_mapID, rec->m_difficulty, NULL);
        lua_pushstring(L, rec->m_name);
        lua_pushnumber(L, rec->m_typeID);
        lua_pushnumber(L, GetLFGDungeonSubtype(rec));
        lua_pushnumber(L, expansion ? expansion->m_hardLevelMin : rec->m_minLevel);
        lua_pushnumber(L, expansion ? expansion->m_hardLevelMax : rec->m_maxLevel);
        lua_pushnumber(L, rec->m_targetLevel);
        lua_pushnumber(L, expansion ? expansion->m_targetLevelMin : rec->m_targetLevelMin);
        lua_pushnumber(L, expansion ? expansion->m_targetLevelMax : rec->m_targetLevelMax);
        lua_pushnumber(L, rec->m_expansionLevel);
        lua_pushnumber(L, -rec->GetTopGroupID());
        lua_pushstring(L, rec->m_textureFilename);
        lua_pushnumber(L, rec->m_difficulty);

        if (difficulty) {
            lua_pushnumber(L, difficulty->m_maxPlayers);
        } else {
            lua_pushnil(L);
        }

        lua_pushstring(L, rec->m_description);
        lua_pushboolean(L, rec->m_flags & 0x4);
        return 15;
        }
    }

    return 0;
}

// OFFSET: 0x00982DC0/0x1406FDA00
static int32_t Script_GetLFGDungeonInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGDungeonInfo(dungeonID)");
        return 0;
    }

    return PushDungeonInfo(L, lua_tointeger(L, 1));
}

// OFFSET: 0x00985990/0x140700B40
static int32_t Script_GetLFDLockInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetLFDLockInfo(dungeonID, playerIndex)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);

    if (dungeonID < 0) {
        return 0;
    }

    int32_t playerIndex = lua_tointeger(L, 2);
    LFGLockInfo* info = NULL;

    if (playerIndex == 1) {
        if (!CGLookingForGroup::m_playerLockInfo.m_valid) {
            return 0;
        }

        info = &CGLookingForGroup::m_playerLockInfo;
    } else {
        int32_t remaining = playerIndex - 1;

        if (!CGLookingForGroup::m_partyLockInfo.m_valid) {
            return 0;
        }

        for (LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.m_members.Head(); !CGLookingForGroup::m_partyLockInfo.m_members.IsEnd(member); member = member->RawNext()) {
            if (!--remaining) {
                info = member;
            }
        }

        if (!info) {
            return 0;
        }
    }

    WOWGUID none = WOWGUID();
    NameCache* record = g_nameDBCache.GetRecord(info->m_guid.m_guid, none, NULL, NULL, false);
    const char* name = record ? record->m_name : FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
    lua_pushstring(L, name);

    for (LFGDungeonEntry* entry = info->m_dungeons.Head(); !info->m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
        if ((entry->m_slot & 0xFFFFFF) == static_cast<uint32_t>(dungeonID)) {
            if (entry->m_locked) {
                lua_pushnumber(L, entry->m_lockReason);
                return PushLockReasonArgs(L, entry) + 2;
            }

            break;
        }
    }

    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x00985B00/0x140700D00
static int32_t Script_GetLFGDungeonRewards(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGDungeonRewards(dungeonID)");
        return 0;
    }

    int32_t dungeonId = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonId);

    if (!rec) {
        return 0;
    }

    LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((rec->m_typeID << 24) | (dungeonId & 0xFFFFFF));

    if (entry) {
        lua_pushboolean(L, entry->m_rewardDone == 0);
        lua_pushnumber(L, entry->m_baseMoney);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, entry->m_baseXP);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, entry->m_rewards.Count());
    } else {
        lua_pushboolean(L, 0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 6;
}

// OFFSET: 0x00982E00/0x1406FDA50
static int32_t Script_GetLFGRandomDungeonInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGRandomDungeonInfo(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index >= CGLookingForGroup::m_randomDungeons.m_count) {
        return 0;
    }

    uint32_t dungeonId = CGLookingForGroup::m_randomDungeons.m_data[index] & 0xFFFFFF;
    lua_pushnumber(L, dungeonId);
    return PushDungeonInfo(L, dungeonId) + 1;
}

// OFFSET: 0x00987BC0/0x140703B20
static int32_t Script_ClearAllLFGDungeons(lua_State* L) {
    CGLookingForGroup::ClearDungeonList();
    return 0;
}

// OFFSET: 0x00987A50/0x1407039D0
static int32_t Script_SetLFGDungeon(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetLFGDungeon(type)");
        return 0;
    }

    int32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(id);

    if (rec) {
        CGLookingForGroup::SetLFGDungeon((id & 0xFFFFFF) | (rec->m_typeID << 24));
    }

    return 0;
}

// OFFSET: 0x00986F00/0x140702610
static int32_t Script_IsLFGDungeonJoinable(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsLFGDungeonJoinable(dungeonID)");
        return 0;
    }

    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(lua_tointeger(L, 1));

    if (rec) {
        lua_pushboolean(L, CGLookingForGroup::IsSlotValidForAll((rec->m_typeID << 24) | (rec->m_id & 0xFFFFFF)));
        lua_pushboolean(L, CGLookingForGroup::IsSlotValidForPlayer((rec->m_typeID << 24) | (rec->m_id & 0xFFFFFF)));
        return 2;
    }

    return 0;
}

// OFFSET: 0x009837C0/0x1406FE5A0
static int32_t Script_GetLFDChoiceCollapseState(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDChoiceCollapseState([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    } else {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    }

    uint32_t count;
    count = CGLookingForGroup::m_lfdEntries.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfdEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfdEntries.m_data[i];
            lua_pushnumber(L, entry.m_id);
            bool collapsed;

            if (entry.m_id >= 0) {
                if (s_cvLfdCollapsedHeaders) {
                    CVarArrayWrapper wrapper(s_cvLfdCollapsedHeaders);
                    collapsed = wrapper.RawGetValue(entry.m_order) != wrapper.m_defaultValue;
                } else {
                    collapsed = false;
                }
            } else {
                uint32_t header = -entry.m_id;

                if (s_cvLfdCollapsedHeaders) {
                    CVarArrayWrapper wrapper(s_cvLfdCollapsedHeaders);
                    collapsed = wrapper.RawGetValue(header) != wrapper.m_defaultValue;
                } else {
                    collapsed = false;
                }
            }

            lua_pushboolean(L, collapsed);
            lua_settable(L, -3);
        } else {
            DebugLog("This shouldn't happen...");
        }
    }

    count = CGLookingForGroup::m_lfrEntries.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfrEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfrEntries.m_data[i];
            lua_pushnumber(L, entry.m_id);
            bool collapsed;

            if (entry.m_id >= 0) {
                if (s_cvLfdCollapsedHeaders) {
                    CVarArrayWrapper wrapper(s_cvLfdCollapsedHeaders);
                    collapsed = wrapper.RawGetValue(entry.m_order) != wrapper.m_defaultValue;
                } else {
                    collapsed = false;
                }
            } else {
                uint32_t header = -entry.m_id;

                if (s_cvLfdCollapsedHeaders) {
                    CVarArrayWrapper wrapper(s_cvLfdCollapsedHeaders);
                    collapsed = wrapper.RawGetValue(header) != wrapper.m_defaultValue;
                } else {
                    collapsed = false;
                }
            }

            lua_pushboolean(L, collapsed);
            lua_settable(L, -3);
        } else {
            DebugLog("This shouldn't happen...");
        }
    }

    return 1;
}

// OFFSET: 0x00983A60/0x1406FE940
static int32_t Script_GetLFDChoiceEnabledState(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDChoiceEnabledState([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    } else {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    }

    uint32_t count;
    count = CGLookingForGroup::m_lfdEntries.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfdEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfdEntries.m_data[i];

            if (entry.m_id > 0 && s_cvLfdSelectedDungeons) {
                CVarArrayWrapper wrapper(s_cvLfdSelectedDungeons);
                bool enabled = wrapper.RawGetValue(entry.m_id) != wrapper.m_defaultValue;

                if (enabled) {
                    lua_pushnumber(L, entry.m_id);
                    lua_pushboolean(L, 1);
                    lua_settable(L, -3);
                }
            }
        } else {
            DebugLog("This shouldn't happen...");
        }
    }

    count = CGLookingForGroup::m_lfrEntries.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfrEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfrEntries.m_data[i];

            if (entry.m_id > 0 && s_cvLfdSelectedDungeons) {
                CVarArrayWrapper wrapper(s_cvLfdSelectedDungeons);
                bool enabled = wrapper.RawGetValue(entry.m_id) != wrapper.m_defaultValue;

                if (enabled) {
                    lua_pushnumber(L, entry.m_id);
                    lua_pushboolean(L, 1);
                    lua_settable(L, -3);
                }
            }
        } else {
            DebugLog("This shouldn't happen...");
        }
    }

    return 1;
}

// OFFSET: 0x00982980/0x1406FD530
static int32_t Script_RequestLFDPlayerLockInfo(lua_State* L) {
    static ThrottleObject s_shortThrottle(1, 1);
    static ThrottleObject s_longThrottle(2, 10);

    if (s_shortThrottle.IncrementAndCheck() && s_longThrottle.IncrementAndCheck()) {
        {
            UserClientLfgLockInfoRequest request;
            request.unk_0010 = true;
            ClientServices::Send(&request);
        }

        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x00982A50/0x1406FD620
static int32_t Script_RequestLFDPartyLockInfo(lua_State* L) {
    static ThrottleObject s_shortThrottle(1, 1);
    static ThrottleObject s_longThrottle(2, 10);

    if (s_shortThrottle.IncrementAndCheck() && s_longThrottle.IncrementAndCheck()) {
        {
            UserClientLfgLockInfoRequest request;
            request.unk_0010 = false;
            ClientServices::Send(&request);
        }

        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// PROVEN: 17860c07 IDENTICAL/NONE
// OFFSET: 0x009818D0/0x1406FC180
static int32_t Script_SetLFGComment(lua_State* L) {
    const char* comment = NULL;

    if (lua_isstring(L, 1)) {
        comment = lua_tolstring(L, 1, NULL);
    }

    CGLookingForGroup::SetLFGComment(comment);
    return 0;
}

// OFFSET: 0x009832B0/0x1406FDFE0
static int32_t Script_UnitHasLFGRandomCooldown(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHasLFGRandomCooldown(\"unit\")");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));

    if (guid) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            lua_pushboolean(L, unit->HasAura(71328, WOWGUID()));
            return 1;
        }

        PartyMemberState* state = CGGameUI::GetPartyMemberState(guid);

        if (state) {
            for (uint32_t i = 0; i < state->m_numAuras; i++) {
                if (state->m_auras.m_data[i].m_spellId == 71328) {
                    lua_pushboolean(L, 1);
                    return 1;
                }
            }

            lua_pushboolean(L, 0);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00983460/0x1406FE1C0
static int32_t Script_UnitHasLFGDeserter(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHasLFGDeserter(\"unit\")");
        return 0;
    }

    return PushUnitHasAura(L, 71041);
}

// OFFSET: 0x009884A0/0x1407048C0
static int32_t Script_SearchLFGLeave(lua_State* L) {
    if (CGGameUI::CanPerformAction(9)) {
        CGLookingForGroup::LeaveLFGSearch();
    }

    return 0;
}

// OFFSET: 0x0097F050/0x1406F9190
static int32_t AddLFGPlayerInfo(lua_State* L, const LFGListPlayer* player) {
    if (player) {
        lua_pushboolean(L, player->m_roles & 0x1);
        lua_pushboolean(L, player->m_roles & 0x2);
        lua_pushboolean(L, player->m_roles & 0x4);
        lua_pushboolean(L, player->m_roles & 0x8);
        lua_pushnumber(L, player->unk_0064);
        lua_pushnumber(L, player->m_talents[0]);
        lua_pushnumber(L, player->m_talents[1]);
        lua_pushnumber(L, player->m_talents[2]);
        lua_pushboolean(L, player->m_leader);
        lua_pushnumber(L, player->m_armor);
        lua_pushnumber(L, player->m_spellDamage);
        lua_pushnumber(L, player->m_spellHeal);
        lua_pushnumber(L, player->m_critMelee);
        lua_pushnumber(L, player->m_critRanged);
        lua_pushnumber(L, player->m_critSpell);
        lua_pushnumber(L, static_cast<uint32_t>(player->m_mp5));
        lua_pushnumber(L, static_cast<uint32_t>(player->m_mp5Combat));
        lua_pushnumber(L, player->m_attackPower);
        lua_pushnumber(L, player->m_agility);
        lua_pushnumber(L, player->m_maxHealth);
        lua_pushnumber(L, player->m_maxMana);
        lua_pushnumber(L, static_cast<uint32_t>(player->m_gearRating));
        lua_pushnumber(L, player->m_avgItemLevel);
        lua_pushnumber(L, player->m_defenseSkill);
        lua_pushnumber(L, player->m_dodge);
        lua_pushnumber(L, player->m_block);
        lua_pushnumber(L, player->m_parry);
        lua_pushnumber(L, player->m_haste);
        lua_pushnumber(L, player->m_expertise);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushboolean(L, 0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 29;
}

// OFFSET: 0x00981E50/0x1406FC800
static int32_t Script_GetLFGProposalMember(lua_State* L) {
    return CGLookingForGroup::GetLFGProposalMember(L);
}

// OFFSET: 0x00981380/0x1406FBA90
static int32_t Script_SearchLFGGetEncounterResults(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SearchLFGGetEncounterResults(index, encounterIndex)");
        return 0;
    }

    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_searchResults.m_lfgID & 0xFFFFFF);

    if (!rec) {
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    double encounter = lua_tonumber(L, 2);
    const LFGListPlayer* player = NULL;
    const LFGListParty* party = NULL;

    if (!CGLookingForGroup::GetLFGListEntryDisplayed(index - 1, party, player)) {
        return 0;
    }

    if (!player) {
        return PushDungeonEncounterInfo(L, 0, 0, 0, 0, 0);
    }

    uint32_t mask = party ? party->m_encounterMask : player->m_encounterMask;
    return PushDungeonEncounterInfo(L, rec->m_mapID, rec->m_difficulty, static_cast<int32_t>(encounter - 1.0), mask, 0);
}

// OFFSET: 0x00981760/0x1406FBFD0
static int32_t Script_CompleteLFGRoleCheck(lua_State* L) {
    if (!s_cvLfgSelectedRoles) {
        return 0;
    }

    uint8_t roles = 0;

    if (StringToBOOL(L, 1, 0)) {
        roles = static_cast<uint8_t>(s_cvLfgSelectedRoles->m_intValue);
        roles &= CGLookingForGroup::GetClassRoles(ClientServices::GetCharacterClass()) | 1;

        if (!(roles & 0xE)) {
            CGGameUI::DisplayError(ERR_LFG_NO_ROLES_SELECTED);
            lua_pushboolean(L, 0);
            return 1;
        }
    }

    static ThrottleObject s_throttle(3, 10);

    if (!s_throttle.IncrementAndCheck()) {
        return 0;
    }

    UserClientLfgSetRoles request;
    request.unk_0010 = roles;
    ClientServices::Send(&request);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x00981B40/0x1406FC450
static int32_t Script_LeaveLFG(lua_State* L) {
    CGLookingForGroup::LeaveLFG();
    return 0;
}

// OFFSET: 0x00981F20/0x1406FC8D0
static int32_t Script_GetLFGProposalEncounter(lua_State* L) {
    return CGLookingForGroup::GetLFGProposalEncounter(L);
}

// OFFSET: 0x00982040/0x1406FCA40
static int32_t Script_AcceptProposal(lua_State* L) {
    if (CGGameUI::CanPerformAction(9)) {
        CGLookingForGroup::ProposalResponse(true);
    }

    return 0;
}

// OFFSET: 0x00982060/0x1406FCA60
static int32_t Script_RejectProposal(lua_State* L) {
    CGLookingForGroup::ProposalResponse(false);
    return 0;
}

// OFFSET: 0x00982360/0x1406FCDA0
static int32_t Script_GetLFGRoleUpdateMember(lua_State* L) {
    return CGLookingForGroup::GetLFGRoleUpdateMember(L);
}

// OFFSET: 0x00982510/0x1406FCF70
static int32_t Script_SetLFGBootVote(lua_State* L) {
    return CGLookingForGroup::SetLFGBootVote(L);
}

// OFFSET: 0x00982650/0x1406FD110
static int32_t Script_GetLFGBootProposal(lua_State* L) {
    return CGLookingForGroup::GetLFGBootProposal(L);
}

// OFFSET: 0x00983160/0x1406FDE20
static int32_t Script_IsInLFGDungeon(lua_State* L) {
    if (CGPartyInfo::m_lfgSlot) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGPartyInfo::m_lfgSlot & 0xFFFFFF);

        if (rec) {
            lua_pushboolean(L, rec->m_mapID == CGGameUI::m_iCurrentMapID && rec->m_difficulty == static_cast<int32_t>(CGGameUI::GetInstanceDifficulty()));
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00981050/0x1406FB550
static int32_t Script_SearchLFGGetResults(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SearchLFGGetResults(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tointeger(L, 1)) - 1;
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_searchResults.m_lfgID & 0xFFFFFF);

    if (!rec) {
        return 0;
    }

    const LFGListPlayer* player = NULL;
    const LFGListParty* party = NULL;

    if (!CGLookingForGroup::GetLFGListEntryDisplayed(index, party, player) || !player) {
        return 0;
    }

    NameCache* name = g_nameDBCache.GetRecord(player->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);
    lua_pushstring(L, name ? name->m_name : FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
    lua_pushnumber(L, player->m_level);
    AreaTableRec* area = g_areaTableDB.GetRecord(player->m_areaID);
    lua_pushstring(L, area ? area->m_areaName : FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
    const char* className;

    if (name && (className = CGUnit_C::GetDisplayClassName(NULL, name))) {
        lua_pushstring(L, className);
    } else {
        lua_pushstring(L, FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
    }

    char comment[256];

    if (party) {
        SStrCopy(comment, party->m_comment, sizeof(comment));
    } else {
        SStrCopy(comment, player->m_comment, sizeof(comment));
    }

    ProfanityFilter(comment, false, false);
    lua_pushstring(L, comment);
    lua_pushnumber(L, party ? party->GetNumMembers() : 0);
    lua_pushboolean(L, player->unk_008C != 0);
    ChrClassesRec* classRec;

    if (name && (classRec = g_chrClassesDB.GetRecord(name->m_class)) && classRec->m_filename) {
        lua_pushstring(L, classRec->m_filename);
    } else {
        lua_pushnil(L);
    }

    uint32_t mask = party ? party->m_encounterMask : player->m_encounterMask;
    uint32_t total = 0;
    uint32_t completed = 0;
    DungeonEncounterCountMatchingRows(rec->m_mapID, rec->m_difficulty, mask, &total, &completed);
    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    return AddLFGPlayerInfo(L, player) + 10;
}

// OFFSET: 0x00981500/0x1406FBCC0
static int32_t Script_SearchLFGGetPartyResults(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SearchLFGGetPartyResults(index, partyIndex)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t partyIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    const LFGListParty* party = NULL;
    const LFGListPlayer* leader;

    if (!CGLookingForGroup::GetLFGListEntryDisplayed(index, party, leader)) {
        return 0;
    }

    const LFGListPlayer* player;
    NameCache* name;

    if (party && (player = party->GetMember(partyIndex)) && (name = g_nameDBCache.GetRecord(player->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false))) {
        lua_pushstring(L, name->m_name);
        lua_pushnumber(L, player->m_level);

        if (FriendList::s_friendList->IsIgnored(player->m_key.m_guid, true)) {
            lua_pushstring(L, "ignored");
        } else if (FriendList::s_friendList->IsFriend(player->m_key.m_guid)) {
            lua_pushstring(L, "friend");
        } else {
            lua_pushnil(L);
        }

        const char* className = CGUnit_C::GetDisplayClassName(NULL, name);
        lua_pushstring(L, className ? className : "");
        AreaTableRec* area = g_areaTableDB.GetRecord(player->m_areaID);
        lua_pushstring(L, area ? area->m_areaName : FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
        char comment[256];
        SStrCopy(comment, player->m_comment, sizeof(comment));
        ProfanityFilter(comment, false, false);
        lua_pushstring(L, comment);
        return AddLFGPlayerInfo(L, player) + 6;
    }

    lua_pushnil(L);
    lua_pushstring(L, "");
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return AddLFGPlayerInfo(L, NULL) + 5;
}

// OFFSET: 0x00983C90/0x1406FEBF0
static int32_t Script_SetLFGHeaderCollapsed(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetLFGHeaderCollapsed(headerID, isCollapsed)");
        return 0;
    }

    int32_t header = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t collapsed = StringToBOOL(L, 2, 0);

    if (header < 0) {
        header = -header;

        if (g_lfgDungeonGroupDB.GetRecord(header)) {
            CGLookingForGroup::SetHeaderCollapsed(header, collapsed != 0);
        }
    }

    return 0;
}

// PROVEN: fddce97a IDENTICAL/NONE
// OFFSET: 0x00983D20/0x1406FEC90
static int32_t Script_SetLFGDungeonEnabled(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetLFGDungeonEnabled(dungeonID, isEnabled)");
        return 0;
    }

    int32_t dungeon = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t enabled = StringToBOOL(L, 2, 1);

    if (dungeon > 0) {
        CGLookingForGroup::SetDungeonEnabled(dungeon, enabled != 0);
    }

    return 0;
}

// OFFSET: 0x00983D90/0x1406FED10
static int32_t Script_LFGTeleport(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    static ThrottleObject s_throttle(2, 10);

    if (!s_throttle.IncrementAndCheck()) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (player->IsInCombat()) {
        CGGameUI::DisplayError(ERR_NOT_IN_COMBAT);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (player->IsDeadOrGhost()) {
        CGGameUI::DisplayError(ERR_PLAYER_DEAD);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (player->m_movement->m_flags & 0x800) {
        CGGameUI::DisplayError(ERR_NOT_WHILE_FALLING);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (IsOnTransport(player)) {
        CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
        lua_pushboolean(L, 0);
        return 1;
    }

    SpellShapeshiftFormRec* form;

    if (player->GetShapeshiftForm() && (form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm())) && (form->m_flags & 0x200)) {
        CGGameUI::DisplayError(ERR_PLAYER_DEAD);
        lua_pushboolean(L, 0);
        return 1;
    }

    MirrorTimerInfo* timer = CGGameUI::MirrorTimerGetInfo(0);

    if (timer && timer->m_scale < 0) {
        CGGameUI::DisplayError(ERR_NOT_WHILE_FATIGUED);
        lua_pushboolean(L, 0);
        return 1;
    }

    int32_t out = StringToBOOL(L, 1, 0);
    UserClientLfgTeleport request;
    request.unk_0010 = out != 0;
    ClientServices::Send(&request);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x00988D30/0x140705510
static int32_t Script_GetLFGDungeonRewardLink(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetLFGDungeonRewardLink(dungeonID, lootIndex)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    uint32_t slot = (dungeonID & 0xFFFFFF) | (rec->m_typeID << 24);
    LFG_SLOT_INFO_LOOT* item = CGLookingForGroup::GetDungeonReward(slot, lua_tointeger(L, 2) - 1);

    if (item) {
        int32_t id = item->m_id;
        WOWGUID none;
        none.m_guid = 0;
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);

        if (stats) {
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item->m_id, stats->m_quality, 0, 0, 0, 0));
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x009883C0/0x1407047F0
static int32_t Script_SearchLFGJoin(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SearchLFGJoin(typeID, lfgID)");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(9)) {
        return 0;
    }

    int32_t type = static_cast<uint32_t>(lua_tonumber(L, 1));
    int32_t id = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (type < 7 && g_lfgDungeonsDB.GetRecord(id)) {
        CGLookingForGroup::JoinLFGSearch((type << 24) | (id & 0xFFFFFF));
    }

    return 0;
}

// OFFSET: 0x00987AF0/0x140703A60
static int32_t Script_ClearLFGDungeon(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ClearLFGDungeon(type, id)");
        return 0;
    }

    uint32_t type = static_cast<uint32_t>(lua_tonumber(L, 1));
    int32_t id = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (type < 7 && g_lfgDungeonsDB.GetRecord(id)) {
        CGLookingForGroup::ClearLFGDungeon((type << 24) | (id & 0xFFFFFF));
    }

    return 0;
}

// OFFSET: 0x00985680/0x140700810
static int32_t Script_GetLastQueueStatusIndex(lua_State* L) {
    if (CGLookingForGroup::GetQueueStatusIndex() >= 0) {
        lua_pushnumber(L, CGLookingForGroup::GetQueueStatusIndex() + 1);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00987BD0/0x140703B30
static int32_t Script_PartyLFGStartBackfill(lua_State* L) {
    if (s_cvLfgSelectedRoles && (s_cvLfgSelectedRoles->m_intValue & 0xE)) {
        if (CanPartyLFGBackfill()) {
            CGLookingForGroup::ClearDungeonList();
            CGLookingForGroup::SetLFGDungeon(CGPartyInfo::m_lfgSlot);
            CGLookingForGroup::JoinLFG();
            lua_pushboolean(L, 1);
            return 1;
        }
    } else {
        CGGameUI::DisplayError(ERR_LFG_NO_ROLES_SELECTED);
    }

    return 0;
}

// OFFSET: 0x0098B050/0x140708D40
static int32_t Script_RefreshLFGList(lua_State* L) {
    CGLookingForGroup::m_searchResults.Copy(CGLookingForGroup::m_prevSearchResults);
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
    return 0;
}

// OFFSET: 0x0098B080/0x140708D70
static int32_t Script_SearchLFGSort(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SearchLFGSort(\"type\")");
        return 0;
    }

    int32_t type = 3;
    const char* name = lua_tolstring(L, 1, NULL);

    if (!SStrCmpI(name, "group", STORM_MAX_STR)) {
        CGLookingForGroup::m_unkACD660 = !CGLookingForGroup::m_unkACD660;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
        return 0;
    }

    if (!SStrCmpI(name, "name", STORM_MAX_STR)) {
        type = 3;
    } else if (!SStrCmpI(name, "level", STORM_MAX_STR)) {
        type = 1;
    } else if (!SStrCmpI(name, "class", STORM_MAX_STR)) {
        type = 2;
    } else if (!SStrCmpI(name, "zone", STORM_MAX_STR)) {
        type = 0;
    } else if (!SStrCmpI(name, "tank", STORM_MAX_STR)) {
        type = 4;
    } else if (!SStrCmpI(name, "healer", STORM_MAX_STR)) {
        type = 5;
    } else if (!SStrCmpI(name, "damage", STORM_MAX_STR)) {
        type = 6;
    }

    int32_t index = 0;

    while (CGLookingForGroup::m_unkBEA820[index].m_id != type) {
        if (++index >= 7) {
            CGLookingForGroup::ResortLFGLists();
            return 0;
        }
    }

    int32_t order = CGLookingForGroup::m_unkBEA820[index].m_order;

    if (!index) {
        order = !order;
    }

    for (int32_t i = index; i > 0; i--) {
        CGLookingForGroup::m_unkBEA820[i] = CGLookingForGroup::m_unkBEA820[i - 1];
    }

    CGLookingForGroup::m_unkBEA820[0].m_id = type;
    CGLookingForGroup::m_unkBEA820[0].m_order = order;
    CGLookingForGroup::ResortLFGLists();
    return 0;
}

// OFFSET: 0x00982370/0x1406FCDB0
static int32_t Script_UnitGetAvailableRoles(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitGetAvailableRoles(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    uint32_t classID;

    if (!_stricmp(token, "player")) {
        classID = ClientServices::GetCharacterClass();
    } else {
        WOWGUID guid = Script_GetGUIDFromName(token);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            classID = unit->m_unit->m_unitFieldBytes0[1];
        } else {
            WOWGUID none = WOWGUID();
            NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

            if (!name) {
                return 0;
            }

            classID = name->m_class;
        }
    }

    if (classID) {
        uint8_t roles = CGLookingForGroup::GetClassRoles(classID);
        lua_pushboolean(L, roles & 0x2);
        lua_pushboolean(L, roles & 0x4);
        lua_pushboolean(L, roles & 0x8);
        return 3;
    }

    return 0;
}

// OFFSET: 0x009834A0/0x1406FE200
static int32_t Script_GetNumRFDungeons(lua_State* L) {
    lua_pushnumber(L, CGLookingForGroup::m_rfEntries.m_count);
    return 1;
}

// OFFSET: 0x009834E0/0x1406FE230
static int32_t Script_GetRFDungeonInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRFDungeonInfo(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index >= CGLookingForGroup::m_rfEntries.m_count) {
        return 0;
    }

    CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_rfEntries.m_data[index];
    lua_pushnumber(L, static_cast<uint32_t>(entry.m_id));
    return PushDungeonInfo(L, entry.m_id) + 1;
}

// OFFSET: 0x00983F70/0x1406FEF40
static int32_t Script_GetNumDungeonForRandomSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNumDungeonForRandomSlot(randomID)");
        return 0;
    }

    LFGRandomSlotDungeons* list = CGLookingForGroup::GetRandomSlot(lua_tointeger(L, 1));
    uint32_t count = list ? list->m_count : 0;
    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00983FE0/0x1406FEFB0
static int32_t Script_GetDungeonForRandomSlot(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetNumDungeonForRandomSlot(randomID, index)");
        return 0;
    }

    LFGRandomSlotDungeons* list = CGLookingForGroup::GetRandomSlot(lua_tointeger(L, 1));

    if (list) {
        uint32_t index = lua_tointeger(L, 2) - 1;

        if (index < list->m_count) {
            LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(list->m_dungeons[index]);

            if (rec) {
                lua_pushnumber(L, rec->m_id);
                return 1;
            }
        }
    }

    lua_pushstring(L, "");
    return 1;
}

// OFFSET: 0x00987C30/0x140703BA0
static int32_t Script_GetBestRFChoice(lua_State* L) {
    int32_t dungeonID = CGLookingForGroup::GetBestRFChoice();

    if (dungeonID) {
        lua_pushinteger(L, dungeonID);
        return 1;
    }

    return 0;
}

// OFFSET: 0x00985C30/0x140700E90
static int32_t Script_GetLFGDungeonRewardCapBarInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGDungeonRewardCapBarInfo(dungeonID)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));

    if (!entry) {
        return 0;
    }

    int32_t currencyID = entry->m_currencyID;

    if (!currencyID) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, rec->m_id);
        lua_pushnumber(L, entry->unk_001C[0]);
        lua_pushnumber(L, entry->unk_001C[1]);
        lua_pushnumber(L, entry->unk_001C[2]);
        lua_pushnumber(L, entry->unk_001C[3]);
        lua_pushnumber(L, entry->unk_001C[4]);
        lua_pushnumber(L, entry->unk_001C[5]);
        lua_pushnumber(L, entry->unk_001C[6]);
        lua_pushnumber(L, entry->unk_001C[7]);
        return 10;
    }

    if (!CGLookingForGroup::m_playerLockInfo.m_valid) {
        return 0;
    }

    int32_t value0 = -1;
    int32_t value2 = -1;
    int32_t value4 = -1;
    int32_t value6 = -1;
    int32_t bestDungeon = -1;
    int32_t best = 0;
    int32_t value3 = 0;
    int32_t value5 = 0;
    int32_t value7 = 0;

    for (LFGDungeonEntry* other = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); !CGLookingForGroup::m_playerLockInfo.m_dungeons.IsEnd(other); other = other->RawNext()) {
        if (other->m_currencyID == currencyID && (!best || other->unk_001C[1] < best)) {
            best = other->unk_001C[1];
            bestDungeon = other->m_slot & 0xFFFFFF;
            value0 = other->unk_001C[0];
            value2 = other->unk_001C[2];
            value3 = other->unk_001C[3];
            value4 = other->unk_001C[4];
            value5 = other->unk_001C[5];
            value6 = other->unk_001C[6];
            value7 = other->unk_001C[7];
        }
    }

    CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyID);

    if (!currency) {
        return 0;
    }

    int32_t divisor = 1;

    if (currency->m_flags & 0x8) {
        divisor = 100;
    }

    lua_pushnumber(L, currencyID);
    lua_pushnumber(L, bestDungeon);
    lua_pushnumber(L, value0 / divisor);
    lua_pushnumber(L, best / divisor);
    lua_pushnumber(L, value2 / divisor);
    lua_pushnumber(L, value3 / divisor);
    lua_pushnumber(L, value4 / divisor);
    lua_pushnumber(L, value5 / divisor);
    lua_pushnumber(L, value6 / divisor);
    lua_pushnumber(L, value7 / divisor);
    return 10;
}

// OFFSET: 0x00985F10/0x140701240
static int32_t Script_GetLFGDungeonRewardCapInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGDungeonRewardCapInfo(dungeonID)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));

    if (!entry) {
        return 0;
    }

    lua_pushnumber(L, entry->m_currencyID);
    lua_pushnumber(L, static_cast<int32_t>(entry->unk_0010));
    lua_pushnumber(L, entry->unk_001C[0]);
    lua_pushnumber(L, entry->unk_001C[1]);
    lua_pushnumber(L, entry->unk_001C[2]);
    lua_pushnumber(L, entry->unk_001C[3]);
    lua_pushnumber(L, entry->unk_001C[4]);
    lua_pushnumber(L, entry->unk_001C[5]);
    lua_pushnumber(L, entry->unk_001C[6]);
    lua_pushnumber(L, entry->unk_001C[7]);
    lua_pushboolean(L, rec->m_flags & 0x10);
    int32_t count = 11;

    if (rec->m_flags & 0x40) {
        int32_t high = g_dungeonEncounterDB.m_numRecords;
        int32_t low = 0;
        int32_t first = -1;

        if (high > 0) {
            do {
                int32_t middle = (high + low) / 2;

                if (g_dungeonEncounterDB.GetRecordByIndex(middle)->m_mapID >= rec->m_mapID) {
                    high = middle;
                } else {
                    low = middle + 1;
                }
            } while (low < high);

            if (low < g_dungeonEncounterDB.m_numRecords && g_dungeonEncounterDB.GetRecordByIndex(low)->m_mapID == rec->m_mapID) {
                first = low;
            }
        }

        if (first >= 0) {
            for (int32_t i = first; i < g_dungeonEncounterDB.m_numRecords; i++) {
                DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecordByIndex(i);

                if (encounter->m_mapID != rec->m_mapID) {
                    break;
                }

                if (encounter->m_difficulty == -1 || encounter->m_difficulty == rec->m_difficulty) {
                    bool available = !((1 << encounter->m_bit) & entry->m_completedEncounters);
                    lua_pushstring(L, encounter->m_name);

                    if (available) {
                        lua_pushnumber(L, 1.0);
                    } else {
                        lua_pushnil(L);
                    }

                    count += 2;
                }
            }
        }
    }

    return count;
}

// OFFSET: 0x00986240/0x140701690
static int32_t Script_GetLFGRoleShortageRewards(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetLFGRoleShortageRewards(dungeonID, shortageSeverity)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    uint32_t severity = lua_tointeger(L, 2) - 1;

    if (severity > 2) {
        return 0;
    }

    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));

    if (entry) {
        lua_pushboolean(L, entry->unk_006C);
        LFGShortageReward* shortage = &entry->m_shortages[severity];

        for (int32_t i = 0; i < 3; i++) {
            lua_pushboolean(L, shortage->m_roleMask & (2 << i));
        }

        lua_pushnumber(L, shortage->m_rewards.m_count);
        lua_pushnumber(L, shortage->m_money);
        lua_pushnumber(L, shortage->m_xp);
    } else {
        lua_pushboolean(L, 0);

        for (uint32_t i = 0; i < 3; i++) {
            lua_pushboolean(L, 0);
        }

        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 7;
}

// OFFSET: 0x009863B0/0x140701860
static int32_t Script_GetLFGDungeonNumEncounters(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGDungeonNumEncounters(dungeonID)");
        return 0;
    }

    int32_t dungeonID = static_cast<int32_t>(lua_tonumber(L, 1));
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));
    uint32_t total = 0;
    uint32_t completed = 0;
    DungeonEncounterCountMatchingRows(rec->m_mapID, rec->m_difficulty, entry ? entry->m_completedEncounters : 0, &total, &completed);
    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    return 2;
}

// OFFSET: 0x009864A0/0x1407019D0
static int32_t Script_GetLFGDungeonEncounterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetLFGDungeonEncounterInfo(dungeonID, encounterIndex)");
        return 0;
    }

    int32_t dungeonID = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (rec) {
        uint32_t mask = 0;
        LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));

        if (entry) {
            mask = entry->m_completedEncounters;
        }

        return PushDungeonEncounterInfo(L, rec->m_mapID, rec->m_difficulty, index, mask, 0);
    }

    return 0;
}

// OFFSET: 0x00988E30/0x140705650
static int32_t Script_GetLFGDungeonShortageRewardLink(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: GetLFGDungeonShortageRewardLink(dungeonID, shortageSeverity, lootIndex)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    uint32_t severity = lua_tointeger(L, 2) - 1;

    if (severity <= 2) {
        uint32_t index = lua_tointeger(L, 3) - 1;
        LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));

        if (entry && index < entry->m_shortages[severity].m_rewards.m_count) {
            LFG_SLOT_INFO_LOOT* item = &entry->m_shortages[severity].m_rewards.m_data[index];
            int32_t id = item->m_id;
            WOWGUID none;
            none.m_guid = 0;
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);

            if (stats) {
                lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item->m_id, stats->m_quality, 0, 0, 0, 0));
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x00988F80/0x1407057F0
static int32_t Script_GetLFGDungeonShortageRewardInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: GetLFGDungeonRewardInfo(dungeonID, shortageSeverity, lootIndex)");
        return 0;
    }

    int32_t dungeonID = lua_tointeger(L, 1);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    if (!rec) {
        return 0;
    }

    uint32_t severity = lua_tointeger(L, 2) - 1;

    if (severity <= 2) {
        uint32_t index = lua_tointeger(L, 3) - 1;
        LFGDungeonEntry* entry = CGLookingForGroup::GetDungeonRewardInfo((dungeonID & 0xFFFFFF) | (rec->m_typeID << 24));

        if (entry && index < entry->m_shortages[severity].m_rewards.m_count) {
            LFG_SLOT_INFO_LOOT* item = &entry->m_shortages[severity].m_rewards.m_data[index];
            const char* path = ClientDBStringLookup(3);

            if (item->m_isCurrency) {
                CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item->m_id);

                if (currency) {
                    SStrPrintf(s_shortageRewardTexture, sizeof(s_shortageRewardTexture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
                    lua_pushstring(L, currency->m_name);
                    lua_pushstring(L, s_shortageRewardTexture);

                    if (currency->m_flags & 0x8) {
                        lua_pushinteger(L, item->m_quantity / 100);
                    } else {
                        lua_pushinteger(L, item->m_quantity);
                    }

                    return 3;
                }
            } else {
                const char* name = "";
                int32_t id = item->m_id;
                WOWGUID none;
                none.m_guid = 0;
                ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, RandomDungeonRewardItemStatsCallback, NULL, false);

                if (stats) {
                    name = stats->GetName(0);
                }

                SStrPrintf(s_shortageRewardTexture, sizeof(s_shortageRewardTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayID));
                lua_pushstring(L, name);
                lua_pushstring(L, s_shortageRewardTexture);
                lua_pushnumber(L, item->m_quantity);
                return 3;
            }
        }
    }

    return 0;
}

// STATIC: 0x00D137C8
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetLFGDungeon", &Script_SetLFGDungeon },
    { "ClearLFGDungeon", &Script_ClearLFGDungeon },
    { "ClearAllLFGDungeons", &Script_ClearAllLFGDungeons },
    { "GetLFGInfoLocal", &Script_GetLFGInfoLocal },
    { "GetLFGInfoServer", &Script_GetLFGInfoServer },
    { "GetLFGQueuedList", &Script_GetLFGQueuedList },
    { "SetLFGComment", &Script_SetLFGComment },
    { "LFGTeleport", &Script_LFGTeleport },
    { "JoinLFG", &Script_JoinLFG },
    { "LeaveLFG", &Script_LeaveLFG },
    { "RefreshLFGList", &Script_RefreshLFGList },
    { "SearchLFGJoin", &Script_SearchLFGJoin },
    { "SearchLFGGetJoinedID", &Script_SearchLFGGetJoinedID },
    { "SearchLFGLeave", &Script_SearchLFGLeave },
    { "SearchLFGGetNumResults", &Script_SearchLFGGetNumResults },
    { "SearchLFGGetResults", &Script_SearchLFGGetResults },
    { "SearchLFGGetEncounterResults", &Script_SearchLFGGetEncounterResults },
    { "SearchLFGGetPartyResults", &Script_SearchLFGGetPartyResults },
    { "SearchLFGSort", &Script_SearchLFGSort },
    { "GetLFGTypes", &Script_GetLFGTypes },
    { "GetLFGRoleUpdate", &Script_GetLFGRoleUpdate },
    { "GetLFGRoleUpdateSlot", &Script_GetLFGRoleUpdateSlot },
    { "GetLFGRoleUpdateMember", &Script_GetLFGRoleUpdateMember },
    { "GetLFGRoles", &Script_GetLFGRoles },
    { "SetLFGRoles", &Script_SetLFGRoles },
    { "CompleteLFGRoleCheck", &Script_CompleteLFGRoleCheck },
    { "GetLFGProposal", &Script_GetLFGProposal },
    { "GetLFGProposalMember", &Script_GetLFGProposalMember },
    { "GetLFGProposalEncounter", &Script_GetLFGProposalEncounter },
    { "AcceptProposal", &Script_AcceptProposal },
    { "RejectProposal", &Script_RejectProposal },
    { "GetLFGBootProposal", &Script_GetLFGBootProposal },
    { "SetLFGBootVote", &Script_SetLFGBootVote },
    { "GetLFGQueueStats", &Script_GetLFGQueueStats },
    { "GetLastQueueStatusIndex", &Script_GetLastQueueStatusIndex },
    { "GetLFGDungeonInfo", &Script_GetLFGDungeonInfo },
    { "GetLFGRandomDungeonInfo", &Script_GetLFGRandomDungeonInfo },
    { "IsLFGDungeonJoinable", &Script_IsLFGDungeonJoinable },
    { "SetLFGHeaderCollapsed", &Script_SetLFGHeaderCollapsed },
    { "SetLFGDungeonEnabled", &Script_SetLFGDungeonEnabled },
    { "GetLFGCompletionReward", &Script_GetLFGCompletionReward },
    { "GetLFGCompletionRewardItem", &Script_GetLFGCompletionRewardItem },
    { "IsInLFGDungeon", &Script_IsInLFGDungeon },
    { "CanPartyLFGBackfill", &Script_CanPartyLFGBackfill },
    { "GetPartyLFGBackfillInfo", &Script_GetPartyLFGBackfillInfo },
    { "PartyLFGStartBackfill", &Script_PartyLFGStartBackfill },
    { "GetLFGRandomCooldownExpiration", &Script_GetLFGRandomCooldownExpiration },
    { "UnitHasLFGRandomCooldown", &Script_UnitHasLFGRandomCooldown },
    { "GetLFGDeserterExpiration", &Script_GetLFGDeserterExpiration },
    { "UnitHasLFGDeserter", &Script_UnitHasLFGDeserter },
    { "GetLFGDungeonNumEncounters", &Script_GetLFGDungeonNumEncounters },
    { "GetLFGDungeonEncounterInfo", &Script_GetLFGDungeonEncounterInfo },
    { "UnitGetAvailableRoles", &Script_UnitGetAvailableRoles },
    { "GetLFDChoiceOrder", &Script_GetLFDChoiceOrder },
    { "GetLFDChoiceCollapseState", &Script_GetLFDChoiceCollapseState },
    { "GetLFDChoiceEnabledState", &Script_GetLFDChoiceEnabledState },
    { "GetLFDChoiceLockedState", &Script_GetLFDChoiceLockedState },
    { "GetLFDLockPlayerCount", &Script_GetLFDLockPlayerCount },
    { "GetLFDLockInfo", &Script_GetLFDLockInfo },
    { "RequestLFDPlayerLockInfo", &Script_RequestLFDPlayerLockInfo },
    { "RequestLFDPartyLockInfo", &Script_RequestLFDPartyLockInfo },
    { "GetNumRandomDungeons", &Script_GetNumRandomDungeons },
    { "GetLFGDungeonRewards", &Script_GetLFGDungeonRewards },
    { "GetLFGDungeonRewardInfo", &Script_GetLFGDungeonRewardInfo },
    { "GetLFGDungeonRewardCapBarInfo", &Script_GetLFGDungeonRewardCapBarInfo },
    { "GetLFGDungeonRewardCapInfo", &Script_GetLFGDungeonRewardCapInfo },
    { "GetLFGDungeonRewardLink", &Script_GetLFGDungeonRewardLink },
    { "GetLFGDungeonShortageRewardLink", &Script_GetLFGDungeonShortageRewardLink },
    { "GetRandomDungeonBestChoice", &Script_GetRandomDungeonBestChoice },
    { "GetLFGRoleShortageRewards", &Script_GetLFGRoleShortageRewards },
    { "GetLFGDungeonShortageRewardInfo", &Script_GetLFGDungeonShortageRewardInfo },
    { "GetLFRChoiceOrder", &Script_GetLFRChoiceOrder },
    { "IsListedInLFR", &Script_IsListedInLFR },
    { "GetNumDungeonForRandomSlot", &Script_GetNumDungeonForRandomSlot },
    { "GetDungeonForRandomSlot", &Script_GetDungeonForRandomSlot },
    { "GetNumRFDungeons", &Script_GetNumRFDungeons },
    { "GetRFDungeonInfo", &Script_GetRFDungeonInfo },
    { "GetBestRFChoice", &Script_GetBestRFChoice },
};

// OFFSET: 0x0097F630
void LookingForGroupRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0097F660
void LookingForGroupUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
