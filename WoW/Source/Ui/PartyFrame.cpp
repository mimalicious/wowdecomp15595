#include "PartyFrame.h"
#include <cstring>
#include <ctime>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/BattlenetUI.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F33318
uint32_t CGPartyInfo::m_memberRoles[4];

// STATIC: 0x00F33328
int32_t CGPartyInfo::m_readyCheckReady[4];

// STATIC: 0x00F33338
int32_t CGPartyInfo::m_readyCheckPending[4];

// STATIC: 0x00F33348
uint8_t CGPartyInfo::m_memberAssignments[4];

// STATIC: 0x00F33350
WOWGUID CGPartyInfo::m_members[4];

// STATIC: 0x00F33370
WOWGUID CGPartyInfo::m_leader;

// STATIC: 0x00F33378
WOWGUID CGPartyInfo::m_lootMaster;

// STATIC: 0x00F33380
uint32_t CGPartyInfo::m_dungeonDifficulty;

// STATIC: 0x00F33384
uint32_t CGPartyInfo::m_raidDifficulty;

// STATIC: 0x00F33388
uint32_t CGPartyInfo::m_nextDifficultyChange;

// STATIC: 0x00F3338C
uint8_t CGPartyInfo::m_playerAssignments;

// STATIC: 0x00F3338D
uint8_t CGPartyInfo::m_playerLFGFlags;

// STATIC: 0x00F3338E
uint8_t CGPartyInfo::m_playerRole;

// STATIC: 0x00F3338F
uint8_t CGPartyInfo::m_onePersonParty;

// STATIC: 0x00F33390
WOWGUID CGPartyInfo::m_realLeader;

// STATIC: 0x00F33398
uint32_t CGPartyInfo::m_realNumMembers;

// STATIC: 0x00F3339C
uint32_t CGPartyInfo::m_lastAssignmentTime;

// STATIC: 0x00F333A0
int32_t CGPartyInfo::m_optOutOfLoot;

// STATIC: 0x00F333A4
int32_t CGPartyInfo::m_lfgSlot;

// STATIC: 0x00F333A8
int32_t CGPartyInfo::m_lfgRestricted;

// STATIC: 0x00F333AC
int32_t CGPartyInfo::m_lfgUnk;

// STATIC: 0x00F333B0
int32_t CGPartyInfo::m_pendingInviteRoles;

// STATIC: 0x00F333B8
PartyMemberState s_partyMemberStates[4];

// STATIC: 0x00D107B8
int32_t CGPartyInfo::m_leaderIndex = -1;

// STATIC: 0x00D107BC
int32_t CGPartyInfo::m_lootMethod = 3;

// STATIC: 0x00D107C0
int32_t CGPartyInfo::m_lootThreshold = 2;

int32_t CGPartyInfo::m_dynamicDifficulty;

// OFFSET: 0x008FC540/0x1406538C0
static void LootMasterNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false)) {
        CGGameUI::DisplayError(ERR_NEW_LOOT_MASTER_S, name->m_name);
    }
}

// OFFSET: 0x008FC590/0x140653910
WOWGUID CGPartyInfo::GetMember(uint32_t index) {
    if (!(index < 4)) {
        return WOWGUID();
    }

    return CGPartyInfo::m_members[index];
}

// OFFSET: 0x008FC5C0/0x140653930
void CGPartyInfo::SetLeader(WOWGUID leader) {
    if (leader == CGPartyInfo::m_leader) {
        return;
    }

    CGPartyInfo::m_leader = leader;
    CGPartyInfo::m_leaderIndex = -1;

    if (leader) {
        uint32_t i;

        for (i = 0; i < 4; i++) {
            if (CGPartyInfo::m_members[i] == leader) {
                break;
            }
        }

        if (i < 4) {
            CGPartyInfo::m_leaderIndex = i;
        }
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_LEADER_CHANGED);
}

// PROVEN: dd5ea47a IDENTICAL/NONE
// OFFSET: 0x008FC650/0x1406539A0
uint32_t CGPartyInfo::NumMembers() {
    uint32_t count = 0;

    for (uint32_t i = 0; i < sizeof(((CGPartyInfo*)0)->m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i]) {
            count++;
        }
    }

    return count;
}

// PROVEN: a0c63dee IDENTICAL/NONE
// OFFSET: 0x008FC6A0/0x1406539D0
void CGPartyInfo::UpdateRealStats(uint32_t count, WOWGUID leader) {
    if (count <= 4) {
        CGPartyInfo::m_realNumMembers = count;
    }

    CGPartyInfo::m_realLeader = leader;
}

