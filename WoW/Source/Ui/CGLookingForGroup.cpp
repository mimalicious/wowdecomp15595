#include "LFGInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include <cstdlib>
#include <cstring>
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Common/ThrottleObject.h"

// STATIC: 0x00C273B8
static const uint8_t s_classRoles[11] = { 0x0A, 0x0E, 0x08, 0x08, 0x0C, 0x0A, 0x0C, 0x08, 0x08, 0x00, 0x0E };

// STATIC: 0x00C9F524
const char* s_roleTokens[3] = { "TANK", "HEALER", "DAMAGER" };

// STATIC: 0x00F4A4D0
static uint32_t s_lastQueueSlot;

// OFFSET: 0x00A30410/0x1407DA340
uint32_t CGLookingForGroup::GetClassRoles(uint32_t classID) {
    if (!classID) {
        return 0;
    }

    if (classID > 11) {
        return 0;
    }

    return s_classRoles[classID - 1];
}

// OFFSET: 0x00A30430/0x1407DA360
const char* GetRoleTokenFromFlags(uint32_t flags) {
    for (int32_t i = 0; i < 3; i++) {
        if ((2 << i) & flags) {
            return s_roleTokens[i];
        }
    }

    return "NONE";
}

// OFFSET: 0x00A30470/0x1407DA390
uint32_t GetRoleFlagFromToken(const char* token) {
    for (int32_t i = 0; i < 3; i++) {
        if (!_stricmp(token, s_roleTokens[i])) {
            return 2 << i;
        }
    }

    return 0;
}

// INLINED
void CGLookingForGroup::RequestLFGStatus() {
    UserClientOpcode2581 request;
    ClientServices::Send(&request);
}

// INLINED
void CGLookingForGroup::RequestLFGPlayerInfo() {
    UserClientLfgLockInfoRequest request;
    request.unk_0010 = true;
    ClientServices::Send(&request);
}

// OFFSET: 0x00980700
static bool OnSetDfFastLaunchResult(ClientSetDfFastLaunchResult* msg) {
    char text[64];
    SStrPrintf(text, sizeof(text), "lfgFastLaunch set to %d", msg->unk_0010);
    CGGameUI::AddErrorMessage(text, 1);
    ConsoleWrite(text, ERROR_COLOR);
    return true;
}

// OFFSET: 0x00980740
static bool OnLfgSlotInvalid(ClientLfgSlotInvalid* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_INVALID_ERROR_MESSAGE, "%d%d%d", msg->unk_0010.m_firstCompleted, msg->unk_0010.m_projectID, msg->unk_0010.m_completionCount);
    return true;
}

// OFFSET: 0x00983570
void CGLookingForGroup::EnterWorld() {
    CGLookingForGroup::m_lastProposalID = 0;
    CGLookingForGroup::RequestLFGStatus();
    CGLookingForGroup::RequestLFGPlayerInfo();
    ClientSetDfFastLaunchResult::s_handler = OnSetDfFastLaunchResult;
    ClientLfgSlotInvalid::s_handler = OnLfgSlotInvalid;
}

// OFFSET: 0x00980C00
void DungeonEncounterCountMatchingRows(int32_t mapId, int32_t difficulty, uint32_t completedMask, uint32_t* total, uint32_t* completed) {
    *total = 0;
    *completed = 0;

    for (int32_t i = 0; i < g_dungeonEncounterDB.m_numRecords; i++) {
        DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecordByIndex(i);

        if (encounter->m_mapID == mapId && (encounter->m_difficulty == -1 || encounter->m_difficulty == difficulty)) {
            ++*total;

            if ((1 << encounter->m_bit) & completedMask) {
                ++*completed;
            }
        }
    }
}

