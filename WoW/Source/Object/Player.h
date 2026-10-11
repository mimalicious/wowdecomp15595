#ifndef WOW_SOURCE_OBJECT_PLAYER_H
#define WOW_SOURCE_OBJECT_PLAYER_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Object/WGUID.h"

class CGPlayerLocalData;

struct PlayerQuestLogEntry {
    /* 0000 */ uint32_t m_questId;
    /* 0004 */ uint32_t m_state;
    /* 0008 */ uint16_t m_counts [4];
    /* 0010 */ uint32_t m_time;
};

struct PlayerSkillInfo {
    /* 0000 */ uint16_t m_skillId;
    /* 0002 */ uint16_t m_step;
    /* 0004 */ uint16_t m_rank;
    /* 0006 */ uint16_t m_maxRank;
    /* 0008 */ uint16_t m_tempBonus;
    /* 000A */ uint16_t m_permBonus;
};

class CGPlayer {
    public:
    PlayerQuestLogEntry* GetQuestLogEntry(int32_t slot) const;
    PlayerQuestLogEntry* GetQuestLogEntryByID(int32_t questId) const;
    uint32_t GetBuybackPrice(uint32_t slot);
    bool HasPvpMedal(uint8_t medal);
    static const uint32_t s_mirrorFields[299];
    static const uint32_t s_localMirrorFields[805];

    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base);
    static uint32_t GetSavedIndexFromDataIndex(uint32_t index, uint32_t count, int32_t local);
    static int32_t TotalFields();
    static int32_t TotalRemoteFields();

    /* 0000 */ CGPlayerLocalData* m_player;
    /* 0004 */ void* m_playerMirror;

    float GetDrunknessTurnValue();
    int32_t HasZoneExplored(int32_t zone);
    bool IsDrunk();
};

class CGPlayerData {
    public:
    /* 0000 */ WOWGUID m_playerDuelArbiter;
    /* 0008 */ uint32_t m_playerFlags;
    /* 000C */ uint32_t m_playerGuildrank;
    /* 0010 */ uint32_t m_playerGuilddeleteDate;
    /* 0014 */ uint32_t m_playerGuildlevel;
    /* 0018 */ uint8_t m_playerBytes [4];
    /* 001C */ uint8_t m_playerBytes2 [4];
    /* 0020 */ uint8_t m_playerBytes3 [4];
    /* 0024 */ uint32_t m_playerDuelTeam;
    /* 0028 */ uint32_t m_playerGuildTimestamp;
    /* 002C */ PlayerQuestLogEntry m_playerQuestLog [50];
    /* 0414 */ VisibleItem_C m_playerVisibleItems [19];
    /* 04AC */ uint32_t m_playerChosenTitle;
    /* 04B0 */ int32_t m_playerFakeInebriation;
    /* 04B4 */ uint32_t m_playerFieldPad0;
};