// OFFSET: 0x008FC6D0/0x1406539F0
int32_t CGPartyInfo::SetLootMethod(int32_t method, WOWGUID master, int32_t threshold, int32_t force) {
    int32_t changed = 0;

    if (force || CGPartyInfo::m_lootMethod != method) {
        CGPartyInfo::m_lootMethod = method;

        switch (method) {
            case 0:
                CGGameUI::DisplayError(ERR_SET_LOOT_FREEFORALL);
                break;

            case 1:
                CGGameUI::DisplayError(ERR_SET_LOOT_ROUNDROBIN);
                break;

            case 2:
                CGGameUI::DisplayError(ERR_SET_LOOT_MASTER);
                break;

            case 3:
                CGGameUI::DisplayError(ERR_SET_LOOT_GROUP);
                break;

            case 4:
                CGGameUI::DisplayError(ERR_SET_LOOT_NBG);
                break;

            default:
                break;
        }

        changed = 1;
    }

    if (force || CGPartyInfo::m_lootMaster != master) {
        CGPartyInfo::m_lootMaster = master;

        if (master && method == 2) {
            if (NameCache* name = g_nameDBCache.GetRecord(master.m_guid, master, &LootMasterNameCallback, NULL, false)) {
                CGGameUI::DisplayError(ERR_NEW_LOOT_MASTER_S, name->m_name);
            }
        }

        changed = 1;
    }

    if (force || CGPartyInfo::m_lootThreshold != threshold) {
        CGPartyInfo::m_lootThreshold = threshold;
        char token[32];
        SStrPrintf(token, sizeof(token), "ITEM_QUALITY%d_DESC", threshold);
        CGGameUI::DisplayError(ERR_SET_LOOT_THRESHOLD_S, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
        Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_LOOT_METHOD_CHANGED);
        return 1;
    }

    if (changed) {
        Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_LOOT_METHOD_CHANGED);
    }

    return changed;
}

// OFFSET: 0x008FC820/0x140653B40
void CGPartyInfo::SetDungeonDifficulty(uint32_t difficulty) {
    CGPartyInfo::m_dungeonDifficulty = difficulty;
    CGGameObject_C::UpdateDungeonDifficulty();
}

// OFFSET: 0x008FC840/0x140653B50
void CGPartyInfo::SetRaidDifficulty(uint32_t difficulty) {
    CGPartyInfo::m_raidDifficulty = difficulty;
    CGGameObject_C::UpdateDungeonDifficulty();
}

// OFFSET: 0x008FC860/0x140653B60
PartyMemberState* CGPartyInfo::GetRemoteStats(const WOWGUID& guid) {
    if (guid) {
        for (uint32_t i = 0; i < 4; i++) {
            if (CGPartyInfo::m_members[i] == guid) {
                return &s_partyMemberStates[i];
            }
        }
    }

    return NULL;
}

// OFFSET: 0x008FC8B0/0x140653BA0
PartyMemberState* CGPartyInfo::GetRemoteStatsFromPet(const WOWGUID& petGuid) {
    for (uint32_t i = 0; i < 4; i++) {
        if (CGPartyInfo::m_members[i] && s_partyMemberStates[i].m_pet.m_guid == petGuid) {
            return &s_partyMemberStates[i];
        }
    }

    return NULL;
}

// OFFSET: 0x008FC900/0x140653BF0
PartyMemberPetState* CGPartyInfo::GetRemotePetStats(const WOWGUID& guid) {
    if (guid) {
        for (uint32_t i = 0; i < 4; i++) {
            if (CGPartyInfo::m_members[i] && (s_partyMemberStates[i].m_flags & 1) && s_partyMemberStates[i].m_pet.m_guid == guid) {
                return &s_partyMemberStates[i].m_pet;
            }
        }
    }

    return NULL;
}

// PROVEN: dec597de IDENTICAL/NONE
// OFFSET: 0x008FC970
void CGPartyInfo::GetReadyCheckInfo(PartyReadyCheckInfo* info, uint32_t index) {
    info->m_guid = WOWGUID();
    info->m_pending = 0;
    info->m_ready = 0;

    if (index < 4) {
        info->m_guid = CGPartyInfo::m_members[index];
        info->m_pending = CGPartyInfo::m_readyCheckPending[index];
        info->m_ready = CGPartyInfo::m_readyCheckReady[index];
    }
}

// PROVEN: fded74e1 IDENTICAL/NONE
// OFFSET: 0x008FC9C0
void CGPartyInfo::SetReadyCheckInfo(const PartyReadyCheckInfo* info, uint32_t index) {
    if (index < 4 && CGPartyInfo::m_members[index] == info->m_guid) {
        CGPartyInfo::m_readyCheckPending[index] = info->m_pending;
        CGPartyInfo::m_readyCheckReady[index] = info->m_ready;
    }
}

// OFFSET: 0x008FCA00
int32_t CanSendPartyCommand() {
    return !CGPartyInfo::m_lastAssignmentTime || static_cast<int32_t>(FrameTime::GetCurTimeMs() - CGPartyInfo::m_lastAssignmentTime - 750) >= 0;
}