// OFFSET: 0x00980C80/0x1406FB0E0
int32_t PushDungeonEncounterInfo(lua_State* L, int32_t mapId, int32_t difficulty, int32_t index, uint32_t completedMask, uint32_t otherMask) {
    for (int32_t i = 0; i < g_dungeonEncounterDB.m_numRecords; i++) {
        DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecordByIndex(i);

        if (encounter->m_mapID != mapId || (encounter->m_difficulty != -1 && encounter->m_difficulty != difficulty)) {
            continue;
        }

        if (index) {
            index--;
            continue;
        }

        lua_pushstring(L, encounter->m_name);
        SpellIconRec* icon;

        if (encounter->m_spellIconID && (icon = g_spellIconDB.GetRecord(encounter->m_spellIconID)) && icon->m_textureFilename) {
            lua_pushstring(L, icon->m_textureFilename);
        } else {
            lua_pushnil(L);
        }

        lua_pushboolean(L, (completedMask & (1 << encounter->m_bit)) != 0);
        lua_pushboolean(L, (otherMask & (1 << encounter->m_bit)) != 0);
        return 4;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 4;
}

// OFFSET: 0x00982070
int32_t CGLookingForGroup::GetLFGRoleUpdate(lua_State* L) {
    lua_pushboolean(L, CGLookingForGroup::m_roleCheckState == 2);
    lua_pushnumber(L, CGLookingForGroup::m_roleCheckSlots.m_count);
    lua_pushnumber(L, CGLookingForGroup::m_roleCheckMembers.m_count);
    return 3;
}

// OFFSET: 0x009820F0
int32_t CGLookingForGroup::GetLFGRoleUpdateSlot(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGRoleUpdateSlot(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLookingForGroup::m_roleCheckSlots.m_count)) {
        return 0;
    }

    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_roleCheckSlots.m_data[index] & 0xFFFFFF);

    if (!rec) {
        return 0;
    }

    lua_pushnumber(L, rec->m_id);
    lua_pushnumber(L, rec->m_typeID);
    lua_pushnumber(L, GetLFGDungeonSubtype(rec));
    return 3;
}

// OFFSET: 0x009851A0/0x140700360
void CGLookingForGroup::JoinLFG() {
    if ((CGLookingForGroup::m_localInfo.m_joined && !CGLookingForGroup::m_localInfo.unk_0003) || !s_cvLfgSelectedRoles) {
        return;
    }

    static ThrottleObject s_throttle(2, 10);

    if (!s_throttle.IncrementAndCheck()) {
        return;
    }

    uint8_t roles = static_cast<uint8_t>(s_cvLfgSelectedRoles->m_intValue) & (CGLookingForGroup::GetClassRoles(ClientServices::GetCharacterClass()) | 1);

    if (!(roles & 0xE)) {
        return;
    }

    UserClientLfgJoin request;
    request.m_roles = roles;

    LFGDungeonEntry* next;

    for (LFGDungeonEntry* entry = CGLookingForGroup::m_localInfo.m_dungeons.Head(); !CGLookingForGroup::m_localInfo.m_dungeons.IsEnd(entry); entry = next) {
        next = entry->RawNext();
        request.m_slots.New(entry->m_slot);
    }

    request.m_needs[0] = CGLookingForGroup::m_localInfo.m_roles[0];
    request.m_needs[1] = CGLookingForGroup::m_localInfo.m_roles[1];
    request.m_needs[2] = CGLookingForGroup::m_localInfo.m_roles[2];
    SStrCopy(request.m_comment, CGLookingForGroup::m_localInfo.m_comment, sizeof(request.m_comment));
    ClientServices::Send(&request);
    CGLookingForGroup::m_localInfo.m_joined = 1;
    CGLookingForGroup::m_localInfo.unk_0003 = 0;
}

// OFFSET: 0x00980E90/0x1406FB390
void CGLookingForGroup::LeaveLFG() {
    if (!CGLookingForGroup::m_localInfo.m_joined) {
        LFGInfo* info = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;

        if (!info->m_joined) {
            return;
        }
    }

    UserClientLfgLeave request;
    request.m_ticket = (CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo)->m_ticket;
    request.unk_0028 = 8;
    ClientServices::Send(&request);
    CGLookingForGroup::m_localInfo.m_joined = 0;
    CGLookingForGroup::m_localInfo.unk_0003 = 0;
}