class CGPlayerLocalData : public CGPlayerData {
    public:
    /* 04B8 */ WOWGUID m_playerFieldInvSlotHead [23];
    /* 0570 */ WOWGUID m_playerFieldPackSlot1 [16];
    /* 05F0 */ WOWGUID m_playerFieldBankSlot1 [28];
    /* 06D0 */ WOWGUID m_playerFieldBankbagSlot1 [7];
    /* 0708 */ WOWGUID m_playerFieldVendorbuybackSlot1 [12];
    /* 0768 */ WOWGUID m_playerFarsight;
    /* 0770 */ uint64_t m_playerFieldKnownTitles [4];
    /* 0790 */ uint32_t m_playerXp;
    /* 0794 */ uint32_t m_playerNextLevelXp;
    /* 0798 */ uint16_t m_playerSkillLineid0 [128];
    /* 0898 */ uint16_t m_playerSkillStep0 [128];
    /* 0998 */ uint16_t m_playerSkillRank0 [128];
    /* 0A98 */ uint16_t m_playerSkillMaxRank0 [128];
    /* 0B98 */ uint16_t m_playerSkillModifier0 [128];
    /* 0C98 */ uint16_t m_playerSkillTalent0 [128];
    /* 0D98 */ uint32_t m_playerCharacterPoints;
    /* 0D9C */ uint32_t m_playerTrackCreatures;
    /* 0DA0 */ uint32_t m_playerTrackResources;
    /* 0DA4 */ uint32_t m_playerExpertise;
    /* 0DA8 */ uint32_t m_playerOffhandExpertise;
    /* 0DAC */ float m_playerBlockPercentage;
    /* 0DB0 */ float m_playerDodgePercentage;
    /* 0DB4 */ float m_playerParryPercentage;
    /* 0DB8 */ float m_playerCritPercentage;
    /* 0DBC */ float m_playerRangedCritPercentage;
    /* 0DC0 */ float m_playerOffhandCritPercentage;
    /* 0DC4 */ float m_playerSpellCritPercentage1 [7];
    /* 0DE0 */ uint32_t m_playerShieldBlock;
    /* 0DE4 */ float m_playerShieldBlockCritPercentage;
    /* 0DE8 */ float m_playerMastery;
    /* 0DEC */ uint8_t m_playerExploredZones1 [624];
    /* 105C */ uint32_t m_playerRestStateExperience;
    /* 1060 */ uint64_t m_playerFieldCoinage;
    /* 1068 */ uint32_t m_playerFieldModDamageDonePos [7];
    /* 1084 */ uint32_t m_playerFieldModDamageDoneNeg [7];
    /* 10A0 */ uint32_t m_playerFieldModDamageDonePct [7];
    /* 10BC */ uint32_t m_playerFieldModHealingDonePos;
    /* 10C0 */ float m_playerFieldModHealingPct;
    /* 10C4 */ float m_playerFieldModHealingDonePct;
    /* 10C8 */ float m_playerFieldWeaponDmgMultipliers [3];
    /* 10D4 */ float m_playerFieldModSpellPowerPct;
    /* 10D8 */ float m_playerFieldOverrideSpellPowerByApPct;
    /* 10DC */ uint32_t m_playerFieldModTargetResistance;
    /* 10E0 */ uint32_t m_playerFieldModTargetPhysicalResistance;
    /* 10E4 */ uint8_t m_playerFieldBytes [4];
    /* 10E8 */ uint32_t m_playerSelfResSpell;
    /* 10EC */ uint32_t m_playerFieldPvpMedals;
    /* 10F0 */ uint32_t m_playerFieldBuybackPrice1 [12];
    /* 1120 */ uint32_t m_playerFieldBuybackTimestamp1 [12];
    /* 1150 */ uint16_t m_playerFieldKills [2];
    /* 1154 */ uint32_t m_playerFieldLifetimeHonorbaleKills;
    /* 1158 */ uint8_t m_playerFieldBytes2 [4];
    /* 115C */ uint32_t m_playerFieldWatchedFactionIndex;
    /* 1160 */ uint32_t m_playerFieldCombatRating1 [26];
    /* 11C8 */ uint32_t m_playerFieldArenaTeamInfo11 [21];
    /* 121C */ uint32_t m_playerFieldBattlegroundRating;
    /* 1220 */ uint32_t m_playerFieldMaxLevel;
    /* 1224 */ uint32_t m_playerFieldDailyQuests1 [25];
    /* 1288 */ float m_playerRuneRegen1 [4];
    /* 1298 */ uint32_t m_playerNoReagentCost1 [3];
    /* 12A4 */ uint32_t m_playerFieldGlyphSlots1 [9];
    /* 12C8 */ uint32_t m_playerFieldGlyphs1 [9];
    /* 12EC */ uint32_t m_playerGlyphsEnabled;
    /* 12F0 */ uint32_t m_playerPetSpellPower;
    /* 12F4 */ uint16_t m_playerFieldResearching1 [16];
    /* 1314 */ uint16_t m_playerFieldReserachSite1 [16];
    /* 1334 */ uint32_t m_playerProfessionSkillLine1 [2];
    /* 133C */ float m_playerFieldUiHitModifier;
    /* 1340 */ float m_playerFieldUiSpellHitModifier;
    /* 1344 */ uint32_t m_playerFieldHomeRealmTimeOffset;
    /* 1348 */ float m_playerFieldModHaste;
    /* 134C */ float m_playerFieldModRangedHaste;
    /* 1350 */ float m_playerFieldModPetHaste;
    /* 1354 */ float m_playerFieldModHasteRegen;
};

#endif