// OFFSET: 0x008FCA30
void CGPartyInfo::ClearRecruitAFriend(WOWGUID guid) {
    if (PartyMemberState* stats = CGPartyInfo::GetRemoteStats(guid)) {
        stats->m_flags &= ~0x100;
    }
}

// PROVEN: 74005542 IDENTICAL/NONE
// OFFSET: 0x008FCA50
void CGPartyInfo::SetLfgSlot(int32_t slot) {
    if (CGPartyInfo::m_lfgSlot != slot) {
        CGPartyInfo::m_lfgSlot = slot;
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE, NULL);
    }
}

// PROVEN: dad316ea IDENTICAL/NONE
// OFFSET: 0x008FCA80
void CGPartyInfo::SetLfgRestricted(int32_t restricted) {
    if ((CGPartyInfo::m_lfgRestricted != 0) != (restricted != 0)) {
        CGPartyInfo::m_lfgRestricted = restricted;
        FrameScript_SignalEvent(SCRIPT_EVENT_PARTY_LFG_RESTRICTED, NULL);
    }
}

// OFFSET: 0x008FCDE0
int32_t Packet_SMSG_INSTANCE_SAVE_CREATED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t debug;
    char text[3000];
    msg->Get(debug);

    if (debug == 0) {
        SStrCopy(text, FrameScript_GetText("INSTANCE_SAVED", -1, GENDER_NOT_APPLICABLE), sizeof(text));
        CGGameUI::SetInstanceLockTime(0, 0, 0);
    } else if (debug == 1) {
        SStrPrintf(text, sizeof(text), "(Debug-Only Lock Notice) %s", FrameScript_GetText("INSTANCE_SAVED", -1, GENDER_NOT_APPLICABLE));
    }

    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
    return 1;
}

// OFFSET: 0x008FCEA0
static void PartyMemberNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false)) {
        char fullName[256];
        CGAutoCompleteName::SetName(guid, name->FullName(fullName, sizeof(fullName)), false);
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBERS_CHANGED);
}

// OFFSET: 0x008FCF10/0x140654310
void CGPartyInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_INSTANCE_SAVE_CREATED);
    ClientServices::ClearMessageHandler(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT);
    ClientServices::ClearMessageHandler(MSG_SET_DUNGEON_DIFFICULTY);
    ClientServices::ClearMessageHandler(MSG_SET_RAID_DIFFICULTY);
    ClientPartyInvite::s_handler = NULL;
    ConsoleCommandUnregister("changePlayerDifficulty");
}

// OFFSET: 0x008FCF70
static void DifficultyChangeNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(id, requester, NULL, NULL, false)) {
        char fullName[305];
        CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_OTHER_HEROIC_S, name->FullName(fullName, sizeof(fullName)));
    }
}

// OFFSET: 0x008FCFD0/0x140654430
void CGPlayer_C::InviteToon(uint32_t toonId, uint32_t roles) {
    CGTutorial::FlagTutorial(17);
    UserClientPartyInvite msg;

    if (BattlenetUI::GetToonInviteInfo(toonId, msg.m_targetCfgRealmID, msg.m_targetGUID, msg.m_targetName, sizeof(msg.m_targetName), msg.m_targetRealm, sizeof(msg.m_targetRealm))) {
        msg.m_proposedRoles = roles;
        ClientServices::Send(&msg);
    }
}

// OFFSET: 0x008FD050/0x1406544C0
void __stdcall CGPlayer_C::Uninvite(WOWGUID guid, const char* name) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_UNINVITE_GUID));
    msg.Put(guid.m_guid);
    msg.PutString(name);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008FD0E0/0x140654570