// OFFSET: 0x00981D70
int32_t CGLookingForGroup::GetLFGProposalMember(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGProposalMember(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (!CGLookingForGroup::m_proposal.m_id || index >= CGLookingForGroup::m_proposal.m_numMembers) {
        return 0;
    }

    lua_pushboolean(L, CGLookingForGroup::m_proposal.m_members[index].m_roles & 0x1);
    lua_pushstring(L, GetRoleTokenFromFlags(CGLookingForGroup::m_proposal.m_members[index].m_roles));
    lua_pushnumber(L, 0.0);
    lua_pushboolean(L, CGLookingForGroup::m_proposal.m_members[index].m_accepted);
    lua_pushboolean(L, CGLookingForGroup::m_proposal.m_members[index].unk_0014);
    lua_pushnil(L);
    lua_pushnil(L);
    return 7;
}

// OFFSET: 0x00981E60
int32_t CGLookingForGroup::GetLFGProposalEncounter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGProposalEncounter(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    LfgDungeonsRec* rec;

    if (CGLookingForGroup::m_proposal.m_id && (rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_proposal.m_id & 0xFFFFFF))) {
        return PushDungeonEncounterInfo(L, rec->m_mapID, rec->m_difficulty, index, CGLookingForGroup::m_proposal.m_completedMask, 0);
    }

    return PushDungeonEncounterInfo(L, 0, 0, 0, 0, 0);
}

// OFFSET: 0x00981F30/0x1406FC8E0
void CGLookingForGroup::ProposalResponse(bool accept) {
    if (!CGLookingForGroup::m_proposal.m_id) {
        return;
    }

    static ThrottleObject s_throttle(2, 10);

    if (!s_throttle.IncrementAndCheck()) {
        return;
    }

    DebugLog("LFGC: Sending a proposal response of: %d", accept);
    UserClientLfgProposalResponse request;
    request.m_ticket = CGLookingForGroup::m_proposal.m_ticket;
    *reinterpret_cast<uint64_t*>(request.m_instanceGuid) = *reinterpret_cast<uint64_t*>(&CGLookingForGroup::m_proposal.unk_0018);
    request.m_proposalID = CGLookingForGroup::m_proposal.unk_0008;
    request.m_accept = accept;
    ClientServices::Send(&request);

    if (accept) {
        for (uint32_t i = 0; i < CGLookingForGroup::m_proposal.m_numMembers; i++) {
            if (CGLookingForGroup::m_proposal.m_members[i].m_isLocal) {
                CGLookingForGroup::m_proposal.m_members[i].m_accepted = 1;
                CGLookingForGroup::m_proposal.m_members[i].unk_0014 = 1;
            }
        }
    } else {
        CGLookingForGroup::m_proposal.Clear();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_UPDATE, NULL);
}

