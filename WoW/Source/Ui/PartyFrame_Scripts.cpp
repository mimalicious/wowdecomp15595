#include "PartyFrame.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x008FCAC0/0x140653D90
static int32_t Script_GetNumPartyMembers(lua_State* L) {
    lua_pushnumber(L, CGPartyInfo::NumMembers());
    return 1;
}

// PROVEN: 2faefb81 IDENTICAL/NONE
// OFFSET: 0x008FCB40/0x140653DD0
static int32_t Script_GetRealNumPartyMembers(lua_State* L) {
    lua_pushnumber(L, CGPartyInfo::m_realNumMembers);
    return 1;
}

// OFFSET: 0x008FCB80/0x140653E00
static int32_t Script_GetPartyMember(lua_State* L) {
    uint32_t index;

    if (!lua_isnumber(L, 1) || (index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1) >= 4) {
        luaL_error(L, "Usage: GetPartyMember(1-4)");
        return 0;
    }

    if (CGPartyInfo::m_members[index]) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: fbd5803e IDENTICAL/NONE
// OFFSET: 0x008FCC20/0x140653E80
static int32_t Script_GetPartyLeaderIndex(lua_State* L) {
    lua_pushnumber(L, CGPartyInfo::m_leaderIndex + 1);
    return 1;
}

// PROVEN: 069c8263 IDENTICAL/NONE
// OFFSET: 0x008FCC50/0x140653EB0
static int32_t Script_GetLootThreshold(lua_State* L) {
    lua_pushnumber(L, CGPartyInfo::m_lootThreshold);
    return 1;
}

// PROVEN: 74dbd5e3 IDENTICAL/NONE
// OFFSET: 0x008FCC80/0x140653ED0
static int32_t Script_GetOptOutOfLoot(lua_State* L) {
    if (CGPartyInfo::m_optOutOfLoot) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 1e9c9de8 IDENTICAL/NONE
// OFFSET: 0x008FCCC0/0x140653F10
static int32_t Script_IsPartyLFG(lua_State* L) {
    if (CGPartyInfo::m_lfgSlot) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008FCD00/0x140653F50
static int32_t Script_GetPartyLFGID(lua_State* L) {
    if (CGPartyInfo::m_lfgSlot) {
        lua_pushnumber(L, static_cast<uint32_t>(CGPartyInfo::m_lfgSlot) & 0xFFFFFF);
        return 1;
    }

    return 0;
}

// PROVEN: 57286fe3 IDENTICAL/NONE
// OFFSET: 0x008FCD50/0x140653F90
static int32_t Script_HasLFGRestrictions(lua_State* L) {
    lua_pushboolean(L, CGPartyInfo::m_lfgRestricted);
    return 1;
}

// OFFSET: 0x008FCD70/0x140653FB0
static int32_t Script_IsOnePersonParty(lua_State* L) {
    lua_pushboolean(L, CGPartyInfo::m_onePersonParty);
    return 1;
}

// OFFSET: 0x008FDA30/0x1406551C0
static int32_t Script_IsPartyLeader(lua_State* L) {
    if (CGPartyInfo::m_leader && CGPartyInfo::m_leader == ClntObjMgrGetActivePlayer()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008FDA90/0x140655220
static int32_t Script_IsRealPartyLeader(lua_State* L) {
    if (CGPartyInfo::m_realLeader && CGPartyInfo::m_realLeader == ClntObjMgrGetActivePlayer()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008FDAF0/0x140655280
static int32_t Script_LeaveParty(lua_State* L) {
    CGPlayer_C::LeaveGroup();
    return 0;
}

// OFFSET: 0x008FDB00/0x140655290
static int32_t Script_GetLootMethod(lua_State* L) {
    const char* method;

    switch (CGPartyInfo::m_lootMethod) {
        case 0:
            method = "freeforall";
            break;

        case 1:
            method = "roundrobin";
            break;

        case 2:
            method = "master";
            break;

        case 3:
            method = "group";
            break;

        case 4:
            method = "needbeforegreed";
            break;

        default:
            method = "ERROR!";
            break;
    }

    lua_pushstring(L, method);
    int32_t partyIndex = -1;
    int32_t raidIndex = -1;
    WOWGUID master = CGPartyInfo::m_lootMaster;

    if (master) {
        if (master == ClntObjMgrGetActivePlayer()) {
            partyIndex = 0;
        } else {
            for (uint32_t i = 0; i < 4; i++) {
                if (master == CGPartyInfo::m_members[i]) {
                    partyIndex = i + 1;
                    break;
                }
            }
        }

        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
            CGRaidMember* member = CGRaidInfo::m_members[i];

            if (member && member->m_guid == master) {
                raidIndex = i + 1;
                break;
            }
        }
    }

    if (partyIndex >= 0) {
        lua_pushnumber(L, partyIndex);
    } else {
        lua_pushnil(L);
    }

    if (raidIndex >= 0) {
        lua_pushnumber(L, raidIndex);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x008FDC60/0x1406553E0
static int32_t Script_SetOptOutOfLoot(lua_State* L) {
    int32_t optOut = lua_toboolean(L, 1);
    CGPartyInfo::SetOptOutOfLoot(optOut);
    CGChat::AddChatMessage(optOut ? FrameScript_GetText("OPT_OUT_LOOT_TOGGLE_ON", -1, GENDER_NOT_APPLICABLE) : FrameScript_GetText("OPT_OUT_LOOT_TOGGLE_OFF", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
    return 0;
}

// OFFSET: 0x008FDCB0/0x140655430
static int32_t Script_GetPartyAssignment(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetPartyAssignment(\"assignment\" [,\"raidmember\"] [,exactMatch])");
        return 0;
    }

    const char* assignment = lua_tolstring(L, 1, NULL);

    if (!_stricmp(assignment, "MAINTANK")) {
        return CGPartyInfo::GetPartyAssignments(L, 0);
    }

    if (_stricmp(assignment, "MAINASSIST")) {
        luaL_error(L, "GetPartyAssignment(): Invalid party assignment");
        return 0;
    }

    return CGPartyInfo::GetPartyAssignments(L, 1);
}

// PROVEN: 732c9a85 IDENTICAL/NONE
// OFFSET: 0x008FDD40/0x140655500
static int32_t Script_CanChangePlayerDifficulty(lua_State* L) {
    lua_pushboolean(L, CGGameUI::CanChangePlayerDifficulty());
    bool cooldown = CGPartyInfo::m_nextDifficultyChange && static_cast<int32_t>(static_cast<uint32_t>(::time(NULL)) - CGPartyInfo::m_nextDifficultyChange) < 0;
    lua_pushboolean(L, !cooldown);
    return 2;
}

// OFFSET: 0x008FDDA0/0x140655560
static int32_t Script_AcceptGroup(lua_State* L) {
    uint32_t roles = 0;

    if (StringToBOOL(L, 1, 0)) {
        roles = 2;
    }

    if (StringToBOOL(L, 2, 0)) {
        roles |= 4;
    }

    if (StringToBOOL(L, 3, 0)) {
        roles |= 8;
    }

    UserClientPartyInviteResponse msg;
    msg.m_accept = true;

    if (roles > 0) {
        msg.m_rolesDesired = roles;
        msg.m_hasRolesDesired = true;
    }

    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x008FDE20/0x1406555F0
static int32_t Script_UninviteUnit(lua_State* L) {
    if (!CGGameUI::CanPerformAction(23)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    const char* reason = lua_tolstring(L, 2, NULL);
    char scrubbed[256];
    scrubbed[0] = 0;

    if (reason && *reason) {
        ScrubString(reason, scrubbed, 64, sizeof(scrubbed), 1);
    }

    int32_t exact = StringToBOOL(L, 3, 0);
    WOWGUID guid;

    if (Script_GetGUIDFromToken(name, guid, exact != 0, false)) {
        CGPlayer_C::Uninvite(guid, scrubbed);
        return 0;
    }

    guid = StringToGUID(name);

    if (CGPartyInfo::IsMember(guid) || IsGuidInMyRaid(guid)) {
        CGPlayer_C::Uninvite(guid, scrubbed);
    }

    return 0;
}

// OFFSET: 0x008FDF20/0x140655740
static int32_t Script_DeclineGroup(lua_State* L) {
    UserClientPartyInviteResponse msg;
    msg.m_accept = false;
    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x008FE700/0x140655F10
static int32_t Script_SilenceMember(lua_State* L) {
    if ((!CGRaidInfo::m_numMembers && !CGPartyInfo::m_members[0]) || !lua_isstring(L, 1)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, (CGRaidInfo::m_numMembers != 0) + 5, 0, 3.402823466e38f, 0);

    if (!guid) {
        CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (CGRaidInfo::m_numMembers) {
        WOWGUID playerGuid = player->m_obj->m_guid;

        if (CGRaidInfo::IsPlayerRaidOfficer(playerGuid)) {
            CGPlayer_C::SilencePlayer(guid, CGRaidInfo::IsPVPRaid() != 0);
            return 0;
        }
    } else if (CGPartyInfo::m_leader == player->m_obj->m_guid) {
        CGPlayer_C::SilencePlayer(guid, CGRaidInfo::IsPVPRaid() != 0);
    }

    return 0;
}

// OFFSET: 0x008FE840/0x140656040
static int32_t Script_UnSilenceMember(lua_State* L) {
    if ((!CGRaidInfo::m_numMembers && !CGPartyInfo::m_members[0]) || !lua_isstring(L, 1)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, (CGRaidInfo::m_numMembers != 0) + 5, 0, 3.402823466e38f, 0);

    if (!guid) {
        CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (CGRaidInfo::m_numMembers) {
        WOWGUID playerGuid = player->m_obj->m_guid;

        if (CGRaidInfo::IsPlayerRaidOfficer(playerGuid)) {
            CGPlayer_C::UnSilencePlayer(guid, CGRaidInfo::IsPVPRaid() != 0);
            return 0;
        }
    } else if (CGPartyInfo::m_leader == player->m_obj->m_guid) {
        CGPlayer_C::UnSilencePlayer(guid, CGRaidInfo::IsPVPRaid() != 0);
    }

    return 0;
}

// OFFSET: 0x008FE980/0x140656170
static int32_t Script_SetLootMethod(lua_State* L) {
    if (!CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
        return 0;
    }

    if (CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_NOT_LEADER);
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetLootMethod(\"method\" [,master])");
        return 0;
    }

    const char* method = lua_tolstring(L, 1, NULL);
    int32_t lootMethod;

    if (!_stricmp(method, "freeforall")) {
        lootMethod = 0;
    } else if (!_stricmp(method, "roundrobin")) {
        lootMethod = 1;
    } else if (!_stricmp(method, "master")) {
        lootMethod = 2;
    } else if (!_stricmp(method, "group")) {
        lootMethod = 3;
    } else {
        if (_stricmp(method, "needbeforegreed")) {
            luaL_error(L, "Invalid loot method");
            return 0;
        }

        lootMethod = 4;
    }

    WOWGUID master;
    master.m_guid = 0;

    if (lootMethod == 2) {
        const char* name;

        if (!lua_isstring(L, 2) || !(name = lua_tolstring(L, 2, NULL)) || !*name) {
            CGGameUI::DisplayError(ERR_SPECIFY_MASTER_LOOTER);
            return 0;
        }

        if (!CGRaidInfo::m_numMembers) {
            master = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 5, StringToBOOL(L, 3, 0), 3.402823466e38f, 0);
        } else {
            master = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 6, StringToBOOL(L, 3, 0), 3.402823466e38f, 0);
        }

        if (!master || (!CGPartyInfo::IsMember(master) && !IsGuidInMyRaid(master))) {
            CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
            return 0;
        }
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::SetLootMethod(lootMethod, master, CGPartyInfo::m_lootThreshold);
    }

    return 0;
}

// OFFSET: 0x008FEBB0/0x140656410
static int32_t Script_SetLootThreshold(lua_State* L) {
    if (!CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
        return 0;
    }

    if (CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_NOT_LEADER);
    } else {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: SetLooThreshold(threshold)");
            return 0;
        }

        int32_t threshold = static_cast<int32_t>(lua_tonumber(L, 1));

        if (!(static_cast<uint32_t>(threshold - 2) > 4)) {
            if (ClntObjMgrGetActivePlayerObj()) {
                CGPlayer_C::SetLootMethod(CGPartyInfo::m_lootMethod, CGPartyInfo::m_lootMaster, threshold);
            }

            return 0;
        }

        luaL_error(L, "SetLootThreshold(): threshold must be between %d and %d", 2, 6);
    }

    return 0;
}

// OFFSET: 0x008FEC90/0x140656510
static int32_t SetOrClearAssignment(lua_State* L, bool set) {
    if (set && !CGGameUI::CanPerformAction(4)) {
        return 0;
    }

    if (!CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
        return 0;
    }

    if (CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
        WOWGUID player = ClntObjMgrGetActivePlayer();

        if (!CGRaidInfo::IsPlayerRaidOfficer(player)) {
            CGGameUI::DisplayError(ERR_NOT_LEADER);
            return 0;
        }
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: %s(\"assignment\" [,\"raidmember\"] [,exactMatch])", set ? "SetPartyAssignment" : "ClearPartyAssignment");
        return 0;
    }

    WOWGUID guid;
    guid.m_guid = 0;
    const char* assignmentName = lua_tolstring(L, 1, NULL);
    int32_t assignment;

    if (!_stricmp(assignmentName, "MAINTANK")) {
        assignment = 0;
    } else {
        if (_stricmp(assignmentName, "MAINASSIST")) {
            luaL_error(L, "Invalid Party assignment");
            return 0;
        }

        assignment = 1;
    }

    if (!CanSendPartyCommand()) {
        return 0;
    }

    if (lua_isstring(L, 2)) {
        const char* name = lua_tolstring(L, 2, NULL);
        guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 6, StringToBOOL(L, 3, 0), 3.402823466e38f, 0);

        if (!guid) {
            WOWGUID unit = CGGameUI::ClosestObjectMatch(name, TYPEMASK_UNIT, 0, StringToBOOL(L, 3, 0), 3.402823466e38f, 0);

            if (ClntObjMgrObjectPtr(unit, TYPEMASK_UNIT, __FILE__, __LINE__)) {
                CGGameUI::DisplayError(ERR_MEETING_STONE_TARGET_NOT_IN_PARTY);
            } else if (name && *name && _stricmp(name, "target")) {
                CGGameUI::DisplayError(ERR_UNIT_NOT_FOUND);
            } else {
                CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
            }

            return 0;
        }
    }

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPartyInfo::m_lastAssignmentTime = FrameTime::GetCurTimeMs();

        if (set) {
            CGPlayer_C::SetPartyAssignment(assignment, guid);
            return 0;
        }

        CGPlayer_C::ClearPartyAssignment(assignment, guid);
    }

    return 0;
}

// OFFSET: 0x008FEF00/0x1406567D0
static int32_t Script_SetPartyAssignment(lua_State* L) {
    return SetOrClearAssignment(L, true);
}

// OFFSET: 0x008FF140/0x140656A30
static int32_t Script_ClearPartyAssignment(lua_State* L) {
    return SetOrClearAssignment(L, false);
}

// OFFSET: 0x008FF160/0x140656A40
static int32_t Script_InviteUnit(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (!name) {
        return 0;
    }

    if (strlen(name) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    uint32_t roles = 0;

    if (StringToBOOL(L, 2, 0)) {
        roles = 2;
    }

    if (StringToBOOL(L, 3, 0)) {
        roles |= 4;
    }

    if (StringToBOOL(L, 4, 0)) {
        roles |= 8;
    }

    CGPlayer_C::Invite(name, roles);
    return 0;
}

// STATIC: 0x00D107C8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumPartyMembers", &Script_GetNumPartyMembers },
    { "GetRealNumPartyMembers", &Script_GetRealNumPartyMembers },
    { "GetPartyMember", &Script_GetPartyMember },
    { "GetPartyLeaderIndex", &Script_GetPartyLeaderIndex },
    { "IsPartyLeader", &Script_IsPartyLeader },
    { "IsRealPartyLeader", &Script_IsRealPartyLeader },
    { "LeaveParty", &Script_LeaveParty },
    { "GetLootMethod", &Script_GetLootMethod },
    { "SetLootMethod", &Script_SetLootMethod },
    { "GetLootThreshold", &Script_GetLootThreshold },
    { "SetLootThreshold", &Script_SetLootThreshold },
    { "SetPartyAssignment", &Script_SetPartyAssignment },
    { "ClearPartyAssignment", &Script_ClearPartyAssignment },
    { "GetPartyAssignment", &Script_GetPartyAssignment },
    { "SilenceMember", &Script_SilenceMember },
    { "UnSilenceMember", &Script_UnSilenceMember },
    { "SetOptOutOfLoot", &Script_SetOptOutOfLoot },
    { "GetOptOutOfLoot", &Script_GetOptOutOfLoot },
    { "CanChangePlayerDifficulty", &Script_CanChangePlayerDifficulty },
    { "IsPartyLFG", &Script_IsPartyLFG },
    { "GetPartyLFGID", &Script_GetPartyLFGID },
    { "HasLFGRestrictions", &Script_HasLFGRestrictions },
    { "IsOnePersonParty", &Script_IsOnePersonParty },
    { "InviteUnit", &Script_InviteUnit },
    { "UninviteUnit", &Script_UninviteUnit },
    { "AcceptGroup", &Script_AcceptGroup },
    { "DeclineGroup", &Script_DeclineGroup },
};

// OFFSET: 0x008FCD90
void CGPartyInfo::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x008FCDC0
void CGPartyInfo::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