void __stdcall CGPlayer_C::LeaveGroup() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_DISBAND));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008FD150
int32_t Packet_SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t result;
    msg->Get(result);

    switch (result) {
        case 0: {
            uint32_t cooldown;
            msg->Get(cooldown);
            char duration[1024];
            CGTooltip::GetDurationString(duration, sizeof(duration), 1000 * cooldown, "INT_SPELL_DURATION", NULL, 1, 0);
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_COOLDOWN_S, duration);
            break;
        }

        case 1:
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_WORLDSTATE);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_ENCOUNTER);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_COMBAT);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_PLAYER_BUSY);
            break;

        case 5: {
            uint32_t delay;
            msg->Get(delay);
            CGPartyInfo::m_nextDifficultyChange = delay + static_cast<uint32_t>(::time(NULL));
            LoadingScreenEnable(CGGameUI::m_iCurrentMapID, false);
            break;
        }

        case 6:
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_ALREADY_STARTED);
            break;

        case 7: {
            uint32_t difficultyId;
            msg->Get(difficultyId);
            MapDifficultyRec* rec = g_mapDifficultyDB.GetRecord(difficultyId);

            if (rec && rec->m_message) {
                CGChat::AddChatMessage(rec->m_message, static_cast<SLASH_COMMAND_ID>(0), NULL);
            }

            break;
        }

        case 8: {
            SmartGUID guid;
            *msg >> guid;
            WOWGUID requester = ClntObjMgrGetActivePlayer();

            if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, &DifficultyChangeNameCallback, NULL, false)) {
                char fullName[305];
                CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_OTHER_HEROIC_S, name->FullName(fullName, sizeof(fullName)));
            }

            break;
        }

        case 9:
            CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_HEROIC_INSTANCE_ALREADY_RUNNING);
            break;

        case 10:
            CGGameUI::DisplayError(ERR_DIFFICULTY_DISABLED_IN_LFG);
            break;

        case 11: {
            uint32_t mapId;
            msg->Get(mapId);
            uint32_t difficulty;
            msg->Get(difficulty);

            if (static_cast<int32_t>(mapId) == CGGameUI::m_iCurrentMapID) {
                CGGameUI::SetInstanceDifficulty(difficulty);
            }

            LoadingScreenDisable();
            ConsoleWrite("Changed difficulty successfully", DEFAULT_COLOR);
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_DIFFICULTY_CHANGED, NULL);
            break;
        }

        default:
            break;
    }

    return 1;
}

// PROVEN: ec05f649 IDENTICAL/NONE
// OFFSET: 0x004DA4A0
bool CGPartyInfo::InParty() {
    return CGPartyInfo::m_members[0].m_guid != 0;
}

// OFFSET: 0x004E8260/0x14012A8A0
PartyMemberState* CGPartyInfo::GetMemberState(uint32_t index) {
    if (CGPartyInfo::m_members[index]) {
        return &s_partyMemberStates[index];
    }

    return NULL;
}

// PROVEN: ab72033d IDENTICAL/NONE
// OFFSET: 0x008FD3E0/0x140654910
int32_t CGPartyInfo::IsMember(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    if (guid == ClntObjMgrGetActivePlayer()) {
        return 1;
    }

    for (uint32_t i = 0; i < sizeof(((CGPartyInfo*)0)->m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008FD440/0x140654960
WOWGUID CGPartyInfo::GetMemberPet(uint32_t index) {
    if (index >= 4 || !CGPartyInfo::m_members[index]) {
        return WOWGUID();
    }

    if (CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[index], TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        CGUnitData* unit = player->m_unit;
        if (unit->m_unitFieldCharm) {
            return unit->m_unitFieldCharm;
        }

        return unit->m_unitFieldSummon;
    }

    return s_partyMemberStates[index].m_flags & 1 ? s_partyMemberStates[index].m_pet.m_guid : WOWGUID();
}

// OFFSET: 0x008FD4C0/0x140654A10
void CGPartyInfo::EnableMember(CGPlayer_C* player, int32_t enable) {
    WOWGUID guid = player->m_obj->m_guid;
    uint32_t index;

    for (index = 0; index < 4; index++) {
        if (CGPartyInfo::m_members[index] == guid) {
            break;
        }
    }

    if (index == 4) {
        return;
    }

    if (!enable) {
        player->FillPartyMemberState(&s_partyMemberStates[index]);
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
        msg.Put(player->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBER_DISABLE);
        return;
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBER_ENABLE);
}

// OFFSET: 0x008FD5C0
uint32_t CGPartyInfo::GetGroupRole(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    if (guid == ClntObjMgrGetActivePlayer()) {
        return CGPartyInfo::m_playerRole;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return CGPartyInfo::m_memberRoles[i];
        }
    }

    return 0;
}

// OFFSET: 0x008FD620/0x140654B80
int32_t CGPartyInfo::IsPlayerOrPetInParty(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        WOWGUID member = i >= 4 ? WOWGUID() : CGPartyInfo::m_members[i];

        if (guid == member) {
            return 1;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(i >= 4 ? WOWGUID() : CGPartyInfo::m_members[i], TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGUnitData* unit = player->m_unit;
            const WOWGUID& pet = unit->m_unitFieldCharm ? unit->m_unitFieldCharm : unit->m_unitFieldSummon;

            if (pet == guid) {
                return 1;
            }
        } else if ((s_partyMemberStates[i].m_flags & 1) && s_partyMemberStates[i].m_pet.m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008FD700
uint8_t CGPartyInfo::GetPartyAssignments(const WOWGUID& guid) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        return CGPartyInfo::m_playerAssignments;
    }

    for (uint32_t i = 0; i < 4; i++) {
        WOWGUID member;

        if (i < 4) {
            member = CGPartyInfo::m_members[i];
        } else {
            member = WOWGUID();
        }

        if (member == guid) {
            return CGPartyInfo::m_memberAssignments[i];
        }
    }

    return 0;
}

// OFFSET: 0x008FD760/0x140654CD0
int32_t CGPartyInfo::GetPartyAssignments(lua_State* L, int32_t assignment) {
    if (CGRaidInfo::m_numMembers) {
        return CGRaidInfo::GetPartyAssignments(L, assignment);
    }

    uint8_t flag = 0;

    if (assignment == 0) {
        flag = 2;
    } else if (assignment == 1) {
        flag = 4;
    }

    if (lua_isstring(L, 2)) {
        const char* name = lua_tolstring(L, 2, NULL);
        WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 5, StringToBOOL(L, 3, 0), 3.402823466e38f, 0);

        if (!guid) {
            return 0;
        }

        if (CGPartyInfo::GetPartyAssignments(guid) & flag) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    int32_t count = 0;

    for (uint32_t i = 1; i <= 4; i++) {
        if (flag & CGPartyInfo::m_memberAssignments[i - 1]) {
            lua_checkstack(L, ++count);
            lua_pushnumber(L, i);
        }
    }

    return count;
}

// OFFSET: 0x008FD890/0x140654E80
void CGPartyInfo::SetOptOutOfLoot(int32_t optOut) {
    CGPartyInfo::m_optOutOfLoot = optOut;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_OPT_OUT_OF_LOOT));
    msg.Put(static_cast<uint8_t>(optOut != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008FD920/0x140654F20
bool CGGameUI::CanChangePlayerDifficulty() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map || map->m_instanceType != 2 || !CGGameUI::IsDynamicDifficultyMap()) {
        return false;
    }

    if (CGRaidInfo::m_numMembers && CGPartyInfo::m_leader && CGPartyInfo::m_leader == ClntObjMgrGetActivePlayer()) {
        return true;
    }

    return false;
}

// OFFSET: 0x008FD990
void CGPartyInfo::CheckClearLFGSlot() {
    int32_t count = CGPartyInfo::m_members[0] ? 1 : 0;

    if (CGPartyInfo::m_members[1]) {
        count++;
    }

    if (CGPartyInfo::m_members[2]) {
        count++;
    }

    if (CGPartyInfo::m_members[3]) {
        count++;
    }

    if (count || !CGPartyInfo::m_lfgSlot) {
        return;
    }

    const LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(CGPartyInfo::m_lfgSlot & 0xFFFFFF);

    if (dungeon && dungeon->m_mapID != CGGameUI::m_iCurrentMapID) {
        CGPartyInfo::m_lfgSlot = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE, NULL);
    }
}

// OFFSET: 0x008FDF50/0x1406541F0
int32_t CCommand_SetDifficulty(const char* command, const char* arguments) {
    uint32_t difficulty = SStrToInt(arguments);

    if (!(difficulty > 3)) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CHANGEPLAYER_DIFFICULTY));
        msg.Put(difficulty);
        msg.Finalize();
        ClientServices::Send2(&msg);
        return 1;
    }

    ConsoleWrite("Invalid difficulty", ERROR_COLOR);
    return 1;
}