// OFFSET: 0x009821D0
int32_t CGLookingForGroup::GetLFGRoleUpdateMember(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGRoleUpdateMember(index)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLookingForGroup::m_roleCheckMembers.m_count)) {
        return 0;
    }

    lua_pushboolean(L, CGLookingForGroup::m_roleCheckMembers.m_data[index].m_confirmed);
    lua_pushnumber(L, CGLookingForGroup::m_roleCheckMembers.m_data[index].m_level);
    NameCache* name = g_nameDBCache.GetRecord(CGLookingForGroup::m_roleCheckMembers.m_data[index].m_guid.m_guid, WOWGUID(), NULL, NULL, false);

    if (name) {
        lua_pushstring(L, name->m_name);
        const char* className = CGUnit_C::GetDisplayClassName(NULL, name);

        if (className) {
            lua_pushstring(L, className);
        } else {
            lua_pushstring(L, "");
        }
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    if (CGLookingForGroup::m_roleCheckMembers.m_data[index].m_confirmed) {
        lua_pushboolean(L, CGLookingForGroup::m_roleCheckMembers.m_data[index].m_roles & 0x1);
        lua_pushboolean(L, CGLookingForGroup::m_roleCheckMembers.m_data[index].m_roles & 0x2);
        lua_pushboolean(L, CGLookingForGroup::m_roleCheckMembers.m_data[index].m_roles & 0x4);
        lua_pushboolean(L, CGLookingForGroup::m_roleCheckMembers.m_data[index].m_roles & 0x8);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 8;
}

// OFFSET: 0x00982470
int32_t CGLookingForGroup::SetLFGBootVote(lua_State* L) {
    bool vote = StringToBOOL(L, 1, 0) != 0;

    if (CGLookingForGroup::m_bootProposal.m_inProgress) {
        static ThrottleObject s_throttle(2, 10);

        if (s_throttle.IncrementAndCheck()) {
            CGLookingForGroup::m_bootProposal.m_myVote = vote;
            UserClientLfgSetBootVote request;
            request.unk_0010 = vote;
            ClientServices::Send(&request);
        }
    }

    return 0;
}

// OFFSET: 0x00982520
int32_t CGLookingForGroup::GetLFGBootProposal(lua_State* L) {
    lua_pushboolean(L, CGLookingForGroup::m_bootProposal.m_inProgress);

    if (CGLookingForGroup::m_bootProposal.m_inProgress) {
        lua_pushboolean(L, CGLookingForGroup::m_bootProposal.m_didVote);
        lua_pushboolean(L, CGLookingForGroup::m_bootProposal.m_myVote);
        NameCache* name = g_nameDBCache.GetRecord(CGLookingForGroup::m_bootProposal.m_target.m_guid, WOWGUID(), NULL, NULL, false);

        if (name) {
            char fullName[256];
            lua_pushstring(L, name->FullName(fullName, sizeof(fullName)));
        } else {
            lua_pushnil(L);
        }

        lua_pushnumber(L, CGLookingForGroup::m_bootProposal.m_totalVotes);
        lua_pushnumber(L, CGLookingForGroup::m_bootProposal.m_bootVotes);
        lua_pushnumber(L, static_cast<int32_t>(CGLookingForGroup::m_bootProposal.m_timeLeft));
        ProfanityFilter(CGLookingForGroup::m_bootProposal.m_reason, false, false);
        lua_pushstring(L, CGLookingForGroup::m_bootProposal.m_reason);
        return 8;
    }

    return 1;
}

// OFFSET: 0x009835E0
bool CGLookingForGroup::SetHeaderCollapsed(uint32_t header, bool collapsed) {
    if (s_cvLfdCollapsedHeaders) {
        CVarArrayWrapper wrapper(s_cvLfdCollapsedHeaders);
        return wrapper.RawSetValue(header, collapsed != wrapper.m_defaultValue);
    }

    return false;
}

// OFFSET: 0x00983630
bool CGLookingForGroup::SetDungeonEnabled(uint32_t dungeon, bool enabled) {
    if (s_cvLfdSelectedDungeons) {
        CVarArrayWrapper wrapper(s_cvLfdSelectedDungeons);
        return wrapper.RawSetValue(dungeon, enabled != wrapper.m_defaultValue);
    }

    return false;
}

// OFFSET: 0x00987E20
void CGLookingForGroup::JoinLFGSearch(int32_t lfgID) {
    if (CGLookingForGroup::m_searchJoinedID != lfgID) {
        CGLookingForGroup::m_prevSearchResults.Clear();
        CGLookingForGroup::m_prevSearchResults.m_time = FrameTime::GetCurTimeMs();
        CGLookingForGroup::m_searchJoinedID = lfgID;
        CGLookingForGroup::m_searchResults.Clear();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
        UserClientLfgLfrJoin request;
        request.unk_0010 = CGLookingForGroup::m_searchJoinedID;
        ClientServices::Send(&request);
    }
}

// OFFSET: 0x00987960
void CGLookingForGroup::ClearLFGDungeon(uint32_t slot) {
    LFGDungeonEntry* next;

    for (LFGDungeonEntry* entry = CGLookingForGroup::m_localInfo.m_dungeons.Head(); !CGLookingForGroup::m_localInfo.m_dungeons.IsEnd(entry); entry = next) {
        next = entry->RawNext();

        if (entry->m_slot == slot) {
            entry->~LFGDungeonEntry();
            SMemFree(entry, "delete", -1, 0);
            CGLookingForGroup::m_localInfo.m_numDungeons--;
            CGLookingForGroup::m_localInfo.unk_0003 = 1;
            return;
        }
    }
}

// OFFSET: 0x00985140
int32_t CGLookingForGroup::GetQueueStatusIndex() {
    LFGInfo* info = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;
    int32_t index = 0;

    LFGDungeonEntry* next;

    for (LFGDungeonEntry* entry = info->m_dungeons.Head(); !info->m_dungeons.IsEnd(entry); entry = next) {
        next = entry->RawNext();

        if (entry->m_slot == s_lastQueueSlot) {
            return index;
        }

        index++;
    }

    return -1;
}

// OFFSET: 0x0098A3D0
void CGLookingForGroup::ClearPartyInfo() {
    CGLookingForGroup::m_partyLockInfo.Clear();

    if (CGLookingForGroup::m_roleCheckState == 2) {
        CGLookingForGroup::m_roleCheckState = 0;
        CGLookingForGroup::m_roleCheckSlots.m_count = 0;
        CGLookingForGroup::m_roleCheckMembers.m_count = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_CHECK_HIDE, NULL);
    }

    if (CGLookingForGroup::m_proposal.m_id) {
        CGLookingForGroup::m_proposal.Clear();
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_FAILED, NULL);
    }
}

