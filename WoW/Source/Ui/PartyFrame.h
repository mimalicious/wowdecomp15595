#ifndef WOW_SOURCE_UI_PARTYFRAME_H
#define WOW_SOURCE_UI_PARTYFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Common/PhaseShiftShared.h"
#include "Engine/Source/Storm/H/stpl.h"

class CDataStore;
class CGPlayer_C;
struct lua_State;

struct PartyAura {
    /* 0000 */ int32_t m_spellId;
    /* 0004 */ int32_t m_effectAmounts[3];
    /* 0010 */ uint16_t m_flags;
    /* 0012 */ uint16_t unk_0012;
};

class CGPartyPetState {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ char m_name[80];
    /* 0058 */ uint8_t m_powerType;
    /* 0059 */ uint8_t unk_0059;
    /* 005A */ uint16_t m_displayId;
    /* 005C */ uint32_t m_health;
    /* 0060 */ uint32_t m_maxHealth;
    /* 0064 */ uint16_t m_power;
    /* 0066 */ uint16_t m_maxPower;
    /* 0068 */ uint32_t m_numAuras;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ TSGrowableArray<PartyAura> m_auras;
};

class CGPartyMemberState {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t m_health;
    /* 000C */ uint32_t m_maxHealth;
    /* 0010 */ uint32_t m_vehicleSeatId;
    /* 0014 */ uint32_t m_numAuras;
    /* 0018 */ uint16_t m_flags;
    /* 001A */ uint16_t m_power;
    /* 001C */ uint16_t m_maxPower;
    /* 001E */ uint16_t m_level;
    /* 0020 */ uint16_t m_zoneID;
    /* 0022 */ uint16_t unk_0022;
    /* 0024 */ int16_t m_posX;
    /* 0026 */ int16_t m_posY;
    /* 0028 */ int16_t m_posZ;
    /* 002A */ uint8_t m_powerType;
    /* 002B */ uint8_t unk_002B;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ CGPartyPetState m_pet;
    /* 00B0 */ TSGrowableArray<PartyAura> m_auras;
    /* 00C0 */ CPhaseShift m_phaseShift;
};

struct PartyReadyCheckInfo {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ int32_t m_pending;
    /* 000C */ int32_t m_ready;
};

class CGPartyInfo {
    public:
    static void LeaveWorld(bool reload);
    static void ChangeRaidDifficulty(int32_t heroic);
    static void SetDungeonDifficulty(uint32_t difficulty);
    static void SetRaidDifficulty(uint32_t difficulty);
    static void LeaveWorld();
    static uint32_t m_memberRoles[4];
    static uint8_t m_memberAssignments[4];
    static uint32_t m_nextDifficultyChange;
    static uint8_t m_playerAssignments;
    static uint8_t m_playerLFGFlags;
    static uint8_t m_playerRole;
    static void InitializeGame();
    static void ShutdownGame();
    static void CheckClearLFGSlot();
    static int32_t IsMemberOrPet(const WOWGUID& guid);
    static void RegisterScriptFunctions();
    static void UnregisterScriptFunctions();
    static WOWGUID m_members[4];
    static uint32_t m_dungeonDifficulty;
    static uint32_t m_raidDifficulty;
    static int32_t m_dynamicDifficulty;
    static int32_t m_lfgRestricted;
    static int32_t m_leaderIndex;
    static int32_t m_lootThreshold;
    static uint32_t m_realNumMembers;
    static int32_t m_optOutOfLoot;
    static int32_t m_lfgSlot;
    static WOWGUID m_leader;
    static WOWGUID m_realLeader;
    static int32_t m_lootMethod;
    static WOWGUID m_lootMaster;

    static bool InParty();

    static uint32_t NumMembers();
    static uint32_t GetGroupRole(const WOWGUID& guid);
    static int32_t IsMember(const WOWGUID& guid);
    static uint8_t GetPartyAssignments(const WOWGUID& guid);
    static int32_t GetPartyAssignments(lua_State* L, int32_t assignment);
    static uint32_t m_lastAssignmentTime;
    static void RemoveAll(int32_t a1);
    static void AddMember(WOWGUID guid, int32_t online, const CGPartyMemberState* state, uint8_t assignments, uint32_t role, const PartyReadyCheckInfo* readyCheck);
    static void GetReadyCheckInfo(PartyReadyCheckInfo* info, uint32_t index);
    static void SetReadyCheckInfo(const PartyReadyCheckInfo* info, uint32_t index);
    static void ResetLootMethod();
    static void SetLeader(WOWGUID leader);
    static void SetLfgRestricted(int32_t restricted);
    static void SetLfgSlot(int32_t slot);
    static int32_t SetLootMethod(int32_t method, WOWGUID master, int32_t threshold, int32_t force);
    static void UpdateRealStats(uint32_t count, WOWGUID leader);
    static void SetOptOutOfLoot(int32_t optOut);
    static void EnableMember(CGPlayer_C* player, int32_t enable);
    static int32_t IsMemberPet(const WOWGUID& guid, int32_t* index);
    static CGPartyMemberState* GetRemoteStats(const WOWGUID& guid);
    static void ClearRecruitAFriend(WOWGUID guid);
    static CGPartyMemberState* GetRemoteStatsFromPet(const WOWGUID& petGuid);
    static CGPartyMemberState* GetMemberState(uint32_t index);
    static WOWGUID GetMember(uint32_t index);
    static int32_t IsPlayerOrPetInParty(const WOWGUID& guid);
    static CGPartyPetState* GetRemotePetStats(const WOWGUID& guid);
};

class CGPartyMemberStateRepository {
    public:
    static CGPartyPetState* FindPetState(const WOWGUID& guid);
    static CGPartyMemberState* GetState(const WOWGUID& guid);
};

extern CGPartyMemberState s_partyMemberStates[4];

WOWGUID GetPartyMemberGUID(uint32_t index);
WOWGUID GetPartyPetGUID(uint32_t index);


int32_t IsGuidInMyRaid(const WOWGUID& guid);
int32_t CanSendPartyCommand();

int32_t Packet_SMSG_INSTANCE_SAVE_CREATED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PLAYER_DIFFICULTY_CHANGE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t CCommand_SetDifficulty(const char* command, const char* arguments);

#endif