// OFFSET: 0x008FDFD0/0x140655770
void CGPartyInfo::LeaveWorld(bool reload) {
    if (reload) {
        UserClientPartyInviteResponse msg;
        msg.m_accept = false;
        ClientServices::Send(&msg);
    }
}

// OFFSET: 0x008FE000
bool CGPartyInfo::ClientPartyInviteHandler(ClientPartyInvite* msg) {
    if (msg->m_mustBeBNetFriend && !BattlenetUI::IsToonFriend(msg->m_inviterCfgRealmID)) {
        UserClientPartyInviteResponse response;
        response.m_accept = false;
        ClientServices::Send(&response);
        return true;
    }

    char name[306];

    if (msg->m_isXRealm) {
        SStrPrintf(name, sizeof(name), "%s-%s", msg->m_inviterName, msg->m_inviterRealmName);
    } else {
        SStrCopy(name, msg->m_inviterName, 305);
    }

    if (msg->m_canAccept) {
        CGPartyInfo::m_pendingInviteRoles = msg->m_proposedRoles;
        FrameScript_SignalEvent(SCRIPT_EVENT_PARTY_INVITE_REQUEST, "%s%b%b%b", name, (msg->m_proposedRoles >> 1) & 1, (msg->m_proposedRoles >> 2) & 1, (msg->m_proposedRoles >> 3) & 1);
        CGGameUI::DisplayError(ERR_INVITED_TO_GROUP_SS, name, name);
    } else {
        CGGameUI::DisplayError(ERR_INVITED_ALREADY_IN_GROUP_SS, name, name);
    }

    return true;
}