// OFFSET: 0x0097FF60
bool CGLookingForGroup::GetLFGListEntryDisplayed(uint32_t index, const LFGListParty*& party, const LFGListPlayer*& player) {
    LFGLIST* list = &CGLookingForGroup::m_searchResults;

    if (CGLookingForGroup::m_unkACD660) {
        if (index < list->m_playerList.m_count) {
            party = NULL;
            player = list->m_playerList.m_data[index];
            return true;
        }

        index -= list->m_playerList.m_count;

        if (index < list->m_partyList.m_count) {
            party = list->m_partyList.m_data[index];
            player = party && party->m_members.m_count > 0 ? party->m_members.m_data[0] : NULL;
            return true;
        }

        return false;
    }

    if (index < list->m_partyList.m_count) {
        party = list->m_partyList.m_data[index];
        player = party && party->m_members.m_count > 0 ? party->m_members.m_data[0] : NULL;
        return true;
    }

    index -= list->m_partyList.m_count;

    if (index < list->m_playerList.m_count) {
        party = NULL;
        player = list->m_playerList.m_data[index];
        return true;
    }

    return false;
}

// OFFSET: 0x0097FA50
static int QSortLFGParty(const void* a, const void* b) {
    LFGListParty* partyA = *static_cast<LFGListParty* const*>(a);
    LFGListParty* partyB = *static_cast<LFGListParty* const*>(b);
    LFGListPlayer* leaderA = partyA->m_members.m_count > 0 ? partyA->m_members.m_data[0] : NULL;
    LFGListPlayer* leaderB = partyB->m_members.m_count > 0 ? partyB->m_members.m_data[0] : NULL;

    if (!leaderA) {
        return leaderB ? -1 : 0;
    }

    if (!leaderB) {
        return 1;
    }

    for (uint32_t i = 0; i < 7; i++) {
        int32_t result = 0;

        switch (CGLookingForGroup::m_unkBEA820[i].m_id) {
            case 0: {
                AreaTableRec* areaA = g_areaTableDB.GetRecord(leaderA->m_areaID);
                AreaTableRec* areaB = g_areaTableDB.GetRecord(leaderB->m_areaID);

                if (areaA && areaB) {
                    result = SStrSortUTF8I(areaA->m_areaName, areaB->m_areaName, STORM_MAX_STR);
                }

                break;
            }

            case 1:
                if (leaderA->m_level != leaderB->m_level) {
                    result = leaderA->m_level < leaderB->m_level ? -1 : 1;
                }

                break;

            case 2: {
                NameCache* nameA = g_nameDBCache.GetRecord(leaderA->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);
                NameCache* nameB = g_nameDBCache.GetRecord(leaderB->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);

                if (nameA && nameB) {
                    ChrClassesRec* classA = g_chrClassesDB.GetRecord(nameA->m_class);
                    ChrClassesRec* classB = g_chrClassesDB.GetRecord(nameB->m_class);

                    if (classA && classB) {
                        result = SStrSortUTF8I(classA->m_name, classB->m_name, STORM_MAX_STR);
                    }
                }

                break;
            }

            case 3: {
                NameCache* nameA = g_nameDBCache.GetRecord(leaderA->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);
                NameCache* nameB = g_nameDBCache.GetRecord(leaderB->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);

                if (nameA && nameB) {
                    result = strcmp(nameA->m_name, nameB->m_name);
                }

                break;
            }

            default:
                break;
        }

        if (result) {
            return CGLookingForGroup::m_unkBEA820[i].m_order ? -result : result;
        }
    }

    return 0;
}

