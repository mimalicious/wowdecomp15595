#include "Player_C.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include <malloc.h>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Object/ObjectClient/UnitDisplayTransition_C.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/DanceMovesRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemBagFamilyRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemSetRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/CGPetition.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/UnitSound_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/LFGuildInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/PetitionInfo.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <new>
#include <ctime>
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include <malloc.h>
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include <cstdlib>
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include <cmath>
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include <cstdio>
#include <cstring>
#include "WoW/Source/DB/Rec/EmotesRec.h"
#include "WoW/Source/DB/Rec/EmotesTextRec.h"
#include "WoW/Source/DB/Rec/TaxiNodesRec.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Object/ObjectClient/LootRoll.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Ui/ArenaTeamInfo.h"
#include "WoW/Source/Ui/GuildRegistrar.h"
#include "WoW/Source/Ui/PetitionInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/TaxiMapFrame.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/GMTicketInfo.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/Reforge.h"
#include "WoW/Source/Ui/Transmogrify.h"
#include "WoW/Source/Ui/VoidStorageInfo.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "Engine/Source/Frame/Movie.h"
#include "WoW/Source/UIUtil/PortraitButton.h"

struct GUILDUPDATETIMESTAMP {
    /* 0000 */ uint64_t m_guildGUID;
    /* 0008 */ uint32_t m_timestamp;
    /* 000C */ uint32_t unk_000C;
};

struct RuneConvert {
    /* 0000 */ uint8_t m_rune;
    /* 0001 */ uint8_t m_type;
};

struct InitialSpellStruct {
    /* 0000 */ uint32_t m_spellId;
    /* 0004 */ uint16_t unk_0004;
    /* 0006 */ uint16_t unk_0006;
};

struct RestStateInfo {
    UIErrors m_error;
    uint32_t m_tutorial;
};

// STATIC: #0x00AD9C94
static const UIErrors s_taxiReplyErrors[13] = {
    ERR_MAX_VALUE,
    ERR_TAXIUNSPECIFIEDSERVERERROR,
    ERR_TAXINOSUCHPATH,
    ERR_TAXINOTENOUGHMONEY,
    ERR_TAXITOOFARAWAY,
    ERR_TAXINOVENDORNEARBY,
    ERR_TAXINOTVISITED,
    ERR_TAXIPLAYERBUSY,
    ERR_TAXIPLAYERALREADYMOUNTED,
    ERR_TAXIPLAYERSHAPESHIFTED,
    ERR_TAXIPLAYERMOVING,
    ERR_TAXISAMENODE,
    ERR_TAXINOTSTANDING,
};

// STATIC: 0x00E4DC24
uint32_t CGPlayer_C::s_displayId;

// STATIC: #0x00C9D328
static int32_t s_areaTriggerEnd;

// STATIC: #0x00C9D32C
static int32_t s_areaTriggerStart;

// STATIC: #0x00C9D330
static AreaTriggerRec* s_currentAreaTrigger;

// STATIC: 0x00DD4D2C
static uint32_t s_areaTriggerTimer;

// STATIC: 0x00DD4D30
static int32_t s_areaTriggerMapID;

// STATIC: 0x00DD4D34
static int32_t s_defenseSpellId;

// STATIC: 0x00DD4D38
static uint32_t s_proficiency[17];

// STATIC: 0x00DD4D7C
static uint32_t s_bindAreaId;

// STATIC: 0x00DD4D80
static uint32_t s_bindMapId;

// STATIC: 0x00DD4D84
static int32_t s_bindPointSet;

// STATIC: 0x00DD4D88
static int32_t s_playerRender;

// STATIC: 0x00DD4D8C
static uint32_t s_attackSwingErrorTime;

// STATIC: #0x00C9EA90
WOWGUID CGPlayer_C::s_lastVendorListReceived;

// STATIC: 0x00DD5020
static WOWGUID s_lastBinderGuid;

// STATIC: 0x00DD501C
static int32_t s_attackSwingError;

// STATIC: 0x00DD5028
static uint32_t s_pendingMovie;

// STATIC: 0x00DD502C
static uint32_t s_pendingMovieID;

// STATIC: 0x00DD5030
WOWGUID CGPlayer_C::s_resurrectOfferer;

// STATIC: #0x00C9EAB0
static uint32_t s_unk_C9EAB0;

// STATIC: 0x00DD5038
uint32_t CGPlayer_C::s_resurrectOfferSickness;

// STATIC: 0x00DD503C
uint32_t CGPlayer_C::s_resurrectOfferUseTimer;

// STATIC: #0x00C9EAB4
static uint32_t s_unk_C9EAB4;

// STATIC: 0x00DD5044
static uint8_t s_tabardSavePending;

// STATIC: 0x00CA56B8
static const UIErrors s_guildEmblemErrors[6] = { ERR_GUILDEMBLEM_SUCCESS, ERR_GUILDEMBLEM_INVALID_TABARD_COLORS, ERR_GUILDEMBLEM_NOGUILD, ERR_GUILDEMBLEM_NOTGUILDMASTER, ERR_GUILDEMBLEM_NOTENOUGHMONEY, ERR_MAX_VALUE };

// STATIC: 0x00DD5048
static WOWGUID s_giftWrapItem;

// STATIC: 0x00DD5058
static InspectData* s_inspectData;

// STATIC: 0x00DD5200
static InspectItem s_emptyInspectItem;

// STATIC: 0x00DD505C
uint32_t CGPlayer_C::s_lastUpdatePVPSequence;

// STATIC: 0x00DD5060
WOWGUID CGPlayer_C::s_lastUpdatePVPGroup;

// STATIC: 0x00DD5068
uint32_t CGPlayer_C::s_lastUpdatePVPTime;

// STATIC: 0x00DD506C
uint32_t CGPlayer_C::s_lastUpdateSequence;

// STATIC: 0x00DD5070
WOWGUID CGPlayer_C::s_lastUpdateGroup;

// STATIC: 0x00DD5078
uint32_t CGPlayer_C::s_lastUpdateTime;

// STATIC: #0x00C9EAD8
static WOWGUID s_lastTrainerRequest;

// STATIC: #0x00C9D7D0
static VendorItem s_vendorItems[150];

// STATIC: #0x00C9EB08
static int32_t s_questsAccepted;

// STATIC: 0x00DD4D90
static WOWGUID s_questListNpc;

// STATIC: 0x00DD4F98
static uint32_t s_questListIds[32];

// STATIC: 0x00DD4F18
static uint32_t s_questListStatus[32];

// STATIC: 0x00DD4E98
static uint32_t s_questListLevels[32];

// STATIC: 0x00DD4E18
static uint32_t s_questListFlags[32];

// STATIC: 0x00DD4D98
static uint32_t s_questListRepeatable[32];

// STATIC: 0x00DD507C
static uint32_t s_questFailedReason;

// STATIC: #0x00C9EB10
static WOWGUID s_spiritHealer;

// STATIC: #0x00C9EB18
static WOWGUID s_talentMaster;

// STATIC: 0x00DD5098
static WOWGUID s_binder;

// STATIC: 0x00DD50A0
static C3Vector s_bindPosition;

// STATIC: 0x00CA531C
static int32_t s_actionButtons[144];

// STATIC: 0x00DD50AC
static TSGrowableArray<InitialSpellStruct> s_initialSpells;

// STATIC: 0x00DD50BC
static TSGrowableArray<int32_t> s_initialCompanionSpells;

// STATIC: 0x00DD50CC
static TSGrowableArray<RuneConvert> s_pendingRuneConverts;

// STATIC: 0x00DD50DC
static TSFixedArray<uint32_t> s_weaponSkills;

// STATIC: 0x00DD50E8
static TSGrowableArray<GUILDUPDATETIMESTAMP> s_guildUpdateTimestamps;

// STATIC: 0x00DD50F8
static TSGrowableArray<ITEMSWAP> s_itemSwaps;

// STATIC: #0x00A31BB8
static const int32_t s_trivialLevelDifference[20] = {
    4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

// STATIC: #0x00C9ECC0
static uint32_t s_combatModeExitCount;

// STATIC: 0x00DD5040
static int32_t s_dualWieldSpell;

// STATIC: 0x00BAF60C
static const RestStateInfo s_restStateInfo[3] = {
    { ERR_MAX_VALUE, 65 },
    { ERR_EXHAUSTION_RESTED, 25 },
    { ERR_EXHAUSTION_NORMAL, 65 },
};

// STATIC: 0x00DD51F8
static int32_t s_zoneUpdateCounter;

// STATIC: 0x00DD51FC
static bool s_dailyQuestsUntouched;

struct DamageCalcLabel {
    /* 0000 */ int32_t m_type;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_id;
    /* 000C */ int32_t m_flags;
};

// STATIC: 0x00CA5028
static const DamageCalcLabel s_damageCalcLabels[43] = { { 0, "Base", 1, 1 }, { 3, "DPS Scaler", 2, 1 }, { 2, "Chain Reduction", 3, 0 }, { 2, "Combo Point Damage", 4, 0 }, { 3, "School Damage Done", 5, 0 }, { 3, "Creature Damage Done", 6, 0 }, { 3, "Creature Weapon Damage Done", 7, 0 }, { 2, "Weapon Effect", 8, 1 }, { 2, "Weapon Effect", 9, 0 }, { 3, "Spell Modifier", 10, 1 }, { 3, "Spell Modifier", 11, 1 }, { 3, "Offhand Penalty", 12, 0 }, { 3, "Attack Power Damage", 13, 1 }, { 3, "Creature Multiplier", 14, 1 }, { 3, "School Damage Done", 15, 1 }, { 3, "Weapon Multiplier", 16, 1 }, { 3, "School Damage Taken", 17, 1 }, { 3, "Source Damage Taken", 18, 1 }, { 3, "School Damage Taken", 19, 1 }, { 3, "Source Damage Taken", 20, 1 }, { 3, "Crit Damage Multiplier", 21, 0 }, { 4, "Crit Damage", 22, 1 }, { 2, "Resistance Coefficient", 23, 1 }, { 2, "Bonus Coefficient", 24, 2 }, { 3, "Critical Chance", 25, 2 }, { 3, "Critical Roll", 26, 2 }, { 3, "Seed For Rounding", 27, 0 }, { 4, "Fraction Adjust", 28, 1 }, { 3, "Resilience Modifier", 29, 1 }, { 3, "Armor Factor", 30, 0 }, { 4, "Armor Reduction", 31, 1 }, { 2, "Shield Block", 32, 1 }, { 2, "Melee Crit/Glance/Crush", 33, 1 }, { 3, "(DEPRECATED) Pet Happiness Mult", 34, 1 }, { 3, "Creature Damage Done", 35, 0 }, { 2, "Number of Targets for Area Damage Cap", 36, 0 }, { 2, "Number of Targets", 37, 1 }, { 2, "Crowd Damage", 38, 1 }, { 3, "Mechanic Modifier Taken", 39, 0 }, { 3, "Bonus From Attack Power Coefficient", 40, 1 }, { 3, "Aura State Modifier", 41, 1 }, { 3, "Melee Autoattack Multiplier", 42, 1 }, { 3, "Mechanic Modifier Done", 250, 834 } };

// STATIC: #0x00ADA1AC
static const UIErrors s_mountResultErrors[] = { ERR_MOUNT_INVALIDMOUNTEE, ERR_MOUNT_TOOFARAWAY, ERR_MOUNT_ALREADYMOUNTED, ERR_MOUNT_NOTMOUNTABLE, ERR_MOUNT_NOTYOURPET, ERR_MOUNT_OTHER, ERR_MOUNT_LOOTING, ERR_MOUNT_RACECANTMOUNT, ERR_MOUNT_SHAPESHIFTED, ERR_MOUNT_FORCEDDISMOUNT, ERR_MAX_VALUE };

// STATIC: #0x00ADA1D8
static const UIErrors s_dismountResultErrors[] = { ERR_DISMOUNT_NOPET, ERR_DISMOUNT_NOTMOUNTED, ERR_DISMOUNT_NOTYOURPET, ERR_MAX_VALUE };

// STATIC: #0x00A32618
static const UIErrors s_buyBankSlotErrors[] = { ERR_BANKSLOT_FAILED_TOO_MANY, ERR_BANKSLOT_INSUFFICIENT_FUNDS, ERR_BANKSLOT_NOTBANKER };

// STATIC: 0x00CA56D0
static uint32_t s_textEmoteStandStates = 0x10B;

// STATIC: 0x00CA52D0
static uint32_t s_textEmoteCooldown = 250;

// STATIC: #0x00ADBA74
static const uint32_t s_playerSoundChance[14] = { 35, 100, 30, 100, 100, 40, 100, 100, 100, 100, 100, 100, 100, 0 };

// STATIC: #0x00C9EB0C
static uint32_t s_talentWipeCost;

// PROVEN: 9c2024c4 IDENTICAL/NONE
// OFFSET: 0x005537E0/0x1401AE720
static int32_t OnUpdatePVPTitle(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameUI::UnitNameUpdate(guid);

    return 1;
}

// OFFSET: 0x00556D80
static int32_t PetChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (guid == ClntObjMgrGetActivePlayer() || CGGameUI::IsPartyMember(guid)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        for (int32_t i = 0; i < 2; i++) {
            WOWGUID oldGuid = static_cast<const WOWGUID*>(oldValue)[i];
            WOWGUID newGuid = i ? player->m_unit->m_unitFieldSummon : player->m_unit->m_unitFieldCharm;

            if (oldGuid) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(oldGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (unit) {
                    unit->UpdateScriptRegistration();
                } else if (oldGuid != newGuid) {
                    CGGameUI::ClearTarget(oldGuid, 1);

                    if (CGGameUI::m_focusTarget == oldGuid) {
                        WOWGUID focus = WOWGUID();
                        CGGameUI::SetFocusTarget(focus);
                    }
                }
            }

            if (newGuid) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(newGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (unit) {
                    unit->UpdateScriptRegistration();
                }
            }
        }

        if (guid == ClntObjMgrGetActivePlayer()) {
            CGPetInfo::FindPossessedSpell();
        }
    }

    return 1;
}

// OFFSET: 0x00556ED0
static int32_t GuildRankUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        CGGuildInfo::QueryPermissions();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_GUILD_UPDATE, NULL);

    return 1;
}

// PROVEN: cb1c0921 IDENTICAL/NONE
// OFFSET: 0x00556F10/0x1401B3CF0
static int32_t DuelTeamUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateReaction(true);
    }

    return 1;
}

// OFFSET: 0x005589C0
static int32_t GetWeaponSubClassCount(int32_t* count) {
    if (!count) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    int32_t maxSubClass = 0;
    int32_t found = 0;
    int32_t index = g_itemSubClassDB.m_numRecords;

    while (index--) {
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(index);

        if (rec->m_classID == 2) {
            found = 1;

            if (maxSubClass <= rec->m_subClassID) {
                maxSubClass = rec->m_subClassID;
            }
        }
    }

    *count = maxSubClass + 1;

    return found;
}

// OFFSET: 0x0055F9E0/0x1401BD430
static int32_t OnUpdateInebriation(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        float drunkness = player->GetDrunknessTurnValue();

        if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (target && player->CanAttack(target)) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_LEVEL, "%s", "target");
            }

            CGUnit_C::NamePlateUpdateLevels(true);
            CGWorldFrame::GetActiveCamera()->UpdateInebriation(drunkness, 0);
        }
    }

    return 1;
}

// OFFSET: 0x0055FB30
static int32_t OnUpdatePlayerHairColor(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (player->m_characterComponent) {
            player->m_characterComponent->SetHairColor(player->m_player->m_playerBytes[3], true, NULL);
        }

        if (ClntObjMgrGetActivePlayer() == guid) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// OFFSET: 0x0055FBB0
static int32_t OnUpdatePlayerSkinID(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (player->m_characterComponent) {
            player->m_characterComponent->SetSkinColor(player->m_player->m_playerBytes[0], true, false, NULL);
        }

        if (ClntObjMgrGetActivePlayer() == guid) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// OFFSET: 0x005657D0
static int32_t OnUpdatePlayerHairStyle(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        bool activePlayer = ClntObjMgrGetActivePlayer() == guid;
        CCharacterComponent* component = player->m_characterComponent;

        if (component) {
            component->SetHairStyle(player->m_player->m_playerBytes[2], NULL);

            if (!(player->m_player->m_playerFlags & 0x400) && (!activePlayer || !CGBarberShop::m_barberShopEnabled)) {
                VisibleItem_C* head = &player->m_player->m_playerVisibleItems[0];

                if (head && head->GetDisplayID() && player->CanEquipItemsInThisForm()) {
                    component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(0), head->GetDisplayID(), 0);
                }
            }
        }

        if (activePlayer) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// OFFSET: 0x005658C0
static int32_t OnUpdatePlayerFacialStyle(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        bool activePlayer = ClntObjMgrGetActivePlayer() == guid;
        CCharacterComponent* component = player->m_characterComponent;

        if (component) {
            component->SetBeardStyle(player->m_player->m_playerBytes2[0], true, NULL);

            if (!(player->m_player->m_playerFlags & 0x400) && (!activePlayer || !CGBarberShop::m_barberShopEnabled)) {
                VisibleItem_C* head = &player->m_player->m_playerVisibleItems[0];

                if (head && head->GetDisplayID() && player->CanEquipItemsInThisForm()) {
                    component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(0), head->GetDisplayID(), 0);
                }
            }
        }

        if (activePlayer) {
            CGBarberShop::AppearanceApplied();
            return 1;
        }

        CGGameUI::UnitModelUpdate(guid, 3);
    }

    return 1;
}

// PROVEN: 85b4f868 IDENTICAL/NONE
// OFFSET: 0x005676C0/0x1401C6040
static int32_t OnUpdatePlayerFlags(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TogglePlayerFlag(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x0056A590
static int32_t OnUpdateInventoryComponent(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateInventoryComponent(static_cast<INVENTORY_SLOTS>((fieldOffset - offsetof(CGPlayerData, m_playerVisibleItems)) / sizeof(VisibleItem_C)), static_cast<const VisibleItem_C*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x00571580/0x1401D28F0
void PlayerClientInitialize() {
    CGPlayer_C::PlayerInitializeMessageHandlers();

    for (uint32_t i = 0; i < 17; i++) {
        s_proficiency[i] = 0;
    }

    CGPlayer_C::s_resurrectOfferer = WOWGUID::Make(0);
    CGPlayer_C::s_resurrectOfferSickness = 0;
    CGPlayer_C::s_resurrectOfferUseTimer = 0;
    s_playerRender = 1;
    PlayerInitializeSounds();
    AreaTriggersInitialize(ClntObjMgrGetMapID());
}

// PROVEN: f3fc01f5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void PlayerInitializeSounds() {
}

// OFFSET: 0x0055B720
void CGPlayer_C::UpdateQuestStatus(CGObject_C* object) {
    if (!object) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID guid = object->m_obj->m_guid;
    CGPlayer_C::QueryQuestGiverStatus(guid);
}

// OFFSET: 0x0055B7C0/0x1401B8750
void CGPlayer_C::UpdateTaxiStatus(CGUnit_C* unit) {
    if (unit->UnitReaction(this) <= 1 || !(unit->m_unit->m_unitNpcFlags & 0x2000)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TAXI_NODE_STATUS_QUERY));
    msg.Put(unit->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00558210
void CGPlayer_C::TurnOnGuildTabardPurchase() {
    if (this->m_characterComponent) {
        this->m_characterComponent->m_flags |= 0x44;
        this->m_characterComponent->RenderPrep(1);
    }

    WOWGUID guid = this->m_obj->m_guid;
    CGGameUI::UnitModelUpdate(guid, 3);
}

// OFFSET: 0x00558480/0x1401B5650
void CGPlayer_C::HandleRepopRequest(bool a2) {
    if (!this->IsDeadOrGhost() || !this->CheckSelfResurrect()) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REPOP_REQUEST));
    msg.Put(static_cast<uint8_t>(a2 != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00568640
void CGPlayer_C::SetClientStandState(uint32_t standState) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (this->m_clientStandState == standState) {
        return;
    }

    this->m_clientStandState = standState;

    if (!standState) {
        CGInputControl::GetActive()->UpdatePlayer(OsGetAsyncTimeMs(), 1);
    } else {
        if (this->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }

        if (this->CGUnit_C::IsInMelee()) {
            this->SetCombatMode(true);
        }

        if (standState >= 4 && standState <= 6) {
            return;
        }
    }

    if (CGBarberShop::m_barberShopEnabled) {
        CGBarberShop::DisableBarberShop();
    }
}

// OFFSET: 0x0055C2C0/0x1401B9030
void CGPlayer_C::PreAnimate(CGWorldFrame* frame) {
    if (!frame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    this->CGUnit_C::PreAnimate(frame);

    uint32_t time = FrameTime::s_curTimeMs;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && this->m_combat.m_attacking.m_guid && static_cast<int32_t>(time - s_attackSwingErrorTime) >= 0) {
        if (s_attackSwingError == 1) {
            s_attackSwingErrorTime = time + 4000;
            CGGameUI::DisplayError(ERR_BADATTACKPOS);
        } else if (s_attackSwingError == 2) {
            s_attackSwingErrorTime = time + 4000;
            CGGameUI::DisplayError(ERR_BADATTACKFACING);
        }
    }
}

// OFFSET: 0x0055C050
uint32_t GetDisplayIdFromRaceAndGender(uint32_t race, uint32_t gender) {
    if (gender >= 3) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);

    if (!raceRec) {
        return 0;
    }

    switch (gender) {
        case 0: {
            return raceRec->m_maleDisplayID;
        }

        case 1: {
            return raceRec->m_femaleDisplayID;
        }

        default: {
            return 0;
        }
    }
}

// OFFSET: 0x00561AB0/0x1401BF9F0
CreatureModelDataRec* GetModelDataRec(uint32_t race, uint32_t gender) {
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);

    if (!raceRec) {
        return NULL;
    }

    int32_t displayID;

    switch (gender) {
        case 0:
            displayID = raceRec->m_maleDisplayID;
            break;

        case 1:
            displayID = raceRec->m_femaleDisplayID;
            break;

        case 2:
            DebugLog("Error, attempted to look up model for player with sex %d (UNITSEX_NONE), all players have sex! =D", 2);
            return NULL;

        default:
            DebugLog("Error, unrecognized sex code %d!", gender);
            return NULL;
    }

    if (!displayID) {
        return NULL;
    }

    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(displayID);

    if (!displayRec) {
        DebugLog("Error, unknown displayInfo %d specified for player race %d sex %d!", displayID, race, gender);
    }

    CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

    if (!modelRec) {
        DebugLog("Error, unknown model record %d specified for player race %d sex %d!", displayRec->m_modelID, race, gender);
    }

    return modelRec;
}

// OFFSET: 0x0055E0E0/0x1401B12A0
bool CGPlayer_C::VehicleAllowsInteraction() const {
    if (!this->HasVehicleTransport()) {
        return true;
    }

    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_movementData.m_transportGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!transport) {
        return true;
    }

    if (!transport->m_vehicle) {
        return false;
    }

    VehicleSeatRec* seat = transport->m_vehicle->GetVehicleSeatRec(static_cast<uint8_t>(this->m_movementData.m_transportSeat));
    return seat && (seat->m_flags & 0x80000000);
}

// OFFSET: 0x00553A90
void CGPlayer_C::SetAutoLoot(int32_t autoLoot) {
    this->m_autoLoot = autoLoot != 0;

    if (autoLoot && this->m_lootTarget.m_guid) {
        CGLootInfo::DoAutoLoot(0);
    }
}

// OFFSET: 0x005C5EE0/0x14023CF30
bool CGPlayer_C::CanInteract(const CGUnit_C* unit) const {
    bool isPlayer = (this->m_obj->m_type & TYPEMASK_PLAYER) != 0;

    if (isPlayer && (this->m_player->m_playerFlags & 0x10) && (!unit->m_creatureCacheEntry || !(unit->m_creatureCacheEntry->m_typeFlags[0] & 0x2))) {
        return false;
    }

    CGUnitData* data = unit->m_unit;
    uint32_t npcFlags = data->m_unitNpcFlags;

    if ((npcFlags & 0x1) && isPlayer && !(unit->m_obj->m_type & TYPEMASK_PLAYER) && unit->m_creatureCacheEntry && (unit->m_creatureCacheEntry->m_typeFlags[0] & 0x800000) && data->m_unitFieldCreatedby != this->m_obj->m_guid) {
        return false;
    }

    return !(data->m_unitFieldFlags & 0x2000000) && npcFlags && ((unit->UnitReaction(this) >= 3 && this->UnitReaction(unit) >= 3) || (unit->m_unit->m_unitFieldFlags2 & 0x4000));
}

// OFFSET: 0x0055E2D0/0x1401BB850
uint64_t CGPlayer_C::GetFarSightGuid() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerFarsight.m_guid;
}

// PROVEN: d2a98841 IDENTICAL/NONE
// OFFSET: 0x005692D0/0x1401C8490
void CGPlayer_C::SetFarSightFocus(CGObject_C* object) {
    this->ToggleFarSight(object);
}

// OFFSET: 0x0044EE00/0x14005D110
uint64_t CGPlayer_C::GetCoinage() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerFieldCoinage;
}

// PROVEN: 3d6f3098 IDENTICAL/NONE
// OFFSET: 0x008FF7A0/0x140657270
const char* CGPlayer_C::GetHairCustomizationName() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        ChrRacesRec* rec = g_chrRacesDB.GetRecord(player->m_unit->m_unitFieldBytes0[0]);

        if (rec) {
            return rec->m_hairCustomization;
        }
    }

    return "NORMAL";
}

// PROVEN: 8ab8a553 IDENTICAL/NONE
// OFFSET: 0x008FF800/0x140657300
const char* CGPlayer_C::GetFacialHairCustomizationName() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        ChrRacesRec* rec = g_chrRacesDB.GetRecord(player->m_unit->m_unitFieldBytes0[0]);

        if (rec) {
            return rec->m_facialHairCustomization[player->m_player->m_playerBytes3[0]];
        }
    }

    return "NORMAL";
}

// OFFSET: 0x0044EDA0
uint32_t CGPlayer_C::GetCurrentXP() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerXp;
}

// OFFSET: 0x00551880
int32_t OnSummonCancel(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, NULL);

    return 1;
}

// OFFSET: 0x00553890/0x1401AE7F0
int32_t OnUpdatePlayerScriptEvent(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    FrameScript_SignalEvent(reinterpret_cast<uintptr_t>(param), "%s", "player");

    return 1;
}

// PROVEN: dd2e236b IDENTICAL/NONE
// OFFSET: 0x00553AD0
uint32_t CGPlayer_C::GetProficiency(uint8_t itemClass) {
    if (itemClass >= sizeof(s_proficiency) / sizeof(s_proficiency[0])) {
        return 0;
    }

    return s_proficiency[itemClass];
}

// OFFSET: 0x00553AF0
int32_t Packet_SMSG_SET_PROFICIENCY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t itemClass;
    uint32_t mask;
    msg->Get(itemClass);
    msg->Get(mask);
    s_proficiency[itemClass] = mask;
    ConsolePrintf("Proficiency in item class %d set to %08x", itemClass, mask);

    return 1;
}

// PROVEN: 471fba9c IDENTICAL/NONE
// OFFSET: 0x00554450/0x1401B0490
bool CGPlayer_C::CanDualWield() {
    return s_dualWieldSpell != 0;
}

// OFFSET: 0x0055BE10
int32_t CGPlayer_C::CanLoot(const CGObject_C* object) {
    if (!object) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    bool autoInteract = this->CanAutoInteract();
    float rangeSq = 0.0f;
    bool checkRange = !autoInteract;

    if (checkRange) {
        float range;

        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            range = this->GetMeleeRange(static_cast<const CGUnit_C*>(object));
        } else {
            range = 5.0f;
        }

        rangeSq = range * range;
    }

    if (this->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (form && (form->m_flags & 0x100)) {
            return 0;
        }
    }

    C3Vector objectPosition = object->GetPosition();
    C3Vector position = this->GetPosition();

    if (checkRange) {
        float dx = position.x - objectPosition.x;
        float dy = position.y - objectPosition.y;
        float dz = position.z - objectPosition.z;

        if (!(dx * dx + dz * dz + dy * dy <= rangeSq)) {
            return 0;
        }
    }

    if (this->m_lootTarget == object->m_obj->m_guid || this->m_lootTarget) {
        return 0;
    }

    return !this->IsDeadOrGhost() && !(this->m_movement->m_flags & 0x1000) && !(this->m_unit->m_unitFieldFlags & 0x40000) && this->IsLocalClientControlled();
}

// PROVEN: 84de2510 IDENTICAL/NONE
// OFFSET: 0x005545C0
int32_t __stdcall CGPlayer_C::OnLootItemNotify(CDataStore* msg) {
    WOWGUID guid;
    uint8_t slot;
    uint8_t unk;
    uint32_t itemId;
    char name[64];
    msg->Get(guid.m_guid);
    msg->Get(slot);
    msg->Get(unk);
    msg->Get(itemId);
    msg->GetString(name, sizeof(name));

    return 1;
}

// PROVEN: 82a1b839 IDENTICAL/NONE
// OFFSET: 0x005546E0/0x1401B09A0
int32_t Player_C_TogglePlayerRender() {
    s_playerRender = s_playerRender == 0;

    return s_playerRender;
}

// PROVEN: 2c9ecf30 IDENTICAL/NONE
// OFFSET: 0x00554750
void CGPlayer_C::SaveBindPoint(CDataStore* msg) {
    msg->Get(s_bindPosition.x);
    msg->Get(s_bindPosition.y);
    msg->Get(s_bindPosition.z);
    msg->Get(s_bindMapId);
    msg->Get(s_bindAreaId);
    s_bindPointSet = 1;
}

// PROVEN: 31c86582 IDENTICAL/NONE
// OFFSET: 0x005547A0/0x1401B0AE0
uint32_t CGPlayer_C::GetBindAreaID() {
    return s_bindAreaId;
}

// OFFSET: 0x00554820
int32_t CGPlayer_C::IsGiftWrapping() {
    return static_cast<bool>(s_giftWrapItem);
}

// PROVEN: 645a2c7d IDENTICAL/NONE
// OFFSET: 0x00554950
uint32_t CGPlayer_C::GetPendingCinematic() {
    return s_pendingMovie;
}

// PROVEN: 171e8f65 IDENTICAL/NONE
// OFFSET: 0x00554960
uint32_t CGPlayer_C::GetPendingMovie() {
    return s_pendingMovieID;
}

// OFFSET: 0x005549B0
int32_t CGPlayer_C::GetCombatModeFlashDuration() {
    return 500;
}

// PROVEN: 09ec8b50 IDENTICAL/NONE
// OFFSET: 0x00554A80/0x1401B13D0
int32_t CGPlayer_C::GetPlayedTime() const {
    if (this->m_playedTime < 0) {
        return 0;
    }

    return this->m_playedTime + static_cast<int32_t>(time(NULL)) - this->m_playedTimeBase;
}

// PROVEN: f04ddedd IDENTICAL/NONE
// OFFSET: 0x00554AE0/0x1401B1440
CGVoidStorage_C* CGPlayer_C::GetVoidStorage() {
    if (!this->m_voidStorage) {
        void* mem = SMemAlloc(sizeof(CGVoidStorage_C), __FILE__, __LINE__, 0);
        this->m_voidStorage = mem ? new (mem) CGVoidStorage_C() : NULL;
    }

    return this->m_voidStorage;
}

// OFFSET: 0x00554AB0
void CGPlayer_C::SetPlayedTime(int32_t playedTime) {
    this->m_playedTime = playedTime;
    this->m_playedTimeBase = static_cast<uint32_t>(time(NULL));
}

// OFFSET: 0x005570B0
int32_t SignalPetSpellPowerUpdate(WOWGUID guid) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_SPELL_POWER_UPDATE, NULL);
    }

    return 1;
}

// OFFSET: 0x005571B0/0x1401B3FB0
void __stdcall CGPlayer_C::AcceptResurrectRequest(int32_t accept) {
    if (!CGPlayer_C::s_resurrectOfferer) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_RESURRECT_RESPONSE));
    msg.Put(CGPlayer_C::s_resurrectOfferer.m_guid);
    msg.Put(static_cast<uint8_t>(accept));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPlayer_C::s_resurrectOfferer = WOWGUID();
}

// OFFSET: 0x00557440/0x1401B42B0
int32_t CGPlayer_C::CheckSpiritHealerDistance() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_spiritHealer, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    return dy * dy + dx * dx + dz * dz <= range * range;
}

// OFFSET: 0x00557500/0x1401B4380
int32_t CGPlayer_C::CheckTalentMasterDistance() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_talentMaster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    return dy * dy + dx * dx + dz * dz <= range * range;
}

// OFFSET: 0x00557750/0x1401B4610
int32_t CGPlayer_C::CheckBinderDistance() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_binder, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    return dy * dy + dx * dx + dz * dz <= range * range;
}

// OFFSET: 0x00557810
void __stdcall CGPlayer_C::TalkToBanker(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BANKER_ACTIVATE));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557950/0x1401B4850
void __stdcall CGPlayer_C::TalkToPetitionVendor(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PETITION_SHOW_LIST));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557A90/0x1401B49C0
void __stdcall CGPlayer_C::TalkToBattleMaster(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BATTLEMASTER_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557B10/0x1401B4A60
void __stdcall CGPlayer_C::TalkToAuctioneer(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_AUCTION_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557B90/0x1401B4B00
void __stdcall CGPlayer_C::TalkToStableMaster(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_LIST_STABLED_PETS));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557C10/0x1401B4BA0
void __stdcall CGPlayer_C::TalkToSpellClick(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SPELL_CLICK));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557C90
void __stdcall CGPlayer_C::TalkToRideVehicle(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PLAYER_VEHICLE_ENTER));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00558520/0x1401B5740
uint8_t CGPlayer_C::FindSlotIndex(WOWGUID guid) const {
    uint32_t numSlots = this->m_bag.m_numSlots;

    if (!numSlots) {
        return 0xFF;
    }

    for (uint8_t slot = 0; slot < numSlots; slot++) {
        WOWGUID slotGuid = slot < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[slot] : WOWGUID();

        if (slotGuid == guid) {
            return slot;
        }
    }

    return 0xFF;
}

// OFFSET: 0x00558750/0x1401B5A40
void __stdcall CGPlayer_C::AutoStoreLootItem(uint8_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUTOSTORE_LOOT_ITEM));
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005587D0/0x1401B5AE0
void CGPlayer_C::SellItem(WOWGUID vendor, WOWGUID item, uint32_t count) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SELL_ITEM));
    msg.Put(vendor.m_guid);
    msg.Put(item.m_guid);
    msg.Put(count);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055AB70
void __stdcall CGPlayer_C::SilencePlayer(WOWGUID guid, bool silence) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PARTY_SILENCE));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(silence));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055AC00
void __stdcall CGPlayer_C::UnSilencePlayer(WOWGUID guid, bool silence) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PARTY_UNSILENCE));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(silence));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055AC90
void __stdcall CGPlayer_C::SetLootMethod(uint32_t method, WOWGUID master, uint32_t threshold) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_LOOT_METHOD));
    msg.Put(method);
    msg.Put(master.m_guid);
    msg.Put(threshold);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005543B0/0x1401B0330
void __stdcall CGPlayer_C::AcceptGuild() {
    UserClientGuildAccept msg;
    ClientServices::Send(&msg);
}

// OFFSET: 0x005543E0/0x1401B0360
void __stdcall CGPlayer_C::DeclineGuild() {
    UserClientOpcode3231 msg;
    ClientServices::Send(&msg);
}

// OFFSET: 0x00554410/0x1401B0390
void __stdcall CGPlayer_C::LeaveGuild() {
    UserClientGuildLeave msg;
    ClientServices::Send(&msg);
}

// OFFSET: 0x0055AE40/0x1401B7B50
void __stdcall CGPlayer_C::AcceptArenaTeam() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_ACCEPT));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055AEB0/0x1401B7BD0
void __stdcall CGPlayer_C::DeclineArenaTeam() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_DECLINE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055AF20
void CGPlayer_C::RemoveItemPassive(int32_t spellId) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    uint32_t count = this->m_companionSpells.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_companionSpells.m_data[i] != spellId) {
            continue;
        }

        if (i != count - 1) {
            this->m_companionSpells.m_data[i] = this->m_companionSpells.m_data[count - 1];
        }

        this->m_companionSpells.m_count--;
        return;
    }
}

// OFFSET: 0x0055AF90/0x1401B7CE0
bool CGPlayer_C::HasItemPassive(int32_t spellId) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return false;
    }

    for (uint32_t i = 0; i < this->m_companionSpells.m_count; i++) {
        if (this->m_companionSpells.m_data[i] == spellId) {
            return true;
        }
    }

    return false;
}

// PROVEN: a0a2e6a3 IDENTICAL/NONE
// OFFSET: 0x0055B110
bool CGPlayer_C::HasEquipBypassSkill(int32_t skillId) const {
    for (uint32_t i = 0; i < this->m_equipBypassSkills.m_count; i++) {
        if (this->m_equipBypassSkills.m_data[i] == skillId) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0055B220/0x1401B7FE0
void CGPlayer_C::LootMoney() {
    if (!this->m_lootTarget) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LOOT_MONEY));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B2A0/0x1401B8070
void __stdcall CGPlayer_C::QueryQuest(const WOWGUID& questGiver, int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_QUERY_QUEST));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint8_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B330
void __stdcall CGPlayer_C::AcceptQuest(const WOWGUID& questGiver, int32_t questId, int32_t a3) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_ACCEPT_QUEST));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint32_t>(a3));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B3D0/0x1401B8200
void __stdcall CGPlayer_C::CompleteQuest(const WOWGUID& questGiver, int32_t questId, int32_t fromScript) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_COMPLETE_QUEST));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint8_t>(fromScript != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B470/0x1401B82D0
void __stdcall CGPlayer_C::GiveQuestItems(const WOWGUID& questGiver, int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_REQUEST_REWARD));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B500
void __stdcall CGPlayer_C::GetQuestReward(const WOWGUID& questGiver, int32_t questId, int32_t reward) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_CHOOSE_REWARD));
    msg.Put(questGiver.m_guid);
    msg.Put(static_cast<uint32_t>(questId));
    msg.Put(static_cast<uint32_t>(reward));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B5A0/0x1401B8450
void __stdcall CGPlayer_C::PushQuestToParty(int32_t questId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PUSH_QUEST_TO_PARTY));
    msg.Put(static_cast<uint32_t>(questId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B620/0x1401B84F0
void __stdcall CGPlayer_C::QuestLogRemoveQuest(uint8_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_LOG_REMOVE_QUEST));
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B6A0
void __stdcall CGPlayer_C::QueryQuestGiverStatus(const WOWGUID& questGiver) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_STATUS_QUERY));
    msg.Put(questGiver.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055B750/0x1401B86D0
void CGPlayer_C::QueryQuestGiverStatusMultiple() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_STATUS_MULTIPLE_QUERY));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C000/0x1401B0840
const CGUnitVirtualItem* CGPlayer_C::GetDefendingItem() const {
    const CGBag_C* bag = this->GetBag();
    WOWGUID guid = static_cast<uint32_t>(bag->m_numSlots) > 4 ? bag->m_slots[4] : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        return &item->m_virtualItem;
    }

    return this->CGUnit_C::GetDefendingItem();
}

// OFFSET: 0x0055C0A0
void __stdcall CGPlayer_C::ReadItem(uint8_t bag, uint8_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_READ_ITEM));
    msg.Put(bag);
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// PROVEN: bb158e16 IDENTICAL/NONE
// OFFSET: 0x0055C280/0x1401B8FF0
uint32_t CGPlayer_C::GetDefaultLanguage() const {
    ChrRacesRec* rec = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    return rec ? rec->m_baseLanguage : 0;
}

// OFFSET: 0x0055C600/0x1401B9360
void __stdcall CGPlayer_C::EnableTaxiNode(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ENABLE_TAXI_NODE));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C680/0x1401B9400
void __stdcall CGPlayer_C::QueryTaxiNodes(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TAXI_QUERY_AVAILABLE_NODES));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C700
void CGPlayer_C::StartTaxi(WOWGUID guid, uint32_t source, uint32_t destination) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ACTIVATE_TAXI));
    msg.Put(guid.m_guid);
    msg.Put(source);
    msg.Put(destination);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C790
void CGPlayer_C::StartTaxiExpress(WOWGUID guid, const uint32_t* nodes, int32_t count) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ACTIVATE_TAXI_EXPRESS));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint32_t>(count));

    for (int32_t i = 0; i < count; i++) {
        msg.Put(nodes[i]);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C840/0x1401B9650
uint32_t CGPlayer_C::GetClientStandState() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_clientStandState;
    }

    return this->m_unit->m_unitFieldBytes1[0];
}

// OFFSET: 0x00562620
int32_t CGPlayer_C::OnPetitionShowSignatures(CDataStore* msg) {
    WOWGUID petition;
    WOWGUID owner;
    uint32_t id;
    uint8_t count;
    msg->Get(petition.m_guid);
    msg->Get(owner.m_guid);
    msg->Get(id);
    msg->Get(count);
    WOWGUID* guids = static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * count));
    uint32_t* choices = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * count));

    for (uint8_t i = 0; i < count; i++) {
        msg->Get(guids[i].m_guid);
        msg->Get(choices[i]);
    }

    if (!FriendList::s_friendList->IsIgnored(owner, true)) {
        CGPetitionInfo::SetPetition(petition, id, owner);
        CGPetitionInfo::SetSignatures(count, guids, choices);
    }

    return 1;
}

// OFFSET: 0x0055D070
void __stdcall CGPlayer_C::RequestPetitionSignatures(WOWGUID petition) {
    if (!petition) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PETITION_SHOW_SIGNATURES));
    msg.Put(petition.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055AD20
uint64_t CGPlayer_C::GetGuildXP() const {
    if (this->m_obj->m_dataType == 1 && this->m_obj->m_data) {
        return this->m_guildXP;
    }

    return 0;
}

// OFFSET: 0x0055AD50/0x1401B03E0
uint64_t CGPlayer_C::GetGuildRemainingXP() const {
    if (this->m_obj->m_dataType == 1 && this->m_obj->m_data) {
        return this->m_guildRemainingXP;
    }

    return 0;
}

// OFFSET: 0x0055AD80/0x1401B0400
uint64_t CGPlayer_C::GetGuildDailyXP() const {
    if (this->m_obj->m_dataType == 1 && this->m_obj->m_data) {
        return this->m_guildDailyXP;
    }

    return 0;
}

// OFFSET: 0x0055ADB0/0x1401B0420
uint64_t CGPlayer_C::GetGuildMaxDailyXP() const {
    if (this->m_obj->m_dataType == 1 && this->m_obj->m_data) {
        return this->m_guildMaxDailyXP;
    }

    return 0;
}

// OFFSET: 0x0055ADE0/0x1401B0440
uint64_t CGPlayer_C::GetGuildPlayerWeeklyXP() const {
    if (this->m_obj->m_dataType == 1 && this->m_obj->m_data) {
        return this->m_guildPlayerWeeklyXP;
    }

    return 0;
}

// OFFSET: 0x0055AE10/0x1401B0460
uint64_t CGPlayer_C::GetGuildPlayerTotalXP() const {
    if (this->m_obj->m_dataType == 1 && this->m_obj->m_data) {
        return this->m_guildPlayerTotalXP;
    }

    return 0;
}

// OFFSET: 0x0055D2E0/0x1401BA5D0
int32_t CGPlayer_C::GetPVPFactionIndex() const {
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    if (!raceRec) {
        return -1;
    }

    FactionTemplateRec* factionRec = g_factionTemplateDB.GetRecord(raceRec->m_factionID);

    if (!factionRec) {
        return -1;
    }

    if (factionRec->m_factionGroup & 0x4) {
        return 0;
    }

    return (factionRec->m_factionGroup & 0x2) ? 1 : -1;
}

// OFFSET: 0x0055D700
void __stdcall CGPlayer_C::RemoveTempEnchantment(int32_t slot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_TEMP_ENCHANTMENT));
    msg.Put(static_cast<uint32_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055D780/0x1407D1010
void __stdcall CGPlayer_C::SetPartyAssignment(uint8_t assignment, WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_PARTY_ASSIGNMENT));
    msg.Put(assignment);
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055D810
void __stdcall CGPlayer_C::ClearPartyAssignment(uint8_t assignment, WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_PARTY_ASSIGNMENT));
    msg.Put(assignment);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055E2A0
uint32_t CGPlayer_C::GetCreatureTracking() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerTrackCreatures;
}

// OFFSET: 0x0055FFC0/0x1401BDAE0
bool CGPlayer_C::IsDeadOrGhost() const {
    return this->m_unit->m_unitFieldHealth <= 0 || ((this->m_player->m_playerFlags >> 4) & 1);
}

// OFFSET: 0x00561BB0/0x1401BFB20
void CGPlayer_C::GetAFKText(char* buffer, uint32_t size) const {
    if ((this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && CGChat::m_afk) || (this->m_player->m_playerFlags & 0x2)) {
        const char* text = FrameScript_GetText("CHAT_FLAG_AFK", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// OFFSET: 0x00561C40/0x1401BFBF0
void CGPlayer_C::GetDNDText(char* buffer, uint32_t size) const {
    if (this->m_player->m_playerFlags & 0x4) {
        const char* text = FrameScript_GetText("CHAT_FLAG_DND", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// OFFSET: 0x00561CB0/0x1401BFC90
void CGPlayer_C::GetGMText(char* buffer, uint32_t size) const {
    uint32_t flags = this->m_player->m_playerFlags;

    if ((flags & 0x8) && !((flags >> 15) & 1)) {
        const char* text = FrameScript_GetText("CHAT_FLAG_GM", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// OFFSET: 0x00561D30/0x1401BFD30
void CGPlayer_C::GetDevText(char* buffer, uint32_t size) const {
    if (this->m_player->m_playerFlags & 0x8000) {
        SStrPack(buffer, "<Dev>", size);
    }
}

// OFFSET: 0x005633C0
bool CGPlayer_C::UnitIsTrivial(const CGUnit_C* unit) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !unit || ((unit->m_obj->m_type >> 4) & 1)) {
        return false;
    }

    int32_t level = player->m_unit->m_unitFieldLevel;
    int32_t difference = level - unit->m_unit->m_unitFieldLevel;

    if (difference <= 0) {
        return false;
    }

    if (static_cast<uint32_t>(level / 5) >= sizeof(s_trivialLevelDifference) / sizeof(s_trivialLevelDifference[0])) {
        return difference > 8;
    }

    return difference > s_trivialLevelDifference[level / 5];
}

// OFFSET: 0x005635E0/0x1401C1360
const VisibleItem_C* CGPlayer_C::GetVisibleItem(int32_t slot) const {
    if (slot < 0 || slot >= 19) {
        return NULL;
    }

    return &this->m_player->m_playerVisibleItems[slot];
}

// OFFSET: 0x00563830
void CGPlayer_C::SetPvpCooldownTimestamp(uint32_t flags) {
    uint32_t playerFlags = this->m_player->m_playerFlags;

    if (playerFlags & 0x200) {
        this->m_pvpCooldownTimestamp = 0;
    } else if (!((playerFlags >> 18) & 1) || !(flags & 0x40000)) {
        this->m_pvpCooldownTimestamp = FrameTime::s_curTimeMs + 300000;
    }
}

// OFFSET: 0x00563880/0x1401C1620
int32_t CGPlayer_C::GetPVPToggleTimer() const {
    if (!(this->m_player->m_playerFlags & 0x40000)) {
        return 300000;
    }

    uint32_t timestamp = this->m_pvpCooldownTimestamp;

    if (timestamp && static_cast<int32_t>(FrameTime::s_curTimeMs - timestamp) < 0) {
        return timestamp - FrameTime::s_curTimeMs;
    }

    return -1;
}

// OFFSET: 0x00563BC0/0x1401C1AE0
int32_t CGPlayer_C::IsCommentatorUberOrInArena() const {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
    uint32_t flags = this->m_player->m_playerFlags;

    return ((flags >> 19) & 1) && (((flags >> 22) & 1) || (map && map->m_instanceType == 4));
}

// OFFSET: 0x0056AB90
void CGPlayer_C::QueueItemEnchantTime(WOWGUID item, int32_t slot, int32_t duration) {
    PlayerPendingItemExpiration* expiration = this->m_pendingItemExpirations.NewNode(2, 0, 0);
    expiration->m_itemGuid = item;
    expiration->m_slot = slot;
    expiration->m_duration = duration;
}

// OFFSET: 0x0055C390
void __stdcall CGPlayer_C::OnTaxiNodeStatus(CDataStore* msg) {
    WOWGUID guid = WOWGUID();
    msg->Get(guid.m_guid);

    uint8_t status = 0;
    msg->Get(status);

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && ((unit->m_unit->m_unitNpcFlags >> 13) & 1)) {
        unit->m_interactIconType = status ? 0 : 7;
        unit->UpdateInteractIcon();
    }
}

// OFFSET: 0x0055E1B0/0x1401B5710
int32_t CGItem::IsUsable() const {
    uint32_t flags = this->m_item->m_itemFieldFlags;

    return (((flags >> 3) & 1) || !this->m_item->m_itemFieldMaxdurability || this->m_item->m_itemFieldDurability) && !((flags >> 4) & 1);
}

// OFFSET: 0x0055FDC0
int32_t OnUpdatePlayerGlyph(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t slot = (offset - offsetof(CGPlayerLocalData, m_playerFieldGlyphs1)) / sizeof(uint32_t);

    if (value && g_glyphPropertiesDB.GetRecord(*static_cast<const int32_t*>(value))) {
        if (g_glyphPropertiesDB.GetRecord(player->m_player->m_playerFieldGlyphs1[slot])) {
            FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_UPDATED, "%d", slot + 1);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_REMOVED, "%d", slot + 1);
        }
    } else if (g_glyphPropertiesDB.GetRecord(player->m_player->m_playerFieldGlyphs1[slot])) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_ADDED, "%d", slot + 1);
    }

    return 1;
}

// OFFSET: [0x0055FEC0]
int32_t OnUpdatePlayerGlyphSlotsEnabled(WOWGUID guid, uint32_t offset, uint32_t count, const void* value) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t enabled = *static_cast<const uint32_t*>(value);

        for (uint32_t i = 0; i < 6; i++) {
            uint32_t bit = 1 << i;

            if (player->m_player->m_playerGlyphsEnabled & bit) {
                if (!(bit & enabled)) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_ENABLED, "%d", i + 1);
                }
            } else if (bit & enabled) {
                FrameScript_SignalEvent(SCRIPT_EVENT_GLYPH_DISABLED, "%d", i + 1);
            }
        }
    }

    return 1;
}

// OFFSET: 0x00565CB0/0x1401C3ED0
uint32_t FindEmptySwapIndex() {
    uint32_t index = 0;

    for (; index < s_itemSwaps.m_count; index++) {
        if (!s_itemSwaps.m_data[index].m_srcContainer) {
            break;
        }
    }

    if (index == s_itemSwaps.m_count) {
        s_itemSwaps.SetCount(s_itemSwaps.m_count + 1);
    }

    return index;
}

// OFFSET: 0x005A5AD0
int32_t CGPlayer_C::GetEquipBypassSkill(uint32_t index) const {
    if (index < this->m_equipBypassSkills.m_count) {
        return this->m_equipBypassSkills.m_data[index];
    }

    return 0;
}

// OFFSET: 0x00554D60/0x1401B14B0
int32_t CGPlayer_C::IsInMelee() const {
    return (this->m_combatModeFlags & 0x2) || this->CGUnit_C::IsInMelee();
}

// OFFSET: 0x0091C910
int32_t CGPlayer_C::HasZoneExplored(int32_t zone) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->CGPlayer::HasZoneExplored(zone);
}

// OFFSET: 0x00558570
void CGPlayer_C::AutoStoreBankItem(WOWGUID bagGuid, uint32_t slot) {
    CGObject_C* bagObject = ClntObjMgrObjectPtr(bagGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!bagObject || !bagObject->GetBag()) {
        return;
    }

    CGBag_C* bag = bagObject->GetBag();

    if (slot >= static_cast<uint32_t>(bag->m_numSlots) || !bag->m_slots[slot]) {
        return;
    }

    uint8_t bagIndex = this->FindSlotIndex(bagGuid);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUTOSTORE_BANK_ITEM));
    msg.Put(bagIndex);
    msg.Put(static_cast<uint8_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00558660
void CGPlayer_C::AutoBankItem(WOWGUID bagGuid, uint32_t slot) {
    CGObject_C* bagObject = ClntObjMgrObjectPtr(bagGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!bagObject || !bagObject->GetBag()) {
        return;
    }

    CGBag_C* bag = bagObject->GetBag();

    if (slot >= static_cast<uint32_t>(bag->m_numSlots) || !bag->m_slots[slot]) {
        return;
    }

    uint8_t bagIndex = this->FindSlotIndex(bagGuid);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUTOBANK_ITEM));
    msg.Put(bagIndex);
    msg.Put(static_cast<uint8_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C120
void CGPlayer_C::ReadItem(WOWGUID guid, uint8_t slot) {
    if (guid == this->m_obj->m_guid) {
        CGPlayer_C::ReadItem(static_cast<uint8_t>(255), slot);
        return;
    }

    for (uint32_t index = 19; index < 23; index++) {
        WOWGUID bagGuid = index < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[index] : WOWGUID();

        if (bagGuid == guid) {
            CGPlayer_C::ReadItem(static_cast<uint8_t>(index), slot);
            return;
        }
    }
}

// OFFSET: 0x0055CD60
int32_t CGPlayer_C::OnPetitionShowList(CDataStore* msg) {
    static GuildPetitionInfo petitionList[10];
    WOWGUID vendor;
    uint8_t count = 0;
    msg->Get(vendor.m_guid);
    msg->Get(count);
    memset(petitionList, 0, sizeof(petitionList));

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(petitionList[i].m_index);
        msg->Get(petitionList[i].m_itemID);
        msg->Get(petitionList[i].m_displayID);
        msg->Get(petitionList[i].m_cost);
        msg->Get(petitionList[i].m_charterType);
        msg->Get(petitionList[i].m_requiredSignatures);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(vendor, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    if ((unit->m_unit->m_unitNpcFlags & 0x40000) && (unit->m_unit->m_unitNpcFlags & 0x80000)) {
        CGGuildRegistrar::SetRegistrar(vendor, petitionList);
    }

    return 1;
}

// OFFSET: 0x0055CE60/0x1401BA130
void __stdcall CGPlayer_C::BuyPetition(const WOWGUID& vendor, CGPetition* petition) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PETITION_BUY));
    msg.Put(vendor.m_guid);
    petition->Pack(&msg);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// PROVEN: 5fb285f0 IDENTICAL/NONE
// OFFSET: 0x0055D3C0/0x1401BA6D0
const char* CGPlayer_C::GetPVPRankString(int32_t rank) const {
    char token[32];
    SStrPrintf(token, sizeof(token), "PVP_RANK_%d_%d", rank, this->GetPVPFactionIndex());

    return this->GetFrameScriptText(token, -1);
}

// OFFSET: 0x00561E00
bool CGPlayer_C::CanTrackUnit(const CGUnit_C* unit) {
    if (this->IsDeadOrGhost()) {
        return false;
    }

    uint8_t flags = unit->m_unit->m_unitFieldBytes1[2];

    if (flags & 0x4) {
        return false;
    }

    if (((unit->m_unit->m_unitDynamicFlags >> 1) & 1) || ((flags & 0x2) && (this->m_player->m_playerFieldBytes[0] & 0x2))) {
        return true;
    }

    int32_t creatureType = unit->GetCreatureType();

    return creatureType && (this->GetCreatureTracking() & (1 << (creatureType - 1)));
}

// OFFSET: 0x0056AC40
void CGPlayer_C::GetPendingEnchantTime(CGItem_C* item) {
    if (!item) {
        return;
    }

    PlayerPendingItemExpiration* expiration = this->m_pendingItemExpirations.Head();

    while (expiration) {
        PlayerPendingItemExpiration* next = expiration->Next();

        if (expiration->m_itemGuid == item->m_obj->m_guid) {
            item->UpdateEnchantmentTime(expiration->m_slot, expiration->m_duration);
            expiration->m_link.Unlink();
            delete expiration;
        }

        expiration = next;
    }
}

// OFFSET: 0x00554700
void SetAttackSwingError(int32_t error) {
    s_attackSwingError = error;
    s_attackSwingErrorTime = FrameTime::s_curTimeMs;
}

// OFFSET: 0x005535E0
static uint32_t __stdcall GetQuestGiverStatusIcon(uint32_t flags) {
    uint32_t icon = 0;

    for (int32_t i = 10; i >= 0; i--) {
        if ((1 << i) & flags) {
            icon = i;
            break;
        }
    }

    if (!CGMinimapFrame::IsTrackingTrivialQuests() || icon != 5 || !(flags & 7)) {
        return icon;
    }

    for (int32_t i = 4; i >= 2; i--) {
        if ((1 << i) & flags) {
            return i;
        }
    }

    return icon;
}

// OFFSET: 0x005566C0
int32_t __stdcall CGPlayer_C::OnQuestGiverStatus(CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    uint32_t status;
    msg->Get(status);

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && object->IsQuestGiver()) {
        object->UpdateInteractIcon(GetQuestGiverStatusIcon(status));
    }

    return 1;
}

// OFFSET: 0x00556740
int32_t __stdcall CGPlayer_C::OnQuestGiverStatusMultiple(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid;
        msg->Get(guid.m_guid);

        uint32_t status;
        msg->Get(status);

        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && object->IsQuestGiver()) {
            object->UpdateInteractIcon(GetQuestGiverStatusIcon(status));
        }
    }

    return 1;
}

// OFFSET: 0x00554540/0x1401B05D0
void __stdcall CGPlayer_C::ClearAFK(int32_t force) {
    if (!CGChat::m_afk || (!force && !g_autoClearAFKCVar->m_intValue)) {
        return;
    }

    CGChat::m_afk = 0;
    CGChat::AddChatMessage(FrameScript_GetText("CLEARED_AFK", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
    UserClientMessageChatAfk msg("");
    ClientServices::Send(&msg);
}

// PROVEN: baaa92e5 IDENTICAL/NONE
// OFFSET: 0x005539C0/0x1401AE960
void CGPlayer_C::GetSpellTextureBlobName(const ChrClassesRec* rec, char* name) {
    strcpy(name, "World\\SpellTextureBlobs\\");
    strcat(name, rec->m_filename);
    strcat(name, "_SpellTextures.tex");
}

// OFFSET: 0x0055AFE0
void CGPlayer_C::RefreshSpellCategoryCooldowns(const SpellRec* spell) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && effect->m_effectAura == 341) {
            Spell_C_RefreshCategoryCooldowns(effect->m_effectMiscValue);
        }
    }
}

// OFFSET: 0x0055D350
int32_t CGPlayer_C::GetPVPFactionIndex(uint8_t raceID) {
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(raceID);

    if (!raceRec) {
        return -1;
    }

    FactionTemplateRec* factionRec = g_factionTemplateDB.GetRecord(raceRec->m_factionID);

    if (!factionRec) {
        return -1;
    }

    if (factionRec->m_factionGroup & 0x4) {
        return 0;
    }

    return 2 * ((factionRec->m_factionGroup & 0x2) != 0) - 1;
}

// OFFSET: 0x00558060/0x1401B5240
bool CGPlayer_C::LoadTabardGuildTextures(uint64_t guildGUID, const VisibleItem_C* item) {
    bool result = true;
    VisibleItem_C* visible = const_cast<VisibleItem_C*>(item);
    bool tabard = visible->GetInventoryType() == 19;
    bool shirt = visible->GetInventoryType() == 4;

    if (!item || (!tabard && !shirt)) {
        return result;
    }

    int32_t displayId = visible->GetDisplayID();

    if (!displayId) {
        return result;
    }

    ItemDisplayInfoRec display;

    if (!this->CanEquipItemsInThisForm() || !this->m_characterComponent || !g_itemDisplayInfoDB.GetRecord(displayId, &display) || (!shirt && (this->m_characterComponent->m_data.m_flags & 0x40))) {
        return result;
    }

    if (tabard) {
        this->m_characterComponent->AddItemBySlot(INVSLOT_TABARD, displayId, 0);
    } else if (shirt) {
        this->m_characterComponent->AddItemBySlot(INVSLOT_BODY, displayId, 0);
    }

    if (!(display.m_flags & 0x19)) {
        return result;
    }

    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (tabard && GuildGetGuildTabard(this->m_obj->m_guid, WOWGUID::Make(guildGUID), CGPlayer_C::TabardDrawCallback, this, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        this->m_characterComponent->ApplyGuildColor(display.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
        return result;
    }

    if (shirt && GuildGetGuildTabard(this->m_obj->m_guid, WOWGUID::Make(guildGUID), CGPlayer_C::ShirtTabardDrawCallback, this, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        this->m_characterComponent->ApplyShirtGuildColor(display.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
        return result;
    }

    return false;
}

// OFFSET: 0x00565F30/0x1401C4580
int32_t CGPlayer_C::GetLanguageSkill(uint32_t language, uint32_t& skill) {
    skill = 0;

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    if (CGSpellBook::m_languageSpells.m_data[language]) {
        SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], CGSpellBook::m_languageSpells.m_data[language]);

        if (!ability) {
            return 0;
        }

        for (int32_t i = 0; i < 128; i++) {
            uint16_t skillLine = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillLineid0[i] : 0;

            if (skillLine != ability->m_skillLine) {
                continue;
            }

            if (!g_skillLineDB.GetRecord(ability->m_skillLine)) {
                return 0;
            }

            skill = this->GetSkillModifier(i) + this->GetSkillValue(i);
            return 1;
        }

        return 0;
    }

    if (!(this->m_unit->m_unitFieldFlags2 & 0x8)) {
        return 0;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        const SpellRec* spell = g_spellDB.GetRecord(this->GetAuraSpellId(i));

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 244 && effect->m_effectMiscValue == static_cast<int32_t>(language)) {
                skill = 300;
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x0056F9E0
void CGPlayer_C::AddKnownSpell(int32_t spellId, int32_t a2, int32_t a3, int32_t a4) {
    if (spellId > g_spellDB.m_maxID) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", spellId);
        return;
    }

    if (a4) {
        if (a3) {
            CGActionBar::ReplaceSpell(0, spellId);
        }

        CGSpellBook::AddKnownSpell(spellId, a3, 0);
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], spellId);
    SkillLineRec* skillLine = ability ? g_skillLineDB.GetRecord(ability->m_skillLine) : NULL;

    if (skillLine && skillLine->m_id == 778) {
        this->m_companionPetSpells.Add(1, &spellId);
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    int32_t effect = 0;

    if (count && effects->m_data[0]) {
        effect = effects->m_data[0]->m_effect;
    }

    if (spell->m_attributes & 0x20) {
        if (skillLine) {
            int32_t lineId = skillLine->m_id;
            TRADESKILLLINE* line = this->m_tradeSkillLines.Ptr(lineId, HASHKEY_NONE());

            if (!line) {
                line = this->m_tradeSkillLines.New(lineId, HASHKEY_NONE(), 0, 0);
            }

            *line->m_spells.New() = spellId;
        }

        CGTradeSkillInfo::RefreshList(1);
    } else if (effect == 101) {
        SpellSetFeedPetSpell(spell->m_id);
    } else if (effect == 155) {
        if (!this->HasEquipBypassSkill(spellId)) {
            this->m_equipBypassSkills.Add(1, &spellId);
        }
    } else if (spell->m_spellEquippedItemsID) {
        SpellEquippedItemsRec* equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID);

        if (equipped) {
            int32_t index;

            for (index = 0; index < static_cast<int32_t>(count); index++) {
                if (effects->m_data[index] && effects->m_data[index]->m_effect == 60) {
                    break;
                }
            }

            if (index < static_cast<int32_t>(count)) {
                for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
                    ItemSubClassRec* subClass = g_itemSubClassDB.GetRecordByIndex(i);

                    if (subClass->m_classID <= 0 || subClass->m_classID != equipped->m_equippedItemClass || (1 << subClass->m_subClassID) != equipped->m_equippedItemSubclass) {
                        continue;
                    }

                    int32_t key = subClass->m_id;

                    if (key) {
                        PROFICIENCYSPELL* proficiency = this->m_proficiencySpells.Ptr(key, HASHKEY_NONE());

                        if (!proficiency) {
                            proficiency = this->m_proficiencySpells.New(key, HASHKEY_NONE(), 0, 0);
                        }

                        proficiency->m_spellId = spellId;
                    }

                    break;
                }
            }
        }
    }

    if (a3) {
        CGClassTrainer::RefreshList();
        CGActionBar::UpdateSpell(spellId);
    }

    if (effect == 40) {
        s_dualWieldSpell = spellId;
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGQuestLog::UpdateQuestSpell(spellId);
    }
}

// OFFSET: 0x00555A90/0x1401B2410
static int32_t GuildChangedUpdateProc(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        unit->UpdateUnitNameText();
    }

    return 1;
}

// OFFSET: 0x00555100
static bool FindArenaOpponent(const WOWGUID& guid, uint32_t* index) {
    for (uint32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponents[i] == guid) {
            *index = i;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005692E0
void CGPlayer_C::AddCategoryCooldown(int32_t category, int32_t cooldown) {
    CATEGORYCOOLDOWN* node = this->m_categoryCooldowns.NewNode(2, 0, 0);
    node->m_category = category;
    node->m_cooldown = cooldown;
}

// OFFSET: 0x00570450/0x1401D14B0
void CGPlayer_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_clientStandState = this->m_unit->m_unitFieldBytes1[0];
    this->CGUnit_C::PostInit(time, create, reenable);
    this->GetUnitName(NULL, 1);
    this->OnGuildChanged(0);
    this->Animate(0.0f);
    CGPartyInfo::EnableMember(this, 1);
    CGRaidInfo::EnableMember(this, 1);

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_player->m_playerBytes3[3] != CGBattlefieldInfo::m_arenaFaction) {
        WOWGUID guid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddArenaOpponent(guid);
        guid = this->m_obj->m_guid;
        uint32_t index;

        if (FindArenaOpponent(guid, &index)) {
            CGBattlefieldInfo::m_arenaOpponentSeen[index] = 1;
        }
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->PostInitActivePlayer();
    } else {
        this->UpdatePartyMemberState();
    }

    this->UpdatePetReaction();
    this->OnMoveUpdate(time, 1, 1);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        PlayerRequestCategoryCooldowns msg;
        ClientServices::Send(&msg);
    }
}

// OFFSET: 0x0056F1B0/0x1401D0250
void CGPlayer_C::Disable() {
    if (this->GetGuildGUID().m_guid) {
        WOWGUID requester;

        if (!g_guildInfoCache.GetRecord2(this->GetGuildGUID().m_guid, requester, NULL, NULL, false)) {
            g_guildInfoCache.CancelCallback(this->GetGuildGUID().m_guid, PlayerNameGuildCallback, this);
            g_guildInfoCache.CancelCallback(this->GetGuildGUID().m_guid, CGPlayer_C::TabardDrawCallback, this);
        }
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->UnsetActiveMirrorHandlers();
        CGGameUI::CancelMirrorTimers();
        CGGameUI::LeaveWorld();

        if (this->m_voidStorage) {
            delete this->m_voidStorage;
        }

        this->m_voidStorage = NULL;
    }

    CGPartyInfo::EnableMember(this, 0);
    CGRaidInfo::EnableMember(this, 0);

    if (this->m_obj->m_guid == CGPetitionInfo::m_petitionGUID) {
        CGPetitionInfo::SetPetition(WOWGUID(), 0, WOWGUID());
    }

    this->CGUnit_C::Disable();

    PlayerDance* dance = this->m_dance;

    if (dance) {
        if (dance->m_moves) {
            SMemFree(dance->m_moves, "delete[]", -1, 0);
        }

        dance->m_moves = NULL;
        SMemFree(dance, "delete", -1, 0);
    }

    this->m_dance = NULL;

    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        s_inspectData->m_guid = WOWGUID();
    }
}

// OFFSET: 0x0056F340/0x1401D0410
void CGPlayer_C::Reenable(const CClientObjCreate& create) {
    this->m_clientStandState = this->m_unit->m_unitFieldBytes1[0];
    this->CGUnit_C::Reenable(create);
    this->m_pendingItemCacheCount = 0;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->SetActiveMirrorHandlers();
        CGGameUI::EnterWorld();
    }
}

// OFFSET: 0x0056C430/0x1401CC420
void CGPlayer_C::PostReenable(const CClientObjCreate& create) {
    this->CGUnit_C::PostReenable(create);
    CGPartyInfo::EnableMember(this, 1);
    CGRaidInfo::EnableMember(this, 1);

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_player->m_playerBytes3[3] != CGBattlefieldInfo::m_arenaFaction) {
        WOWGUID guid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddArenaOpponent(guid);
        guid = this->m_obj->m_guid;
        uint32_t index;

        if (FindArenaOpponent(guid, &index)) {
            CGBattlefieldInfo::m_arenaOpponentSeen[index] = 1;
        }
    }

    this->UpdatePartyMemberState();
    this->UpdatePetReaction();
}

// OFFSET: 0x00560780/0x1401BE380
void CGPlayer_C::HandleGuildIDUpdate() {
    this->OnGuildChanged(1);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGGuildInfo::QueryPermissions();

        if (CGMerchantInfo::m_merchant) {
            FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
        }

        CGGuildBankInfo::ClearContents();
        CGLFGuildInfo::ClearRecruitmentSettings(true);
        ClntObjMgrEnumVisibleObjects(GuildChangedUpdateProc, NULL);
    }
}

// OFFSET: 0x00565C40/0x1401C3E50
bool CGPlayer_C::LoadGuildTextures(uint64_t guildGUID) {
    bool loaded = this->LoadTabardGuildTextures(guildGUID, &this->m_player->m_playerVisibleItems[18]) != 0;

    if (!this->LoadTabardGuildTextures(guildGUID, &this->m_player->m_playerVisibleItems[3])) {
        loaded = false;
    }

    bool result = this->CGUnit_C::LoadGuildTextures(guildGUID);

    if (result) {
        return loaded;
    }

    return result;
}

// OFFSET: 0x00566140/0x1401C47C0
void CGPlayer_C::ShouldRender(uint32_t flags, int32_t& culled, int32_t& hidden) {
    this->CGUnit_C::ShouldRender(flags, culled, hidden);

    if (this->IsCommentatorUberOrInArena() || ((this->m_clientFlags & 0x400000) && this->m_pendingItemCacheCount)) {
        hidden = 1;
    } else if (!s_playerRender && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        hidden = 1;
    }
}

// PROVEN: df936654 IDENTICAL/NONE
// OFFSET: 0x00553900
void CGPlayer_C::UpdateObjectComplete(bool transport) {
    if (s_pendingMovie) {
        CGGameUI::StartCinematic(s_pendingMovie);
        s_pendingMovie = 0;
    } else if (s_pendingMovieID) {
        LoadingScreenDisable();
        CGGameUI::StartMovie(s_pendingMovieID);
        s_pendingMovieID = 0;
    } else if (this->GetTransportGUID()) {
        LoadingScreenSetTransparent(true);
        World::Preload(this->GetPosition(), 1);
        LoadingScreenDisable();
    } else {
        if (transport) {
            World::UpdateAreaOfInterest(this->GetPosition(), 1);
        }

        LoadingScreenDisable();
    }
}

// OFFSET: 0x00567700/0x1401C6080
void CGPlayer_C::ConstructActivePlayer() {
    for (uint32_t i = 0; i < 936; i++) {
        new (&this->m_playerLocalMirrorLists[i]) TSList<void>();
    }

    if (this->m_unit->m_unitFieldHealth <= 0) {
        if ((this->m_movementData.m_flags & 0x400) || (this->m_player->m_playerFlags & 0x4000)) {
            this->HandleRepopRequest(true);
        } else {
            this->RequestRepopOnLanding();
        }
    }

    CGWorldFrame::UpdateScreenEffect();
    Spell_C_SetMissileTrajectoryNextSequenceIndex(static_cast<uint32_t>(this->m_obj->m_guid.m_guid));

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PLAYED_TIME));
    msg.Put(static_cast<uint8_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);

    StaticSingleton<CCameraManager>::Instance()->SetGameCamera(4, this->m_obj->m_guid);
    StaticSingleton<CCameraManager>::Instance()->ResetCameras(this->m_obj->m_guid);
}

// OFFSET: 0x0056DDC0/0x1401CE9E0
CGPlayer_C::CGPlayer_C(uint32_t time, const CClientObjCreate& create)
    : CGUnit_C(time, create),
      unk_209C(-1),
      m_combatModeFlags(0),
      m_pendingItemCacheCount(0),
      m_guildXP(0),
      m_guildRemainingXP(0),
      m_guildDailyXP(0),
      m_guildMaxDailyXP(0),
      m_guildPlayerWeeklyXP(0),
      m_guildPlayerTotalXP(0),
      unk_217C(0),
      m_lootTarget(),
      m_autoLoot(0) {

    CGPlayerLocalData* player = this->m_player;
    bool activePlayer = this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
    this->m_bag.m_guid = this->m_obj->m_guid;
    this->m_bag.m_numSlots = activePlayer ? 86 : 0;
    this->m_bag.m_slots = player->m_playerFieldInvSlotHead;
    this->m_bag.m_hasBank = 1;
    this->m_combatModeTime = FrameTime::GetCurTimeMs() - 500;
    this->m_combatTarget = WOWGUID();
    this->m_pvpCooldownTimestamp = 0;
    this->m_dance = NULL;
    this->m_danceMoveIndex = 0;
    this->m_playedTime = -1;
    this->m_playedTimeBase = 0;
    this->unk_21FC = 0;
    this->m_voidStorage = NULL;

    this->m_gemCounts[0] = 0;
    this->m_gemCounts[1] = 0;
    this->m_gemCounts[2] = 0;
    this->m_gemCounts[3] = 0;
    this->unk_21D4[0] = 0;
    this->unk_21D4[1] = 0;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->ConstructActivePlayer();
    }
}

// OFFSET: 0x0056E180/0x1401CED70
CGPlayer_C::~CGPlayer_C() {
    this->m_tradeSkillLines.InternalClear(0);
    this->m_proficiencySpells.InternalClear(0);
    this->m_pendingItemExpirations.Clear();
    this->m_categoryCooldowns.Clear();

    if (!(this->m_modelFlags & 0x10000)) {
        CGPartyInfo::EnableMember(this, 0);
        CGRaidInfo::EnableMember(this, 0);
    }

    if (this->GetGuildGUID().m_guid) {
        WOWGUID requester;

        if (!g_guildInfoCache.GetRecord2(this->GetGuildGUID().m_guid, requester, NULL, NULL, false)) {
            g_guildInfoCache.CancelCallback(this->GetGuildGUID().m_guid, PlayerNameGuildCallback, this);
            g_guildInfoCache.CancelCallback(this->GetGuildGUID().m_guid, CGPlayer_C::TabardDrawCallback, this);
        }
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->DestructActivePlayer();
    }

    PlayerDance* dance = this->m_dance;

    if (dance) {
        if (dance->m_moves) {
            SMemFree(dance->m_moves, "delete[]", -1, 0);
        }

        dance->m_moves = NULL;
        SMemFree(dance, "delete", -1, 0);
    }

    this->m_dance = NULL;
}

// OFFSET: 0x00557170
void CGPlayer_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGUnit_C::SetStorage(storage, saved);
    this->m_player = reinterpret_cast<CGPlayerLocalData*>(storage + OBJECT_END + sizeof(CGUnitData) / sizeof(uint32_t));
    this->m_playerMirror = saved + CGUnit::TotalFields();
}

// INLINED
static float ClampPitch(float pitch) {
    if (pitch > 1.5707964f) {
        return 1.5707964f;
    }

    if (pitch < -1.5707964f) {
        return -1.5707964f;
    }

    return pitch;
}

// OFFSET: 0x00554A10/0x1401B1320
static float GetDrunkTurnOffset(uint32_t time, float drunkness) {
    if (drunkness == 0.0f) {
        return 0.0f;
    }

    float phase = time * 0.017453292f * drunkness;
    return (cosf(0.154f * phase) + cosf(phase * 0.195f) + cosf(phase * 0.093f)) * 0.333333f * (drunkness * 0.01308997f);
}

// OFFSET: 0x00563C20/0x1401C1B40
int32_t CGPlayer_C::CanGrantLevel(WOWGUID guid) {
    if (!guid) {
        return 8;
    }

    CGPlayer_C* target = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!target) {
        return 4;
    }

    if (!CGPlayer_C::IsRAFPlayer(guid)) {
        return 1;
    }

    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(this->m_unit->m_unitFieldFactiontemplate);
    FactionTemplateRec* targetFaction = g_factionTemplateDB.GetRecord(target->m_unit->m_unitFieldFactiontemplate);

    if (faction && targetFaction && faction->m_factionGroup != targetFaction->m_factionGroup) {
        return 5;
    }

    if (target->m_unit->m_unitFieldLevel >= 60) {
        return 7;
    }

    if (this->m_unit->m_unitFieldLevel <= target->m_unit->m_unitFieldLevel) {
        return 2;
    }

    if (!this->CanAssist(target, false)) {
        return 6;
    }

    return this->m_player->m_playerFieldBytes[1] == 0 ? 3 : 0;
}

// OFFSET: 0x00563D70
void CGPlayer_C::UpdateActiveMoverDisplay(uint32_t time) {
    float offset = 0.0f;
    float drunkness = this->GetDrunknessTurnValue();

    if (drunkness > 0.0f) {
        offset = GetDrunkTurnOffset(time, drunkness);
    }

    if (this->IsAutoTracking()) {
        this->HandleTracking(drunkness, offset);
        return;
    }

    if (drunkness <= 0.0f || !(this->m_movement->m_flags & 0xF)) {
        return;
    }

    if (!CGInputControl::GetActive()->CanTurn(this)) {
        return;
    }

    if (!(this->m_movement->m_flags & 0x30) || CGInputControl::GetActive()->CameraCanTurnPlayer()) {
        this->OnSetRawFacingLocal(time, CMath::normalizeangle0to2pi_(this->GetRawFacing() + offset));
    }

    if ((this->m_movement->m_flags & 0x2200000) || (this->m_movement->m_flags2 & 0x20)) {
        if (CGInputControl::GetActive()->CameraCanTurnPlayer() || !(this->m_movementData.m_flags & 0xC0)) {
            this->OnSetPitchLocal(time, ClampPitch(this->GetPitch() + offset * 4.0f));
        }
    }
}

// OFFSET: 0x0055B8E0/0x1401B04A0
static int32_t TrackingMaskUpdateProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_GAMEOBJECT)) {
        static_cast<CGGameObject_C*>(object)->CheckForPassiveHighlight();
    }

    return 1;
}

// PROVEN: d7964a29 IDENTICAL/NONE
// OFFSET: 0x0055D610/0x1401B1130
static int32_t AuraVisionUpdateHandler(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->AuraVisionUpdated(*static_cast<uint8_t*>(param));
    }

    return 1;
}

// OFFSET: 0x0056FF20
void CGPlayer_C::PostInitActivePlayer() {
    this->SetActiveMirrorHandlers();
    CGGlyphInfo::ListGlyphs();
    CGSpellBook::ClearSpells();

    for (uint32_t i = 0; i < s_initialSpells.Count(); i++) {
        this->AddKnownSpell(s_initialSpells[i].m_spellId, static_cast<int16_t>(s_initialSpells[i].unk_0004), 0, 1);
    }

    this->UpdateAuraEffectMask();

    for (uint32_t i = 0; i < s_initialSpells.Count(); i++) {
        const SpellRec* spell = g_spellDB.GetRecord(s_initialSpells[i].m_spellId);

        if (spell && (spell->m_attributes & 0x40)) {
            this->RefreshSpellCategoryCooldowns(spell);
        }
    }

    for (uint32_t i = 0; i < s_initialCompanionSpells.Count(); i++) {
        int32_t spellId = s_initialCompanionSpells[i];

        if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
            continue;
        }

        bool found = false;

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            for (uint32_t j = 0; j < this->m_companionSpells.Count(); j++) {
                if (this->m_companionSpells[j] == spellId) {
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            *this->m_companionSpells.New() = spellId;
        }
    }

    CGSpellBook::UpdateSpells(1, 0, 1);
    CGSpellBook::UpdateCompanions();
    CGClassTrainer::RefreshList();

    if (!ClntObjMgrGetPlayerType()) {
        char blobName[260];
        CGPlayer_C::GetSpellTextureBlobName(g_chrClassesDB.GetRecord(this->m_unit->m_unitFieldBytes0[1]), blobName);
        TextureLoadBlob(blobName);

        for (uint32_t i = 0; i < 144; i++) {
            if ((s_actionButtons[i] & 0xF0000000) || this->IsSpellKnown(s_actionButtons[i] & 0xF1FFFFFF)) {
                CGActionBar::SetAction(i, s_actionButtons[i], 0, 1);
                PrefetchSpellVisuals(s_actionButtons[i], 1);
            }

            LoadingScreenSpellUpdateProgress(static_cast<float>(i + 1) * 0.0069444445f);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", 0);
    }

    if (this->IsShapeShifted()) {
        CGSpellBook::UpdateUsable();
        CGSpellBook::UpdateSelection(0);
        CGActionBar::UpdateShapeShiftBar();
        CGActionBar::UpdateOverrideSpellBar();
        CGActionBar::UpdateBonusBar();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORM, NULL);
    }

    uint8_t auraVision = 0xFF;
    CGWorldFrame::UpdateScreenEffect();
    ClntObjMgrEnumVisibleObjects(AuraVisionUpdateHandler, &auraVision);
    CGWorldFrame::GetActiveCamera()->SetTelescope(0);

    if (!ClntObjMgrGetPlayerType()) {
        this->SetLocalClientControl(true);

        WOWGUID farsight = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerFarsight : WOWGUID();
        WOWGUID charm = this->m_unit->m_unitFieldCharm;
        CGUnit_C* charmed = NULL;
        WOWGUID controlled;

        if (charm) {
            charmed = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(charm, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (charmed && (charmed->m_unit->m_unitFieldFlags & 0x1000000) && farsight == charm) {
            charmed->SetLocalClientControl(true);
            controlled = charm;
        } else {
            controlled = this->m_obj->m_guid;
        }

        CGGameUI::InitClientControlState(controlled);

        if (controlled != this->m_obj->m_guid) {
            this->ToggleFarSight(charmed);
        }

        CGGameUI::EnterWorld();

        if (this->m_unit->m_unitFieldHealth <= 0) {
            uint8_t flags = this->m_player->m_playerFieldBytes[0];

            if (flags & 0x10) {
                CGGameUI::SetRepopTimeLeft(0);
            } else if (!(flags & 0x8) || (this->m_player->m_playerFlags & 0x4000)) {
                CGGameUI::ClearRepopTime();
            } else {
                CGGameUI::SetRepopTimeLeft(360000);
            }
        }

        CGGameUI::UpdateActivePlayer();
    }

    for (uint32_t i = 0; i < s_pendingRuneConverts.Count(); i++) {
        CGRuneInfo::ConvertRune(s_pendingRuneConverts[i].m_rune, s_pendingRuneConverts[i].m_type);
    }

    s_pendingRuneConverts.Clear();

    ClientServices::Connection()->CharacterSetInGame(1);
    Spell_C_SetPlayerClass(this->m_unit->m_unitFieldBytes0[1]);
    this->CountEquippedGems();

    if (this->m_player->m_playerFlags & 0x200) {
        this->m_pvpCooldownTimestamp = 0;
    } else if (!(this->m_player->m_playerFlags & 0x40000)) {
        this->m_pvpCooldownTimestamp = FrameTime::GetCurTimeMs() + 300000;
    }

    ClntObjMgrEnumVisibleObjects(TrackingMaskUpdateProc, NULL);
    CGCommentator::PostInit(this);
    CGAchievementInfo::PostInitActivePlayer();
}

// PROVEN: 55ad6dcf IDENTICAL/NONE
// OFFSET: 0x005547F0/0x1401B0B50
void CGPlayer_C::CancelGiftWrap() {
    CGGameUI::UnlockItem(s_giftWrapItem);
    s_giftWrapItem = WOWGUID();
    CursorSetResetMode(1);
    CursorSetMode(1);
}

// OFFSET: 0x005625A0/0x1401C02F0
CGUnit_C* CGPlayer_C::GetPossessedUnit() {
    if (!(this->m_combatModeFlags & 0x1)) {
        return NULL;
    }

    WOWGUID farsight;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        farsight = this->m_player->m_playerFarsight;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(farsight, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return NULL;
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (!unit->IsPossessedBy(guid)) {
        return NULL;
    }

    return unit;
}

// OFFSET: 0x00566EF0/0x1401C57A0
void CGPlayer_C::SetCombatMode(bool sendStop) {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && (this->CGUnit_C::IsInMelee() || this->m_combat.m_attackSent)) {
        this->StopAttack(sendStop);
        Spell_C_CancelMeleeSpells();
    }

    if (Spell_C_GetAutoRangedCombatSpell()) {
        Spell_C_SetAutoRangedCombatSpell(0);
        int32_t autoRangedSpell = CGSpellBook::m_autoRangedSpell;

        if (Spell_C_GetAutoRepeatingSpell() == autoRangedSpell) {
            Spell_C_CancelAutoRepeat(true);
        }
    }

    Spell_C_ClearLastAutoRangedCombatError();
    Spell_C_SetAutoRangedCombatSpell(0);
    EventUnregister(EVENT_ID_IDLE, CGPlayer_C::AutoCombatModeEventHandler);
    this->m_combatModeFlags &= ~0xEu;
    CGGameUI::UpdateCombatMode();

    if (static_cast<int32_t>(this->m_unit->m_unitFieldLevel) < 3) {
        return;
    }

    if (CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[0] & 0x20)) {
        return;
    }

    s_combatModeExitCount++;

    if (CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[1] & 0x800000) && s_combatModeExitCount > 4) {
        CGTutorial::TriggerTutorial(5);
    }

    CGTutorial::TriggerTutorial(55);
}

// OFFSET: 0x00551800
int32_t Packet_SMSG_QUEST_CONFIRM_ACCEPT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId == SMSG_QUEST_CONFIRM_ACCEPT) {
        uint32_t questId;
        char title[1024];
        WOWGUID guid;
        msg->Get(questId);
        msg->GetString(title, sizeof(title));
        msg->Get(guid.m_guid);
        CGQuestInfo::ConfirmAcceptQuest(questId, title, &guid);
    }

    return 1;
}

// OFFSET: 0x005518A0
int32_t Packet_SMSG_GROUP_CANCEL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));
    CGGameUI::CancelPartyInvite();
    return 1;
}

// OFFSET: 0x005518D0
int32_t Packet_SMSG_GROUP_DECLINE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));
    CGGameUI::DisplayError(ERR_DECLINE_GROUP_S, name);
    return 1;
}

// OFFSET: 0x005519A0
int32_t Packet_SMSG_GROUP_UNINVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPartyInfo::RemoveAll(1);
    CGGameUI::DisplayError(ERR_UNINVITE_YOU);
    CGLookingForGroup::ClearPartyInfo();
    FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, NULL);
    return 1;
}

// OFFSET: 0x005519D0
int32_t Packet_SMSG_GROUP_DESTROYED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (CGPartyInfo::NumMembers()) {
        CGGameUI::DisplayError(ERR_GROUP_DISBANDED);
    }

    CGPartyInfo::RemoveAll(1);
    CGLookingForGroup::ClearPartyInfo();
    FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, NULL);
    return 1;
}

// OFFSET: 0x00551A10
int32_t Packet_SMSG_GROUPACTION_THROTTLED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_GROUP_ACTION_THROTTLED);
    return 1;
}

// OFFSET: 0x00552BD0
int32_t Packet_SMSG_PLAY_TIME_WARNING(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t flags;
    int32_t seconds;
    msg->Get(reinterpret_cast<uint32_t&>(flags));
    msg->Get(reinterpret_cast<uint32_t&>(seconds));
    char remaining[30];
    SStrPrintf(remaining, sizeof(remaining), "%2d:%02d", seconds / 60 / 60, seconds / 60 % 60);

    if (flags & 0x1000) {
        CGGameUI::DisplayError(ERR_APPROACHING_PARTIAL_PLAY_TIME, remaining);
    } else if (flags & 0x40000000) {
        CGGameUI::DisplayError(ERR_APPROACHING_PARTIAL_PLAY_TIME_2, remaining);
    } else if (flags & 0x2000) {
        CGGameUI::DisplayError(ERR_APPROACHING_NO_PLAY_TIME, remaining);
    } else if (flags & 0x20000000) {
        CGGameUI::DisplayError(ERR_APPROACHING_NO_PLAY_TIME_2, remaining);
    } else if (flags < 0) {
        CGGameUI::DisplayError(ERR_UNHEALTHY_TIME);
    }

    return 1;
}

// OFFSET: 0x00552CE0
int32_t Packet_SMSG_CALENDAR_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t command;
    char eventName[128];
    char name[48];
    uint32_t result;
    msg->Get(command);
    msg->GetString(eventName, sizeof(eventName));
    msg->GetString(name, sizeof(name));
    msg->Get(result);

    if (!result) {
        return 1;
    }

    switch (result) {
        case 1: {
            CGCalendar::CalendarError("CALENDAR_ERROR_GUILD_EVENTS_EXCEEDED", 100);
            break;
        }

        case 2: {
            CGCalendar::CalendarError("CALENDAR_ERROR_EVENTS_EXCEEDED", 30);
            break;
        }

        case 3: {
            CGCalendar::CalendarError("CALENDAR_ERROR_SELF_INVITES_EXCEEDED", 0);
            break;
        }

        case 4: {
            CGCalendar::CalendarError("CALENDAR_ERROR_OTHER_INVITES_EXCEEDED", reinterpret_cast<uintptr_t>(name));
            break;
        }

        case 5: {
            CGCalendar::CalendarError("CALENDAR_ERROR_PERMISSIONS", 0);
            break;
        }

        case 6: {
            CGCalendar::CalendarError("CALENDAR_ERROR_EVENT_INVALID", 0);
            break;
        }

        case 7: {
            CGCalendar::CalendarError("CALENDAR_ERROR_NOT_INVITED", 0);
            break;
        }

        case 8: {
            CGCalendar::CalendarError("CALENDAR_ERROR_INTERNAL", 0);
            break;
        }

        case 9: {
            CGCalendar::CalendarError("ERR_GUILD_PLAYER_NOT_IN_GUILD", 0);
            break;
        }

        case 10: {
            CGCalendar::CalendarError("CALENDAR_ERROR_ALREADY_INVITED_TO_EVENT_S", reinterpret_cast<uintptr_t>(name));
            break;
        }

        case 11: {
            CGCalendar::CalendarError("PLAYER_NOT_FOUND", 0);
            break;
        }

        case 12: {
            CGCalendar::CalendarError("CALENDAR_ERROR_NOT_ALLIED", 0);
            break;
        }

        case 13: {
            CGCalendar::CalendarError("ERR_IGNORING_YOU_S", reinterpret_cast<uintptr_t>(name));
            break;
        }

        case 14: {
            CGCalendar::CalendarError("CALENDAR_ERROR_INVITES_EXCEEDED", 100);
            break;
        }

        case 16: {
            CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_DATE", 0);
            break;
        }

        case 17: {
            CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_TIME", 0);
            break;
        }

        case 19: {
            CGCalendar::CalendarError("CALENDAR_ERROR_NEEDS_TITLE", 0);
            break;
        }

        case 20: {
            CGCalendar::CalendarError("CALENDAR_ERROR_EVENT_PASSED", 0);
            break;
        }

        case 21: {
            CGCalendar::CalendarError("CALENDAR_ERROR_EVENT_LOCKED", 0);
            break;
        }

        case 22: {
            CGCalendar::CalendarError("CALENDAR_ERROR_DELETE_CREATOR_FAILED", 0);
            break;
        }

        case 24: {
            CGCalendar::CalendarError("ERR_SYSTEM_DISABLED", 0);
            break;
        }

        case 25: {
            CGCalendar::CalendarError("ERR_RESTRICTED_ACCOUNT", 0);
            break;
        }

        case 26: {
            CGCalendar::CalendarError("CALENDAR_ERROR_ARENA_EVENTS_EXCEEDED", 0);
            break;
        }

        case 27: {
            CGCalendar::CalendarError("CALENDAR_ERROR_RESTRICTED_LEVEL", 0);
            break;
        }

        case 28: {
            CGCalendar::CalendarError("ERR_USER_SQUELCHED", 0);
            break;
        }

        case 29: {
            CGCalendar::CalendarError("CALENDAR_ERROR_NO_INVITE", 0);
            break;
        }

        case 36: {
            CGCalendar::CalendarError("CALENDAR_ERROR_EVENT_WRONG_SERVER", 0);
            break;
        }

        case 37: {
            CGCalendar::CalendarError("CALENDAR_ERROR_INVITE_WRONG_SERVER", 0);
            break;
        }

        case 38: {
            CGCalendar::CalendarError("CALENDAR_ERROR_NO_GUILD_INVITES", 0);
            break;
        }

        case 39: {
            CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_SIGNUP", 0);
            break;
        }

        case 40: {
            CGCalendar::CalendarError("CALENDAR_ERROR_NO_MODERATOR", 0);
            break;
        }
    }

    return 1;
}

// OFFSET: 0x005530D0
int32_t Packet_SMSG_TALENTS_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t pet = 0;
    msg->Get(pet);

    if (pet) {
        CGTalentInfo::UpdatePetTalentData(msg);
    } else {
        CGTalentInfo::UpdateTalentData(msg);
    }

    return 1;
}

// OFFSET: 0x00553B40
int32_t Packet_SMSG_PLAYER_SKINNED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t freeRepop;
    msg->Get(freeRepop);
    CGGameUI::OpenPlayerSkinned(freeRepop != 0);
    return 1;
}

// PROVEN: 2c7512f4 IDENTICAL/NONE
// OFFSET: 0x00559210
int32_t Packet_SMSG_SPELLINSTAKILLLOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID casterGuid;
    WOWGUID targetGuid;
    msg->Get(casterGuid.m_guid);
    msg->Get(targetGuid.m_guid);
    uint32_t spellId = 0;
    msg->Get(spellId);
    UnitCombatLogInstakill(casterGuid, targetGuid, g_spellDB.GetRecord(spellId));
    return 1;
}

// OFFSET: 0x00559AA0
int32_t Packet_Group_19(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellRemoveLog(msgId, msg, 0);
}

// OFFSET: 0x00559AC0
int32_t Packet_SMSG_SPELLORDAMAGE_IMMUNE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID casterGuid = WOWGUID::Make(value);
    value = 0;
    msg->Get(value);
    WOWGUID targetGuid = WOWGUID::Make(value);
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint8_t debug = 0;
    msg->Get(debug);
    bool isDebug = debug != 0;
    UnitCombatLogSpellMissed(casterGuid, targetGuid, g_spellDB.GetRecord(spellId), 7, debug == 0, isDebug, 0);
    return 1;
}

// OFFSET: 0x005541D0
int32_t Packet_SMSG_RESURRECT_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::UpdateActivePlayer();
    return 1;
}

// OFFSET: 0x00555A10
int32_t Packet_SMSG_EMOTE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t emote;
    WOWGUID guid;
    msg->Get(emote);
    msg->Get(guid.m_guid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_unit->m_unitFieldBytes1[0] != 3 && !(unit->m_movement->m_flags & 0x200000)) {
        unit->PlayEmoteAnimation(emote, 0, 0);
    }

    return 1;
}

// OFFSET: 0x00558C10
int32_t Packet_SMSG_SPELLLOGMISS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellLogMiss(msg, 0);
}

// OFFSET: 0x005590D0
int32_t Packet_SMSG_SPELLLOGEXECUTE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellLogExecute(msg, 0);
}

// OFFSET: 0x005590F0
int32_t Packet_SMSG_SPELLDAMAGESHIELD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID casterGuid = WOWGUID::Make(value);
    value = 0;
    msg->Get(value);
    WOWGUID targetGuid = WOWGUID::Make(value);
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint32_t damage = 0;
    msg->Get(damage);
    uint32_t overkill = 0;
    msg->Get(overkill);
    uint32_t schoolMask = 0;
    msg->Get(schoolMask);
    uint32_t absorbed = 0;
    msg->Get(absorbed);
    UnitCombatLogDamageShield(casterGuid, targetGuid, g_spellDB.GetRecord(spellId), damage, overkill, schoolMask, absorbed);
    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        target->AddWorldDamageText(casterGuid, damage, NULL, false, false, true);
    }

    CGGameUI::ShowCombatFeedback(targetGuid, damage, schoolMask, 0);
    return 1;
}

// OFFSET: 0x00559650
int32_t Packet_SMSG_SPELLNONMELEEDAMAGELOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellNonMeleeDamageLog(msg, 0);
}

// OFFSET: 0x005597F0
int32_t Packet_SMSG_SPELLHEALLOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellHealLog(msg, 0);
}

// OFFSET: 0x00559900
int32_t Packet_SMSG_SPELLENERGIZELOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandleSpellLogEnergizeLog(msg, 0);
}

// OFFSET: 0x0055EBA0
int32_t Packet_Group_17(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t questId;
    msg->Get(questId);
    int32_t expired = CGQuestLog::IsQuestExpiredByID(questId);

    if (!expired) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player) {
            return 1;
        }

        for (int32_t i = 0; i < 25; i++) {
            PlayerQuestLogEntry* entry = i >= 0 && i <= 25 ? &player->m_player->m_playerQuestLog[i] : NULL;

            if (entry && entry->m_questId == questId) {
                if (entry->m_state & 0x2) {
                    expired = 1;
                }

                break;
            }
        }
    }

    WOWGUID none = WOWGUID();
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);

    if (!quest) {
        ConsoleWriteA("Unknown questID (%d) in QuestUpdate", DEFAULT_COLOR, questId);
    }

    if (msgId == SMSG_QUEST_UPDATE_ADD_PVP_CREDIT) {
        uint32_t count;
        msg->Get(count);

        if (expired) {
            return 1;
        }

        if (!quest) {
            return 0;
        }

        CGGameUI::DisplayError(ERR_QUEST_ADD_PLAYER_KILL_SII, count, quest->m_requiredPlayersKilled);
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
        return 1;
    }

    switch (msgId) {
        case SMSG_QUEST_UPDATE_FAILED:
        case SMSG_QUEST_UPDATE_FAILED_TIMER: {
            if (msgId == SMSG_QUEST_UPDATE_FAILED_TIMER) {
                CGQuestLog::SetQuestExpiredByID(questId);
            }

            if (quest && !expired) {
                CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
            }

            return 1;
        }

        case SMSG_QUEST_UPDATE_COMPLETE: {
            if (!quest || expired) {
                return 1;
            }

            if (!quest->m_endText[0]) {
                CGGameUI::DisplayError(ERR_QUEST_UNKNOWN_COMPLETE);
                return 1;
            }

            CGGameUI::DisplayError(ERR_QUEST_OBJECTIVE_COMPLETE_S, quest->m_endText);
            FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
            return 1;
        }

        case SMSG_QUEST_UPDATE_ADD_KILL: {
            int32_t entry;
            uint32_t count;
            uint32_t required;
            WOWGUID guid;
            msg->Get(reinterpret_cast<uint32_t&>(entry));
            msg->Get(count);
            msg->Get(required);
            msg->Get(guid.m_guid);

            if (expired) {
                return 1;
            }

            if (quest) {
                for (uint32_t i = 0; i < 4; i++) {
                    if (static_cast<int32_t>(quest->m_objectiveId[i]) == entry && quest->m_objectiveText[i][0]) {
                        CGGameUI::DisplayError(ERR_QUEST_ADD_FOUND_SII, quest->m_objectiveText[i], count, required);
                        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
                        return 1;
                    }
                }
            }

            if (entry >= 0) {
                WOWGUID requester = WOWGUID();
                CreatureStats_C* creature = g_creatureDBCache.GetRecord(entry, requester, NULL, NULL, false);

                if (creature) {
                    const char* name = creature->GetName(FrameScript_GetPluralIndex(required));
                    CGGameUI::DisplayError(ERR_QUEST_ADD_KILL_SII, name, count, required);
                    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
                }
            } else {
                WOWGUID requester = WOWGUID();
                GameObjectStats_C* gameObject = g_gameObjectDBCache.GetRecord(entry & 0x7FFFFFFF, requester, NULL, NULL, false);

                if (gameObject) {
                    const char* name = gameObject->GetName(FrameScript_GetPluralIndex(required));
                    CGGameUI::DisplayError(ERR_QUEST_ADD_FOUND_SII, name, count, required);
                    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
                }
            }

            return 1;
        }

        default: {
            return 0;
        }
    }
}

// OFFSET: 0x0055EED0
int32_t Packet_Group_13(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer.m_guid || !ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (msgId == SMSG_TRAINER_LIST) {
        return CGPlayer_C::OnTrainerList(msg);
    }

    if (msgId != SMSG_TRAINER_BUY_FAILED) {
        return 0;
    }

    WOWGUID trainer;
    uint32_t service;
    uint32_t reason;
    msg->Get(trainer.m_guid);
    msg->Get(service);
    msg->Get(reason);

    if (reason == 0) {
        ConsoleWriteA("Trainer service %d unavailable", DEFAULT_COLOR, service);
    } else if (reason == 1) {
        ConsoleWriteA("Not enough money for trainer service %d", DEFAULT_COLOR, service);
    } else if (reason == 2) {
        ConsoleWriteA("Not enough skill points for trainer service %d", DEFAULT_COLOR, service);
    }

    return 1;
}

// OFFSET: 0x00564940
int32_t PH_LOOT_OPCODES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    switch (msgId) {
        case SMSG_LOOT_RESPONSE:
            return player->OnLootResponse(time, msg);
        case SMSG_LOOT_RELEASE:
            return player->OnLootReleaseResponse(msg);
        case SMSG_LOOT_REMOVED:
            return player->OnLootRemoved(msg);
        case SMSG_LOOT_MONEY_NOTIFY:
            return CGPlayer_C::OnLootMoneyNotify(msg);
        case SMSG_LOOT_ITEM_NOTIFY:
            return CGPlayer_C::OnLootItemNotify(msg);
        case SMSG_LOOT_CLEAR_MONEY:
            CGPlayer_C::OnLootClearMoney();
            return 1;
        case SMSG_TEST_DROP_RATE_RESULT: {
            char text[1024];
            msg->GetString(text, sizeof(text));
            ConsoleWrite(text, DEFAULT_COLOR);
            return 1;
        }
        case SMSG_LOOT_ALL_PASSED:
            return LootRollAllPassed(msg);
        case SMSG_LOOT_ROLL_WON:
            return LootRollWon(msg);
        case SMSG_LOOT_START_ROLL:
            return LootRollStart(msg);
        case SMSG_LOOT_ROLL:
            return LootRollRolled(msg);
        case SMSG_LOOT_MASTER_LIST:
            return LootRollMasterList(msg);
        case SMSG_LOOT_SLOT_CHANGED:
            return player->OnLootSlotChanged(msg);
        default:
            return 0;
    }
}

// OFFSET: 0x00555940
int32_t Packet_SMSG_SUMMON_REQUEST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID summoner;
    uint32_t area;
    uint32_t timeout;
    msg->Get(summoner.m_guid);
    msg->Get(area);
    msg->Get(timeout);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    if (!player->IsDeadOrGhost()) {
        CGGameUI::SetSummonRequest(summoner, area, timeout);
        return 1;
    }

    WDataStore response;
    response.Put(static_cast<uint32_t>(CMSG_SUMMON_RESPONSE));
    response.Put(summoner.m_guid);
    response.Put(static_cast<uint8_t>(0));
    response.Finalize();
    ClientServices::Send2(&response);
    return 0;
}

// OFFSET: 0x00560CB0
int32_t Packet_Group_14(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::ReadItemResult(msgId, msg);
    }

    return 1;
}

// OFFSET: 0x00564550
int32_t Packet_Group_15(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer.m_guid) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    switch (msgId) {
        case SMSG_PETITION_SHOW_LIST:
            return CGPlayer_C::OnPetitionShowList(msg);
        case SMSG_PETITION_SHOW_SIGNATURES:
            return CGPlayer_C::OnPetitionShowSignatures(msg);
        case SMSG_PETITION_SIGN_RESULTS:
            return player->OnSignedResults(msg);
        case MSG_PETITION_DECLINE:
            return CGPlayer_C::OnPetitionDecline(msg);
        case SMSG_TURN_IN_PETITION_RESULT:
            return CGPlayer_C::OnTurnInPetitionResults(msg);
        case MSG_PETITION_RENAME:
            return CGPlayer_C::OnPetitionRename(msg);
        default:
            return 0;
    }
}

// OFFSET: 0x00564AD0
int32_t Packet_SMSG_INITIAL_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t talentSpec;
    msg->Get(talentSpec);
    uint16_t count;
    msg->Get(count);
    s_initialSpells.SetCount(count);

    for (uint16_t i = 0; i < count; i++) {
        msg->Get(s_initialSpells[i].m_spellId);
        msg->Get(s_initialSpells[i].unk_0004);
    }

    msg->Get(count);

    for (int32_t i = 0; i < count; i++) {
        uint32_t spellId;
        uint16_t itemId;
        uint16_t category;
        uint32_t spellCooldown;
        uint32_t categoryCooldown;
        msg->Get(spellId);
        msg->Get(itemId);
        msg->Get(category);
        msg->Get(spellCooldown);
        msg->Get(categoryCooldown);
        bool onHold = (categoryCooldown >> 31) & 0x1;
        categoryCooldown &= 0x7FFFFFFF;
        Spell_C_SetCooldownLeft(spellId, itemId, category, spellCooldown, categoryCooldown, onHold, 0);
    }

    return 1;
}

// OFFSET: 0x00567BA0
int32_t Packet_SMSG_CANCEL_COMBAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->SetCombatMode(true);
    }

    return 1;
}

// OFFSET: 0x0056B200
int32_t Packet_Group_18(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!msg) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID itemGuid;
    msg->Get(itemGuid.m_guid);
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    switch (msgId) {
        case SMSG_ITEM_TIME_UPDATE: {
            uint32_t seconds;
            msg->Get(seconds);

            if (item) {
                item->UpdateExpirationTime(seconds);
            }

            return 1;
        }
        case SMSG_ITEM_ENCHANT_TIME_UPDATE: {
            uint32_t slot;
            uint32_t seconds;
            WOWGUID playerGuid;
            msg->Get(slot);
            msg->Get(seconds);
            msg->Get(playerGuid.m_guid);

            if (item) {
                item->UpdateEnchantmentTime(slot, seconds);
                return 1;
            }

            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(playerGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player || (player = ClntObjMgrGetActivePlayerObj())) {
                player->QueueItemEnchantTime(itemGuid, slot, seconds);
            }

            return 1;
        }
        case SMSG_SOCKET_GEMS: {
            uint32_t a2 = 0;
            msg->Get(a2);
            uint32_t a3 = 0;
            msg->Get(a3);
            uint32_t a4 = 0;
            msg->Get(a4);
            uint32_t a5 = 0;
            msg->Get(a5);
            item->UpdateSockets(a2, a3, a4, a5);
            return 1;
        }
        default:
            return 0;
    }
}

// OFFSET: 0x00569B30
int32_t Packet_SMSG_REMOVED_SPELL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t spellId;
    msg->Get(spellId);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->DelKnownSpell(spellId, 0);
        return 1;
    }

    for (uint32_t i = 0; i < s_initialSpells.Count(); i++) {
        if (s_initialSpells[i].m_spellId == spellId) {
            s_initialSpells[i].m_spellId = 0;
            s_initialSpells[i].unk_0004 = 0;
        }
    }

    return 1;
}

int32_t Packet_SMSG_DEBUG_SERVER_GEO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return 1;
}

// OFFSET: 0x00554060
static bool OnTimeSyncReq(ClientTimeSyncReq* msg) {
    PlayerTimeSyncResponse response;
    response.m_sequenceIndex = msg->m_sequenceIndex;
    response.m_clientTime = OsGetAsyncTimeMs();
    ClientServices::Send(&response);
    return true;
}

// OFFSET: 0x005540A0
static bool OnTimeSyncReqSuspended(ClientTimeSyncReq* msg) {
    if (msg->m_sequenceIndex) {
        PlayerTimeSyncResponseFailed response(msg->m_sequenceIndex);
        ClientServices::Send(&response);
    } else {
        ClientTimeSyncReq::s_handler = OnTimeSyncReq;
        PlayerTimeSyncResponse response;
        response.m_sequenceIndex = msg->m_sequenceIndex;
        response.m_clientTime = OsGetAsyncTimeMs();
        ClientServices::Send(&response);
    }

    return true;
}

// OFFSET: 0x00554110
static bool OnTimeAdjustment(ClientTimeAdjustment* msg) {
    OsTimeScaleTime(msg->m_timeScale);
    SysMsgPrintf(STATUS_INFO, 7, "Time elapse scaled by %g to %g", static_cast<double>(msg->m_timeScale), OsTimeGetCurrentScale());
    PlayerTimeAdjustmentResponse response;
    response.m_sequenceIndex = msg->m_sequenceIndex;
    response.m_clientTime = OsGetAsyncTimeMs();
    ClientServices::Send(&response);
    return true;
}

// OFFSET: 0x00554180
static bool OnMoveSetActiveMover(ClientMoveSetActiveMover* msg) {
    CGUnit_C::SetActiveMover(*reinterpret_cast<uint64_t*>(msg->m_moverGUID));
    return true;
}

// PROVEN: 29b08fbb IDENTICAL/NONE
// OFFSET: 0x005541A0
void CGPlayer_C::PlayerShutdownClientHandlers() {
    ClientTimeSyncReq::s_handler = NULL;
    ClientTimeAdjustment::s_handler = NULL;
    ClientDisenchantCredit::s_handler = NULL;
    ClientItemAddPassive::s_handler = NULL;
    ClientItemRemovePassive::s_handler = NULL;
    ClientItemSendPassive::s_handler = NULL;
    ClientMoveSetActiveMover::s_handler = NULL;
}

// OFFSET: 0x00555380
static bool OnFeatureSystemStatus(ClientFeatureSystemStatus* msg) {
    CGChat::m_complaintsSystemStatus = msg->m_complaintStatus;
    CGChat::m_voiceChatSystemStatus = msg->m_voiceEnabled;
    CGGameUI::m_hasTravelPass = msg->m_travelPassEnabled;
    CGGameUI::SetCanSendSoRByText(msg->m_scrollOfResurrectionEnabled);
    CGGameUI::SetSoRCounts(msg->m_scrollOfResurrectionMaxRequestsPerDay, msg->m_scrollOfResurrectionRequestsRemaining);
    CGGameUI::s_cfgRealmID = msg->m_cfgRealmID;
    CGGameUI::s_cfgRealmRecID = msg->m_cfgRealmRecID;
    CGGMTicketInfo::SetQuickTicketSystemStatus(msg->m_europaTicketSystemEnabled);

    if (msg->m_europaTicketSystemEnabled) {
        CGGMTicketInfo::SetQuickTicketThrottle(reinterpret_cast<const ThrottleObject*>(&msg->m_europaTicketThrottle));
    }

    CGGMTicketInfo::SetItemRestorationButtonStatus(msg->m_itemRestorationButtonEnabled);

    if (msg->m_sessionAlertEnabled) {
        CGGameUI::SetSessionTimeAlert(true, msg->m_sessionAlertDelay, msg->m_sessionAlertPeriod, msg->m_sessionAlertDisplayTime);
    } else {
        CGGameUI::SetSessionTimeAlert(false, 0, 0, 0);
    }

    return true;
}

// OFFSET: 0x00558A20
void MovementSuspendActiveMover() {
    ClientTimeSyncReq::s_handler = OnTimeSyncReqSuspended;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        mover->SuspendMovement();
    }
}

// OFFSET: 0x00558A60
void MovementResumeActiveMover() {
    ClientTimeSyncReq::s_handler = OnTimeSyncReq;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        mover->ResumeMovement();
    }
}

// OFFSET: 0x00559F00
static bool OnRequestCemeteryListResponse(ClientRequestCemeteryListResponse* msg) {
    uint32_t count = msg->m_cemeteryIDs.Count();
    CGGameUI::SetCemeteryList(count, count ? msg->m_cemeteryIDs.m_data : NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_REQUEST_CEMETERY_LIST_RESPONSE, "%b", msg->m_isGossipTriggered);
    return true;
}

// OFFSET: 0x0055EF90
static bool OnUpdateActionButtons(ClientUpdateActionButtons* msg) {
    uint8_t reason = msg->m_reason;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (reason == 2) {
        if (player) {
            player->unk_21FC = 1;
        }

        return true;
    }

    if (reason > 1) {
        return true;
    }

    for (uint32_t i = 0; i < 144; i++) {
        s_actionButtons[i] = msg->m_actionButtons[i];
    }

    if (reason == 1 && player) {
        player->unk_21FC = 0;

        for (uint32_t i = 0; i < 144; i++) {
            int32_t action = s_actionButtons[i];

            if (((action & 0xF0000000) || player->IsSpellKnown(action & 0xF1FFFFFF)) && ((action & 0xF0000000) != 0x20000000 || CGEquipmentManager::HasSet(static_cast<int32_t>(action & 0xD1FFFFFF)))) {
                CGActionBar::SetAction(i, action, 0, 1);
            } else {
                CGActionBar::RemoveAction(i, 0);
            }
        }

        CGActionBar::UpdateOverrideSpellBar();
        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", 0);
    }

    return true;
}

// OFFSET: 0x00560F50
static bool OnItemRemovePassive(ClientItemRemovePassive* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->RemoveItemPassive(msg->m_spellID);
    }

    return true;
}

// OFFSET: 0x00568FA0
static bool OnItemAddPassive(ClientItemAddPassive* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t spellID = msg->m_spellID;

        if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !player->HasItemPassive(spellID)) {
            player->m_companionSpells.Add(1, &spellID);
        }
    }

    return true;
}

// OFFSET: 0x00569010
static bool OnItemSendPassive(ClientItemSendPassive* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        for (uint32_t i = 0; i < msg->m_spellIDs.Count(); i++) {
            int32_t spellID = msg->m_spellIDs.m_data[i];

            if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !player->HasItemPassive(spellID)) {
                player->m_companionSpells.Add(1, &spellID);
            }
        }

        return true;
    }

    s_initialCompanionSpells.SetCount(msg->m_spellIDs.Count());

    for (uint32_t i = 0; i < msg->m_spellIDs.Count(); i++) {
        s_initialCompanionSpells.m_data[i] = msg->m_spellIDs.m_data[i];
    }

    return true;
}

// OFFSET: 0x0056D0C0
static bool OnDisenchantCredit(ClientDisenchantCredit* msg) {
    WOWGUID looted = WOWGUID::Make(*reinterpret_cast<uint64_t*>(msg->m_lootedGUID));

    if (g_nameDBCache.GetRecord(looted.m_guid, WOWGUID(), NULL, NULL, false)) {
        ItemRecSparse* item = g_itemSparseDB.GetRecord(msg->m_itemID, WOWGUID(), NULL, NULL, false);

        if (item) {
            const char* link = CGTooltip::GetItemHyperlinkString(msg->m_itemID, item->m_quality, 0, msg->m_randomPropertiesID, msg->m_randomPropertiesSeed, 0);
            char text[3000];
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_DISENCHANT_CREDIT", -1, GENDER_NOT_APPLICABLE), link);
            CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
        }
    }

    return true;
}

// OFFSET: 0x0056D170
void CGPlayer_C::PlayerInitializeClientHandlers() {
    ClientTimeSyncReq::s_handler = OnTimeSyncReq;
    ClientTimeAdjustment::s_handler = OnTimeAdjustment;
    ClientDisenchantCredit::s_handler = OnDisenchantCredit;
    ClientItemAddPassive::s_handler = OnItemAddPassive;
    ClientItemRemovePassive::s_handler = OnItemRemovePassive;
    ClientItemSendPassive::s_handler = OnItemSendPassive;
    ClientMoveSetActiveMover::s_handler = OnMoveSetActiveMover;
}

// OFFSET: 0x0056D1C0
static bool OnCategoryCooldown(ClientCategoryCooldown* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->m_categoryCooldowns.Clear();

        for (uint32_t i = 0; i < msg->m_categoryCooldowns.Count(); i++) {
            player->AddCategoryCooldown(msg->m_categoryCooldowns.m_data[i].m_category, msg->m_categoryCooldowns.m_data[i].m_modCooldown);
        }
    }

    return true;
}

// OFFSET: 0x00551860
int32_t Packet_SMSG_QUEST_POI_QUERY_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGQuestLog::OnQuestPOIResponse(msg);
    return 1;
}

// OFFSET: 0x00551A40
int32_t Packet_SMSG_PARTY_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t operation;
    char name[305];
    uint32_t result;
    int32_t value;
    WOWGUID guid;
    msg->Get(operation);
    msg->GetString(name, sizeof(name));
    msg->Get(result);
    msg->Get(value);
    msg->Get(guid.m_guid);

    if (!result) {
        if (!operation) {
            if (name[0]) {
                CGGameUI::DisplayError(ERR_INVITE_PLAYER_S, name);
            }
        } else if (operation == 2) {
            CGPartyInfo::RemoveAll(1);
            CGGameUI::DisplayError(ERR_LEFT_GROUP_YOU);
            CGLookingForGroup::ClearPartyInfo();
            FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_SUMMON, NULL);
        }

        return 1;
    }

    char duration[256];

    switch (result) {
        case 1:
            CGGameUI::DisplayError(ERR_BAD_PLAYER_NAME_S, name);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_TARGET_NOT_IN_INSTANCE_S, name);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_GROUP_FULL);
            break;

        case 5:
            CGGameUI::DisplayError(ERR_ALREADY_IN_GROUP_S, name);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_NOT_LEADER);
            break;

        case 8:
            CGGameUI::DisplayError(ERR_PLAYER_WRONG_FACTION);
            break;

        case 9:
            CGGameUI::DisplayError(ERR_IGNORING_YOU_S, name);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_LFG_PENDING);
            break;

        case 13:
            CGGameUI::DisplayError(ERR_INVITE_RESTRICTED);
            break;

        case 14:
            if (operation == 4) {
                CGGameUI::DisplayError(ERR_GROUP_SWAP_FAILED);
                FrameScript_SignalEvent(SCRIPT_EVENT_RAID_ROSTER_UPDATE, NULL);
            } else {
                CGGameUI::DisplayError(ERR_INVITE_IN_COMBAT);
            }

            break;

        case 15:
            CGGameUI::DisplayError(ERR_INVITE_UNKNOWN_REALM);
            break;

        case 16:
            CGGameUI::DisplayError(ERR_INVITE_NO_PARTY_SERVER);
            break;

        case 17:
            CGGameUI::DisplayError(ERR_INVITE_PARTY_BUSY);
            break;

        case 18:
            CGGameUI::DisplayError(ERR_PARTY_TARGET_AMBIGUOUS);
            break;

        case 19:
            CGGameUI::DisplayError(ERR_PARTY_LFG_INVITE_RAID_LOCKED, name);
            break;

        case 20:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_LIMIT);
            break;

        case 21:
            if (value < 0) {
                value = 1;
            }

            CGTooltip::GetDurationString(duration, sizeof(duration), static_cast<int64_t>(1000 * value), "INT_SPELL_DURATION", false, true, false);
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_COOLDOWN_S, duration);
            break;

        case 22:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_IN_PROGRESS);
            break;

        case 23:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_TOO_FEW_PLAYERS);
            break;

        case 24:
            if (value < 0) {
                value = 1;
            }

            CGTooltip::GetDurationString(duration, sizeof(duration), static_cast<int64_t>(1000 * value), "INT_SPELL_DURATION", false, true, false);
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S, duration);
            break;

        case 25:
            CGGameUI::DisplayError(ERR_RAID_DISALLOWED_BY_LEVEL);
            break;

        case 26:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_IN_COMBAT);
            break;

        case 27: {
            char guidText[32];
            FrameScript_SignalEvent(SCRIPT_EVENT_VOTE_KICK_REASON_NEEDED, "%s%s", name, GUIDToString(guid, guidText));
            break;
        }

        case 28:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_DUNGEON_COMPLETE);
            break;

        case 29:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_LOOT_ROLLS);
            break;

        case 30:
            CGGameUI::DisplayError(ERR_PARTY_LFG_TELEPORT_IN_COMBAT);
            break;

        case 31:
            CGGameUI::DisplayError(ERR_PARTY_ALREADY_IN_BATTLEGROUND_QUEUE);
            break;

        case 32:
            CGGameUI::DisplayError(ERR_PARTY_CONFIRMING_BATTLEGROUND_QUEUE);
            break;

        case 33:
            CGGameUI::DisplayError(ERR_CROSS_REALM_RAID_INVITE);
            break;

        case 34:
            CGGameUI::DisplayError(ERR_RAID_DISALLOWED_BY_CROSS_REALM);
            break;

        case 35:
            CGGameUI::DisplayError(ERR_PARTY_ROLE_NOT_AVAILABLE, name);
            break;

        case 36:
            CGGameUI::DisplayError(ERR_PARTY_LFG_BOOT_VOTE_REGISTERED, name, value);
            break;
    }

    return 1;
}

// OFFSET: 0x00552020
int32_t Packet_SMSG_GUILD_DECLINE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));
    uint8_t autoDecline = 0;
    msg->Get(autoDecline);

    if (autoDecline) {
        CGGameUI::DisplayError(ERR_GUILD_DECLINE_AUTO_S, name);
    } else {
        CGGameUI::DisplayError(ERR_GUILD_DECLINE_S, name);
    }

    return 1;
}

// OFFSET: 0x0055E9F0
int32_t Packet_Group_10(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (!activePlayer || !ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    switch (msgId) {
        case SMSG_BUY_SUCCEEDED:
            return CGPlayer_C::OnBuySucceeded(msg);
        case SMSG_SELL_ITEM:
            return CGPlayer_C::OnSellResponse(msg);
        case SMSG_BUY_FAILED:
            return CGPlayer_C::OnBuyFailed(msg);
        default:
            return 0;
    }
}

// OFFSET: 0x0055EAA0
int32_t Packet_Group_12(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        msg->m_read = msg->m_size;
        return 0;
    }

    switch (msgId) {
        case SMSG_QUEST_GIVER_REQUEST_ITEMS:
            return CGPlayer_C::OnQuestGiverRequestItems(msg);
        case SMSG_QUEST_GIVER_STATUS_MULTIPLE:
            return CGPlayer_C::OnQuestGiverStatusMultiple(msg);
        case SMSG_QUEST_GIVER_INVALID_QUEST:
            return CGPlayer_C::OnQuestGiverInvalidQuest(msg);
        case SMSG_QUEST_GIVER_QUEST_FAILED:
            return CGPlayer_C::OnQuestGiverQuestFailed(msg);
        case SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE:
            return CGPlayer_C::OnQuestGiverChooseReward(msg);
        case SMSG_QUEST_GIVER_QUEST_DETAILS:
            return CGPlayer_C::OnQuestGiverSendQuest(msg);
        case SMSG_QUEST_GIVER_STATUS:
            return CGPlayer_C::OnQuestGiverStatus(msg);
        case SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE:
            return CGPlayer_C::OnQuestGiverListQuests(msg);
        case SMSG_QUEST_LOG_FULL:
            CGGameUI::DisplayError(ERR_QUEST_LOG_FULL);
            return 1;
        default:
            return 0;
    }
}

// OFFSET: 0x0055EE50
int32_t Packet_SMSG_QUEST_FORCE_REMOVE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    uint32_t questId;
    msg->Get(questId);
    QuestCache* quest = g_questDBCache.GetRecord(questId, WOWGUID(), NULL, NULL, false);

    if (quest) {
        CGGameUI::DisplayError(ERR_QUEST_FORCE_REMOVED_S, quest->m_name);
    }

    return 1;
}

// OFFSET: 0x00550EA0/0x1401ABCA0
static void RandomRollNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    uint32_t* roll = static_cast<uint32_t*>(param);

    if (!roll) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (success) {
        WOWGUID requester = WOWGUID();
        NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

        if (name) {
            char text[3000];
            SStrPrintf(text, sizeof(text), FrameScript_GetText("RANDOM_ROLL_RESULT", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), roll[2], roll[0], roll[1]);
            CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
        }
    }

    operator delete(roll);
}

// OFFSET: 0x005546A0
static void __stdcall DisplayMountResult(uint32_t result) {
    if (result < 11) {
        CGGameUI::DisplayError(s_mountResultErrors[result]);
    }
}

// OFFSET: 0x005546C0
static void __stdcall DisplayDismountResult(uint32_t result) {
    if (result < 4) {
        CGGameUI::DisplayError(s_dismountResultErrors[result]);
    }
}

// OFFSET: 0x00550F40
int32_t Packet_SMSG_FORCEACTIONSHOW(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t a1;
    uint32_t a2;
    msg->Get(a1);
    msg->Get(a2);
    return 1;
}

// OFFSET: 0x00550F70
int32_t Packet_SMSG_PET_NAME_INVALID(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t error;
    msg->Get(error);
    CGGameUI::DisplayNameError(error);
    char name[80];
    msg->GetString(name, sizeof(name));
    uint8_t declined;
    msg->Get(declined);

    if (declined) {
        char declensions[5][96];

        for (uint32_t i = 0; i < 5; i++) {
            msg->GetString(declensions[i], sizeof(declensions[i]));
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_PET_FORCE_NAME_DECLENSION, "%s%s%s%s%s%s", name, declensions[0], declensions[1], declensions[2], declensions[3], declensions[4]);
    }

    return 1;
}

// OFFSET: 0x00551020
int32_t Packet_SMSG_BINDZONEREPLY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t a1;
    uint32_t a2;
    msg->Get(a1);
    msg->Get(a2);
    return 1;
}

// OFFSET: 0x00551050
int32_t Packet_SMSG_BUY_BANK_SLOT_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t result;
    msg->Get(result);

    if (result < 3) {
        CGGameUI::DisplayError(s_buyBankSlotErrors[result], result);
    }

    return 1;
}

// OFFSET: 0x00551090
int32_t Packet_MSG_MINIMAP_PING(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    C2Vector position(0.0f, 0.0f);
    msg->Get(guid.m_guid);
    msg->Get(position.x);
    msg->Get(position.y);
    CGMinimapFrame::SetPingPosition(guid, position);
    return 1;
}

// OFFSET: 0x005510F0
int32_t Packet_MSG_RANDOM_ROLL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t* roll = static_cast<uint32_t*>(SMemAlloc(3 * sizeof(uint32_t), __FILE__, __LINE__, 0));
    msg->Get(roll[0]);
    msg->Get(roll[1]);
    msg->Get(roll[2]);
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (FriendList::s_friendList && FriendList::s_friendList->IsIgnored(guid, true)) {
        if (roll) {
            SMemFree(roll, "delete", -1, 0);
        }

        return 1;
    }

    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, RandomRollNameQueryCallback, roll, false);

    if (name) {
        char text[3000];
        SStrPrintf(text, sizeof(text), FrameScript_GetText("RANDOM_ROLL_RESULT", -1, GENDER_NOT_APPLICABLE), name->m_name, roll[2], roll[0], roll[1]);
        CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
        SMemFree(roll, "delete", -1, 0);
    }

    return 1;
}

// OFFSET: 0x005511E0
int32_t Packet_SMSG_CHAT_PLAYER_NOTFOUND(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[305];
    msg->GetString(name, sizeof(name));
    CGGameUI::DisplayError(ERR_CHAT_PLAYER_NOT_FOUND_S, name);
    CGAutoCompleteName::RemoveFlagsByName(name, 32, true, false);
    return 1;
}

// OFFSET: 0x00551230
int32_t Packet_SMSG_CHAT_PLAYER_AMBIGUOUS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[305];
    msg->GetString(name, sizeof(name));
    CGGameUI::DisplayError(ERR_CHAT_PLAYER_AMBIGUOUS_S, name);
    return 1;
}

// OFFSET: 0x00551270
int32_t Packet_SMSG_CHAT_RESTRICTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t type = 0;
    msg->Get(type);

    switch (type) {
        case 0:
            CGGameUI::DisplayError(ERR_CHAT_RESTRICTED);
            break;

        case 1:
            CGGameUI::DisplayError(ERR_CHAT_THROTTLED);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_USER_SQUELCHED);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_YELL_RESTRICTED);
            break;
    }

    return 1;
}

// OFFSET: 0x00551300
int32_t Packet_SMSG_CHAT_NOT_IN_PARTY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t type = 0;
    msg->Get(type);

    switch (type) {
        case 2:
        case 51:
            CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
            break;

        case 3:
        case 39:
        case 40:
            CGGameUI::DisplayError(ERR_NOT_IN_RAID);
            break;
    }

    return 1;
}

// OFFSET: 0x005513A0
int32_t Packet_MSG_QUEST_PUSH_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint8_t result;
    msg->Get(guid.m_guid);
    msg->Get(result);
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);

    if (!name) {
        return 1;
    }

    switch (result) {
        case 0:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_SUCCESS_S, name);
            break;

        case 1:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_INVALID_S, name);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_ACCEPTED_S, name);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_DECLINED_S, name);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_BUSY_S, name);
            break;

        case 5:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_DEAD_S, name);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_LOG_FULL_S, name);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_ONQUEST_S, name);
            break;

        case 8:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_ALREADY_DONE_S, name);
            break;

        case 9:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_NOT_DAILY_S, name);
            break;

        case 10:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_TIMER_EXPIRED_S, name);
            break;

        case 11:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_NOT_IN_PARTY_S, name);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_DIFFERENT_SERVER_DAILY_S, name);
            break;

        case 13:
            CGGameUI::DisplayError(ERR_QUEST_PUSH_NOT_ALLOWED_S, name);
            break;
    }

    return 1;
}

// OFFSET: 0x00551580
int32_t Packet_SMSG_RAID_GROUP_ONLY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t bootTime;
    uint32_t reason = 0;
    msg->Get(bootTime);
    msg->Get(reason);

    if (!bootTime) {
        switch (reason) {
            case 1:
                CGGameUI::DisplayError(ERR_RAID_GROUP_LOWLEVEL, bootTime);
                break;

            case 2:
                CGGameUI::DisplayError(ERR_RAID_GROUP_ONLY, bootTime);
                break;

            case 3:
                CGGameUI::DisplayError(ERR_RAID_GROUP_FULL, bootTime);
                break;

            case 4:
                CGGameUI::DisplayError(ERR_RAID_GROUP_REQUIREMENTS_UNMATCH, bootTime);
                break;
        }
    }

    CGGameUI::SetInstanceBootTime(bootTime);
    return 1;
}

// OFFSET: 0x00551610
int32_t Packet_SMSG_COMPLAINT_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t result;
    uint8_t reason;
    msg->Get(result);
    msg->Get(reason);

    if (result == 1) {
        CGChat::m_complaintsSystemStatus = 0;
    }

    if (reason == 12) {
        CGCalendar::CalendarError("COMPLAINT_ADDED", 0);
    }

    return 1;
}

// OFFSET: 0x00551660
int32_t Packet_SMSG_REPORT_PVP_AFK_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t result;
    uint8_t numBlackMarksOnOffender;
    uint8_t numPlayersIHaveReported;
    WOWGUID offender;
    msg->Get(result);
    msg->Get(numBlackMarksOnOffender);
    msg->Get(numPlayersIHaveReported);
    msg->Get(offender.m_guid);

    if (result == 5) {
        if (!g_enablePVPNotifyAFKCVar->m_intValue) {
            g_enablePVPNotifyAFKCVar->Set(1, true, false, false, true);
            CGChat::AddChatMessage(FrameScript_GetText("PVP_REPORT_AFK_SYSTEM_ENABLED", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
        }
    } else if (result == 6 && g_enablePVPNotifyAFKCVar->m_intValue) {
        g_enablePVPNotifyAFKCVar->Set(0, true, false, false, true);
        CGChat::AddChatMessage(FrameScript_GetText("PVP_REPORT_AFK_SYSTEM_DISABLED", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
    }

    return 1;
}

// OFFSET: 0x00551710
int32_t Packet_SMSG_INSTANCE_LOCK_WARNING_QUERY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t lockTime = 0;
    msg->Get(lockTime);
    uint32_t completedMask = 0;
    msg->Get(completedMask);
    uint8_t extending = 0;
    msg->Get(extending);
    uint8_t warningOnly = 0;
    msg->Get(warningOnly);

    if (warningOnly) {
        CGGameUI::SetInstanceLockWarning(completedMask);
    } else {
        CGGameUI::SetInstanceLockTime(lockTime, completedMask, extending);
    }

    return 1;
}

// OFFSET: 0x00551790
int32_t Packet_SMSG_TALENTS_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t error = 0;
    msg->Get(error);

    switch (error) {
        case 1:
            CGGameUI::DisplayError(ERR_TALENT_FAILED_UNKNOWN, 1);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_TALENT_FAILED_NOT_ENOUGH_TALENTS_IN_PRIMARY_TREE, 2);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_TALENT_FAILED_NO_PRIMARY_TREE_SELECTED, 3);
            break;
    }

    return 1;
}

// OFFSET: 0x00551A30
static bool OnBattlegroundInfoThrottled(ClientBattlegroundInfoThrottled* msg) {
    CGGameUI::DisplayError(ERR_BATTLEGROUND_INFO_THROTTLED);
    return true;
}

// OFFSET: 0x00552080
int32_t Packet_SMSG_GUILD_INVITE_CANCEL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::CancelGuildInvite();
    return 1;
}

// OFFSET: 0x005520C0
static void GuildRosterUpdateCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGGuildInfo::UpdateRoster(1);
    }
}

// OFFSET: 0x005520E0/0x1401ACFA0
static int32_t HandleGuildCommandResult(uint32_t command, const char* name, uint32_t result) {
    if (result) {
        switch (result) {
            case 1:
                CGGameUI::DisplayError(ERR_GUILD_INTERNAL, name);
                return 1;

            case 2:
                CGGameUI::DisplayError(ERR_ALREADY_IN_GUILD, name);
                return 1;

            case 3:
                CGGameUI::DisplayError(ERR_ALREADY_IN_GUILD_S, name);
                return 1;

            case 4:
                CGGameUI::DisplayError(ERR_INVITED_TO_GUILD, name);
                return 1;

            case 5:
                CGGameUI::DisplayError(ERR_ALREADY_INVITED_TO_GUILD_S, name);
                return 1;

            case 6:
                CGGameUI::DisplayNameError(1);
                return 1;

            case 7:
                CGGameUI::DisplayError(ERR_GUILD_NAME_EXISTS_S, name);
                return 1;

            case 8:
                if (command == 3) {
                    CGGameUI::DisplayError(ERR_GUILD_LEADER_LEAVE, name);
                } else {
                    CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS, name);
                }

                return 1;

            case 9:
                CGGameUI::DisplayError(ERR_GUILD_PLAYER_NOT_IN_GUILD, name);
                return 1;

            case 10:
                CGGameUI::DisplayError(ERR_GUILD_PLAYER_NOT_IN_GUILD_S, name);
                return 1;

            case 11:
                CGGameUI::DisplayError(ERR_GUILD_PLAYER_NOT_FOUND_S, name);
                return 1;

            case 12:
                CGGameUI::DisplayError(ERR_GUILD_NOT_ALLIED, name);
                return 1;

            case 13:
                CGGameUI::DisplayError(ERR_GUILD_RANK_TOO_HIGH_S, name);
                return 1;

            case 14:
                CGGameUI::DisplayError(ERR_GUILD_RANK_TOO_LOW_S, name);
                return 1;

            case 17:
                CGGameUI::DisplayError(ERR_GUILD_RANKS_LOCKED, name);
                return 1;

            case 18:
                CGGameUI::DisplayError(ERR_GUILD_RANK_IN_USE, name);
                return 1;

            case 19:
                CGGameUI::DisplayError(ERR_IGNORING_YOU_S, name);
                return 1;

            case 20:
                if (command == 5) {
                    CGGuildInfo::UpdateRoster(1);
                }

                return 1;

            case 25:
                CGGameUI::DisplayError(ERR_GUILD_WITHDRAW_LIMIT, name);
                return 1;

            case 26:
                CGGameUI::DisplayError(ERR_GUILD_NOT_ENOUGH_MONEY, name);
                return 1;

            case 28:
                CGGameUI::DisplayError(ERR_GUILD_BANK_FULL, name);
                return 1;

            case 29:
                CGGameUI::DisplayError(ERR_ITEM_NOT_FOUND, name);
                return 1;

            case 31:
                CGGameUI::DisplayError(ERR_GUILD_TOO_MUCH_MONEY, name);
                return 1;

            case 32:
                CGGameUI::DisplayError(ERR_GUILD_BANK_WRONG_TAB, name);
                return 1;

            case 34:
                CGGameUI::DisplayError(ERR_RANK_REQUIRES_AUTHENTICATOR, name);
                return 1;

            case 35:
                CGGameUI::DisplayError(ERR_GUILD_BANK_VOUCHER_FAILED, name);
                return 1;

            case 36:
                CGGameUI::DisplayError(ERR_GUILD_TRIAL_ACCOUNT, name);
                return 1;

            case 37:
                CGGameUI::DisplayError(ERR_GUILD_UNDELETABLE_DUE_TO_LEVEL, name);
                return 1;

            case 38:
                CGGameUI::AddErrorMessage(FrameScript_GetText("GUILD_MOVE_STARTING", -1, GENDER_NOT_APPLICABLE), 0);
                return 1;

            case 39:
                CGGameUI::AddErrorMessage(FrameScript_GetText("ERR_GUILD_REP_TOO_LOW", -1, GENDER_NOT_APPLICABLE), 1);
                return 1;

            default:
                return 1;
        }
    }

    switch (command) {
        case 0:
            CGGameUI::DisplayError(ERR_GUILD_CREATE_S, name);
            return 1;

        case 1:
            CGGameUI::DisplayError(ERR_GUILD_INVITE_S, name);
            return 1;

        case 14:
            CGGameUI::DisplayError(ERR_GUILD_FOUNDER_S, name);
            return 1;

        case 20:
        case 21:
            CGGuildInfo::UpdateRoster(1);
            return 1;

        default:
            return 1;
    }
}

// OFFSET: 0x00552410
static bool OnGuildCommandResult(ClientGuildCommandResult* msg) {
    return HandleGuildCommandResult(msg->m_command, msg->m_name, msg->m_result) != 0;
}

// OFFSET: 0x00552430
int32_t Packet_SMSG_GUILD_COMMAND_RESULT_2(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t command;
    char name[96];
    uint32_t result;
    msg->Get(command);
    msg->GetString(name, sizeof(name));
    msg->Get(result);
    return static_cast<uint8_t>(HandleGuildCommandResult(command, name, result));
}

// OFFSET: 0x005524F0
static bool OnArenaTeamCommandResult(ClientArenaTeamCommandResult* msg) {
    if (!msg->m_result) {
        switch (msg->m_action) {
            case 0:
                CGGameUI::DisplayError(ERR_ARENA_TEAM_CREATE_S, msg->m_teamName);
                break;

            case 1:
                CGGameUI::DisplayError(ERR_ARENA_TEAM_INVITE_SS, msg->m_name, msg->m_teamName);
                break;

            case 3:
                CGGameUI::DisplayError(ERR_ARENA_TEAM_QUIT_S, msg->m_teamName);
                break;

            case 14:
                CGGameUI::DisplayError(ERR_ARENA_TEAM_FOUNDER_S, msg->m_teamName);
                break;
        }

        return true;
    }

    switch (msg->m_result) {
        case 1:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_INTERNAL);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_ALREADY_IN_ARENA_TEAM);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_ALREADY_IN_ARENA_TEAM_S, msg->m_name);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_INVITED_TO_ARENA_TEAM);
            break;

        case 5:
            CGGameUI::DisplayError(ERR_ALREADY_INVITED_TO_ARENA_TEAM_S, msg->m_name);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NAME_INVALID);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NAME_EXISTS_S, msg->m_teamName);
            break;

        case 8:
            if (msg->m_action == 3) {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_LEADER_LEAVE_S, msg->m_teamName);
            } else {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_PERMISSIONS);
            }

            break;

        case 9:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM);
            break;

        case 10:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS, msg->m_name, msg->m_teamName);
            break;

        case 11:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S, msg->m_name);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NOT_ALLIED);
            break;

        case 19:
            CGGameUI::DisplayError(ERR_IGNORING_YOU_S, msg->m_name);
            break;

        case 21:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TARGET_TOO_LOW_S, msg->m_name);
            break;

        case 22:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TARGET_TOO_HIGH_S, msg->m_name);
            break;

        case 23:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S, msg->m_teamName);
            break;

        case 27:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_NOT_FOUND);
            break;

        case 30:
            CGGameUI::DisplayError(ERR_ARENA_TEAMS_LOCKED);
            break;

        case 33:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_TOO_MANY_CREATE);
            break;
    }

    return true;
}

// OFFSET: 0x00553B70
static void ReadInspectItems(WOWGUID guid, CDataStore* msg, bool guild) {
    if (!s_inspectData) {
        return;
    }

    memset(s_inspectData->m_items, 0, sizeof(s_inspectData->m_items));
    uint32_t mask = 0;
    msg->Get(mask);

    for (uint32_t i = 0; i < 19; i++, mask >>= 1) {
        if (!(mask & 1)) {
            continue;
        }

        InspectItem* item = &s_inspectData->m_items[i];
        int32_t itemId = 0;
        msg->Get(itemId);

        if (itemId < 0) {
            item->m_flags |= 1;
            itemId = -itemId;
        }

        item->m_itemId = itemId;
        uint16_t enchantMask = 0;
        msg->Get(enchantMask);
        int32_t* enchant = item->m_enchants;

        for (uint32_t bits = enchantMask; bits; bits >>= 1, enchant++) {
            if (bits & 1) {
                uint16_t enchantId = 0;
                msg->Get(enchantId);
                *enchant = enchantId;
            }
        }

        uint16_t randomPropertiesId = 0;
        msg->Get(randomPropertiesId);
        item->m_randomPropertiesId = randomPropertiesId;
        SmartGUID creator;
        *msg >> creator;
        item->m_creator = creator;
        uint32_t seed = 0;
        msg->Get(seed);
        item->m_propertySeed = seed;
    }

    if (guild) {
        uint64_t guildGUID = 0;
        msg->Get(guildGUID);
        s_inspectData->m_guildGUID = guildGUID;
        uint32_t level = 0;
        msg->Get(level);
        s_inspectData->m_guildLevel = level;
        uint64_t xp = 0;
        msg->Get(xp);
        s_inspectData->m_guildXP = xp;
        uint32_t members = 0;
        msg->Get(members);
        s_inspectData->m_guildNumMembers = members;
    }

    char text[32];
    FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_READY, "%s", GUIDToString(guid, text));
}

// OFFSET: 0x00553D60
int32_t Packet_SMSG_INSPECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;

    if (guid == CGCharacterInfo::m_inspectTarget) {
        ReadInspectItems(guid, msg, false);
    } else {
        msg->m_read = msg->m_size;
    }

    return 1;
}

// OFFSET: 0x00553DC0
int32_t Packet_SMSG_INSPECT_TALENT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (guid == CGCharacterInfo::m_inspectTarget) {
        CGTalentInfo_Inspect::UpdateTalentData(msg);

        if (!s_inspectData) {
            s_inspectData = static_cast<InspectData*>(SMemAlloc(sizeof(InspectData), __FILE__, __LINE__, 0));
        }

        s_inspectData->m_guid = guid;
        ReadInspectItems(guid, msg, true);
    }

    return 1;
}

// OFFSET: 0x005541E0
static bool OnGuildMembersWithRecipe(ClientGuildMembersWithRecipe* msg) {
    CGGuildInfo::SetRecipeKnownByMembers(msg->m_skillLineID, msg->m_spellID, msg->m_members);
    return true;
}

// OFFSET: 0x00554200
static bool OnRoleChangedInform(ClientRoleChangedInform* msg) {
    NameCache* changedBy = g_nameDBCache.GetRecord(*reinterpret_cast<uint64_t*>(msg->m_changedByGUID), WOWGUID(), NULL, NULL, false);
    NameCache* target = g_nameDBCache.GetRecord(*reinterpret_cast<uint64_t*>(msg->m_targetGUID), WOWGUID(), NULL, NULL, false);

    if (target) {
        const char* from = changedBy ? changedBy->m_name : FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
        FrameScript_SignalEvent(SCRIPT_EVENT_ROLE_CHANGED_INFORM, "%s%s%s%s", target->m_name, from, GetRoleTokenFromFlags(msg->m_oldRole), GetRoleTokenFromFlags(msg->m_newRole));
    }

    return true;
}

// OFFSET: 0x005542B0
static bool OnRolePollBegin(ClientRolePollBegin* msg) {
    NameCache* initiator = g_nameDBCache.GetRecord(*reinterpret_cast<uint64_t*>(msg->m_initiatorGUID), WOWGUID(), NULL, NULL, false);
    const char* name = initiator ? initiator->m_name : FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
    FrameScript_SignalEvent(SCRIPT_EVENT_ROLE_POLL_BEGIN, "%s", name);
    return true;
}

// OFFSET: 0x00554310
static bool OnRaidMarkersChanged(ClientRaidMarkersChanged* msg) {
    CGRaidInfo::m_raidMarkers = msg->m_raidMarkers;
    return true;
}

// OFFSET: 0x00554330
static bool OnPetitionAlreadySigned(ClientPetitionAlreadySigned* msg) {
    NameCache* signer = g_nameDBCache.GetRecord(*reinterpret_cast<uint64_t*>(msg->m_signerGUID), WOWGUID(), NULL, NULL, false);

    if (signer) {
        CGGameUI::DisplayError(ERR_PETITION_ALREADY_SIGNED_BY_S, signer->m_name);
    }

    return true;
}

// OFFSET: 0x00554380
static bool OnChatIgnoredAccountMuted(ClientChatIgnoredAccountMuted* msg) {
    CGGameUI::DisplayError(ERR_PARENTAL_CONTROLS_CHAT_MUTED);
    return true;
}

// OFFSET: 0x00554390
static bool OnSorStartExperienceIncomplete(ClientSorStartExperienceIncomplete* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_SOR_START_EXPERIENCE_INCOMPLETE, NULL);
    CGGameUI::DisplayError(ERR_SOR_START_EXPERIENCE_INCOMPLETE);
    return true;
}

// OFFSET: 0x00555270
int32_t Packet_SMSG_PLAYER_BOUND(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID binder;
    uint32_t areaId;
    msg->Get(binder.m_guid);
    msg->Get(areaId);
    s_lastBinderGuid = binder;
    SE3SoundKitProperties properties;
    properties.ResetToDefaults();
    SE3::PlaySoundKit(1141, NULL, &properties, NULL, __FILE__, __LINE__, "SMSG_PLAYERBOUND", 0, NULL);
    AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (area) {
        CGGameUI::DisplayError(ERR_DEATHBIND_SUCCESS_S, static_cast<const char*>(area->m_areaName));
    }

    return 1;
}

// OFFSET: 0x00559B80
int32_t Packet_SMSG_DAMAGE_CALC_LOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID attacker;
    WOWGUID target;
    uint32_t sourceType;
    int32_t spellId;
    uint32_t count;
    msg->Get(attacker.m_guid);
    msg->Get(target.m_guid);
    msg->Get(sourceType);
    msg->Get(spellId);
    msg->Get(count);
    CGUnit_C* attackerUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(attacker, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGUnit_C* targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));
    const SpellRec* spell = g_spellDB.GetRecord(spellId);
    const char* spellName = spell ? spell->m_name : "None";
    const char* targetName = targetUnit ? targetUnit->GetUnitName(NULL, 1) : "Unknown Unit";
    const char* attackerName = attackerUnit ? attackerUnit->GetUnitName(NULL, 1) : "Non-unit";
    ConsolePrintf("DMG: '%s' attacks '%s' sourceType=%d, spell='%s'", attackerName, targetName, sourceType, spellName);

    for (uint32_t i = 0; i < count; i++) {
        char label[30];
        uint32_t type;
        float oldValue;
        float newValue;
        msg->GetString(label, sizeof(label));
        msg->Get(type);
        msg->Get(oldValue);
        msg->Get(newValue);

        if (!_stricmp(label, "val")) {
            ConsolePrintf("DMG: NOT VALIDATED: Old=%f, New=%f", oldValue, newValue);
        } else if (type > 42) {
            ConsolePrintf("DMG: (%s) '%d'=%8.2f, total=%f", label, type, oldValue, newValue);
        } else {
            ConsolePrintf("DMG: (%s) '%s'=%8.2f, total=%f", label, s_damageCalcLabels[type].m_name, oldValue, newValue);
        }
    }

    ConsolePrintf("DMG: ==============================================");

    return 1;
}

// OFFSET: 0x00559D60
static bool OnSummonRaidMemberValidateFailed(ClientSummonRaidMemberValidateFailed* msg) {
    for (uint32_t i = 0; i < msg->m_members.Count(); i++) {
        JamSummonRaidMemberValidateFailed* member = &msg->m_members.m_data[i];
        NameCache* name = g_nameDBCache.GetRecord(*reinterpret_cast<uint64_t*>(member->m_memberGUID), WOWGUID(), NULL, NULL, false);

        if (!name) {
            continue;
        }

        const char* format;

        switch (member->m_reasonCode) {
            case 1:
                format = FrameScript_GetText("RAID_SUMMON_FAILED_REALM_MISMATCH", -1, GENDER_NOT_APPLICABLE);
                break;

            case 2:
                format = FrameScript_GetText("RAID_SUMMON_FAILED_RAID_LOCKED", -1, GENDER_NOT_APPLICABLE);
                break;

            case 3:
                format = FrameScript_GetText("RAID_SUMMON_FAILED_MAP_CONDITION", -1, GENDER_NOT_APPLICABLE);
                break;

            case 4:
                format = FrameScript_GetText("RAID_SUMMON_FAILED_DEAD_OR_GHOST", -1, GENDER_NOT_APPLICABLE);
                break;

            case 5:
                format = FrameScript_GetText("RAID_SUMMON_FAILED_OFFLINE", -1, GENDER_NOT_APPLICABLE);
                break;

            default:
                continue;
        }

        char text[3000];
        SStrPrintf(text, sizeof(text), format, name->m_name);
        CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    }

    return true;
}

// OFFSET: 0x00559EA0
static bool OnQuestNpcQueryResponse(ClientQuestNpcQueryResponse* msg) {
    for (uint32_t i = 0; i < msg->m_quests.Count(); i++) {
        JamQuestCompletionNPC* quest = &msg->m_quests.m_data[i];

        for (uint32_t j = 0; j < quest->m_npcs.Count(); j++) {
            CGQuestLog::AddQuestCompletionNPC(quest->m_questID, quest->m_npcs.m_data[j]);
        }
    }

    return true;
}

// OFFSET: 0x00559F40
static bool OnSpellInterruptLog(ClientSpellinterruptlog* msg) {
    const SpellRec* interrupting = g_spellDB.GetRecord(msg->m_interruptingSpellID);
    const SpellRec* interrupted = g_spellDB.GetRecord(msg->m_interruptedSpellID);
    UnitCombatLogInterrupt(*reinterpret_cast<WOWGUID*>(msg->m_casterGUID), *reinterpret_cast<WOWGUID*>(msg->m_victimGUID), interrupting, interrupted);
    return true;
}

// OFFSET: 0x00559FA0
static bool OnXpGainAborted(ClientXpGainAborted* msg) {
    if (msg->m_reason) {
        return true;
    }

    if (ClientServices::LoginConnection() && ClientServices::LoginConnection()->IsTrialAccount() && CGGameUI::s_restrictedLevel) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRIAL_CAP_REACHED_LEVEL, NULL);
    }

    return true;
}

// OFFSET: 0x0055E4B0
int32_t Packet_SMSG_LEVEL_UP_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t level;
    uint32_t health;
    uint32_t power[5];
    uint32_t stats[5];
    msg->Get(level);
    msg->Get(health);

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(power[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(stats[i]);
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 1;
    }

    NumTalentsAtLevelRec* current = g_numTalentsAtLevelDB.GetRecord(level);
    NumTalentsAtLevelRec* previous = g_numTalentsAtLevelDB.GetRecord(level - 1);
    int32_t talentPoints = 0;

    if (current && previous) {
        talentPoints = static_cast<int32_t>(current->m_numTalents - previous->m_numTalents);
    }

    if (level >= 4) {
        CGTutorial::TriggerTutorial(13);
    }

    if (level >= 6) {
        CGTutorial::TriggerTutorial(22);
    }

    if (level >= 7) {
        CGTutorial::TriggerTutorial(21);
        CGTutorial::TriggerTutorial(37);
    }

    if (level >= 8) {
        CGTutorial::TriggerTutorial(32);
    }

    if (talentPoints) {
        CGTutorial::TriggerTutorial(12);
    }

    if (level >= 12) {
        CGTutorial::TriggerTutorial(38);
    }

    if (level >= 15) {
        CGTutorial::TriggerTutorial(50);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LEVEL_UP, "%d%d%d%d%d%d%d%d%d", level, health, power[0], talentPoints, stats[0], stats[1], stats[2], stats[3], stats[4]);
    return 1;
}

// OFFSET: 0x0055E670
int32_t Packet_SMSG_EXPLORATION_EXPERIENCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t areaId;
    int32_t experience;
    msg->Get(areaId);
    msg->Get(experience);
    AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (!area) {
        return 1;
    }

    CGGameUI::DisplayError(ERR_ZONE_EXPLORED, static_cast<const char*>(area->m_areaName));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (experience > 0) {
        if (!player) {
            return 1;
        }

        if (!(player->m_player->m_playerFlags & 0x2000000)) {
            CGGameUI::DisplayError(ERR_ZONE_EXPLORED_XP, static_cast<const char*>(area->m_areaName), experience);
        }
    }

    if (!player) {
        return 1;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(player->m_unit->m_unitFieldBytes0[0]);

    if (race && race->m_explorationSoundID) {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit(race->m_explorationSoundID, NULL, &properties, NULL, __FILE__, __LINE__, "SMSG_EXPLORATION_EXPERIENCE", 0, NULL);
    }

    return 1;
}

// OFFSET: 0x0055E7A0
int32_t Packet_SMSG_BINDER_CONFIRM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TalkToBinder(guid);
    }

    return 1;
}

// OFFSET: 0x0055E7F0
int32_t Packet_SMSG_DEATH_RELEASE_LOC(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    C3Vector position(0.0f, 0.0f, 0.0f);
    int32_t mapId;
    msg->Get(mapId);
    msg->Get(position.x);
    msg->Get(position.y);
    msg->Get(position.z);

    if (!player || (player->m_player->m_playerFlags & 0x10)) {
        position = C3Vector(0.0f, 0.0f, 0.0f);
        CGGameUI::SetDeathReleaseLocation(-1, position);
    } else {
        CGGameUI::SetDeathReleaseLocation(mapId, position);
    }

    return 1;
}

// OFFSET: 0x0055E8C0
int32_t Packet_SMSG_CAMERA_SHAKE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t shakesId;
    uint32_t soundId;
    msg->Get(shakesId);
    msg->Get(soundId);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    C3Vector position = player->GetPosition();
    SpellEffectCameraShakesRec* shakes = g_spellEffectCameraShakesDB.GetRecord(shakesId);

    if (shakes) {
        for (uint32_t i = 0; i < 3; i++) {
            StaticSingleton<CCameraManager>::Instance()->AddShakeById(shakes->m_cameraShake[i], position);
        }
    }

    if (soundId) {
        SE3SoundKitProperties properties;
        properties.ResetToDefaults();
        SE3::PlaySoundKit(soundId, &position, &properties, NULL, __FILE__, __LINE__, "OnCameraShake", 0, NULL);
    }

    return 1;
}

// OFFSET: 0x0055EA60
static bool OnClientItemExpirePurchaseRefund(ClientItemExpirePurchaseRefund* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::OnItemExpirePurchaseRefund(*reinterpret_cast<WOWGUID*>(msg->m_itemGUID));
    }

    return true;
}

// OFFSET: 0x0055F180
int32_t Packet_MSG_SAVE_GUILD_EMBLEM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t result = 0;
    msg->Get(result);

    if (result > 5) {
        return 1;
    }

    if (s_guildEmblemErrors[result] != ERR_MAX_VALUE) {
        CGGameUI::DisplayError(s_guildEmblemErrors[result]);
    }

    s_tabardSavePending = 0;

    if (result) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TABARD_SAVE_PENDING, NULL);
        return 1;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID guild = player->m_obj->m_dataType == 1 ? player->m_obj->m_data : WOWGUID();
        g_guildInfoCache.Invalidate(guild.m_guid);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDTABARD_UPDATE, NULL);
    return 1;
}

// OFFSET: 0x00560F90
static bool OnGuildSendGuildXP(ClientGuildSendGuildXP* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->m_guildXP = msg->m_guildCurrentXP;
        player->m_guildRemainingXP = msg->m_guildRemainingXP;
        player->m_guildDailyXP = msg->m_guildTodayXP;
        player->m_guildPlayerWeeklyXP = msg->m_memberWeeklyXP;
        player->m_guildPlayerTotalXP = msg->m_memberTotalXP;
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_XP_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x00561030
static bool OnGuildSendMaxDailyXP(ClientGuildSendMaxDailyXP* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->m_guildMaxDailyXP = msg->m_xp;
    }

    return true;
}

// OFFSET: 0x00561070
static bool OnGuildSendRankChange(ClientGuildSendRankChange* msg) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return true;
    }

    uint64_t officerGUID = *reinterpret_cast<uint64_t*>(msg->m_officerGUID);
    uint64_t otherGUID = *reinterpret_cast<uint64_t*>(msg->m_otherGUID);
    GuildMemberInfo* officer = NULL;
    GuildMemberInfo* other = NULL;
    uint32_t count = CGGuildInfo::GetNumMembers(1);

    for (uint32_t i = 0; i < count; i++) {
        if (i >= CGGuildInfo::m_numMembers) {
            continue;
        }

        GuildMemberInfo* member = CGGuildInfo::m_members.m_data[i];

        if (!member) {
            continue;
        }

        if (officerGUID && member->m_guid == officerGUID) {
            officer = member;
        } else if (member->m_guid == otherGUID) {
            other = member;
        }

        if ((officer || !officerGUID) && other) {
            break;
        }
    }

    const char* rankName = CGGuildInfo::GetRankNameFromRankID(msg->m_rankID);

    if (other && rankName) {
        if (msg->m_promote) {
            if (officer) {
                CGGameUI::DisplayError(ERR_GUILD_PROMOTE_SSS, officer->m_name, other->m_name, rankName);
            }
        } else if (officer) {
            CGGameUI::DisplayError(ERR_GUILD_DEMOTE_SSS, officer->m_name, other->m_name, rankName);
        } else {
            CGGameUI::DisplayError(ERR_GUILD_DEMOTE_SS, other->m_name, rankName);
        }
    }

    CGGuildInfo::UpdateRoster(1);
    return true;
}

// OFFSET: 0x005611D0
static bool OnGuildMemberRecipes(ClientGuildMemberRecipes* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGGuildInfo::SetGuildMemberRecipes(*reinterpret_cast<uint64_t*>(msg->m_memberGUID), msg->m_skillLineID, msg->m_skillRank, msg->m_skillStep, msg->m_skillLineBitArray, sizeof(msg->m_skillLineBitArray));
    }

    return true;
}

// OFFSET: 0x00561220
static bool OnGuildKnownRecipes(ClientGuildKnownRecipes* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        for (uint32_t i = 0; i < msg->m_recipes.Count(); i++) {
            JamGuildKnownRecipes* recipes = &msg->m_recipes.m_data[i];
            CGGuildInfo::SetGuildKnownRecipes(recipes->m_skillLineID, recipes->m_skillLineBitArray, sizeof(recipes->m_skillLineBitArray));
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_TRADESKILL_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x00561290
static bool OnStartTimer(ClientStartTimer* msg) {
    if (ClntObjMgrGetActivePlayerObj() && !msg->m_type) {
        FrameScript_SignalEvent(SCRIPT_EVENT_START_TIMER, "%d%d%d", 0, msg->m_timeLeft, msg->m_totalTime);
    }

    return true;
}

// OFFSET: 0x005612E0/0x1401BF180
bool CGPlayer_C::IsGuildLeader() const {
    if (this->m_obj->m_dataType != 1 || !this->m_obj->m_data) {
        return false;
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return CGGuildInfo::GetRankOrderFromRankID(this->m_player->m_playerGuildrank) == 0;
    }

    WOWGUID guid = this->m_obj->m_guid;
    WOWGUID guild = this->m_obj->m_dataType == 1 ? this->m_obj->m_data : WOWGUID();
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(guild.m_guid, guid, NULL, NULL, false);

    if (!stats || stats->m_numRanks <= 0) {
        return false;
    }

    for (int32_t i = 0; i < stats->m_numRanks; i++) {
        if (stats->m_rankIds[i] == static_cast<int32_t>(this->m_player->m_playerGuildrank)) {
            return stats->m_rankOrders[i] == 0;
        }
    }

    return false;
}

// OFFSET: 0x00564380
int32_t Packet_SMSG_SPIRIT_HEALER_CONFIRM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TalkToSpiritHealer(guid);
    }

    return 1;
}

// OFFSET: 0x005643D0
int32_t Packet_MSG_TALENT_WIPE_CONFIRM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (!guid) {
        CGGameUI::DisplayError(ERR_TALENT_WIPE_ERROR);
        return 1;
    }

    uint32_t cost;
    msg->Get(cost);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TalkToTalentMaster(guid, cost);
    }

    return 1;
}

// OFFSET: 0x00564450
int32_t Packet_SMSG_FORCED_DEATH_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGPlayerLocalData* data = player->m_player;
    uint8_t restFlags = data->m_playerFieldBytes[0];

    if (restFlags & 0x10) {
        CGGameUI::SetRepopTimeLeft(0);
    } else if (!(restFlags & 0x8) || (data->m_playerFlags & 0x4000)) {
        CGGameUI::ClearRepopTime();
    } else {
        CGGameUI::SetRepopTimeLeft(360000);
    }

    CGGameUI::UpdateActivePlayer();
    return 1;
}

// OFFSET: 0x005644D0
int32_t Packet_SMSG_CONVERT_RUNE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t index;
    uint8_t rune;
    msg->Get(index);
    msg->Get(rune);

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGRuneInfo::ConvertRune(index, rune);
    } else {
        RuneConvert convert;
        convert.m_rune = index;
        convert.m_type = rune;
        s_pendingRuneConverts.Add(1, &convert);
    }

    return 1;
}

// OFFSET: 0x00564C20
int32_t Packet_SMSG_GUILD_EVENT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t event = 0;
    uint8_t numStrings;
    char strings[3][1024];
    msg->Get(event);
    msg->Get(numStrings);
    strings[0][0] = 0;
    strings[1][0] = 0;
    strings[2][0] = 0;

    for (uint8_t i = 0; i < numStrings; i++) {
        msg->GetString(strings[i], sizeof(strings[i]));
        TruncateNewLines(strings[i]);
    }

    UIErrors error;
    WOWGUID guid;

    switch (event) {
        case 1:
            error = ERR_GUILD_PROMOTE_SSS;
            break;

        case 2:
            error = ERR_GUILD_DEMOTE_SSS;
            break;

        case 3: {
            CVar* filter = CVar::Lookup("profanityFilter");

            if (!filter || filter->m_intValue) {
                ProfanityFilter(strings[0], 0, 0);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_MOTD, "%s", strings[0]);
            CGGuildInfo::SetMOTD(strings[0]);
            CGGuildInfo::UpdateRoster(1);
            msg->m_read = msg->m_size;
            return 1;
        }

        case 4:
            error = ERR_GUILD_JOIN_S;
            msg->Get(guid.m_guid);
            CGAutoCompleteName::AddFlags(guid, 34, 0, strings[0]);
            ClntObjMgrEnumVisibleObjects(GuildChangedUpdateProc, NULL);
            break;

        case 5: {
            error = ERR_GUILD_LEAVE_S;
            msg->Get(guid.m_guid);
            CGAutoCompleteName::RemoveFlags(guid, 2);
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player && player->m_obj->m_guid == guid) {
                CGGuildBankInfo::SetGuildBanker(WOWGUID());
                CGGuildInfo::ClearGuildInfo();
                CGAchievementInfo::ClearGuildInfo();
                ClntObjMgrEnumVisibleObjects(GuildChangedUpdateProc, NULL);
            }

            break;
        }

        case 6: {
            error = ERR_GUILD_REMOVE_SS;
            msg->Get(guid.m_guid);
            CGAutoCompleteName::RemoveFlagsByName(strings[0], 2, true, false);
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player && player->m_obj->m_guid == guid) {
                CGAutoCompleteName::RemoveFlagsFromAllNodes(2);
                CGGuildBankInfo::SetGuildBanker(WOWGUID());
                CGGuildInfo::ClearGuildInfo();
                CGAchievementInfo::ClearGuildInfo();
                ClntObjMgrEnumVisibleObjects(GuildChangedUpdateProc, NULL);
            }

            break;
        }

        case 7:
            error = ERR_GUILD_LEADER_IS_S;
            break;

        case 8:
            error = ERR_GUILD_LEADER_CHANGED_SS;
            break;

        case 9:
            error = ERR_GUILD_DISBANDED;
            CGAutoCompleteName::RemoveFlagsFromAllNodes(2);
            CGGuildBankInfo::SetGuildBanker(WOWGUID());
            memset(CGGuildBankInfo::m_tabInfo, 0, sizeof(CGGuildBankInfo::m_tabInfo));
            CGGuildInfo::ClearGuildInfo();
            CGAchievementInfo::ClearGuildInfo();
            break;

        case 11:
        case 12:
        case 13:
        case 14:
        case 15: {
            CGGuildInfo::QueryRanks();
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                WOWGUID guild = player->m_obj->m_dataType == 1 ? player->m_obj->m_data : WOWGUID();
                g_guildInfoCache.Invalidate(guild.m_guid);
                WOWGUID playerGUID = player->m_obj->m_guid;
                guild = player->m_obj->m_dataType == 1 ? player->m_obj->m_data : WOWGUID();
                g_guildInfoCache.GetRecord2(guild.m_guid, playerGUID, GuildRosterUpdateCallback, NULL, false);

                if (event == 11 && (player->m_player->m_playerGuildrank == static_cast<uint32_t>(atoi(strings[0])) || player->IsGuildLeader())) {
                    CGGuildInfo::QueryPermissions();

                    if (CGGuildBankInfo::m_banker) {
                        CGGuildBankInfo::QueryGuildBankTab(CGGuildBankInfo::m_currentTab);
                    }
                }
            }

            msg->m_read = msg->m_size;
            return 1;
        }

        case 16:
            msg->Get(guid.m_guid);
            CGAutoCompleteName::AddFlags(guid, 32, 0, strings[0]);

            if (g_guildMemberNotifyCVar->m_intValue && strcmp(ClientServices::GetCharacterName(), strings[0]) && !FriendList::s_friendList->IsFriend(guid)) {
                CGGameUI::DisplayError(ERR_FRIEND_ONLINE_SS, strings[0], strings[0]);
            }

            CGGuildInfo::SetMemberOnlineStatus(guid.m_guid, 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
            msg->m_read = msg->m_size;
            return 1;

        case 17:
            msg->Get(guid.m_guid);
            CGAutoCompleteName::RemoveFlags(guid, 32);

            if (g_guildMemberNotifyCVar->m_intValue && strcmp(ClientServices::GetCharacterName(), strings[0]) && !FriendList::s_friendList->IsFriend(guid)) {
                CGGameUI::DisplayError(ERR_FRIEND_OFFLINE_S, strings[0]);
            }

            CGGuildInfo::SetMemberOnlineStatus(guid.m_guid, 0);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
            msg->m_read = msg->m_size;
            return 1;

        case 18:
            CGGuildBankInfo::QueryGuildBankTab(CGGuildBankInfo::m_currentTab);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANKBAGSLOTS_CHANGED, NULL);
            msg->m_read = msg->m_size;
            return 1;

        case 19:
            CGGuildBankInfo::m_numTabs++;
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_TABS, NULL);
            msg->m_read = msg->m_size;
            return 1;

        case 20:
            CGGuildBankInfo::UpdateTabInfo(static_cast<uint8_t>(SStrToInt(strings[0])), strings[1], strings[2]);
            msg->m_read = msg->m_size;
            return 1;

        case 21: {
            uint64_t money;
            sscanf(strings[0], "%016llX", &money);
            CGGuildBankInfo::m_money = money;
            GlobalGuildBankRemainingWithdrawMoneyQuery query;
            ClientServices::Send(&query);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_MONEY, NULL);
            msg->m_read = msg->m_size;
            return 1;
        }

        case 23: {
            int32_t tab = atoi(strings[0]);
            CGGuildBankInfo::SetTextUpdate(static_cast<uint8_t>(tab), 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_TEXT_CHANGED, "%d", tab + 1);
            msg->m_read = msg->m_size;
            return 1;
        }

        case 24:
            error = ERR_GUILD_LEADER_REPLACED;
            break;

        case 25:
            msg->Get(guid.m_guid);
            CGGuildInfo::SetMemberMobile(guid.m_guid, 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
            msg->m_read = msg->m_size;
            return 1;

        case 26:
            msg->Get(guid.m_guid);
            CGGuildInfo::SetMemberMobile(guid.m_guid, 0);
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
            msg->m_read = msg->m_size;
            return 1;

        default:
            error = ERR_GUILD_INTERNAL;
            break;
    }

    switch (numStrings) {
        case 1:
            CGGameUI::DisplayError(error, strings[0]);
            break;

        case 2:
            CGGameUI::DisplayError(error, strings[0], strings[1]);
            break;

        case 3:
            CGGameUI::DisplayError(error, strings[0], strings[1], strings[2]);
            break;

        default:
            CGGameUI::DisplayError(error);
            break;
    }

    msg->m_read = msg->m_size;
    CGGuildInfo::UpdateRoster(1);
    return 1;
}

// OFFSET: 0x00567430
int32_t Packet_SMSG_INVENTORY_CHANGE_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t result = 0;
    msg->Get(result);

    if (!result) {
        return 1;
    }

    UIErrors error = static_cast<UIErrors>(GetDisplayErrorByInventoryError(result));
    WOWGUID item;
    WOWGUID item2;
    uint8_t bag;
    msg->Get(item.m_guid);
    msg->Get(item2.m_guid);
    msg->Get(bag);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    switch (result) {
        case 1:
        case 87: {
            uint32_t requiredLevel = 0;
            msg->Get(requiredLevel);
            CGGameUI::DisplayError(error, requiredLevel);
            break;
        }

        case 16:
            if (!ClntObjMgrGetActivePlayerObj() || !ClntObjMgrGetActivePlayerObj()->ReportBagItemSubtypeMismatch(bag)) {
                CGGameUI::DisplayError(error);
            }

            break;

        case 81: {
            WOWGUID a2;
            uint32_t a3 = 0;
            WOWGUID a4;
            msg->Get(a2.m_guid);
            msg->Get(a3);
            msg->Get(a4.m_guid);

            if (player) {
                CGPlayer_C::ServerBindConfirm(item, a2.m_guid, a3, a4.m_guid, bag);
            }

            goto unlock;
        }

        case 84:
        case 85:
        case 89: {
            uint32_t category = 0;
            msg->Get(category);
            ItemLimitCategoryRec* rec = g_itemLimitCategoryDB.GetRecord(category);
            CGGameUI::DisplayError(error, rec->m_quantity, static_cast<const char*>(rec->m_name));
            break;
        }

        default:
            CGGameUI::DisplayError(error);
            break;
    }

unlock:
    if (item) {
        if (player) {
            if (item == player->m_lootTarget) {
                CGGameUI::CloseLoot(true, false, false);

                if (result == 36) {
                    player->ClearLootingObject();
                }
            }

            Spell_C_CancelSpellByCaster(item);
        }

        CGGameUI::UnlockItem(item);
    }

    if (item2) {
        CGGameUI::UnlockItem(item2);
    }

    if (CGLootInfo::m_lootObj && player && player->m_autoLoot) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LOOT_OPENED, "%d", 1);
    }

    return 1;
}

// OFFSET: 0x00568A10
int32_t Packet_Group_9(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    switch (msgId) {
        case SMSG_PETGODMODE: {
            uint8_t enabled = 0;
            msg->Get(enabled);
            ConsoleWrite(enabled ? "Pet Godmode enabled" : "Pet Godmode disabled", DEFAULT_COLOR);
            return 1;
        }

        case SMSG_FISH_ESCAPED:
            CGGameUI::DisplayError(ERR_FISH_ESCAPED);
            return 1;

        case SMSG_FISH_NOT_HOOKED:
            CGGameUI::DisplayError(ERR_FISH_NOT_HOOKED);
            return 1;

        case SMSG_GOD_MODE: {
            uint8_t enabled = 0;
            msg->Get(enabled);
            ConsoleWrite(enabled ? "Godmode enabled" : "Godmode disabled", DEFAULT_COLOR);
            return 1;
        }

        case SMSG_BIND_POINT_UPDATE:
            CGPlayer_C::SaveBindPoint(msg);
            return 1;

        case SMSG_GM_TICKET_GET_SYSTEM_STATUS: {
            uint32_t status = 0;
            msg->Get(status);
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_GM_STATUS, "%d", status);
            return 1;
        }

        case SMSG_DISMOUNT_RESULT:
            if (ClntObjMgrGetActivePlayerObj()) {
                uint32_t result = 0;
                msg->Get(result);
                DisplayDismountResult(result);
            }

            return 1;

        case SMSG_TAXI_NODE_STATUS:
            if (ClntObjMgrGetActivePlayerObj()) {
                CGPlayer_C::OnTaxiNodeStatus(msg);
            }

            return 1;

        case SMSG_MOUNT_RESULT:
            if (ClntObjMgrGetActivePlayerObj()) {
                uint32_t result = 0;
                msg->Get(result);
                DisplayMountResult(result);
            }

            return 1;

        case SMSG_SHOW_BANK: {
            WOWGUID banker;
            banker.m_guid = 0;
            msg->Get(banker.m_guid);
            CGBankInfo::SetBanker(banker);
            return 1;
        }

        case SMSG_CLEAR_FAR_SIGHT_IMMEDIATE: {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                player->ClearFarSight();
            }

            return 1;
        }

        case SMSG_SHOW_TAXI_NODES:
            if (ClntObjMgrGetActivePlayerObj()) {
                CGPlayer_C::ShowTaxiNodes(msg);
            }

            return 1;

        case SMSG_CHAT_WRONG_FACTION:
            CGGameUI::DisplayError(ERR_CHAT_WRONG_FACTION);
            return 1;

        case SMSG_NEW_TAXI_PATH:
            CGGameUI::DisplayError(ERR_NEWTAXIPATH);
            CGGossipInfo::SetGossip(WOWGUID());
            return 1;

        case SMSG_TRIGGER_MOVIE: {
            uint32_t movie = 0;
            msg->Get(movie);
            CGPlayer_C::TriggerMovie(movie);
            return 1;
        }

        case SMSG_OPEN_CONTAINER: {
            WOWGUID guid;
            guid.m_guid = 0;
            msg->Get(guid.m_guid);
            CGContainerInfo::OpenContainer(guid);
            return 1;
        }

        case SMSG_RESYNC_RUNES:
            CGRuneInfo::ResyncRunes(msg);
            return 1;

        case SMSG_TOGGLE_XP_GAIN: {
            uint8_t enabled = 0;
            msg->Get(enabled);
            ConsoleWrite(enabled ? "XP Gain Enabled" : "XP Gain Disabled", DEFAULT_COLOR);
            return 1;
        }

        case SMSG_ADD_RUNE_POWER:
            CGRuneInfo::AddRunePower(msg);
            return 1;

        case SMSG_PLAYERBINDERROR:
            CGGameUI::DisplayError(ERR_DEATHBINDALREADYBOUND);
            return 1;

        case SMSG_ACTIVATE_TAXI_REPLY:
            if (ClntObjMgrGetActivePlayerObj()) {
                uint32_t reply = 0;
                msg->Get(reply);
                CGPlayer_C::HandleActivateTaxiReply(reply);
            }

            return 1;

        case SMSG_TRIGGER_CINEMATIC: {
            uint32_t cinematic = 0;
            msg->Get(cinematic);

            if (ClientServices::Connection()->m_inGame) {
                CGGameUI::StartCinematic(cinematic);
            } else {
                s_pendingMovie = cinematic;
            }

            return 1;
        }

        case SMSG_ARENA_TEAM_CHANGE_FAILED_QUEUED: {
            uint32_t failed = 0;
            msg->Get(failed);

            if (failed) {
                CGGameUI::DisplayError(ERR_ARENA_TEAM_CHANGE_FAILED_QUEUED);
            }

            return 1;
        }

        default:
            return 0;
    }
}

// OFFSET: 0x0056B100
int32_t Packet_SMSG_ITEM_PUSH_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint32_t received;
    uint32_t created;
    uint32_t showInChat;
    uint8_t bag;
    uint32_t slot;
    uint32_t itemId;
    uint32_t suffixFactor;
    uint32_t randomPropertyId;
    uint32_t count;
    uint32_t totalCount;
    msg->Get(guid.m_guid);
    msg->Get(received);
    msg->Get(created);
    msg->Get(showInChat);
    msg->Get(bag);
    msg->Get(slot);
    msg->Get(itemId);
    msg->Get(suffixFactor);
    msg->Get(randomPropertyId);
    msg->Get(count);
    msg->Get(totalCount);
    CGGameUI::OnItemPush(guid, bag, slot, itemId, suffixFactor, randomPropertyId, received, created, showInChat, count);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->CheckForQuestItemLoot(itemId, count, totalCount);
    }

    return 1;
}

// OFFSET: 0x0056DD00
static bool OnItemRefundInfoResponse(ClientItemRefundInfoResponse* msg) {
    if (!ClntObjMgrGetActivePlayer() || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    CGPlayer_C::OnRefundInfo(&msg->m_info);
    return true;
}

// OFFSET: 0x0056DD40
static bool OnItemRefundResult(ClientItemRefundResult* msg) {
    if (!ClntObjMgrGetActivePlayer() || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    CGPlayer_C::OnItemPurchaseRefundResult(&msg->m_result);
    return true;
}

// OFFSET: 0x0056DD80
static bool OnClientQuestGiverQuestComplete(ClientQuestGiverQuestComplete* msg) {
    if (!ClntObjMgrGetActivePlayer() || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    CGPlayer_C::OnQuestGiverQuestComplete(&msg->m_complete);
    return true;
}

// OFFSET: 0x0056FCB0
static bool OnLearnedSpell(ClientLearnedSpell* msg) {
    uint16_t suppress = static_cast<uint16_t>(msg->m_suppressMessaging);
    int32_t spellId = msg->m_spellID;

    if (SFile::IsStreamingMode() && !std::uncaught_exception()) {
        PrefetchSpellVisuals(spellId, 0);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->AddKnownSpell(spellId, static_cast<int16_t>(suppress), 1, 1);
        player->UpdateAuraEffectMask();
        const SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (spell) {
            player->RefreshSpellCategoryCooldowns(spell);
        }

        return true;
    }

    InitialSpellStruct* initial = s_initialSpells.New();
    initial->m_spellId = spellId;
    initial->unk_0004 = suppress;
    return true;
}

// OFFSET: 0x0056FDC0
static bool OnSupercededSpells(ClientSupercededSpells* msg) {
    int32_t oldSpellId = msg->m_supercededSpellID;
    int32_t newSpellId = msg->m_spellID;

    if (SFile::IsStreamingMode() && !std::uncaught_exception()) {
        PrefetchSpellVisuals(newSpellId, 0);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        CGActionBar::ReplaceSpell(oldSpellId, newSpellId);
        CGSpellBook::ReplaceSpell(oldSpellId, newSpellId);
        player->DelKnownSpell(oldSpellId, 1);
        player->AddKnownSpell(newSpellId, 0, 0, 0);
        player->UpdateAuraEffectMask();
        const SpellRec* spell = g_spellDB.GetRecord(newSpellId);

        if (spell) {
            player->RefreshSpellCategoryCooldowns(spell);
            CGClassTrainer::RefreshList();
            return true;
        }
    } else {
        uint32_t i;

        for (i = 0; i < s_initialSpells.Count(); i++) {
            if (s_initialSpells.m_data[i].m_spellId == oldSpellId) {
                s_initialSpells.m_data[i].m_spellId = newSpellId;
                break;
            }
        }

        if (i == s_initialSpells.Count()) {
            InitialSpellStruct* initial = s_initialSpells.New();
            initial->m_spellId = newSpellId;
            initial->unk_0004 = 0;
        }
    }

    CGClassTrainer::RefreshList();
    return true;
}

// OFFSET: 0x005551D0/0x1401B1A30
void CGPlayer_C::CheckQuestAutoComplete(int32_t questId, int32_t itemId, int32_t count) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !questId) {
        return;
    }

    QuestCache* quest = g_questDBCache.GetRecord(questId, WOWGUID(), NULL, NULL, false);

    if (!quest || !(quest->m_flags & 0x10000)) {
        return;
    }

    int32_t index = CGQuestLog::GetQuestIndex(quest->m_id) - 1;

    if (!CGQuestLog::HasAutoCompleteBeenTried(index) && CGQuestLog::IsQuestCompleteEx(this, index, 1, itemId, count)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_AUTOCOMPLETE, "%d", quest->m_id);
    }
}

// OFFSET: 0x00555340
void CGPlayer_C::TriggerMovie(uint32_t movie) {
    if (!MovieStreaming::IsMoviePlayable(movie)) {
        MovieStreaming::WaitForDownloadingMovies(movie);
    }

    if (ClientServices::Connection()->m_inGame) {
        CGGameUI::StartMovie(movie);
    } else {
        s_pendingMovieID = movie;
    }
}

// OFFSET: 0x005557A0
static int32_t ReportQuestKillCredit(const uint32_t* credit, int32_t questId) {
    QuestCache* quest = g_questDBCache.GetRecord(questId, WOWGUID(), NULL, NULL, false);

    if (quest) {
        for (uint32_t i = 0; i < 4; i++) {
            if (static_cast<int32_t>(quest->m_objectiveId[i]) == static_cast<int32_t>(credit[0]) && quest->m_objectiveText[i][0]) {
                CGGameUI::DisplayError(ERR_QUEST_ADD_FOUND_SII, quest->m_objectiveText[i], credit[1], credit[2]);
                FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
                return 1;
            }
        }
    }

    int32_t entry = static_cast<int32_t>(credit[0]);

    if (entry >= 0) {
        CreatureStats_C* creature = g_creatureDBCache.GetRecord(entry, WOWGUID(), NULL, NULL, false);

        if (creature) {
            CGGameUI::DisplayError(ERR_QUEST_ADD_KILL_SII, creature->GetName(FrameScript_GetPluralIndex(credit[2])), credit[1], credit[2]);
            FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
        }
    } else {
        GameObjectStats_C* gameObject = g_gameObjectDBCache.GetRecord(entry & 0x7FFFFFFF, WOWGUID(), NULL, NULL, false);

        if (gameObject) {
            CGGameUI::DisplayError(ERR_QUEST_ADD_FOUND_SII, gameObject->GetName(FrameScript_GetPluralIndex(credit[2])), credit[1], credit[2]);
            FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(questId));
        }
    }

    return 1;
}

// OFFSET: 0x005558F0
static void QuestKillCreditCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    uint32_t* credit = static_cast<uint32_t*>(param);

    if (success) {
        ReportQuestKillCredit(credit, id);
    }

    if (credit) {
        SMemFree(credit, "delete", -1, 0);
    }
}

// OFFSET: 0x0056AE70
static int32_t ReportQuestItemLoot(CGPlayer_C* player, QuestCache* quest, int32_t itemId, int32_t count, int32_t total) {
    if (!quest) {
        return 0;
    }

    uint32_t index = 0;

    while (static_cast<int32_t>(quest->m_collectItemId[index]) != itemId) {
        if (++index >= 6) {
            return 0;
        }
    }

    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!item) {
        return 1;
    }

    CGPlayer_C* active = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (active && total - count < static_cast<int32_t>(quest->m_collectItemCount[index])) {
        CGGameUI::DisplayError(ERR_QUEST_ADD_ITEM_SII, item->GetName(FrameScript_GetPluralIndex(quest->m_collectItemCount[index])), total, quest->m_collectItemCount[index]);
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_WATCH_UPDATE, "%d", CGQuestLog::GetQuestIndex(quest->m_id));
        active->CheckQuestAutoComplete(quest->m_id, itemId, total);
    }

    return 1;
}

// OFFSET: 0x0056AF70
static void QuestItemLootCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    int32_t* loot = static_cast<int32_t*>(param);

    if (success) {
        QuestCache* quest = g_questDBCache.GetRecord(id, WOWGUID(), NULL, NULL, false);
        ReportQuestItemLoot(NULL, quest, loot[0], loot[1], loot[2]);
    }

    if (loot) {
        SMemFree(loot, "delete", -1, 0);
    }
}

// OFFSET: 0x0056AFE0
void CGPlayer_C::CheckForQuestItemLoot(int32_t itemId, int32_t count, int32_t total) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    for (uint32_t i = 0; i < 50; i++) {
        PlayerQuestLogEntry* entry = &this->m_player->m_playerQuestLog[i];

        if (entry->m_questId <= 0 || CGQuestLog::IsQuestExpiredByID(entry->m_questId)) {
            continue;
        }

        QuestCache* quest = g_questDBCache.GetRecord(entry->m_questId, WOWGUID(), NULL, NULL, false);

        if (quest) {
            if (ReportQuestItemLoot(this, quest, itemId, count, total)) {
                return;
            }
        } else {
            int32_t* loot = static_cast<int32_t*>(SMemAlloc(3 * sizeof(int32_t), __FILE__, __LINE__, 0));
            loot[0] = itemId;
            loot[1] = count;
            loot[2] = total;
            g_questDBCache.GetRecord(entry->m_questId, WOWGUID(), QuestItemLootCallback, loot, false);
        }
    }
}

// OFFSET: 0x00556B30
void __stdcall CGPlayer_C::OnItemExpirePurchaseRefund(WOWGUID itemGuid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        if (item->m_itemPurchaseData) {
            SMemFree(item->m_itemPurchaseData, "delete", -1, 0);
        }

        item->m_itemPurchaseData = NULL;
    }
}

// OFFSET: 0x0056BC90/0x1401CB9D0
static void QuestRewardItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    ItemRecSparse* item = g_itemSparseDB.GetRecord(id, WOWGUID(), NULL, NULL, false);

    if (!item) {
        return;
    }

    int32_t count = static_cast<int32_t>(reinterpret_cast<intptr_t>(param));
    const char* link = CGTooltip::GetItemHyperlinkString(id, item->m_quality, 0, 0, 0, 0);

    if (count <= 1) {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_ITEM_S, link);
    } else {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_ITEM_MULT_IS, count, link);
    }
}

// OFFSET: 0x0056BD10/0x1401CBA80
static void QuestCompleteCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    JamQuestGiverQuestComplete* complete = static_cast<JamQuestGiverQuestComplete*>(param);

    if (!success) {
        if (complete) {
            SMemFree(complete, "delete", -1, 0);
        }

        return;
    }

    QuestCache* quest = g_questDBCache.GetRecord(id, WOWGUID(), NULL, NULL, false);
    CGGameUI::DisplayError(ERR_QUEST_COMPLETE_S, quest->m_name);

    if (quest->m_soundTurnIn) {
        SE3SoundKitProperties properties;
        properties.ResetToDefaults();
        SE3::PlaySoundKit(quest->m_soundTurnIn, NULL, NULL, NULL, __FILE__, __LINE__, "QuestCompleteCallback()", 0, NULL);
    }

    if (complete->m_xpReward) {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_EXP_I, complete->m_xpReward);
    }

    if (complete->m_moneyReward > 0) {
        char money[1024];
        GetCurrencyText(complete->m_moneyReward, money, sizeof(money), ", ");
        CGGameUI::DisplayError(ERR_QUEST_REWARD_MONEY_S, money);
    }

    if (complete->m_talentReward > 0) {
        const char* format = FrameScript_GetText("LEVEL_UP_CHAR_POINTS", -1, GENDER_NOT_APPLICABLE);

        if (format && *format) {
            char text[3000];
            SStrPrintf(text, sizeof(text), format, complete->m_talentReward);
            CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(28), NULL);
        }
    }

    WOWGUID player = ClntObjMgrGetActivePlayer();

    for (uint32_t i = 0; i < 4; i++) {
        int32_t itemId = quest->m_rewardItem[i];

        if (itemId > 0 && g_itemSparseDB.GetRecord(itemId, player, QuestRewardItemCallback, reinterpret_cast<void*>(static_cast<intptr_t>(quest->m_rewardItemCount[i])), false)) {
            QuestRewardItemCallback(itemId, player, reinterpret_cast<void*>(static_cast<intptr_t>(quest->m_rewardItemCount[i])), true);
        }
    }

    int32_t chosen = CGQuestInfo::m_lastChosenItem;

    if (chosen) {
        if (g_itemSparseDB.GetRecord(chosen, player, QuestRewardItemCallback, NULL, false)) {
            ItemRecSparse* item = g_itemSparseDB.GetRecord(chosen, WOWGUID(), NULL, NULL, false);

            if (item) {
                CGGameUI::DisplayError(ERR_QUEST_REWARD_ITEM_S, CGTooltip::GetItemHyperlinkString(chosen, item->m_quality, 0, 0, 0, 0));
            }
        }

        CGQuestInfo::m_lastChosenItem = 0;
    }

    int32_t launchGossip = quest->m_nextQuestId && !(quest->m_flags & 0x10000) && complete->m_launchGossip ? 1 : 0;
    CGQuestInfo::QuestGiverFinished(launchGossip, id, 0, complete->m_useQuestReward);
    SMemFree(complete, "delete", -1, 0);
}

// OFFSET: 0x0056BF60
void __stdcall CGPlayer_C::OnQuestGiverQuestComplete(const JamQuestGiverQuestComplete* complete) {
    JamQuestGiverQuestComplete* copy = static_cast<JamQuestGiverQuestComplete*>(SMemAlloc(sizeof(JamQuestGiverQuestComplete), __FILE__, __LINE__, 0));

    if (copy) {
        memcpy(copy, complete, sizeof(JamQuestGiverQuestComplete));
    }

    if (g_questDBCache.GetRecord(copy->m_questID, WOWGUID(), QuestCompleteCallback, copy, false)) {
        QuestCompleteCallback(copy->m_questID, WOWGUID(), copy, true);
    }
}

// OFFSET: 0x0056BFE0/0x1401CBED0
static void SetPurchaseDataFromRefundCost(const JamItemPurchaseRefundCost* cost, CGItem_C::ItemPurchaseData* data) {
    data->m_money = cost->m_money;

    for (uint32_t i = 0; i < 5; i++) {
        data->m_itemId[i] = cost->m_items[i].m_id;
        data->m_itemCount[i] = cost->m_items[i].m_count;
        g_itemSparseDB.GetRecord(cost->m_items[i].m_id, WOWGUID(), NULL, NULL, true);
    }

    for (uint32_t i = 0; i < 5; i++) {
        data->m_currencyId[i] = cost->m_currencies[i].m_id;
        data->m_currencyCount[i] = cost->m_currencies[i].m_count;
    }
}

// OFFSET: 0x0056C080
void __stdcall CGPlayer_C::OnRefundInfo(const JamItemRefundInfo* info) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(*reinterpret_cast<const WOWGUID*>(info->m_itemGUID), TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return;
    }

    CGItem_C::ItemPurchaseData* data = item->m_itemPurchaseData;

    if (!data) {
        data = static_cast<CGItem_C::ItemPurchaseData*>(SMemAlloc(sizeof(CGItem_C::ItemPurchaseData), __FILE__, __LINE__, 0));

        if (!data) {
            return;
        }

        item->SetItemPurchaseData(data);
    }

    SetPurchaseDataFromRefundCost(&info->m_cost, data);
    data->m_flags = info->m_flags;
    data->m_refundTimeLeft = info->m_refundTimeLeft;
}

// OFFSET: 0x0056C100/0x1401CC040
void __stdcall CGPlayer_C::OnItemPurchaseRefundResult(const JamItemRefundResult* result) {
    uint8_t error = result->m_result;

    if (*reinterpret_cast<const uint64_t*>(result->m_itemGUID)) {
        ClntObjMgrObjectPtr(*reinterpret_cast<const WOWGUID*>(result->m_itemGUID), TYPEMASK_ITEM, __FILE__, __LINE__);
    }

    if (error) {
        char text[256];

        if (error == 9) {
            CGGameUI::DisplayError(ERR_INV_FULL);
            SStrPrintf(text, sizeof(text), "Item purchase refund error code %d", 9);
            ConsoleWrite(text, ERROR_COLOR);
        } else if (error == 10) {
            CGGameUI::DisplayError(ERR_REFUND_RESULT_EXCEED_MAX_CURRENCY);
        } else {
            CGChat::AddChatMessage(FrameScript_GetText("UNABLE_TO_REFUND_ITEM", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
            SStrPrintf(text, sizeof(text), "Item purchase refund error code %d", error);
            ConsoleWrite(text, ERROR_COLOR);
        }

        return;
    }

    CGItem_C::ItemPurchaseData data;
    SetPurchaseDataFromRefundCost(result->m_hasCost ? &result->m_cost : NULL, &data);
    data.m_flags = 0;
    data.m_refundTimeLeft = 0;
    char text[3000];

    for (uint32_t i = 0; i < 5; i++) {
        int32_t count = data.m_itemCount[i];

        if (count <= 0) {
            continue;
        }

        int32_t itemId = data.m_itemId[i];
        ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

        if (!item) {
            continue;
        }

        const char* link = CGTooltip::GetItemHyperlinkString(itemId, item->m_quality, 0, 0, 0, 0);

        if (count > 1) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ITEM_REFUND_MULTIPLE", -1, GENDER_NOT_APPLICABLE), link, count);
        } else {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ITEM_REFUND", -1, GENDER_NOT_APPLICABLE), link);
        }

        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(27), NULL);
    }

    int32_t money = data.m_money;
    bool sound = false;

    for (uint32_t i = 0; i < 5; i++) {
        int32_t count = data.m_currencyCount[i];

        if (count <= 0) {
            continue;
        }

        CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(data.m_currencyId[i]);

        if (!currency) {
            continue;
        }

        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_CURRENCY_REFUND", -1, GENDER_NOT_APPLICABLE), CGTooltip::GetCurrencyHyperlinkString(currency->m_id), count);
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(64), NULL);

        if (!sound && !money) {
            SE3SoundKitProperties properties;
            properties.ResetToDefaults();
            SE3::PlaySoundKit("LOOTWINDOWCOINSOUND", NULL, &properties, NULL, __FILE__, __LINE__, "CGPlayer_C::OnItemPurchaseRefundResult");
            sound = true;
        }
    }

    if (money > 0) {
        char moneyText[1024];
        GetCurrencyText(money, moneyText, sizeof(moneyText), ", ");
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_MONEY_REFUND", -1, GENDER_NOT_APPLICABLE), moneyText);
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(28), NULL);
    }
}

// OFFSET: 0x00569BC0
int32_t Packet_SMSG_GROUP_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t groupType;
    msg->Get(groupType);
    bool isRaid = (groupType & 0x2) != 0;
    uint8_t isBattleground = groupType & 0x1;
    bool onePersonParty = (groupType & 0x20) != 0;
    uint8_t everyoneIsAssistant = (groupType & 0x40) != 0;
    CGRaidInfo::m_everyoneIsAssistant = everyoneIsAssistant;
    uint8_t subgroup;
    msg->Get(subgroup);
    uint8_t assignments;
    msg->Get(assignments);
    uint8_t roles;
    msg->Get(roles);
    uint8_t lfgFlags = 0;
    uint32_t lfgSlot = 0;
    uint8_t lfgUnk = 0;

    if (groupType & 0x8) {
        msg->Get(lfgFlags);
        msg->Get(lfgSlot);
        uint8_t value = 0;
        msg->Get(value);
        lfgUnk = value;
    }

    WOWGUID group;
    msg->Get(group.m_guid);
    uint32_t sequence;
    msg->Get(sequence);

    if (group && sequence) {
        if (groupType & 0x1) {
            if (CGPlayer_C::s_lastUpdatePVPGroup == group && CGPlayer_C::s_lastUpdatePVPTime && CGPlayer_C::s_lastUpdatePVPSequence >= sequence && static_cast<int32_t>(static_cast<uint32_t>(_time32(NULL)) - CGPlayer_C::s_lastUpdatePVPTime - 60) < 0) {
                msg->m_read = msg->m_size;
                return 1;
            }

            CGPlayer_C::s_lastUpdatePVPGroup = group;
            CGPlayer_C::s_lastUpdatePVPSequence = sequence;
            CGPlayer_C::s_lastUpdatePVPTime = static_cast<uint32_t>(_time32(NULL));
        } else {
            if (CGPlayer_C::s_lastUpdateGroup == group && CGPlayer_C::s_lastUpdateTime && CGPlayer_C::s_lastUpdateSequence >= sequence && static_cast<int32_t>(static_cast<uint32_t>(_time32(NULL)) - CGPlayer_C::s_lastUpdateTime - 60) < 0) {
                msg->m_read = msg->m_size;
                return 1;
            }

            CGPlayer_C::s_lastUpdateGroup = group;
            CGPlayer_C::s_lastUpdateSequence = sequence;
            CGPlayer_C::s_lastUpdateTime = static_cast<uint32_t>(_time32(NULL));
        }
    }

    uint32_t memberCount;
    msg->Get(memberCount);
    int32_t wasInGroup = 0;
    PartyMemberState oldStates[4];
    WOWGUID oldMembers[4] = {};
    WOWGUID partyGuids[4] = {};
    WOWGUID raidGuids[41] = {};
    PartyReadyCheckInfo oldReadyChecks[4];
    uint8_t oldRoles = CGPartyInfo::m_playerRole;
    uint32_t oldCount = 0;
    WOWGUID oldLeader = CGPartyInfo::m_leader;

    for (uint32_t i = 0; i < 4; i++) {
        oldMembers[i] = CGGameUI::GetPartyMember(i);

        if (!oldMembers[i]) {
            continue;
        }

        const PartyMemberState* state = CGPartyInfo::m_members[i] ? &s_partyMemberStates[i] : NULL;
        oldStates[i].Copy(state);
        CGPartyInfo::GetReadyCheckInfo(&oldReadyChecks[i], i);
        oldCount++;
        wasInGroup = 1;
    }

    CGPartyInfo::RemoveAll(0);

    if (CGRaidInfo::m_numMembers) {
        wasInGroup = 1;
    }

    uint32_t partyCount = 0;
    int32_t joined = 0;
    uint32_t subgroups[41];
    uint8_t raidFlags[41];
    uint32_t raidRoles[41];
    uint8_t online[41];
    char name[48];

    for (uint32_t j = 0; j < memberCount; j++) {
        msg->GetString(name, sizeof(name));
        WOWGUID guid;
        msg->Get(guid.m_guid);
        msg->Get(online[j]);
        uint8_t memberSubgroup;
        msg->Get(memberSubgroup);
        uint8_t memberFlags;
        msg->Get(memberFlags);
        uint8_t memberRoles;
        msg->Get(memberRoles);

        if (guid == ClntObjMgrGetActivePlayer()) {
            continue;
        }

        subgroups[j] = memberSubgroup;
        raidFlags[j] = memberFlags;
        raidGuids[j] = guid;
        raidRoles[j] = memberRoles;

        if (memberSubgroup != subgroup || partyCount >= 4) {
            continue;
        }

        partyGuids[partyCount] = guid;
        uint32_t i = 4;

        if (wasInGroup) {
            for (i = 0; i < 4; i++) {
                if (oldMembers[i] == guid) {
                    break;
                }
            }

            if (!isRaid && i == 4) {
                CGGameUI::DisplayError(ERR_JOINED_GROUP_S, name);
                joined = 1;
            }
        } else {
            joined = 1;
        }

        const PartyMemberState* state = NULL;
        const PartyReadyCheckInfo* readyCheck = NULL;

        if (i < 4) {
            state = &oldStates[i];
            readyCheck = &oldReadyChecks[i];
        }

        CGPartyInfo::AddMember(guid, online[j] != 0, state, memberFlags, memberRoles, readyCheck);
        partyCount++;
    }

    WOWGUID leader;
    msg->Get(leader.m_guid);
    CGRaidInfo::m_isInFakeRaid = isBattleground;

    if (!isBattleground) {
        if (isRaid) {
            bool converted = CGPartyInfo::m_realNumMembers && !CGRaidInfo::m_realNumMembers;
            CGRaidInfo::UpdateRealStats(memberCount ? memberCount + 1 : 0, leader);

            if (converted && CGRaidInfo::m_realNumMembers) {
                Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_CONVERTED_TO_RAID);
            }
        } else {
            CGRaidInfo::UpdateRealStats(0, leader);
        }

        CGPartyInfo::UpdateRealStats(partyCount, leader);
    }

    CGPartyInfo::SetLeader(leader);
    CGPartyInfo::m_onePersonParty = onePersonParty;
    CGPartyInfo::SetLfgSlot(lfgSlot);
    CGPartyInfo::m_lfgUnk = lfgUnk;
    CGPartyInfo::SetLfgRestricted((groupType >> 2) & 1);
    CGGameUI::UpdateThreatWarningFromCVar();
    Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBERS_CHANGED);

    if (joined) {
        SE3SoundKitProperties properties;
        properties.ResetToDefaults();
        SE3::PlaySoundKit("igPlayerInviteAccept", NULL, &properties, NULL, __FILE__, __LINE__, "OnGroupList - New Member Joining Party");

        if (!wasInGroup && leader == ClntObjMgrGetActivePlayer() && name[0]) {
            CGGameUI::DisplayError(ERR_JOINED_GROUP_S, name);
        }
    }

    if (!isRaid) {
        for (uint32_t j = 0; j < 4; j++) {
            WOWGUID old = oldMembers[j];

            if (!old) {
                continue;
            }

            uint32_t k = 0;

            while (k < memberCount && partyGuids[k] != old) {
                k++;
            }

            if (k != memberCount) {
                continue;
            }

            NameCache* oldName = g_nameDBCache.GetRecord(old.m_guid, WOWGUID(), NULL, NULL, false);

            if (oldName) {
                CGGameUI::DisplayError(ERR_LEFT_GROUP_S, oldName->m_name);
            }

            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(old, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                const WOWGUID& petGuid = unit->m_unit->m_unitFieldCharm.m_guid ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

                if (petGuid && !ClntObjMgrObjectPtr(petGuid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
                    CGGameUI::ClearTarget(petGuid, 1);
                }
            } else {
                CGGameUI::ClearTarget(old, 1);
            }
        }
    }

    if (memberCount) {
        uint8_t lootMethod;
        msg->Get(lootMethod);
        WOWGUID lootMaster;
        msg->Get(lootMaster.m_guid);
        uint8_t lootThreshold;
        msg->Get(lootThreshold);
        uint8_t dungeonDifficulty;
        msg->Get(dungeonDifficulty);
        uint8_t raidDifficulty;
        msg->Get(raidDifficulty);
        CGPartyInfo::m_playerAssignments = assignments;
        CGPartyInfo::m_playerRole = roles;
        CGPartyInfo::m_playerLFGFlags = lfgFlags;
        CGPartyInfo::SetLootMethod(lootMethod, lootMaster, lootThreshold, wasInGroup == 0);
        CGGameUI::SetDungeonDifficulty(dungeonDifficulty, 1, 1);
        CGGameUI::SetRaidDifficulty(raidDifficulty, 1, 1, 1);
    } else {
        CGPartyInfo::ResetLootMethod();
    }

    bool wasRaid = CGRaidInfo::m_numMembers != 0;

    if (isRaid) {
        raidGuids[memberCount] = ClntObjMgrGetActivePlayer();
        subgroups[memberCount] = subgroup;
        raidFlags[memberCount] = assignments;
        raidRoles[memberCount] = roles;
        online[memberCount] = 1;
        memberCount++;
        CGRaidInfo::UpdateRoster(raidGuids, subgroups, raidFlags, raidRoles, online, memberCount, isBattleground != 0, everyoneIsAssistant);
    } else {
        if (CGRaidInfo::m_numMembers || !CGPartyInfo::NumMembers()) {
            CGRaidInfo::ClearRoster();
        }

        if (!wasInGroup && memberCount) {
            UserClientRequestPartyJoinUpdates request;
            ClientServices::Send(&request);
        }
    }

    if (wasRaid != (CGRaidInfo::m_numMembers != 0) && ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CDataStore query;
        query.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_STATUS_MULTIPLE_QUERY));
        query.Finalize();
        ClientServices::Send2(&query);
    }

    if ((!wasInGroup && joined) || (!wasRaid && isRaid)) {
        CGChat::JoiningParty(isRaid, CGRaidInfo::IsPVPRaid() != 0);
        CGLookingForGroup::JoiningParty();
    }

    if (oldRoles != CGPartyInfo::m_playerRole) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ROLES_ASSIGNED, NULL);
    }

    CGActionBar::UpdateUsable();
    int32_t lost = static_cast<int32_t>(oldCount - partyCount);
    CGRaidInfo::RosterUpdateRefreshReadyCheck(lost > 0 ? lost : 0, oldLeader);
    return 1;
}

// OFFSET: 0x00A3AA30/0x1404BDD20
static bool OnDebugRuneRegen(ClientDebugRuneRegen* msg) {
    return true;
}

// OFFSET: 0x005705A0/0x1401D1660
void CGPlayer_C::PlayerInitializeMessageHandlers() {
    ClientGuildSendMaxDailyXP::s_handler = OnGuildSendMaxDailyXP;
    ClientGuildSendRankChange::s_handler = OnGuildSendRankChange;
    ClientBattlegroundInfoThrottled::s_handler = OnBattlegroundInfoThrottled;
    ClientGuildSendGuildXP::s_handler = OnGuildSendGuildXP;
    ClientGuildMemberRecipes::s_handler = OnGuildMemberRecipes;
    ClientGuildMembersWithRecipe::s_handler = OnGuildMembersWithRecipe;
    ClientGuildCommandResult::s_handler = OnGuildCommandResult;
    ClientCategoryCooldown::s_handler = OnCategoryCooldown;
    ClientRolePollBegin::s_handler = OnRolePollBegin;
    ClientGuildKnownRecipes::s_handler = OnGuildKnownRecipes;
    ClientRaidMarkersChanged::s_handler = OnRaidMarkersChanged;
    ClientQuestNpcQueryResponse::s_handler = OnQuestNpcQueryResponse;
    ClientRoleChangedInform::s_handler = OnRoleChangedInform;
    ClientRequestCemeteryListResponse::s_handler = OnRequestCemeteryListResponse;
    ClientPetitionAlreadySigned::s_handler = OnPetitionAlreadySigned;
    ClientSummonRaidMemberValidateFailed::s_handler = OnSummonRaidMemberValidateFailed;
    ClientStartTimer::s_handler = OnStartTimer;
    ClientSpellinterruptlog::s_handler = OnSpellInterruptLog;
    ClientArenaTeamCommandResult::s_handler = OnArenaTeamCommandResult;
    ClientXpGainAborted::s_handler = OnXpGainAborted;
    ClientSorStartExperienceIncomplete::s_handler = OnSorStartExperienceIncomplete;
    ClientDebugRuneRegen::s_handler = OnDebugRuneRegen;
    ClientChatIgnoredAccountMuted::s_handler = OnChatIgnoredAccountMuted;
    ClientServices::SetMessageHandler(SMSG_MOUNT_RESULT, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_DISMOUNT_RESULT, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_INVENTORY_CHANGE_FAILURE, Packet_SMSG_INVENTORY_CHANGE_FAILURE, NULL);
    ClientServices::SetMessageHandler(SMSG_OPEN_CONTAINER, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_ITEM_PUSH_RESULT, Packet_SMSG_ITEM_PUSH_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_BUY_FAILED, Packet_Group_10, NULL);
    ClientServices::SetMessageHandler(SMSG_BUY_SUCCEEDED, Packet_Group_10, NULL);
    ClientServices::SetMessageHandler(SMSG_SELL_ITEM, Packet_Group_10, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_RESPONSE, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_RELEASE, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_REMOVED, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_SLOT_CHANGED, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_MONEY_NOTIFY, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_ITEM_NOTIFY, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_CLEAR_MONEY, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_UNLEARNED_SPELLS, Packet_SMSG_REMOVED_SPELL, NULL);
    ClientServices::SetMessageHandler(SMSG_SEND_KNOWN_SPELLS, Packet_SMSG_INITIAL_SPELLS, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_CANCEL, Packet_SMSG_GROUP_CANCEL, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_DECLINE, Packet_SMSG_GROUP_DECLINE, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_UNINVITE, Packet_SMSG_GROUP_UNINVITE, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_SET_LEADER, Packet_SMSG_GROUP_SET_LEADER, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_DESTROYED, Packet_SMSG_GROUP_DESTROYED, NULL);
    ClientServices::SetMessageHandler(SMSG_PARTY_COMMAND_RESULT, Packet_SMSG_PARTY_COMMAND_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_LIST, Packet_SMSG_GROUP_LIST, NULL);
    ClientServices::SetMessageHandler(SMSG_REAL_GROUP_UPDATE, Packet_SMSG_REAL_GROUP_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_INVALID_QUEST, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_DETAILS, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_REQUEST_ITEMS, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_QUEST_FAILED, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_STATUS, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_GIVER_STATUS_MULTIPLE, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_LOG_FULL, Packet_Group_12, NULL);
    ClientServices::SetMessageHandler(SMSG_TRAINER_LIST, Packet_Group_13, NULL);
    ClientServices::SetMessageHandler(CMSG_QUERY_OBJECT_POSITION, Packet_Group_13, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_PROFICIENCY, Packet_SMSG_SET_PROFICIENCY, NULL);
    ClientServices::SetMessageHandler(SMSG_RESURRECT_REQUEST, Packet_SMSG_RESURRECT_REQUEST, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAYER_SKINNED, Packet_SMSG_PLAYER_SKINNED, NULL);
    ClientServices::SetMessageHandler(SMSG_INSPECT_RESULTS_UPDATE, Packet_SMSG_INSPECT, NULL);
    ClientServices::SetMessageHandler(SMSG_INSPECT_TALENT, Packet_SMSG_INSPECT_TALENT, NULL);
    ClientServices::SetMessageHandler(SMSG_READ_ITEM_RESULT_OK, Packet_Group_14, NULL);
    ClientServices::SetMessageHandler(SMSG_READ_ITEM_RESULT_FAILED, Packet_Group_14, NULL);
    ClientServices::SetMessageHandler(SMSG_CANCEL_COMBAT, Packet_SMSG_CANCEL_COMBAT, NULL);
    ClientServices::SetMessageHandler(SMSG_TAXI_NODE_STATUS, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_SHOW_TAXI_NODES, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_ACTIVATE_TAXI_REPLY, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_GUILD_DECLINE, Packet_SMSG_GUILD_DECLINE, NULL);
    ClientServices::SetMessageHandler(SMSG_GUILD_INVITE_CANCEL, Packet_SMSG_GUILD_INVITE_CANCEL, NULL);
    ClientServices::SetMessageHandler(SMSG_GUILD_EVENT, Packet_SMSG_GUILD_EVENT, NULL);
    ClientServices::SetMessageHandler(SMSG_GUILD_COMMAND_RESULT_2, Packet_SMSG_GUILD_COMMAND_RESULT_2, NULL);
    ClientServices::SetMessageHandler(MSG_SAVE_GUILD_EMBLEM, Packet_MSG_SAVE_GUILD_EMBLEM, NULL);
    ClientServices::SetMessageHandler(MSG_TABARDVENDOR_ACTIVATE, Packet_MSG_TABARDVENDOR_ACTIVATE, NULL);
    ClientServices::SetMessageHandler(SMSG_PETITION_SHOW_LIST, Packet_Group_15, NULL);
    ClientServices::SetMessageHandler(SMSG_PETITION_SHOW_SIGNATURES, Packet_Group_15, NULL);
    ClientServices::SetMessageHandler(SMSG_PETITION_SIGN_RESULTS, Packet_Group_15, NULL);
    ClientServices::SetMessageHandler(MSG_PETITION_DECLINE, Packet_Group_15, NULL);
    ClientServices::SetMessageHandler(SMSG_TURN_IN_PETITION_RESULT, Packet_Group_15, NULL);
    ClientServices::SetMessageHandler(MSG_PETITION_RENAME, Packet_Group_15, NULL);
    ClientServices::SetMessageHandler(SMSG_BIND_POINT_UPDATE, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_BINDZONEREPLY, Packet_SMSG_BINDZONEREPLY, NULL);
    ClientServices::SetMessageHandler(SMSG_EMOTE, Packet_SMSG_EMOTE, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAYER_BOUND, Packet_SMSG_PLAYER_BOUND, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAYERBINDERROR, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_NEW_TAXI_PATH, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_NAME_INVALID, Packet_SMSG_PET_NAME_INVALID, NULL);
    ClientServices::SetMessageHandler(SMSG_EXPLORATION_EXPERIENCE, Packet_SMSG_EXPLORATION_EXPERIENCE, NULL);
    ClientServices::SetMessageHandler(SMSG_PARTY_MEMBER_STATS, Packet_HandlePartyMemberStats, NULL);
    ClientServices::SetMessageHandler(SMSG_PARTY_MEMBER_STATS_FULL, Packet_HandlePartyMemberStats, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_FAILED, Packet_Group_17, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_FAILED_TIMER, Packet_Group_17, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_COMPLETE, Packet_Group_17, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_ADD_KILL, Packet_Group_17, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_UPDATE_ADD_PVP_CREDIT, Packet_Group_17, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_CONFIRM_ACCEPT, Packet_SMSG_QUEST_CONFIRM_ACCEPT, NULL);
    ClientServices::SetMessageHandler(SMSG_SHOW_BANK, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_BUY_BANK_SLOT_RESULT, Packet_SMSG_BUY_BANK_SLOT_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_LEVEL_UP_INFO, Packet_SMSG_LEVEL_UP_INFO, NULL);
    ClientServices::SetMessageHandler(MSG_MINIMAP_PING, Packet_MSG_MINIMAP_PING, NULL);
    ClientServices::SetMessageHandler(SMSG_TRIGGER_CINEMATIC, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_TRIGGER_MOVIE, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_ITEM_TIME_UPDATE, Packet_Group_18, NULL);
    ClientServices::SetMessageHandler(SMSG_ITEM_ENCHANT_TIME_UPDATE, Packet_Group_18, NULL);
    ClientServices::SetMessageHandler(MSG_RANDOM_ROLL, Packet_MSG_RANDOM_ROLL, NULL);
    ClientServices::SetMessageHandler(SMSG_FISH_NOT_HOOKED, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_FISH_ESCAPED, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_FORCEACTIONSHOW, Packet_SMSG_FORCEACTIONSHOW, NULL);
    ClientServices::SetMessageHandler(SMSG_GOD_MODE, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_PETGODMODE, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_IGNORE_DIMINISHING_RETURNS_CHEAT, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_IGNORE_REQUIREMENTS_CHEAT, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_CLEAR_FAR_SIGHT_IMMEDIATE, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_WRONG_FACTION, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_PLAYER_NOTFOUND, Packet_SMSG_CHAT_PLAYER_NOTFOUND, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_PLAYER_AMBIGUOUS, Packet_SMSG_CHAT_PLAYER_AMBIGUOUS, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_RESTRICTED, Packet_SMSG_CHAT_RESTRICTED, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_NOT_IN_PARTY, Packet_SMSG_CHAT_NOT_IN_PARTY, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_GET_SYSTEM_STATUS, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_SPIRIT_HEALER_CONFIRM, Packet_SMSG_SPIRIT_HEALER_CONFIRM, NULL);
    ClientServices::SetMessageHandler(CMSG_AUTOSTORE_GROUND_ITEM, Packet_MSG_TALENT_WIPE_CONFIRM, NULL);
    ClientServices::SetMessageHandler(SMSG_BINDER_CONFIRM, Packet_SMSG_BINDER_CONFIRM, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_EXECUTE_LOG, Packet_SMSG_SPELLLOGEXECUTE, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_MISS_LOG, Packet_SMSG_SPELLLOGMISS, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_DAMAGE_SHIELD, Packet_SMSG_SPELLDAMAGESHIELD, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_INSTAKILL_LOG, Packet_SMSG_SPELLINSTAKILLLOG, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_NON_MELEE_DAMAGE_LOG, Packet_SMSG_SPELLNONMELEEDAMAGELOG, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_HEAL_LOG, Packet_SMSG_SPELLHEALLOG, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_ENERGIZE_LOG, Packet_SMSG_SPELLENERGIZELOG, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_DISPELL_LOG, Packet_Group_19, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_STEAL_LOG, Packet_Group_19, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_BREAK_LOG, Packet_Group_19, NULL);
    ClientServices::SetMessageHandler(SMSG_RESURRECT_FAILED, Packet_SMSG_RESURRECT_FAILED, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_OR_DAMAGE_IMMUNE, Packet_SMSG_SPELLORDAMAGE_IMMUNE, NULL);
    ClientServices::SetMessageHandler(MSG_QUEST_PUSH_RESULT, Packet_MSG_QUEST_PUSH_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_DAMAGE_CALC_LOG, Packet_SMSG_DAMAGE_CALC_LOG, NULL);
    ClientServices::SetMessageHandler(SMSG_RAID_GROUP_ONLY, Packet_SMSG_RAID_GROUP_ONLY, NULL);
    ClientServices::SetMessageHandler(SMSG_INSTANCE_LOCK_WARNING_QUERY, Packet_SMSG_INSTANCE_LOCK_WARNING_QUERY, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_START_ROLL, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_ALL_PASSED, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_ROLL_WON, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_ROLL, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_MASTER_LIST, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_TEST_DROP_RATE_RESULT, PH_LOOT_OPCODES, NULL);
    ClientServices::SetMessageHandler(SMSG_SUMMON_REQUEST, Packet_SMSG_SUMMON_REQUEST, NULL);
    ClientServices::SetMessageHandler(SMSG_SUMMON_CANCEL, OnSummonCancel, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAY_TIME_WARNING, Packet_SMSG_PLAY_TIME_WARNING, NULL);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_INVITE, Packet_SMSG_ARENA_TEAM_INVITE, NULL);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_EVENT, Packet_SMSG_ARENA_TEAM_EVENT, NULL);
    ClientServices::SetMessageHandler(SMSG_ARENA_ERROR, Packet_SMSG_ARENA_ERROR, NULL);
    ClientServices::SetMessageHandler(SMSG_DEATH_RELEASE_LOC, Packet_SMSG_DEATH_RELEASE_LOC, NULL);
    ClientServices::SetMessageHandler(SMSG_FORCED_DEATH_UPDATE, Packet_SMSG_FORCED_DEATH_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_VOICE_SESSION_ROSTER_UPDATE, Packet_SMSG_VOICE_SESSION_ROSTER_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_VOICE_SESSION_LEAVE, Packet_SMSG_VOICE_SESSION_LEAVE, NULL);
    ClientServices::SetMessageHandler(SMSG_VOICE_SET_TALKER_MUTED, Packet_SMSG_VOICE_SET_TALKER_MUTED, NULL);
    ClientServices::SetMessageHandler(SMSG_VOICE_PARENTAL_CONTROLS, Packet_SMSG_VOICE_PARENTAL_CONTROLS, NULL);
    ClientServices::SetMessageHandler(SMSG_CROSSED_INEBRIATION_THRESHOLD, Packet_SMSG_CROSSED_INEBRIATION_THRESHOLD, NULL);
    ClientServices::SetMessageHandler(SMSG_COMPLAINT_RESULT, Packet_SMSG_COMPLAINT_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_AVAILABLE_VOICE_CHANNEL, Packet_SMSG_AVAILABLE_VOICE_CHANNEL, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_COMMAND_RESULT, Packet_SMSG_CALENDAR_COMMAND_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_REPORT_PVP_AFK_RESULT, Packet_SMSG_REPORT_PVP_AFK_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_GROUP_ACTION_THROTTLED, Packet_SMSG_GROUPACTION_THROTTLED, NULL);
    ClientServices::SetMessageHandler(SMSG_SEND_UNLEARN_SPELLS, Packet_SMSG_SEND_UNLEARN_SPELLS, NULL);
    ClientServices::SetMessageHandler(SMSG_CONVERT_RUNE, Packet_SMSG_CONVERT_RUNE, NULL);
    ClientServices::SetMessageHandler(SMSG_RESYNC_RUNES, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_ADD_RUNE_POWER, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_TALENT_DATA, Packet_SMSG_TALENTS_INFO, NULL);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_CHANGE_FAILED_QUEUED, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_POI_QUERY_RESPONSE, Packet_SMSG_QUEST_POI_QUERY_RESPONSE, NULL);
    ClientServices::SetMessageHandler(SMSG_TOGGLE_XP_GAIN, Packet_Group_9, NULL);
    ClientServices::SetMessageHandler(CMSG_GUILD_SET_OFFICER_NOTE, Packet_SMSG_DEBUG_SERVER_GEO, NULL);
    ClientServices::SetMessageHandler(SMSG_QUEST_FORCE_REMOVED, Packet_SMSG_QUEST_FORCE_REMOVE, NULL);
    ClientServices::SetMessageHandler(SMSG_CAMERA_SHAKE, Packet_SMSG_CAMERA_SHAKE, NULL);
    ClientServices::SetMessageHandler(SMSG_SOCKET_GEMS, Packet_Group_18, NULL);
    ClientServices::SetMessageHandler(SMSG_TALENTS_ERROR, Packet_SMSG_TALENTS_ERROR, NULL);
    ClientItemRefundInfoResponse::s_handler = OnItemRefundInfoResponse;
    ClientItemExpirePurchaseRefund::s_handler = OnClientItemExpirePurchaseRefund;
    ClientItemRefundResult::s_handler = OnItemRefundResult;
    ClientQuestGiverQuestComplete::s_handler = OnClientQuestGiverQuestComplete;
    ClientFeatureSystemStatus::s_handler = OnFeatureSystemStatus;
    ClientSupercededSpells::s_handler = OnSupercededSpells;
    ClientLearnedSpell::s_handler = OnLearnedSpell;
    ClientUpdateActionButtons::s_handler = OnUpdateActionButtons;
}

// OFFSET: 0x00569380/0x1401C8570
void CGPlayer_C::CombatModeEnter(const WOWGUID& target, int32_t queueRanged, int32_t silent, int32_t a4, int32_t a5) {
    if (!CGGameUI::CanPerformAction(1)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    int32_t rangedSpell = CGSpellBook::m_autoRangedSpell;

    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && !this->MountedCombatAllowed()) {
        float minRange;
        float maxRange;

        if (rangedSpell) {
            Spell_C_GetMinMaxRange(this, rangedSpell, &minRange, &maxRange, unit);
        } else {
            maxRange = this->CGUnit::GetMeleeRange(unit);
        }

        C3Vector position = unit->GetPosition();

        if (maxRange * maxRange < this->GetDistanceToPos(position)) {
            if (!silent) {
                CGGameUI::DisplayError(ERR_OUT_OF_RANGE);
            }

            return;
        }

        this->Dismount();
    }

    if ((!(this->m_combatModeFlags & 0x2) && !this->CGUnit_C::IsInMelee()) || target != this->m_combatTarget) {
        this->m_combatModeTime = FrameTime::s_curTimeMs;
    }

    this->m_combatTarget = target;

    if (!rangedSpell) {
        this->MeleeModeEnter(target, 0, 0);
        return;
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || (this->m_unit->m_unitFieldFlags & 0x20000) || !CGGameUI::m_hasControl) {
        return;
    }

    if (this->m_player->m_playerFlags & 0x1000000) {
        CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
        return;
    }

    if (!this->CanAttackNow(unit)) {
        if (this->IsInMelee()) {
            this->SetCombatMode(true);
        }

        return;
    }

    EventRegister(EVENT_ID_IDLE, CGPlayer_C::AutoCombatModeEventHandler);
    this->m_combatModeFlags |= 0x2;
    CGGameUI::UpdateCombatMode();

    if (!this->IsInMeleeRange(unit)) {
        if (queueRanged) {
            this->m_combatModeFlags |= 0x8;
        }

        this->m_combatModeFlags |= 0x4;
    }
}

// OFFSET: 0x00554970/0x1401B11C0
int32_t GetModalNextSpell() {
    return CGSpellBook::m_autoRangedSpell;
}

// OFFSET: 0x005638C0
int32_t CGPlayer_C::IsXRayVisionActive() {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    return (player->m_auraEffectMask[0x22] >> 1) & 1;
}

// PROVEN: d05e3c2a IDENTICAL/NONE
// OFFSET: 0x00554440/0x1401B0480
int32_t CGPlayer_C::GetNewContinentID() {
    return ClntObjMgrGetMapID();
}

// OFFSET: 0x0044EDD0/0x14005D0C0
uint32_t CGPlayer_C::GetNextLevelXP() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return this->m_player->m_playerNextLevelXp;
    }

    return 0;
}

// OFFSET: 0x00551F70
int32_t Packet_SMSG_REAL_GROUP_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t flags;
    uint32_t count;
    WOWGUID leader;
    msg->Get(flags);
    msg->Get(count);
    msg->Get(leader.m_guid);

    if (flags & 0x2) {
        if (count) {
            CGRaidInfo::UpdateRealStats(count + 1, leader);
        } else {
            CGRaidInfo::UpdateRealStats(0, leader);
        }
    } else {
        if (CGRaidInfo::m_realNumMembers) {
            CGRaidInfo::UpdateRealStats(0, leader);
        }

        CGPartyInfo::UpdateRealStats(count, leader);
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBERS_CHANGED);
    return 1;
}

// OFFSET: 0x00552090
int32_t Packet_MSG_TABARDVENDOR_ACTIVATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID vendor;
    msg->Get(vendor.m_guid);
    CGTabardCreationFrame::Open(vendor);
    return 1;
}

// OFFSET: 0x00555AD0
int32_t Packet_SMSG_ARENA_TEAM_EVENT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t event = 0;
    msg->Get(event);
    uint8_t count;
    msg->Get(count);
    char strings[3][256];

    for (uint8_t i = 0; i < count; i++) {
        msg->GetString(strings[i], sizeof(strings[i]));
    }

    UIErrors error;
    WOWGUID guid;

    switch (event) {
        case 3: {
            if (SStrCmpUTF8I(ClientServices::GetCharacterName(), strings[0], STORM_MAX_STR)) {
                error = ERR_ARENA_TEAM_JOIN_SS;
            } else {
                error = ERR_ARENA_TEAM_YOU_JOIN_S;
                SStrCopy(strings[0], strings[1], sizeof(strings[0]));
                count = 1;
            }

            msg->Get(guid.m_guid);
            break;
        }

        case 4: {
            error = ERR_ARENA_TEAM_LEAVE_SS;
            msg->Get(guid.m_guid);
            break;
        }

        case 5: {
            error = ERR_ARENA_TEAM_REMOVE_SSS;
            break;
        }

        case 6: {
            error = ERR_ARENA_TEAM_LEADER_IS_SS;
            break;
        }

        case 7: {
            error = ERR_ARENA_TEAM_LEADER_CHANGED_SSS;
            break;
        }

        case 8: {
            error = ERR_ARENA_TEAM_DISBANDED_S;
            break;
        }

        default: {
            error = ERR_GUILD_INTERNAL;
            break;
        }
    }

    switch (count) {
        case 1: {
            CGGameUI::DisplayError(error, strings[0]);
            break;
        }

        case 2: {
            CGGameUI::DisplayError(error, strings[0], strings[1]);
            break;
        }

        case 3: {
            CGGameUI::DisplayError(error, strings[0], strings[1], strings[2]);
            break;
        }

        default: {
            CGGameUI::DisplayError(error);
            break;
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        CGArenaTeamInfo::UpdateRoster(i, 1);
    }

    return 1;
}

// OFFSET: 0x005547B0
void CGPlayer_C::OnLootGameObject(const WOWGUID& guid) {
    this->m_lootTarget = guid;

    if (this->ShouldKneelForLoot()) {
        this->PlayLootStartAnimKit();
    }
}

// OFFSET: 0x00552480
int32_t Packet_SMSG_ARENA_TEAM_INVITE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char inviter[48];
    char team[96];
    msg->GetString(inviter, sizeof(inviter));
    msg->GetString(team, sizeof(team));
    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_INVITE_REQUEST, "%s%s", inviter, team);
    CGGameUI::DisplayError(ERR_INVITED_TO_GUILD_SSS, inviter, inviter, team);
    return 1;
}

// OFFSET: 0x005527B0
int32_t Packet_SMSG_ARENA_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t error;
    msg->Get(error);

    if (error == 0) {
        uint8_t type;
        msg->Get(type);
        CGGameUI::DisplayError(ERR_ARENA_NO_TEAM_II, type, type);
    } else if (error == 1) {
        CGGameUI::DisplayError(ERR_ARENA_EXPIRED_CAIS);
    } else if (error == 2) {
        CGGameUI::DisplayError(ERR_LFG_CANT_USE_BATTLEGROUND);
    }

    return 1;
}

// OFFSET: 0x00551900
int32_t Packet_SMSG_GROUP_SET_LEADER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[48];
    msg->GetString(name, sizeof(name));

    if (SStrCmpUTF8I(ClientServices::GetCharacterName(), name, STORM_MAX_STR)) {
        if (CGPartyInfo::m_lfgRestricted) {
            CGGameUI::DisplayError(ERR_NEW_GUIDE_S, name);
        } else {
            CGGameUI::DisplayError(ERR_NEW_LEADER_S, name);
        }
    } else {
        if (CGPartyInfo::m_lfgRestricted) {
            CGGameUI::DisplayError(ERR_NEW_GUIDE_YOU);
        } else {
            CGGameUI::DisplayError(ERR_NEW_LEADER_YOU);
        }
    }

    return 1;
}

// OFFSET: 0x00554840
int32_t CGPlayer_C::OnPetitionDecline(CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        CGGameUI::DisplayError(ERR_PETITION_DECLINED_S, name->m_name);
    }

    return 1;
}

// OFFSET: 0x005548A0
int32_t __stdcall CGPlayer_C::OnTurnInPetitionResults(CDataStore* msg) {
    uint32_t result;
    msg->Get(result);

    switch (result) {
        case 0: {
            FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_REGISTRAR_CLOSED, NULL);
            break;
        }

        case 2: {
            CGGameUI::DisplayError(ERR_PETITION_IN_GUILD);
            break;
        }

        case 4: {
            CGGameUI::DisplayError(ERR_PETITION_NOT_ENOUGH_SIGNATURES);
            break;
        }

        case 11: {
            CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
            break;
        }

        default: {
            ConsoleWrite("Petition error", DEFAULT_COLOR);
            break;
        }
    }

    return 1;
}

// OFFSET: 0x00561E90/0x1401BFEE0
bool CGPlayer_C::CanTrackObject(CGObject_C* object) {
    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(object);

    if (this->IsDeadOrGhost() || (gameObject->m_gameObject->m_typeID != 25 && !gameObject->CanUse())) {
        return false;
    }

    uint32_t trackResources = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerTrackResources : 0;
    LockRec* lock = gameObject->GetLockRec();

    if (!lock) {
        return false;
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (lock->m_type[i] == 2 && ((1 << (static_cast<uint8_t>(lock->m_index[i]) - 1)) & trackResources)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00567D40
int32_t Packet_SMSG_SEND_UNLEARN_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (!msg) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (msgId != SMSG_SEND_UNLEARN_SPELLS) {
        return 1;
    }

    uint32_t count;
    msg->Get(count);
    CGSpellBook::m_unlearnedSpells.SetCount(count);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        uint32_t spellId;
        msg->Get(spellId);
        CGSpellBook::m_unlearnedSpells.m_data[i] = spellId;
    }

    return 1;
}

// OFFSET: 0x0055DA80/0x1401BAFA0
void __stdcall CGPlayer_C::ReportPVPAFK(WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REPORT_PVP_PLAYER_AFK));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        char buffer[3000];
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("REPORT_PVP_AFK_SENT", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name));
        CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
    }
}

// PROVEN: 63b86c08 IDENTICAL/NONE
// OFFSET: 0x0055DB90/0x1401BB0F0
bool __stdcall CGPlayer_C::IsRAFPlayer(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        return (player->m_unit->m_unitDynamicFlags >> 6) & 1;
    }

    PartyMemberState* stats = CGPartyInfo::GetRemoteStats(guid);

    if (!stats) {
        stats = CGRaidInfo::GetRemoteStats(guid);
    }

    if (stats && (stats->m_flags & 0x100)) {
        return true;
    }

    if (FriendList::s_friendList) {
        FriendInfo* info = FriendList::s_friendList->GetFriend(guid);

        if (info) {
            return (info->m_flags >> 3) & 1;
        }
    }

    return false;
}

// OFFSET: 0x0056E170/0x1401CED60
const CGBag_C* CGPlayer_C::GetBag() const {
    return &this->m_bag;
}

// OFFSET: 0x0056E170/0x1401CED60
CGBag_C* CGPlayer_C::GetBag() {
    return &this->m_bag;
}

// OFFSET: 0x0055E350/0x1401BB8F0
int32_t Player_C_ZoneUpdateHandler(const void* data, void* param) {
    if (++s_zoneUpdateCounter >= 10) {
        s_zoneUpdateCounter -= 10;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit || (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) != NULL) {
            AreaListRegisterLocation(ClntObjMgrGetMapID(), unit->m_worldObject);
        }
    }

    return 1;
}

// OFFSET: 0x00561870
bool CGPlayer_C::MeetsItemSetSkill(const ItemSetRec* set) const {
    if (set->m_requiredSkill) {
        int32_t index = this->GetSkillIndex(set->m_requiredSkill);

        if (index < 0 || static_cast<int32_t>(this->GetSkillValue(index)) < set->m_requiredSkillRank) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x00563610/0x1401C1380
InspectItem* CGPlayer_C::GetInspectedItem(int32_t slot) {
    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        if (slot < 0) {
            return NULL;
        }

        if (static_cast<uint32_t>(slot) < 19) {
            return &s_inspectData->m_items[slot];
        }
    }

    if (slot < 0 || static_cast<uint32_t>(slot) >= 19) {
        return NULL;
    }

    VisibleItem_C* visible = &this->m_player->m_playerVisibleItems[slot];

    if (!visible) {
        return NULL;
    }

    memset(&s_emptyInspectItem, 0, sizeof(s_emptyInspectItem));
    s_emptyInspectItem.m_itemId = abs(visible->m_itemId);
    s_emptyInspectItem.m_enchants[0] = visible->m_enchantment[0];
    s_emptyInspectItem.m_enchants[1] = visible->m_enchantment[1];

    if (visible->m_itemId <= 0) {
        s_emptyInspectItem.m_flags |= 1;
    }

    return &s_emptyInspectItem;
}

// PROVEN: 49ebd1d0 IDENTICAL/NONE
// OFFSET: 0x00563780/0x1401C1500
static void CountGemsItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->CountEquippedGems();
    }
}

// OFFSET: 0x0056D310
void CGPlayer_C::CountEquippedGems() {
    this->m_gemCounts[0] = 0;
    this->m_gemCounts[1] = 0;
    this->m_gemCounts[2] = 0;
    this->m_gemCounts[3] = 0;
    this->unk_21D4[0] = 0;
    this->unk_21D4[1] = 0;

    for (uint32_t slot = 0; slot <= 18; slot++) {
        CGItem_C* item = NULL;
        InspectItem* inspect = NULL;

        if (ClntObjMgrGetActivePlayer() == this->m_obj->m_guid) {
            item = const_cast<CGItem_C*>(this->m_bag.GetItemPointer(slot));

            if (!item) {
                continue;
            }

            uint32_t flags = item->m_item->m_itemFieldFlags;

            if (!(flags & 0x8) && item->m_item->m_itemFieldMaxdurability && !item->m_item->m_itemFieldDurability) {
                continue;
            }

            if (flags & 0x10) {
                continue;
            }
        } else {
            inspect = this->GetInspectedItem(slot);

            if (!inspect || !(~inspect->m_flags & 1)) {
                continue;
            }
        }

        for (uint32_t socket = 2; socket <= 4; socket++) {
            int32_t enchantId;

            if (ClntObjMgrGetActivePlayer() == this->m_obj->m_guid) {
                enchantId = (item->GetItemStaticFlags(0) & 0x2000) ? 0 : item->m_item->m_itemFieldEnchantment[socket].m_id;
            } else if (socket <= 14) {
                enchantId = inspect->m_enchants[socket];
            } else {
                enchantId = 0;
            }

            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchantId);

            if (!enchant || !enchant->m_srcItemID) {
                continue;
            }

            ItemRecSparse* gem = g_itemSparseDB.GetRecord(enchant->m_srcItemID, WOWGUID(), CountGemsItemCacheCallback, NULL, false);

            if (!gem) {
                continue;
            }

            GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(gem->m_gemProperties);

            if (!properties || !properties->m_type) {
                continue;
            }

            if (properties->m_type & 0x1) {
                this->m_gemCounts[0]++;
            }

            if (properties->m_type & 0x2) {
                this->m_gemCounts[1]++;
            }

            if (properties->m_type & 0x4) {
                this->m_gemCounts[2]++;
            }

            if (properties->m_type & 0x8) {
                this->m_gemCounts[3]++;
            }
        }
    }
}

// OFFSET: 0x005572A0/0x1401B40B0
void __stdcall CGPlayer_C::TalkToGossip(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GOSSIP_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557320/0x1401B4150
void __stdcall CGPlayer_C::TalkToQuestUnit(const WOWGUID& guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_HELLO));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005573A0/0x1401B41F0
void __stdcall CGPlayer_C::TalkToTrainer(const WOWGUID& guid) {
    s_lastTrainerRequest = guid;
    WOWGUID none = { 0 };
    CGClassTrainer::SetTrainer(none, 0, 0);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TRAINER_LIST));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005575C0/0x1401B4450
void CGPlayer_C::TalkToBinder(const WOWGUID& guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid ? guid : s_binder, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (dy * dy + dx * dx + dz * dz > range * range) {
        return;
    }

    if (guid) {
        s_binder = guid;
        AreaTableRec* area = CGGameUI::GetCurrentSubzoneRec();
        const char* name = area ? area->m_areaName : FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE);
        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_BINDER, "%s", name);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BINDER_ACTIVATE));
    msg.Put(s_binder.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00557890/0x1401B4780
void __stdcall CGPlayer_C::TalkToGuildBanker(const WOWGUID& guid) {
    if (CGGuildBankInfo::m_banker && CGGuildBankInfo::m_banker == guid) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_ACTIVATE));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(CGGuildBankInfo::m_tabInfo[0].m_contentsDirty != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGuildBankInfo::m_pendingBanker = guid;
}

// OFFSET: 0x005579D0/0x1401B48F0
void CGPlayer_C::TalkToTabardVendor(const WOWGUID& guid) {
    if (CGTabardCreationFrame::m_vendor == guid) {
        return;
    }

    if (!this->CanEquipItemsInThisForm()) {
        CGGameUI::DisplayError(ERR_EMBLEMERROR_NOTABARDGEOSET);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_TABARDVENDOR_ACTIVATE));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00560350/0x1401BDF00
void __stdcall CGPlayer_C::ShopFromMerchant(const WOWGUID& guid) {
    WOWGUID item = CGGameUI::GetCursorItem();

    if (item) {
        CGPlayer_C::SellItem(guid, item, 0);
        WOWGUID none = { 0 };
        CGGameUI::SetCursorItem(none, none, 0, 0, 0, 1);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LIST_INVENTORY));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00560430/0x1401BE010
void CGPlayer_C::TalkToSpiritHealer(const WOWGUID& guid) {
    if (!(this->m_player->m_playerFlags & 0x10)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid ? guid : s_spiritHealer, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dx = unitPosition.x - position.x;
    float dy = unitPosition.y - position.y;
    float dz = unitPosition.z - position.z;
    float range = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (dy * dy + dx * dx + dz * dz > range * range) {
        return;
    }

    if (guid) {
        s_spiritHealer = guid;
        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_XP_LOSS, "%d", static_cast<int32_t>(static_cast<int32_t>(this->GetNextLevelXP()) * 0.05f));
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SPIRIT_HEALER_ACTIVATE));
    msg.Put(s_spiritHealer.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005605B0
void CGPlayer_C::TalkToSpiritGuide(const WOWGUID& guid) {
    if (this->m_player->m_playerFlags & 0x10) {
        WOWGUID none = { 0 };
        CGGameUI::SetAreaSpiritHealer(none);
        CGGameUI::SetAreaSpiritHealer(guid);
    }
}

// OFFSET: 0x0055CBF0/0x1401B9F70
void CGPlayer_C::HideHelm() {
    if (this->CanEquipItemsInThisForm() && this->m_characterComponent) {
        this->m_characterComponent->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(0));
    }

    if (!(this->m_player->m_playerFlags & 0x400) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SHOWING_HELM));
        msg.Put(static_cast<uint8_t>(0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    this->ApplyComponentItemsFromEffects();
}

// OFFSET: 0x0055CCB0/0x1401BA060
void CGPlayer_C::HideCloak() {
    if (this->CanEquipItemsInThisForm() && this->m_characterComponent) {
        this->m_characterComponent->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(14));
    }

    if (!(this->m_player->m_playerFlags & 0x800) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SHOWING_CLOAK));
        msg.Put(static_cast<uint8_t>(0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// OFFSET: 0x00562720
int32_t CGPlayer_C::OnPetitionRename(CDataStore* msg) {
    WOWGUID guid;
    char name[96];
    msg->Get(guid.m_guid);
    msg->GetString(name, sizeof(name));

    if (!name[0]) {
        return 1;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || !(item->GetItemStaticFlags(0) & 0x2000) || !item->m_item->m_itemFieldEnchantment[0].m_id) {
        return 1;
    }

    WOWGUID none = WOWGUID();
    int32_t id = (item->GetItemStaticFlags(0) & 0x2000) ? item->m_item->m_itemFieldEnchantment[0].m_id : 0;
    CGPetition* petition = g_petitionCache.GetRecord(id, none, NULL, NULL, false);

    if (petition) {
        SStrCopy(petition->m_title, name, sizeof(petition->m_title));
        FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_SHOW, NULL);
    }

    return 1;
}

// OFFSET: 0x00562820
void GuildCharterTurnInCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->TurnInGuildCharter();
    }
}

// OFFSET: 0x00562860
void CGPlayer_C::TurnInGuildCharter() {
    for (uint32_t i = 23; i <= 38; i++) {
        WOWGUID guid = i < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[i] : WOWGUID();

        if (!guid) {
            continue;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item || !(item->GetItemStaticFlags(0) & 0x2000)) {
            continue;
        }

        int32_t id = (item->GetItemStaticFlags(0) & 0x2000) ? item->m_item->m_itemFieldEnchantment[0].m_id : 0;
        CGPetition* petition = g_petitionCache.GetRecord(id, guid, GuildCharterTurnInCallback, NULL, false);

        if (!petition) {
            return;
        }

        if (!petition->m_staticType) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_TURN_IN_PETITION));
            msg.Put(item->m_obj->m_guid.m_guid);
            msg.Finalize();
            ClientServices::Send2(&msg);
            return;
        }
    }

    for (uint32_t i = 19; i <= 22; i++) {
        WOWGUID bagGuid = i < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[i] : WOWGUID();
        CGItem_C* container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(bagGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!container) {
            continue;
        }

        for (uint32_t j = 0; j < static_cast<uint32_t>(container->GetBag()->m_numSlots); j++) {
            CGBag_C* bag = container->GetBag();
            WOWGUID guid = j < static_cast<uint32_t>(bag->m_numSlots) ? bag->m_slots[j] : WOWGUID();

            if (!guid) {
                continue;
            }

            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (!item || !(item->GetItemStaticFlags(0) & 0x2000)) {
                continue;
            }

            int32_t id = (item->GetItemStaticFlags(0) & 0x2000) ? item->m_item->m_itemFieldEnchantment[0].m_id : 0;
            CGPetition* petition = g_petitionCache.GetRecord(id, guid, GuildCharterTurnInCallback, NULL, false);

            if (!petition) {
                return;
            }

            if (!petition->m_staticType) {
                CDataStore msg;
                msg.Put(static_cast<uint32_t>(CMSG_TURN_IN_PETITION));
                msg.Put(item->m_obj->m_guid.m_guid);
                msg.Finalize();
                ClientServices::Send2(&msg);
                return;
            }
        }
    }

    CGGameUI::DisplayError(ERR_NO_GUILD_CHARTER);
}

// OFFSET: 0x00562D00/0x1401C0AD0
void CGPlayer_C::HandleOnRightClick(CGUnit_C* unit, bool autoInteract) {
    if (this->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (!form || !(form->m_flags & 0x8)) {
            if (!this->CanAutoUnshift()) {
                CGGameUI::DisplayError(ERR_CANT_INTERACT_SHAPESHIFTED);
                return;
            }

            this->CancelShapeshift();
        }
    }

    C3Vector unitPosition = unit->GetPosition();
    C3Vector position = this->GetPosition();
    float dx = position.x - unitPosition.x;
    float dy = position.y - unitPosition.y;
    float dz = position.z - unitPosition.z;
    float distanceSq = dz * dz + dy * dy + dx * dx;

    if (autoInteract) {
        float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;

        if (reach * reach < distanceSq) {
            WOWGUID guid = unit->m_obj->m_guid;

            if (this->SetTrackingTarget(guid, 5)) {
                return;
            }
        }
    }

    float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (reach * reach < distanceSq) {
        if (!autoInteract) {
            CGGameUI::DisplayError(ERR_TOO_FAR_TO_INTERACT);
        }

        return;
    }

    if (unit->m_obj->m_type == (TYPEMASK_OBJECT | TYPEMASK_UNIT)) {
        CreatureStats_C* stats = unit->m_creatureCacheEntry;

        if (!stats) {
            return;
        }

        if (stats->m_expansion > ClientServices::Connection()->m_accountExpansion) {
            char token[64];
            SStrPrintf(token, sizeof(token), "EXPANSION_NAME%d", stats->m_expansion);
            CGGameUI::DisplayError(ERR_REQUIRES_EXPANSION_S, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
            return;
        }
    }

    if (unit->m_obj->m_guid == CGGameUI::m_interactTarget) {
        return;
    }

    if (unit->m_questStatus == 1) {
        CGTutorial::TriggerTutorial(0x2A);
    }

    WOWGUID guid = unit->m_obj->m_guid;

    if (unit->m_interactIconType == 2) {
        CGPlayer_C::EnableTaxiNode(guid);

        if (!CGUnit_C::UnitCanGossip(unit)) {
            return;
        }
    }

    if (unit->m_unit->m_unitNpcFlags & 0x1) {
        CGPlayer_C::TalkToGossip(guid);
        this->RequestTalkEmote(0);
        return;
    }

    if (CGUnit_C::UnitCanGossip(unit)) {
        CGPlayer_C::TalkToQuestUnit(guid);
        this->RequestTalkEmote(0);
        return;
    }

    uint32_t npcFlags = unit->m_unit->m_unitNpcFlags;

    if (npcFlags & 0x2000) {
        if (unit->m_interactIconType == 3) {
            CGGameUI::DisplayError(ERR_TAXINOTELIGIBLE);
        } else {
            CGPlayer_C::QueryTaxiNodes(guid);
        }

        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x80) {
        CGPlayer_C::ShopFromMerchant(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x10) {
        CGPlayer_C::TalkToTrainer(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x4000) {
        this->TalkToSpiritHealer(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x8000) {
        this->TalkToSpiritGuide(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x10000) {
        this->TalkToBinder(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x20000) {
        CGPlayer_C::TalkToBanker(guid);
        this->RequestTalkEmote(0);
    } else if (unit->IsGuildMaster()) {
        CGPlayer_C::TalkToPetitionVendor(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x40000) {
        CGPlayer_C::TalkToPetitionVendor(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x80000) {
        this->TalkToTabardVendor(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x100000) {
        CGPlayer_C::TalkToBattleMaster(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x200000) {
        CGPlayer_C::TalkToAuctioneer(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x400000) {
        CGPlayer_C::TalkToStableMaster(guid);
        this->RequestTalkEmote(0);
    } else if (npcFlags & 0x800000) {
        CGPlayer_C::TalkToGuildBanker(guid);
        this->RequestTalkEmote(0);
    } else if (unit->IsInPartyOrRaid()) {
        this->SetAutoLoot(CGGameUI::IsAutoLooting());
        CGPlayer_C::TalkToSpellClick(guid);
    } else if (unit->CanBeSpellClickedByLocalPlayer(this)) {
        CGPlayer_C::TalkToRideVehicle(guid);
    } else if (unit->m_unit->m_unitNpcFlags & 0x4000000) {
        CGPlayer_C::TalkToMailbox(unit->m_obj->m_guid);
    } else if (unit->m_unit->m_unitNpcFlags & 0x8000000) {
        CGReforge::SetForgeMaster(guid);
    } else if (unit->m_unit->m_unitNpcFlags & 0x20000000) {
        CGVoidStorageInfo::SetNPC(guid);
    } else if (unit->m_unit->m_unitNpcFlags & 0x10000000) {
        CGTransmogrify::SetTransmogrifier(guid);
    }
}

// OFFSET: 0x00556B80
void CGPlayer_C::RequestAllItemPurchaseData() {
    for (int32_t i = 0; i <= 18; i++) {
        if (static_cast<uint32_t>(i) >= static_cast<uint32_t>(this->m_bag.m_numSlots) || !this->m_bag.m_slots[i]) {
            continue;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(this->m_bag.m_slots[i], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && !item->m_itemPurchaseData && !(item->m_itemFlags & 0x4) && item->GetStats() && (item->GetStats()->m_flags[0] & 0x1000)) {
            item->RequestItemPurchaseData();
        }
    }

    for (int32_t i = 23; i <= 38; i++) {
        if (static_cast<uint32_t>(i) >= static_cast<uint32_t>(this->m_bag.m_numSlots) || !this->m_bag.m_slots[i]) {
            continue;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(this->m_bag.m_slots[i], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && !item->m_itemPurchaseData && !(item->m_itemFlags & 0x4) && item->GetStats() && (item->GetStats()->m_flags[0] & 0x1000)) {
            item->RequestItemPurchaseData();
        }
    }

    for (int32_t i = 19; i <= 22; i++) {
        WOWGUID guid = static_cast<uint32_t>(i) < static_cast<uint32_t>(this->m_bag.m_numSlots) ? this->m_bag.m_slots[i] : WOWGUID();
        CGItem_C* container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!container) {
            continue;
        }

        for (uint32_t j = 0; j < static_cast<uint32_t>(container->GetBag()->m_numSlots); j++) {
            CGBag_C* bag = container->GetBag();

            if (j >= static_cast<uint32_t>(bag->m_numSlots) || !bag->m_slots[j]) {
                continue;
            }

            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(bag->m_slots[j], TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && !item->m_itemPurchaseData && !(item->m_itemFlags & 0x4) && item->GetStats() && (item->GetStats()->m_flags[0] & 0x1000)) {
                item->RequestItemPurchaseData();
            }
        }
    }
}

// OFFSET: 0x00553220
static void ReadQuestRewards(CDataStore* msg, QuestRewards_C& rewards) {
    msg->Get(rewards.m_numChoiceItems);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_choiceItemIDs), 6);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_choiceItemCounts), 6);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_choiceItemDisplayIDs), 6);
    msg->Get(rewards.m_numRewardItems);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_rewardItemIDs), 4);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_rewardItemCounts), 4);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_rewardItemDisplayIDs), 4);
    msg->Get(rewards.m_money);
    msg->Get(rewards.m_xp);
    msg->Get(rewards.m_title);
    msg->Get(rewards.m_honor);
    msg->Get(rewards.m_honorMultiplier);
    msg->Get(rewards.m_talents);
    msg->Get(rewards.m_arenaPoints);
    msg->Get(rewards.m_factionFlags);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_factionIDs), 5);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_factionValues), 5);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_factionOverrides), 5);
    msg->Get(rewards.m_spell);
    msg->Get(rewards.m_spellCast);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_currencyIDs), 4);
    msg->GetArray(reinterpret_cast<uint32_t*>(rewards.m_currencyCounts), 4);
    msg->Get(rewards.m_skillLine);
    msg->Get(rewards.m_skillPoints);
}

// OFFSET: 0x00555F90
int32_t __stdcall CGPlayer_C::OnQuestGiverSendQuest(CDataStore* msg) {
    QuestRewards_C rewards;
    memset(&rewards, 0, sizeof(rewards));
    WOWGUID npc;
    msg->Get(npc.m_guid);
    WOWGUID inform;
    msg->Get(inform.m_guid);
    int32_t questId;
    msg->Get(questId);
    char title[512];
    msg->GetString(title, sizeof(title));
    char details[3000];
    msg->GetString(details, sizeof(details));
    char objectives[3000];
    msg->GetString(objectives, sizeof(objectives));
    char giverText[1024];
    msg->GetString(giverText, sizeof(giverText));
    char giverName[256];
    msg->GetString(giverName, sizeof(giverName));
    char turnInText[1024];
    msg->GetString(turnInText, sizeof(turnInText));
    char turnInName[256];
    msg->GetString(turnInName, sizeof(turnInName));
    int32_t giverPortrait;
    msg->Get(giverPortrait);
    int32_t turnInPortrait;
    msg->Get(turnInPortrait);
    uint8_t autoLaunched;
    msg->Get(autoLaunched);
    uint32_t flags;
    msg->Get(flags);
    int32_t suggestedGroupNum;
    msg->Get(suggestedGroupNum);
    uint8_t startCheat;
    msg->Get(startCheat);
    uint8_t fromAreaTrigger;
    msg->Get(fromAreaTrigger);
    int32_t criteriaSpell;
    msg->Get(criteriaSpell);
    ReadQuestRewards(msg, rewards);
    uint32_t emoteCount;
    msg->Get(emoteCount);
    QUESTGIVEREMOTEQUEUE emotes;
    emotes.m_nodes = static_cast<QUESTGIVEREMOTENODE*>(alloca(emoteCount * sizeof(QUESTGIVEREMOTENODE)));
    emotes.m_count = emoteCount;

    for (int32_t i = 0; i < static_cast<int32_t>(emoteCount); i++) {
        msg->Get(reinterpret_cast<uint32_t&>(emotes.m_nodes[i].m_emote));
        msg->Get(emotes.m_nodes[i].m_delay);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            unit->PlayEmotes(emotes);
        }

        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && CGQuestInfo::m_npc) {
            CDataStore response;
            response.Put(static_cast<uint32_t>(MSG_QUEST_PUSH_RESULT));
            response.Put(inform ? inform.m_guid : npc.m_guid);
            response.Put(static_cast<uint32_t>(questId));
            response.Put(static_cast<uint8_t>(4));
            response.Finalize();
            ClientServices::Send2(&response);
            return 1;
        }
    }

    CGQuestInfo::SetState(npc, inform, 1, details, questId, startCheat, fromAreaTrigger, criteriaSpell);
    CGQuestInfo::SetLogDescription(objectives);
    CGQuestInfo::AddReward(title, rewards, autoLaunched, suggestedGroupNum, flags);
    CGQuestInfo::SetPortraits(giverPortrait, giverText, giverName, turnInPortrait, turnInText, turnInName);
    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_DETAIL, NULL);
    return 1;
}

// OFFSET: 0x005616D0/0x1401BF5E0
int32_t CGPlayer_C::CanUseItem(const ItemRecSparse* stats, int32_t& error) {
    error = ERR_MAX_VALUE;
    ItemRec* rec;

    if (!stats || !(rec = stats->GetNonSparse())) {
        error = ERR_ITEM_NOT_FOUND;
        return 0;
    }

    CGUnitData* unit = this->m_unit;

    if (stats->m_requiredLevel > static_cast<int32_t>(unit->m_unitFieldLevel)) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (!((1 << (unit->m_unitFieldBytes0[1] - 1)) & stats->m_allowableClass) || !((1 << (unit->m_unitFieldBytes0[0] - 1)) & stats->m_allowableRace)) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    uint32_t proficiency = CGPlayer_C::GetProficiency(static_cast<uint8_t>(rec->m_classID));

    if (proficiency && !((1 << rec->m_subclassID) & proficiency)) {
        error = ERR_PROFICIENCY_NEEDED;
        return 0;
    }

    if (stats->m_requiredSkill && !(stats->m_flags[0] & 0x20040000)) {
        int32_t index = this->GetSkillIndex(stats->m_requiredSkill);

        if (index < 0 || static_cast<int32_t>(this->GetSkillValue(index)) < stats->m_requiredSkillRank) {
            error = ERR_CANT_USE_ITEM;
            return 0;
        }
    }

    if (stats->m_requiredSpell && !this->IsSpellKnown(stats->m_requiredSpell) && !this->IsSpellSuperceded(stats->m_requiredSpell)) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (stats->m_requiredHonorRank && static_cast<int32_t>(this->m_player->m_playerFieldBytes[3]) < stats->m_requiredHonorRank) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (stats->m_requiredCityRank && !this->HasPvpMedal(static_cast<uint8_t>(stats->m_requiredCityRank))) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    if (stats->m_requiredReputationFaction && GetRepListRepValue(stats->m_requiredReputationFaction) < CGReputationInfo::m_reactionThresholds[stats->m_requiredReputationRank]) {
        error = ERR_CANT_USE_ITEM;
        return 0;
    }

    return 1;
}

// OFFSET: 0x005564D0
int32_t __stdcall CGPlayer_C::OnQuestGiverChooseReward(CDataStore* msg) {
    WOWGUID npc;
    msg->Get(npc.m_guid);
    int32_t questId;
    msg->Get(questId);
    char title[512];
    msg->GetString(title, sizeof(title));
    char text[3000];
    msg->GetString(text, sizeof(text));
    char giverText[1024];
    msg->GetString(giverText, sizeof(giverText));
    char giverName[256];
    msg->GetString(giverName, sizeof(giverName));
    char turnInText[1024];
    msg->GetString(turnInText, sizeof(turnInText));
    char turnInName[256];
    msg->GetString(turnInName, sizeof(turnInName));
    int32_t giverPortrait;
    msg->Get(giverPortrait);
    int32_t turnInPortrait;
    msg->Get(turnInPortrait);
    uint8_t autoLaunched;
    msg->Get(autoLaunched);
    uint32_t flags;
    msg->Get(flags);
    int32_t suggestedGroupNum;
    msg->Get(suggestedGroupNum);
    uint32_t emoteCount;
    msg->Get(emoteCount);
    QUESTGIVEREMOTEQUEUE emotes;
    emotes.m_nodes = static_cast<QUESTGIVEREMOTENODE*>(alloca(emoteCount * sizeof(QUESTGIVEREMOTENODE)));
    emotes.m_count = emoteCount;

    for (int32_t i = 0; i < static_cast<int32_t>(emoteCount); i++) {
        msg->Get(emotes.m_nodes[i].m_delay);
        msg->Get(reinterpret_cast<uint32_t&>(emotes.m_nodes[i].m_emote));
    }

    if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__))) {
        unit->PlayEmotes(emotes);
    }

    QuestRewards_C rewards;
    ReadQuestRewards(msg, rewards);
    CGQuestInfo::SetState(npc, WOWGUID(), 3, text, questId, 0, 0, 0);
    CGQuestInfo::AddReward(title, rewards, autoLaunched, suggestedGroupNum, flags);
    CGQuestInfo::SetPortraits(giverPortrait, giverText, giverName, turnInPortrait, turnInText, turnInName);
    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_COMPLETE, NULL);
    return 1;
}

// OFFSET: 0x005653D0
static void QuestAcceptCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(id, none, NULL, NULL, false);

    if (!quest) {
        return;
    }

    CGGameUI::DisplayError(ERR_QUEST_ACCEPTED_S, quest->m_name);

    if (quest->m_infoId == 1) {
        CGTutorial::TriggerTutorial(0x28);
        CGTutorial::TriggerTutorial(0x26);
    }

    if (!IsTutorialFlagged(0x14) && ++s_questsAccepted >= 4) {
        CGTutorial::TriggerTutorial(0x14);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_ACCEPTED, "%d", CGQuestLog::GetQuestIndex(quest->m_id));
}

// OFFSET: 0x005654D0
int32_t OnUpdateQuest(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    CGQuestLog::Update(1);
    uint32_t slot = (offset - 0x28) / sizeof(PlayerQuestLogEntry);
    PlayerQuestLogEntry* entry = slot > 25 ? NULL : &player->m_player->m_playerQuestLog[slot];
    const PlayerQuestLogEntry* old = static_cast<const PlayerQuestLogEntry*>(value);
    uint32_t questId = entry->m_questId;
    WOWGUID none = { 0 };

    if (questId && (questId != old->m_questId || (!(entry->m_state & 2) && (old->m_state & 2)))) {
        QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestAcceptCallback, NULL, false);

        if (quest) {
            CGGameUI::DisplayError(ERR_QUEST_ACCEPTED_S, quest->m_name);

            if (quest->m_infoId == 1) {
                CGTutorial::TriggerTutorial(0x28);
                CGTutorial::TriggerTutorial(0x26);
            }

            if (!IsTutorialFlagged(0x14) && ++s_questsAccepted >= 4) {
                CGTutorial::TriggerTutorial(0x14);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_ACCEPTED, "%d", CGQuestLog::GetQuestIndex(quest->m_id));
        }
    }

    QuestCache* quest = g_questDBCache.GetRecord(entry->m_questId, none, NULL, NULL, false);

    if (quest) {
        int32_t index = CGQuestLog::GetQuestIndex(quest->m_id) - 1;

        if (index >= 0 && (quest->m_flags & 0x10000) && CGQuestLog::IsQuestCompleteEx(ClntObjMgrGetActivePlayerObj(), index, true, 0, 0)) {
            WOWGUID self = player->m_obj->m_guid;
            CGPlayer_C::CompleteQuest(self, quest->m_id, 1);
        }
    }

    if (old && old->m_questId && entry->m_questId != old->m_questId) {
        CGQuestLog::ClearQuest(old->m_questId);
    }

    return 1;
}

// OFFSET: 0x00553800/0x1401AE740
static int32_t OnUpdatePlayerNameScriptEvent(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGGameUI::UnitNameUpdate(guid);
    FrameScript_SignalEvent(reinterpret_cast<uintptr_t>(param), "%s", "player");

    return 1;
}

// OFFSET: 0x00553840
static int32_t OnUpdateBattlegroundRating(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    if (CGMerchantInfo::m_merchant) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
    }

    return 1;
}

// OFFSET: 0x005538E0
static int32_t OnUpdatePlayerMastery(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    FrameScript_SignalEvent(SCRIPT_EVENT_MASTERY_UPDATE, NULL);

    return 1;
}

// OFFSET: 0x005570F0
static int32_t OnUpdatePlayerHitModifiers(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_RATING_UPDATE, NULL);
    }

    return 1;
}

// OFFSET: 0x00557130
static int32_t OnUpdatePlayerHaste(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_RATING_UPDATE, NULL);
    }

    return 1;
}

// OFFSET: 0x0055F240
static int32_t OnUpdatePlayerGuildLevel(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    uint32_t oldLevel = *static_cast<const uint32_t*>(value);

    if (guid == ClntObjMgrGetActivePlayer()) {
        CGGuildInfo::UpdateGuildLevel();
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_PERK_UPDATE, NULL);
    }

    if (oldLevel) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && static_cast<int32_t>(player->m_player->m_playerGuildlevel) > 1) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_GUILD_LEVEL, "%d", player->m_player->m_playerGuildlevel);
            CEffect* effect = CEffect::Create();
            effect->AddSpecialEffect(14, guid, &CGObject_C::EffectOnceHandler);
            effect->m_flags |= 0x20;
            effect->Release();

            if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
                SI3::PlayUISound(23285, NULL, true);
            }
        }
    }

    return 1;
}

// OFFSET: 0x0055FF50
static int32_t OnUpdatePlayerFieldBytes(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    uint8_t oldBytes = *static_cast<const uint8_t*>(value);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint8_t bytes = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerFieldBytes[0] : 0;

        if ((oldBytes ^ bytes) & 0x20) {
            CGPetInfo::UpdatePetTempBar();
        }
    }

    return 1;
}

// OFFSET: 0x0055F440/0x1401BCD30
int32_t OnUpdateMoney(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("LOOTWINDOWCOINSOUND", NULL, &properties, NULL, __FILE__, __LINE__, "OnUpdateMoney()");
    CGClassTrainer::RefreshList();
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_MONEY, NULL);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    uint64_t tradeMoney = CGTradeInfo::m_playerTrade.m_money;

    if (tradeMoney && player && tradeMoney > player->GetCoinage()) {
        CGTradeInfo::RemoveMoney(static_cast<int64_t>(tradeMoney - player->GetCoinage()));
    }

    return 1;
}

// OFFSET: 0x0055F520/0x1401BCE50
int32_t OnUpdateShapeshiftForm(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (player->m_shapeshiftSuppressed) {
            player->m_shapeshiftSuppressed = 0;
        }

        if (player->IsShapeShifted()) {
            const SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm());

            if (form && !(form->m_flags & 8)) {
                CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, 0);
            }

            CGMinimapFrame::RefreshShapeshiftTrackingSpells();
        } else if (value) {
            const SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(*static_cast<const uint8_t*>(value));

            if (form && form->unk_0050) {
                C3Vector position = player->GetPosition();

                if (CGUnit_C::s_activeMover == player->m_obj->m_guid || player->IsActivePlayer()) {
                    bool listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
                    SE3SoundKitProperties properties;
                    properties.m_priority = 110;
                    SE3::PlaySoundKit(form->unk_0050, listenerAtCharacter ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "OnUpdateShapeshiftForm", 0, NULL);
                } else {
                    SE3SoundKitProperties properties;
                    SE3::PlaySoundKit(form->unk_0050, &position, &properties, NULL, __FILE__, __LINE__, "OnUpdateShapeshiftForm", 0, NULL);
                }
            }
        }
    }

    CGSpellBook::UpdateUsable();
    CGSpellBook::UpdateSelection(0);
    CGActionBar::UpdateShapeShiftBar();
    CGActionBar::UpdateOverrideSpellBar();
    CGActionBar::UpdateBonusBar();
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORM, NULL);

    return 1;
}

// OFFSET: 0x0055F6F0/0x1401BD0D0
void CGPlayer_C::OnSkillRankChanged(int32_t index, uint16_t oldRank) {
    ConsolePrintf("Skill %d increased from %d to %d", this->GetSkillLineID(index), oldRank, this->GetSkillValue(index));
    uint16_t skillId = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillLineid0[index] : 0;
    const SkillLineRec* skill = g_skillLineDB.GetRecord(skillId);

    if (!skill) {
        return;
    }

    uint16_t lineId = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillLineid0[index] : 0;
    const SkillRaceClassInfoRec* info = SpellTableLookupInfo(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], lineId);

    if (info && !(info->m_flags & 0x402)) {
        if (oldRank) {
            if (static_cast<int32_t>(this->GetSkillValue(index)) > 0) {
                CGGameUI::DisplayError(ERR_SKILL_UP_SI, skill->m_displayName, this->GetSkillValue(index));
            }
        } else {
            CGGameUI::DisplayError(ERR_SKILL_GAINED_S, skill->m_displayName);
        }
    }

    if (skill->m_categoryID == 9 || skill->m_categoryID == 11) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }
}

// OFFSET: 0x0055F890
int32_t SkillRankChangeHandler(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_ATTACK, "%s", "player");
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_DEFENSE, "%s", "player");
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_RANGED_ATTACK_POWER, "%s", "player");
    PlayerSignalSkillStatChanges();
    const int16_t* oldRanks = static_cast<const int16_t*>(value);

    for (uint32_t i = 0; i < 128; i++) {
        if (static_cast<int16_t>(player->m_player->m_playerSkillRank0[i]) != oldRanks[i]) {
            player->OnSkillRankChanged(i, oldRanks[i]);
        }
    }

    CGClassTrainer::RefreshList();
    CGTradeSkillInfo::RefreshList(0);
    CGMerchantInfo::RefreshList();

    return 1;
}

// OFFSET: 0x0056E430/0x1401CF150
void CGPlayer_C::SetActiveMirrorHandlers() {
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x10, 8, CritterUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t i = 0x4B8; i < 0x570; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x5F0; i < 0x6D0; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x6D0; i < 0x708; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, InventoryUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x2C; i < 0x414; i += 20) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 20, OnUpdateQuest, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t i = 0x1224; i < 0x1288; i += 4) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 4, ReportDailyQuestsRemaining, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1060, 4, OnUpdateMoney, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x18, 8, CharmChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x1B7, 1, OnUpdateShapeshiftForm, NULL, static_cast<HANDLER_PRIORITY>(0), 1);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x768, 8, FarsightChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 1);

    for (uint32_t i = 0x770; i < 0x790; i += 8) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 8, OnUpdatePlayerNameScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_KNOWN_TITLES_UPDATE)), static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x998, 0x100, SkillRankChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xA98, 0x100, SkillMaxRankChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xC98, 0x100, SkillMaxRankChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xB98, 0x100, SignalPlayerSkillStatEventsAndRefresh, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1F, 1, OnUpdateRestState, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x105C, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_EXHAUSTION)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x790, 8, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_XP_UPDATE)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1068, 0x5C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10D8, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1150, 2, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1154, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x20, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_RANK_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x115B, 1, OnUpdatePlayerAuraVision, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1158, 2, OnUpdatePlayerOverrideSpellsID, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x115C, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_FACTION)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x11C8, 0x54, OnArenaTeamsChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x121C, 4, OnUpdateBattlegroundRating, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1160, 0x68, SignalCombatRatingUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDE0, 4, SignalCombatRatingUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDAC, 0x34, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x78, 0x28, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t i = 0x1288; i < 0x1298; i += 4) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 4, OnUpdatePlayerRuneRegen, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDA0, 4, OnUpdatePlayerTrackingMask, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E0, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)), static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10DC, 4, SignalCombatRatingUpdate, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1298, 0xC, OnUpdatePlayerNoReagentCost, NULL, static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t i = 0x12C8; i < 0x12EC; i += 4) {
        ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, 4, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyph), NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x12EC, 4, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyphSlotsEnabled), NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x12F0, 4, reinterpret_cast<MIRRORHANDLERFUNC>(SignalPetSpellPowerUpdate), NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDE8, 4, OnUpdatePlayerMastery, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E4, 1, OnUpdatePlayerFieldBytes, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x133C, 8, OnUpdatePlayerHitModifiers, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1348, 0x10, OnUpdatePlayerHaste, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E8, 4, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_SELF_RES_SPELL_CHANGED)), static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x0056EB60/0x1401CFB00
void CGPlayer_C::UnsetActiveMirrorHandlers() {
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x10, CritterUpdate, NULL);

    for (uint32_t i = 0x4B8; i < 0x570; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, NULL);
    }

    for (uint32_t i = 0x5F0; i < 0x6D0; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, NULL);
    }

    for (uint32_t i = 0x6D0; i < 0x708; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, InventoryUpdate, NULL);
    }

    for (uint32_t i = 0x2C; i < 0x414; i += 20) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, OnUpdateQuest, NULL);
    }

    for (uint32_t i = 0x1224; i < 0x1288; i += 4) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, ReportDailyQuestsRemaining, NULL);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1060, OnUpdateMoney, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x18, CharmChangeHandler, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x768, FarsightChangeHandler, NULL);

    for (uint32_t i = 0x770; i < 0x790; i += 8) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, OnUpdatePlayerNameScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_KNOWN_TITLES_UPDATE)));
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_UNIT, 0x1B7, OnUpdateShapeshiftForm, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x998, SkillRankChangeHandler, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xA98, SkillMaxRankChangeHandler, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xC98, SkillMaxRankChangeHandler, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xB98, SignalPlayerSkillStatEventsAndRefresh, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1F, OnUpdateRestState, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x105C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_EXHAUSTION)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x790, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_XP_UPDATE)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1068, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10D8, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1150, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1154, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_KILLS_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x20, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_PVP_RANK_CHANGED)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x115B, OnUpdatePlayerAuraVision, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1158, OnUpdatePlayerOverrideSpellsID, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x115C, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_UPDATE_FACTION)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x11C8, OnArenaTeamsChanged, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x121C, OnUpdateBattlegroundRating, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1160, SignalCombatRatingUpdate, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDE0, SignalCombatRatingUpdate, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDAC, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x78, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));

    for (uint32_t i = 0x1288; i < 0x1298; i += 4) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, OnUpdatePlayerRuneRegen, NULL);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDA0, OnUpdatePlayerTrackingMask, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E0, OnUpdatePlayerScriptEvent, reinterpret_cast<void*>(static_cast<uintptr_t>(SCRIPT_EVENT_PLAYER_DAMAGE_DONE_MODS)));
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10DC, SignalCombatRatingUpdate, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1298, OnUpdatePlayerNoReagentCost, NULL);

    for (uint32_t i = 0x12C8; i < 0x12EC; i += 4) {
        ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, i, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyph), NULL);
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x12EC, reinterpret_cast<MIRRORHANDLERFUNC>(OnUpdatePlayerGlyphSlotsEnabled), NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x12F0, reinterpret_cast<MIRRORHANDLERFUNC>(SignalPetSpellPowerUpdate), NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0xDE8, OnUpdatePlayerMastery, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x10E4, OnUpdatePlayerFieldBytes, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x133C, OnUpdatePlayerHitModifiers, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1340, OnUpdatePlayerHitModifiers, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1348, OnUpdatePlayerHaste, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x134C, OnUpdatePlayerHaste, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1350, OnUpdatePlayerHaste, NULL);
    ClntObjMgrUnsetObjMirrorHandler(this->m_obj->m_guid, ID_PLAYER, 0x1354, OnUpdatePlayerHaste, NULL);
}

// OFFSET: 0x0055F0C0/0x1401BC960
static int32_t GuildTimestampEnumCallback(WOWGUID guid, void* param) {
    CGObject_C* object = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (object) {
        const uint64_t& guild = *static_cast<const uint64_t*>(param);

        if (!(object->m_obj->m_type & TYPEMASK_PLAYER)) {
            uint64_t objectGuild = object->m_obj->m_dataType == 1 ? object->m_obj->m_data.m_guid : 0;

            if (objectGuild == guild) {
                object->LoadGuildTextures(guild);
                return 1;
            }
        }

        uint64_t objectGuild = object->m_obj->m_dataType == 1 ? object->m_obj->m_data.m_guid : 0;

        if (objectGuild == guild) {
            static_cast<CGPlayer_C*>(object)->OnGuildChanged(0);
        }
    }

    return 1;
}

// OFFSET: 0x0055F160/0x1401BCA50
static void GuildTimestampChanged(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        ClntObjMgrEnumVisibleObjects(GuildTimestampEnumCallback, const_cast<uint64_t*>(&key));
    }
}

// OFFSET: 0x00565680/0x1401C37A0
static int32_t OnUpdateGuildTimeStamp(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t index = 0;

    if (!player) {
        return 1;
    }

    uint64_t guild = player->m_obj->m_dataType == 1 ? player->m_obj->m_data.m_guid : 0;
    uint32_t timestamp = player->m_player->m_playerGuildTimestamp;

    for (; index < s_guildUpdateTimestamps.m_count; index++) {
        if (s_guildUpdateTimestamps.m_data[index].m_guildGUID == guild) {
            break;
        }
    }

    if (index < s_guildUpdateTimestamps.m_count) {
        if (s_guildUpdateTimestamps.m_data[index].m_timestamp == timestamp) {
            return 1;
        }
    } else {
        s_guildUpdateTimestamps.SetCount(index + 1);
        s_guildUpdateTimestamps.m_data[index].m_guildGUID = guild;
    }

    s_guildUpdateTimestamps.m_data[index].m_timestamp = timestamp;
    g_guildInfoCache.Invalidate(guild);
    g_guildInfoCache.GetRecord2(guild, guid, GuildTimestampChanged, NULL, false);

    return 1;
}

// OFFSET: 0x00567BD0/0x1401C3F60
static void InitializeWeaponSkills() {
    int32_t count;

    if (!GetWeaponSubClassCount(&count)) {
        return;
    }

    s_weaponSkills.SetCount(count);

    for (int32_t i = 0; i < count; i++) {
        s_weaponSkills.m_data[i] = 0;
    }

    s_defenseSpellId = 0;
    int32_t index = g_spellDB.m_numRecords;

    while (index--) {
        const SpellRec* spell = g_spellDB.GetRecordByIndex(index);

        if (!(spell->m_attributes & 0x40)) {
            continue;
        }

        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

        if (!s_defenseSpellId && effect && effect->m_effect == 26) {
            s_defenseSpellId = spell->m_id;
        }

        if (!spell->m_spellEquippedItemsID) {
            continue;
        }

        const SpellEquippedItemsRec* items = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID);

        if (items && items->m_equippedItemClass == 2) {
            uint32_t mask = items->m_equippedItemSubclass;

            if (!((mask - 1) & mask)) {
                uint32_t bit;

                if (_BitScanForward(reinterpret_cast<unsigned long*>(&bit), mask) && static_cast<int32_t>(bit) < count) {
                    s_weaponSkills.m_data[bit] = spell->m_id;
                }
            }
        }
    }
}

// OFFSET: 0x0056A7B0/0x1401C9920
void CGPlayer_C::Initialize() {
    s_giftWrapItem = WOWGUID();
    s_bindPointSet = 0;
    s_lastUpdatePVPGroup = WOWGUID();
    s_lastUpdatePVPSequence = 0;
    s_lastUpdatePVPTime = 0;
    s_lastUpdateGroup = WOWGUID();
    s_lastUpdateSequence = 0;
    s_lastUpdateTime = 0;
    InitializeWeaponSkills();
    s_attackSwingError = 0;
    s_attackSwingErrorTime = FrameTime::GetCurTimeMs();

    for (uint32_t i = 0x414; i < 0x4AC; i += 8) {
        ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, i, 8, OnUpdateInventoryComponent, NULL, static_cast<HANDLER_PRIORITY>(1), 0);
    }

    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, 0, 16, PetChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0xC, 4, GuildRankUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x14, 4, OnUpdatePlayerGuildLevel, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x24, 4, DuelTeamUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x8, 4, OnUpdatePlayerFlags, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x28, 4, OnUpdateGuildTimeStamp, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x22, 1, OnUpdatePVPTitle, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x21, 1, OnUpdateInebriation, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x4AC, 4, OnUpdatePlayerNameScriptEvent, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x1B, 1, OnUpdatePlayerHairColor, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x1A, 1, OnUpdatePlayerHairStyle, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x1C, 1, OnUpdatePlayerFacialStyle, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_PLAYER, 0x18, 1, OnUpdatePlayerSkinID, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x0056A940
void CGPlayer_C::Shutdown() {
    s_guildUpdateTimestamps.Clear();
    s_initialSpells.Clear();
    s_initialCompanionSpells.Clear();
    s_pendingRuneConverts.Clear();
    s_itemSwaps.Clear();
    s_weaponSkills.Clear();
    s_lastBinderGuid = WOWGUID();
    s_tabardSavePending = 0;

    if (s_inspectData) {
        SMemFree(s_inspectData, "delete", -1, 0);
        s_inspectData = NULL;
    }

    for (uint32_t i = 0x414; i <= 0x4A4; i += 8) {
        ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, i, OnUpdateInventoryComponent, NULL);
    }

    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, 0, PetChangeHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0xC, GuildRankUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x14, OnUpdatePlayerGuildLevel, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x24, DuelTeamUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x8, OnUpdatePlayerFlags, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x28, OnUpdateGuildTimeStamp, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x22, OnUpdatePVPTitle, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x21, OnUpdateInebriation, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x4AC, OnUpdatePlayerNameScriptEvent, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x1B, OnUpdatePlayerHairColor, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x1A, OnUpdatePlayerHairStyle, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_PLAYER, 0x1C, OnUpdatePlayerFacialStyle, NULL);
}

struct QuestCompleteRewards {
    /* 0000 */ int32_t m_xp;
    /* 0004 */ int32_t m_money;
    /* 0008 */ int32_t m_honor;
    /* 000C */ int32_t m_talents;
    /* 0010 */ int32_t m_arenaPoints;
};

// OFFSET: 0x0056BA60/0x1401CB680
static void CrossedInebriationThresholdItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    int32_t threshold = static_cast<int32_t>(reinterpret_cast<intptr_t>(param));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    char token[32];
    char buffer[3000];
    WOWGUID none = WOWGUID();
    ItemRecSparse* stats;

    if (id && (stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false)) != NULL) {
        if (player->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
            SStrPrintf(token, sizeof(token), "DRUNK_MESSAGE_ITEM_OTHER%d", threshold + 1);
            const char* link = CGTooltip::GetItemHyperlinkString(id, stats->m_quality, 0, 0, 0, 0);
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), player->GetUnitName(NULL, 1), link);
            CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
            return;
        }

        SStrPrintf(token, sizeof(token), "DRUNK_MESSAGE_ITEM_SELF%d", threshold + 1);
        const char* link = CGTooltip::GetItemHyperlinkString(id, stats->m_quality, 0, 0, 0, 0);
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), link);
    } else {
        if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            SStrPrintf(token, sizeof(token), "DRUNK_MESSAGE_SELF%d", threshold + 1);
        } else {
            SStrPrintf(token, sizeof(token), "DRUNK_MESSAGE_OTHER%d", threshold + 1);
        }

        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), player->GetUnitName(NULL, 1));
    }

    CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
}

// OFFSET: 0x0056BC00
int32_t Packet_SMSG_CROSSED_INEBRIATION_THRESHOLD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint32_t threshold;
    uint32_t itemId;
    msg->Get(guid.m_guid);
    msg->Get(threshold);
    msg->Get(itemId);

    if (!itemId) {
        CrossedInebriationThresholdItemCallback(0, guid, reinterpret_cast<void*>(static_cast<intptr_t>(threshold)), true);
        return 1;
    }

    if (g_itemSparseDB.GetRecord(itemId, guid, &CrossedInebriationThresholdItemCallback, reinterpret_cast<void*>(static_cast<intptr_t>(threshold)), false)) {
        CrossedInebriationThresholdItemCallback(itemId, guid, reinterpret_cast<void*>(static_cast<intptr_t>(threshold)), true);
    }

    return 1;
}

// OFFSET: 0x00555D70
int32_t __stdcall CGPlayer_C::OnQuestGiverListQuests(CDataStore* msg) {
    WOWGUID npc;
    msg->Get(npc.m_guid);

    char greeting[2048];
    msg->GetString(greeting, sizeof(greeting));

    QUESTGIVEREMOTENODE emote;
    msg->Get(emote.m_delay);
    msg->Get(reinterpret_cast<uint32_t&>(emote.m_emote));

    uint8_t count;
    msg->Get(count);

    s_questListNpc = npc;
    memset(s_questListIds, 0, sizeof(s_questListIds));
    memset(s_questListStatus, 0, sizeof(s_questListStatus));
    memset(s_questListLevels, 0, sizeof(s_questListLevels));
    memset(s_questListFlags, 0, sizeof(s_questListFlags));
    memset(s_questListRepeatable, 0, sizeof(s_questListRepeatable));
    CGQuestInfo::SetState(npc, WOWGUID(), 0, greeting, 0, 0, 0, 0);

    char titles[32][512];

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(s_questListIds[i]);
        msg->Get(s_questListStatus[i]);
        msg->Get(s_questListLevels[i]);
        msg->Get(s_questListFlags[i]);

        uint8_t repeatable = 0;
        msg->Get(repeatable);
        s_questListRepeatable[i] = repeatable;

        msg->GetString(titles[i], sizeof(titles[i]));

        uint32_t status = s_questListStatus[i];

        if (status == 3 || status == 4) {
            CGQuestInfo::AddQuestInProgress(s_questListIds[i], titles[i], s_questListLevels[i], s_questListFlags[i], s_questListRepeatable[i]);
        } else {
            CGQuestInfo::AddQuest(s_questListIds[i], titles[i], s_questListLevels[i], s_questListFlags[i], status == 0, s_questListRepeatable[i]);
        }
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->SetEmoteQueue(&emote, 1);
    }

    CGQuestInfo::EndQuestList();
    return 1;
}

// OFFSET: 0x005562C0
int32_t __stdcall CGPlayer_C::OnQuestGiverRequestItems(CDataStore* msg) {
    QuestRequest request;
    memset(&request, 0, sizeof(request));
    WOWGUID npc;
    msg->Get(npc.m_guid);
    int32_t questId;
    msg->Get(questId);
    char title[512];
    msg->GetString(title, sizeof(title));
    char text[3000];
    msg->GetString(text, sizeof(text));
    QUESTGIVEREMOTENODE emote;
    msg->Get(emote.m_delay);
    msg->Get(reinterpret_cast<uint32_t&>(emote.m_emote));
    int32_t autoLaunched;
    msg->Get(autoLaunched);
    uint32_t flags;
    msg->Get(flags);
    int32_t suggestedGroupNum;
    msg->Get(suggestedGroupNum);
    int32_t requiredMoney;
    msg->Get(requiredMoney);
    int32_t numItems;
    msg->Get(numItems);

    for (int32_t i = 0; i < numItems && i < 6; i++) {
        msg->Get(request.m_items[i].m_itemID);
        msg->Get(request.m_items[i].m_count);
        msg->Get(request.m_items[i].m_displayID);
    }

    int32_t numCurrencies;
    msg->Get(numCurrencies);

    for (int32_t i = 0; i < numCurrencies && i < 4; i++) {
        msg->Get(request.m_currencies[i].m_currencyID);
        msg->Get(request.m_currencies[i].m_count);
    }

    uint32_t completable0;
    msg->Get(completable0);
    uint32_t completable1;
    msg->Get(completable1);
    uint32_t completable2;
    msg->Get(completable2);
    uint32_t completable3;
    msg->Get(completable3);
    uint32_t completable4;
    msg->Get(completable4);
    CGQuestInfo::SetState(npc, WOWGUID(), 2, text, questId, 0, 0, 0);
    int32_t canComplete = completable0 && completable1 && completable2 && completable3 && completable4;
    CGQuestInfo::AddItemRequest(title, request, requiredMoney, canComplete, autoLaunched, suggestedGroupNum, flags);

    if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(npc, TYPEMASK_UNIT, __FILE__, __LINE__))) {
        unit->SetEmoteQueue(&emote, 1);
    }

    return 1;
}

// OFFSET: 0x00553120
int32_t __stdcall CGPlayer_C::OnQuestGiverInvalidQuest(CDataStore* msg) {
    uint32_t reason;
    msg->Get(reason);

    switch (reason) {
        case 1:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_LOW_LEVEL);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_WRONG_RACE);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_QUEST_ALREADY_DONE);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_QUEST_ONLY_ONE_TIMED);
            break;

        case 13:
        case 18:
            CGGameUI::DisplayError(ERR_QUEST_ALREADY_ON);
            break;

        case 16:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_EXPANSION);
            break;

        case 21:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_MISSING_ITEMS);
            break;

        case 23:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_NOT_ENOUGH_MONEY);
            break;

        case 26:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I, 25);
            break;

        case 27:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_CAIS);
            break;

        case 29:
            CGGameUI::DisplayError(ERR_QUEST_ALREADY_DONE_DAILY);
            break;

        case 31:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_SPELL);
            break;

        default:
            CGGameUI::DisplayError(ERR_QUEST_NEED_PREREQS);
            break;
    }

    CGQuestInfo::QuestGiverFinished(0, 0, 0, 0);
    return 1;
}

// OFFSET: 0x00553380
static void QuestFailedCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(id, none, NULL, NULL, false);

    if (!quest) {
        return;
    }

    switch (s_questFailedReason) {
        case 4:
        case 16:
        case 50:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_BAG_FULL_S, quest->m_name);
            CGGameUI::DisplayError(ERR_INV_FULL);
            break;

        case 17:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_MAX_COUNT_S, quest->m_name);
            break;

        case 74:
            CGGameUI::DisplayError(ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I, 25);
            break;

        default:
            if (!CGQuestLog::IsQuestExpiredByID(id)) {
                CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
            }

            break;
    }
}

// OFFSET: 0x005534B0
int32_t __stdcall CGPlayer_C::OnQuestGiverQuestFailed(CDataStore* msg) {
    uint32_t questId;
    msg->Get(questId);
    msg->Get(s_questFailedReason);

    QuestCache* quest = g_questDBCache.GetRecord(questId, CGQuestInfo::m_npc, QuestFailedCallback, NULL, false);

    if (quest) {
        switch (s_questFailedReason) {
            case 4:
            case 16:
            case 50:
                CGGameUI::DisplayError(ERR_QUEST_FAILED_BAG_FULL_S, quest->m_name);
                CGGameUI::DisplayError(ERR_INV_FULL);
                break;

            case 17:
                CGGameUI::DisplayError(ERR_QUEST_FAILED_MAX_COUNT_S, quest->m_name);
                break;

            case 74:
                CGGameUI::DisplayError(ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I, 25);
                break;

            default:
                if (!CGQuestLog::IsQuestExpiredByID(questId)) {
                    CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
                }

                break;
        }
    }

    CGQuestInfo::QuestGiverFinished(0, questId, 0, 0);
    return 1;
}

// OFFSET: 0x00468F10/0x14007D0C0
uint32_t CGItem::GetEnchantmentId(int32_t slot) {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return 0;
    }

    return this->m_item->m_itemFieldEnchantment[slot].m_id;
}

// OFFSET: 0x008A6580
uint32_t CGItem::GetEnchantmentDuration(int32_t slot) {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return 0;
    }

    return this->m_item->m_itemFieldEnchantment[slot].m_duration;
}

// OFFSET: 0x0055E170/0x1401BB6F0
void CGPlayer_C::CastRandomCritter() {
    uint32_t count = this->m_companionSpells.m_count;

    if (count) {
        Spell_C_CastSpell(this->m_companionSpells.m_data[rand() % count], NULL, WOWGUID::Make(0), false);
    }
}

// INLINED
int32_t ClientPlaySpellVisualHandler(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->IsHostileCritter()) {
        const SpellRec* spell = static_cast<const SpellRec*>(param);

        if (!spell) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        const SpellVisualRec* visual = GetSpellVisual(spell);

        if (visual) {
            SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_targetImpactKit);

            if (kit) {
                PlaySpellVisuaKitData data(spell, kit, 6);
                unit->PlaySpellVisualKit(data);
            }
        }
    }

    return 1;
}

// INLINED
void EnumVisibleForAuraEffect(const SpellRec* spell, int32_t a2, int32_t a3) {
    s_unk_C9EAB0 = a2;
    s_unk_C9EAB4 = a2 ? a3 : 0;
    ClntObjMgrEnumVisibleObjects(ClientPlaySpellVisualHandler, const_cast<SpellRec*>(spell));
}

// OFFSET: 0x0055D950
void CGPlayer_C::UpdateXRayVision(const SpellRec* spell) {
    ClntObjMgrEnumVisibleObjects(CGPlayer_C::XRayVisionEffectProc, const_cast<SpellRec*>(spell));
}

// PROVEN: 48b8bf8b IDENTICAL/NONE
// OFFSET: 0x00557020
int32_t OnUpdatePlayerOverrideSpellsID(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGSpellBook::UpdateUsable();
        CGSpellBook::UpdateSelection(0);
        CGActionBar::UpdateOverrideSpellBar();
        CGActionBar::UpdateBonusBar();
    }

    return 1;
}

// OFFSET: 0x0055FAB0
int32_t OnUpdatePlayerAuraVision(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint8_t changed = *static_cast<const uint8_t*>(value) ^ reinterpret_cast<const uint8_t*>(&player->m_player->m_playerFieldBytes2)[3];

        if (changed & 0x40) {
            CGWorldFrame::UpdateScreenEffect();
        }

        ClntObjMgrEnumVisibleObjects(AuraVisionUpdateHandler, &changed);
    }

    return 1;
}

// OFFSET: 0x00567DD0
void CGPlayer_C::RemoveEquipBypassSkill(int32_t spellId) {
    bool found = false;

    for (uint32_t i = 0; i < this->m_equipBypassSkills.m_count; i++) {
        if (this->m_equipBypassSkills.m_data[i] == spellId) {
            found = true;
        } else if (!found) {
            continue;
        }

        uint32_t count = this->m_equipBypassSkills.m_count;

        if (i) {
            if (i < count - 1) {
                this->m_equipBypassSkills.m_data[i - 1] = this->m_equipBypassSkills.m_data[i];
            }

            if (i == this->m_equipBypassSkills.m_count - 1) {
                this->m_equipBypassSkills.SetCount(i);
            }
        } else if (count == 1) {
            this->m_equipBypassSkills.m_count = 0;
        }
    }
}

// OFFSET: 0x00554980
void CGPlayer_C::ExitAutoCombatMode() {
    Spell_C_SetAutoRangedCombatSpell(0);
    EventUnregister(EVENT_ID_IDLE, CGPlayer_C::AutoCombatModeEventHandler);
    this->m_combatModeFlags &= ~0xEu;
    CGGameUI::UpdateCombatMode();
}

// OFFSET: 0x0055C340/0x1401B90B0
void CGPlayer_C::MeleeModeLeave(bool sendStop) {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && (this->CGUnit_C::IsInMelee() || this->m_combat.m_attackSent)) {
        this->StopAttack(sendStop);
        Spell_C_CancelMeleeSpells();
    }
}

// OFFSET: 0x005AE0B0
static int32_t IsCastAnimation(uint32_t anim) {
    return AnimationDataHasFlags(anim, 0xE0000);
}

// OFFSET: 0x00567EA0/0x1401C6AE0
void CGPlayer_C::ToggleSheathe() {
    int32_t casting = 0;

    if (this->m_unit->m_unitChannelSpell || (this->m_animationState & 0x400)) {
        casting = 1;
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !this->IsLocalClientControlled()) {
        return;
    }

    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000)) {
        return;
    }

    if ((this->m_unit->m_unitFieldFlags & 0x40000) || this->m_unit->m_unitFieldHealth <= 0) {
        return;
    }

    if (this->m_combat.m_attacking || casting || this->m_castSpellId) {
        return;
    }

    if (IsCastAnimation(this->GetCurrentTorsoAnimId()) || this->GetClientStandState()) {
        return;
    }

    CM2Model* model = this->m_worldModel;

    if (model) {
        if (!model->IsLoaded()) {
            return;
        }

        int32_t hasSheathe = model->HasSequence(90);
        int32_t hasUnsheathe = model->HasSequence(89);

        if (!hasSheathe || !hasUnsheathe || !model->HasBone(3) || !model->HasBone(2)) {
            return;
        }

        if (model->GetBoneSequenceId(3) == 90 || model->GetBoneSequenceId(2) == 90 || model->GetBoneSequenceId(3) == 89 || model->GetBoneSequenceId(2) == 89) {
            return;
        }
    }

    const CGUnitVirtualItem* mainHand = this->GetVirtualItem(0, false);
    const CGUnitVirtualItem* offHand = this->GetVirtualItem(1, false);
    const CGUnitVirtualItem* ranged = this->GetVirtualItem(2, false);
    ChrClassesRec* playerClass = g_chrClassesDB.GetRecord(this->m_unit->m_unitFieldBytes0[1]);

    if (!playerClass || (playerClass->m_flags & 0x8)) {
        ranged = NULL;
    }

    switch (this->m_sheatheState) {
        case 0:
            if (mainHand || offHand) {
                this->SetSheatheState(1, 0, 0);
            } else if (ranged) {
                this->SetSheatheState(2, 0, 0);
            }

            if (this->CGUnit_C::IsInMelee()) {
                this->SetCombatMode(true);
            }

            break;

        case 1:
            if (ranged) {
                this->SetSheatheState(2, 0, 0);
            } else {
                this->SetSheatheState(0, 0, 0);
            }

            break;

        case 2:
            this->SetSheatheState(0, 0, 0);
            break;
    }
}

// OFFSET: 0x005686C0/0x1401C75B0
int32_t CGPlayer_C::AutoCombatModeEventHandler(const void* data, void* param) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 1;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!target) {
        return 1;
    }

    if (!CGSpellBook::m_autoRangedSpell) {
        Spell_C_SetAutoRangedCombatSpell(0);
        return 1;
    }

    if (player->IsInMeleeRange(target)) {
        if (!player->CGUnit_C::IsInMelee()) {
            WOWGUID guid = target->m_obj->m_guid;
            player->MeleeModeEnter(guid, 0, 0);
        }

        player->m_combatModeFlags &= ~4u;
        return 1;
    }

    if (Spell_C_GetAutoRepeatingSpell()) {
        return 1;
    }

    if (!player->CanAttackNow(target)) {
        player->SetCombatMode(true);
        return 1;
    }

    if (player->m_combatModeFlags & 8) {
        player->ExitAutoCombatMode();
        return 1;
    }

    C3Vector toTarget = target->GetPosition() - player->GetPosition();
    float minRange;
    float maxRange;
    Spell_C_GetMinMaxRange(player, CGSpellBook::m_autoRangedSpell, &minRange, &maxRange, target);
    float distanceSq = toTarget.y * toTarget.y + toTarget.z * toTarget.z + toTarget.x * toTarget.x;

    if (minRange * minRange < distanceSq || !player->CGUnit_C::IsInMelee()) {
        if (player->CGUnit_C::IsInMelee()) {
            player->MeleeModeLeave(true);
        }

        if (!Spell_C_GetAutoRangedCombatSpell()) {
            Spell_C_SetAutoRangedCombatSpell(CGSpellBook::m_autoRangedSpell);
            player->m_combatModeFlags |= 4;
        }

        C3Vector direction = player->m_movementData.GetDirection();

        if (direction.y * toTarget.y + direction.z * toTarget.z + direction.x * toTarget.x > 0.0f && !(player->m_movement->m_flags & 0x33) && maxRange * maxRange > distanceSq && Spell_C_CastSpell(CGSpellBook::m_autoRangedSpell, NULL, target->m_obj->m_guid, false)) {
            Spell_C_ClearLastAutoRangedCombatError();
        }
    }

    return 1;
}

// OFFSET: 0x0055C880
void CGPlayer_C::OnLevelChange() {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGSpellBook::UpdateSpells(0, 0, 0);
        CGClassTrainer::RefreshList();
        CGUnit_C::NamePlateUpdateLevels(true);
        FrameScript_SignalEvent(SCRIPT_EVENT_PVPQUEUE_ANYWHERE_UPDATE_AVAILABLE, NULL);
    }
}

// OFFSET: 0x00553870
int32_t SkillMaxRankChangeHandler(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGClassTrainer::RefreshList();
    CGTradeSkillInfo::RefreshList(0);
    PlayerSignalSkillStatChanges();
    return 1;
}

// OFFSET: 0x00561960/0x1401BF8B0
int32_t __stdcall CGPlayer_C::OnLootMoneyNotify(CDataStore* msg) {
    uint32_t amount;
    uint8_t alone;
    msg->Get(amount);
    msg->Get(alone);

    if (amount) {
        char money[1024];
        GetCurrencyText(amount, money, sizeof(money), ", ");
        char text[3000];
        SStrPrintf(text, sizeof(text), FrameScript_GetText(alone ? "YOU_LOOT_MONEY" : "LOOT_MONEY_SPLIT", -1, GENDER_NOT_APPLICABLE), money);
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(28), NULL);
    }

    return 1;
}

// OFFSET: 0x00554680/0x1401B0820
void CGPlayer_C::ClearLootingObject() {
    this->m_lootTarget = WOWGUID();
    this->UpdateLootAnimKit(0);
}

// OFFSET: 0x0055B920
int32_t CGPlayer_C::OnLootResponse(uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint8_t lootType;
    msg->Get(guid.m_guid);
    msg->Get(lootType);

    if (!this->m_lootTarget || this->m_lootTarget != guid) {
        if (this->m_lootTarget || (lootType != 2 && lootType != 3 && lootType != 4 && lootType != 6 && lootType != 7 && lootType != 8 && lootType != 9)) {
            if (lootType) {
                CDataStore release;
                release.Put(static_cast<uint32_t>(CMSG_LOOT_RELEASE));
                release.Put(guid.m_guid);
                release.Finalize();
                ClientServices::Send2(&release);
            } else if (msg->m_size != msg->m_read) {
                uint8_t error;
                msg->Get(error);

                if (error == 15) {
                    CGGameUI::DisplayError(ERR_ALREADY_PICKPOCKETED);
                }
            }

            this->m_lootTarget = WOWGUID();
            this->UpdateLootAnimKit(0);
            CGGameUI::UnlockItem(guid);
            msg->m_read = msg->m_size;
            return 1;
        }
    }

    if (lootType) {
        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!object) {
            if ((guid.m_guid >> 32 & 0xF0000000) == 0x40000000) {
                CGGameUI::DisplayError(ERR_NO_LOOT);
            } else {
                CGGameUI::DisplayError(ERR_LOOT_DIDNT_KILL);
            }

            CDataStore release;
            release.Put(static_cast<uint32_t>(CMSG_LOOT_RELEASE));
            release.Put(guid.m_guid);
            release.Finalize();
            ClientServices::Send2(&release);
            return 1;
        }

        if (object->m_obj->m_type & TYPEMASK_ITEM) {
            CGGameUI::LockItem(guid);
        }

        this->m_lootTarget = guid;
        CGLootInfo::SetObject(object, msg, lootType);

        if (this->m_movement->m_flags & 0x6000FF) {
            CGGameUI::CloseLoot(true, true, false);
        }

        return 1;
    }

    uint8_t error;
    msg->Get(error);

    switch (error) {
        case 4:
            CGGameUI::DisplayError(ERR_LOOT_TOO_FAR);
            break;
        case 5:
            CGGameUI::DisplayError(ERR_LOOT_BAD_FACING);
            break;
        case 6:
            CGGameUI::DisplayError(ERR_LOOT_LOCKED);
            break;
        case 8:
            CGGameUI::DisplayError(ERR_LOOT_NOTSTANDING);
            break;
        case 9:
            CGGameUI::DisplayError(ERR_LOOT_STUNNED);
            break;
        case 10:
            CGGameUI::DisplayError(ERR_LOOT_PLAYER_NOT_FOUND);
            return 1;
        case 11:
            CGGameUI::DisplayError(ERR_PLAY_TIME_EXCEEDED);
            break;
        case 12:
            CGGameUI::DisplayError(ERR_LOOT_MASTER_INV_FULL);
            return 1;
        case 13:
            CGGameUI::DisplayError(ERR_LOOT_MASTER_UNIQUE_ITEM);
            return 1;
        case 14:
            CGGameUI::DisplayError(ERR_LOOT_MASTER_OTHER);
            return 1;
        case 16:
            CGGameUI::DisplayError(ERR_NOT_WHILE_SHAPESHIFTED);
            return 1;
        case 17:
            CGGameUI::DisplayError(ERR_NO_LOOT);
            return 1;
        default:
            CGGameUI::DisplayError(ERR_LOOT_DIDNT_KILL);
            break;
    }

    this->m_lootTarget = WOWGUID();
    this->UpdateLootAnimKit(0);
    CGGameUI::UnlockItem(guid);
    return 1;
}

// PROVEN: 0ff6f30e IDENTICAL/NONE
// OFFSET: 0x0055BD30
int32_t CGPlayer_C::OnLootRemoved(CDataStore* msg) {
    uint8_t slot;
    msg->Get(slot);

    if (ClntObjMgrObjectPtr(this->m_lootTarget, TYPEMASK_OBJECT, __FILE__, __LINE__)) {
        CGLootInfo::ClearSlot(slot);
    }

    return 1;
}

// PROVEN: 29e9f44a IDENTICAL/NONE
// OFFSET: 0x0055BD80
int32_t CGPlayer_C::OnLootSlotChanged(CDataStore* msg) {
    WOWGUID guid;
    uint8_t slot;
    msg->Get(guid.m_guid);
    msg->Get(slot);

    if (!ClntObjMgrObjectPtr(this->m_lootTarget, TYPEMASK_OBJECT, __FILE__, __LINE__) || this->m_lootTarget != guid) {
        msg->m_read = msg->m_size;
        return 1;
    }

    CGLootInfo::ChangedSlot(slot, msg);
    return 1;
}

// OFFSET: 0x00554610
int32_t CGPlayer_C::OnLootReleaseResponse(CDataStore* msg) {
    WOWGUID guid;
    uint8_t released;
    msg->Get(guid.m_guid);
    msg->Get(released);

    if (released && guid == this->m_lootTarget) {
        this->m_lootTarget = WOWGUID();
        CGGameUI::CloseLoot(false, false, false);
    }

    return 1;
}

// OFFSET: 0x0055FFF0
void CGPlayer_C::LootCorpse(CGCorpse_C* corpse, bool autoInteract) {
    if (!this->CanLoot(corpse) || !corpse->CanBeLooted() || (this->m_movement->m_flags & 0xC010FF)) {
        return;
    }

    if (autoInteract) {
        C3Vector corpsePosition = corpse->GetPosition();
        C3Vector position = this->GetPosition();
        float dz = position.z - corpsePosition.z;
        float dy = position.y - corpsePosition.y;
        float dx = position.x - corpsePosition.x;

        if (dx * dx + (dy * dy + dz * dz) > 25.0f) {
            WOWGUID guid = corpse->m_obj->m_guid;

            if (this->SetTrackingTarget(guid, 6)) {
                return;
            }
        }
    }

    CGGameUI::CloseLoot(true, false, false);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LOOT_UNIT));
    msg.Put(corpse->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    this->m_lootTarget = corpse->m_obj->m_guid;
    this->PlayLootStartAnimKit();
}

// OFFSET: 0x00560160
void CGPlayer_C::LootUnit(CGUnit_C* unit, bool autoInteract) {
    if (!this->CanLoot(unit) || !unit->CanBeLooted(FrameTime::s_curTimeMs)) {
        return;
    }

    if (autoInteract) {
        float range = this->GetMeleeRange(unit);
        C3Vector unitPosition = unit->GetPosition();
        C3Vector position = this->GetPosition();
        float dz = position.z - unitPosition.z;
        float dy = position.y - unitPosition.y;
        float dx = position.x - unitPosition.x;

        if (range * range < dx * dx + (dy * dy + dz * dz)) {
            WOWGUID guid = unit->m_obj->m_guid;

            if (this->SetTrackingTarget(guid, 6)) {
                return;
            }
        }
    }

    if (this->m_movement->m_flags & 0xC010FF) {
        return;
    }

    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && !this->MountedCombatAllowed()) {
        if (!this->CanAutoDismount()) {
            CGGameUI::DisplayError(ERR_NOT_WHILE_MOUNTED);
            return;
        }

        this->Dismount();
    }

    if (this->HasVehicleTransport()) {
        this->CheckVehicleExitOnCast();
    }

    CGGameUI::CloseLoot(true, false, false);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LOOT_UNIT));
    msg.Put(unit->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    this->m_lootTarget = unit->m_obj->m_guid;
    this->PlayLootStartAnimKit();
}

// OFFSET: 0x0055B840/0x1401B87F0
static int32_t InteractUpdateProc(WOWGUID guid, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(param);
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 1;
    }

    uint32_t type = object->m_obj->m_type;

    if (type != (TYPEMASK_UNIT | TYPEMASK_OBJECT) && type != (TYPEMASK_GAMEOBJECT | TYPEMASK_OBJECT)) {
        return 1;
    }

    if (type & TYPEMASK_GAMEOBJECT) {
        if (player->CGUnit_C::CanInteract(static_cast<CGGameObject_C*>(object))) {
            return 1;
        }
    } else {
        if (player->CanInteract(static_cast<CGUnit_C*>(object))) {
            if (static_cast<CGUnit_C*>(object)->m_unit->m_unitNpcFlags & 0x2000) {
                player->UpdateTaxiStatus(static_cast<CGUnit_C*>(object));
            }

            return 1;
        }
    }

    object->m_questStatus = 0;
    object->m_interactIconType = 0;
    object->RemoveInteractIcon();
    return 1;
}

// OFFSET: 0x005618C0/0x1401BF7F0
void SendQuestGiverStatusMultipleQuery() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        ClntObjMgrEnumVisibleObjects(InteractUpdateProc, player);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_STATUS_MULTIPLE_QUERY));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005621C0
bool CGPlayer_C::GetPackAndSlot(const CGItem_C* item, uint8_t* bag, uint8_t* slot) {
    WOWGUID container = item->m_item->m_itemFieldContained;

    if (container == this->m_obj->m_guid) {
        *bag = 0xFF;
        *slot = this->FindSlotIndex(item->m_obj->m_guid);
        return *slot != 0xFF;
    }

    *bag = this->FindSlotIndex(container);

    if (*bag == 0xFF) {
        return false;
    }

    CGContainer_C* object = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(container, TYPEMASK_CONTAINER, __FILE__, __LINE__));

    if (!object) {
        return false;
    }

    int32_t index = object->GetBag()->FindItemSlot(item->m_obj->m_guid);

    if (index < 0) {
        return false;
    }

    *slot = static_cast<uint8_t>(index);
    return true;
}

// OFFSET: 0x00562270
void CGPlayer_C::OpenLootItem(CGItem_C* item) {
    uint8_t bag;
    uint8_t slot;

    if (!this->GetPackAndSlot(item, &bag, &slot)) {
        return;
    }

    CGGameUI::CloseLoot(true, false, false);
    this->m_autoLoot = CGGameUI::IsAutoLooting() != 0;

    if (this->m_autoLoot && this->m_lootTarget) {
        CGLootInfo::DoAutoLoot(0);
    }

    this->m_lootTarget = item->m_obj->m_guid;
    CGGameUI::LockItem(item->m_obj->m_guid);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_OPEN_ITEM));
    msg.Put(bag);
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00558870
void CGPlayer_C::BuyItemInSlot(WOWGUID merchant, const VendorItem* item, uint32_t count, WOWGUID bag, uint8_t slot) {
    if (!item) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BUY_ITEM));
    msg.Put(merchant.m_guid);
    msg.Put(static_cast<uint8_t>(item->m_type));
    msg.Put(static_cast<uint32_t>(item->m_itemID));
    msg.Put(static_cast<uint32_t>(item->m_slot));
    msg.Put(count);
    msg.Put(bag.m_guid);
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055C9C0/0x1401B0BA0
uint8_t CGPlayer_C::FindItemSlot(WOWGUID container, const CGItem_C* item) {
    CGObject_C* object = ClntObjMgrObjectPtr(container, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !object->GetBag()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (!object->GetBag()->m_numSlots) {
        return 0xFF;
    }

    for (uint8_t slot = 0; slot < static_cast<uint32_t>(object->GetBag()->m_numSlots); slot++) {
        if (object->GetBag()->GetItemGuid(slot) == item->m_obj->m_guid.m_guid) {
            return slot;
        }
    }

    return 0xFF;
}

// OFFSET: 0x0056C5A0/0x1401CC5F0
static void AutoEquipItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID none = WOWGUID();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    for (uint32_t i = 0; i < s_itemSwaps.m_count; i++) {
        ITEMSWAP& swap = s_itemSwaps.m_data[i];

        if (swap.m_itemId != id) {
            continue;
        }

        if (stats && player) {
            player->AutoEquipItemToSlot(swap.m_item, swap.m_srcSlot, 0);
        }

        ITEMSWAP& entry = s_itemSwaps.m_data[i];
        entry.m_srcContainer = WOWGUID();
        entry.m_dstContainer = WOWGUID();
        entry.m_srcSlot = 0xFFFFFFFF;
        entry.m_dstSlot = 0xFFFFFFFF;
        entry.m_itemId = 0;
        entry.m_item = NULL;
    }
}

// OFFSET: 0x005607E0
void CGPlayer_C::SwapGuildBankItem(int32_t itemId, uint32_t virtualSlot, uint8_t bag, uint8_t slot, uint32_t count) {
    WOWGUID banker = CGGuildBankInfo::m_banker;
    uint8_t tab = virtualSlot / 98;
    uint8_t tabSlot = virtualSlot % 98;
    CDataStore msg;

    if (CGGuildInfo::GetNumGuildBankTabItemWithdrawalsByRankID(tab, this->m_player->m_playerGuildrank)) {
        msg.Put(static_cast<uint32_t>(CMSG_GUILD_BANK_SWAP_ITEMS));
        msg.Put(banker.m_guid);
        msg.Put(static_cast<uint8_t>(0));
        msg.Put(tab);
        msg.Put(tabSlot);
        msg.Put(static_cast<uint32_t>(itemId));
        msg.Put(static_cast<uint8_t>(0));
        msg.Put(bag);
        msg.Put(slot);
        msg.Put(static_cast<uint8_t>(1));
        msg.Put(count);
        msg.Finalize();
        ClientServices::Send2(&msg);
    } else {
        CGGameUI::DisplayError(ERR_GUILD_PERMISSIONS);
    }
}

// OFFSET: 0x00560920
void CGPlayer_C::SplitItem(WOWGUID item, WOWGUID srcContainer, uint32_t srcSlot, WOWGUID dstContainer, uint32_t dstSlot, uint32_t count) {
    if (srcSlot > 255 || dstSlot > 255) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (static_cast<int32_t>(count) <= 0) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CDataStore msg;

    if (!srcContainer) {
        return;
    }

    uint8_t srcBag = this->FindSlotIndex(srcContainer);
    uint8_t dstBag = this->FindSlotIndex(dstContainer);
    msg.Put(static_cast<uint32_t>(CMSG_SPLIT_ITEM));
    msg.Put(srcBag);
    msg.Put(static_cast<uint8_t>(srcSlot));
    msg.Put(dstBag);
    msg.Put(static_cast<uint8_t>(dstSlot));
    msg.Put(count);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0056C4D0/0x1401CC4D0
static void SwapItemsStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID none = WOWGUID();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, none, NULL, NULL, false);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    for (uint32_t i = 0; i < s_itemSwaps.m_count; i++) {
        ITEMSWAP& swap = s_itemSwaps.m_data[i];

        if (swap.m_itemId != id) {
            continue;
        }

        if (stats && player) {
            player->SwapItems(WOWGUID(), swap.m_srcContainer, swap.m_srcSlot, swap.m_dstContainer, swap.m_dstSlot, 0);
        }

        swap.m_srcContainer = WOWGUID();
        swap.m_dstContainer = WOWGUID();
        swap.m_srcSlot = 0xFFFFFFFF;
        swap.m_dstSlot = 0xFFFFFFFF;
        swap.m_itemId = 0;
        swap.m_item = 0;
    }
}

// OFFSET: 0x00562460
void CGPlayer_C::GiftWrap(CGItem_C* item) {
    CGItem_C* wrap = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(s_giftWrapItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!wrap || !item || (item->m_itemFlags & 1)) {
        return;
    }

    WOWGUID wrapContainer = wrap->m_item->m_itemFieldContained;
    uint8_t wrapBag = this->FindSlotIndex(wrapContainer);
    uint8_t wrapSlot = this->FindItemSlot(wrapContainer, wrap);
    WOWGUID itemContainer = item->m_item->m_itemFieldContained;
    uint8_t itemBag = this->FindSlotIndex(itemContainer);
    uint8_t itemSlot = this->FindItemSlot(itemContainer, item);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_WRAP_ITEM));
    msg.Put(wrapBag);
    msg.Put(wrapSlot);
    msg.Put(itemBag);
    msg.Put(itemSlot);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPlayer_C::CancelGiftWrap();
}

// OFFSET: 0x0056C660/0x1401CC6F0
void CGPlayer_C::SwapItems(WOWGUID item, WOWGUID srcContainer, uint32_t srcSlot, WOWGUID dstContainer, uint32_t dstSlot, int32_t ignoreBind) {
    if (srcSlot > 255 || dstSlot > 255) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (!srcContainer) {
        return;
    }

    WOWGUID self = this->m_obj->m_guid;

    if ((srcContainer == self || dstContainer == self) && !CGGameUI::CanPerformAction(22)) {
        return;
    }

    CDataStore msg;
    bool srcEquipped = srcContainer == self && (srcSlot < 23 || srcSlot - 67 <= 6);
    bool dstEquipped = dstContainer == self && (dstSlot < 23 || dstSlot - 67 <= 6);

    if (srcEquipped != dstEquipped) {
        WOWGUID container = srcEquipped ? dstContainer : srcContainer;
        uint32_t slot = srcEquipped ? dstSlot : srcSlot;
        CGObject_C* object = ClntObjMgrObjectPtr(container, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!object || !object->GetBag()) {
            return;
        }

        WOWGUID itemGuid;
        itemGuid.m_guid = object->GetBag()->GetItemGuid(slot);
        CGItem_C* bagItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (bagItem && !bagItem->IsBound() && !ignoreBind) {
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(bagItem->m_obj->m_entryID, bagItem->m_obj->m_guid, SwapItemsStatsCallback, NULL, false);

            if (!stats) {
                uint32_t index = FindEmptySwapIndex();
                s_itemSwaps.m_data[index].m_srcContainer = srcContainer;
                s_itemSwaps.m_data[index].m_srcSlot = srcSlot;
                s_itemSwaps.m_data[index].m_dstContainer = dstContainer;
                s_itemSwaps.m_data[index].m_dstSlot = dstSlot;
                s_itemSwaps.m_data[index].m_itemId = bagItem->m_obj->m_entryID;
                return;
            }

            if (stats->m_bonding == 2 && this->CanUseItem(stats, ignoreBind)) {
                uint32_t index = FindEmptySwapIndex();
                s_itemSwaps.m_data[index].m_srcContainer = srcContainer;
                s_itemSwaps.m_data[index].m_srcSlot = srcSlot;
                s_itemSwaps.m_data[index].m_dstContainer = dstContainer;
                s_itemSwaps.m_data[index].m_dstSlot = dstSlot;
                s_itemSwaps.m_data[index].m_itemId = bagItem->m_obj->m_entryID;
                FrameScript_SignalEvent(SCRIPT_EVENT_EQUIP_BIND_CONFIRM, "%d", index);
                return;
            }
        }
    }

    if (srcContainer == dstContainer && srcContainer == this->m_obj->m_guid) {
        msg.Put(static_cast<uint32_t>(CMSG_SWAP_INV_ITEM));
        msg.Put(static_cast<uint8_t>(dstSlot));
        msg.Put(static_cast<uint8_t>(srcSlot));
    } else {
        uint8_t srcBag = this->FindSlotIndex(srcContainer);
        uint8_t dstBag = this->FindSlotIndex(dstContainer);

        if (dstBag == 0xFF && dstContainer != this->m_obj->m_guid) {
            return;
        }

        msg.Put(static_cast<uint32_t>(CMSG_SWAP_ITEM));
        msg.Put(dstBag);
        msg.Put(static_cast<uint8_t>(dstSlot));
        msg.Put(srcBag);
        msg.Put(static_cast<uint8_t>(srcSlot));
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::ClearCursor(0, 1);
}

// OFFSET: 0x0055B150/0x1401B7ED0
int32_t CGPlayer_C::ValidateSlot(uint32_t slot, WOWGUID item) {
    CGItem_C* itemObject = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!itemObject) {
        return 0;
    }

    if ((itemObject->m_obj->m_type & TYPEMASK_CONTAINER) && (slot < 19 || slot > 22) && (slot < 67 || slot > 73) && itemObject->GetBag()->GetNumItems()) {
        return 0;
    }

    ChrClassesRec* classRec = g_chrClassesDB.GetRecord(this->m_unit->m_unitFieldBytes0[1]);

    if (classRec && (classRec->m_flags & 0x8)) {
        return itemObject->CanGoInSlot(slot, 1);
    }

    return itemObject->CanGoInSlot(slot, 0);
}

// OFFSET: 0x005609F0
void CGPlayer_C::AutoStoreItemInBag(WOWGUID item, WOWGUID srcContainer, uint32_t srcSlot, WOWGUID dstBag, int32_t a5) {
    if (srcSlot > 255) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CDataStore msg;

    if (srcContainer) {
        uint8_t srcBag = this->FindSlotIndex(srcContainer);
        uint8_t dstBagIndex = this->FindSlotIndex(dstBag);

        if (dstBagIndex == 0xFF && dstBag != this->m_obj->m_guid) {
            return;
        }

        msg.Put(static_cast<uint32_t>(CMSG_AUTO_STORE_BAG_ITEM));
        msg.Put(srcBag);
        msg.Put(static_cast<uint8_t>(srcSlot));
        msg.Put(dstBagIndex);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0056CA00/0x1401CCB70
static void AutoEquipStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = WOWGUID();

    if (!g_itemSparseDB.GetRecord(id, none, NULL, NULL, false)) {
        return;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && static_cast<int32_t>(item->m_obj->m_entryID) == id) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player) {
            player->AutoEquipCursorItem(0);
        }
    }
}

// OFFSET: 0x0056CA80/0x1401CCC10
void CGPlayer_C::AutoEquipCursorItem(int32_t ignoreBind) {
    if (!CGGameUI::CanPerformAction(22)) {
        return;
    }

    if (CGGameUI::m_cursorItemType == 9) {
        int32_t ammo = CGGameUI::GetCursorVirtualItem();

        if (ammo) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_SET_AMMO));
            msg.Put(static_cast<uint32_t>(ammo));
            msg.Finalize();
            ClientServices::Send2(&msg);
            CGGameUI::ClearCursor(1, 1);
        }

        return;
    }

    WOWGUID item;
    WOWGUID container;
    uint32_t slot;
    CGGameUI::GetCursorItem(item, container, slot);

    if (!item) {
        return;
    }

    if (container == this->m_obj->m_guid && slot < 23) {
        CGGameUI::ClearCursor(1, 1);
        return;
    }

    int32_t clearMode = 0;
    CDataStore msg;

    if (container) {
        CGItem_C* itemObject = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!itemObject) {
            return;
        }

        uint8_t bag = this->FindSlotIndex(container);

        if (!itemObject->IsBound() && !ignoreBind) {
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemObject->m_obj->m_entryID, item, AutoEquipStatsCallback, NULL, false);

            if (!stats) {
                return;
            }

            if (stats->m_bonding == 2 && this->CanUseItem(stats, ignoreBind)) {
                uint32_t index = FindEmptySwapIndex();
                s_itemSwaps.m_data[index].m_srcContainer = container;
                s_itemSwaps.m_data[index].m_srcSlot = slot;
                s_itemSwaps.m_data[index].m_dstContainer = WOWGUID();
                s_itemSwaps.m_data[index].m_dstSlot = 0;
                s_itemSwaps.m_data[index].m_itemId = itemObject->m_obj->m_entryID;
                FrameScript_SignalEvent(SCRIPT_EVENT_AUTOEQUIP_BIND_CONFIRM, "%d", index);
                return;
            }
        }

        if (itemObject->GetClassID() == 6) {
            clearMode = 1;
            msg.Put(static_cast<uint32_t>(CMSG_SET_AMMO));
            msg.Put(static_cast<uint32_t>(itemObject->m_obj->m_entryID));
        } else {
            CGGameUI::LockItem(itemObject->m_obj->m_guid);
            msg.Put(static_cast<uint32_t>(CMSG_AUTO_EQUIP_ITEM));
            msg.Put(bag);
            msg.Put(static_cast<uint8_t>(slot));
        }
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::ClearCursor(clearMode, 1);
}

// OFFSET: 0x0055CF60
void CGPlayer_C::UpdateObjComponentVisualsSlot(const VisibleItem_C* item, int32_t slot) {
    int32_t hand;

    if (slot == 15) {
        hand = 0;
    } else if (slot == 16) {
        hand = 1;
    } else {
        return;
    }

    const CGUnitVirtualItem* virtualItem = this->GetVirtualItem(hand, false);

    if (!virtualItem) {
        return;
    }

    SHEATHE_TYPE sheatheType = static_cast<SHEATHE_TYPE>(virtualItem->m_sheatheType);

    if (this->m_sheatheState) {
        if (slot == 15) {
            this->UpdateObjComponentVisuals(item, CCharacterComponent::GetSheatheLink(sheatheType, true));
            this->UpdateObjComponentVisuals(item, static_cast<GEOCOMPONENTLINKS>(1));
        } else if (const_cast<VisibleItem_C*>(item)->GetInventoryType() == 14) {
            this->UpdateObjComponentVisuals(item, static_cast<GEOCOMPONENTLINKS>(0));
        } else {
            this->UpdateObjComponentVisuals(item, CCharacterComponent::GetSheatheLink(sheatheType, false));
            this->UpdateObjComponentVisuals(item, static_cast<GEOCOMPONENTLINKS>(2));
        }
    } else {
        GEOCOMPONENTLINKS link = CCharacterComponent::GetSheatheLink(sheatheType, slot == 15);
        this->UpdateObjComponentVisuals(item, static_cast<GEOCOMPONENTLINKS>((slot != 15) + 1));
        this->UpdateObjComponentVisuals(item, link);
    }
}

// OFFSET: 0x0056CCA0/0x1401CCE90
void CGPlayer_C::AutoEquipItem(WOWGUID container, uint32_t slot, int32_t a3) {
    CGObject_C* object = ClntObjMgrObjectPtr(container, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !object->GetBag()) {
        return;
    }

    CGBag_C* bag = object->GetBag();

    if (slot >= static_cast<uint32_t>(bag->m_numSlots)) {
        return;
    }

    WOWGUID item = bag->m_slots[slot];

    if (!item) {
        return;
    }

    if (CGTradeInfo::IsItemBeingTraded(item)) {
        CGGameUI::DisplayError(ERR_EQUIP_TRADE_ITEM);
        return;
    }

    CGGameUI::SetCursorItem(item, container, slot, 1, 0, 0);
    this->AutoEquipCursorItem(a3);
}

// OFFSET: 0x00562130
void CGPlayer_C::GetRangedAttackSkillRank(int32_t& rank, int32_t& bonus) {
    bonus = 0;
    rank = 0;
    int32_t spellId = 0;
    const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

    if (!item || item->m_classID == 2) {
        uint32_t subclass = item ? item->m_subclassID : ClientDBGetUnarmedWeapon();

        if (subclass < s_weaponSkills.m_count) {
            spellId = s_weaponSkills.m_data[subclass];
        }
    }

    SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], spellId);

    if (ability && ability->m_spell == spellId) {
        this->GetExpandedSkillRank(ability->m_skillLine, rank, bonus);
    }

    bonus += static_cast<int32_t>(this->GetCombatRatingPct(0) - 0.5f);
    bonus += static_cast<int32_t>(this->GetCombatRatingPct(22) - 0.5f);
}

// OFFSET: 0x005665D0/0x1401C4D90
void CGPlayer_C::ShowHelm() {
    VisibleItem_C* head = &this->m_player->m_playerVisibleItems[0];

    if (head && head->GetDisplayID() && this->CanEquipItemsInThisForm() && this->m_characterComponent) {
        this->m_characterComponent->AddItemBySlot(static_cast<INVENTORY_SLOTS>(0), head->GetDisplayID(), 0);
    }

    if ((this->m_player->m_playerFlags & 0x400) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SHOWING_HELM));
        msg.Put(static_cast<uint8_t>(1));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    this->ApplyComponentItemsFromEffects();
}

// OFFSET: 0x005666C0/0x1401C4EB0
void CGPlayer_C::ShowCloak() {
    VisibleItem_C* back = &this->m_player->m_playerVisibleItems[14];

    if (back && back->GetDisplayID() && this->CanEquipItemsInThisForm() && this->m_characterComponent) {
        this->m_characterComponent->AddItemBySlot(static_cast<INVENTORY_SLOTS>(14), back->GetDisplayID(), 0);
    }

    if ((this->m_player->m_playerFlags & 0x800) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SHOWING_CLOAK));
        msg.Put(static_cast<uint8_t>(1));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// OFFSET: 0x00566AE0
void CGPlayer_C::UpdateHandItemVisual(GEOCOMPONENTLINKS link, int32_t hand) {
    uint32_t slot;

    if (hand == 0) {
        slot = 15;
    } else if (hand == 1) {
        slot = 16;
    } else {
        return;
    }

    VisibleItem_C* item = &this->m_player->m_playerVisibleItems[slot];

    if (item && abs(item->m_itemId)) {
        this->UpdateObjComponentVisuals(item, link);
    }
}

// INLINED
int32_t CGItem::GetEnchantmentCharges(int32_t slot) {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return 0;
    }

    return this->m_item->m_itemFieldEnchantment[slot].m_charges;
}

// OFFSET: 0x005636C0/0x1401C1440
float CGPlayer_C::GetCombatRatingPct(int32_t rating) {
    float base = GameFormulaEvaluateTable(1, this->m_unit->m_unitFieldLevel - 1, rating);
    float scalar = GameFormulaEvaluateTable(11, rating + 1, this->m_unit->m_unitFieldBytes0[1] - 1);

    if (base == 0.0f || scalar == 0.0f) {
        return 0.0f;
    }

    return scalar * (this->m_player->m_playerFieldCombatRating1[rating] / base);
}

// OFFSET: 0x0056DBD0
int32_t InventoryUpdate(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    uint32_t slot = (offset - offsetof(CGPlayerLocalData, m_playerFieldInvSlotHead)) / sizeof(WOWGUID);
    WOWGUID item;
    item.m_guid = player->m_bag.GetItemGuid(slot);

    if (*static_cast<const WOWGUID*>(value) != item) {
        CGGameUI::UnlockItem(item);
    }

    if (slot == 15 || slot == 16) {
        CGActionBar::UpdateAttackAction();
    } else if (slot == 17) {
        CGActionBar::UpdateRangedAttackAction();
    }

    if (slot <= 18) {
        player->CountEquippedGems();

        if (item) {
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_EQUIPMENT_CHANGED, "%d%d", slot + 1, 1);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_EQUIPMENT_CHANGED, "%d", slot + 1);
        }

        return 1;
    }

    if ((slot >= 39 && slot <= 66) || slot - 67 <= 6) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYERBANKSLOTS_CHANGED, "%d", slot - 38);
    }

    return 1;
}

// PROVEN: 18d55527 IDENTICAL/NONE
// OFFSET: 0x00557070/0x1401B3E60
void PlayerNameGuildCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateUnitNameText();
    }
}

// OFFSET: 0x005614D0/0x1401BF380
int32_t CGPlayer_C::GetSkillIndex(int32_t skillId) const {
    uint32_t index = 0;

    for (; index < 128; index++) {
        uint16_t line = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillLineid0[index] : 0;

        if (line == skillId) {
            break;
        }
    }

    return index == 128 ? -1 : index;
}

// OFFSET: 0x0055E220
uint16_t CGPlayer_C::GetSkillLineID(int32_t index) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerSkillLineid0[index];
}

// INLINED
uint16_t CGPlayer_C::GetSkillStep(int32_t index) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerSkillStep0[index];
}

// OFFSET: 0x00561540/0x1401BF410
bool CGPlayer_C::GetExpandedSkillRank(int32_t skillId, int32_t& rank, int32_t& bonus) const {
    int32_t index = this->GetSkillIndex(skillId);

    if (index < 0) {
        return false;
    }

    rank = this->GetSkillValue(index);

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        bonus = 0;
        return true;
    }

    bonus = static_cast<int16_t>(this->m_player->m_playerSkillModifier0[index]);
    return true;
}

// OFFSET: 0x00557D40/0x1401B4D80
void CGPlayer_C::ApplyGuildTabard(int32_t emblemStyle, int32_t emblemColor, int32_t borderStyle, int32_t borderColor, int32_t background) {
    if (this->m_characterComponent) {
        this->m_characterComponent->ApplyGuildColor(1, emblemStyle, emblemColor, borderStyle, borderColor, background);
        CImVector backgroundColor(0);
        CImVector borderColorValue(0);
        CImVector emblemColorValue(0);
        char fileName[64];
        CGObject_C::GetGuildLogoInfo(background, borderColor, emblemColor, emblemStyle, backgroundColor, borderColorValue, emblemColorValue, fileName, sizeof(fileName));
        CGObject_C::ReplaceGuildTextures(this->m_characterComponent->m_data.m_model, backgroundColor, borderColorValue, emblemColorValue, fileName);
        WOWGUID guid = this->m_obj->m_guid;
        CGGameUI::UnitModelUpdate(guid, 3);
    }

    if (this->m_displayTransition) {
        this->m_displayTransition->ApplyGuildColor(1, emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// PROVEN: c1904e2d IDENTICAL/NONE
// OFFSET: 0x0055E260/0x1401BB7A0
int16_t CGPlayer_C::GetSkillModifier(int32_t index) const {
    WOWGUID guid = this->m_obj->m_guid;

    if (guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return this->m_player->m_playerSkillModifier0[index];
}

// OFFSET: 0x0090FD00/0x14066B450
uint32_t CGPlayer_C::GetSkillMaxRank(int32_t index) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    uint32_t value = this->m_player->m_playerSkillMaxRank0[index];

    if (value) {
        value += this->m_player->m_playerSkillTalent0[index];
    }

    return value;
}

// OFFSET: 0x00468FF0/0x14007D170
uint32_t CGPlayer_C::GetSkillValue(int32_t index) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    uint32_t value = this->m_player->m_playerSkillRank0[index];

    if (value) {
        value += this->m_player->m_playerSkillTalent0[index];
    }

    return value;
}

// OFFSET: 0x00553E40/0x1401AEF20
static bool AreaTriggerContains(const AreaTriggerRec* rec, const C3Vector& position) {
    if (rec->m_continentID != s_areaTriggerMapID) {
        return false;
    }

    if (rec->m_radius == 0.0f) {
        CAaBox box;
        box.t.x = rec->m_boxLength * 0.5f;
        box.b.x = -box.t.x;
        box.t.y = rec->m_boxWidth * 0.5f;
        box.b.y = -box.t.y;
        box.t.z = rec->m_boxHeight * 0.5f;
        box.b.z = -box.t.z;

        C44Matrix matrix;
        matrix.Translate(*reinterpret_cast<const C3Vector*>(rec->m_pos));
        matrix.RotateAroundZ(rec->m_boxYaw);
        C3Vector local = position * matrix.AffineInverse();

        return box.ContainsPointExclusive(local);
    }

    float dx = rec->m_pos[0] - position.x;
    float dy = rec->m_pos[1] - position.y;
    float dz = rec->m_pos[2] - position.z;

    return dy * dy + dz * dz + dx * dx <= rec->m_radius * rec->m_radius;
}

// OFFSET: 0x00558940/0x1401B5CC0
void AreaTriggersInitialize(uint32_t mapId) {
    int32_t start = -1;
    int32_t i = 0;
    s_currentAreaTrigger = NULL;
    s_areaTriggerMapID = mapId;
    s_areaTriggerStart = -1;

    for (; i < g_areaTriggerDB.m_numRecords; i++) {
        AreaTriggerRec* rec = g_areaTriggerDB.GetRecordByIndex(i);

        if (!rec) {
            s_areaTriggerStart = start;
            SErrDisplayAppFatalMessage("", "", 0);
        }

        if (rec->m_continentID == static_cast<int32_t>(mapId)) {
            if (start == -1) {
                start = i;
            }
        } else if (rec->m_continentID > static_cast<int32_t>(mapId)) {
            break;
        }
    }

    s_areaTriggerStart = start;
    s_areaTriggerEnd = i;
    s_areaTriggerTimer = ClientSetTimer(100, AreaTriggerCheck, NULL);
}

// OFFSET: 0x00560CF0/0x1401BEB10
int32_t AreaTriggerCheck(const void* data, void* param) {
    s_areaTriggerTimer = ClientSetTimer(100, AreaTriggerCheck, NULL);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_unit->m_unitFieldHealth <= 0) {
        return 1;
    }

    if (ClntObjMgrGetMapID() != s_areaTriggerMapID) {
        if (s_areaTriggerTimer) {
            ClientKillTimer(s_areaTriggerTimer, AreaTriggerCheck, "AreaTriggerCheck");
            s_areaTriggerTimer = 0;
        }

        AreaTriggersInitialize(ClntObjMgrGetMapID());
    }

    if (s_areaTriggerStart == -1) {
        return 1;
    }

    C3Vector position = player->GetPosition();

    if (s_currentAreaTrigger) {
        if (AreaTriggerContains(s_currentAreaTrigger, position)) {
            return 1;
        }

        s_currentAreaTrigger = NULL;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 1;
    }

    for (int32_t i = s_areaTriggerStart; i < s_areaTriggerEnd; i++) {
        AreaTriggerRec* rec = g_areaTriggerDB.GetRecordByIndex(i);

        if (!rec) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        if (AreaTriggerContains(rec, position)) {
            mover->SendMovementHeartbeat();

            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_AREA_TRIGGER));
            msg.Put(static_cast<uint32_t>(rec->m_id));
            msg.Finalize();
            ClientServices::Send2(&msg);
            s_currentAreaTrigger = rec;

            return 1;
        }
    }

    return 1;
}

// PROVEN: 36c36b78 IDENTICAL/NONE
// OFFSET: 0x00560F40
void AreaTriggerCheckNow() {
    AreaTriggerCheck(NULL, NULL);
}

// OFFSET: 0x0055DC30/0x1401BB1A0
int32_t __stdcall CGPlayer_C::CanSummonFriend(WOWGUID guid) {
    if (!guid) {
        return 8;
    }

    if (!CGPlayer_C::IsRAFPlayer(guid)) {
        return 1;
    }

    if (!CGPartyInfo::IsMember(guid) && !IsGuidInMyRaid(guid)) {
        return 9;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (static_cast<int32_t>(player->m_unit->m_unitFieldLevel) > 60) {
            return 10;
        }
    } else {
        PartyMemberState* stats = CGPartyInfo::GetRemoteStats(guid);

        if (!stats) {
            stats = CGRaidInfo::GetRemoteStats(guid);

            if (!stats) {
                return 9;
            }
        }

        if (!(stats->m_flags & 0x1)) {
            return 13;
        }

        if (stats->m_level > 60) {
            return 10;
        }
    }

    return Spell_C_GetSpellCooldown(CGSpellBook::m_summonFriendSpell, 0, NULL, NULL, NULL) ? 11 : 0;
}

// OFFSET: 0x0055DD20
int32_t CGPlayer_C::UpdateMountedCollisionBox(float mountHeight, int32_t force) {
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldNativedisplayid);
    CreatureModelDataRec* modelData = displayInfo ? g_creatureModelDataDB.GetRecord(displayInfo->m_modelID) : NULL;

    if (!modelData) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    float modelScale;
    float scale = this->GetScaledModelScale(displayInfo, modelData, &modelScale);
    float height = this->m_mountScale * mountHeight + modelData->m_collisionHeight * 0.5f;

    if (height <= modelData->m_collisionHeight) {
        height = modelData->m_collisionHeight;
    } else if (!force && this->IsActivePlayer()) {
        C3Vector position = this->GetPosition();
        CAaBox box(position);
        box.b.z = this->m_movementData.m_collisionHeight + box.b.z;
        box.t.z = height * scale + box.t.z;
        box.b.x = box.b.x - this->m_movementData.m_collisionRadius;
        box.t.x = box.t.x + this->m_movementData.m_collisionRadius;
        box.b.y = box.b.y - this->m_movementData.m_collisionRadius;
        box.t.y = box.t.y + this->m_movementData.m_collisionRadius;

        if (this->m_movementData.CollisionBoxDoesntFit(box)) {
            return 0;
        }
    }

    this->m_movementData.SetCollisionBox(this->m_movementData.m_collisionRadius + this->m_movementData.m_collisionRadius, height, scale, modelScale, force);

    return 1;
}

// OFFSET: 0x0055DF10/0x1401BB4C0
int32_t CGPlayer_C::UpdateUnitCollisionBox(int32_t a2, int32_t a3) {
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldNativedisplayid);
    CreatureModelDataRec* modelData = displayInfo ? g_creatureModelDataDB.GetRecord(displayInfo->m_modelID) : NULL;

    if (!modelData) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    float modelScale;
    float scale = this->GetScaledModelScale(displayInfo, modelData, &modelScale);

    if (fabsf(modelData->m_collisionWidth) < 0.00000095367432f || fabsf(modelData->m_collisionHeight) < 0.00000095367432f) {
        SysMsgPrintf(STATUS_ERROR, 0x10, "NOCOLLISIONBOX|%s", static_cast<const char*>(modelData->m_modelName));
        return 1;
    }

    if (CMath::fequal(scale, 0.0f)) {
        SysMsgPrintf(STATUS_ERROR, 2, "ZEROSCALEUNIT|%d|%s", this->m_obj->m_entryID, this->GetUnitName(NULL, 1));
        return 1;
    }

    if (modelData->m_collisionWidth + 0.00000011920929f < 0.4167f) {
        SysMsgPrintf(STATUS_ERROR, 2, "CreatureModelData record %d (%s) collision width is too small.", displayInfo->m_modelID, static_cast<const char*>(modelData->m_modelName));
    }

    if (!a2 && this->IsActivePlayer() && this->m_movementData.CollisionBoxDoesntFit(modelData->m_collisionWidth, modelData->m_collisionHeight, scale)) {
        return 0;
    }

    this->m_movementData.SetCollisionBox(modelData->m_collisionWidth, modelData->m_collisionHeight, scale, modelScale, a3);
    return 1;
}

// OFFSET: 0x005582C0
bool CGPlayer_C::OnGuildChanged(int32_t a2) {
    this->UpdateUnitNameText();

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TABARD_CANSAVE_CHANGED, NULL);
    }

    WOWGUID guid = this->m_obj->m_guid;
    Script_SendUnitSignal(guid, SCRIPT_EVENT_PLAYER_GUILD_UPDATE);

    if (a2 && ClntObjMgrGetActivePlayer() == this->m_obj->m_guid) {
        CGCalendar::GuildChanged();
    }

    this->LoadGuildTextures(this->GetGuildGUID().m_guid);
    return true;
}

// OFFSET: 0x00567DA0
TSGrowableArray<int32_t>* CGPlayer_C::GetTradeSkills(int32_t skillLine) {
    TRADESKILLLINE* line = this->m_tradeSkillLines.Ptr(skillLine, HASHKEY_NONE());

    return line ? &line->m_spells : NULL;
}

// INLINED
uint32_t GetVisibleAuraEffectDisplay() {
    return s_unk_C9EAB0;
}

// INLINED
uint32_t GetVisibleAuraEffectFlags() {
    return s_unk_C9EAB4;
}

// PROVEN: 07b73796 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void PlayerShutdownSounds() {
}

// OFFSET: 0x0056CD50/0x1401CCF60
void CGPlayer_C::AutoEquipItemToSlot(CGItem_C* item, uint32_t slot, int32_t ignoreBind) {
    if (!item) {
        return;
    }

    WOWGUID guid = item->m_obj->m_guid;

    if (CGTradeInfo::IsItemBeingTraded(guid)) {
        CGGameUI::DisplayError(ERR_EQUIP_TRADE_ITEM);
        return;
    }

    if (!CGGameUI::CanPerformAction(22)) {
        return;
    }

    CDataStore msg;

    if (!item->IsBound() && !ignoreBind) {
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, guid, &AutoEquipItemCallback, NULL, false);

        if (!stats) {
            uint32_t index = FindEmptySwapIndex();
            s_itemSwaps.m_data[index].m_item = item;
            s_itemSwaps.m_data[index].m_srcSlot = slot;
            s_itemSwaps.m_data[index].m_srcContainer = item->m_item->m_itemFieldContained;
            s_itemSwaps.m_data[index].m_itemId = item->m_obj->m_entryID;
            return;
        }

        int32_t error;

        if (stats->m_bonding == 2 && this->CanUseItem(stats, error)) {
            uint32_t index = FindEmptySwapIndex();
            s_itemSwaps.m_data[index].m_item = item;
            s_itemSwaps.m_data[index].m_srcSlot = slot;
            s_itemSwaps.m_data[index].m_srcContainer = item->m_item->m_itemFieldContained;
            s_itemSwaps.m_data[index].m_itemId = item->m_obj->m_entryID;
            FrameScript_SignalEvent(SCRIPT_EVENT_EQUIP_BIND_CONFIRM, "%d", index);
            return;
        }
    }

    CGGameUI::LockItem(guid);
    msg.Put(static_cast<uint32_t>(CMSG_AUTO_EQUIP_ITEM_SLOT));
    msg.Put(guid.m_guid);
    msg.Put(static_cast<uint8_t>(slot));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0056CF60/0x1401CD1B0
void CGPlayer_C::ClearPendingEquip(uint32_t index, int32_t equip) {
    if (index >= s_itemSwaps.m_count) {
        return;
    }

    ITEMSWAP* swap = &s_itemSwaps.m_data[index];

    if (equip) {
        if (swap->m_item) {
            this->AutoEquipItemToSlot(swap->m_item, swap->m_srcSlot, 1);
        } else if (swap->m_dstContainer) {
            this->SwapItems(WOWGUID(), swap->m_srcContainer, swap->m_srcSlot, swap->m_dstContainer, swap->m_dstSlot, 1);
        } else {
            this->AutoEquipItem(swap->m_srcContainer, swap->m_srcSlot, 1);
        }
    } else {
        CGObject_C* source = ClntObjMgrObjectPtr(swap->m_srcContainer, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (source && source->GetBag()) {
            WOWGUID item;
            item.m_guid = source->GetBag()->GetItemGuid(swap->m_srcSlot);
            CGGameUI::UnlockItem(item);
        }

        CGObject_C* destination = ClntObjMgrObjectPtr(swap->m_dstContainer, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (destination && destination->GetBag()) {
            WOWGUID item;
            item.m_guid = destination->GetBag()->GetItemGuid(swap->m_dstSlot);
            CGGameUI::UnlockItem(item);
        }
    }

    swap = &s_itemSwaps.m_data[index];
    swap->m_srcContainer = WOWGUID();
    swap->m_dstContainer = WOWGUID();
    swap->m_itemId = 0;
    swap->m_item = NULL;
    swap->m_srcSlot = 0xFFFFFFFF;
    swap->m_dstSlot = 0xFFFFFFFF;
}

// OFFSET: 0x00565CF0/0x1401C3F20
void PlayerClientShutdown() {
    CGPlayer_C::PlayerShutdownMessageHandlers();
    PlayerShutdownSounds();

    if (s_areaTriggerTimer) {
        ClientKillTimer(s_areaTriggerTimer, AreaTriggerCheck, "AreaTriggerCheck");
        s_areaTriggerTimer = 0;
    }
}

// OFFSET: 0x0055A000/0x1401AF5C0
void CGPlayer_C::PlayerShutdownMessageHandlers() {
    ClientGuildSendGuildXP::s_handler = NULL;
    ClientGuildSendMaxDailyXP::s_handler = NULL;
    ClientGuildSendRankChange::s_handler = NULL;
    ClientGuildCommandResult::s_handler = NULL;
    ClientGuildRoster::s_handler = NULL;
    ClientBattlegroundInfoThrottled::s_handler = NULL;
    ClientGuildMemberRecipes::s_handler = NULL;
    ClientGuildKnownRecipes::s_handler = NULL;
    ClientGuildMembersWithRecipe::s_handler = NULL;
    ClientCategoryCooldown::s_handler = NULL;
    ClientSummonRaidMemberValidateFailed::s_handler = NULL;
    ClientQuestNpcQueryResponse::s_handler = NULL;
    ClientRequestCemeteryListResponse::s_handler = NULL;
    ClientArenaTeamCommandResult::s_handler = NULL;
    ClientPetitionAlreadySigned::s_handler = NULL;
    ClientStartTimer::s_handler = NULL;
    ClientDebugRuneRegen::s_handler = NULL;
    ClientSpellinterruptlog::s_handler = NULL;
    ClientXpGainAborted::s_handler = NULL;
    ClientChatIgnoredAccountMuted::s_handler = NULL;
    ClientSorStartExperienceIncomplete::s_handler = NULL;
    ClientServices::ClearMessageHandler(SMSG_MOUNT_RESULT);
    ClientServices::ClearMessageHandler(SMSG_DISMOUNT_RESULT);
    ClientServices::ClearMessageHandler(SMSG_INVENTORY_CHANGE_FAILURE);
    ClientServices::ClearMessageHandler(SMSG_OPEN_CONTAINER);
    ClientServices::ClearMessageHandler(SMSG_ITEM_PUSH_RESULT);
    ClientServices::ClearMessageHandler(SMSG_BUY_FAILED);
    ClientServices::ClearMessageHandler(SMSG_BUY_SUCCEEDED);
    ClientServices::ClearMessageHandler(SMSG_SELL_ITEM);
    ClientServices::ClearMessageHandler(SMSG_LOOT_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_LOOT_RELEASE);
    ClientServices::ClearMessageHandler(SMSG_LOOT_REMOVED);
    ClientServices::ClearMessageHandler(SMSG_LOOT_SLOT_CHANGED);
    ClientServices::ClearMessageHandler(SMSG_LOOT_MONEY_NOTIFY);
    ClientServices::ClearMessageHandler(SMSG_LOOT_ITEM_NOTIFY);
    ClientServices::ClearMessageHandler(SMSG_LOOT_CLEAR_MONEY);
    ClientServices::ClearMessageHandler(SMSG_UNLEARNED_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_SEND_KNOWN_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_GROUP_CANCEL);
    ClientServices::ClearMessageHandler(SMSG_GROUP_DECLINE);
    ClientServices::ClearMessageHandler(SMSG_GROUP_UNINVITE);
    ClientServices::ClearMessageHandler(SMSG_GROUP_SET_LEADER);
    ClientServices::ClearMessageHandler(SMSG_GROUP_DESTROYED);
    ClientServices::ClearMessageHandler(SMSG_PARTY_COMMAND_RESULT);
    ClientServices::ClearMessageHandler(SMSG_GROUP_LIST);
    ClientServices::ClearMessageHandler(SMSG_REAL_GROUP_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_INVALID_QUEST);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_QUEST_DETAILS);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_REQUEST_ITEMS);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_QUEST_FAILED);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_STATUS);
    ClientServices::ClearMessageHandler(SMSG_QUEST_GIVER_STATUS_MULTIPLE);
    ClientServices::ClearMessageHandler(SMSG_QUEST_LOG_FULL);
    ClientServices::ClearMessageHandler(SMSG_TRAINER_LIST);
    ClientServices::ClearMessageHandler(CMSG_QUERY_OBJECT_POSITION);
    ClientServices::ClearMessageHandler(SMSG_SET_PROFICIENCY);
    ClientServices::ClearMessageHandler(SMSG_RESURRECT_REQUEST);
    ClientServices::ClearMessageHandler(SMSG_PLAYER_SKINNED);
    ClientServices::ClearMessageHandler(SMSG_INSPECT_RESULTS_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_INSPECT_TALENT);
    ClientServices::ClearMessageHandler(SMSG_READ_ITEM_RESULT_OK);
    ClientServices::ClearMessageHandler(SMSG_READ_ITEM_RESULT_FAILED);
    ClientServices::ClearMessageHandler(SMSG_CANCEL_COMBAT);
    ClientServices::ClearMessageHandler(SMSG_TAXI_NODE_STATUS);
    ClientServices::ClearMessageHandler(SMSG_SHOW_TAXI_NODES);
    ClientServices::ClearMessageHandler(SMSG_ACTIVATE_TAXI_REPLY);
    ClientServices::ClearMessageHandler(SMSG_GUILD_DECLINE);
    ClientServices::ClearMessageHandler(SMSG_GUILD_INVITE_CANCEL);
    ClientServices::ClearMessageHandler(SMSG_GUILD_EVENT);
    ClientServices::ClearMessageHandler(SMSG_GUILD_COMMAND_RESULT_2);
    ClientServices::ClearMessageHandler(MSG_SAVE_GUILD_EMBLEM);
    ClientServices::ClearMessageHandler(MSG_TABARDVENDOR_ACTIVATE);
    ClientServices::ClearMessageHandler(SMSG_PETITION_SHOW_LIST);
    ClientServices::ClearMessageHandler(SMSG_PETITION_SHOW_SIGNATURES);
    ClientServices::ClearMessageHandler(SMSG_PETITION_SIGN_RESULTS);
    ClientServices::ClearMessageHandler(MSG_PETITION_DECLINE);
    ClientServices::ClearMessageHandler(SMSG_TURN_IN_PETITION_RESULT);
    ClientServices::ClearMessageHandler(MSG_PETITION_RENAME);
    ClientServices::ClearMessageHandler(SMSG_BIND_POINT_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_BINDZONEREPLY);
    ClientServices::ClearMessageHandler(SMSG_EMOTE);
    ClientServices::ClearMessageHandler(SMSG_PLAYER_BOUND);
    ClientServices::ClearMessageHandler(SMSG_PLAYERBINDERROR);
    ClientServices::ClearMessageHandler(SMSG_NEW_TAXI_PATH);
    ClientServices::ClearMessageHandler(SMSG_PET_NAME_INVALID);
    ClientServices::ClearMessageHandler(SMSG_EXPLORATION_EXPERIENCE);
    ClientServices::ClearMessageHandler(SMSG_PARTY_MEMBER_STATS);
    ClientServices::ClearMessageHandler(SMSG_PARTY_MEMBER_STATS_FULL);
    ClientServices::ClearMessageHandler(SMSG_QUEST_UPDATE_FAILED);
    ClientServices::ClearMessageHandler(SMSG_QUEST_UPDATE_FAILED_TIMER);
    ClientServices::ClearMessageHandler(SMSG_QUEST_UPDATE_COMPLETE);
    ClientServices::ClearMessageHandler(SMSG_QUEST_UPDATE_ADD_KILL);
    ClientServices::ClearMessageHandler(SMSG_QUEST_UPDATE_ADD_PVP_CREDIT);
    ClientServices::ClearMessageHandler(SMSG_QUEST_CONFIRM_ACCEPT);
    ClientServices::ClearMessageHandler(SMSG_SHOW_BANK);
    ClientServices::ClearMessageHandler(SMSG_BUY_BANK_SLOT_RESULT);
    ClientServices::ClearMessageHandler(SMSG_LEVEL_UP_INFO);
    ClientServices::ClearMessageHandler(MSG_MINIMAP_PING);
    ClientServices::ClearMessageHandler(SMSG_TRIGGER_CINEMATIC);
    ClientServices::ClearMessageHandler(SMSG_TRIGGER_MOVIE);
    ClientServices::ClearMessageHandler(SMSG_ITEM_TIME_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_ITEM_ENCHANT_TIME_UPDATE);
    ClientServices::ClearMessageHandler(MSG_RANDOM_ROLL);
    ClientServices::ClearMessageHandler(SMSG_FISH_NOT_HOOKED);
    ClientServices::ClearMessageHandler(SMSG_FISH_ESCAPED);
    ClientServices::ClearMessageHandler(SMSG_FORCEACTIONSHOW);
    ClientServices::ClearMessageHandler(SMSG_GOD_MODE);
    ClientServices::ClearMessageHandler(SMSG_PETGODMODE);
    ClientServices::ClearMessageHandler(SMSG_IGNORE_DIMINISHING_RETURNS_CHEAT);
    ClientServices::ClearMessageHandler(SMSG_IGNORE_REQUIREMENTS_CHEAT);
    ClientServices::ClearMessageHandler(SMSG_CLEAR_FAR_SIGHT_IMMEDIATE);
    ClientServices::ClearMessageHandler(SMSG_CHAT_WRONG_FACTION);
    ClientServices::ClearMessageHandler(SMSG_CHAT_PLAYER_NOTFOUND);
    ClientServices::ClearMessageHandler(SMSG_CHAT_PLAYER_AMBIGUOUS);
    ClientServices::ClearMessageHandler(SMSG_CHAT_RESTRICTED);
    ClientServices::ClearMessageHandler(SMSG_CHAT_NOT_IN_PARTY);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_GET_SYSTEM_STATUS);
    ClientServices::ClearMessageHandler(SMSG_SPIRIT_HEALER_CONFIRM);
    ClientServices::ClearMessageHandler(CMSG_AUTOSTORE_GROUND_ITEM);
    ClientServices::ClearMessageHandler(SMSG_BINDER_CONFIRM);
    ClientServices::ClearMessageHandler(SMSG_SPELL_EXECUTE_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_MISS_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_DAMAGE_SHIELD);
    ClientServices::ClearMessageHandler(SMSG_SPELL_INSTAKILL_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_NON_MELEE_DAMAGE_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_HEAL_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_ENERGIZE_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_DISPELL_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_STEAL_LOG);
    ClientServices::ClearMessageHandler(SMSG_SPELL_BREAK_LOG);
    ClientServices::ClearMessageHandler(SMSG_RESURRECT_FAILED);
    ClientServices::ClearMessageHandler(SMSG_SPELL_OR_DAMAGE_IMMUNE);
    ClientServices::ClearMessageHandler(MSG_QUEST_PUSH_RESULT);
    ClientServices::ClearMessageHandler(SMSG_DAMAGE_CALC_LOG);
    ClientServices::ClearMessageHandler(SMSG_RAID_GROUP_ONLY);
    ClientServices::ClearMessageHandler(SMSG_INSTANCE_LOCK_WARNING_QUERY);
    ClientServices::ClearMessageHandler(SMSG_LOOT_START_ROLL);
    ClientServices::ClearMessageHandler(SMSG_LOOT_ALL_PASSED);
    ClientServices::ClearMessageHandler(SMSG_LOOT_ROLL_WON);
    ClientServices::ClearMessageHandler(SMSG_LOOT_ROLL);
    ClientServices::ClearMessageHandler(SMSG_LOOT_MASTER_LIST);
    ClientServices::ClearMessageHandler(SMSG_TEST_DROP_RATE_RESULT);
    ClientServices::ClearMessageHandler(SMSG_SUMMON_REQUEST);
    ClientServices::ClearMessageHandler(SMSG_SUMMON_CANCEL);
    ClientServices::ClearMessageHandler(SMSG_PLAY_TIME_WARNING);
    ClientServices::ClearMessageHandler(SMSG_ARENA_TEAM_INVITE);
    ClientServices::ClearMessageHandler(SMSG_ARENA_TEAM_EVENT);
    ClientServices::ClearMessageHandler(SMSG_ARENA_ERROR);
    ClientServices::ClearMessageHandler(SMSG_DEATH_RELEASE_LOC);
    ClientServices::ClearMessageHandler(SMSG_FORCED_DEATH_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_VOICE_SESSION_ROSTER_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_VOICE_SESSION_LEAVE);
    ClientServices::ClearMessageHandler(SMSG_VOICE_SET_TALKER_MUTED);
    ClientServices::ClearMessageHandler(SMSG_VOICE_PARENTAL_CONTROLS);
    ClientServices::ClearMessageHandler(SMSG_CROSSED_INEBRIATION_THRESHOLD);
    ClientServices::ClearMessageHandler(SMSG_COMPLAINT_RESULT);
    ClientServices::ClearMessageHandler(SMSG_AVAILABLE_VOICE_CHANNEL);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_COMMAND_RESULT);
    ClientServices::ClearMessageHandler(SMSG_REPORT_PVP_AFK_RESULT);
    ClientServices::ClearMessageHandler(SMSG_GROUP_ACTION_THROTTLED);
    ClientServices::ClearMessageHandler(SMSG_SEND_UNLEARN_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_CONVERT_RUNE);
    ClientServices::ClearMessageHandler(SMSG_RESYNC_RUNES);
    ClientServices::ClearMessageHandler(SMSG_ADD_RUNE_POWER);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_TALENT_DATA);
    ClientServices::ClearMessageHandler(SMSG_ARENA_TEAM_CHANGE_FAILED_QUEUED);
    ClientServices::ClearMessageHandler(SMSG_QUEST_POI_QUERY_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_TOGGLE_XP_GAIN);
    ClientServices::ClearMessageHandler(CMSG_GUILD_SET_OFFICER_NOTE);
    ClientServices::ClearMessageHandler(SMSG_QUEST_FORCE_REMOVED);
    ClientServices::ClearMessageHandler(SMSG_CAMERA_SHAKE);
    ClientServices::ClearMessageHandler(SMSG_SOCKET_GEMS);
    ClientServices::ClearMessageHandler(SMSG_TALENTS_ERROR);
    ClientItemRefundInfoResponse::s_handler = NULL;
    ClientItemExpirePurchaseRefund::s_handler = NULL;
    ClientItemRefundResult::s_handler = NULL;
    ClientQuestGiverQuestComplete::s_handler = NULL;
    ClientFeatureSystemStatus::s_handler = NULL;
    ClientSupercededSpells::s_handler = NULL;
    ClientLearnedSpell::s_handler = NULL;
    ClientUpdateActionButtons::s_handler = NULL;
}

// INLINED
bool CGPlayer_C::IsInFlyableZone(int32_t checkContinent) {
    return IsInFlyableArea() && (!(this->m_obj->m_type & TYPEMASK_PLAYER) || !checkContinent || CGGameUI::m_iCurrentMapID != 571 || HasSpell(static_cast<uint32_t>(54197)));
}

// PROVEN: 1722fa35 IDENTICAL/NONE
// OFFSET: 0x00553830/0x1401AE780
int32_t OnArenaTeamsChanged(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGArenaTeamInfo::UpdateTeams(1);
    return 1;
}

// OFFSET: 0x005538B0
int32_t SignalCombatRatingUpdate(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_RATING_UPDATE, NULL);
    PlayerSignalSkillStatChanges();
    return 1;
}

// PROVEN: 5cff6dfb IDENTICAL/NONE
// OFFSET: 0x005538D0/0x1401AE840
int32_t OnUpdatePlayerNoReagentCost(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGSpellBook::UpdateUsable();
    return 1;
}

// OFFSET: 0x00555150
int32_t CritterUpdate(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        CGActionBar::UpdateCritter((static_cast<const WOWGUID*>(value)->m_guid >> 24) & 0xFFFFFFF);
        CGActionBar::UpdateCritter((player->m_unit->m_unitFieldCritter.m_guid >> 24) & 0xFFFFFFF);
        CGSpellBook::SendCompanionUpdate(0);
    }

    return 1;
}

// OFFSET: 0x00556F50
int32_t CharmChangeHandler(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateReaction(false);

        if (guid == ClntObjMgrGetActivePlayer()) {
            FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
        }
    }

    return 1;
}

// OFFSET: 0x00556FB0
int32_t SignalPlayerSkillStatEventsAndRefresh(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_ATTACK, "%s", "player");
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_DEFENSE, "%s", "player");
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_RANGED_ATTACK_POWER, "%s", "player");
    }

    PlayerSignalSkillStatChanges();
    return 1;
}

// OFFSET: 0x0055F310/0x1401BCC20
int32_t ReportDailyQuestsRemaining(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    int32_t completed = 0;

    for (uint32_t i = 0; i < 25; i++) {
        if (player->m_player->m_playerFieldDailyQuests1[i]) {
            completed++;
        }
    }

    if (!completed && s_dailyQuestsUntouched) {
        return 1;
    }

    char text[3000];

    if (completed == 25) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("NO_DAILY_QUESTS_REMAINING", -1, GENDER_NOT_APPLICABLE));
        s_dailyQuestsUntouched = false;
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("DAILY_QUESTS_REMAINING", -1, GENDER_NOT_APPLICABLE), 25 - completed);
        s_dailyQuestsUntouched = completed == 0;
    }

    if (text[0]) {
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
    }

    return 1;
}

// OFFSET: 0x0055F950
int32_t OnUpdateRestState(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_EXHAUSTION, NULL);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t state = player->m_player->m_playerBytes2[3];

        if (state < 3) {
            if (s_restStateInfo[state].m_error != ERR_MAX_VALUE) {
                CGGameUI::DisplayError(s_restStateInfo[state].m_error);
            }

            if (s_restStateInfo[state].m_tutorial != 60) {
                CGTutorial::TriggerTutorial(s_restStateInfo[state].m_tutorial);
            }
        }
    }

    return 1;
}

// PROVEN: 949ee31d IDENTICAL/NONE
// OFFSET: 0x0055FB10/0x1401B3E40
int32_t OnUpdatePlayerTrackingMask(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    ClntObjMgrEnumVisibleObjects(TrackingMaskUpdateProc, NULL);
    return 1;
}

// OFFSET: 0x00553640
int32_t __stdcall CGPlayer_C::OnBuyFailed(CDataStore* msg) {
    WOWGUID vendor;
    uint32_t itemId;
    uint8_t result;
    msg->Get(vendor.m_guid);
    msg->Get(itemId);
    msg->Get(result);

    if (result == 1 && vendor == CGPlayer_C::s_lastVendorListReceived) {
        for (uint32_t i = 0; i < 150; i++) {
            if (s_vendorItems[i].m_slot == itemId) {
                s_vendorItems[i].m_maxCount = 0;
                CGMerchantInfo::UpdateItemQuantity(vendor, itemId, 0);
            }
        }
    }

    switch (result) {
        case 1:
        case 7:
            CGGameUI::DisplayError(ERR_VENDOR_SOLD_OUT);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_VENDOR_HATES_YOU);
            break;

        case 5:
            CGGameUI::DisplayError(ERR_VENDOR_TOO_FAR);
            break;

        case 6:
            break;

        case 8:
            CGGameUI::DisplayError(ERR_ITEM_MAX_COUNT);
            break;

        case 11:
            CGGameUI::DisplayError(ERR_CANT_EQUIP_RANK);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_CANT_EQUIP_REPUTATION);
            break;

        default:
            CGGameUI::DisplayError(ERR_ITEM_NOT_FOUND);
            break;
    }

    return 1;
}

// OFFSET: 0x00553780
int32_t __stdcall CGPlayer_C::OnBuySucceeded(CDataStore* msg) {
    WOWGUID vendor;
    uint32_t slot;
    uint32_t quantity;
    uint32_t count;
    msg->Get(vendor.m_guid);
    msg->Get(slot);
    msg->Get(quantity);
    msg->Get(count);

    if (vendor == CGPlayer_C::s_lastVendorListReceived) {
        CGMerchantInfo::UpdateItemQuantity(vendor, slot, quantity);
    }

    return 1;
}

// OFFSET: 0x00553A60
void CGPlayer_C::ReceiveResurrectRequest(const char* name) {
    if (name && *name && this->IsDeadOrGhost()) {
        CGGameUI::OpenResurrectRequest(name);
    }
}

// OFFSET: 0x00556A20
int32_t __stdcall CGPlayer_C::OnSellResponse(CDataStore* msg) {
    WOWGUID vendor;
    WOWGUID item;
    uint8_t result;
    msg->Get(vendor.m_guid);
    msg->Get(item.m_guid);
    msg->Get(result);

    if (!result) {
        return 1;
    }

    UIErrors error;

    switch (result) {
        case 1:
            error = ERR_ITEM_NOT_FOUND;
            break;

        case 2:
            error = ERR_VENDOR_NOT_INTERESTED;
            break;

        case 3:
            error = ERR_VENDOR_HATES_YOU;
            break;

        case 4:
            error = ERR_NOT_OWNER;
            break;

        case 6:
            error = ERR_DESTROY_NONEMPTY_BAG;
            break;

        case 7:
            error = ERR_VENDOR_DOESNT_BUY;
            break;

        case 8:
            error = ERR_MUST_REPAIR_DURABILITY;
            break;

        case 9:
            error = ERR_INTERNAL_BAG_ERROR;
            break;

        default:
            error = ERR_MAX_VALUE;
            break;
    }

    if (error != ERR_MAX_VALUE) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(vendor, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            CGGameUI::AddErrorMessage(unit->GetFrameScriptText(CGGameUI::GetErrorToken(error), -1), 1);
        }
    }

    if (item) {
        CGGameUI::UnlockItem(item);
    }

    return 1;
}

// OFFSET: 0x0055E3D0
void CGPlayer_C::CheckForQuestItemLoot(int32_t itemId) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !itemId) {
        return;
    }

    for (uint32_t i = 0; i < 50; i++) {
        PlayerQuestLogEntry* entry = &this->m_player->m_playerQuestLog[i];

        if (!entry || static_cast<int32_t>(entry->m_questId) <= 0 || CGQuestLog::IsQuestExpiredByID(entry->m_questId)) {
            continue;
        }

        WOWGUID requester = WOWGUID();
        QuestCache* quest = g_questDBCache.GetRecord(entry->m_questId, requester, NULL, NULL, false);

        if (!quest || !(quest->m_flags & 0x10000)) {
            continue;
        }

        for (uint32_t j = 0; j < 6; j++) {
            if (static_cast<int32_t>(quest->m_collectItemId[j]) == itemId) {
                this->CheckQuestAutoComplete(quest->m_id, 0, 0);
                break;
            }
        }
    }
}

// OFFSET: 0x00560AE0
static void ResurrectNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    NameCache* name = g_nameDBCache.GetRecord(CGPlayer_C::s_resurrectOfferer.m_guid, CGPlayer_C::s_resurrectOfferer, NULL, NULL, false);

    if (!name) {
        CGPlayer_C::s_resurrectOfferer.m_guid = 0;
        CGPlayer_C::s_resurrectOfferSickness = 0;
        CGPlayer_C::s_resurrectOfferUseTimer = 0;
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && name->m_name[0] && player->IsDeadOrGhost()) {
        CGGameUI::OpenResurrectRequest(name->m_name);
    }
}

// OFFSET: 0x00560B80
int32_t Packet_SMSG_RESURRECT_REQUEST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID offerer;
    uint32_t length;
    void* data;
    uint8_t sickness;
    uint8_t useTimer;
    msg->Get(offerer.m_guid);
    msg->Get(length);
    msg->GetDataInSitu(data, length);
    const char* name = static_cast<const char*>(data);
    msg->Get(sickness);
    msg->Get(useTimer);
    CGPlayer_C::s_resurrectOfferer = offerer;
    CGPlayer_C::s_resurrectOfferSickness = sickness;
    CGPlayer_C::s_resurrectOfferUseTimer = useTimer;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    if (*name) {
        if (player->IsDeadOrGhost()) {
            CGGameUI::OpenResurrectRequest(name);
        }

        return 1;
    }

    NameCache* record = g_nameDBCache.GetRecord(offerer.m_guid, offerer, ResurrectNameQueryCallback, NULL, false);

    if (record) {
        player->ReceiveResurrectRequest(record->m_name);
    }

    return 1;
}

// OFFSET: 0x00561400/0x1401BF280
void CGPlayer_C::OnDeath() {
    if (this->m_unit->m_unitDynamicFlags & 0x20) {
        return;
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        WOWGUID guid = this->m_obj->m_guid;

        if (CGGameUI::IsInParty(guid)) {
            CGGameUI::DisplayError(ERR_PLAYER_DIED_S, this->GetUnitName(NULL, 1));
        }

        return;
    }

    this->UpdateRepopTimer();
    CGGameUI::Target(WOWGUID());
    CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, 0);
    CGGameUI::CloseLoot(true, false, false);
    Spell_C_CancelPlayerSpells();

    if (this->m_unit->m_unitFieldFlags & 0x100000) {
        this->HandleRepopRequest(false);
    }
}

// OFFSET: 0x00554720
void __stdcall CGPlayer_C::HandleActivateTaxiReply(uint32_t reply) {
    if (reply >= 13) {
        return;
    }

    if (reply) {
        CGGameUI::DisplayError(s_taxiReplyErrors[reply]);
    } else {
        CGTaxiMap::CloseMap();
    }
}

// OFFSET: 0x0055C410/0x1401B9110
void __stdcall CGPlayer_C::ShowTaxiNodes(CDataStore* msg) {
    uint32_t knownNodes = 0;
    WOWGUID unit;
    uint32_t currentNode = 0;
    uint32_t show = 0;
    msg->Get(show);

    if (show) {
        uint64_t guid = 0;
        msg->Get(guid);
        unit.m_guid = guid;
        uint32_t node = 0;
        msg->Get(node);
        currentNode = node;
    }

    uint32_t nodeMaskSize = 0;
    msg->Get(nodeMaskSize);

    if (!nodeMaskSize) {
        if (show) {
            CGGameUI::DisplayError(ERR_TAXINOPATHS);
            CGGossipInfo::SetGossip(WOWGUID());
        } else {
            ConsolePrintf("ShowTaxiNodes : No nodes found\n");
        }

        return;
    }

    uint8_t* nodeMask = new (__FILE__, __LINE__) uint8_t[nodeMaskSize];

    for (uint32_t i = 0; i < nodeMaskSize; i++) {
        uint8_t mask = 0;
        msg->Get(mask);
        nodeMask[i] = mask;

        for (uint32_t bit = 0; bit < 8; bit++) {
            if (mask & (1 << bit)) {
                knownNodes++;
            }
        }
    }

    if (show) {
        if (knownNodes >= 2) {
            CGTaxiMap::SetupMap(unit, currentNode, nodeMask, nodeMaskSize);
        } else {
            CGGameUI::DisplayError(ERR_TAXINOPATHS);
            CGGossipInfo::SetGossip(WOWGUID());
        }
    } else {
        int32_t nodeID = 1;

        for (int32_t i = 0; i < static_cast<int32_t>(nodeMaskSize); i++) {
            for (uint32_t bit = 0; bit < 8; bit++) {
                TaxiNodesRec* node;

                if ((nodeMask[i] & (1 << bit)) && (node = g_taxiNodesDB.GetRecord(nodeID))) {
                    ConsolePrintf("[%02d]: %s", nodeID, static_cast<const char*>(node->m_name));
                }

                nodeID++;
            }
        }
    }

    if (nodeMask) {
        SMemFree(nodeMask, "delete[]", -1, 0);
    }
}

// OFFSET: 0x00564600/0x1401C2730
uint32_t CGPlayer_C::BuildPartyMemberStatsChanges(PartyMemberState& oldState, PartyMemberState& newState) {
    uint32_t changes = newState.m_flags != oldState.m_flags;

    if (newState.m_health != oldState.m_health) {
        changes |= 0x2;
    }

    if (newState.m_maxHealth != oldState.m_maxHealth) {
        changes |= 0x4;
    }

    if (newState.m_powerType != oldState.m_powerType) {
        changes |= 0x8;
    }

    if (newState.m_power != oldState.m_power) {
        changes |= 0x10;
    }

    if (newState.m_maxPower != oldState.m_maxPower) {
        changes |= 0x20;
    }

    if (newState.m_level != oldState.m_level) {
        changes |= 0x40;
    }

    if (newState.m_zoneID != oldState.m_zoneID) {
        changes |= 0x80;
    }

    if (newState.unk_0022 != oldState.unk_0022) {
        changes |= 0x100;
    }

    if (newState.m_posX != oldState.m_posX || newState.m_posY != oldState.m_posY || newState.m_posZ != oldState.m_posZ) {
        changes |= 0x200;
    }

    if (newState.m_numAuras > oldState.m_auras.Count()) {
        oldState.m_auras.SetCount(newState.m_numAuras);
    } else if (oldState.m_numAuras > newState.m_auras.Count()) {
        newState.m_auras.SetCount(oldState.m_numAuras);
    }

    if (newState.m_numAuras != oldState.m_numAuras) {
        changes |= 0x400;
    } else {
        for (uint32_t i = 0; i < newState.m_numAuras; i++) {
            if (newState.m_auras[i].m_spellId != oldState.m_auras[i].m_spellId || newState.m_auras[i].m_flags != oldState.m_auras[i].m_flags) {
                changes |= 0x400;
                break;
            }
        }
    }

    if (newState.m_pet.m_guid != oldState.m_pet.m_guid) {
        changes |= 0x800;
    }

    if (strcmp(newState.m_pet.m_name, oldState.m_pet.m_name)) {
        changes |= 0x1000;
    }

    if (newState.m_pet.m_displayId != oldState.m_pet.m_displayId) {
        changes |= 0x2000;
    }

    if (newState.m_pet.m_health != oldState.m_pet.m_health) {
        changes |= 0x4000;
    }

    if (newState.m_pet.m_maxHealth != oldState.m_pet.m_maxHealth) {
        changes |= 0x8000;
    }

    if (newState.m_pet.m_powerType != oldState.m_pet.m_powerType) {
        changes |= 0x10000;
    }

    if (newState.m_pet.m_power != oldState.m_pet.m_power) {
        changes |= 0x20000;
    }

    if (newState.m_pet.m_maxPower != oldState.m_pet.m_maxPower) {
        changes |= 0x40000;
    }

    if (newState.m_pet.m_numAuras > oldState.m_pet.m_auras.Count()) {
        oldState.m_pet.m_auras.SetCount(newState.m_pet.m_numAuras);
    } else if (oldState.m_pet.m_numAuras > newState.m_pet.m_auras.Count()) {
        newState.m_pet.m_auras.SetCount(oldState.m_pet.m_numAuras);
    }

    if (newState.m_pet.m_numAuras != oldState.m_pet.m_numAuras) {
        changes |= 0x80000;
    } else {
        for (uint32_t i = 0; i < newState.m_pet.m_numAuras; i++) {
            if (newState.m_pet.m_auras[i].m_spellId != oldState.m_pet.m_auras[i].m_spellId || newState.m_pet.m_auras[i].m_flags != oldState.m_pet.m_auras[i].m_flags) {
                changes |= 0x80000;
                break;
            }
        }
    }

    if (newState.m_vehicleSeatId != oldState.m_vehicleSeatId) {
        changes |= 0x100000;
    }

    if (!(oldState.m_phaseShift == newState.m_phaseShift)) {
        changes |= 0x200000;
    }

    return changes;
}

// OFFSET: 0x00555430/0x1401B1CB0
void CGPlayer_C::DispatchPartyMemberStatsChanges(const WOWGUID& guid, uint32_t changes, const PartyMemberState& oldState, const PartyMemberState& newState) {
    if (changes & 0x1) {
        uint8_t flagChanges = static_cast<uint8_t>(newState.m_flags ^ oldState.m_flags);

        if (flagChanges & 0xC) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEALTH);
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEALTH_FREQUENT);
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER, "%s", Spell_C_GetPowerTypeToken(newState.m_powerType));
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER_FREQUENT, "%s", Spell_C_GetPowerTypeToken(newState.m_powerType));
            changes &= ~0x12;
        }

        if (flagChanges & 0x1) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_CONNECTION, "%b", newState.m_flags & 1);
        }

        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_FLAGS);
    }

    if (changes & 0x40) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_LEVEL);
    }

    if (changes & 0x2) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEALTH);
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEALTH_FREQUENT);
    }

    if (changes & 0x4) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_MAXHEALTH);
    }

    if (changes & 0x8) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_DISPLAYPOWER);
    }

    if (changes & 0x10) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER, "%s", Spell_C_GetPowerTypeToken(newState.m_powerType));
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER_FREQUENT, "%s", Spell_C_GetPowerTypeToken(newState.m_powerType));
    }

    if (changes & 0x20) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_MAXPOWER, "%s", Spell_C_GetPowerTypeToken(newState.m_powerType));
    }

    if (static_cast<uint8_t>(newState.m_flags ^ oldState.m_flags) & 0x12) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_FACTION);
    }

    if (changes & 0x400) {
        Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_AURA);
    }

    if (changes & 0x800) {
        if (CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(oldState.m_pet.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__))) {
            pet->UpdateScriptRegistration();
        }

        if (CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(newState.m_pet.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__))) {
            pet->UpdateScriptRegistration();
        }

        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_PET);
    }

    if (newState.m_pet.m_guid) {
        const WOWGUID& pet = newState.m_pet.m_guid;

        if (changes & 0x1000) {
            CGGameUI::UnitNameUpdate(pet);
        }

        if (changes & 0x2000) {
            CGGameUI::UnitModelUpdate(pet, 3);
        }

        if (changes & 0x4000) {
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_HEALTH);
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_HEALTH_FREQUENT);
        }

        if (changes & 0x8000) {
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_MAXHEALTH);
        }

        if (changes & 0x10000) {
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_DISPLAYPOWER);
        }

        if (changes & 0x20000) {
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_POWER, "%s", Spell_C_GetPowerTypeToken(newState.m_pet.m_powerType));
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_POWER_FREQUENT, "%s", Spell_C_GetPowerTypeToken(newState.m_pet.m_powerType));
        }

        if (changes & 0x40000) {
            Script_SendUnitSignal(pet, SCRIPT_EVENT_UNIT_MAXPOWER, "%s", Spell_C_GetPowerTypeToken(newState.m_pet.m_powerType));
        }

        if (changes & 0x80000) {
            Script_SendUniqueSignal(pet, SCRIPT_EVENT_UNIT_AURA);
        }
    }

    if (changes & 0x100000) {
        if (VehicleSeatRec* seat = g_vehicleSeatDB.GetRecord(newState.m_vehicleSeatId)) {
            CVehiclePassenger_C::SendTransitionUIEvents(guid, WOWGUID(), NULL, seat, 0, 3);
        } else {
            CVehiclePassenger_C::SendTransitionUIEvents(guid, WOWGUID(), NULL, NULL, 3, 0);
        }
    }

    if (changes & 0x200000) {
        Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_PHASE);
    }
}

// OFFSET: 0x0056B390
int32_t Packet_HandlePartyMemberStats(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t arena = 0;

    if (msgId == SMSG_PARTY_MEMBER_STATS_FULL) {
        msg->Get(arena);
    }

    SmartGUID guid;
    *msg >> guid;

    if (arena) {
        WOWGUID opponent = guid;
        CGBattlefieldInfo::AddArenaOpponent(opponent);
    }

    WOWGUID member = guid;
    PartyMemberState* state = CGGameUI::GetPartyMemberState(member);
    PartyMemberState oldState;
    PartyMemberState newState;

    if (state) {
        oldState.Copy(state);
    } else {
        oldState.Reset();
    }

    if (msgId == SMSG_PARTY_MEMBER_STATS_FULL) {
        newState.Reset();
    } else {
        newState.Copy(&oldState);
    }

    uint32_t mask;
    msg->Get(mask);

    if (mask & 0x1) {
        msg->Get(newState.m_flags);
        WOWGUID flagged = guid;

        if (newState.m_flags & 0x1) {
            CGAutoCompleteName::AddFlags(flagged, 32, 0, NULL);
        } else {
            CGAutoCompleteName::RemoveFlags(flagged, 32);
        }
    }

    if (static_cast<int32_t>(mask) >= 0) {
        newState.m_flags &= ~0x20;
    } else {
        newState.m_flags |= 0x20;
    }

    if (mask & 0x2) {
        msg->Get(newState.m_health);
    }

    if (mask & 0x4) {
        msg->Get(newState.m_maxHealth);
    }

    if (mask & 0x8) {
        msg->Get(newState.m_powerType);
    }

    if (mask & 0x10) {
        msg->Get(newState.m_power);
    }

    if (mask & 0x20) {
        msg->Get(newState.m_maxPower);
    }

    if (mask & 0x40) {
        msg->Get(newState.m_level);
    }

    if (mask & 0x80) {
        msg->Get(newState.m_zoneID);
    }

    if (mask & 0x100) {
        msg->Get(newState.unk_0022);
    }

    if (mask & 0x200) {
        msg->Get(reinterpret_cast<uint16_t&>(newState.m_posX));
        msg->Get(reinterpret_cast<uint16_t&>(newState.m_posY));
        msg->Get(reinterpret_cast<uint16_t&>(newState.m_posZ));
    }

    if (mask & 0x400) {
        uint8_t full = 0;
        msg->Get(full);
        uint64_t auraMask;
        msg->Get(auraMask);
        uint32_t count;
        msg->Get(count);

        if (count > newState.m_auras.Count()) {
            newState.m_auras.SetCount(count);
        }

        newState.m_numAuras = count;

        for (uint32_t i = 0; i < count; i++) {
            PartyMemberAuraStates& aura = newState.m_auras[i];

            if (auraMask & (static_cast<uint64_t>(1) << i)) {
                msg->Get(reinterpret_cast<uint32_t&>(aura.m_spellId));
                msg->Get(aura.m_flags);

                if (aura.m_flags & 0x40) {
                    for (uint32_t j = 0; j < 3; j++) {
                        msg->Get(reinterpret_cast<uint32_t&>(aura.m_effectAmounts[j]));
                    }
                }
            } else if (full) {
                memset(&aura, 0, sizeof(aura));
            }
        }
    }

    if (mask & 0x800) {
        msg->Get(newState.m_pet.m_guid.m_guid);

        if (arena && newState.m_pet.m_guid) {
            WOWGUID owner = guid;
            CGBattlefieldInfo::AddArenaOpponentPet(newState.m_pet.m_guid, owner);
        }
    }

    if (mask & 0x1000) {
        msg->GetString(newState.m_pet.m_name, sizeof(newState.m_pet.m_name));
    }

    if (mask & 0x2000) {
        msg->Get(newState.m_pet.m_displayId);
    }

    if (mask & 0x4000) {
        msg->Get(newState.m_pet.m_health);
    }

    if (mask & 0x8000) {
        msg->Get(newState.m_pet.m_maxHealth);
    }

    if (mask & 0x10000) {
        msg->Get(newState.m_pet.m_powerType);
    }

    if (mask & 0x20000) {
        msg->Get(newState.m_pet.m_power);
    }

    if (mask & 0x40000) {
        msg->Get(newState.m_pet.m_maxPower);
    }

    if (mask & 0x80000) {
        uint8_t full = 0;
        msg->Get(full);
        uint64_t auraMask;
        msg->Get(auraMask);
        uint32_t count;
        msg->Get(count);

        if (count > newState.m_pet.m_auras.Count()) {
            newState.m_pet.m_auras.SetCount(count);
        }

        newState.m_pet.m_numAuras = count;

        for (uint32_t i = 0; i < count; i++) {
            PartyMemberAuraStates& aura = newState.m_pet.m_auras[i];

            if (auraMask & (static_cast<uint64_t>(1) << i)) {
                msg->Get(reinterpret_cast<uint32_t&>(aura.m_spellId));
                msg->Get(aura.m_flags);

                if (aura.m_flags & 0x40) {
                    for (uint32_t j = 0; j < 3; j++) {
                        msg->Get(reinterpret_cast<uint32_t&>(aura.m_effectAmounts[j]));
                    }
                }
            } else if (full) {
                memset(&aura, 0, sizeof(aura));
            }
        }
    }

    if (mask & 0x100000) {
        msg->Get(newState.m_vehicleSeatId);
    }

    if (mask & 0x200000) {
        newState.m_phaseShift.SetPhasesFromDataStore(msg);
    }

    uint32_t changes = CGPlayer_C::BuildPartyMemberStatsChanges(oldState, newState);
    int32_t index;
    WOWGUID opponent = guid;

    if (CGBattlefieldInfo::GetArenaOpponentIndex(opponent, &index) && CGBattlefieldInfo::m_arenaOpponentSeen[index]) {
        changes = 0xFFFFFFFF;

        if (CGBattlefieldInfo::GetArenaOpponentIndex(opponent, &index)) {
            CGBattlefieldInfo::m_arenaOpponentSeen[index] = 0;
        }
    }

    if (state) {
        if (state->m_flags & 0x100) {
            newState.m_flags |= 0x100;
        }

        state->Copy(&newState);
        WOWGUID raidGuid = guid;

        if (PartyMemberState* remote = CGRaidInfo::GetRemoteStats(raidGuid)) {
            remote->Copy(&newState);
        }
    }

    WOWGUID dispatch = guid;
    CGPlayer_C::DispatchPartyMemberStatsChanges(dispatch, changes, oldState, newState);
    return 1;
}

// OFFSET: 0x005659B0/0x1401C3B60
void CGPlayer_C::UpdateGhostStatus() {
    if (this->m_player->m_playerFlags & 0x10) {
        CGTutorial::TriggerTutorial(24);
    } else {
        CGGameUI::SetAreaSpiritHealer(WOWGUID());
        C3Vector position(0.0f, 0.0f, 0.0f);
        CGGameUI::SetDeathReleaseLocation(-1, position);
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_UNGHOST, NULL);
    }

    CGGameUI::UpdateCorpseLocation();
    CGWorldFrame::UpdateScreenEffect();
    SendQuestGiverStatusMultipleQuery();
    ClntObjMgrEnumVisibleObjects(TrackingMaskUpdateProc, NULL);
    PlayerNameTriggerColorUpdateAll();
    this->UpdateReaction(false);
}

// OFFSET: 0x005667A0/0x1401C4FC0
void CGPlayer_C::TogglePlayerFlag(uint32_t oldFlags) {
    uint32_t changed = oldFlags ^ this->m_player->m_playerFlags;

    if (changed & 0x800E) {
        PlayerNameTriggerNameRegenerate(this->m_nameDesc);

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGChat::m_afk = this->m_player->m_playerFlags & 0x2;
        }
    }

    if (changed & 0x40300) {
        this->SetPvpCooldownTimestamp(oldFlags);
    }

    if (changed & 0xC00) {
        UpdateModelForUI(this->m_obj->m_guid, 3);
    }

    Script_SendUnitSignal(this->m_obj->m_guid, SCRIPT_EVENT_PLAYER_FLAGS_CHANGED);

    if (changed & 0x400) {
        if (this->m_player->m_playerFlags & 0x400) {
            this->HideHelm();
        } else {
            this->ShowHelm();
        }
    }

    if (changed & 0x800) {
        if (this->m_player->m_playerFlags & 0x800) {
            this->HideCloak();
        } else {
            this->ShowCloak();
        }
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    if (changed & 0x10) {
        this->UpdateGhostStatus();
    }

    if (changed & 0x20) {
        if (this->m_player->m_playerFlags & 0x20) {
            CGTutorial::TriggerTutorial(29);
            CGTutorial::TriggerTutorial(30);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_UPDATE_RESTING, NULL);
    }

    if (changed & 0x200) {
        const char* text;

        if (this->m_player->m_playerFlags & 0x200) {
            CGGameUI::DisplayError(ERR_PVP_TOGGLE_ON);
            text = FrameScript_GetText("PVP_TOGGLE_ON_VERBOSE", -1, GENDER_NOT_APPLICABLE);
        } else {
            CGGameUI::DisplayError(ERR_PVP_TOGGLE_OFF);
            text = FrameScript_GetText("PVP_TOGGLE_OFF_VERBOSE", -1, GENDER_NOT_APPLICABLE);
        }

        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
    }

    if (changed & 0x3000) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYTIME_CHANGED, NULL);
    }

    if (changed & 0x20000) {
        if (this->m_player->m_playerFlags & 0x20000) {
            FrameScript_SignalEvent(SCRIPT_EVENT_ENABLE_TAXI_BENCHMARK, NULL);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_DISABLE_TAXI_BENCHMARK, NULL);
        }
    }

    if (changed & 0x1800000) {
        CGSpellBook::UpdateUsable();
    }

    if (changed & 0x2000000) {
        if (this->m_player->m_playerFlags & 0x2000000) {
            FrameScript_SignalEvent(SCRIPT_EVENT_DISABLE_XP_GAIN, NULL);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_ENABLE_XP_GAIN, NULL);
        }
    }

    if (changed & 0x10000) {
        if (this->m_player->m_playerFlags & 0x10000) {
            FrameScript_SignalEvent(SCRIPT_EVENT_ENABLE_LOW_LEVEL_RAID, NULL);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_DISABLE_LOW_LEVEL_RAID, NULL);
        }
    }
}

// OFFSET: 0x005549C0
void CGPlayer_C::StopDance() {
    PlayerDance* dance = this->m_dance;

    if (dance) {
        delete[] dance->m_moves;
        dance->m_moves = NULL;
        delete dance;
    }

    this->m_dance = NULL;
}

// OFFSET: 0x0055D970/0x1401BADC0
void CGPlayer_C::Dance() {
    PlayerDance* dance = this->m_dance;

    if (!dance) {
        return;
    }

    int32_t index = this->m_danceMoveIndex;

    if (index < dance->m_count) {
        DanceMovesRec* move = g_danceMovesDB.GetRecord(dance->m_moves[index]);

        if (move) {
            this->PlayDanceMove(move);
        } else {
            char text[512];
            SStrPrintf(text, sizeof(text), "ERROR! danceMovesDB does not have record id %d", dance->m_moves[index]);
            ConsoleWrite(text, ERROR_COLOR);
        }

        return;
    }

    delete[] dance->m_moves;
    dance->m_moves = NULL;
    delete dance;
    this->m_dance = NULL;

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_STOP_DANCE));
    msg.Put(static_cast<uint8_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00563910/0x1401C16D0
void CGPlayer_C::DanceMoveComplete() {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SYNC_DANCE));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    this->m_danceMoveIndex++;
    this->Dance();
}

// PROVEN: 7eb2a76f IDENTICAL/NONE
// OFFSET: 0x005639E0/0x1401C17D0
static int32_t DanceTimerCallback(const void* data, uint64_t guid, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(guid), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->DanceMoveComplete();
    }

    return 1;
}

// OFFSET: 0x00567000
void CGPlayer_C::DanceMoveAnimComplete() {
    PlayerDance* dance = this->m_dance;

    if (!dance) {
        return;
    }

    int32_t index = this->m_danceMoveIndex;
    DanceMovesRec* move = g_danceMovesDB.GetRecord(dance->m_moves[index]);

    if (move->m_type == 4 && index > 0) {
        this->PlayDanceMove(g_danceMovesDB.GetRecord(dance->m_moves[index - 1]));
    } else {
        this->DanceMoveComplete();
    }
}

// OFFSET: 0x0056A6D0/0x1401C97F0
void CGPlayer_C::DestructActivePlayer() {
    if (ClntObjMgrGetPlayerType() != 1) {
        if (this->m_unit->m_unitFieldFlags & 0x80000) {
            CGGameUI::SetRegenEnabled(1);
        }

        CGGameUI::LeaveWorld();
    }

    for (uint32_t i = 0; i < 936; i++) {
        this->m_playerLocalMirrorLists[i].~TSList<void>();
    }

    Spell_C_SetAutoRangedCombatSpell(0);
    EventUnregister(EVENT_ID_IDLE, CGPlayer_C::AutoCombatModeEventHandler);
    this->m_combatModeFlags &= ~0xEu;
    CGGameUI::UpdateCombatMode();

    if (this->m_voidStorage) {
        delete this->m_voidStorage;
    }

    this->m_voidStorage = NULL;
}

// OFFSET: 0x005637C0/0x1401C1540
void CGPlayer_C::GetDefenseSkillRank(int32_t& rank, int32_t& bonus) const {
    int32_t defenseSpell = s_defenseSpellId;
    bonus = 0;
    rank = 0;
    SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], defenseSpell);

    if (ability && ability->m_spell == s_defenseSpellId) {
        this->GetExpandedSkillRank(ability->m_skillLine, rank, bonus);
    }

    bonus += static_cast<int32_t>(const_cast<CGPlayer_C*>(this)->GetCombatRatingPct(1) - 0.5f);
}

// OFFSET: 0x00562090/0x1401C0190
void CGPlayer_C::GetAttackSkillRank(int32_t hand, int32_t& rank, int32_t& bonus) const {
    bonus = 0;
    rank = 0;
    int32_t skillSpell = 0;

    if (hand < 3) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(hand, false);
        uint32_t subclass;

        if (!item) {
            subclass = ClientDBGetUnarmedWeapon();
        } else if (item->m_classID == 2) {
            subclass = item->m_subclassID;
        } else {
            subclass = 0xFFFFFFFF;
        }

        if (subclass < s_weaponSkills.Count()) {
            skillSpell = s_weaponSkills.m_data[subclass];
        }
    }

    SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], skillSpell);

    if (ability && ability->m_spell == skillSpell) {
        this->GetExpandedSkillRank(ability->m_skillLine, rank, bonus);
    }

    CGPlayer_C* player = const_cast<CGPlayer_C*>(this);
    bonus += static_cast<int32_t>(player->GetCombatRatingPct(0) - 0.5f);

    if (hand == 0) {
        bonus += static_cast<int32_t>(player->GetCombatRatingPct(20) - 0.5f);
    } else if (hand == 1) {
        bonus += static_cast<int32_t>(player->GetCombatRatingPct(21) - 0.5f);
    }
}

// OFFSET: 0x0055D100
int32_t CGPlayer_C::OnSignedResults(CDataStore* msg) {
    WOWGUID petition;
    msg->Get(petition.m_guid);
    WOWGUID signer;
    msg->Get(signer.m_guid);
    uint32_t result;
    msg->Get(result);

    if (signer != this->m_obj->m_guid) {
        CGPetitionInfo::AddSignature(petition, signer);
        return 1;
    }

    switch (result) {
        case 0:
            CGGameUI::DisplayError(ERR_PETITION_SIGNED);
            FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_CLOSED, NULL);
            break;
        case 1:
            CGGameUI::DisplayError(ERR_PETITION_ALREADY_SIGNED);
            break;
        case 2:
            CGGameUI::DisplayError(ERR_PETITION_IN_GUILD);
            break;
        case 3:
            CGGameUI::DisplayError(ERR_PETITION_CREATOR);
            break;
        case 5:
            CGGameUI::DisplayError(ERR_PETITION_NOT_SAME_SERVER);
            break;
        case 8:
            CGGameUI::DisplayError(ERR_PETITION_FULL);
            break;
        case 10:
            CGGameUI::DisplayError(ERR_PETITION_ALREADY_SIGNED_OTHER);
            break;
        case 11:
            CGGameUI::DisplayError(ERR_PETITION_RESTRICTED_ACCOUNT);
            break;
        default:
            ConsoleWrite("Petition error", DEFAULT_COLOR);
            break;
    }

    return 1;
}

// OFFSET: 0x005567D0
int32_t __stdcall CGPlayer_C::OnTrainerList(CDataStore* msg) {
    WOWGUID trainer;
    msg->Get(trainer.m_guid);
    uint32_t type;
    msg->Get(type);
    uint32_t trainerID;
    msg->Get(trainerID);
    uint32_t count;
    msg->Get(count);

    int32_t* spellIds = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    uint8_t* states = static_cast<uint8_t*>(alloca(count));
    int32_t* moneyCosts = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    uint8_t* levelReqs = static_cast<uint8_t*>(alloca(count));
    int32_t* skillReqs = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    uint32_t* skillRankReqs = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * count));
    int32_t* abilityReqs0 = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    int32_t* abilityReqs1 = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    int32_t* abilityReqs2 = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    int32_t* talentTabReqs = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(spellIds[i]));
        msg->Get(states[i]);
        msg->Get(reinterpret_cast<uint32_t&>(moneyCosts[i]));
        msg->Get(levelReqs[i]);
        msg->Get(reinterpret_cast<uint32_t&>(skillReqs[i]));
        msg->Get(skillRankReqs[i]);
        msg->Get(reinterpret_cast<uint32_t&>(abilityReqs0[i]));
        msg->Get(reinterpret_cast<uint32_t&>(abilityReqs1[i]));
        msg->Get(reinterpret_cast<uint32_t&>(abilityReqs2[i]));
        msg->Get(reinterpret_cast<uint32_t&>(talentTabReqs[i]));
    }

    char greeting[2048];
    msg->GetString(greeting, sizeof(greeting));
    const int32_t* abilityReqs[3] = { abilityReqs0, abilityReqs1, abilityReqs2 };

    if (trainer == s_lastTrainerRequest || trainer == CGGossipInfo::m_gossip) {
        CGClassTrainer::SetTrainer(trainer, trainerID, type);
        CGClassTrainer::AddServices(count, spellIds, moneyCosts, levelReqs, skillReqs, skillRankReqs, abilityReqs, talentTabReqs, states, greeting);
        FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_SHOW, NULL);
    }

    return 1;
}

// OFFSET: 0x00557EE0/0x1401B5020
void CGPlayer_C::TabardDrawCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (!key || !success) {
        return;
    }

    if (!g_guildInfoCache.GetRecord(key, WOWGUID(), NULL, NULL, false)) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    void (*noCallback)(const uint64_t& key, const WOWGUID& guid, void* param, bool success) = NULL;
    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (GuildGetGuildTabard(guid, WOWGUID::Make(key), noCallback, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        player->ApplyGuildTabard(emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x00557FA0
void CGPlayer_C::ShirtTabardDrawCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (!key || !success) {
        return;
    }

    if (!g_guildInfoCache.GetRecord(key, WOWGUID(), NULL, NULL, false)) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    void (*noCallback)(const uint64_t& key, const WOWGUID& guid, void* param, bool success) = NULL;
    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (GuildGetGuildTabard(guid, WOWGUID::Make(key), noCallback, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        player->ApplyShirtGuildTabard(emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x0055C1A0
void __stdcall CGPlayer_C::ReadItemResult(NETMESSAGE msgId, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (msgId == SMSG_READ_ITEM_RESULT_OK) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item) {
            return;
        }

        item->SetTranslated();
        CGItemText::DisplayText(guid, 1);
        return;
    }

    uint8_t result;
    msg->Get(result);

    if (!result) {
        CGItemText::DisplayText(guid, 1);
    } else if (result == 1) {
        CGItemText::DisplayText(guid, 0);
        uint32_t delay;
        msg->Get(delay);
        FrameScript_SignalEvent(SCRIPT_EVENT_ITEM_TEXT_TRANSLATION, "%f", delay * 0.001);
    } else if (guid == CGItemText::m_itemGUID) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ITEM_TEXT_CLOSED, NULL);
    }
}

// OFFSET: 0x0055C8C0
bool CGPlayer_C::ReportBagItemSubtypeMismatch(uint8_t bag) {
    if (bag == 0xFF) {
        return false;
    }

    WOWGUID guid = bag < this->m_bag.m_numSlots ? this->m_bag.m_slots[bag] : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || !item->GetBagFamily()) {
        return false;
    }

    int32_t index = CMath::fint_n(CMath::log2_(static_cast<float>(item->GetBagFamily()))) + 1;

    if (!index) {
        return false;
    }

    ItemBagFamilyRec* family = g_itemBagFamilyDB.GetRecord(index);

    if (!family) {
        return false;
    }

    CGGameUI::DisplayError(ERR_WRONG_BAG_TYPE_SUBCLASS, family->m_name);
    return true;
}

// PROVEN: b9832e48 IDENTICAL/NONE
// OFFSET: 0x0055C980/0x1401B0B20
void CGPlayer_C::StartGiftWrap(CGItem_C* item) {
    s_giftWrapItem = item->m_obj->m_guid;
    CGGameUI::LockItem(s_giftWrapItem);
    CursorSetResetMode(2);
    CursorSetMode(2);
}

// OFFSET: 0x0055CEF0
void CGPlayer_C::UpdateObjComponentVisuals(const VisibleItem_C* item, GEOCOMPONENTLINKS link) {
    int32_t visual = const_cast<VisibleItem_C*>(item)->GetItemEnchantmentVisualID();

    if (visual == -1) {
        return;
    }

    for (CM2Model* model = this->m_worldModel->m_attachList; model; model = model->m_attachNext) {
        if (model->m_attachmentId == static_cast<uint32_t>(link)) {
            CCharacterComponent::ComponentUtilAddItemVisual(model, visual);
            CGGameUI::UnitModelUpdate(this->m_obj->m_guid, 3);
            return;
        }
    }
}

// OFFSET: 0x0055D430
void CGPlayer_C::HandleSkinning(CGObject_C* object, bool autoInteract, bool a4) {
    float minRange;
    float maxRange;
    Spell_C_GetMinMaxRange(this, CGSpellBook::GetSkinningSpell(object), &minRange, &maxRange, object);
    C3Vector target = object->GetPosition();
    C3Vector position = this->GetPosition();
    float dx = position.x - target.x;
    float dy = position.y - target.y;
    float dz = position.z - target.z;

    if (maxRange * maxRange <= dx * dx + dz * dz + dy * dy && autoInteract) {
        this->SetTrackingTarget(object->m_obj->m_guid, 9);
    } else {
        Spell_C_CastSpell(CGSpellBook::GetSkinningSpell(object), NULL, object->m_obj->m_guid, false);
    }

    if (a4) {
        this->m_autoLoot = CGGameUI::IsAutoLooting() != 0;

        if (this->m_autoLoot && this->m_lootTarget) {
            CGLootInfo::DoAutoLoot(0);
        }
    }
}

// OFFSET: 0x0055FC30
int32_t OnUpdatePlayerRuneRegen(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    float rate = *static_cast<const float*>(value);
    uint32_t type = (offset - 0x1214) >> 2;
    uint32_t predicted = CGRuneInfo::m_predictedRunes;
    float current = player->m_player->m_playerRuneRegen1[type];

    if (rate < 0.0099999998f || current < 0.0099999998f) {
        return 1;
    }

    for (int32_t i = 0; i < 8; i++) {
        if (static_cast<uint32_t>(CGRuneInfo::GetRuneTypeByIndex(i, false)) != type || ((1 << i) & predicted) || !CGRuneInfo::GetRuneLastUsedTime(i)) {
            continue;
        }

        uint32_t now = FrameTime::s_curTimeMs;
        double progress = (now - CGRuneInfo::GetRuneLastUsedTime(i)) * rate * 0.001;
        float clamped = static_cast<float>(progress);

        if (progress > 1.0) {
            progress = 1.0;
            clamped = 1.0f;
        } else if (progress < 0.0) {
            progress = 0.0;
            clamped = 0.0f;
        }

        CGRuneInfo::SetRuneLastUsedTime(i, FrameTime::s_curTimeMs - static_cast<uint32_t>(progress * 1000.0 / current));
        CGRuneInfo::ComputeRuneRechargeTime(i, clamped);
        CGActionBar::UpdateCooldowns();
    }

    return 1;
}

// OFFSET: 0x005605F0/0x1401BE1C0
void CGPlayer_C::TalkToTalentMaster(const WOWGUID& guid, uint32_t cost) {
    WOWGUID target = guid ? guid : s_talentMaster;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    C3Vector position = this->GetPosition();
    C3Vector unitPosition = unit->GetPosition();
    float dy = unitPosition.y - position.y;
    float dx = unitPosition.x - position.x;
    float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;

    if (reach * reach < dx * dx + dy * dy + (unitPosition.z - position.z) * (unitPosition.z - position.z)) {
        return;
    }

    if (guid) {
        s_talentMaster = guid;
        s_talentWipeCost = cost;
        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_TALENT_WIPE, "%d", cost);
    } else if (s_talentWipeCost <= this->GetCoinage()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_TALENT_WIPE_CONFIRM));
        msg.Put(s_talentMaster.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    } else {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
    }
}

// OFFSET: 0x00554460/0x1401B04F0
void CGPlayer_C::SetAFK(const char* message) {
    const char* text = message;

    if (!text || !*text) {
        text = FrameScript_GetText("DEFAULT_AFK_MESSAGE", -1, GENDER_NOT_APPLICABLE);
    }

    if (!CGChat::m_afk) {
        if (CGChat::m_dnd) {
            CGChat::AddChatMessage(FrameScript_GetText("CLEARED_DND", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
            CGChat::m_dnd = 0;
        }

        char buffer[3000];
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("MARKED_AFK_MESSAGE", -1, GENDER_NOT_APPLICABLE), text);
        CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
        CGChat::m_afk = 1;
    }

    UserClientMessageChatAfk msg(text);
    ClientServices::Send(&msg);
}

// OFFSET: 0x00566480
void CGPlayer_C::EquipItem(CGItem_C* item) {
    if (!item || !this->m_characterComponent || item->GetDisplayID() <= 0) {
        return;
    }

    VisibleItem_C visible;
    visible.m_itemId = item->m_obj->m_entryID;
    visible.m_enchantment[0] = 0;
    visible.m_enchantment[1] = 0;

    for (int32_t i = 0; i < 19; i++) {
        if (!((1 << i) & 0x7C3FD)) {
            continue;
        }

        WOWGUID slotGuid = i < this->m_bag.m_numSlots ? this->m_bag.m_slots[i] : WOWGUID();

        if (slotGuid == item->m_obj->m_guid) {
            this->EquipVisibleItem(&visible, i);
        }
    }
}

// OFFSET: 0x00566B80/0x1401C5400
void CGPlayer_C::FillPartyMemberState(PartyMemberState* state) {
    state->m_flags = 1;

    if (this->m_player->m_playerFlags & 0x10) {
        state->m_flags = 9;
    }

    if (this->m_unit->m_unitFieldBytes2[1] & 0x1) {
        state->m_flags |= 0x2;
    }

    if (this->m_unit->m_unitFieldBytes2[1] & 0x4) {
        state->m_flags |= 0x10;
    }

    if (this->m_unit->m_unitFieldHealth <= 0) {
        state->m_flags |= 0x4;
    }

    if (this->m_player->m_playerFlags & 0x2) {
        state->m_flags |= 0x40;
    }

    if (this->m_player->m_playerFlags & 0x4) {
        state->m_flags |= 0x80;
    }

    if (this->m_unit->m_unitDynamicFlags & 0x40) {
        state->m_flags |= 0x100;
    }

    if (this->HasVehicleTransport()) {
        state->m_flags |= 0x200;
    }

    state->m_health = this->m_unit->m_unitFieldHealth;
    state->m_maxHealth = this->m_unit->m_unitFieldMaxhealth;
    uint8_t powerType = this->m_unit->m_unitFieldBytes0[3];
    state->m_powerType = powerType;
    state->m_power = static_cast<uint16_t>(this->GetPower(powerType));
    state->m_maxPower = static_cast<uint16_t>(this->GetMaxPower(powerType));
    state->m_level = static_cast<uint16_t>(this->m_unit->m_unitFieldLevel);

    if (this->GetTransportGUID()) {
        state->m_zoneID = 0;
    } else {
        state->m_zoneID = static_cast<uint16_t>(CGGameUI::m_areaID);
        state->m_zoneID = static_cast<uint16_t>(CGGameUI::m_wmoGroupID);
        C3Vector position = this->GetPosition();
        state->m_posX = static_cast<int16_t>(static_cast<int32_t>(position.x));
        state->m_posY = static_cast<int16_t>(static_cast<int32_t>(position.y));
        state->m_posZ = static_cast<int16_t>(static_cast<int32_t>(position.z));
    }

    uint32_t limit = this->GetAuraCount() >= 64 ? 64 : this->GetAuraCount();

    if (limit > state->m_auras.Count()) {
        state->m_auras.SetCount(limit);
    }

    uint32_t count = 0;

    for (uint32_t i = 0; i < limit; i++) {
        AuraData* aura = this->GetAura(i);

        if (!aura->m_spellId || !(aura->m_flags & 0x7)) {
            continue;
        }

        state->m_auras[count].m_spellId = aura->m_spellId;
        state->m_auras[count].m_flags = aura->m_flags;

        if (aura->m_flags & 0x40) {
            state->m_auras[count].m_effectAmounts[0] = aura->m_effectAmounts[0];
            state->m_auras[count].m_effectAmounts[1] = aura->m_effectAmounts[1];
            state->m_auras[count].m_effectAmounts[2] = aura->m_effectAmounts[2];
        }

        count++;
    }

    state->m_numAuras = count;

    for (uint32_t i = count; i < limit; i++) {
        state->m_auras[i].m_flags = 0;
        state->m_auras[i].m_spellId = 0;
    }

    CGUnitData* unit = this->m_unit;
    state->m_pet.m_guid = unit->m_unitFieldCharm ? unit->m_unitFieldCharm : unit->m_unitFieldSummon;

    if (state->m_flags & 0x200) {
        CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));
        VehicleRec* vehicle = transport && transport->m_vehicle ? transport->m_vehicle->m_vehicleRec : NULL;
        uint8_t seat = this->m_movementData.m_transportSeat;

        if (vehicle && seat < 8) {
            state->m_vehicleSeatId = vehicle->m_seatID[seat];
        } else {
            state->m_vehicleSeatId = 0;
        }
    } else {
        state->m_vehicleSeatId = 0;
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        state->m_phaseShift = *PhaseShift_C_GetPhase();
    }
}

// OFFSET: 0x0056AD20
void CGPlayer_C::UpdatePartyMemberState() {
    WOWGUID guid = this->m_obj->m_guid;
    PartyMemberState* remote = CGPartyInfo::GetRemoteStats(guid);
    WOWGUID raidGuid = this->m_obj->m_guid;
    PartyMemberState* raid = CGRaidInfo::GetRemoteStats(raidGuid);
    PartyMemberState oldState;
    PartyMemberState newState;

    if (remote || (remote = raid) != NULL) {
        oldState.Copy(remote);
        newState.Copy(remote);
        this->FillPartyMemberState(&newState);
        uint32_t changes = CGPlayer_C::BuildPartyMemberStatsChanges(oldState, newState);
        WOWGUID self = this->m_obj->m_guid;
        CGPlayer_C::DispatchPartyMemberStatsChanges(self, changes, oldState, newState);
    }
}

// OFFSET: 0x00567080
void __stdcall CGPlayer_C::ServerBindConfirm(WOWGUID item, uint64_t a2, uint32_t a3, uint64_t a4, uint8_t a5) {
    CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!object) {
        return;
    }

    CGGameUI::SetCursorItem(item, WOWGUID::Make(a2), a3, 1, 0, 1);
    uint32_t index = FindEmptySwapIndex();
    ITEMSWAP* swap = &s_itemSwaps.m_data[index];
    swap->m_srcContainer.m_guid = a2;
    swap->m_srcSlot = a3;
    swap->m_dstContainer.m_guid = a4;
    swap->m_dstSlot = a5;
    swap->m_itemId = object->m_obj->m_entryID;
    FrameScript_SignalEvent(SCRIPT_EVENT_AUTOEQUIP_BIND_CONFIRM, "%d", index);
}

// OFFSET: 0x00568420
bool CGPlayer_C::ToggleFarSight(CGObject_C* object) {
    WOWGUID farsight = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerFarsight : WOWGUID();
    CGObject_C* target = ClntObjMgrObjectPtr(farsight, TYPEMASK_OBJECT, __FILE__, __LINE__);
    bool hasFarsight = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && this->m_player->m_playerFarsight;

    if (hasFarsight && !target) {
        if (Spell_C_IsTargeting()) {
            Spell_C_StopTargeting();
        }

        CGGameUI::ResetCamera(WOWGUID());
        CGUnit_C::SetActiveMover(this->IsLocalClientControlled() ? this->m_obj->m_guid.m_guid : 0);
        CGInputControl::GetActive()->UpdatePlayer(FrameTime::s_curTimeMs, 1);
        return false;
    }

    if (target && (object || !(this->m_combatModeFlags & 0x1))) {
        if (!(this->m_combatModeFlags & 0x1)) {
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_FAR_SIGHT));
            msg.Put(static_cast<uint8_t>(1));
            msg.Finalize();
            ClientServices::Send2(&msg);
            this->m_combatModeFlags |= 0x1;
        }

        this->SetCombatMode(true);

        if ((target->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(target)->IsLocalClientControlled()) {
            CGUnit_C::SetActiveMover(target->m_obj->m_guid.m_guid);
            CGWorldFrame::s_currentWorldFrame->SetCameraTarget(target, 0);
        } else {
            CGUnit_C::SetActiveMover(this->IsLocalClientControlled() ? this->m_obj->m_guid.m_guid : 0);
            CGInputControl::GetActive()->UpdatePlayer(OsGetAsyncTimeMs(), 1);
            CGWorldFrame::s_currentWorldFrame->SetCameraTarget(target, 0);
        }

        CGInputControl::GetActive()->UpdatePlayer(FrameTime::s_curTimeMs, 1);
        return true;
    }

    if (this->m_combatModeFlags & 0x1) {
        this->m_combatModeFlags &= ~0x1u;
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_FAR_SIGHT));
        msg.Put(static_cast<uint8_t>(0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (Spell_C_IsTargeting()) {
        Spell_C_StopTargeting();
    }

    CGGameUI::ResetCamera(WOWGUID());
    CGUnit_C::SetActiveMover(this->IsLocalClientControlled() ? this->m_obj->m_guid.m_guid : 0);
    CGInputControl::GetActive()->UpdatePlayer(FrameTime::s_curTimeMs, 1);
    return true;
}

// OFFSET: 0x00568610
void CGPlayer_C::ClearFarSight() {
    if (!(this->m_combatModeFlags & 0x1)) {
        return;
    }

    this->ToggleFarSight(NULL);
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera) {
        camera->PickVehicleCamera();
        this->VehiclePassengerUpdateWorldCameraStateInstantly();
    }
}

// OFFSET: 0x0056A5E0
int32_t FarsightChangeHandler(WOWGUID guid, uint32_t offset, uint32_t count, const void* value, void* param) {
    if (guid != ClntObjMgrGetActivePlayer()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(WOWGUID::Make(player->GetFarSightGuid()), TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        player->ToggleFarSight(target);
        CGPetInfo::FindPossessedSpell();
    } else {
        player->ClearFarSight();
        CGPetInfo::SetPossessedSpell(0);
    }

    CGPetInfo::UpdatePetTempBar();
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_FARSIGHT_FOCUS_CHANGED, NULL);

    if (player->m_unit->m_unitFieldHealth <= 0) {
        CGGameUI::UpdateActivePlayer();
    }

    return 1;
}

// OFFSET: 0x00552830
int32_t Packet_SMSG_VOICE_SESSION_ROSTER_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t session = 0;
    msg->Get(session);
    uint16_t networkId = 0;
    msg->Get(networkId);
    uint8_t type = 0;
    msg->Get(type);
    char name[128];
    msg->GetString(name, sizeof(name));
    uint8_t key[16];
    msg->GetArray(key, sizeof(key));
    uint32_t ip = 0;
    msg->Get(ip);
    uint16_t port = 0;
    msg->Get(port);
    char address[32];
    SStrPrintf(address, sizeof(address), "%d.%d.%d.%d:%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, port);
    uint8_t count = 0;
    msg->Get(count);
    uint64_t self = 0;
    msg->Get(self);
    uint8_t selfNetworkId = 0;
    msg->Get(selfNetworkId);
    uint8_t selfFlags = 0;
    msg->Get(selfFlags);
    VoiceRosterMember members[40];
    int32_t remote = count - 1;

    for (int32_t i = 0; i < remote; i++) {
        uint64_t guid = 0;
        msg->Get(guid);
        members[i].m_guid.m_guid = guid;
        uint8_t value = 0;
        msg->Get(value);
        members[i].m_priority = value;
        value = 0;
        msg->Get(value);
        members[i].m_talkerPriority = value;
        value = 0;
        msg->Get(value);
        members[i].m_flags = value;
        SStrCopy(members[i].m_address, address, sizeof(members[i].m_address));
    }

    VoiceSessionRosterUpdate(session, networkId, type, name, key, self, selfNetworkId, selfFlags, remote, members);
    ConsolePrintf("VoiceSessionRosterUpdate - type %d, channelName <%s>, %d members", type, name, count);
    return 1;
}

// OFFSET: 0x00552A40
int32_t Packet_SMSG_AVAILABLE_VOICE_CHANNEL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t session = 0;
    msg->Get(session);
    uint8_t type = 0;
    msg->Get(type);
    char name[128];
    msg->GetString(name, sizeof(name));
    uint64_t self = 0;
    msg->Get(self);
    int32_t current = ComSatClient_IsCurrentSession(session);
    VoiceSessionLeave(self, session, type);
    ComSatClient_PopulateSessionDataWithoutJoin(session, type, name, self);

    if (current) {
        ComSatClient_SetCurrentSession(type, name);
    }

    ConsolePrintf("OnAvailableVoiceSession - type %d, name = <%s>", type, name);
    return 1;
}

// OFFSET: 0x00552B10
int32_t Packet_SMSG_VOICE_SESSION_LEAVE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t talker = 0;
    msg->Get(talker);
    uint64_t session = 0;
    msg->Get(session);
    VoiceSessionLeave(talker, session, 4);
    return 1;
}

// OFFSET: 0x00552B70
int32_t Packet_SMSG_VOICE_SET_TALKER_MUTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    guid.m_guid = 0;
    msg->Get(guid.m_guid);
    uint8_t muted = 0;
    msg->Get(muted);
    VoiceSessionSetTalkerMuted(guid, muted == 0);
    return 1;
}

// OFFSET: 0x00555C70
int32_t Packet_SMSG_VOICE_PARENTAL_CONTROLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t value = 0;
    msg->Get(value);
    int32_t voiceChat = value;
    value = 0;
    msg->Get(value);
    int32_t microphone = value;

    if (!s_cvarEnableVoiceChat || !s_cvarEnableMicrophone) {
        return 1;
    }

    int32_t oldVoiceChat = s_cvarEnableVoiceChat->m_intValue;
    int32_t oldMicrophone = s_cvarEnableMicrophone->m_intValue;
    char text[10];
    SStrPrintf(text, sizeof(text), "%i", voiceChat);
    s_cvarEnableVoiceChat->Set(text, true, false, false, true);
    SStrPrintf(text, sizeof(text), "%i", microphone);
    s_cvarEnableMicrophone->Set(text, true, false, false, true);

    if (!voiceChat && oldVoiceChat) {
        if (CGGameUI::m_initialized) {
            CGChat::DisplayParentControlsNotice(ERR_VOICE_CHAT_PARENTAL_DISABLE_ALL);
        }
    } else if (!microphone && oldMicrophone && CGGameUI::m_initialized) {
        CGChat::DisplayParentControlsNotice(ERR_VOICE_CHAT_PARENTAL_DISABLE_MIC);
    }

    return 1;
}

// OFFSET: 0x0055BFA0/0x1401B8E30
int32_t CGPlayer_C::GetVirtualItemDisplayRec(uint32_t slot, ItemDisplayInfoRec& rec) const {
    return g_itemDisplayInfoDB.GetRecord(this->GetVirtualItemDisplayID(slot), &rec);
}

// OFFSET: 0x0055B050/0x1401B7DE0
bool CGPlayer_C::SpellUsesWeaponSkill(const SpellRec* spell) const {
    if (!spell->m_spellEquippedItemsID) {
        return false;
    }

    const SpellEquippedItemsRec* items = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID);

    if (!items) {
        return false;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if ((spell->m_attributesExB & 0x8000000) || (effect && effect->m_effect == 25)) {
        return false;
    }

    if (items->m_equippedItemClass != 2 || !items->m_equippedItemSubclass) {
        return false;
    }

    for (uint32_t hand = 0; hand < 3; hand++) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(hand, false);

        if (item && item->m_classID == items->m_equippedItemClass && ((1 << item->m_subclassID) & items->m_equippedItemSubclass) && s_weaponSkills.m_data[item->m_subclassID]) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0055CA50/0x1401B9DB0
int32_t CGPlayer_C::GetSpellCastingTime(const SpellRec* spell) const {
    if (!spell) {
        return 0;
    }

    int32_t castTime = 0;
    const SpellScalingRec* scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID);

    if (scaling) {
        if (!Spell_C_SpellScalingCastTime(this, scaling, castTime)) {
            return 0;
        }
    } else {
        const SpellCastTimesRec* times = g_spellCastTimesDB.GetRecord(spell->m_castingTimeIndex);

        if (!times) {
            return 0;
        }

        int32_t rank = this->GetSpellRank(spell);
        int32_t baseLevel = 0;

        if (spell->m_spellLevelsID) {
            const SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

            if (levels) {
                baseLevel = levels->m_baseLevel;
            }
        }

        castTime = times->m_base + times->m_perLevel * (rank / 5 - baseLevel);

        if (castTime < times->m_minimum) {
            castTime = times->m_minimum;
        }
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        Spell_C_ModifySpellValue(spell, castTime, 10);
    }

    if (!(spell->m_attributes & 0x30) && !(spell->m_attributesExC & 0x20000000) && castTime > 0) {
        float speed = this->m_unit->m_unitModCastSpeed;

        if (fnotequal(speed, 1.0f)) {
            castTime = static_cast<int32_t>(static_cast<float>(castTime) * speed);
        }
    }

    return castTime <= 0 ? 0 : castTime;
}

// OFFSET: 0x0055D550/0x1401B1070
uint64_t CGPlayer_C::GetInspectGuildGUID() const {
    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        return s_inspectData->m_guildGUID;
    }

    return 0;
}

// OFFSET: 0x0055D580/0x1401B10A0
uint32_t CGPlayer_C::GetInspectGuildLevel() const {
    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        return s_inspectData->m_guildLevel;
    }

    return 0;
}

// OFFSET: 0x0055D5B0/0x1401B10D0
uint32_t CGPlayer_C::GetInspectGuildNumMembers() const {
    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        return s_inspectData->m_guildNumMembers;
    }

    return 0;
}

// OFFSET: 0x0055D5E0/0x1401B1100
uint64_t CGPlayer_C::GetInspectGuildXP() const {
    if (s_inspectData && s_inspectData->m_guid == this->m_obj->m_guid) {
        return s_inspectData->m_guildXP;
    }

    return 0;
}

// OFFSET: 0x0055D650
int32_t CGPlayer_C::GetAttackPowerContributionFromStat(int32_t stat, int32_t value) {
    const ChrClassesRec* classRec = g_chrClassesDB.GetRecord(this->m_unit->m_unitFieldBytes0[1]);

    if (!classRec) {
        return 0;
    }

    int32_t bonus = value - 10;

    if (bonus < 0) {
        bonus = 0;
    }

    int32_t attackPower = 0;

    if (!stat) {
        return bonus * classRec->unk_002C;
    }

    if (stat == 1) {
        attackPower = bonus * classRec->unk_0030;
        const SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (form && (form->m_flags & 0x20)) {
            attackPower += bonus * classRec->unk_002C;
        }
    }

    return attackPower;
}

// OFFSET: 0x0055DB50
bool CGPlayer_C::IsInArena() {
    const MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    return map && map->m_instanceType == 4;
}

// OFFSET: 0x005613C0
void CGPlayer_C::UpdateRepopTimer() {
    uint8_t bytes = this->m_player->m_playerFieldBytes[0];

    if (bytes & 0x10) {
        CGGameUI::SetRepopTimeLeft(0);
    } else if ((bytes & 0x8) && !(this->m_player->m_playerFlags & 0x4000)) {
        CGGameUI::SetRepopTimeLeft(360000);
    } else {
        CGGameUI::ClearRepopTime();
    }
}

// OFFSET: 0x005615C0/0x1401BF490
int32_t CGPlayer_C::GetSkillRank(int32_t skillId) const {
    int32_t index = this->GetSkillIndex(skillId);

    if (index < 0) {
        return 0;
    }

    uint16_t modifier = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? this->m_player->m_playerSkillModifier0[index] : 0;
    int32_t rank = modifier + this->GetSkillValue(index);

    return rank < 0 ? 0 : rank;
}

// OFFSET: 0x00561620/0x1401BF510
int32_t CGPlayer_C::GetSpellRank(const SpellRec* spell) const {
    if (this->SpellUsesWeaponSkill(spell)) {
        return 5 * this->m_unit->m_unitFieldLevel;
    }

    const SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], spell->m_id);
    int32_t rank = ability ? this->GetSkillRank(ability->m_skillLine) : 5 * this->m_unit->m_unitFieldLevel;

    if (spell->m_spellLevelsID) {
        const SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

        if (levels && levels->m_maxLevel > 0 && rank >= 5 * levels->m_maxLevel) {
            rank = 5 * levels->m_maxLevel;
        }
    }

    return rank < 0 ? 0 : rank;
}

// OFFSET: 0x00561D90/0x1401BFDA0
void CGPlayer_C::GetComText(char* buffer, uint32_t size) const {
    if (this->m_player->m_playerFlags & 0x80000) {
        const char* text = FrameScript_GetText("CHAT_FLAG_COM", -1, GENDER_NOT_APPLICABLE);

        if (text && *text) {
            SStrPack(buffer, text, size);
        }
    }
}

// OFFSET: 0x00561F30/0x1401BFFA0
void CGPlayer_C::ChangeStandState(uint32_t standState) {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000))) {
        return;
    }

    if ((this->m_unit->m_unitFieldFlags & 0x100000) || this->m_unit->m_unitFieldHealth <= 0 || (this->m_unit->m_unitFieldFlags2 & 1)) {
        return;
    }

    if (this->IsInVehicle() || (this->m_animationState & 0x10) || this->m_castSpellId || !this->IsLocalClientControlled()) {
        return;
    }

    if (this->m_unit->m_unitChannelSpell) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

        if (!spell || !(spell->m_attributes & 0x8000000)) {
            return;
        }
    }

    uint32_t flags = this->m_movement->m_flags;

    if ((flags & 0x30) && (standState == 3 || standState == 1)) {
        return;
    }

    if (standState && (flags & 0x170080F)) {
        return;
    }

    if (standState != this->GetClientStandState()) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_STAND_STATE_CHANGE));
        msg.Put(standState);
        msg.Finalize();
        ClientServices::Send2(&msg);
        this->UpdateClientStandState(this->m_clientStandState, standState);
    }
}

// OFFSET: 0x00562370/0x1401B9940
void CGPlayer_C::OpenWrappedItem(CGItem_C* item) {
    uint8_t bag;
    uint8_t slot;

    if (!this->GetPackAndSlot(item, &bag, &slot)) {
        return;
    }

    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("UnwrapGift", NULL, &properties, NULL, __FILE__, __LINE__, "CGPlayer_C::OpenWrappedItem");
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_OPEN_ITEM));
    msg.Put(bag);
    msg.Put(slot);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00562AE0/0x1401C0840
void CGPlayer_C::SendTextEmote(const EmotesTextRec* emote, WOWGUID target, int32_t silent) {
    if (!emote || (this->m_unit->m_unitFieldFlags & 0x1000000)) {
        return;
    }

    const EmotesRec* emoteRec = g_emotesDB.GetRecord(emote->m_emoteID);

    if (!emoteRec) {
        return;
    }

    int32_t moving;
    int32_t inCombat;

    if (!this->CanPlayEmote(emoteRec->m_emoteFlags, 1, &moving, this->m_combat.m_attacking != 0, &inCombat, (this->m_animationState & 0x10) != 0)) {
        return;
    }

    if ((moving || inCombat) && this->IsClientControlled()) {
        if (!silent) {
            if (moving) {
                CGGameUI::DisplayError(ERR_NOEMOTEWHILERUNNING);
            } else if (inCombat) {
                CGGameUI::DisplayError(ERR_NOT_IN_COMBAT);
            }
        }

        return;
    }

    if (this->GetClientStandState() == 3 && emoteRec->m_emoteSpecProc != 1) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (!(emoteRec->m_emoteFlags & 0x800000) && now < this->unk_217C) {
        return;
    }

    this->unk_217C = now + s_textEmoteCooldown;

    if (target == this->m_obj->m_guid) {
        target = WOWGUID();
    }

    if (emoteRec->m_emoteSpecProc == 1 && ((1 << emoteRec->m_emoteSpecProcParam) & s_textEmoteStandStates)) {
        this->ChangeStandState(emoteRec->m_emoteSpecProcParam);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SEND_TEXT_EMOTE));
    msg.Put(static_cast<uint32_t>(emote->m_id));
    uint32_t sex;
    uint32_t race;

    if (this->m_unit->m_unitFieldDisplayid == this->m_unit->m_unitFieldNativedisplayid) {
        sex = this->m_unit->m_unitFieldBytes0[2];
        race = this->GetCurrentClientRace();
    } else {
        sex = this->GetDisplaySex();
        race = this->GetDisplayRace();
    }

    msg.Put(static_cast<uint32_t>(SI3::GetTextEmoteSoundIndex(emote->m_id, race, sex)));
    msg.Put(target.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00563460/0x1401C12D0
bool CGPlayer_C::CanSaveTabardNow() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    WOWGUID none = WOWGUID();
    WOWGUID guild = player->m_obj->m_dataType == 1 ? player->m_obj->m_data : WOWGUID();

    return g_guildInfoCache.GetRecord2(guild.m_guid, none, NULL, NULL, false) && !s_tabardSavePending;
}

// OFFSET: 0x005634E0
const CGItem_C* CGPlayer_C::GetEquippedItemForSpell(const SpellEquippedItemsRec* items, uint32_t slotMask) {
    if ((this->m_unit->m_unitFieldFlags & 0x200000) || static_cast<int8_t>(this->m_unit->m_unitFieldFlags2) < 0) {
        if (this->IsDisarmed(0)) {
            slotMask &= ~0x8000u;
        }

        if (this->IsDisarmed(1)) {
            slotMask &= ~0x10000u;
        }
    }

    if (this->m_unit->m_unitFieldFlags2 & 0x400) {
        slotMask &= ~0x20000u;
    }

    for (uint32_t slot = 0; slot < 23; slot++) {
        if (!((1 << slot) & slotMask)) {
            continue;
        }

        CGItem_C* item = this->m_bag.GetItemPointer(slot);

        if (!item) {
            continue;
        }

        const CGItemData* data = item->m_item;

        if (((data->m_itemFieldFlags & 8) || !data->m_itemFieldMaxdurability || data->m_itemFieldDurability) && !(data->m_itemFieldFlags & 0x10) && items->m_equippedItemClass == static_cast<int32_t>(item->GetClassID()) && ((1 << item->GetSubtypeID()) & items->m_equippedItemSubclass)) {
            return item;
        }
    }

    return NULL;
}

// OFFSET: 0x00565A40/0x1401C3BF0
void CGPlayer_C::SaveTabard(int32_t emblemStyle, int32_t emblemColor, int32_t borderStyle, int32_t borderColor, int32_t background, WOWGUID npc) {
    if (emblemStyle < 0 || emblemColor < 0 || borderStyle < 0 || borderColor < 0 || background < 0 || emblemStyle >= 196 || emblemColor >= 17 || borderStyle >= 6 || borderColor >= 17 || background >= 51) {
        CGGameUI::DisplayError(ERR_GUILDEMBLEM_INVALID_TABARD_COLORS);
        return;
    }

    WOWGUID none = WOWGUID();
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(this->GetGuildGUID().m_guid, none, NULL, NULL, false);

    if (!stats) {
        CGGameUI::DisplayError(ERR_GUILDEMBLEM_NOGUILD);
        return;
    }

    if (emblemStyle == stats->m_emblemStyle && emblemColor == stats->m_emblemColor && borderStyle == stats->m_borderStyle && borderColor == stats->m_borderColor && background == stats->m_background) {
        CGGameUI::DisplayError(ERR_GUILDEMBLEM_SAME);
        return;
    }

    if (!this->IsGuildLeader()) {
        CGGameUI::DisplayError(ERR_GUILDEMBLEM_NOTGUILDMASTER);
        return;
    }

    if (this->GetCoinage() < static_cast<uint32_t>(GuildGetTabardCost())) {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_SAVE_GUILD_EMBLEM));
    msg.Put(npc.m_guid);
    msg.Put(static_cast<uint32_t>(emblemStyle));
    msg.Put(static_cast<uint32_t>(emblemColor));
    msg.Put(static_cast<uint32_t>(borderStyle));
    msg.Put(static_cast<uint32_t>(borderColor));
    msg.Put(static_cast<uint32_t>(background));
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_tabardSavePending = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_TABARD_SAVE_PENDING, NULL);
}

// OFFSET: 0x00566B30
void CGPlayer_C::ModifyCategoryCooldown(int32_t& cooldown, int32_t category) {
    for (CATEGORYCOOLDOWN* entry = this->m_categoryCooldowns.Head(); entry; entry = this->m_categoryCooldowns.Next(entry)) {
        if (entry->m_category == category) {
            if (entry->m_cooldown <= cooldown) {
                cooldown -= entry->m_cooldown;
            } else {
                cooldown = 0;
            }

            return;
        }
    }
}

// OFFSET: 0x00567150/0x1401C5A90
int32_t CGPlayer_C::GetModDamageDonePos(int32_t schoolMask) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    if ((schoolMask & 0x7E) && (this->m_auraEffectMask[45] & 0x40)) {
        return static_cast<int32_t>(static_cast<float>(this->m_unit->m_unitFieldAttackPower + this->m_unit->m_unitFieldAttackPowerModPos + this->m_unit->m_unitFieldAttackPowerModNeg) * (this->m_unit->m_unitFieldAttackPowerMultiplier + 1.0f) * this->m_player->m_playerFieldOverrideSpellPowerByApPct * 0.0099999998f);
    }

    int32_t result = 0;
    bool found = false;
    uint32_t bit = 1;

    for (uint32_t i = 0; i < 7; i++) {
        if ((schoolMask & bit) && (!found || static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(this->m_player->m_playerFieldModDamageDonePos[i])) * this->m_player->m_playerFieldModSpellPowerPct) > result)) {
            result = static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(this->m_player->m_playerFieldModDamageDonePos[i])) * this->m_player->m_playerFieldModSpellPowerPct);
            found = true;
        }

        bit = _rotl(bit, 1);
    }

    return result;
}

// OFFSET: 0x00567280/0x1401C5BD0
int32_t CGPlayer_C::GetModDamageDoneNeg(int32_t schoolMask) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || ((schoolMask & 0x7E) && (this->m_auraEffectMask[45] & 0x40))) {
        return 0;
    }

    int32_t result = 0;
    bool found = false;
    uint32_t bit = 1;

    for (uint32_t i = 0; i < 7; i++) {
        if ((schoolMask & bit) && (!found || static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(this->m_player->m_playerFieldModDamageDoneNeg[i])) * this->m_player->m_playerFieldModSpellPowerPct) < result)) {
            result = static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(this->m_player->m_playerFieldModDamageDoneNeg[i])) * this->m_player->m_playerFieldModSpellPowerPct);
            found = true;
        }

        bit = _rotl(bit, 1);
    }

    return result;
}

// OFFSET: 0x00567360/0x1401C5CB0
int32_t CGPlayer_C::GetModHealingDonePos() const {
    if (this->m_auraEffectMask[45] & 0x40) {
        return static_cast<int32_t>(static_cast<float>(this->m_unit->m_unitFieldAttackPower + this->m_unit->m_unitFieldAttackPowerModPos + this->m_unit->m_unitFieldAttackPowerModNeg) * (this->m_unit->m_unitFieldAttackPowerMultiplier + 1.0f) * this->m_player->m_playerFieldOverrideSpellPowerByApPct * 0.0099999998f);
    }

    return static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(this->m_player->m_playerFieldModHealingDonePos)) * this->m_player->m_playerFieldModSpellPowerPct);
}

// OFFSET: 0x00563F00/0x1401C1E70
float CGPlayer_C::GetModDamageDonePct(int32_t schoolMask) const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0.0f;
    }

    float result = 1.0f;
    bool found = false;
    uint32_t bit = 1;

    for (uint32_t i = 0; i < 7; i++) {
        if ((bit & schoolMask) && (!found || *reinterpret_cast<const float*>(&this->m_player->m_playerFieldModDamageDonePct[i]) > result)) {
            result = *reinterpret_cast<const float*>(&this->m_player->m_playerFieldModDamageDonePct[i]);
            found = true;
        }

        bit = _rotl(bit, 1);
    }

    return result;
}

// OFFSET: 0x00565DA0/0x1401C4300
const VisibleItem_C* CGPlayer_C::GetVirtualVisibleItem(uint32_t slot, bool ignoreDisarm) const {
    const VisibleItem_C* effectItem = NULL;
    const CEffectVirtualItem* effect = this->FindVirtualItemEffect(slot);

    if (effect) {
        effectItem = &effect->m_item;
    }

    if (slot == 0) {
        if (ignoreDisarm || !this->IsDisarmed(0)) {
            return effectItem ? effectItem : &this->m_player->m_playerVisibleItems[15];
        }
    } else if (slot == 1) {
        if (ignoreDisarm || !this->IsDisarmed(1)) {
            return effectItem ? effectItem : &this->m_player->m_playerVisibleItems[16];
        }
    } else if (slot == 2 && (ignoreDisarm || !(this->m_unit->m_unitFieldFlags2 & 0x400))) {
        return effectItem ? effectItem : &this->m_player->m_playerVisibleItems[17];
    }

    return NULL;
}

// OFFSET: 0x00565E70/0x1401C43F0
const CGUnitVirtualItem* CGPlayer_C::GetVirtualItem(uint32_t slot, bool ignoreDisarm) const {
    const VisibleItem_C* item = this->GetVirtualVisibleItem(slot, ignoreDisarm);

    if ((!ignoreDisarm && (!item || item->m_itemId <= 0)) || !item || !abs(item->m_itemId)) {
        return NULL;
    }

    CGUnitVirtualItem* virtualItem = const_cast<CGUnitVirtualItem*>(&this->m_virtualItems[slot]);
    VisibleItem_C* visible = const_cast<VisibleItem_C*>(item);
    virtualItem->m_classID = visible->GetClassID();
    virtualItem->m_subclassID = visible->GetSubtypeID();
    virtualItem->m_material = visible->GetMaterial();
    virtualItem->m_inventoryType = visible->GetInventoryType();
    virtualItem->m_sheatheType = visible->GetSheatheType();
    virtualItem->m_soundOverrideSubclassID = visible->GetSoundOverrideSubclassID();

    return virtualItem;
}

// OFFSET: 0x00565F00
uint32_t CGPlayer_C::GetVirtualItemDisplayID(uint32_t slot) const {
    const VisibleItem_C* item = this->GetVirtualVisibleItem(slot, false);

    return item ? const_cast<VisibleItem_C*>(item)->GetDisplayID() : 0;
}

// OFFSET: 0x00558360/0x1401B5510
const char* CGPlayer_C::GetPlayerGuildNameText() {
    if (this->m_obj->m_dataType != 1 || !this->m_obj->m_data) {
        return NULL;
    }

    WOWGUID guid = this->m_obj->m_guid;
    WOWGUID guild = this->m_obj->m_dataType == 1 ? this->m_obj->m_data : WOWGUID();
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(guild.m_guid, guid, PlayerNameGuildCallback, this, false);

    return stats ? stats->m_name : NULL;
}

// OFFSET: 0x005583D0/0x1401B5590
void CGPlayer_C::GetPlayerGuildNameTextWithTitle(char* buffer, uint32_t size, bool showRank) {
    if (!buffer) {
        return;
    }

    const char* name = this->GetPlayerGuildNameText();

    if (!name) {
        *buffer = 0;
        return;
    }

    const char* title;

    if (showRank && (title = CGGuildInfo::GetGuildReputationTitleString(this->m_obj->m_guid.m_guid)) != NULL) {
        SStrPrintf(buffer, size, title, name);
    } else {
        SStrCopy(buffer, name, size);
    }
}

// OFFSET: 0x00557E10
void CGPlayer_C::ApplyShirtGuildTabard(int32_t emblemStyle, int32_t emblemColor, int32_t borderStyle, int32_t borderColor, int32_t background) {
    if (this->m_characterComponent) {
        this->m_characterComponent->ApplyShirtGuildColor(1, emblemStyle, emblemColor, borderStyle, borderColor, background);
        CImVector backgroundColor = 0;
        CImVector border = 0;
        CImVector emblem = 0;
        char fileName[64];
        CGObject_C::GetGuildLogoInfo(background, borderColor, emblemColor, emblemStyle, backgroundColor, border, emblem, fileName, sizeof(fileName));
        CGObject_C::ReplaceGuildTextures(this->m_characterComponent->m_data.m_model, backgroundColor, border, emblem, fileName);
        CGGameUI::UnitModelUpdate(this->m_obj->m_guid, 3);
    }

    if (this->m_displayTransition) {
        this->m_displayTransition->ApplyShirtGuildColor(1, emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x00558260/0x1401AEA70
void CGPlayer_C::TurnOffGuildTabardPurchase() {
    if (this->m_characterComponent) {
        this->m_characterComponent->TurnOffGuildTabardPurchase();
    }

    this->LoadGuildTextures(this->m_obj->m_dataType == 1 ? this->m_obj->m_data.m_guid : 0);
    CGGameUI::UnitModelUpdate(this->m_obj->m_guid, 3);
}

// OFFSET: 0x0055D270/0x1401BA540
void CGPlayer_C::ClearEmote() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_EMOTE));
    msg.Put(static_cast<uint32_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0055D400
bool CGPlayer_C::CanChangeTalentsInRaid() const {
    return !CGRaidInfo::IsPVPRaid() || (this->m_unit->m_unitFieldAurastate & 0x80000);
}

// OFFSET: 0x00563A20/0x1401C1810
void CGPlayer_C::PlayDanceMove(const DanceMovesRec* move) {
    switch (move->m_type) {
        case 1: {
            const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(move->m_param);

            if (kit) {
                PlaySpellVisuaKitData data(NULL, kit, 0);
                this->PlaySpellVisualKit(data);
            }

            break;
        }

        case 2: {
            C3Vector position = this->GetPosition();
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player && (CGPartyInfo::IsMember(this->m_obj->m_guid) || player->m_unit->m_unitFieldTarget == this->m_obj->m_guid || ClntObjMgrGetActivePlayer() == this->m_obj->m_guid)) {
                SE3SoundKitProperties properties;
                SE3::PlaySoundKit(move->m_param, &position, &properties, NULL, __FILE__, __LINE__, "CGPlayer_C::PlayDanceMove", 0, NULL);
            }

            this->DanceMoveComplete();
            break;
        }

        case 3:
        case 4:
            ClientSetTimer(1000 * move->m_param, DanceTimerCallback, this->m_obj->m_guid.m_guid, NULL);

            if (move->m_type == 4) {
                this->PlayDanceMove(g_danceMovesDB.GetRecord(this->m_dance->m_moves[this->m_danceMoveIndex - 1]));
            }

            break;
    }
}

// OFFSET: 0x005638F0/0x1401C16B0
void CGPlayer_C::StartDance(PlayerDance* dance) {
    this->m_dance = dance;
    this->m_danceMoveIndex = dance->m_start;
    this->Dance();
}

// OFFSET: 0x0055D8A0
int32_t CGPlayer_C::XRayVisionEffectProc(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->UsesNpcBakedTexture()) {
        if (unit->m_characterComponent) {
            unit->m_characterComponent->RemoveItemBySlot(INVSLOT_HEAD);
            unit->m_characterComponent->RemoveItemBySlot(INVSLOT_SHOULDER);
        }

        unit->FreeComponent();
        const SpellRec* spell = static_cast<const SpellRec*>(param);
        const SpellVisualRec* visual = GetSpellVisual(spell, unit);

        if (visual) {
            const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_targetImpactKit);

            if (kit) {
                PlaySpellVisuaKitData data(spell, kit, 6);
                unit->PlaySpellVisualKit(data);
            }
        }
    }

    return 1;
}

// OFFSET: 0x00566510
void CGPlayer_C::HideHelmForBarberShop(int32_t hide) {
    if (hide) {
        if (!(this->m_combatModeFlags & 0x10)) {
            this->m_combatModeFlags |= 0x10;

            if (!(this->m_player->m_playerFlags & 0x400) && this->CanEquipItemsInThisForm()) {
                this->m_characterComponent->RemoveItemBySlot(INVSLOT_HEAD);
            }
        }
    } else if (this->m_combatModeFlags & 0x10) {
        this->m_combatModeFlags &= ~0x10u;

        if (!(this->m_player->m_playerFlags & 0x400) && this->CanEquipItemsInThisForm()) {
            VisibleItem_C* head = &this->m_player->m_playerVisibleItems[0];

            if (head && head->GetDisplayID()) {
                this->m_characterComponent->AddItemBySlot(INVSLOT_HEAD, head->GetDisplayID(), 0);
            }
        }
    }
}

// OFFSET: 0x005661C0
void CGPlayer_C::EquipVisibleItem(const VisibleItem_C* item, int32_t slot) {
    VisibleItem_C* visible = const_cast<VisibleItem_C*>(item);

    if (!item || !abs(item->m_itemId) || static_cast<int32_t>(visible->GetDisplayID()) <= 0 || ((this->m_combatModeFlags & 0x10) && CGBarberShop::IsSlotHidden(slot))) {
        return;
    }

    switch (slot) {
        case INVSLOT_MAINHAND:
            this->AddHandItem(0);
            this->SetWeaponTrail(this->m_worldModel, 0);
            return;

        case INVSLOT_OFFHAND:
            this->AddHandItem(1);
            this->SetWeaponTrail(this->m_worldModel, 1);
            return;

        case INVSLOT_RANGED:
            this->AddHandItem(2);
            this->SetWeaponTrail(this->m_worldModel, 2);
            return;
    }

    if (!this->m_characterComponent || !this->CanEquipItemsInThisForm() || (CGPlayer_C::IsXRayVisionActive() && this->UsesNpcBakedTexture())) {
        return;
    }

    if (slot != INVSLOT_TABARD && slot != INVSLOT_BODY) {
        if (slot == INVSLOT_HEAD) {
            if (this->m_player->m_playerFlags & 0x400) {
                return;
            }
        } else if (slot == INVSLOT_BACK) {
            if (this->m_player->m_playerFlags & 0x800) {
                return;
            }
        }

        this->m_characterComponent->AddItemBySlot(static_cast<INVENTORY_SLOTS>(slot), visible->GetDisplayID(), 0);

        if (slot != INVSLOT_BACK) {
            return;
        }
    }

    this->LoadGuildTextures(this->GetGuildGUID().m_guid);
}

// OFFSET: 0x00566320/0x1401C4A00
int32_t CGPlayer_C::RefreshVisibleItems() {
    if (!this->CanEquipItemsInThisForm()) {
        return 0;
    }

    if (!this->GetBag()) {
        return 1;
    }

    for (uint32_t slot = 0; slot <= INVSLOT_TABARD; slot++) {
        VisibleItem_C* item = &this->m_player->m_playerVisibleItems[slot];
        int32_t itemId = item->m_itemId;

        if (abs(itemId) && (itemId <= 0 || !this->m_sheatheState)) {
            if (slot == INVSLOT_MAINHAND) {
                if (this->m_sheatheState <= 1) {
                    this->RemoveHandItemModel(INVSLOT_MAINHAND, 0, 0);
                }
            } else if (slot == INVSLOT_OFFHAND) {
                if (this->m_sheatheState <= 1) {
                    this->RemoveHandItemModel(INVSLOT_OFFHAND, 0, 0);
                    this->RemoveHandItemModel(INVSLOT_OFFHAND, 0, 1);
                }
            } else if (slot == INVSLOT_RANGED) {
                uint32_t state = this->m_sheatheState;

                if (state == 2 || !state) {
                    this->RemoveHandItemModel(INVSLOT_RANGED, 0, 0);
                }

                if (this->m_sheatheState == 2) {
                    this->RemoveHandItemModel(INVSLOT_OFFHAND, 0, 0);
                    this->RemoveHandItemModel(INVSLOT_MAINHAND, 0, 0);
                }
            } else if (itemId <= 0 && this->m_characterComponent) {
                this->m_characterComponent->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(slot));
            }
        }

        this->EquipVisibleItem(item, slot);

        if (!item->m_itemId && slot == INVSLOT_MAINHAND) {
            this->RemoveHandItemModel(INVSLOT_MAINHAND, 0, 0);
            this->RemoveHandItemModel(INVSLOT_MAINHAND, 1, 0);
            this->RemoveHandItemModel(INVSLOT_MAINHAND, 2, 0);
            this->RemoveHandItemModel(INVSLOT_MAINHAND, 3, 0);
        }
    }

    return 1;
}

// OFFSET: 0x00567910
void CGPlayer_C::UnequipVisibleItem(uint32_t slot, const VisibleItem_C* oldItem) {
    bool rangedRight = false;
    const VisibleItem_C* item = slot > INVSLOT_TABARD ? NULL : &this->m_player->m_playerVisibleItems[slot];
    VisibleItem_C* old = const_cast<VisibleItem_C*>(oldItem);
    const CGUnitVirtualItem* ranged = this->GetVirtualItem(2, false);

    if (ranged && (ranged->m_inventoryType == 26 || ranged->m_inventoryType == 25)) {
        rangedRight = true;
    }

    if (slot == INVSLOT_MAINHAND) {
        this->ClearWeaponTrail(0);

        if (rangedRight && this->m_sheatheState == 2) {
            CM2Model* child = this->m_worldModel->m_attachList;

            for (; child; child = child->m_attachNext) {
                if (child->m_attachmentId == 1) {
                    break;
                }
            }

            if (child) {
                child->m_refCount++;
                child->DetachFromParent();
            }

            this->RemoveHandItemModel(INVSLOT_MAINHAND, old->GetSheatheType(), 0);

            if (child) {
                child->AttachToParent(this->m_worldModel, 1, NULL, 0);
                child->Release();
            }
        } else {
            this->RemoveHandItemModel(INVSLOT_MAINHAND, old->GetSheatheType(), 0);
        }

        this->UpdateCombatReadyAnimKit(0);
    } else if (slot == INVSLOT_OFFHAND) {
        this->ClearWeaponTrail(1);

        if (rangedRight || this->m_sheatheState != 2) {
            bool shield = old->GetInventoryType() == 14;
            this->RemoveHandItemModel(INVSLOT_OFFHAND, old->GetSheatheType(), shield);
        } else {
            CM2Model* child = this->m_worldModel->m_attachList;

            for (; child; child = child->m_attachNext) {
                if (child->m_attachmentId == 2) {
                    break;
                }
            }

            if (child) {
                child->m_refCount++;
                child->DetachFromParent();
            }

            bool shield = old->GetInventoryType() == 14;
            this->RemoveHandItemModel(INVSLOT_OFFHAND, old->GetSheatheType(), shield);

            if (child) {
                child->AttachToParent(this->m_worldModel, 2, NULL, 0);
                child->Release();
            }
        }

        this->UpdateCombatReadyAnimKit(0);
    } else if (slot != INVSLOT_RANGED) {
        if (this->CanEquipItemsInThisForm() && this->m_characterComponent) {
            this->m_characterComponent->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(slot));
        }
    } else {
        if (this->m_sheatheState == 2) {
            uint32_t inventoryType = old->GetInventoryType();

            if (inventoryType == 26 || inventoryType == 25) {
                this->m_worldModel->DetachAllChildrenById(1);
            } else {
                this->m_worldModel->DetachAllChildrenById(2);
            }
        }

        if (this->m_characterComponent) {
            this->m_characterComponent->RemoveItem(ITEMSLOT_11);
        }

        this->UpdateCombatReadyAnimKit(0);
        CM2Model* model = this->m_rangedWeaponModel;

        if (!model) {
            return;
        }

        if (this->m_sheatheState != 2) {
            if (model->m_attachParent) {
                model->DetachFromParent();
            }

            this->m_rangedWeaponModel->m_drawCallback = NULL;
            this->m_rangedWeaponModel->Release();
            this->m_rangedWeaponModel = NULL;
            return;
        }
    }

    this->EquipVisibleItem(item, slot);
}

// OFFSET: 0x00568E10
void CGPlayer_C::UpdateInventoryComponent(INVENTORY_SLOTS slot, const VisibleItem_C* oldItem) {
    const VisibleItem_C* item = slot > INVSLOT_TABARD ? NULL : &this->m_player->m_playerVisibleItems[slot];
    this->ExpediteDisplayTransition(1);

    if (abs(oldItem->m_itemId) == abs(item->m_itemId) && (oldItem->m_itemId > 0) == (item->m_itemId > 0)) {
        this->UpdateObjComponentVisualsSlot(item, slot);
        return;
    }

    CGTradeSkillInfo::RefreshList(0);
    CGCharacterInfo::UpdateInventoryAlerts();

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGActionBar::UpdateUsable();
        CGActionBar::UpdateItem(abs(oldItem->m_itemId));
        CGActionBar::UpdateItem(abs(item->m_itemId));

        switch (slot) {
            case INVSLOT_MAINHAND:
                FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_ATTACK, "%s", "player");
                CGSpellBook::UpdateSpells(0, 0, 0);
                break;

            case INVSLOT_OFFHAND:
                FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_ATTACK, "%s", "player");
                break;

            case INVSLOT_RANGED:
                FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_RANGED_ATTACK_POWER, "%s", "player");
                CGSpellBook::UpdateSpells(0, 0, 0);
                break;
        }
    }

    this->UnequipVisibleItem(slot, oldItem);
    this->ApplyComponentItemsFromEffects();

    if (this->m_sheatheState == 2 && slot == INVSLOT_RANGED) {
        this->SetSheatheState(2, 0, 0);
    }

    WOWGUID guid = this->m_obj->m_guid;
    CGGameUI::UnitModelUpdate(guid, 3);
}

// OFFSET: 0x00568140/0x1401C6DF0
void CGPlayer_C::MeleeModeEnter(const WOWGUID& target, int32_t force, int32_t spellId) {
    if (!CGGameUI::CanPerformAction(1) || this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    if ((this->m_unit->m_unitFieldFlags & 0x20000) || (this->m_unit->m_unitFieldFlags2 & 0x800000) || !CGGameUI::m_hasControl || (this->m_player->m_playerFlags & 0x1000000)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (!this->CanAttackNow(unit)) {
        if (this->IsInMelee()) {
            this->SetCombatMode(true);
        }

        return;
    }

    uint16_t overrideSpells = *reinterpret_cast<const uint16_t*>(this->m_player->m_playerFieldBytes2);
    const OverrideSpellDataRec* overrideRec;

    if (overrideSpells && (overrideRec = g_overrideSpellDataDB.GetRecord(overrideSpells)) != NULL && !(overrideRec->m_flags & 0xA)) {
        return;
    }

    if (this->IsVehiclePassengerSeated()) {
        CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));
        const VehicleSeatRec* seat;

        if (!vehicle || (seat = vehicle->GetVehicleSeatRec(this->m_movementData.m_transportSeat)) == NULL || !(seat->m_flags & 0x4000)) {
            return;
        }
    }

    if (spellId) {
        Spell_C_CancelOtherRangedSpells(spellId);
    } else {
        Spell_C_CancelRangedSpells();
    }

    if (this->m_unit->m_unitFieldBytes1[0]) {
        this->ChangeStandState(0);
    }

    CGGameUI::Target(unit->m_obj->m_guid);

    if (!force && Spell_C_IsCastingInterruptedByCombat(this->m_obj->m_guid) && !this->m_combat.m_attacking) {
        return;
    }

    if (this->m_stopAttackSent || (!this->m_combat.m_attacking && this->m_combat.m_attackSent == 0)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_ATTACK_SWING));
        msg.Put(unit->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        this->m_combat.SetAttackSent(unit->m_obj->m_guid);
    }

    const EmotesRec* emote = g_emotesDB.GetRecord(this->m_unit->m_unitNpcEmotestate);

    if (emote && (emote->m_emoteFlags & 0x40000)) {
        CGPlayer_C::ClearEmote();
    }

    const SpellRec* spell;

    if (!spellId || (spell = g_spellDB.GetRecord(spellId)) == NULL || !(spell->m_attributes & 2) || this->m_unit->m_unitFieldBytes2[0] != 2) {
        this->SetSheatheState(1, 1, 0);
    }

    Spell_C_SetAutoRangedCombatSpell(0);
    this->SpellInterrupted(this->m_castCount, this->m_castSpellId);
    Spell_C_CancelAutoRepeat(1);
    Spell_C_ClearLastAutoRangedCombatError();
}

// OFFSET: 0x005690D0
void CGPlayer_C::DelKnownSpell(int32_t spellId, int32_t silent) {
    if (spellId > g_spellDB.m_maxID) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", spellId);
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
    bool notify = silent == 0;

    if (spell->m_attributes & 0x20) {
        SkillLineAbilityRec* ability = SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], spellId);

        if (ability) {
            SkillLineRec* skillLine = g_skillLineDB.GetRecord(ability->m_skillLine);

            if (skillLine) {
                TRADESKILLLINE* line = this->m_tradeSkillLines.Ptr(skillLine->m_id, HASHKEY_NONE());

                if (line) {
                    uint32_t count = line->m_spells.m_count;

                    for (uint32_t i = count; i; ) {
                        i--;

                        if (line->m_spells.m_data[i] == spellId) {
                            if (i < count - 1) {
                                memcpy(&line->m_spells.m_data[i], &line->m_spells.m_data[i + 1], sizeof(int32_t) * (count - 1 - i));
                            }

                            line->m_spells.SetCount(count - 1);
                            CGTradeSkillInfo::RefreshList(1);
                            break;
                        }
                    }
                }
            }
        }
    } else {
        if (effect && effect->m_effect == 155) {
            this->RemoveEquipBypassSkill(spellId);
        }

        if (notify && !(spell->m_attributes & 0x80) && !IsGuildPerkSpell(spellId)) {
            CGGameUI::DisplayError(ERR_SPELL_UNLEARNED_S, CGTooltip::GetSpellHyperlinkString(spell->m_id));
        }
    }

    CGSpellBook::DelKnownSpell(spellId, 0);
    CGActionBar::UpdateSpell(spellId);

    if (!silent) {
        CGClassTrainer::RefreshList();
    }

    if (spellId == s_dualWieldSpell) {
        s_dualWieldSpell = 0;
    }

    if (spellId == CGGameUI::GetCursorSpell()) {
        CGGameUI::DropCursorSpell();
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGQuestLog::UpdateQuestSpell(spellId);
    }

    this->UpdateAuraEffectMask();

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->RefreshSpellCategoryCooldowns(spell);
    }
}

// OFFSET: 0x0056D230
static int32_t IsSoulstone(const CGItem_C* item, void* param) {
    WOWGUID none = WOWGUID();
    const ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        const SpellRec* spell = g_spellDB.GetRecord(rec->m_spellID[i]);

        if (!spell || rec->m_spellTrigger[i]) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effect == 94) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x0056D2F0
CGItem_C* CGPlayer_C::GetSoulstone() {
    return this->m_bag.FindItem(IsSoulstone, NULL, 0);
}