// OFFSET: 0x008FE130/0x1406558E0
void CGPlayer_C::Invite(const char* name, uint32_t roles) {
    CGTutorial::FlagTutorial(17);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (!SStrCmpUTF8I(name, player->GetUnitName(NULL, 1), STORM_MAX_STR)) {
        CGGameUI::DisplayError(ERR_INVITE_SELF);
        return;
    }

    UserClientPartyInvite msg;
    SStrCopy(msg.m_targetName, name, 305);
    msg.m_targetCfgRealmID = 0;
    memset(msg.m_targetGUID, 0, sizeof(msg.m_targetGUID));
    SStrCopy(msg.m_targetRealm, "", sizeof(msg.m_targetRealm));
    msg.m_proposedRoles = roles;
    ClientServices::Send(&msg);
}

// OFFSET: 0x008FE240/0x140655A20
int32_t CGPartyInfo::IsMemberPet(const WOWGUID& guid, int32_t* index) {
    if (!guid) {
        return 0;
    }

    if (CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet == guid) {
            if (index) {
                *index = -1;
            }

            return 1;
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (!CGPartyInfo::m_members[i]) {
            continue;
        }

        CGPlayer_C* member = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[i], TYPEMASK_PLAYER, __FILE__, __LINE__));
        bool match;

        if (member) {
            const WOWGUID& pet = member->m_unit->m_unitFieldCharm ? member->m_unit->m_unitFieldCharm : member->m_unit->m_unitFieldSummon;
            match = pet == guid;
        } else {
            match = (s_partyMemberStates[i].m_flags & 1) && s_partyMemberStates[i].m_pet.m_guid == guid;
        }

        if (match) {
            if (index) {
                *index = i;
            }

            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008FE340/0x140655B50
int32_t CGPartyInfo::IsMemberOrPet(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    if (guid == ClntObjMgrGetActivePlayer()) {
        return 1;
    }

    if (CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet == guid) {
            return 1;
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (!CGPartyInfo::m_members[i]) {
            continue;
        }

        if (CGPartyInfo::m_members[i] == guid) {
            return 1;
        }

        if (CGPlayer_C* member = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[i], TYPEMASK_PLAYER, __FILE__, __LINE__))) {
            const WOWGUID& pet = member->m_unit->m_unitFieldCharm ? member->m_unit->m_unitFieldCharm : member->m_unit->m_unitFieldSummon;

            if (pet == guid) {
                return 1;
            }
        } else if ((s_partyMemberStates[i].m_flags & 1) && s_partyMemberStates[i].m_pet.m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008FE440
void CGPartyInfo::RemoveAll(int32_t notify) {
    uint32_t dungeonDifficulty = CGGameUI::GetCurrentDungeonDifficulty();
    uint32_t raidDifficulty = CGGameUI::GetCurrentRaidDifficulty();

    for (uint32_t i = 0; i < 4; i++) {
        if (!CGPartyInfo::m_members[i]) {
            continue;
        }

        if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[i], TYPEMASK_UNIT, __FILE__, __LINE__))) {
            const WOWGUID& petGuid = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

            if (petGuid) {
                if (CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon, TYPEMASK_UNIT, __FILE__, __LINE__))) {
                    pet->UnregisterScript(32);
                } else if (notify) {
                    CGGameUI::ClearTarget(unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon, 1);
                }
            }

            unit->UnregisterScript(16);
            unit->UpdateReaction(true);
        } else if (notify) {
            CGGameUI::ClearTarget(CGPartyInfo::m_members[i], 1);
        }

        CGPartyInfo::m_members[i] = WOWGUID();
        CGPartyInfo::m_memberAssignments[i] = 0;
        CGPartyInfo::m_memberRoles[i] = 0;
    }

    if (CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        player->UpdateReaction(true);
    }

    CGPartyInfo::m_leader = WOWGUID();
    CGPartyInfo::m_leaderIndex = 0;
    CGPartyInfo::m_playerAssignments = 0;
    CGPartyInfo::m_playerLFGFlags = 0;
    CGPartyInfo::m_playerRole = 0;
    CGPartyInfo::m_lfgSlot = 0;
    CGPartyInfo::m_lfgRestricted = 0;
    CGPartyInfo::m_lfgUnk = 0;
    CGPartyInfo::m_onePersonParty = 0;
    CGAutoCompleteName::RemoveFlagsFromAllNodes(1);

    if (notify) {
        CGGameUI::CheckInstanceDifficultyMessages(dungeonDifficulty, raidDifficulty);
        Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBERS_CHANGED);
        Script_SendUniqueSignal(SCRIPT_EVENT_PLAYER_ROLES_ASSIGNED);
    }
}

// OFFSET: 0x008FE610
void CGPartyInfo::ResetLootMethod() {
    CGPartyInfo::m_lootMethod = 3;
    CGPartyInfo::m_lootMaster = WOWGUID();
    CGPartyInfo::m_lootThreshold = 2;
    CGPartyInfo::SetOptOutOfLoot(0);
}