// OFFSET: 0x0097FCD0
static int QSortLFGPlayer(const void* a, const void* b) {
    LFGListPlayer* playerA = *static_cast<LFGListPlayer* const*>(a);
    LFGListPlayer* playerB = *static_cast<LFGListPlayer* const*>(b);

    for (uint32_t i = 0; i < 7; i++) {
        int32_t result = 0;

        switch (CGLookingForGroup::m_unkBEA820[i].m_id) {
            case 0: {
                AreaTableRec* areaA = g_areaTableDB.GetRecord(playerA->m_areaID);
                AreaTableRec* areaB = g_areaTableDB.GetRecord(playerB->m_areaID);

                if (areaA && areaB) {
                    result = SStrSortUTF8I(areaA->m_areaName, areaB->m_areaName, STORM_MAX_STR);
                }

                break;
            }

            case 1:
                if (playerA->m_level != playerB->m_level) {
                    result = playerA->m_level < playerB->m_level ? -1 : 1;
                }

                break;

            case 2: {
                NameCache* nameA = g_nameDBCache.GetRecord(playerA->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);
                NameCache* nameB = g_nameDBCache.GetRecord(playerB->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);

                if (nameA && nameB) {
                    ChrClassesRec* classA = g_chrClassesDB.GetRecord(nameA->m_class);
                    ChrClassesRec* classB = g_chrClassesDB.GetRecord(nameB->m_class);

                    if (classA && classB) {
                        result = SStrSortUTF8I(classA->m_name, classB->m_name, STORM_MAX_STR);
                    }
                }

                break;
            }

            case 3: {
                NameCache* nameA = g_nameDBCache.GetRecord(playerA->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);
                NameCache* nameB = g_nameDBCache.GetRecord(playerB->m_key.m_guid.m_guid, WOWGUID(), NULL, NULL, false);

                if (nameA && nameB) {
                    result = strcmp(nameA->m_name, nameB->m_name);
                }

                break;
            }

            case 4:
                result = (playerB->m_roles & 0x2) - (playerA->m_roles & 0x2);
                break;

            case 5:
                result = (playerB->m_roles & 0x4) - (playerA->m_roles & 0x4);
                break;

            case 6:
                result = (playerB->m_roles & 0x8) - (playerA->m_roles & 0x8);
                break;

            default:
                break;
        }

        if (result) {
            return CGLookingForGroup::m_unkBEA820[i].m_order ? -result : result;
        }
    }

    return 0;
}

// OFFSET: 0x0098A9D0
void CGLookingForGroup::ResortLFGLists() {
    LFGLIST* list = &CGLookingForGroup::m_prevSearchResults;

    if (list->m_partyList.m_count) {
        qsort(list->m_partyList.m_data, list->m_partyList.m_count, sizeof(LFGListParty*), QSortLFGParty);
    }

    if (list->m_playerList.m_count) {
        qsort(list->m_playerList.m_data, list->m_playerList.m_count, sizeof(LFGListPlayer*), QSortLFGPlayer);
    }

    list->Reindex();
    CGLookingForGroup::m_searchResults.Copy(*list);
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
}
