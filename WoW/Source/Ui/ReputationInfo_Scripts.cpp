#include "ReputationInfo.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// OFFSET: 0x009A7F00/0x14072EB50
static int32_t Script_CollapseAllFactionHeaders(lua_State* L) {
    CGReputationInfo::CollapseHeader(CGReputationInfo::m_numFactions, 1);
    return 0;
}

// OFFSET: 0x009A7F20/0x14072EB70
static int32_t Script_ExpandAllFactionHeaders(lua_State* L) {
    CGReputationInfo::CollapseHeader(CGReputationInfo::m_numFactions, 0);
    return 0;
}

// OFFSET: 0x009A5A90/0x14072BE80
static int32_t Script_GetNumFactions(lua_State* L) {
    lua_pushnumber(L, CGReputationInfo::m_numShownFactions);
    return 1;
}

// OFFSET: 0x009A7EB0/0x14072EB00
static int32_t Script_CollapseFactionHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CollapseFactionHeader(index)");
        return 0;
    }

    CGReputationInfo::CollapseHeader(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, 1);
    return 0;
}

// OFFSET: 0x009A7F40/0x14072EB90
static int32_t Script_ExpandFactionHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ExpandFactionHeader(index)");
        return 0;
    }

    CGReputationInfo::CollapseHeader(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, 0);
    return 0;
}

// OFFSET: 0x009A6160/0x14072C630
static int32_t Script_IsFactionInactive(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsFactionInactive(index)");
        return 0;
    }

    if (CGReputationInfo::IsFactionInactive(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009A61D0/0x14072C6B0
static int32_t Script_SetSelectedFaction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedFaction(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (!(index <= CGReputationInfo::m_numFactions)) {
        CGReputationInfo::m_selectedFaction = 0;
    } else {
        CGReputationInfo::m_selectedFaction = CGReputationInfo::m_factionList.m_data[index]->m_factionId;
    }

    return 0;
}

// OFFSET: 0x009A6240/0x14072C730
static int32_t Script_GetSelectedFaction(lua_State* L) {
    lua_pushnumber(L, CGReputationInfo::GetSelectedFactionIndex() + 1);
    return 1;
}

// OFFSET: 0x009A6D30/0x14072D530
static int32_t Script_GetFactionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetFactionInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t faction = index > CGReputationInfo::m_numFactions ? 0 : CGReputationInfo::m_factionList.m_data[index]->m_factionId;
    return GetFactionInfoHelper(L, faction);
}

// OFFSET: 0x009A6DC0/0x14072D5B0
static int32_t Script_GetFactionInfoByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetFactionInfoByID(id)");
        return 0;
    }

    return GetFactionInfoHelper(L, static_cast<uint32_t>(lua_tonumber(L, 1)));
}

// OFFSET: 0x009A6E20/0x14072D600
static int32_t Script_GetWatchedFactionInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    FactionRec* rec = NULL;
    int32_t faction = 0;

    if (player) {
        uint32_t index = player->m_player->m_playerFieldWatchedFactionIndex;
        faction = index > 255 ? 0 : CGReputationInfo::m_factionStandings[index].m_factionId;
        rec = g_factionDB.GetRecord(faction);
    }

    if (rec) {
        lua_pushstring(L, rec->m_name);
        int32_t reaction = CGReputationInfo::GetFactionStandingReaction(faction);
        lua_pushnumber(L, reaction + 1);
        int32_t max = CGReputationInfo::m_reactionThresholds[reaction + 1];
        int32_t value = GetRepListRepValue(faction);
        lua_pushnumber(L, CGReputationInfo::m_reactionThresholds[reaction]);
        lua_pushnumber(L, max);
        lua_pushnumber(L, value);
        return 5;
    }

    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 5;
}

// OFFSET: 0x009A6F90/0x14072D800
static int32_t Script_FactionToggleAtWar(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: FactionToggleAtWar(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index <= CGReputationInfo::m_numFactions && CGReputationInfo::m_factionList.m_data[index]->m_factionId) {
        int32_t faction = CGReputationInfo::m_factionList.m_data[index]->m_factionId;
        CGReputationInfo::SetAtWar(faction, !CGReputationInfo::IsAtWar(faction));
    }

    return 0;
}

// OFFSET: 0x009A7020/0x14072D8C0
static int32_t Script_SetWatchedFactionIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetWatchedFactionIndex(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (!(index <= CGReputationInfo::m_numFactions)) {
        CGReputationInfo::SetWatchedFaction(0);
    } else {
        CGReputationInfo::SetWatchedFaction(CGReputationInfo::m_factionList.m_data[index]->m_factionId);
    }

    return 0;
}

// OFFSET: 0x009A7DB0/0x14072EA20
static int32_t Script_SetFactionInactive(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetFactionInactive(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index <= CGReputationInfo::m_numFactions) {
        int32_t faction = CGReputationInfo::m_factionList.m_data[index]->m_factionId;

        if (faction) {
            CGReputationInfo::SetFactionInactive(faction, true);
        }
    }

    return 0;
}

// OFFSET: 0x009A7E30/0x14072EA90
static int32_t Script_SetFactionActive(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetFactionActive(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index <= CGReputationInfo::m_numFactions) {
        int32_t faction = CGReputationInfo::m_factionList.m_data[index]->m_factionId;

        if (faction) {
            CGReputationInfo::SetFactionInactive(faction, false);
        }
    }

    return 0;
}

// OFFSET: 0x009A7090/0x14072D940
static int32_t Script_GetGuildExpirationTime(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushnumber(L, player->m_player->m_playerGuilddeleteDate);
    return 1;
}

// OFFSET: 0x009A70F0/0x14072D9A0
static int32_t Script_GetGuildFactionInfo(lua_State* L) {
    int32_t count = GetFactionInfoHelper(L, 1168);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint64_t guid = player->m_obj->m_guid.m_guid;
        lua_pushinteger(L, CGGuildInfo::GetMemberReputationToCap(guid));
        lua_pushinteger(L, CGGuildInfo::GetGuildReputationWeeklyCap());
        count += 2;
    }

    return count;
}

// STATIC: 0x00D151A8/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumFactions", &Script_GetNumFactions },
    { "GetFactionInfo", &Script_GetFactionInfo },
    { "GetFactionInfoByID", &Script_GetFactionInfoByID },
    { "GetWatchedFactionInfo", &Script_GetWatchedFactionInfo },
    { "SetWatchedFactionIndex", &Script_SetWatchedFactionIndex },
    { "FactionToggleAtWar", &Script_FactionToggleAtWar },
    { "CollapseFactionHeader", &Script_CollapseFactionHeader },
    { "CollapseAllFactionHeaders", &Script_CollapseAllFactionHeaders },
    { "SetFactionInactive", &Script_SetFactionInactive },
    { "SetFactionActive", &Script_SetFactionActive },
    { "IsFactionInactive", &Script_IsFactionInactive },
    { "ExpandFactionHeader", &Script_ExpandFactionHeader },
    { "ExpandAllFactionHeaders", &Script_ExpandAllFactionHeaders },
    { "SetSelectedFaction", &Script_SetSelectedFaction },
    { "GetSelectedFaction", &Script_GetSelectedFaction },
    { "GetGuildExpirationTime", &Script_GetGuildExpirationTime },
    { "GetGuildFactionInfo", &Script_GetGuildFactionInfo },
};

// OFFSET: 0x009A5AD0/0x14072BEB0
void ReputationInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009A5B00/0x14072BF00
void ReputationInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