// OFFSET: 0x008FE640/0x140654FB0
void CGPartyInfo::ChangeRaidDifficulty(int32_t difficulty) {
    if (!CGGameUI::CanChangePlayerDifficulty()) {
        return;
    }

    if (CGPartyInfo::m_nextDifficultyChange && static_cast<int32_t>(static_cast<uint32_t>(::time(NULL)) - CGPartyInfo::m_nextDifficultyChange) < 0) {
        char duration[1024];
        CGTooltip::GetDurationString(duration, sizeof(duration), 1000 * (CGPartyInfo::m_nextDifficultyChange - static_cast<uint32_t>(::time(NULL))), "INT_SPELL_DURATION", NULL, 1, 0);
        CGGameUI::DisplayError(ERR_DIFFICULTY_CHANGE_COOLDOWN_S, duration);
        return;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CHANGEPLAYER_DIFFICULTY));
    msg.Put(static_cast<uint32_t>(difficulty));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008FF200/0x140656B10
void CGPartyInfo::InitializeGame() {
    CGPartyInfo::m_members[0] = WOWGUID();
    CGPartyInfo::m_members[1] = WOWGUID();
    CGPartyInfo::m_members[2] = WOWGUID();
    CGPartyInfo::m_members[3] = WOWGUID();
    memset(CGPartyInfo::m_memberAssignments, 0, sizeof(CGPartyInfo::m_memberAssignments));
    CGPartyInfo::m_memberRoles[0] = 0;
    CGPartyInfo::m_memberRoles[1] = 0;
    CGPartyInfo::m_memberRoles[2] = 0;
    CGPartyInfo::m_memberRoles[3] = 0;
    CGPartyInfo::m_leader = WOWGUID();
    CGPartyInfo::m_leaderIndex = -1;
    CGPartyInfo::m_dungeonDifficulty = 0;
    CGPartyInfo::m_raidDifficulty = 0;
    CGPartyInfo::m_playerAssignments = 0;
    CGPartyInfo::m_playerLFGFlags = 0;
    CGPartyInfo::m_playerRole = 0;
    CGPartyInfo::m_realLeader = WOWGUID();
    CGPartyInfo::m_realNumMembers = 0;
    CGPartyInfo::m_lootMethod = 3;
    CGPartyInfo::m_lootMaster = WOWGUID();
    CGPartyInfo::m_lootThreshold = 2;
    CGPartyInfo::m_nextDifficultyChange = 0;
    CGPartyInfo::m_lfgSlot = 0;
    CGPartyInfo::m_lfgRestricted = 0;
    CGPartyInfo::m_lfgUnk = 0;
    CGPartyInfo::m_onePersonParty = 0;
    ClientServices::SetMessageHandler(SMSG_INSTANCE_SAVE_CREATED, Packet_SMSG_INSTANCE_SAVE_CREATED, NULL);
    ClientServices::SetMessageHandler(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT, Packet_SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT, NULL);
    ClientPartyInvite::s_handler = &CGPartyInfo::ClientPartyInviteHandler;
    ConsoleCommandRegister("changePlayerDifficulty", CCommand_SetDifficulty, CATEGORY_DEFAULT, NULL, NULL);
}

// OFFSET: 0x008FF310
void CGPartyInfo::AddMember(WOWGUID guid, int32_t online, const PartyMemberState* state, uint8_t assignments, uint32_t role, const PartyReadyCheckInfo* readyCheck) {
    if (!guid) {
        return;
    }

    uint32_t index = 0;

    while (index < 4 && CGPartyInfo::m_members[index]) {
        index++;
    }

    if (index == 4) {
        return;
    }

    CGPartyInfo::m_members[index] = guid;
    CGPartyInfo::m_memberRoles[index] = role;
    CGPartyInfo::m_memberAssignments[index] = assignments;
    PartyMemberState* stats = &s_partyMemberStates[index];

    if (state) {
        stats->Copy(state);
    } else {
        stats->Reset();
        stats->m_health = 1;
        stats->m_maxHealth = 1;
        stats->m_power = 1;
        stats->m_maxPower = 1;
    }

    if (online) {
        stats->m_flags |= 0x1;
        CGAutoCompleteName::AddFlags(guid, 33, 0, NULL);
    } else {
        stats->m_flags &= ~0x1;
        CGAutoCompleteName::AddFlags(guid, 1, 0, NULL);
        CGAutoCompleteName::RemoveFlags(guid, 32);
    }

    if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        unit->RegisterScript(16);
        const WOWGUID& petGuid = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

        if (CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(petGuid, TYPEMASK_UNIT, __FILE__, __LINE__))) {
            pet->RegisterScript(32);
        }

        unit->UpdateReaction(true);
    } else if (!state) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
        msg.Put(guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (!CGPartyInfo::m_realNumMembers) {
        if (CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
            player->UpdateReaction(true);
        }
    }

    if (!online) {
        CGGameUI::ClearTarget(guid, 1);
    }

    if (readyCheck) {
        CGPartyInfo::m_readyCheckPending[index] = readyCheck->m_pending;
        CGPartyInfo::m_readyCheckReady[index] = readyCheck->m_ready;
    } else {
        CGPartyInfo::m_readyCheckPending[index] = 0;
        CGPartyInfo::m_readyCheckReady[index] = 0;
    }

    if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, &PartyMemberNameCallback, NULL, false)) {
        char fullName[256];
        CGAutoCompleteName::SetName(guid, name->FullName(fullName, sizeof(fullName)), false);
    }

    if (state) {
        uint32_t changes = CGPlayer_C::BuildPartyMemberStatsChanges(const_cast<PartyMemberState&>(*state), *stats);
        CGPlayer_C::DispatchPartyMemberStatsChanges(guid, changes, *state, *stats);
    }
}

// OFFSET: 0x005689A0/0x1401C7C60
void PartyMemberState::Reset() {
    this->m_guid = WOWGUID();
    this->m_health = 0;
    this->m_maxHealth = 0;
    this->m_vehicleSeatId = 0;
    this->m_numAuras = 0;
    this->m_flags = 0;
    this->m_power = 0;
    this->m_maxPower = 0;
    this->m_level = 0;
    this->m_zoneID = 0;
    this->unk_0022 = 0;
    this->m_posX = 0;
    this->m_posY = 0;
    this->m_posZ = 0;
    this->m_powerType = 0;
    memset(&this->m_pet, 0, offsetof(PartyMemberPetState, m_auras));
    this->m_auras.Clear();
    this->m_phaseShift.m_flags = 0;
    this->m_phaseShift.m_phases.SetCount(0);
}

// OFFSET: 0x00564110/0x1401C2140
void PartyMemberState::Copy(const PartyMemberState* source) {
    this->m_guid = source->m_guid;
    this->m_numAuras = source->m_numAuras;
    this->m_vehicleSeatId = source->m_vehicleSeatId;
    this->m_maxHealth = source->m_maxHealth;
    this->m_health = source->m_health;
    this->unk_0022 = source->unk_0022;
    this->m_power = source->m_power;
    this->m_flags = source->m_flags;
    this->m_posZ = source->m_posZ;
    this->m_level = source->m_level;
    this->m_posY = source->m_posY;
    this->m_maxPower = source->m_maxPower;
    this->m_posX = source->m_posX;
    this->m_zoneID = source->m_zoneID;
    this->m_powerType = source->m_powerType;
    memcpy(&this->m_pet, &source->m_pet, offsetof(PartyMemberPetState, m_auras));

    if (this->m_numAuras > this->m_auras.Count()) {
        this->m_auras.SetCount(this->m_numAuras);
    }

    uint32_t i = 0;

    for (; i < this->m_numAuras; i++) {
        this->m_auras[i].m_spellId = source->m_auras.m_data[i].m_spellId;
        this->m_auras[i].m_flags = source->m_auras.m_data[i].m_flags;

        if (this->m_auras[i].m_flags & 0x40) {
            this->m_auras[i].m_effectAmounts[0] = source->m_auras.m_data[i].m_effectAmounts[0];
            this->m_auras[i].m_effectAmounts[1] = source->m_auras.m_data[i].m_effectAmounts[1];
            this->m_auras[i].m_effectAmounts[2] = source->m_auras.m_data[i].m_effectAmounts[2];
        }
    }

    for (; i < this->m_auras.Count(); i++) {
        this->m_auras[i].m_spellId = 0;
        this->m_auras[i].m_flags = 0;
    }

    if (this->m_pet.m_numAuras > this->m_pet.m_auras.Count()) {
        this->m_pet.m_auras.SetCount(this->m_pet.m_numAuras);
    }

    i = 0;

    for (; i < this->m_pet.m_numAuras; i++) {
        this->m_pet.m_auras[i].m_spellId = source->m_pet.m_auras.m_data[i].m_spellId;
        this->m_pet.m_auras[i].m_flags = source->m_pet.m_auras.m_data[i].m_flags;

        if (this->m_pet.m_auras[i].m_flags & 0x40) {
            this->m_pet.m_auras[i].m_effectAmounts[0] = source->m_pet.m_auras.m_data[i].m_effectAmounts[0];
            this->m_pet.m_auras[i].m_effectAmounts[1] = source->m_pet.m_auras.m_data[i].m_effectAmounts[1];
            this->m_pet.m_auras[i].m_effectAmounts[2] = source->m_pet.m_auras.m_data[i].m_effectAmounts[2];
        }
    }

    for (; i < this->m_pet.m_auras.Count(); i++) {
        this->m_pet.m_auras[i].m_spellId = 0;
        this->m_pet.m_auras[i].m_flags = 0;
    }

    this->m_phaseShift = source->m_phaseShift;
}
