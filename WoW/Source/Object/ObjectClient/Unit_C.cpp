#include "Unit_C.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "WoW/Source/JamClientAutoCode/JamClientMove.h"
#include "WoW/Source/JamClientAutoCode/JamPlayerMove.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/DB/Rec/SpellAuraRestrictionsRec.h"
#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellInterruptsRec.h"
#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"
#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"
#include "WoW/Source/JamClientAutoCode/JamPlayerMove.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/Object/ObjectClient/UnitDisplayTransition_C.h"
#include <cmath>
#include <new>
#include <typeinfo>
#include <cstring>
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AnimationDataRec.h"
#include "WoW/Source/DB/Rec/AreaGroupRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/CreatureMovementInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"
#include "WoW/Source/DB/Rec/EmotesRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitMissileTrajectory_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/NamePlateFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "ThirdParty/zlib/zlib.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/EnvironmentalDamageRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"
#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"
#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/TerrainTypeRec.h"
#include "WoW/Source/DB/Rec/UnitBloodRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/Game/GameClient/tables.h"
#include "WoW/Source/Game/GameClient/WTOBJECT.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Magic/Spell.h"
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/UnitSound_C.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/ObjectAlloc/ObjectAlloc.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/DuelInfo.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TaxiMapFrame.h"
#include "WoW/Source/Ui/TotemInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/SmartScreenRect.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CA64E4
static INVENTORY_SLOTS s_virtualItemInvSlots[2] = { INVSLOT_MAINHAND, INVSLOT_OFFHAND };

// STATIC: #0x00CA1398
static char s_objectNameBuffers[2][305];

// STATIC: #0x00CA15FC
static uint32_t s_objectNameIndex;

// STATIC: #0x00ADAA10
static int32_t s_baseSpiritRegenCap = 50;

// STATIC: #0x00ADAA14
static const char* s_genderSuffix[2] = { "M", "F" };

// STATIC: 0x00DD6188
static WOWGUID s_pendingInteractTarget;

// STATIC: 0x00DD6190
static WOWGUID s_pendingSkinTarget;

// STATIC: 0x00E283B8
static uint32_t s_npcFeedbackSuppressCount;

// STATIC: 0x00DD5E9C
int32_t CGUnit_C::s_powerTypeBySlot[12][5];

// STATIC: 0x00DD5F64
int32_t CGUnit_C::s_powerSlotByType[12][11];

// STATIC: 0x00DD6198
static WOWGUID s_pendingUseObject;

// STATIC: 0x00DD61A0
static WOWGUID s_pendingLootTarget;

// STATIC: 0x00DD61A8
static WOWGUID s_pendingAttackTarget;

// STATIC: 0x00DD61E4
int32_t g_environmentalDamageVisualKits[6];

// STATIC: #0x00CA1224
static EmotesRec* s_talkEmotes[5];

// STATIC: 0x00DD6220
static TSObjectAllocPool<UnitThreat>* s_unitThreatPool;

// STATIC: 0x00DD6230
int32_t CGUnit_C::s_initialized;

// STATIC: 0x00DD6210
WOWGUID CGUnit_C::s_activeMover;

// STATIC: 0x00DD6218
static uint32_t s_swimStartTime;

// STATIC: 0x00DD621C
static uint32_t s_swimStopTime;

// STATIC: 0x00CA627C
static uint32_t s_swimTransitionDelay = 2000;

// STATIC: 0x00CA6280
static uint32_t s_flightTransitionDelay = 2000;

// STATIC: 0x00CA6284
static uint32_t s_swimTransitionDelayFlag = 100;

// STATIC: 0x00CA6288
static uint32_t s_flightTransitionDelayFlag = 100;

// STATIC: 0x00DD61D4
uint32_t CGUnit_C::s_trackingType;

// STATIC: 0x00DD6318
static int32_t s_attackFlashToggle;

// STATIC: 0x00DD631C
static uint32_t s_attackFlashTime;

// STATIC: #0x00A2D394
extern const uint32_t s_environmentalDamageSchools[6] = { 0, 0, 0, 2, 3, 2 };

// STATIC: #0x00A349B4
static const int32_t s_animTierFallback[5] = { 0, 0, 3, 0, 0 };

// STATIC: #0x00ADAC0C
static float s_trackCursorMinSize = 0.5f;

// STATIC: #0x00ADAC10
static float s_trackCursorMaxSize = 0.75f;

// STATIC: #0x00ADAC14
static uint32_t s_trackCursorPeriod = 500;

// STATIC: 0x00CA64CC
static uint8_t s_rippleDirectional[4] = { 0, 0, 1, 0 };

// STATIC: 0x00CA630C
static uint8_t s_attackTargetColor[4] = { 0x00, 0x00, 0xFF, 0xFF };

// STATIC: #0x00CA11D8
float CGUnit_C::s_trackingFacing;

// STATIC: #0x00CA1208
uint32_t CGUnit_C::s_namePlateFlags;

// STATIC: #0x00CA0AB0
uint32_t CGUnit_C::s_missileTrajectoryTargetingSpell;

// STATIC: #0x00CA0AB4
uint32_t CGUnit_C::s_pitchMissileTrajectoryTargetingNextFrame;

// STATIC: #0x00CA1264
C3Vector CGUnit_C::s_trackingPosition;

// STATIC: #0x00CA11EC
float CGUnit_C::s_trackingDirection;

// STATIC: #0x00CA12CC
int32_t CGUnit_C::s_dismountSoundKitId;

// STATIC: 0x00DD6178
WOWGUID CGUnit_C::s_lastNPCSoundGUID;

// STATIC: 0x00DD6228
WOWGUID CGUnit_C::s_deferredClientControlUpdateGUID;

// STATIC: 0x00DD6224
uint8_t CGUnit_C::s_deferredClientControlUpdateState;

// STATIC: 0x00DD6218
uint32_t CGUnit_C::s_flightStatusTime;

// STATIC: 0x00DD621C
uint32_t CGUnit_C::s_flightEndTime;

// STATIC: #0x00CA11D0
float CGUnit_C::s_unk_CA11D0;

// STATIC: 0x00DD61B4
uint32_t CGUnit_C::s_unk_CA11D4;

// STATIC: #0x00CA11F8
WOWGUID CGUnit_C::s_trackingGUID;

// STATIC: #0x00CA1200
uint32_t CGUnit_C::s_trackingFlags;

// STATIC: #0x00CA1204
CGNamePlateFrame* CGUnit_C::s_namePlateFocus;

// STATIC: #0x00ADAA80
static TSList<CGNamePlateFrame> s_namePlates;

// STATIC: #0x00ADAA8C
static TSList<CGNamePlateFrame> s_unusedNamePlates;

// STATIC: #0x00ADAAB8
static ClickToMoveActionInfo s_clickToMoveActions[12] = {
    { 0, { 0, 0, 0 }, 0.0f, 0, 0, { 0, 0 } },
    { 0, { 0, 0, 0 }, 0.0f, 0, 0, { 0, 0 } },
    { 0, { 0, 0, 0 }, 0.0f, 0, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 900.0f, 0, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 0.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 0.0f, 1, 1, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 6400.0f, 1, 0, { 0, 0 } },
    { 1, { 0, 0, 0 }, 0.0f, 1, 0, { 0, 0 } },
};

// STATIC: 0x00CA6290
static const uint32_t s_effectAttachmentIds[16] = { 20, 34, 19, 21, 22, 17, 23, 24, 25, 15, 16, 37, 38, 22, 47, 48 };

// STATIC: #0x00CA1190
static int32_t s_lastNPCSoundCount;

// STATIC: #0x00CA11A4
static float s_forcedRunSpeed;

// STATIC: #0x00CA11DC
static float s_trackingTurnRate;

// STATIC: #0x00CA11E0
static float s_trackingInteractDistanceSq;

// STATIC: #0x00CA11E4
static float s_trackingInteractDistance;

// STATIC: #0x00CA11E8
static float s_trackingObjectRange;

// STATIC: #0x00CA11F0
static uint32_t s_trackingLastTime;

// STATIC: #0x00CA1258
static C3Vector s_trackingStartPosition;

// STATIC: #0x00ADAA74
static float s_trackingStuckDistanceSq = 7.71604937e-06f;

// STATIC: #0x00ADAA9C
static float s_trackingFollowDistance = 2.77777767f;

// STATIC: #0x00ADAAA0
static float s_trackingFollowDistanceSq = 9.0f;

// STATIC: #0x00ADAAAC
static float s_trackingTerrainDistanceSq = 0.25f;

// STATIC: #0x00CA1270
static float s_trackingDefaultInteractDistanceSq = s_trackingFollowDistance * s_trackingFollowDistance;

// STATIC: #0x00ADAC18
static uint32_t s_spellInterruptTrackingMask = 1;

// STATIC: #0x00ADBFDC
static const char* s_combatTextTokens[12] = { "COMBAT_TEXT_NONE", "COMBAT_TEXT_MISS", "COMBAT_TEXT_RESIST", "COMBAT_TEXT_DODGE", "COMBAT_TEXT_PARRY", "COMBAT_TEXT_BLOCK", "COMBAT_TEXT_EVADE", "COMBAT_TEXT_IMMUNE", "COMBAT_TEXT_IMMUNE", "COMBAT_TEXT_DEFLECT", "COMBAT_TEXT_ABSORB", "COMBAT_TEXT_REFLECT" };

// STATIC: #0x00A34C20
static const uint32_t s_combatTextTypes[12] = { 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3 };

// STATIC: #0x00ADAA6C
static uint32_t s_petSpellDamageColor = 0xFFFFDE00;

// STATIC: #0x00ADAA70
static uint32_t s_petMeleeDamageColor = 0xFFFF8400;

// STATIC: 0x00CA64B8
static float s_headLookEpsilon = 0.001f;

// STATIC: 0x00CA64BC
static float s_headLookSpineTime = 2.0f;

// STATIC: 0x00CA64C0
static float s_headLookHeadTime = 1.0f;

// STATIC: 0x00CA64C4
static float s_headLookHeadRate = 0.75f;

// STATIC: 0x00CA64C8
static float s_headLookSpineRate = 0.2f;

// STATIC: 0x00CA64DC
static float s_twistEpsilon = 0.00001f;

// STATIC: 0x00DD6258
static CDataAllocator s_activeHeadLookAllocator(sizeof(ActiveHeadLook), 8);

// STATIC: 0x00CA6308
static float s_namePlateMaxDistanceSquared = 1681.0f;

// OFFSET: 0x00599DD0
bool CGUnit_C::IsPowerRegenInterrupted() const {
    return (this->m_unit->m_unitFieldFlags & 0x80000) || (this->m_auraEffectMask[11] & 0x40);
}

// PROVEN: ebcf5943 IDENTICAL/NONE
// OFFSET: 0x005AA8A0
void CGUnit_C::RetargetAuraEffects(int32_t spellId, const WOWGUID& oldCaster, const WOWGUID& newCaster) {
    CGObject_C::RetargetEffects(this->m_effectList, spellId, oldCaster.m_guid, newCaster.m_guid);
    CGObject_C::RetargetEffects(this->m_procEffects, spellId, oldCaster.m_guid, newCaster.m_guid);
    CGObject_C::RetargetEffects(this->m_auraEffects, spellId, oldCaster.m_guid, newCaster.m_guid);
}

// PROVEN: a249a7a3 IDENTICAL/NONE
// OFFSET: 0x005AB780/0x14021DC00
void CGUnit_C::MarkFallingSent() {
    this->m_clientFlags |= 0x80;
}

// PROVEN: 8aeb0bbc IDENTICAL/NONE
// OFFSET: 0x005AC0C0/0x14021E5B0
void CGUnit_C::ResetFloodsafeMoveEvents() {
    this->unk_09FC &= ~0x80;
    this->m_floodsafeOpcode[0] = 8229;
    this->m_floodsafeOpcode[1] = 8229;
}

// PROVEN: 2ba49bfa IDENTICAL/NONE
// OFFSET: 0x005AC510
uint32_t CGUnit_C::NextVisualKitSequence() {
    uint32_t sequence = this->unk_1020;
    this->unk_1020 = sequence + 2;
    return sequence;
}

// PROVEN: 5b21c1ef IDENTICAL/NONE
// OFFSET: 0x005AC630/0x14021EB10
void CGUnit_C::FlipDisplayTransitionState() {
    uint32_t flags = this->m_currentClientRaceFlags;

    if (flags & 0x2) {
        this->m_currentClientRaceFlags = flags ^ 0x1;
    }
}

// OFFSET: 0x005AC920
ATTACKROUNDINFO::ATTACKROUNDINFO() {
    this->m_attacker = WOWGUID();
    this->m_target = WOWGUID();
    this->m_hitInfo = 0;
    this->m_victimState = 0;
    this->unk_005C = 0;
    this->m_meleeSpellId = 0;
    this->m_blocked = 0;
    this->m_rageGained = 0;
    this->unk_006C = 0;

    for (int32_t i = 0; i < 8; i++) {
        this->unk_0070[i] = 0.0f;
    }

    for (int32_t i = 0; i < 5; i++) {
        this->unk_0090[i] = 0;
    }

    this->m_damage = 0;
    this->m_overkill = -1;
    this->unk_001C = 0;

    for (int32_t i = 0; i < 2; i++) {
        this->m_schoolMask[i] = 0;
        this->unk_0028[i] = 0.0f;
        this->unk_0030[i] = 0.0f;
        this->m_subDamage[i] = 0.0f;
        this->m_subDamageInt[i] = 0;
        this->m_absorbed[i] = 0;
        this->m_resisted[i] = 0;
    }
}

// OFFSET: 0x005AC9E0
static CTexture* CreateTexture(const char* fileName, CStatus* status) {
    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipNearest;
    return TextureCreate(fileName, flags, status, 0, NULL, 2);
}

// PROVEN: a9f729ae IDENTICAL/NONE
// OFFSET: 0x005ACED0/0x14021F2B0
void CGUnit_C::InitPowerTypeSlots() {
    for (int32_t i = 1; i < 12; i++) {
        for (int32_t j = 0; j < 5; j++) {
            CGUnit_C::s_powerTypeBySlot[i][j] = 11;
        }
    }

    for (int32_t i = 1; i < 12; i++) {
        for (int32_t j = 0; j < 11; j++) {
            CGUnit_C::s_powerSlotByType[i][j] = -1;
        }
    }

    int32_t classID = 0;
    int32_t slot = 0;

    for (int32_t i = 0; i < g_chrClassesXPowerTypesDB.m_numRecords; i++) {
        ChrClassesXPowerTypesRec* rec = g_chrClassesXPowerTypesDB.GetRecordByIndex(i);

        if (rec->m_classID != classID) {
            classID = rec->m_classID;
            slot = 0;
        }

        CGUnit_C::s_powerSlotByType[classID][rec->m_power] = slot;
        CGUnit_C::s_powerTypeBySlot[classID][slot] = rec->m_power;
        slot++;
    }
}

// OFFSET: 0x005AD2C0/0x14021F790
int32_t AuraStateUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (guid == ClntObjMgrGetActivePlayer() || guid == CGGameUI::m_lockedTarget) {
        CGSpellBook::UpdateUsable();
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE_USABLE, NULL);
        CGActionBar::UpdateUsable();
    }

    if (CGUnit_C::s_activeMover != ClntObjMgrGetActivePlayer() && (guid == CGUnit_C::s_activeMover || guid == CGGameUI::m_lockedTarget)) {
        CGActionBar::UpdateUsablePower();
    }

    return 1;
}

// OFFSET: 0x005AD360
int32_t DisplayIDUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() && CGBarberShop::m_barberShopEnabled) {
            CGBarberShop::DisableBarberShop();
        }

        unit->UpdateDisplayInfo(0);
    }

    return 1;
}

// OFFSET: 0x005ADB90
int32_t CGUnit_C::AddHandItemModel(const ItemDisplayInfoRec* displayRec, int32_t invSlot, int32_t sheatheType, bool sheathed, bool shield, bool mainHand, int32_t itemVisual) {
    if (this->m_characterComponent) {
        return this->m_characterComponent->AddHandItem(displayRec, static_cast<INVENTORY_SLOTS>(invSlot), static_cast<SHEATHE_TYPE>(sheatheType), sheathed, shield, mainHand, itemVisual);
    }

    uint8_t attachMask = 0;

    return CCharacterComponent::AddHandItem(this->m_worldModel, &attachMask, displayRec, static_cast<INVENTORY_SLOTS>(invSlot), static_cast<SHEATHE_TYPE>(sheatheType), sheathed, shield, mainHand, itemVisual);
}

// OFFSET: 0x005ADC00
void CGUnit_C::RemoveHandItemModel(int32_t invSlot, int32_t sheatheType, bool shield) {
    if (this->m_characterComponent) {
        this->m_characterComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(invSlot), static_cast<SHEATHE_TYPE>(sheatheType), shield);
        return;
    }

    uint8_t attachMask = 0;
    CCharacterComponent::RemoveHandItem(this->m_worldModel, &attachMask, static_cast<INVENTORY_SLOTS>(invSlot), static_cast<SHEATHE_TYPE>(sheatheType), shield);
}

// OFFSET: 0x005ADC50
void CGUnit_C::TabardDrawCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (!key || !success) {
        return;
    }

    if (!g_guildInfoCache.GetRecord(key, WOWGUID(), NULL, NULL, false)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    void (*noCallback)(const uint64_t& key, const WOWGUID& guid, void* param, bool success) = NULL;
    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (GuildGetGuildTabard(guid, WOWGUID::Make(key), noCallback, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background) && unit->m_characterComponent) {
        unit->m_characterComponent->ApplyGuildColor(reinterpret_cast<uintptr_t>(param), emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x005ADD20
void CGUnit_C::ShirtTabardDrawCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (!key || !success) {
        return;
    }

    if (!g_guildInfoCache.GetRecord(key, WOWGUID(), NULL, NULL, false)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    void (*noCallback)(const uint64_t& key, const WOWGUID& guid, void* param, bool success) = NULL;
    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;

    if (GuildGetGuildTabard(guid, WOWGUID::Make(key), noCallback, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background) && unit->m_characterComponent) {
        unit->m_characterComponent->ApplyShirtGuildColor(reinterpret_cast<uintptr_t>(param), emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x005AE0D0/0x1402208D0
uint8_t CGUnit_C::KitFlagsToPlayFlags(const SpellRec* spell, const SpellVisualRec* visual, const SpellVisualKitRec* kit) {
    uint8_t playFlags = 0;

    if (kit->m_flags & 0x4) {
        playFlags = 2;
    }

    if (kit->m_flags & 0x8) {
        playFlags |= 0x4;
    }

    if (kit->m_flags & 0x10) {
        playFlags |= 0x8;
    }

    if (kit->m_flags & 0x100) {
        playFlags |= 0x20;
    }

    if (spell && (spell->m_attributes & 0x40000)) {
        playFlags |= 0x20;
    }

    if (visual) {
        SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(visual->m_missileModel);

        if (effectName && (static_cast<uint8_t>(effectName->m_type) == 1 || static_cast<uint8_t>(effectName->m_type) == 2)) {
            return playFlags | 0x20;
        }
    }

    return playFlags;
}

// OFFSET: 0x005AE3A0
void CGUnit_C::UpdateRangedWeaponRotation(uint32_t anim) {
    if (!this->m_rangedWeaponModel) {
        return;
    }

    float rotation = 0.0f;
    ItemDisplayInfoRec displayRec;

    if (AnimationDataHasFlags(anim, 0x4000000) && this->GetVirtualItem(2, false) && this->GetVirtualItemDisplayRec(2, displayRec) && (displayRec.m_flags & 0x20)) {
        rotation = 3.1415927f;
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    this->m_rangedWeaponModel->SetWorldTransform(position, rotation, 1.0f);
}

// OFFSET: 0x005AE680
void CGUnit_C::ReleaseProcEffectsBySpell(int32_t spellId, WOWGUID caster) {
    CEffect* effect = this->m_procEffects;

    while (effect) {
        CEffect* next = effect->m_next;

        if (effect->m_spellId == spellId && (!caster || !effect->m_targetGuid || caster == effect->m_targetGuid)) {
            effect->Release();
        }

        effect = next;
    }
}

// PROVEN: 53c7ac09 IDENTICAL/NONE
// OFFSET: 0x005AE7E0
void CGUnit_C::PlayTaxiDismountKit() {
    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(19519);

    if (kit) {
        PlaySpellVisuaKitData data(NULL, kit, 1);
        data.m_someGUID = this->m_obj->m_guid;
        this->PlaySpellVisualKit(data);
    }
}

// PROVEN: b9afc657 IDENTICAL/NONE
// OFFSET: 0x005AEB30/0x1402214B0
void CGUnit_C::UpdateLastSentMovement() {
    this->m_lastSentFacing = this->GetRawFacing();

    if ((this->m_movement->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (this->m_movement->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        this->m_lastSentPitch = this->GetPitch();
    }

    if (this->m_movement->m_flags & (MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT)) {
        this->unk_09FC |= 0x100;
    }

    if (this->m_movement->m_flags & (MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN)) {
        this->unk_09FC |= 0x200;
    }
}

// OFFSET: 0x005B00E0/0x140222F90
CEffectVirtualItem* CGUnit_C::FindVirtualItemEffect(int32_t hand) const {
    if (!(this->unk_09FC & 0x400)) {
        return NULL;
    }

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if ((effect->m_flags & 0x80000) && effect->m_data.m_virtualItem && effect->m_data.m_virtualItem->m_hand == hand) {
            return effect->m_data.m_virtualItem;
        }
    }

    return NULL;
}

// OFFSET: 0x005B1F40/0x140234400
void CGUnit_C::UpdateAmmoModel(int32_t effectId) {
    if (this->m_ammoDisplayId == static_cast<uint32_t>(effectId) && this->m_ammoModel) {
        return;
    }

    if (this->m_ammoModel) {
        if (this->m_ammoModel->m_attachParent) {
            this->m_ammoModel->DetachFromParent();
        }

        this->m_ammoModel->Release();
        this->m_ammoModel = NULL;
    }

    this->m_ammoDisplayId = effectId;

    if (effectId) {
        this->m_ammoModel = CEffect::CreateModelFromEffect(effectId, NULL, NULL, this->m_obj->m_guid, 0, 0);
    }
}

// PROVEN: bf15f928 IDENTICAL/NONE
// OFFSET: 0x005B25E0
bool IsGuildPerkSpell(int32_t spellId) {
    for (int32_t i = 0; i < g_guildPerkSpellsDB.m_numRecords; i++) {
        GuildPerkSpellsRec* rec = g_guildPerkSpellsDB.GetRecordByIndex(i);

        if (rec && rec->m_spellID == spellId) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005B2630/0x140225AD0
bool CGUnit_C::CanLearnSpell(int32_t spellId, int32_t& level, bool a4) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return false;
    }

    SkillLineAbilityRec* ability = this->LookupAbility(spellId);

    if (!ability || IsGuildPerkSpell(spellId)) {
        return false;
    }

    int32_t spellLevel = 0;

    if (spell->m_spellLevelsID) {
        SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

        if (levels) {
            spellLevel = levels->m_spellLevel;
        }
    }

    const SkillRaceClassInfoRec* info = SpellTableLookupInfo(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], ability->m_skillLine);

    if (!info) {
        level = spellLevel;
        return true;
    }

    if (((this->m_obj->m_type & TYPEMASK_PLAYER) && (info->m_flags & 0x800)) || (a4 && info->m_availability == 2)) {
        return false;
    }

    int32_t required = info->m_minLevel;

    if (spellLevel > required) {
        required = spellLevel;
    }

    level = required;
    return true;
}

// PROVEN: 9d1fe78f IDENTICAL/NONE
// OFFSET: 0x005B29F0
void CGUnit_C::StartFlash(const CImVector& color, uint32_t delay, uint32_t duration) {
    this->m_flashStartTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    this->m_flashColor = color;
    this->m_flashDelay = delay;
    this->m_flashDuration = duration;
}

// PROVEN: c6af9c16 IDENTICAL/NONE
// OFFSET: 0x005B2A30
void CGUnit_C::StartProceduralFade(uint8_t alpha, uint32_t type, uint32_t time, uint32_t hold, uint32_t endAlpha) {
    if (type < 13) {
        this->m_proceduralFadeAlpha = alpha;
        this->m_proceduralFadeType = type;
        this->m_proceduralFadeStart = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
        this->m_proceduralFadeDuration = time + hold;
        this->m_proceduralFadeEndAlpha = endAlpha;
        this->DoFade(static_cast<float>(this->m_proceduralFadeAlpha), time);
    }
}

// OFFSET: 0x005B2F80
void CGUnit_C::ReleaseAurasBySpell(int32_t spellId, WOWGUID caster) {
    CEffect* effect = this->m_auraEffects;

    while (effect) {
        CEffect* next = effect->m_next;

        if (effect->m_spellId == spellId && (!caster || !effect->m_targetGuid || caster == effect->m_targetGuid)) {
            effect->Release();
        }

        effect = next;
    }
}

// OFFSET: 0x005B3160/0x140226530
void CGUnit_C::DoMountFade(float alpha, uint32_t time) {
    uint8_t target = static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f));

    if (target == this->m_mountAlpha) {
        this->m_mountFadeEndAlpha = target;
        this->m_mountFadeDuration = 0;
        this->m_mountAlpha = target;
        return;
    }

    this->m_mountFadeStartTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    this->m_mountFadeStartAlpha = this->m_mountAlpha;
    this->m_mountFadeDuration = time;
    this->m_mountFadeEndAlpha = target;

    if (!time) {
        this->m_mountAlpha = target;
    }
}

// PROVEN: 9bd157e0 IDENTICAL/NONE
// OFFSET: 0x005B3680
int32_t CGUnit_C::IsPitchUnchangedSinceSent() {
    return (!(this->m_movement->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) && !(this->m_movement->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) || fabs(this->GetPitch() - this->m_lastSentPitch) < 0.1f;
}

// OFFSET: 0x005B4850/0x140227DF0
void CGUnit_C::ConnectToLightningThisFrame(uint32_t a1, int32_t a2) {
    CGObject_C::ConnectToLightningThisFrame(a1, a2);
    const SpellRec* spell = NULL;

    for (uint32_t i = 0; i < this->m_delayedSpellVisualKits.m_count; i++) {
        DelayedSpellVisualKit* delayed = &this->m_delayedSpellVisualKits.m_data[i];

        if (!(delayed->m_flags & 0x8) || delayed->m_delayType || delayed->m_spellId != static_cast<int32_t>(a1) || delayed->m_delay != a2) {
            continue;
        }

        if (spell || (spell = g_spellDB.GetRecord(a1)) != NULL) {
            SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(delayed->m_kitId);

            if (kit) {
                PlaySpellVisuaKitData data(spell, kit, delayed->m_kitType);

                if (delayed->m_flags & 0x1) {
                    data.m_position = &delayed->m_position;
                    data.m_transportGUID = delayed->m_targetGuid;
                }

                data.unk_0024 = (delayed->m_flags >> 2) & 0x1;
                data.m_animParam = (delayed->m_flags >> 1) & 0x1;
                data.unk_0028 = 1;
                data.unk_002C = delayed->m_delay;
                this->PlaySpellVisualKit(data);
            }
        }

        this->m_delayedSpellVisualKits.m_data[i].m_flags &= ~0x8;
    }
}

// OFFSET: 0x005B5110
void CGUnit_C::UpdateCamouflage() {
    bool found = false;
    float fadeIn = 15.0f;
    float fadeOut = 15.0f;

    for (CEffect* effect = this->m_effectList; effect; ) {
        CEffect* next = effect->m_next;

        if (effect->GetCamouflage(fadeIn, fadeOut)) {
            found = true;

            if (fadeIn >= 15.0f) {
                fadeIn = 15.0f;
            }

            if (fadeOut >= 15.0f) {
                fadeOut = 15.0f;
            }
        }

        effect = next;
    }

    float tick = FrameTime::GetTickTimeSec();
    float camouflage;

    if (found) {
        this->m_camouflageFadeIn = fadeIn;
        this->m_camouflageFadeOut = fadeOut;

        if (fadeIn <= 0.00000011920929f) {
            camouflage = 1.0f;
        } else {
            camouflage = tick / fadeIn + this->m_camouflage;

            if (camouflage > 1.0f) {
                camouflage = 1.0f;
            }
        }
    } else if (this->m_camouflageFadeOut <= 0.00000011920929f) {
        camouflage = 0.0f;
    } else {
        camouflage = this->m_camouflage - tick / this->m_camouflageFadeOut;

        if (camouflage < 0.0f) {
            camouflage = 0.0f;
        }
    }

    CM2Model* worldModel = this->m_worldModel;
    this->m_camouflage = camouflage;
    CM2Model* mountModel = this->m_mountModel;
    float alpha = (3.0f - camouflage * 2.0f) * camouflage * camouflage;

    if (worldModel) {
        worldModel->m_baseAlpha3 = alpha;
    }

    if (mountModel) {
        mountModel->m_baseAlpha3 = alpha;
    }
}

// OFFSET: 0x005B5870
float CGUnit_C::GetPowerRegenBase(int32_t powerType, uint8_t inCombat) const {
    static float s_regen[11][2] = {
        { 0.0f, 0.0f },
        { -12.5f, 0.0f },
        { 5.0f, 5.0f },
        { 10.0f, 10.0f },
        { 0.0f, 0.0f },
        { 0.0f, 0.0f },
        { -12.5f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 0.0f },
        { -0.1f, 0.0f },
        { 0.0f, 0.0f }
    };

    if (powerType == 10 && this->m_powerBarID) {
        UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(this->m_powerBarID);

        if (bar) {
            return inCombat ? bar->m_regenerationCombat : bar->m_regenerationPeace;
        }
    }

    return s_regen[powerType][inCombat];
}

// OFFSET: 0x005B59C0
int32_t CGUnit_C::GetPowerTypeSlot(int32_t powerType) const {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return CGUnit_C::s_powerSlotByType[this->m_unit->m_unitFieldBytes0[1]][powerType];
    }

    if (powerType == this->m_unit->m_unitFieldBytes0[3]) {
        return 0;
    }

    return powerType == 10 ? 1 : -1;
}

// OFFSET: 0x005B5A20
int32_t CGUnit_C::GetSlotPowerType(int32_t slot) const {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return CGUnit_C::s_powerTypeBySlot[this->m_unit->m_unitFieldBytes0[1]][slot];
    }

    if (slot) {
        return (slot != 1) + 10;
    }

    return this->m_unit->m_unitFieldBytes0[3];
}

// OFFSET: 0x005B5A80/0x140229330
int32_t CGUnit_C::GetPower(int32_t powerType) const {
    if (powerType == -2) {
        return this->m_unit->m_unitFieldHealth;
    }

    int32_t slot = this->GetPowerTypeSlot(powerType);
    return slot == -1 ? 0 : this->m_unit->m_unitFieldPower[slot];
}

// OFFSET: 0x005B5AC0/0x1402293B0
int32_t CGUnit_C::GetMaxPower(int32_t powerType) const {
    if (powerType == -2) {
        return this->m_unit->m_unitFieldMaxhealth;
    }

    int32_t slot = this->GetPowerTypeSlot(powerType);
    return slot == -1 ? 0 : this->m_unit->m_unitFieldMaxpower[slot];
}

// OFFSET: 0x005B5B00
void CGUnit_C::SubtractChanneledHealPrediction(int32_t spellId, const WOWGUID& caster, int32_t amount) {
    uint32_t count = this->m_healPredictions.Count();

    for (uint32_t index = 0; index < count; index++) {
        HealPredictionInfo& info = this->m_healPredictions[index];

        if (info.m_caster == caster && (info.m_spellID == spellId || info.unk_000C == spellId) && info.m_channeled == 1) {
            info.m_amount -= amount;

            if (info.m_amount < 0) {
                info.m_amount = 0;
            }

            WOWGUID guid = this->m_obj->m_guid;
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEAL_PREDICTION);
            return;
        }
    }
}

// PROVEN: ac89fbbf IDENTICAL/NONE
// OFFSET: 0x005B68F0
int32_t NPCFlagsHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->NPCFlagChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x005B74E0
void CGUnit_C::UpdateRangedWeaponOrientation() {
    if (!this->m_rangedWeaponModel) {
        return;
    }

    uint32_t boneDataID = 0xFFFFFFFF;
    uint32_t anim = this->m_animKitManager.GetExpressingHandItemAnim(boneDataID, 1, 1);
    this->UpdateRangedWeaponRotation(anim);
}

// OFFSET: 0x005BA920/0x14022ED70
void CGUnit_C::UpdateAlphaEffectAlpha() {
    bool separate = this->GetObjectModel() != this->m_worldModel;
    bool foundWorld = false;
    bool foundMount = false;
    float worldAlpha = this->GetBaseAlpha();
    uint32_t worldTime = 1000;
    float mountAlpha = this->GetBaseAlpha();
    uint32_t mountTime = 1000;

    for (CEffect* effect = this->m_auraEffects; effect; effect = effect->m_next) {
        if (!foundWorld && (!separate || (effect->m_kitRec && (effect->m_kitRec->m_flags & 0x400)))) {
            worldAlpha = effect->m_fadeAlpha * worldAlpha;
            foundWorld = true;

            if (effect->m_param) {
                worldTime = effect->m_param;
            }

            continue;
        }

        if (foundWorld && foundMount) {
            break;
        }

        if (!foundMount && separate && (!effect->m_kitRec || !(effect->m_kitRec->m_flags & 0x400))) {
            mountAlpha = effect->m_fadeAlpha * mountAlpha;
            foundMount = true;

            if (effect->m_param) {
                mountTime = effect->m_param;
            }
        }
    }

    this->DoFade(worldAlpha, worldTime);

    if (separate) {
        this->DoMountFade(mountAlpha, mountTime);
    } else {
        this->DoMountFade(1.0f, 0);
    }
}

// PROVEN: f3dd70cc IDENTICAL/NONE
// OFFSET: 0x005B3620/0x140226A60
void CGUnit_C::OnSwimStartLocal(uint32_t time) {
    this->m_movementData.OnSwimStartLocal(time);
    CGTutorial::TriggerTutorial(27);
    s_swimStartTime = time;
}

// OFFSET: 0x005B8710
void CGUnit_C::UpdateSwimmingStatusFlightStyle(uint32_t time, float depth, float swimDepth, float exitDepth) {
    if (this->m_obj->m_guid != CGUnit_C::s_activeMover) {
        return;
    }

    uint32_t delay = (this->m_unit->m_unitFieldFlags & 0x8) ? s_swimTransitionDelayFlag : s_swimTransitionDelay;

    if (this->m_movement->m_flags & MOVEMENTFLAG_SWIMMING) {
        if (exitDepth > depth || ((this->m_movementData.m_flags2 & 0x800) && time - s_swimStartTime > delay)) {
            this->m_movementData.OnSwimStopLocal(time);
        }
    } else if (time - s_swimStopTime > delay && depth > swimDepth && !this->m_movementData.IsJumpingUp() && !(this->m_movementData.m_flags2 & 0x800)) {
        if ((this->m_movementData.m_flags & (MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR)) || this->m_movementData.HasFallVelocity()) {
            this->OnSwimStartLocal(time);
        }
    }
}

// OFFSET: 0x005BEA20
void CGUnit_C::UpdateSwimmingStatusDefault(uint32_t time, int32_t spline, float depth, float swimDepth, float exitDepth) {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_obj->m_guid != CGUnit_C::s_activeMover) {
        return;
    }

    if (this->m_movement->m_flags & MOVEMENTFLAG_SWIMMING) {
        if (exitDepth > depth) {
            this->OnSwimStop(time, spline);
        }
    } else if (depth > swimDepth && !this->m_movementData.IsJumpingUp()) {
        this->OnSwimStart(time, spline);
    }
}

// OFFSET: 0x005C4380/0x14023ADA0
void CGUnit_C::PlayFlightBoundsFeedback(uint32_t time) {
    // STATIC: 0x00DD62D0
    static uint32_t s_nextFlightSoundTime = time + 2000;

    if (static_cast<int32_t>(time - s_nextFlightSoundTime) < 0) {
        return;
    }

    float bound;

    if (!World::GetFlightBoundsUpper(this->GetPosition(), bound)) {
        return;
    }

    float collisionHeight = this->m_movementData.m_collisionHeight;

    if (bound - 2.0f >= this->GetPosition().z + collisionHeight) {
        return;
    }

    if (!this->m_vehicle || !this->m_vehicle->m_vehicleRec || !this->m_vehicle->HasHighPriorityAnim()) {
        const UnitAnimationSet* animations = this->SelectAnimationSet();
        this->PlayMovementAnimKit(6, animations->m_move[0].m_backward, 0xFFFFFFFF, 0);
        this->PlayWoundAnimKit(0);
    }

    CreatureSoundDataRec* sound;

    if (this->m_mountDisplayId <= 0 || (this->m_clientFlags & 0x10000000) || this->IsMountSelf(0)) {
        sound = this->m_soundDataOverride;

        if (!sound) {
            sound = this->m_soundData;
        }
    } else {
        sound = this->m_soundDataOverride;
    }

    if (sound) {
        C3Vector position = this->GetPosition();

        if (this->IsActivePlayer()) {
            bool atCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
            SE3SoundKitProperties properties;
            properties.m_priority = 110;
            SE3::PlaySoundKit(sound->m_soundExertionID, atCharacter ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayFlightBoundsFeedback");
        } else {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit(sound->m_soundExertionID, &position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayFlightBoundsFeedback");
        }
    }

    s_nextFlightSoundTime = time + 2000;
}

// PROVEN: 35e096f0 IDENTICAL/NONE
// OFFSET: 0x005D8850
void CGUnit_C::DismountIfMountRulesFail() {
    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && !this->CheckCurrentMountRules()) {
        this->Dismount();
    }
}

// OFFSET: 0x005BAB00
void CGUnit_C::OnSwimStart(uint32_t time, int32_t spline) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        this->m_movementData.OnSwimStartLocal(time);
        CGTutorial::TriggerTutorial(27);
        s_swimStartTime = time;
    } else if (spline || (this->m_movement->m_spline && !(this->m_movement->m_spline->m_flags & 0x80))) {
        this->m_movementData.OnSplineStartSwim();
        this->UpdateMovementAnimKit(0);
    } else {
        this->m_movementData.OnSwimStart(time);
    }
}

// OFFSET: 0x005BAB90
void CGUnit_C::OnSwimStop(uint32_t time, int32_t spline) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        this->m_movementData.OnSwimStopLocal(time);
        s_swimStopTime = time;
    } else if (spline || (this->m_movement->m_spline && !(this->m_movement->m_spline->m_flags & 0x80))) {
        this->m_movementData.OnSplineStopSwim();
        this->UpdateMovementAnimKit(0);
    } else {
        this->m_movementData.OnSwimStop(time);
    }
}

// OFFSET: 0x005BBC50
bool CGUnit_C::IsControlledByMountedPlayer() const {
    const WOWGUID* controller = &this->m_unit->m_unitFieldCharmedby;

    if (!*controller) {
        controller = &this->m_unit->m_unitFieldCreatedby;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return false;
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<int32_t>(unit->m_unit->m_unitFieldMountdisplayid) > 0 && map && map->m_instanceType != 3 && map->m_instanceType != 4) {
        return true;
    }

    return false;
}

// OFFSET: 0x005BC0D0/0x140230AD0
void CGUnit_C::UpdateAlteredForm(int32_t mode) {
    if (!this->m_currentClientRace || !this->m_worldModel) {
        return;
    }

    uint32_t altered = this->HasAuraEffect(358);

    if (this->m_unit->m_unitFieldNativedisplayid != this->m_unit->m_unitFieldDisplayid) {
        if (altered) {
            this->m_currentClientRaceFlags |= 0x1;
        } else {
            this->m_currentClientRaceFlags &= ~0x1u;
        }

        return;
    }

    if (this->m_displayTransition && !this->m_displayTransition->UpdateState(1)) {
        this->StopDisplayTransition();
    }

    uint32_t inForm = this->m_currentClientRaceFlags & 0x1;
    CUnitDisplayTransition_C* transition = this->m_displayTransition;

    if (transition) {
        if (!(this->m_currentClientRaceFlags & 0x2)) {
            if (altered) {
                this->m_currentClientRaceFlags |= 0x1;
            } else {
                this->m_currentClientRaceFlags &= ~0x1u;
            }

            return;
        }

        bool finished = transition->m_state == 5;

        if (altered == (inForm == finished)) {
            return;
        }

        this->StopDisplayTransition();

        if (!finished) {
            return;
        }
    }

    if (altered == inForm) {
        return;
    }

    int32_t race;
    int32_t displayId;
    int32_t a4;
    uint32_t anim;
    uint32_t endAnim;

    if (this->m_currentClientRaceFlags & 0x1) {
        race = this->m_currentClientRace;
        ChrRacesRec* rec = g_chrRacesDB.GetRecord(race);
        displayId = this->m_unit->m_unitFieldBytes0[2] ? rec->m_femaleDisplayID : rec->m_maleDisplayID;
        anim = 487;
        endAnim = 497;
        a4 = -1;
    } else {
        race = this->m_unit->m_unitFieldBytes0[0];
        displayId = this->m_unit->m_unitFieldNativedisplayid;

        if (mode == 2) {
            anim = 0xFFFFFFFF;
            endAnim = 0xFFFFFFFF;
        } else {
            anim = 486;
            endAnim = 496;
        }

        a4 = 50;
    }

    uint32_t filtered = mode && this->m_worldModel->IsLoaded() ? this->m_animKitManager.FilterAnim(anim, 0) : 0xFFFFFFFF;
    int32_t effectId = (filtered != 0xFFFFFFFF) + 12;
    const SpellVisualEffectNameRec* effectName = GetSpecialSpellVisualEffectNameRec(effectId);
    const char* fileName;
    int32_t attachment;
    int32_t fadeTime;

    if (effectName) {
        fileName = effectName->m_fileName;
        attachment = GetSpecialUnitEffectAttachment(effectId);
        fadeTime = effectId != 12 ? 0 : 250;
    } else {
        fileName = NULL;
        attachment = -1;
        fadeTime = 0;
    }

    this->m_displayTransition = CUnitDisplayTransition_C::Create(this, race, displayId, a4, filtered, endAnim, fileName, attachment, fadeTime);

    if (this->m_displayTransition) {
        this->m_currentClientRaceFlags |= 0x2;

        if (filtered == 0xFFFFFFFF) {
            this->m_currentClientRaceFlags &= ~0x4u;
        } else {
            this->m_currentClientRaceFlags |= 0x4;
        }
    } else {
        this->m_currentClientRaceFlags &= 0xFFFFFFF9;
    }
}

// OFFSET: 0x005BC380/0x140230E20
void CGUnit_C::SetPredictedPowerSlot(int32_t slot, int32_t powerType, int32_t value, bool force) {
    int32_t maximum;

    if (powerType == 10 && this->m_powerBarID) {
        UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(this->m_powerBarID);

        if (bar && value < bar->m_minPower) {
            value = bar->m_minPower;
        }

        maximum = this->GetMaxPower(powerType);
    } else if (powerType == -2) {
        maximum = this->m_unit->m_unitFieldMaxhealth;
    } else {
        maximum = this->GetMaxPower(powerType);
    }

    if (value > maximum) {
        value = maximum;
    } else if (value < -maximum) {
        value = -maximum;
    }

    if (value == this->m_predictedPower[slot]) {
        return;
    }

    this->m_predictedPower[slot] = value;

    if (force || value == maximum || value == -maximum) {
        Script_SendUnitSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_POWER, "%s", Spell_C_GetPowerTypeToken(powerType));
    }

    Script_SendUnitSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_POWER_FREQUENT, "%s", Spell_C_GetPowerTypeToken(powerType));

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() || this->m_obj->m_guid == CGPetInfo::GetPet(0)) {
        CGSpellBook::UpdateUsablePower();
    }
}

// OFFSET: 0x005BC690/0x1402311C0
float CGUnit_C::GetPowerRegen(int32_t powerType, bool interrupted) const {
    int32_t slot = this->GetPowerTypeSlot(powerType);
    float regen;

    if (!interrupted) {
        float base = this->GetPowerRegenBase(powerType, 0);
        regen = slot == -1 ? base : this->m_unit->m_unitFieldPowerRegenFlatModifier[slot] + base;
    } else {
        float base = this->GetPowerRegenBase(powerType, 1);
        regen = slot == -1 ? base : this->m_unit->m_unitFieldPowerRegenInterruptedFlatModifier[slot] + base;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (powerType == 3 || powerType == 2)) {
        float haste = static_cast<const CGPlayer_C*>(this)->m_player->m_playerFieldModHasteRegen;

        if (haste > 0.0099999998f) {
            return regen / haste;
        }
    }

    return regen;
}

// OFFSET: 0x005BCF70
int32_t UnitPowerUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t slot = (fieldOffset - offsetof(CGUnitData, m_unitFieldPower)) / sizeof(uint32_t);
    int32_t powerType = unit->GetSlotPowerType(slot);
    int32_t power = unit->GetPower(powerType);
    int32_t predicted = unit->GetPredictedPower(powerType);
    bool update = true;

    if (g_predictedPowerCVar->m_intValue && powerType != 9 && (guid == ClntObjMgrGetActivePlayer() || guid == CGPetInfo::GetPet(0))) {
        update = (power > 0 && power > predicted) || (power < 0 && power < predicted);
    }

    if (update) {
        unit->SetPredictedPowerSlot(slot, powerType, unit->GetPower(powerType), 0);
    }

    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER, "%s", Spell_C_GetPowerTypeToken(powerType));
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER_FREQUENT, "%s", Spell_C_GetPowerTypeToken(powerType));
    return 1;
}

// OFFSET: 0x005BD0C0
int32_t UnitMaxPowerUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t slot = (fieldOffset - offsetof(CGUnitData, m_unitFieldMaxpower)) / sizeof(uint32_t);
    int32_t powerType = unit->GetSlotPowerType(slot);
    unit->SetPredictedPowerSlot(slot, powerType, unit->GetPower(powerType), 0);
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_MAXPOWER, "%s", Spell_C_GetPowerTypeToken(powerType));
    return 1;
}

// OFFSET: 0x005BDB50
void CGUnit_C::OnAuraAddedSpecial(const SpellRec* spell, AuraData* aura, int32_t a4) {
    bool active = this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
    bool updateSpells = false;
    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        if (effect->m_effectAura == 332 || effect->m_effectAura == 333) {
            if (active && CGActionBar::SetOverrideSpellclass(spell, i, aura->m_effectAmounts[i]) && !updateSpells) {
                updateSpells = true;
            }
        } else if (effect->m_effectAura == 358) {
            if (a4 || CGGameUI::s_loadingScreen) {
                this->ExpediteDisplayTransition(1);
                this->m_currentClientRaceFlags = 1;
                this->UpdateDisplayInfo(0);
            } else {
                this->UpdateAlteredForm(aura->m_effectAmounts[i]);
            }
        }
    }

    if (!active) {
        return;
    }

    CGActionBar::OnAuraChanged(spell->m_id);

    if (updateSpells) {
        CGSpellBook::UpdateSpells(1, 0, 0);
    }

    if (CGSpellBook::IsTrackingSpell(spell->m_id)) {
        CGMinimapFrame::UpdateTrackingSpell();
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        int32_t form = player->m_shapeshiftSuppressed ? 0 : player->m_unit->m_unitFieldBytes2[3];

        if (Spell_C_SpellHasShapeshiftMask(spell, form - 1)) {
            CGMinimapFrame::AddShapeshiftTrackingSpell(spell->m_id);
        }
    }

    if (!a4 && CGSpellBook::IsShapeshiftSpell(spell->m_id) && !SpellHasAuraEffect(spell, 36)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORM, NULL);
    }
}

// OFFSET: 0x005BF7E0/0x140235060
const char* CGUnit_C::GetAuraOverrideName(const char** realm) {
    const char* name = NULL;
    uint32_t effectCount;

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData* aura = i < this->GetAuraCount() ? &this->m_auras[i] : NULL;
        const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

        if (!spell) {
            continue;
        }

        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect) {
                continue;
            }

            if (effect->m_effectAura == 279 && (aura->m_flags & (1 << j))) {
                CGUnit_C* creator = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(aura->m_creator, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (creator) {
                    return creator->GetUnitName(realm, 0);
                }

                return GetCombatLogObjectName(aura->m_creator);
            }

            if (!name && effect->m_effectAura == 56 && (aura->m_flags & (1 << j)) && (spell->m_attributesExH & 0x400)) {
                WOWGUID requester = this->m_obj->m_guid;
                CreatureStats_C* stats = g_creatureDBCache.GetRecord(effect->m_effectMiscValue, requester, NameQueryCallback, NULL, true);

                if (stats) {
                    name = stats->m_name[0];
                }
            }
        }
    }

    return name;
}

// PROVEN: 5c7dbb06 IDENTICAL/NONE
// OFFSET: 0x005C0750
void CGUnit_C::ResetUICastingSpell() {
    this->m_castingSpellId = 0;
    this->m_castStartTime = 0;
    this->m_castEndTime = 0;
    this->UpdateSpellCastBars();
}

// PROVEN: 8c4951f5 IDENTICAL/NONE
// OFFSET: 0x005C0770
void CGUnit_C::ResetUIChannelSpell() {
    this->m_channelSpellId = 0;
    this->m_channelStartTime = 0;
    this->m_channelEndTime = 0;
    this->UpdateSpellCastBars();
}

// PROVEN: af5dec65 IDENTICAL/NONE
// OFFSET: 0x005C0790
void CGUnit_C::DelayUICastingSpell(int32_t delay) {
    this->m_castStartTime += delay;
    this->m_castEndTime += delay;
    this->m_channelSpellId = 0;
    this->m_channelStartTime = 0;
    this->m_channelEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005C0D40
void CGUnit_C::CreateProceduralEffect(const PlaySpellVisualKitAnimData* data, int32_t a3) {
    uint32_t spellId = data->m_spellRec ? data->m_spellRec->m_id : 0;

    for (uint32_t i = 0; i < 4; i++) {
        const SpellVisualKitRec* kit = data->m_kit;

        switch (kit->m_charProc[i]) {
            case 0:
            case 12: {
                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddLightning(a3, spellId, data->m_kit, owner, data->m_guid, data->m_flags & 0x300001);
                effect->Release();

                CGUnit_C* target = this;
                WOWGUID source = WOWGUID();
                const WOWGUID* targets;
                uint32_t count;

                if ((data->m_flags & 0x1000) && data->m_guid && data->m_guid != this->m_obj->m_guid) {
                    target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(data->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (!target) {
                        break;
                    }

                    source = this->m_obj->m_guid;
                    targets = &source;
                    count = 1;
                } else if (this->m_clientFlags & 0x800000) {
                    targets = NULL;
                    count = 0;
                } else if (spellId == static_cast<uint32_t>(this->m_unit->m_unitChannelSpell) && this->m_unit->m_unitFieldChannelObject && this->m_chainSpellTargets.Count() <= 1) {
                    targets = &this->m_unit->m_unitFieldChannelObject;
                    count = 1;
                } else {
                    count = this->m_chainSpellTargets.Count();

                    if (!count) {
                        this->m_chainSpellTargets.SetCount(0);
                        break;
                    }

                    targets = this->m_chainSpellTargets.m_data;
                }

                uint32_t sequence = this->unk_1020;
                this->unk_1020 = sequence + 2;
                const C3Vector* location = (this->m_clientFlags & 0x800000) ? &this->m_chainSpellLocation : NULL;
                SpellVisualsCreateChainLightning(target, effect, static_cast<int32_t>(data->m_kit->m_charParamZero[i]), data->m_kit->m_charParamTwo[i] != 0.0f, data->m_kit->m_charParamThree[i] != 0.0f, spellId, targets, count, effect->m_lightning, 12, location, sequence);
                this->m_chainSpellTargets.SetCount(0);

                if ((this->m_clientFlags & 0x800000) && data->m_spellRec && data->m_visual && data->m_visual->m_impactAreaKit != data->m_kit->m_id) {
                    SpellVisualKitRec* areaKit = g_spellVisualKitDB.GetRecord(data->m_visual->m_impactAreaKit);
                    C3Vector position = this->m_chainSpellLocation;

                    if (target && CanPlaySpellVisualKit(target->m_obj->m_guid.m_guid, 0, areaKit, data->m_visual)) {
                        PlaySpellVisuaKitData kitData(data->m_spellRec, areaKit, 3);
                        kitData.m_position = &position;
                        kitData.m_orientation = NULL;
                        kitData.unk_0014 = sequence;
                        this->PlaySpellVisualKit(kitData);
                    }
                }

                break;
            }

            case 1: {
                if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x40) && data->m_spellRec && SpellRec::IsHelpfulOrHarmful(data->m_spellRec) == 2) {
                    break;
                }

                this->ReleaseProcEffectsBySpell(spellId, WOWGUID());
                CEffect* effect = CEffect::Create();
                effect->m_spellId = spellId;
                effect->m_kitRec = data->m_kit;
                effect->m_targetGuid = data->m_guid;
                effect->m_tintColor = static_cast<uint32_t>(static_cast<int64_t>(data->m_kit->m_charParamZero[i])) | 0xFF000000;
                effect->LinkTo(&this->m_procEffects);
                break;
            }

            case 6: {
                CImVector color;
                *reinterpret_cast<uint32_t*>(&color) = static_cast<uint32_t>(static_cast<int64_t>(kit->m_charParamZero[i])) | 0xFF000000;
                SpellVisualsHandleEclipse(&color, kit->m_charParamOne[i], data->m_spellRec);
                break;
            }

            case 8: {
                this->unk_0AE4[0] = static_cast<uint32_t>(static_cast<int64_t>(kit->m_charParamZero[i])) | (static_cast<uint32_t>(static_cast<int64_t>(kit->m_charParamThree[i])) << 24);
                this->unk_0AE4[1] = static_cast<uint32_t>(static_cast<int64_t>(data->m_kit->m_charParamTwo[i]));
                break;
            }

            case 9: {
                CEffect* effect = CEffect::Create();
                C3Vector position = this->GetPosition();
                uint32_t flags = data->m_flags;

                if (data->m_position) {
                    position = *data->m_position;
                    flags &= 0x4000;
                }

                WOWGUID owner = this->m_obj->m_guid;
                effect->AddBlizzard(a3, &position, spellId, i, data->m_kit, owner, data->m_guid, flags);
                effect->Release();
                break;
            }

            case 11: {
                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddFreeze(a3, data->m_kit->m_charParamZero[i], spellId, data->m_kit, owner, data->m_guid, data->m_flags & 0x300001);
                effect->Release();
                break;
            }

            case 13: {
                if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x40) && data->m_spellRec && SpellRec::IsHelpfulOrHarmful(data->m_spellRec) == 2) {
                    break;
                }

                CImVector color;
                *reinterpret_cast<uint32_t*>(&color) = static_cast<uint32_t>(static_cast<int64_t>(data->m_kit->m_charParamZero[i])) | 0xFF000000;
                int32_t duration = FloatToInt64(data->m_kit->m_charParamTwo[i] * 1000.0f);
                int32_t delay = FloatToInt64(data->m_kit->m_charParamOne[i] * 1000.0f);
                this->StartFlash(color, delay, duration);
                break;
            }

            case 14: {
                this->ReleaseAurasBySpell(spellId, WOWGUID());
                float alpha = kit->m_charParamZero[i];

                if (alpha >= 0.0f && alpha <= 1.0f) {
                    CEffect* effect = CEffect::Create();
                    effect->m_spellId = spellId;
                    effect->m_kitRec = data->m_kit;
                    effect->m_targetGuid = data->m_guid;

                    if (data->m_kit->m_flags & 0x200) {
                        effect->m_flags |= 0x2000000;
                    }

                    effect->m_param = FloatToInt64(data->m_kit->m_charParamTwo[i] * 1000.0f);
                    effect->m_fadeAlpha = data->m_kit->m_charParamZero[i];
                    effect->LinkTo(&this->m_auraEffects);
                }

                this->UpdateAlphaEffectAlpha();
                break;
            }

            case 15: {
                uint8_t alpha = static_cast<uint8_t>(FloatToInt64(this->GetBaseAlpha() * kit->m_charParamZero[i]));
                int32_t endAlpha = FloatToInt64(kit->m_charParamTwo[i]);
                int32_t hold = FloatToInt64(kit->m_charParamThree[i]);
                int32_t type = FloatToInt64(kit->m_charParamOne[i]);
                this->StartProceduralFade(alpha, type, 500, hold, endAlpha);
                break;
            }

            case 16: {
                if (this->IsMounted() || this->m_mountModel) {
                    break;
                }

                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddMountTransition(spellId, data->m_kit, owner, data->m_guid);
                effect->m_flags |= data->m_flags & 0xC0;
                effect->Release();
                break;
            }

            case 17: {
                if (!data->m_kitType) {
                    break;
                }

                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddItemVisual(a3, static_cast<int32_t>(static_cast<int64_t>(data->m_kit->m_charParamOne[i])), spellId, data->m_kit, owner, data->m_guid, data->m_flags);
                effect->Release();
                break;
            }

            case 18: {
                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddCamouflage(a3, data->m_kit->m_charParamZero[0], data->m_kit->m_charParamOne[0], spellId, data->m_kit, owner, data->m_guid, data->m_flags);
                effect->Release();
                break;
            }

            case 19: {
                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddOrientation(a3, data->m_kit->m_charParamZero[0], data->m_kit->m_charParamOne[0], spellId, data->m_kit, owner, data->m_guid, data->m_flags);
                effect->Release();
                break;
            }

            case 20: {
                WOWGUID owner = this->m_obj->m_guid;

                if (owner == ClntObjMgrGetActivePlayer() && data->unk_0040) {
                    CEffectCurve* curve = CEffectCurve::New();
                    curve->AddPoint(0.0f, 1.0f, 0.0f, 0.0f);
                    curve->AddPoint(data->m_kit->m_charParamZero[0], data->m_kit->m_charParamOne[0], 0.0f, 0.0f);
                    curve->AddPoint(data->m_kit->m_charParamTwo[0], data->m_kit->m_charParamThree[0], 0.0f, 0.0f);
                    curve->AddPoint(1.0f, 1.0f, 0.0f, 0.0f);
                    CEffect* effect = CEffect::Create();
                    effect->AddCurveEffect(data->unk_0040, curve, spellId, data->m_kit, owner, data->m_guid, data->m_flags);
                    effect->Release();
                }

                break;
            }

            default: {
                break;
            }
        }
    }
}

// OFFSET: 0x005C1710/0x140237650
void CGUnit_C::CancelShapeshift() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || this->m_shapeshiftSuppressed) {
        return;
    }

    if (!this->m_unit->m_unitFieldBytes2[3]) {
        return;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        if (!(this->GetAura(i)->m_flags & 0x10)) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(this->GetAura(i)->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 36 && effect->m_effectMiscValue == static_cast<int32_t>(this->GetShapeshiftForm())) {
                Spell_C_CancelAura(spell, this, -1);
            }
        }
    }
}

// OFFSET: 0x005C1850/0x1402377E0
void CGUnit_C::CancelOpenStable() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !this->HasAuraEffect(292)) {
        return;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        if (!(this->GetAura(i)->m_flags & 0x10)) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(this->GetAura(i)->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 292) {
                Spell_C_CancelAura(spell, this, -1);
            }
        }
    }
}

// OFFSET: 0x005C1980
void CGUnit_C::CancelShapeshiftForMount() {
    if (this->IsShapeShifted()) {
        this->CancelShapeshift();
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        return;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData& aura = this->m_auras[i];

        if (!(aura.m_flags & 0x10)) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(aura.m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect || effect->m_effectAura != 36) {
                continue;
            }

            SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(effect->m_effectMiscValue);

            if (!form) {
                continue;
            }

            for (uint32_t d = 0; d < 4; d++) {
                if (!form->m_creatureDisplayID[d]) {
                    continue;
                }

                CreatureDisplayInfoRec* display = g_creatureDisplayInfoDB.GetRecord(form->m_creatureDisplayID[d]);
                CreatureModelDataRec* model = display ? g_creatureModelDataDB.GetRecord(display->m_modelID) : NULL;

                if (model && static_cast<int8_t>(model->m_flags) >= 0) {
                    Spell_C_CancelAura(spell, this, -1);
                }
            }
        }
    }
}

// OFFSET: 0x005C1C40
int32_t CGUnit_C::IsSplineDestinationInLiquid() {
    if (!this->m_movement->m_spline) {
        return 0;
    }

    CMoveSpline* spline = this->m_movementData.m_spline;
    C3Vector destination = spline ? spline->m_finalDestination : this->m_movementData.m_position;
    float standHeight = 0.0f;
    CreatureModelDataRec* modelData = this->m_modelData;

    if (modelData || (modelData = this->GetModelData())) {
        standHeight = modelData->m_geoBoxMaxZ - modelData->m_geoBoxMinZ;
        standHeight = this->GetTrueScale() * standHeight;
    }

    uint32_t liquidType;
    float liquidHeight;
    return World::QueryLiquid(destination, liquidType, liquidHeight) && liquidHeight - standHeight * 0.5f + 0.1f >= destination.z;
}

// OFFSET: 0x005C23C0/0x1402385E0
bool CGUnit_C::CheckSelfResurrect() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !(this->m_auraEffectMask[314 / 8] & (1 << (314 % 8)))) {
        return true;
    }

    const SpellRec* selfRes = g_spellDB.GetRecord(player->m_player->m_playerSelfResSpell);

    if (selfRes && (selfRes->m_attributesExG & 0x8000000)) {
        return true;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_auras[i].m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 314) {
                char buffer[1024];
                GetCustomSpellFailedString(buffer, sizeof(buffer), effects->m_data[j]->m_effectMiscValue);
                return false;
            }
        }
    }

    return true;
}

// OFFSET: 0x005C2B30/0x140238E10
DelayedSpellVisualKit* CGUnit_C::DelaySpellVisualKit(int32_t spellId, int32_t kitId, int32_t kitType, const C3Vector* position, WOWGUID targetGuid, int32_t a7, int32_t a8, int32_t delay, int32_t delayType, int32_t startTime, int32_t displayId, int32_t inventoryType) {
    uint32_t index;

    for (index = 0; index < this->m_delayedSpellVisualKits.m_count; index++) {
        if (!(this->m_delayedSpellVisualKits.m_data[index].m_flags & 0x8)) {
            break;
        }
    }

    if (index == this->m_delayedSpellVisualKits.m_count) {
        this->m_delayedSpellVisualKits.SetCount(this->m_delayedSpellVisualKits.m_count + 1);
    }

    DelayedSpellVisualKit* kit = &this->m_delayedSpellVisualKits.m_data[index];
    kit->m_kitId = kitId;
    kit->m_spellId = spellId;
    kit->m_kitType = kitType;

    if (position) {
        kit->m_position = *position;
        kit->m_flags |= 0x1;
        kit->m_targetGuid = targetGuid;
    } else {
        kit->m_position.x = 0.0f;
        kit->m_position.y = 0.0f;
        kit->m_position.z = 0.0f;
        kit->m_flags &= ~0x1u;
        kit->m_targetGuid = WOWGUID();
    }

    kit->m_inventoryType = inventoryType;
    kit->m_displayId = displayId;
    kit->m_delay = delay;
    kit->m_flags = (kit->m_flags & ~0x6u) | (((a7 != 0) | (((a8 != 0) | 2) << 1)) << 1);
    kit->m_delayType = delayType;
    kit->m_startTime = startTime;
    return kit;
}

// PROVEN: 98b49736 IDENTICAL/NONE
// OFFSET: 0x005C33A0
float CGUnit_C::GetDisplayModelScale() {
    return this->GetModelScale(this->m_unit->m_unitFieldDisplayid);
}

// OFFSET: 0x005C3520
void CGUnit_C::CheckApplyPeriodicClientTriggers() {
    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_auras[i].m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 48) {
                int32_t period = effect->m_effectAuraPeriod;
                Spell_C_ModifySpellValue(spell, period, 19);
                Spell_C_AddPeriodicClientTrigger(this->m_obj->m_guid, spell->m_id, effects->m_data[j]->m_effectTriggerSpell, period);
            }
        }
    }
}

// PROVEN: 29dc391e IDENTICAL/NONE
// OFFSET: 0x005C3680
void CGUnit_C::SetPredictedPower(int32_t powerType, int32_t value, bool force) {
    int32_t slot = this->GetPowerTypeSlot(powerType);

    if (slot != -1) {
        this->SetPredictedPowerSlot(slot, powerType, value, force);
    }
}

// OFFSET: 0x005C37C0
void CGUnit_C::AddHealPrediction(int32_t spellId, int32_t a3, const WOWGUID& caster, int32_t amount, bool periodic, bool channeled) {
    uint32_t count = this->m_healPredictions.Count();
    uint32_t index = 0;

    for (; index < count; index++) {
        HealPredictionInfo& info = this->m_healPredictions[index];

        if (info.m_caster == caster && info.m_spellID == spellId && info.m_periodic == periodic && info.m_channeled == channeled) {
            break;
        }
    }

    if (index == count) {
        this->m_healPredictions.SetCount(count + 1);
    }

    HealPredictionInfo& info = this->m_healPredictions[index];
    info.m_caster = caster;
    info.m_spellID = spellId;
    info.unk_000C = a3;
    info.m_amount = amount;
    info.m_periodic = periodic;
    info.m_channeled = channeled;
    WOWGUID guid = this->m_obj->m_guid;
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEAL_PREDICTION);
}

// OFFSET: 0x005C3900/0x14023A180
void CGUnit_C::RemoveHealPrediction(int32_t spellId, const WOWGUID& caster, bool periodic, bool channeled) {
    uint32_t count = this->m_healPredictions.Count();

    for (uint32_t index = 0; index < count; index++) {
        HealPredictionInfo& info = this->m_healPredictions[index];

        if (info.m_caster == caster && info.m_spellID == spellId && info.m_periodic == periodic && info.m_channeled == channeled) {
            uint32_t last = count - 1;

            if (index < last) {
                memcpy(&this->m_healPredictions[index], &this->m_healPredictions[index + 1], sizeof(HealPredictionInfo) * (last - index));
            }

            this->m_healPredictions.SetCount(last);
            WOWGUID guid = this->m_obj->m_guid;
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_HEAL_PREDICTION);
            return;
        }
    }
}

// OFFSET: 0x005C40B0
int32_t UnitDisplayPowerUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t powerType = unit->m_unit->m_unitFieldBytes0[3];
    int32_t slot = unit->GetPowerTypeSlot(powerType);
    int32_t power = slot == -1 ? 0 : unit->m_unit->m_unitFieldPower[slot];
    slot = unit->GetPowerTypeSlot(powerType);

    if (slot != -1) {
        unit->SetPredictedPowerSlot(slot, powerType, power, 0);
    }

    return 1;
}

// PROVEN: bcc25670 IDENTICAL/NONE
// OFFSET: 0x005BD160
int32_t UnitFactionUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateReaction(false);
    }

    return 1;
}

// PROVEN: 327d74fa IDENTICAL/NONE
// OFFSET: 0x005C4070
int32_t UnitPvPFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnPvPFlagChanged(*static_cast<const uint8_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x005C4120
int32_t UnitCharmedUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->m_nameDesc) {
            PlayerNameTriggerNameRegenerate(unit->m_nameDesc);
        }

        if (unit->m_namePlateFrame) {
            unit->m_namePlateFrame->UpdateNameDisplay(unit);
        }

        unit->UpdateReaction(false);
        unit->m_clientFlags &= ~0x1000;
    }

    if (*static_cast<const WOWGUID*>(oldValue) == ClntObjMgrGetActivePlayer()) {
        Spell_C_RemovePeriodicClientTrigger(guid, 0);
        return 1;
    }

    if (unit->m_unit->m_unitFieldCharmedby == ClntObjMgrGetActivePlayer()) {
        unit->CheckApplyPeriodicClientTriggers();
    }

    return 1;
}

// OFFSET: 0x005C4670/0x14023B180
void CGUnit_C::RemoveAuraEffect(uint32_t index, const SpellRec* spell, const AuraData* aura) {
    uint32_t count = this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count;

    if (index >= count) {
        return;
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect) {
                continue;
            }

            switch (effect->m_effectAura) {
                case 76:
                    CGWorldFrame::GetActiveCamera()->SetTelescope(0);
                    break;

                case 78:
                    CGSpellBook::SendCompanionUpdate(1);
                    break;

                case 260:
                    CGWorldFrame::UpdateScreenEffect();
                    break;

                case 262:
                    CGSpellActivationOverlay::UpdateUsable();
                    CGSpellBook::UpdateUsable();
                    break;

                case 273:
                    CGPlayer_C::UpdateXRayVision(spell);
                    break;

                case 275:
                    CGSpellBook::UpdateUsable();
                    break;

                case 292:
                    CGStableInfo::CloseStables(true);
                    break;

                case 334:
                    CGSpellBook::UpdateUsable();
                    CGWorldFrame::UpdateScreenEffect();
                    break;

                case 336:
                    CGGameUI::UpdateMountFlags();
                    break;

                case 341:
                    Spell_C_UpdateOverrideSpell(effect->m_effectMiscValue);
                    break;

                case 357: {
                    WOWGUID caster = WOWGUID::Make(this->GetAuraCaster(index));

                    if (caster) {
                        CGInstanceEncounter_C::DisengageUnit(caster);
                    }

                    break;
                }

                case 365:
                case 370:
                    World::SetFarClipOverride(0.0f);
                    World::SetFarClipOverrideEnabled(1);
                    World::SetUnderwaterViewEnabled(1);
                    break;

                default:
                    break;
            }
        }

        if (spell->m_id == 67483) {
            if (!this->HasAura(67484, WOWGUID())) {
                FrameScript_SignalEvent(SCRIPT_EVENT_ECLIPSE_DIRECTION_CHANGE, "%s", "none");
            }
        } else if (spell->m_id == 67484 && !this->HasAura(67483, WOWGUID())) {
            FrameScript_SignalEvent(SCRIPT_EVENT_ECLIPSE_DIRECTION_CHANGE, "%s", "none");
        }
    } else if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGSpellActivationOverlay::UpdateActiveMoverOverlays();
    }

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        switch (effect->m_effectAura) {
            case 56:
                if (spell->m_attributesExH & 0x400) {
                    WOWGUID guid = this->m_obj->m_guid;
                    CGGameUI::UnitNameUpdate(guid);
                }

                break;

            case 78:
                if (spell->m_spellVisualID[0]) {
                    SpellVisualRec* visual = g_spellVisualDB.GetRecord(spell->m_spellVisualID[0]);

                    if (visual) {
                        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);

                        if (kit && kit->m_animID >= 0 && this->m_mountAnimBehaviorId == kit->m_animID) {
                            this->m_mountAnimBehaviorId = 91;
                        }
                    }
                }

                break;

            case 279: {
                WOWGUID guid = this->m_obj->m_guid;
                CGGameUI::UnitNameUpdate(guid);
                break;
            }

            case 315:
                this->m_clientFlags &= ~0x80000000;
                break;

            case 346: {
                if (g_unitPowerBarDB.GetRecord(this->m_powerBarID)) {
                    int32_t slot;

                    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
                        slot = CGUnit_C::s_powerSlotByType[this->m_unit->m_unitFieldBytes0[1]][10];

                        if (slot != -1) {
                            this->SetPredictedPowerSlot(slot, 10, 0, 0);
                        }
                    } else {
                        slot = this->m_unit->m_unitFieldBytes0[3] != 10;
                        this->SetPredictedPowerSlot(slot, 10, 0, 0);
                    }
                }

                this->m_powerBarID = 0;
                WOWGUID guid = this->m_obj->m_guid;
                Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER_BAR_HIDE);
                break;
            }

            case 358:
                this->UpdateAlteredForm(0);
                break;

            case 369: {
                WOWGUID guid = this->m_obj->m_guid;
                Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_POWER_BAR_TIMER_UPDATE);
                break;
            }

            default:
                break;
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (this->GetControllingPlayer() == player || this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (effect && (effect->m_effectAura == 48 || effect->m_effectAura == 363)) {
                Spell_C_RemovePeriodicClientTrigger(this->m_obj->m_guid, spell->m_id);
            }
        }
    }

    if (!this->unk_0F9C[index]) {
        return;
    }

    WOWGUID caster = aura ? aura->m_creator : WOWGUID();
    this->ReleaseAurasBySpell(this->unk_0F9C[index], caster);
    this->UpdateAlphaEffectAlpha();
    this->ReleaseProcEffectsBySpell(this->unk_0F9C[index], caster);
    this->ClearSpellEffect(this->unk_0F9C[index], 1, caster.m_guid);
    const SpellVisualRec* visual = GetSpellVisual(spell, static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__)));

    if (visual) {
        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_stateDoneKit);

        if (!kit || !(kit->m_flags & 0x4000)) {
            if (CanPlaySpellVisualKit(0, this->m_obj->m_guid.m_guid, kit, visual)) {
                PlaySpellVisuaKitData data(NULL, kit, 8);
                this->PlaySpellVisualKit(data);
            }
        }
    }

    this->unk_0F9C[index] = 0;
}

// OFFSET: 0x005C4D90/0x14023BAD0
void CGUnit_C::AddAuraEffect(uint32_t index, const SpellRec* spell) {
    uint32_t count = this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count;

    if (index >= count) {
        return;
    }

    int32_t spellId = this->m_auras[index].m_spellId;

    if (!this->unk_0F9C[index] || (spell && this->unk_0F9C[index] != static_cast<uint32_t>(spell->m_id))) {
        if (spell) {
            uint32_t visionMask = GetRequiredAuraVisionBitmask(spell);

            if (visionMask) {
                CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

                if (!player || !(static_cast<uint8_t>(visionMask) & player->m_player->m_playerFieldBytes2[3])) {
                    return;
                }
            }
        }
    } else if (spell) {
        return;
    }

    this->unk_0F9C[index] = spellId;

    if (!spell) {
        return;
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    if (this->IsActivePlayer()) {
        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect) {
                continue;
            }

            switch (effect->m_effectAura) {
                case 76:
                    CGWorldFrame::GetActiveCamera()->SetTelescope(effect->m_effectMiscValue);
                    break;

                case 260:
                    CGWorldFrame::UpdateScreenEffect();
                    break;

                case 262:
                    CGSpellActivationOverlay::UpdateUsable();
                    CGSpellBook::UpdateUsable();
                    break;

                case 273:
                    CGPlayer_C::UpdateXRayVision(spell);
                    break;

                case 275:
                    CGSpellBook::UpdateUsable();
                    break;

                case 334:
                    CGSpellBook::UpdateUsable();
                    CGWorldFrame::UpdateScreenEffect();
                    break;

                case 336:
                    CGGameUI::UpdateMountFlags();
                    break;

                case 341:
                    Spell_C_UpdateOverrideSpell(effect->m_effectMiscValue);
                    break;

                case 357: {
                    int32_t priority = effect->m_effectMiscValue;
                    WOWGUID caster = WOWGUID::Make(this->GetAuraCaster(index));

                    if (static_cast<uint32_t>(priority - 1) <= 0xFD && caster) {
                        CGInstanceEncounter_C::EngageUnit(caster, priority);
                    }

                    break;
                }

                case 365:
                    World::SetFarClipOverride(2000.0f);
                    World::SetFarClipOverrideEnabled(0);
                    World::SetUnderwaterViewEnabled(0);
                    break;

                case 370:
                    World::SetFarClipOverride(450.0f);
                    break;

                default:
                    break;
            }
        }

        if (spell->m_id == 67483) {
            FrameScript_SignalEvent(SCRIPT_EVENT_ECLIPSE_DIRECTION_CHANGE, "%s", "sun");
        } else if (spell->m_id == 67484) {
            FrameScript_SignalEvent(SCRIPT_EVENT_ECLIPSE_DIRECTION_CHANGE, "%s", "moon");
        }
    } else if (this->IsActiveMover()) {
        CGSpellActivationOverlay::UpdateActiveMoverOverlays();
    }

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        switch (effect->m_effectAura) {
            case 8:
            case 20:
                if (spell->m_attributesExH & 0x1000000) {
                    AuraData* aura = this->GetAura(index);

                    if (aura && aura->m_effectAmounts[i]) {
                        WOWGUID caster = WOWGUID::Make(this->GetAuraCaster(index));
                        this->AddHealPrediction(spellId, 0, caster, aura->m_effectAmounts[i], true, false);
                    }
                }

                break;

            case 56:
                if (spell->m_attributesExH & 0x400) {
                    WOWGUID guid = this->m_obj->m_guid;
                    CGGameUI::UnitNameUpdate(guid);
                }

                break;

            case 78:
                CGSpellBook::SendCompanionUpdate(1);

                if (spell->m_spellVisualID[0]) {
                    SpellVisualRec* visual = g_spellVisualDB.GetRecord(spell->m_spellVisualID[0]);

                    if (visual) {
                        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);

                        if (kit && kit->m_animID >= 0) {
                            this->m_mountAnimBehaviorId = kit->m_animID;
                        }

                        SpellVisualKitRec* doneKit = g_spellVisualKitDB.GetRecord(visual->m_stateDoneKit);

                        if (doneKit && (doneKit->m_flags & 0x4000)) {
                            this->m_mountSpellVisualKitID = doneKit->m_id;
                        }
                    }
                }

                if (this->m_mountedAnimKit) {
                    this->m_mountedAnimKit->SetAnim(0, this->m_mountAnimBehaviorId, 255, 0, 0, 0);
                }

                break;

            case 279: {
                WOWGUID guid = this->m_obj->m_guid;
                CGGameUI::UnitNameUpdate(guid);
                break;
            }

            case 312:
                this->UpdateCharacterAnimReplacementSets();
                break;

            case 313:
                this->UpdateMountAnimReplacementSets();
                break;

            case 315:
                this->m_clientFlags |= 0x80000000;
                break;

            case 346: {
                this->m_powerBarID = effect->m_effectMiscValue;
                UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(this->m_powerBarID);
                int32_t slot = this->GetPowerTypeSlot(10);
                this->m_unit->m_unitFieldMaxpower[slot] = bar->m_maxPower;
                this->m_unit->m_unitFieldPower[slot] = bar->m_startPower;
                this->SetPredictedPowerSlot(slot, 10, bar->m_startPower, 0);
                WOWGUID guid = this->m_obj->m_guid;
                Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_POWER_BAR_SHOW);
                break;
            }

            case 369: {
                WOWGUID guid = this->m_obj->m_guid;
                Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_POWER_BAR_TIMER_UPDATE);
                break;
            }

            default:
                break;
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (this->GetControllingPlayer() == player || this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect || (effect->m_effectAura != 48 && effect->m_effectAura != 363)) {
                continue;
            }

            int32_t period = 0;

            if (effect->m_effectAura == 48) {
                period = effect->m_effectAuraPeriod;
                Spell_C_ModifySpellValue(spell, period, 19);
            }

            Spell_C_AddPeriodicClientTrigger(this->m_obj->m_guid, spell->m_id, effect->m_effectTriggerSpell, period);
        }
    }

    AuraData& aura = this->m_auras[index];
    const SpellVisualRec* visual = GetSpellVisual(spell, static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(aura.m_creator, TYPEMASK_UNIT, __FILE__, __LINE__)));

    if (!visual) {
        return;
    }

    if (visual->m_hasMissile) {
        for (CMissile* missile = this->m_missiles; missile; missile = missile->m_next) {
            if (missile->m_spellID == static_cast<uint32_t>(spellId)) {
                return;
            }
        }
    }

    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);

    if (!kit) {
        return;
    }

    for (int32_t i = 0; i < 4; i++) {
        if (kit->m_charProc[i] == 11 && !this->m_worldModel->IsLoaded()) {
            this->m_pendingStateKitSpell = spell->m_id;

            if (visual->m_flags & 0x8) {
                this->ShowHandItemSpellEffects(1, 1);
            }

            return;
        }
    }

    WOWGUID caster = this->m_auras[index].m_creator;

    if (CanPlaySpellVisualKit(caster.m_guid, this->m_obj->m_guid.m_guid, kit, visual)) {
        PlaySpellVisuaKitData data(spell, kit, 2);
        data.m_someGUID = caster;
        data.unk_0048 = this->m_auras[index].m_duration;
        this->PlaySpellVisualKit(data);
    }

    if (visual->m_flags & 0x8) {
        this->ShowHandItemSpellEffects(1, 1);
    }
}

// OFFSET: 0x005C6110/0x14023D1E0
bool CGUnit_C::CanAttack(const CGUnit_C* unit, bool ignoreFlags) const {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<const CGPlayer_C*>(this)->m_player->m_playerFlags & 0x80000)) {
        return false;
    }

    if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<const CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x10)) {
        if (!this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags[0] & 0x2)) {
            return false;
        }
    }

    uint32_t targetFlags = unit->m_unit->m_unitFieldFlags;

    if ((targetFlags & 0x2) || (targetFlags & 0x100000) || (targetFlags & 0x80) || (targetFlags & 0x10000) || (targetFlags & 0x2000000)) {
        return false;
    }

    if (unit->m_auraEffectMask[311 / 8] & (1 << (311 % 8))) {
        return false;
    }

    uint32_t flags = this->m_unit->m_unitFieldFlags;

    if (!(flags & 0x8) && (targetFlags & 0x200)) {
        return false;
    }

    if (!(targetFlags & 0x8) && (flags & 0x200)) {
        return false;
    }

    if ((!ignoreFlags && (flags & 0x8) && (targetFlags & 0x100)) || ((targetFlags & 0x8) && (flags & 0x100)) || ((unit->m_unit->m_unitFieldFlags2 & 0x10000000) && !this->IsFriendly(unit)) || unit->IsControlledByMountedPlayer()) {
        return false;
    }

    CGUnit_C* self = const_cast<CGUnit_C*>(this);
    CGUnit_C* other = const_cast<CGUnit_C*>(unit);

    if ((this->m_unit->m_unitFieldFlags2 & 0x10000) || (unit->m_unit->m_unitFieldFlags2 & 0x10000)) {
        const VehicleRec* vehicleRec = self->GetVehicleRec();
        const VehicleRec* otherVehicleRec = other->GetVehicleRec();

        if ((vehicleRec && (vehicleRec->m_flags & 0x20000000) && other->FindRootVehicle(self) == self) || (otherVehicleRec && (otherVehicleRec->m_flags & 0x20000000) && self->FindRootVehicle(other) == other)) {
            return false;
        }
    }

    if (!(this->m_unit->m_unitFieldFlags & 0x8) && !(unit->m_unit->m_unitFieldFlags & 0x8)) {
        return this->IsHatedTowards(unit) || unit->IsHatedTowards(this);
    }

    if (this->m_unit->m_unitFieldFlags & 0x8) {
        if (unit->m_unit->m_unitFieldFlags & 0x8) {
            if (this->IsFriendly(unit)) {
                return false;
            }

            CGUnit_C* controller = self->GetControllingPlayer();
            CGUnit_C* otherController = other->GetControllingPlayer();

            if (controller && otherController) {
                CGPlayerLocalData* player = static_cast<CGPlayer_C*>(controller)->m_player;
                CGPlayerLocalData* otherPlayer = static_cast<CGPlayer_C*>(otherController)->m_player;

                if (player->m_playerDuelTeam && otherPlayer->m_playerDuelTeam && player->m_playerDuelArbiter == otherPlayer->m_playerDuelArbiter) {
                    return true;
                }
            } else {
                if (controller == ClntObjMgrGetActivePlayerObj() && *unit->GetCharmerOrCreatorGuid() == CGDuelInfo::m_dueler) {
                    return true;
                }

                if (otherController == ClntObjMgrGetActivePlayerObj() && *this->GetCharmerOrCreatorGuid() == CGDuelInfo::m_dueler) {
                    return true;
                }
            }

            uint8_t otherPvp = unit->m_unit->m_unitFieldBytes2[1];
            uint8_t pvp = this->m_unit->m_unitFieldBytes2[1];
            bool notFfa;

            if (otherPvp & 0x1) {
                notFfa = !(pvp & 0x8);
            } else {
                if ((pvp & 0x4) && (otherPvp & 0x4)) {
                    return true;
                }

                if (!(pvp & 0x2) && !(otherPvp & 0x2)) {
                    return false;
                }

                notFfa = !(pvp & 0x8);
            }

            return notFfa && !(otherPvp & 0x8);
        }

        if (unit->m_unit->m_unitFieldBytes2[1] & 0x8) {
            return false;
        }
    }

    if ((unit->m_unit->m_unitFieldFlags & 0x8) && (this->m_unit->m_unitFieldBytes2[1] & 0x8)) {
        return false;
    }

    return !this->IsFriendly(unit);
}

// OFFSET: 0x005C7570/0x14023E830
int32_t CGUnit_C::UpdateObjectNameStringForUnit(CGUnit_C* unit, int32_t a1, char* buffer, uint32_t size, uint32_t a5) {
    if (a5 && (unit->m_auraEffectMask[279 / 8] & (1 << (279 % 8)))) {
        for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
            AuraData* aura = unit->GetAura(i);
            const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

            if (!spell) {
                continue;
            }

            uint32_t effectCount;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

            for (uint32_t j = 0; j < effectCount; j++) {
                const SpellEffectRec* effect = effects->m_data[j];

                if (!effect || effect->m_effectAura != 279 || !(aura->m_flags & (1 << j))) {
                    continue;
                }

                CGUnit_C* creator = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(aura->m_creator, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (creator) {
                    return CGUnit_C::UpdateObjectNameStringForUnit(creator, a1, buffer, size, 0);
                }
            }
        }
    }

    char afkText[40] = {};
    char dndText[40] = {};
    char gmText[40] = {};
    char devText[40] = {};
    char comText[40] = {};
    unit->GetAFKText(afkText, 32);
    unit->GetDNDText(dndText, 32);
    unit->GetGMText(gmText, 32);
    unit->GetDevText(devText, 32);
    unit->GetComText(comText, 32);

    char titleText[1024] = {};
    char guildText[1024] = {};
    char guildName[1024] = {};
    char subNameText[1024] = {};
    char summonedText[1024] = {};
    char foreignText[8] = {};
    uint32_t lines = 0;
    unit->GetUnitChosenTitle(titleText, sizeof(titleText), (a1 & 8) != 0, reinterpret_cast<int32_t*>(&lines));

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        if ((a1 & 4) && unit->m_obj->m_dataType == 1 && unit->m_obj->m_data) {
            WOWGUID unitGuild = unit->m_obj->m_dataType == 1 ? unit->m_obj->m_data : WOWGUID();
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
            WOWGUID playerGuild = player->m_obj->m_dataType == 1 ? player->m_obj->m_data : WOWGUID();

            if (unitGuild == playerGuild) {
                static_cast<CGPlayer_C*>(unit)->GetPlayerGuildNameTextWithTitle(guildName, sizeof(guildName), (a1 & 0x8000) != 0);

                if (guildName[0]) {
                    SStrPrintf(guildText, sizeof(guildText), "\n<%s>", guildName);
                    lines++;
                }
            } else if (static_cast<CGPlayer_C*>(unit)->GetPlayerGuildNameText()) {
                SStrPrintf(guildText, sizeof(guildText), "\n<%s>", static_cast<CGPlayer_C*>(unit)->GetPlayerGuildNameText());
                lines++;
            }
        }

        const char* realm = NULL;
        unit->GetUnitName(&realm, 1);

        if (realm && *realm && unit->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
            SStrPrintf(foreignText, sizeof(foreignText), FrameScript_GetText("FOREIGN_SERVER_LABEL", -1, GENDER_NOT_APPLICABLE));
        }
    } else {
        CreatureStats_C* stats = unit->m_creatureCacheEntry;

        if (stats) {
            const char* subName = stats->m_subName;

            if (subName && *subName && !unit->m_unit->m_unitFieldPetnumber && *subName) {
                SStrPrintf(subNameText, sizeof(subNameText), "\n<%s>", subName);
                lines++;
            }
        }

        char summonedBy[128] = {};
        CGTooltip::GetSummonedByString(unit, summonedBy, sizeof(summonedBy));

        if (summonedBy[0]) {
            SStrPrintf(summonedText, sizeof(summonedText), "\n<%s>", summonedBy);
            lines++;
        }
    }

    SStrPrintf(buffer, size, "%s%s%s%s%s%s%s%s%s%s", afkText, dndText, gmText, devText, comText, titleText, guildText, subNameText, summonedText, foreignText);

    return lines;
}

// OFFSET: 0x005C7F10
void CGUnit_C::SetUICastingSpell(const SpellRec* spell, const WOWGUID& target, int32_t castTime) {
    if (!spell) {
        return;
    }

    this->m_castingSpellId = spell->m_id;
    this->m_castingObject = target;
    uint32_t now = FrameTime::GetCurTimeMs();
    this->m_castStartTime = now;
    this->m_castEndTime = castTime + now;
    int32_t arenaIndex;

    if (this->m_obj->m_guid == CGGameUI::m_lockedTarget || this->m_obj->m_guid == CGGameUI::m_focusTarget || CGBattlefieldInfo::GetArenaOpponentIndex(this->m_obj->m_guid, &arenaIndex)) {
        this->ProcessCastNotInterruptible(spell);
    }

    this->m_channelSpellId = 0;
    this->m_channelStartTime = 0;
    this->m_channelEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005C7FD0
void CGUnit_C::SetUICastingDirect(const SpellRec* spell, const WOWGUID& target, uint32_t start, uint32_t end) {
    if (!spell) {
        return;
    }

    this->m_castingSpellId = spell->m_id;
    this->m_castingObject = target;
    this->m_castEndTime = end;
    this->m_castStartTime = start;

    if (this->m_obj->m_guid == CGGameUI::m_lockedTarget || this->m_obj->m_guid == CGGameUI::m_focusTarget) {
        this->ProcessCastNotInterruptible(spell);
    }

    this->m_channelSpellId = 0;
    this->m_channelStartTime = 0;
    this->m_channelEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005C8070
void CGUnit_C::SetUIChannelSpell(const SpellRec* spell, int32_t duration) {
    if (!spell) {
        return;
    }

    this->m_channelSpellId = spell->m_id;
    uint32_t now = FrameTime::GetCurTimeMs();
    this->m_channelStartTime = now;
    this->m_channelEndTime = duration + now;
    int32_t arenaIndex;

    if (this->m_obj->m_guid == CGGameUI::m_lockedTarget || this->m_obj->m_guid == CGGameUI::m_focusTarget || CGBattlefieldInfo::GetArenaOpponentIndex(this->m_obj->m_guid, &arenaIndex)) {
        this->ProcessCastNotInterruptible(spell);
    }

    this->m_castingSpellId = 0;
    this->m_castStartTime = 0;
    this->m_castEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005C8120
void CGUnit_C::SetUIChannelDirect(const SpellRec* spell, uint32_t start, uint32_t end) {
    if (!spell) {
        return;
    }

    this->m_channelSpellId = spell->m_id;
    this->m_channelEndTime = end;
    this->m_channelStartTime = start;

    if (this->m_obj->m_guid == CGGameUI::m_lockedTarget || this->m_obj->m_guid == CGGameUI::m_focusTarget) {
        this->ProcessCastNotInterruptible(spell);
    }

    this->m_castingSpellId = 0;
    this->m_castStartTime = 0;
    this->m_castEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005C8540/0x14023FB50
int32_t CGUnit_C::PlaySpellVisualKit_DelayLightningEffects(const SpellRec* spell, const SpellVisualKitRec* kit, SPELL_VISUAL_KIT_TYPE type, const C3Vector* position, WOWGUID guid, int32_t a6, int32_t a7, int32_t a8, int32_t a9, uint32_t a10, const SpellVisualRec* visual, uint32_t a12, int32_t a13) {
    if (!spell || !visual || !Spell_C_SpellVisualHasChainEffect(visual) || a8 || !(kit->m_flags & 0x1)) {
        return 0;
    }

    this->DelaySpellVisualKit(spell->m_id, kit->m_id, type, position, guid, a6, a7, a9, 0, FrameTime::GetCurTimeMs() + 10000, a12, a13);
    return 1;
}

// OFFSET: 0x005C9060
void CGUnit_C::OnAuraRemoved(uint32_t index, AuraData* aura) {
    int32_t spellId = aura->m_spellId;
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    this->RemoveAuraEffect(index, spell, aura);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGSpellActivationOverlay::OnAuraRemoved(spell);
    }

    if (spell->m_attributesExH & 0x1000000) {
        this->RemoveHealPrediction(spellId, aura->m_creator, true, (spell->m_attributesEx & 0x44) != 0);
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        bool updateSpells = false;
        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (effect && (effect->m_effectAura == 332 || effect->m_effectAura == 333)) {
                int32_t overrideSpell = aura->m_effectAmounts[i];

                if (CGActionBar::ClearOverrideSpellclass(overrideSpell) && !updateSpells) {
                    updateSpells = true;
                }

                Spell_C_CancelSpellsWithId(overrideSpell);
            }
        }

        CGActionBar::OnAuraChanged(spellId);

        if (updateSpells) {
            CGSpellBook::UpdateSpells(1, 0, 0);
        }

        if (CGSpellBook::IsTrackingSpell(spellId)) {
            CGMinimapFrame::UpdateTrackingSpell();
        }

        if (CGSpellBook::IsShapeshiftSpell(spellId) && !SpellHasAuraEffect(spell, 36)) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORM, NULL);
        }
    }

    UnitCombatLogAuraAddedOrRemoved(this, aura, spell, false);

    if (SpellHasAuraEffect(spell, 312)) {
        this->UpdateCharacterAnimReplacementSets();
    }

    if (SpellHasAuraEffect(spell, 313)) {
        this->UpdateMountAnimReplacementSets();
    }
}

// OFFSET: 0x005C9240
void CGUnit_C::OnAuraAdded(uint32_t index, AuraData* aura) {
    const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

    if (!spell) {
        return;
    }

    this->AddAuraEffect(index, spell);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGSpellActivationOverlay::OnAuraAdded(spell);
    }

    UnitCombatLogAuraAddedOrRemoved(this, aura, spell, true);
    this->OnAuraAddedSpecial(spell, aura, 0);
}

// OFFSET: 0x005C92D0
void CGUnit_C::UpdateAuraHealPrediction(uint32_t index, AuraData* aura) {
    const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

    if (!spell) {
        return;
    }

    if (SpellHasAuraEffect(spell, 369)) {
        WOWGUID guid = this->m_obj->m_guid;
        Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_POWER_BAR_TIMER_UPDATE);
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && (effect->m_effectAura == 8 || effect->m_effectAura == 20) && (spell->m_attributesExH & 0x1000000) && aura->m_effectAmounts[i]) {
            this->AddHealPrediction(spell->m_id, 0, aura->m_creator, aura->m_effectAmounts[i], true, false);
        }
    }
}

// OFFSET: 0x005C9750/0x140241220
void CGUnit_C::UpdateChannelEffects() {
    this->m_clientFlags &= ~0x8000u;
    const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

    if (!spell) {
        return;
    }

    if (spell->m_attributesEx & 0x4000) {
        this->m_clientFlags |= 0x8000;

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            WOWGUID target = this->m_unit->m_unitFieldChannelObject;
            this->SetTrackingTarget(target, 1);
        }
    }

    const SpellVisualRec* visual = GetSpellVisual(spell, this);

    if (!visual) {
        return;
    }

    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_channelKit);

    if (!kit) {
        return;
    }

    SpellVisualKitRec* castKit = g_spellVisualKitDB.GetRecord(visual->m_castKit);
    uint32_t playAnim = !castKit || castKit->m_animID;

    if (CanPlaySpellVisualKit(this->m_obj->m_guid.m_guid, this->m_unit->m_unitFieldChannelObject.m_guid, kit, visual)) {
        PlaySpellVisuaKitData data(spell, kit, 2);
        data.m_animParam = playAnim;
        data.m_someGUID = this->m_obj->m_guid;
        this->PlaySpellVisualKit(data);
    }

    this->CreateFishingLineEffect();
}

// OFFSET: 0x005C98D0
void CGUnit_C::AuraVisionUpdated(uint8_t vision) {
    bool changed = false;

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_auras[i].m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t mask = GetRequiredAuraVisionBitmask(spell);

        if (!(mask & vision)) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (static_cast<uint8_t>(mask) & player->m_player->m_playerFieldBytes2[3]) {
            this->AddAuraEffect(i, spell);
        } else {
            this->RemoveAuraEffect(i, spell, NULL);
        }

        changed = true;
    }

    if (changed) {
        Script_SendUniqueSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_AURA);
    }
}

// OFFSET: 0x005CB270/0x1402432F0
void CGUnit_C::ScanAuraSpellsOnDisplayUpdate() {
    for (uint32_t i = this->GetAuraCount(); i; ) {
        i--;

        const SpellRec* spell = g_spellDB.GetRecord(this->m_auras[i].m_spellId);

        if (spell) {
            this->AddAuraEffect(i, spell);
        }
    }
}

// OFFSET: 0x005CD060
void CGUnit_C::OnChannelSpellChanged(int32_t oldSpell) {
    if (oldSpell) {
        this->ReleaseAurasBySpell(oldSpell, WOWGUID());
        this->UpdateAlphaEffectAlpha();
        this->ReleaseProcEffectsBySpell(oldSpell, WOWGUID());
        this->ClearSpellEffect(oldSpell, 1, this->m_obj->m_guid.m_guid);

        if (this->m_fishingLineEffect) {
            this->m_fishingLineEffect->Release();
            this->m_fishingLineEffect = NULL;
        }

        for (uint32_t i = 0; i < (this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count); i++) {
            if (this->unk_0F9C[i] && this->unk_0F9C[i] == static_cast<uint32_t>(oldSpell)) {
                this->unk_0F9C[i] = 0;
            }
        }
    }

    if (this->m_unit->m_unitChannelSpell) {
        this->UpdateChannelEffects();
    } else {
        const SpellRec* spell = g_spellDB.GetRecord(oldSpell);

        if (spell && (spell->m_attributesEx & 0x4000) && this->IsActiveMoverTracking(2)) {
            this->ClearTrackingTarget(false, true);
        }

        this->m_castSchoolImmunities = 0;
        this->m_castImmunities = 0;
        this->ClearSpellEffect(oldSpell, 1, this->m_obj->m_guid.m_guid);

        if (this->m_fishingLineEffect) {
            this->m_fishingLineEffect->Release();
            this->m_fishingLineEffect = NULL;
        }
    }

    this->UpdateEffectAnimKit();
    this->m_animationState |= 0x80000;
}

// OFFSET: 0x005CEAD0
void CGUnit_C::OnAuraUpdate(CDataStore* msg, bool updateAll) {
    uint32_t time = FrameTime::GetCurTimeMs();
    uint32_t oldCount = this->GetAuraCount();
    AuraData* oldAuras = static_cast<AuraData*>(alloca(sizeof(AuraData) * oldCount));
    memcpy(oldAuras, &this->m_auras[0], sizeof(AuraData) * oldCount);
    uint8_t updated[256];

    if (updateAll) {
        memset(&this->m_auras[0], 0, sizeof(AuraData) * this->GetAuraCount());
        memset(updated, 1, sizeof(updated));
    } else {
        memset(updated, 0, sizeof(updated));
    }

    while (!msg->IsRead()) {
        uint8_t slot;
        msg->Get(slot);
        updated[slot] = 1;
        this->AddAuraSlot(slot);
        this->m_auras[slot].Unpack(this, time, msg);
    }

    this->m_clientFlags &= ~0x40000;
    this->UpdateAuraEffectMask();
    bool changed = false;

    for (uint32_t i = 0; i < oldCount; i++) {
        if (!updated[i]) {
            continue;
        }

        int32_t oldSpellId = oldAuras[i].m_spellId;
        uint32_t oldActive = oldAuras[i].m_flags & 0x7;
        int32_t spellId = this->m_auras[i].m_spellId;
        uint32_t active = this->m_auras[i].m_flags & 0x7;
        bool wasActive = oldSpellId && oldActive;
        bool stillActive = oldSpellId == spellId && active;

        if ((wasActive && !stillActive) || !spellId) {
            if (updateAll) {
                const SpellRec* spell = g_spellDB.GetRecord(oldSpellId);

                if (spell) {
                    this->RemoveAuraEffect(i, spell, NULL);
                }
            } else {
                this->OnAuraRemoved(i, &oldAuras[i]);
                changed = true;
            }
        }
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        if (!updated[i]) {
            continue;
        }

        bool hadOld = i < oldCount;
        int32_t oldSpellId = hadOld ? oldAuras[i].m_spellId : 0;
        uint32_t oldActive = hadOld ? oldAuras[i].m_flags & 0x7 : 0;
        AuraData& aura = this->m_auras[i];
        int32_t spellId = aura.m_spellId;
        bool same = oldSpellId == spellId && oldActive;

        if (!spellId || !(aura.m_flags & 0x7)) {
            continue;
        }

        if (updateAll) {
            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (!spell) {
                continue;
            }

            this->AddAuraEffect(i, spell);
            this->OnAuraAddedSpecial(spell, &aura, 1);
        } else if (same) {
            if (!hadOld) {
                continue;
            }

            if (oldAuras[i].m_creator != aura.m_creator) {
                this->RetargetAuraEffects(aura.m_spellId, oldAuras[i].m_creator, aura.m_creator);
            }

            if (oldAuras[i].m_stackCount == aura.m_stackCount) {
                if (aura.m_flags & 0x100) {
                    const SpellRec* spell = g_spellDB.GetRecord(this->m_auras[i].m_spellId);

                    if (spell) {
                        UnitCombatLogAuraRefreshed(this, &this->m_auras[i], spell);
                    }
                }
            } else {
                const SpellRec* spell = g_spellDB.GetRecord(spellId);
                UnitCombatLogAuraModifiedApplications(this, aura.m_creator, spell, oldAuras[i].m_stackCount, aura.m_stackCount, (aura.m_flags & 0x80) != 0);
            }

            this->UpdateAuraHealPrediction(i, &aura);
        } else {
            this->OnAuraAdded(i, &aura);
            changed = true;
        }
    }

    WOWGUID guid = this->m_obj->m_guid;
    int32_t arenaIndex;
    int32_t arenaOpponent = CGBattlefieldInfo::GetArenaOpponentIndex(guid, &arenaIndex);
    bool update = arenaOpponent != 0;

    if (changed && (guid == ClntObjMgrGetActivePlayer() || this->m_unit->m_unitFieldCharmedby == ClntObjMgrGetActivePlayer() || *this->GetCharmerOrCreatorGuid() == ClntObjMgrGetActivePlayer() || this->m_obj->m_guid == CGGameUI::m_lockedTarget || this->m_obj->m_guid == CGGameUI::m_focusTarget)) {
        update = true;
    }

    if (update) {
        CGSpellBook::UpdateUsable();
        CGSpellActivationOverlay::UpdateUsable();
        CGSpellActivationOverlay::UpdateActiveMoverOverlays();

        if (this->m_obj->m_guid == CGGameUI::m_lockedTarget || this->m_obj->m_guid == CGGameUI::m_focusTarget || arenaOpponent) {
            const SpellRec* spell = NULL;

            if (this->m_castingSpellId) {
                spell = g_spellDB.GetRecord(this->m_castingSpellId);
            } else if (this->m_channelSpellId) {
                spell = g_spellDB.GetRecord(this->m_channelSpellId);
            }

            if (spell && spell->m_spellCategoriesID) {
                SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

                if (categories && (categories->m_preventionType & 0x1)) {
                    bool processed = this->ProcessCastNotInterruptible(spell);

                    if (this->IsCastNotInterruptible() != processed) {
                        this->UpdateSpellCastBars();
                        WOWGUID target = this->m_obj->m_guid;
                        Script_SendUnitSignal(target, this->IsCastNotInterruptible() + SCRIPT_EVENT_UNIT_SPELLCAST_INTERRUPTIBLE);
                    }
                }
            }
        }
    }

    guid = this->m_obj->m_guid;
    Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_AURA);
}

// OFFSET: 0x005CFFD0
void CGUnit_C::OnFlagChanged(uint32_t oldFlags) {
    uint32_t flags = this->m_unit->m_unitFieldFlags;
    uint32_t changed = oldFlags ^ flags;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->OnFlagChangedActivePlayer(flags, changed);
    } else if (static_cast<int32_t>(this->m_unit->m_unitFieldHealth) > 0 && (changed & 0x400)) {
        this->UpdateLootAnimKit(0);
    }

    if (changed & 0x200000) {
        const CGUnitVirtualItem* mainHand = this->GetVirtualItem(0, true);

        if (mainHand && mainHand->m_classID == 2) {
            const CGUnitVirtualItem* item;

            if ((this->m_unit->m_unitFieldFlags & 0x200000) && (item = this->GetVirtualItem(0, true)) && item->m_classID == 2 && this->m_sheatheState <= 1) {
                this->RemoveHandItemModel(INVSLOT_MAINHAND, this->m_sheatheState ? 0 : mainHand->m_sheatheType, false);
            } else {
                if (this->unk_0948) {
                    this->unk_0948->DeleteSelf();
                }

                this->AddHandItem(0);
            }
        }

        const CGUnitVirtualItem* offHand = this->GetVirtualItem(1, true);

        if (offHand && offHand->m_classID == 2) {
            const CGUnitVirtualItem* item;

            if ((((this->m_unit->m_unitFieldFlags & 0x200000) && (this->GetVirtualItem(0, true), !this->IsDisarmed(0)) && (item = this->GetVirtualItem(1, true)) && item->m_classID == 2) || static_cast<int8_t>(this->m_unit->m_unitFieldFlags2) < 0) && this->m_sheatheState == 1) {
                this->RemoveHandItemModel(INVSLOT_OFFHAND, 0, false);
            } else {
                this->AddHandItem(1);
            }
        }

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGSpellBook::UpdateUsable();
        }
    }

    if (changed & 0x180) {
        this->UpdateReaction(false);
    }

    if (changed & 0x800) {
        const WOWGUID& owner = this->m_unit->m_unitFieldCharmedby ? this->m_unit->m_unitFieldCharmedby : this->m_unit->m_unitFieldSummonedby;

        if (owner == ClntObjMgrGetActivePlayer()) {
            CGPetInfo::SetIsAttackingTarget((flags >> 11) & 1);
        }
    }

    if ((oldFlags & 0x10) && !(flags & 0x10) && this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        CMoveSpline* spline = this->m_movement->m_spline;

        if ((!spline || (spline->m_flags & 0x80)) && !this->CanSwim() && !(flags & 0x4000) && !(oldFlags & 0x4000)) {
            if (this->m_movement->m_flags & 0x100000) {
                this->OnSwimStop(0, 1);
            }

            this->m_clientFlags &= ~0x200000u;
        }
    }

    if (changed & 0x4000000) {
        CGGameUI::UnitTooltipUpdate(this->m_obj->m_guid);
    }

    if (changed & 0x8000000) {
        Script_SendUniqueSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_AURA);
    }

    if (changed & 0x2000000) {
        if (this->m_unit->m_unitFieldFlags & 0x2000000) {
            CGGameUI::BreakTarget(this->m_obj->m_guid);
        } else {
            this->RefreshInteractIcon();
        }

        Script_SendUniqueSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_TARGETABLE_CHANGED);
    }

    if ((changed & 0x100000) && !(flags & 0x100000) && static_cast<int32_t>(this->m_unit->m_unitFieldMountdisplayid) <= 0) {
        this->PlayTaxiDismountKit();
    }
}

// PROVEN: d1639ee8 IDENTICAL/NONE
// OFFSET: 0x005D1020
void CGUnit_C::UpdateChainTargets(const TSStackArray<WOWGUID>& targets, int32_t spellId) {
    WOWGUID first = targets.m_data[0];

    if (first && first != this->m_unit->m_unitFieldChannelObject) {
        const SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (spell && (spell->m_attributesEx & 0x4)) {
            this->m_unit->m_unitFieldChannelObject = first;
        }
    }

    this->SaveChainSpellTargets(targets);
    this->OnChannelSpellChanged(spellId);
}

// PROVEN: e871f182 IDENTICAL/NONE
// OFFSET: 0x005D3440
int32_t Packet_SMSG_STANDSTATE_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t standState = 0;
    msg->Get(standState);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateClientStandState(player->GetClientStandState(), standState);
    }

    return 1;
}

// OFFSET: 0x005D4A70
void CGUnit_C::UpdateDisplay(uint32_t time) {
    if ((this->m_modelFlags & 0x200000) && this->GetObjectModel()->IsLoaded()) {
        this->ModelChanged();
    }

    if (this->m_missiles) {
        if (this->m_clientFlags & 0x40000000) {
            this->ReleaseMissiles(this->m_animEventPosition);
        }

        if (static_cast<int32_t>(time - this->m_lastMissileTime - 2000) >= 0) {
            C3Vector position(0.0f, 0.0f, 0.0f);
            position = this->GetMissileFirePosition();
            this->ReleaseMissiles(position);
        }

        if (!this->m_castAnimKit) {
            C3Vector position(0.0f, 0.0f, 0.0f);
            position = this->GetMissileFirePosition();
            this->ReleaseMissiles(position);
        }
    }

    this->m_clientFlags &= ~0x40000000;

    if (this->m_clientFlags & 0x800) {
        if (this->m_missiles && this->m_missiles->m_model) {
            if (this->m_missiles->m_model->IsLoaded()) {
                this->ReleaseMissiles();
                this->m_clientFlags &= ~0x800;
            }
        } else {
            this->m_clientFlags &= ~0x800;
        }
    }

    if (this->m_displayTransition && !this->m_displayTransition->UpdateState(1)) {
        this->StopDisplayTransition();
    }

    if (!CGGameUI::FoundHiddenHostile() && !this->IsReadyToDraw()) {
        CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (this->UnitReaction(player) <= 1) {
            CGGameUI::SetFoundHiddenHostile();
        }
    }

    if (!this->m_worldObject || !this->GetObjectModel() || !this->GetObjectModel()->IsLoaded()) {
        return;
    }

    if (!World::QueryObjectVisible(this->m_worldObject)) {
        this->DestroyNamePlate();
    }

    if (this->m_clientFlags & 0x2) {
        this->HandleBloodPool(time);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && !(this->m_obj->m_type & TYPEMASK_PLAYER) && this->IsAutoTracking()) {
        this->HandleTracking(0.0f, 0.0f);
    }

    if (this->m_vehiclePassenger && (this->m_vehiclePassenger->m_flags & 0x10)) {
        this->UpdateWorldObject(0);
    }

    for (CEffect* effect = this->m_effectList; effect; ) {
        CEffect* next = effect->m_next;
        effect->EffectUpdate();
        effect = next;
    }

    this->UpdateCamouflage();
    uint32_t now = FrameTime::GetCurTimeMs();

    if (CGGameUI::m_lockedTarget == this->m_obj->m_guid) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && ((player->m_combatModeFlags & 0x2) || player->m_combat.m_attacking) && player->CanAttack(this, false)) {
            uint32_t flashTime = s_attackFlashTime;
            this->m_clientFlags |= 0x10;

            if (static_cast<int32_t>(now - flashTime - 500) >= 0) {
                s_attackFlashToggle = s_attackFlashToggle == 0;
                flashTime = now;
                s_attackFlashTime = now;
            }

            float fraction = static_cast<float>(flashTime - now + 500) * 0.0020000001f;

            if (s_attackFlashToggle) {
                fraction = 1.0f - fraction;
            }

            s_attackTargetColor[1] = static_cast<uint8_t>(static_cast<int32_t>(fraction * 128.0f));
            PlayerNameTriggerColorUpdate(this->m_nameDesc);
        } else if (this->m_clientFlags & 0x10) {
            this->m_clientFlags &= ~0x10u;
            PlayerNameTriggerColorUpdate(this->m_nameDesc);
        }
    } else if (this->m_clientFlags & 0x10) {
        this->m_clientFlags &= ~0x10u;
        PlayerNameTriggerColorUpdate(this->m_nameDesc);
    }

    PlayerNameUpdateDrawCallback(this->m_nameDesc);
}

// OFFSET: 0x005D5130/0x14024F6D0
void CGUnit_C::PlaySpellVisualKit_PlayAnims(const PlaySpellVisualKitAnimData* data) {
    int32_t spellId = 0;

    if (data->m_spellRec) {
        spellId = data->m_spellRec->m_id;
    }

    this->m_animationState &= ~0x80;
    int32_t updateAnimKits = 1;

    if (data->m_animParam) {
        if (data->m_kit->m_animKitID > 0) {
            CEffect* effect = CEffect::Create();
            WOWGUID owner = this->m_obj->m_guid;
            effect->AddAnimKitEffect(spellId, data->m_kit, owner, data->m_guid, data->m_flags, data->unk_0020, data->m_stopCallback, data->m_stopParam);
            effect->Release();
        }

        const SpellVisualKitRec* kit = data->m_kit;
        uint8_t playFlags = CGUnit_C::KitFlagsToPlayFlags(data->m_spellRec, data->m_visual, kit);

        if (kit->m_animID >= 0) {
            SpellVisualKitRec* castKit = data->m_visual ? g_spellVisualKitDB.GetRecord(data->m_visual->m_castKit) : NULL;
            bool sameAnim = castKit && castKit->m_animID > 0 && castKit->m_animID == static_cast<int32_t>(this->GetCurrentTorsoAnimId());
            int32_t castTime = this->GetSpellCastingTime(data->m_spellRec);
            bool holdCast = (data->m_flags & 0x40) && castTime > 0 && sameAnim;
            bool holdChannel = (data->m_flags & 0x1000) && sameAnim;

            if (data->m_kitType == 2) {
                if (!holdChannel) {
                    if (data->m_hasEffect) {
                        goto finish;
                    }

                    CEffect* effect = CEffect::Create();
                    WOWGUID owner = this->m_obj->m_guid;
                    effect->AddAnimEffect(spellId, data->m_kit, owner, data->m_guid, data->m_flags, data->unk_0020);
                    effect->Release();
                }
            } else if (!(data->m_flags & 0x40) || (castTime > 0 && !holdCast)) {
                uint32_t anim = data->m_kit->m_animID;

                if (AnimationDataHasFlags(anim, 0x800000)) {
                    this->PlayWoundAnimKit(anim == ANIM_COMBAT_CRITICAL);
                } else {
                    if (data->m_flags & 0x40) {
                        int32_t precast = data->m_kit->m_flags & 0x40;

                        if (static_cast<int32_t>(anim) > 0) {
                            this->PlayPrecastAnimKit(1, precast, spellId, playFlags, anim, data->m_kit->m_startAnimID > 0 ? data->m_kit->m_startAnimID : anim);
                        }
                    } else {
                        int32_t fullBody = this->IsVehicle() && this->m_vehicle->HasHighPriorityCastingAnim();
                        uint32_t castAnim = data->m_kit->m_animID;

                        if ((data->m_flags & 0x80) && AnimationDataHasFlags(castAnim, 0x200000)) {
                            this->PlayCombatActionAnimKit(AnimationDataHasFlags(castAnim, 0x400000) ? 4 : 1, castAnim, playFlags, spellId);
                        } else if (data->m_flags & 0x80) {
                            this->PlayCastAnimKit(castAnim, playFlags, fullBody, spellId);
                        } else {
                            this->PlayOneShotSpellAnimKit(castAnim, playFlags, fullBody, spellId);
                        }
                    }

                    updateAnimKits = 0;
                }

                if (data->m_flags & 0x80) {
                    this->m_specialIdleAnim = data->m_kit->m_animID;
                }
            }

            if (!data->m_hasEffect && (holdCast || holdChannel)) {
                CEffect* effect = CEffect::Create();
                WOWGUID owner = this->m_obj->m_guid;
                effect->AddAnimEffect(spellId, data->m_kit, owner, data->m_guid, data->m_flags, data->unk_0020);
                effect->Release();
            }
        }
    }

finish:
    if (data->m_spellRec && (data->m_spellRec->m_attributes & 0x2) && (data->m_kitType == 4 || data->m_kitType == 0)) {
        this->SetSheatheState(2, 1, 0);
    }

    if (updateAnimKits) {
        this->UpdateEffectAnimKit();
        this->UpdateDeathAnimKit();
    }

    if (data->m_animParam && data->unk_0014) {
        this->PlayWoundAnimKit(0);
    }

    this->CreateProceduralEffect(data, 0);

    if (this->unk_0AE4[1]) {
        this->UpdateWeaponTrails(&this->unk_0AE4[0], this->unk_0AE4[1]);
        this->unk_0AE4[1] = 0;
    }

    if (!data->m_hasEffect && data->m_kit->m_shakeID > 0) {
        C3Vector objectPosition;
        const C3Vector* shakePosition = data->m_position;

        if (!shakePosition) {
            objectPosition = this->GetPosition();
            shakePosition = &objectPosition;
        }

        SpellVisualsPlayCameraShakeID(data->m_kit->m_shakeID, shakePosition);
    }
}

// OFFSET: 0x005D5520/0x14024FB80
void CGUnit_C::PlaySpellVisualKit_HandleWeapons(const SpellRec* spell, const SpellVisualKitRec* kit, const C3Vector* position, WOWGUID guid, uint32_t a5, uint32_t flags, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, int32_t), int32_t* hasEffect, uint32_t a9, int32_t a10) {
    int32_t spellId = 0;

    if (spell) {
        spellId = spell->m_id;
    }

    SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(kit->m_leftWeaponEffect);

    if (effectName) {
        CreatureModelDataRec* modelData = this->GetModelData();

        if (modelData && !(modelData->m_flags & 0x10)) {
            if (!spell || !(spell->m_attributes & 0x40000)) {
                this->SetSheatheState(0, 1, 0);
            }

            this->PlayEffectName(2, 0, spellId, kit, effectName, flags, callback, position, guid.m_guid, a5, NULL, -1, a9, a10);
            *hasEffect = 1;
        }
    }

    effectName = g_spellVisualEffectNameDB.GetRecord(kit->m_rightWeaponEffect);

    if (effectName) {
        CreatureModelDataRec* modelData = this->GetModelData();

        if (modelData && !(modelData->m_flags & 0x10)) {
            if (!spell || !(spell->m_attributes & 0x40000)) {
                this->SetSheatheState(0, 1, 0);
            }

            this->PlayEffectName(1, 0, spellId, kit, effectName, flags, callback, position, guid.m_guid, a5, NULL, -1, a9, a10);
            *hasEffect = 1;
        }
    }
}

// PROVEN: bd97f245 IDENTICAL/NONE
// OFFSET: 0x005D5AE0
int32_t VirtualItemIDMirrorHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnVirtualItemChanged((fieldOffset - offsetof(CGUnitData, m_unitVirtualItemSlotId)) / sizeof(uint32_t), *static_cast<const int32_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x005CADF0
int32_t UnitModeUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    CreatureStats_C* stats = g_creatureDBCache.GetRecord(unit->m_obj->m_entryID, guid, CreatureQueryCallback, NULL, false);

    if (stats) {
        unit->DestroyNamePlate();
        unit->m_creatureCacheEntry = stats;
        CGGameUI::UnitNameUpdate(guid);
        Script_SendUnitSignal(guid, 148);

        TotemInfo* totem = CGGameUI::GetTotemInfo(guid);

        if (totem) {
            totem->m_name = unit->GetUnitName(NULL, 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TOTEM_UPDATE, "%d", totem->m_slot + 1);
        }
    }

    return 1;
}

// PROVEN: 1f23a8e0 IDENTICAL/NONE
// OFFSET: 0x005C41E0
int32_t PetNameChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->GetUnitName(NULL, 1);

        const WOWGUID* owner = unit->m_unit->m_unitFieldCharmedby ? &unit->m_unit->m_unitFieldCharmedby : &unit->m_unit->m_unitFieldSummonedby;

        if (*owner == ClntObjMgrGetActivePlayer()) {
            FrameScript_SignalEvent(SCRIPT_EVENT_LOCALPLAYER_PET_RENAMED, NULL);
        }
    }

    return 1;
}

// OFFSET: 0x005CB020
int32_t PetNumberChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->GetUnitName(NULL, 1);
        unit->UpdateModelScale(NULL);
        unit->DestroyNamePlate();
        unit->RefreshSoundData();
    }

    Script_SendUnitSignal(guid, 144);
    CGPetInfo::UpdatePetTempBar();

    if (guid == CGPetInfo::GetPet(0)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_UI_UPDATE, NULL);
    }

    return 1;
}

// PROVEN: eb3b4c8f IDENTICAL/NONE
// OFFSET: 0x005CB110
int32_t UnitScaleUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->HandleScaleUpdate(*static_cast<const float*>(oldValue));
    }

    return 1;
}

// PROVEN: d25e5ae7 IDENTICAL/NONE
// OFFSET: 0x005CF580
int32_t UnitLevelUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    uint32_t oldLevel = *static_cast<const uint32_t*>(oldValue);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_unit->m_unitFieldLevel != oldLevel) {
        unit->OnLevelChange();
    }

    return 1;
}

// OFFSET: 0x005D33B0
int32_t SheatheStateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    uint32_t oldState = *static_cast<const uint32_t*>(oldValue);

    if (unit->m_obj->m_guid != ClntObjMgrGetActivePlayer() || unit->m_sheatheState == oldState) {
        unit->SetSheatheState(unit->m_unit->m_unitFieldBytes2[0], 1, 1);
    }

    return 1;
}

// PROVEN: 65d73fe3 IDENTICAL/NONE
// OFFSET: 0x005D32D0
int32_t UnitFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnFlagChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// PROVEN: 295dd4fc IDENTICAL/NONE
// OFFSET: 0x005D3310
int32_t UnitFlag2UpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnFlag2Changed(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// PROVEN: 71ed7a7f IDENTICAL/NONE
// OFFSET: 0x005B68B0
int32_t UnitVisFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnVisFlagChanged(*static_cast<const uint8_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x005CAEB0
int32_t UnitHealthUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    unit->m_predictedHealth = unit->m_unit->m_unitFieldHealth;

    if (!(static_cast<float>(unit->m_unit->m_unitFieldHealth) / static_cast<int32_t>(unit->m_unit->m_unitFieldMaxhealth) < 0.2f) || unit->IsDeadOrGhost()) {
        unit->m_clientFlags &= ~0x2;
    } else if (unit->m_bloodlevels) {
        unit->m_clientFlags |= 0x2;
    }

    int32_t oldHealth = *static_cast<const int32_t*>(oldValue);

    if (oldHealth > unit->m_unit->m_unitFieldHealth && unit->m_namePlateFrame) {
        unit->m_namePlateFrame->TriggerDamageFeedback();
    }

    int32_t health = unit->m_unit->m_unitFieldHealth;

    if (health <= 0) {
        if (oldHealth > 0) {
            if (guid == CGUnit_C::s_activeMover) {
                CGInputControl::GetActive()->UpdatePlayer(FrameTime::GetCurTimeMs(), 1);
            }

            unit->OnDeath();
        }
    } else if (oldHealth <= 0) {
        if (guid == CGUnit_C::s_activeMover) {
            CGInputControl::GetActive()->UpdatePlayer(FrameTime::GetCurTimeMs(), 1);
        }

        unit->OnLife(false);
    }

    if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGSpellBook::UpdateUsable();
    }

    return 1;
}

// PROVEN: 7856a082 IDENTICAL/NONE
// OFFSET: 0x005D3350
int32_t StandStateUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (guid != ClntObjMgrGetActivePlayer()) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->UpdateClientStandState(*static_cast<const uint8_t*>(oldValue), unit->m_unit->m_unitFieldBytes1[0]);
        }
    }

    return 1;
}

// PROVEN: dc584554 IDENTICAL/NONE
// OFFSET: 0x005AD3C0
int32_t EmoteStateUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateEmoteAnimKit();
    }

    return 1;
}

// PROVEN: 0248f14a IDENTICAL/NONE
// OFFSET: 0x005D5A90
int32_t ChannelSpellOrObjectChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        const WOWGUID* oldObject = static_cast<const WOWGUID*>(oldValue);
        int32_t oldSpell = *reinterpret_cast<const int32_t*>(oldObject + 1);
        unit->OnChannelObjectChanged(*oldObject, oldSpell);
        unit->OnChannelSpellChanged(oldSpell);
    }

    return 1;
}

// OFFSET: 0x005D8880/0x140253E60
bool CGUnit_C::CheckAndReportSpellInhibitFlags(PendingSpellCast* cast, const SpellRec* spell, const CGItem_C* item, int32_t ignorePower) {
    if (!spell) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return false;
    }

    int32_t reason = 0;
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
    int32_t instanceType;

    if (map) {
        if (!CGUnit_C::CheckSpellPvPUsability(spell, &reason)) {
            Spell_C_SpellFailed(cast, spell, reason, -1, -1, false);
            return false;
        }

        instanceType = map->m_instanceType;
    } else {
        instanceType = 0;
    }

    if (instanceType != 4 && (spell->m_attributesExF & 0x2)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_IN_ARENA, -1, -1, false);
        return false;
    }

    if (item) {
        if (Spell_C_GetItemCooldown(const_cast<CGItem_C*>(item), NULL, NULL, NULL)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ITEM_NOT_READY, -1, -1, false);
            return false;
        }
    } else {
        bool player = this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
        int32_t remaining = 0;

        if (Spell_C_GetSpellCooldown(spell->m_id, !player, NULL, NULL, NULL, &remaining) && (!remaining || remaining >= Spell_C_GetMaxStartRecoveryOffset())) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_READY, -1, -1, false);
            return false;
        }

        int32_t powerType;
        int32_t powerCost;

        if (!ignorePower && !Spell_C_HaveSpellPower(this, spell, powerType, powerCost)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NO_POWER, powerType, powerCost, false);
            return false;
        }
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        if (effect->m_effect == 36) {
            int32_t learn = effect->m_effectTriggerSpell;

            if (!learn) {
                WOWGUID none = WOWGUID();
                ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

                if (stats) {
                    for (uint32_t j = 0; j < 5; j++) {
                        if (stats->m_spellTrigger[j] == 6) {
                            learn = stats->m_spellId[j];
                            break;
                        }
                    }
                }
            }

            if (this->IsSpellKnown(learn) || this->IsSpellSuperceded(learn)) {
                const SpellRec* known = g_spellDB.GetRecord(learn);
                CGGameUI::DisplayError(ERR_SPELL_ALREADY_KNOWN_S, known ? known->m_name : "UNKNOWN");
                return false;
            }
        }

        if (effect->m_effect == 56 || effect->m_effect == 55 || effect->m_effectAura == 6 || effect->m_effectAura == 2) {
            const WOWGUID* controlled = &this->m_unit->m_unitFieldCharm;

            if (!*controlled) {
                controlled = &this->m_unit->m_unitFieldSummon;
            }

            if (*controlled && !(spell->m_attributesEx & 0x1)) {
                if (CGStableInfo::FindPetIndexBySpellID(spell->m_id, i) == -1) {
                    Spell_C_SpellFailed(cast, spell, this->m_unit->m_unitFieldCharm ? SPELL_FAILED_ALREADY_HAVE_CHARM : SPELL_FAILED_ALREADY_HAVE_SUMMON, -1, -1, false);
                } else {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ALREADY_HAVE_PET, -1, -1, false);
                }

                return false;
            }
        }

        if (effect->m_implicitTargetA == 5 || effect->m_implicitTargetB == 5) {
            const WOWGUID* controlled = &this->m_unit->m_unitFieldCharm;

            if (!*controlled) {
                controlled = &this->m_unit->m_unitFieldSummon;
            }

            if (!*controlled) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NO_PET, -1, -1, false);
                return false;
            }

            if (effect->m_effect == 36) {
                CGUnit_C* petUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controlled, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (petUnit) {
                    int32_t learn = effect->m_effectTriggerSpell;
                    int32_t level;

                    if (!petUnit->CanLearnSpell(learn, level, false)) {
                        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_SPELL_UNAVAILABLE, -1, -1, false);
                        return false;
                    }

                    if (petUnit->IsSpellKnown(learn) || petUnit->IsSpellSuperceded(learn)) {
                        CGGameUI::DisplayError(ERR_PET_SPELL_ALREADY_KNOWN_S, spell->m_name);
                        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_DONT_REPORT, -1, -1, false);
                        return false;
                    }
                }
            }
        }

        if ((this->m_obj->m_type & TYPEMASK_PLAYER) && effect->m_effect == 105 && !CGResearchFrame::CanScanResearchSite(effect->m_effectMiscValue)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_HERE, -1, -1, false);
            return false;
        }
    }

    if (this->m_unit->m_unitFieldHealth > 0) {
        if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(this)->m_player->m_playerFieldBytes[0] & 0x4)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_SPELL_IN_PROGRESS, -1, -1, false);
            return false;
        }

        if (this->m_unit->m_unitFieldCharmedby && this->m_unit->m_unitFieldCharmedby != ClntObjMgrGetActivePlayer() && !(spell->m_attributesExH & 0x20) && !Spell_C_CancelsCharm(this, spell, &reason)) {
            Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_CHARMED, reason ? reason : -1, -1, false);
            return false;
        }

        if ((this->m_unit->m_unitFieldFlags & 0x40000) && !(spell->m_attributesExE & 0x8) && !(spell->m_attributesExG & 0x100000) && !Spell_C_CancelsStun(this, spell, &reason)) {
            Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_STUNNED, reason ? reason : -1, -1, false);
            return false;
        }

        if (spell->m_spellCategoriesID) {
            SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

            if (categories) {
                if ((categories->m_preventionType & 0x1) && (this->m_unit->m_unitFieldFlags & 0x2000) && !Spell_C_CancelsSilence(this, spell, &reason)) {
                    Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_SILENCED, reason ? reason : -1, -1, false);
                    return false;
                }

                if ((categories->m_preventionType & 0x2) && (this->m_unit->m_unitFieldFlags & 0x20000) && !Spell_C_CancelsPacify(this, spell, &reason)) {
                    Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_PACIFIED, reason ? reason : -1, -1, false);
                    return false;
                }

                if ((categories->m_preventionType & 0x4) && (this->m_unit->m_unitFieldFlags2 & 0x800000) && !Spell_C_CancelsNoActions(this, spell, &reason)) {
                    Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_NO_ACTIONS, reason ? reason : -1, -1, false);
                    return false;
                }
            }
        }

        if ((this->m_unit->m_unitFieldFlags & 0x800000) && !(spell->m_attributesExE & 0x20000) && !(spell->m_attributesExG & 0x100000) && !Spell_C_CancelsFear(this, spell, &reason)) {
            Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_FLEEING, reason ? reason : -1, -1, false);
            return false;
        }

        if ((this->m_unit->m_unitFieldFlags & 0x400000) && !(spell->m_attributesExE & 0x40000) && !(spell->m_attributesExG & 0x100000) && !Spell_C_CancelsConfuse(this, spell, &reason)) {
            Spell_C_SpellFailed(cast, spell, reason ? SPELL_FAILED_PREVENTED_BY_MECHANIC : SPELL_FAILED_CONFUSED, reason ? reason : -1, -1, false);
            return false;
        }
    }

    uint32_t auraInterrupt = 0;
    uint32_t auraInterrupt2 = 0;
    uint32_t channelInterrupt = 0;
    uint32_t channelInterrupt2 = 0;
    uint32_t interruptFlags = 0;

    if (spell->m_spellInterruptsID) {
        SpellInterruptsRec* interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID);

        if (interrupts) {
            auraInterrupt = interrupts->m_auraInterruptFlags;
            auraInterrupt2 = interrupts->m_auraInterruptFlags2;
            channelInterrupt = interrupts->m_channelInterruptFlags;
            channelInterrupt2 = interrupts->m_channelInterruptFlags2;
            interruptFlags = interrupts->m_interruptFlags;
        }
    }

    int32_t channeled = spell->m_attributesEx & 0x44;

    if (((auraInterrupt & 0x200) && !this->m_sheatheState) || (channeled && (channelInterrupt & 0x200) && !this->m_sheatheState)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_UNSHEATHED, -1, -1, false);
        return false;
    }

    if (((auraInterrupt & 0x40) && (this->m_mountDisplayId <= 0 || (this->m_clientFlags & 0x10000000))) || (channeled && (channelInterrupt & 0x40) && !this->IsMounted())) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_MOUNTED, -1, -1, false);
        return false;
    }

    if (!(spell->m_attributes & 0x1000000) && this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && !(this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x800)) && !this->CanAutoDismount()) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_MOUNTED, -1, -1, false);
        return false;
    }

    if (!(spell->m_attributes & 0x8000000) && this->GetClientStandState() && !this->CanAutoStand()) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_STANDING, -1, -1, false);
        return false;
    }

    if ((spell->m_attributes & 0x1000) && !g_clientGameTime.IsDaytime()) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_DAYTIME, -1, -1, false);
        return false;
    }

    if ((spell->m_attributes & 0x2000) && g_clientGameTime.IsDaytime()) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_NIGHTTIME, -1, -1, false);
        return false;
    }

    if (((auraInterrupt & 0x100) && !(this->m_clientFlags & 0x200000)) || (channeled && (channelInterrupt & 0x100) && !(this->m_clientFlags & 0x200000))) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_UNDERWATER, -1, -1, false);
        return false;
    }

    if (((auraInterrupt & 0x80) && (this->m_clientFlags & 0x200000)) || (channeled && (channelInterrupt & 0x80) && (this->m_clientFlags & 0x200000))) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_ABOVEWATER, -1, -1, false);
        return false;
    }

    if (((auraInterrupt2 & 0x2) && (this->m_movement->m_flags & 0x100000)) || (channeled && (channelInterrupt2 & 0x2) && (this->m_movement->m_flags & 0x100000))) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_NOT_SWIMMING, -1, -1, false);
        return false;
    }

    int32_t targets = 0;

    if (spell->m_spellTargetRestrictionsID) {
        SpellTargetRestrictionsRec* restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID);

        if (restrictions) {
            targets = restrictions->m_targets;
        }
    }

    if (channeled && (this->m_movement->m_flags & 0x60080F) && !(targets & 0x60)) {
        if ((channelInterrupt & 0x18) && !this->HasAuraMatchingSpellClass(330, spell)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_MOVING, -1, -1, false);
            return false;
        }

        if ((this->m_movement->m_flags & 0x800) && (channelInterrupt2 & 0x1)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_FALLING, -1, -1, false);
            return false;
        }
    }

    if ((spell->m_attributesEx & 0x44) && !(this->m_movement->m_flags & 0x60080F) && !(targets & 0x60) && (channelInterrupt2 & 0x4)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_BY_NOT_MOVING, -1, -1, false);
        return false;
    }

    if ((interruptFlags & 0x1) && (this->m_movement->m_flags & 0x60080F) && !(targets & 0x60) && !(spell->m_attributesExB & 0x20) && !this->HasAuraMatchingSpellClass(330, spell)) {
        if (Spell_C_GetCastTime(spell, this->m_unit->m_unitFieldPetnumber != 0, 0, 0) || (auraInterrupt & 0x18)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_MOVING, -1, -1, false);
            return false;
        }

        if ((this->m_movement->m_flags & 0x800) && (auraInterrupt2 & 0x1)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_FALLING, -1, -1, false);
            return false;
        }
    }

    if (!this->CheckShapeshiftRules(spell, &reason) && (reason == SPELL_FAILED_ONLY_SHAPESHIFT || !this->CanAutoUnshift())) {
        Spell_C_SpellFailed(cast, spell, reason, -1, -1, false);
        return false;
    }

    if (!this->CheckStealthRules(spell, &reason) || !this->CheckMountRules(spell, reinterpret_cast<SpellCastResult*>(&reason))) {
        Spell_C_SpellFailed(cast, spell, reason, -1, -1, false);
        return false;
    }

    if ((spell->m_attributes & 0x10000000) && (this->m_unit->m_unitFieldFlags & 0x80000) && !this->HasIgnoreAuraState(spell)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_AFFECTING_COMBAT, -1, -1, false);
        return false;
    }

    if (spell->m_spellAuraRestrictionsID) {
        SpellAuraRestrictionsRec* restrictions = g_spellAuraRestrictionsDB.GetRecord(spell->m_spellAuraRestrictionsID);

        if (restrictions) {
            int32_t casterAura;
            int32_t excludeCasterAura;

            if ((restrictions->m_casterAuraState && !this->HasAuraState(restrictions->m_casterAuraState) && !this->HasIgnoreAuraState(spell))
                || (restrictions->m_excludeCasterAuraState && this->HasAuraState(restrictions->m_excludeCasterAuraState) && !this->HasIgnoreAuraState(spell))
                || ((casterAura = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_casterAuraSpell)) && !this->HasAura(casterAura, WOWGUID()) && !this->HasIgnoreAuraState(spell))
                || ((excludeCasterAura = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_excludeCasterAuraSpell)) && this->HasAura(excludeCasterAura, WOWGUID()) && !this->HasIgnoreAuraState(spell))) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CASTER_AURASTATE, -1, -1, false);
                return false;
            }
        }
    }

    if (spell->m_spellCastingRequirementsID) {
        SpellCastingRequirementsRec* requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID);

        if (requirements) {
            if (requirements->m_minFactionID && GetRepListRepValue(requirements->m_minFactionID) < CGReputationInfo::m_reactionThresholds[requirements->m_minReputation]) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_REPUTATION, -1, -1, false);
                return false;
            }

            if (requirements->m_requiredAreasID > 0) {
                AreaTableRec* area = CGGameUI::GetCurrentSubzoneRec();

                if (!area) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_INCORRECT_AREA, -1, -1, false);
                    return false;
                }

                int32_t group = requirements->m_requiredAreasID;
                int32_t checked;

                if (!CGUnit_C::CheckIsInArea(area->m_id, group, &checked)) {
                    if (checked == 1) {
                        AreaGroupRec* groupRec = g_areaGroupDB.GetRecord(group);

                        if (groupRec) {
                            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_REQUIRES_AREA, groupRec->m_areaID[0], -1, false);
                            return false;
                        }
                    }

                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_INCORRECT_AREA, -1, -1, false);
                    return false;
                }
            }
        }
    }

    if (spell->m_attributes & 0xC000) {
        if (this->IsOutdoors()) {
            if (spell->m_attributes & 0x4000) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_INDOORS, -1, -1, false);
                return false;
            }
        } else if (spell->m_attributes & 0x8000) {
            Spell_C_SpellFailed(cast, spell, SpellHasAuraEffect(spell, 78) ? SPELL_FAILED_NO_MOUNTS_ALLOWED : SPELL_FAILED_ONLY_OUTDOORS, -1, -1, false);
            return false;
        }
    }

    if ((spell->m_attributesExD & 0x4000000) && !this->FlyingMountAllowed(!(spell->m_attributesExG & 0x400000))) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_HERE, -1, -1, false);
        return false;
    }

    AreaTableRec* areaRec = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (areaRec && areaRec->m_worldPvpID) {
        World_PVP_AreaRec* pvpArea = g_world_PVP_AreaDB.GetRecord(areaRec->m_worldPvpID);

        if (pvpArea && GetWorldState(pvpArea->m_gameTimeWorldState) > 0 && (spell->m_attributesExH & 0x10000000)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_IN_BATTLEGROUND, -1, -1, false);
            return false;
        }
    }

    if (instanceType == 3) {
        if (spell->m_attributesExH & 0x10000000) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_IN_BATTLEGROUND, -1, -1, false);
            return false;
        }
    } else if (spell->m_attributesExC & 0x800) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ONLY_BATTLEGROUNDS, -1, -1, false);
        return false;
    }

    if ((spell->m_attributesExF & 0x800) && instanceType == 2) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_IN_RAID_INSTANCE, -1, -1, false);
        return false;
    }

    if ((spell->m_attributesExD & 0x40000000) && map && !map->m_instanceType) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_HERE, -1, -1, false);
        return false;
    }

    if ((spell->m_attributesExB & 0x10000) && (this->m_obj->m_type & TYPEMASK_PLAYER) && (this->GetActivePlayerFieldBytes0() & 0x1)) {
        Spell_C_PetTameFailure(CGStableInfo::GetNumActivePets() < 5 ? 5 : 2);
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_DONT_REPORT, -1, -1, false);
        return false;
    }

    uint8_t used;
    uint8_t maximum;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && (Spell_C_GetSpellCategoryWeeklyUses(spell, used, maximum), maximum) && used >= maximum) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_EXCEEDED_WEEKLY_USAGE, maximum, -1, false);
        return false;
    }

    return true;
}

// PROVEN: 3bb402d6 IDENTICAL/NONE
// OFFSET: 0x005D9D70
int32_t MountDisplayIDUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnMountDisplayChanged(unit->m_unit->m_unitFieldMountdisplayid);
    }

    return 1;
}

// PROVEN: 754310a8 IDENTICAL/NONE
// OFFSET: 0x005CF5D0
int32_t DynamicFlagsChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnDynamicFlagsChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x005ABB30/0x14021E050
void CGUnit_C::DestroyNamePlate() {
    if (!this->m_namePlateFrame) {
        return;
    }

    if (CGNamePlateFrame::GetNamePlateFocus() == this->m_namePlateFrame) {
        CGUnit_C::ClearNamePlateFocus();
    }

    CGNamePlateFrame::ReleaseNameplateFrame(this->m_namePlateFrame);
    this->m_namePlateFrame->Hide();
    this->m_namePlateFrame = NULL;
    this->ObjectNameVisibilityChanged();
}

// PROVEN: a53b6a17 IDENTICAL/NONE
// OFFSET: 0x005CDC20
void CGUnit_C::PostAnimate(CGWorldFrame* worldFrame) {
    C3Vector position(0.0f, 0.0f, 0.0f);

    if (this->ShouldShowNamePlate(worldFrame, &position)) {
        this->NamePlateAddOrUpdate(position, worldFrame);
    } else {
        this->DestroyNamePlate();
    }
}

// PROVEN: 64bc0121 IDENTICAL/NONE
// OFFSET: 0x005B4DF0/0x140228520
static float GetExtraDisplayScale(const CreatureDisplayInfoRec* displayInfo) {
    CreatureDisplayInfoExtraRec* extra = g_creatureDisplayInfoExtraDB.GetRecord(displayInfo->m_extendedDisplayInfoID);

    if (!extra) {
        return 1.0f;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(extra->m_displayRaceID);

    if (!race) {
        return 1.0f;
    }

    int32_t displayId = 0;

    switch (extra->m_displaySexID) {
        case 0: {
            displayId = race->m_maleDisplayID;
            break;
        }

        case 1: {
            displayId = race->m_femaleDisplayID;
            break;
        }
    }

    CreatureDisplayInfoRec* rec = g_creatureDisplayInfoDB.GetRecord(displayId);

    return rec ? rec->m_creatureModelScale : 1.0f;
}

// OFFSET: 0x005B4ED0/0x140228600
float CGUnit_C::GetModelScale(const CreatureDisplayInfoRec* displayInfo, const CreatureModelDataRec* modelData) {
    CreatureFamilyRec* family = g_creatureFamilyDB.GetRecord(this->m_creatureCacheEntry ? this->m_creatureCacheEntry->m_family : 0);
    float scale = GetExtraDisplayScale(displayInfo) * displayInfo->m_creatureModelScale * modelData->m_modelScale;
    float fraction = 0.0f;

    if (scale <= 0.0f) {
        scale = 1.0f;
    }

    if (family) {
        int32_t level = this->m_unit->m_unitFieldLevel;
        int32_t delta = level < family->m_minScaleLevel ? 0 : level - family->m_minScaleLevel;
        int32_t range = family->m_maxScaleLevel - family->m_minScaleLevel;

        if (delta > range) {
            delta = range;
        }

        if (range) {
            fraction = static_cast<float>(delta) / range;
        }

        float familyScale = fraction * (family->m_maxScale - family->m_minScale) + family->m_minScale;

        if (familyScale > scale || this->m_unit->m_unitFieldPetnumber) {
            return familyScale;
        }
    }

    return scale;
}

// OFFSET: 0x005C3000/0x140239400
void CGUnit_C::UpdateWorldObject(int32_t a2) {
    if (this->m_worldObject) {
        C44Matrix matrix;
        CM2Model* model = this->GetObjectModel();
        CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

        if (passenger && passenger->m_state && (passenger->m_flags & 0x10) && model && (model->m_fFlags & 0x4000)) {
            matrix = model->GetAnimatedWorldTransform();
        } else {
            matrix = this->GetMatrix();
            matrix.Scale(this->GetTrueScale());
        }

        float height = this->m_movementData.m_collisionHeight * 0.5f;
        CAaBox box(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
        CAaSphere sphere;
        sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
        sphere.r = 0.0f;
        C3Vector vec(0.0f, 0.0f, height / this->GetTrueScale());

        if (model && model->IsLoaded()) {
            if (model->IsLoadedRecursive()) {
                box = model->GetMaxBoundingBoxPlusAttachments();
                sphere = model->GetMaxBoundingSpherePlusAttachments();
            } else {
                box = model->GetBoundingBox();
                sphere = model->GetBoundingSphere();
            }

            CVehicle_C* vehicle = this->m_vehicle;

            if (vehicle && vehicle->m_vehicleRec) {
                float radius = vehicle->m_largestPassengerBoundsRadius * 2.0f;

                if (box.t.x <= box.b.x || box.t.y <= box.b.y || box.t.z <= box.b.z) {
                    box.Set(-radius, radius);
                } else {
                    box.b.x = box.b.x - radius;
                    box.b.y = box.b.y - radius;
                    box.b.z = box.b.z - radius;
                    box.t.x = box.t.x + radius;
                    box.t.y = box.t.y + radius;
                    box.t.z = box.t.z + radius;
                }

                sphere.r = sphere.r + radius;
            }
        }

        World::ObjectUpdate(this->m_worldObject, matrix, box, sphere, vec, a2, this->m_torsoKeyBone);

        if (this->m_displayTransition) {
            this->m_displayTransition->MirrorTransformToSwapWorldObject(matrix, box, sphere, vec, a2, this->m_torsoKeyBone);
        }
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec) {
        for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
            unit->UpdateWorldObject(0);
        }
    }
}

// PROVEN: ab491225 IDENTICAL/NONE
// OFFSET: 0x005B8990/0x14022C7A0
int32_t CGUnit_C::AppearsDead(int32_t a2) {
    if (this->m_unit->m_unitFieldHealth <= 0 || (this->m_unit->m_unitFieldFlags2 & 0x1) || this->GetClientStandState() == 7) {
        return 1;
    }

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        const SpellVisualKitRec* kit = effect->m_kitRec;

        if (kit && (effect->m_flags & 0x1000) && !(effect->m_flags & 0x100) && (a2 || !(kit->m_flags & 0x4)) && AnimationDataHasFlags(kit->m_animID, 0x100000)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005BFFE0/0x140235A10
int32_t CGUnit_C::DisplayInfoNeedsUpdate(int32_t* texturesChanged) {
    *texturesChanged = 0;
    CGUnitData* unit = this->m_unit;
    uint32_t displayId = unit->m_unitFieldNativedisplayid == unit->m_unitFieldDisplayid ? this->GetCurrentClientNativeDisplayID() : unit->m_unitFieldDisplayid;
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!displayInfo) {
        SysMsgPrintf(STATUS_ERROR, 2, "NOUNITDISPLAYID|%d|%s", displayId, this->GetUnitName(NULL, 1));
        return 0;
    }

    CreatureDisplayInfoRec* current = this->m_displayInfo;

    if (displayInfo != current) {
        if (displayInfo->m_extendedDisplayInfoID != current->m_extendedDisplayInfoID) {
            return 1;
        }

        if (g_creatureModelDataDB.GetRecord(displayInfo->m_modelID) != this->m_modelData || this->GetSoundDataForID(displayInfo->m_soundID) != this->m_soundData) {
            return 1;
        }

        for (int32_t i = 0; i < 3; i++) {
            if (strcmp(displayInfo->m_textureVariation[i], this->m_displayInfo->m_textureVariation[i])) {
                *texturesChanged = 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x005B22D0/0x140225710
CreatureSoundDataRec* CGUnit_C::GetSoundDataForID(int32_t soundId) const {
    CreatureSoundDataRec* soundData = g_creatureSoundDataDB.GetRecord(soundId);

    if (soundData && this->m_unit->m_unitFieldPetnumber && soundData->m_creatureSoundDataIDPet) {
        CreatureSoundDataRec* pet = g_creatureSoundDataDB.GetRecord(soundData->m_creatureSoundDataIDPet);

        if (pet) {
            return pet;
        }
    }

    return soundData;
}

// OFFSET: 0x005D9BD0/0x1402555C0
void CGUnit_C::UpdateDisplayInfo(int32_t force) {
    CM2Model* pending = this->m_pendingDisplayModel;
    this->m_clientFlags &= 0xF7FDFFFF;

    if (pending) {
        pending->SetLoadedCallback(NULL, NULL);
        this->m_pendingDisplayModel->Release();
        this->m_pendingDisplayModel = NULL;
    }

    int32_t texturesChanged = 0;

    if (force || this->DisplayInfoNeedsUpdate(&texturesChanged)) {
        if (!this->m_displayTransition || this->m_unit->m_unitFieldNativedisplayid != this->m_unit->m_unitFieldDisplayid) {
            const char* fileName;
            this->GetModelFileName(&fileName);
            CM2Model* model = World::GetScene()->CreateModel(fileName, 32, 3);

            if (model) {
                if (model->IsLoaded()) {
                    this->OnDisplayModelLoaded(model);
                } else {
                    this->m_pendingDisplayModel = model;
                    model->SetLoadedCallback(CGUnit_C::DisplayModelLoadedCallback, this);
                }
            }
        }
    } else if (texturesChanged) {
        this->RefreshDataPointers();
        CCharacterComponent::ReplaceMonsterSkin(this->m_worldModel, this->m_displayInfo, this->m_modelData);
    }
}

// OFFSET: 0x005D6D00/0x140251A90
void CGUnit_C::OnDisplayModelLoaded(CM2Model* model) {
    if (!model) {
        return;
    }

    if (this->m_displayTransition) {
        if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
            model->Release();
            return;
        }

        if (!this->m_displayTransition->UpdateState(1)) {
            this->StopDisplayTransition();
        }

        if (this->m_displayTransition) {
            this->m_displayTransition->Expedite(0);
            this->StopDisplayTransition();
        }
    }

    this->m_wheelMask = 0;
    this->KillCreatureLoopSound();
    CreatureModelDataRec* oldModelData = this->m_modelData;
    this->RefreshDataPointers();
    PLAYERNAMEDESC* nameDesc = this->m_nameDesc;
    this->m_animationState &= 0xFFFFFFF9;
    this->m_torsoKeyBone = -1;

    if (nameDesc) {
        PlayerNameDelete(nameDesc);
    }

    this->m_nameDesc = NULL;
    model->SetSequenceCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEndCallback), this->m_obj->m_guid.m_guid);
    model->SetSequenceStartCallback(&CGUnit_C::SequenceStartCallback, this->m_obj->m_guid.m_guid);
    model->SetEventCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEventCallback), this->m_obj->m_guid.m_guid);
    this->SetCharacterModel(model);
    model->Release();
    model->m_fFlags ^= (model->m_fFlags ^ (this->m_modelData->m_flags >> 7)) & 0x4;
    this->UpdateModelScale(oldModelData);

    if (this->m_mountModel) {
        float scale = 1.0f / this->m_mountScale;
        model->m_fFlags |= 0x4000;
        C44Matrix matrix;
        matrix.a0 = scale;
        matrix.b1 = scale;
        matrix.c2 = scale;
        model->m_worldTransform = matrix;
    }

    this->Animate(0.0f);
    model->ForceAnimate();
    this->RefreshNameDesc();
    CCharacterComponent::ApplyMonsterGeosets(this->m_worldModel, this->m_displayInfo);
    CCharacterComponent::ReplaceMonsterSkin(this->m_worldModel, this->m_displayInfo, this->m_modelData);
    this->SetupFootprints();
    this->CheckLoopSound();

    if (this->m_obj->m_guid == CGWorldFrame::GetActiveCamera()->m_targetGUID) {
        CGGameUI::ResetCamera(this->m_obj->m_guid);
    }

    this->RefreshInteractIcon();

    if (!(this->m_unit->m_unitFieldFlags2 & 0x10) && !(this->m_clientFlags & 0x8020000)) {
        this->FreeComponent();
        this->m_clientFlags &= ~0x20000;
    }

    this->m_clientFlags |= 0x400000;
    this->UpdateAlphaEffectAlpha();
    this->ScanAuraSpellsOnDisplayUpdate();
    this->UpdateChannelEffects();
    float facing = this->m_renderFacing;
    this->unk_09FC &= 0xFFFFFFF8;
    this->unk_08D4 = 0.0f;
    facing = this->m_movementData.GetFacing(facing);
    this->m_smoothFacing = facing;
    this->m_smoothFacingTarget = facing;
    this->unk_08D8 = 0.0f;
    this->m_specialIdleAnim = -1;

    if ((this->m_modelData->m_flags & 0x40) || (this->m_unit->m_unitFieldFlags2 & 0x80000)) {
        this->m_clientFlags |= 0x2000000;
    } else {
        this->m_clientFlags &= ~0x2000000;
    }

    if (this->m_mountModel) {
        model->AttachToParent(this->m_mountModel, 0, NULL, 0);

        if (this->GetObjectModel()->IsLoaded()) {
            this->InitWheels();
        }
    }

    this->CreateOrDestroyObjectEffectManager();
    this->UpdateCharacterAnimReplacementSets();
    this->UpdateEmoteAnimKit();
}

// OFFSET: 0x005B55E0/0x140228E00
void CGUnit_C::OnDisplayTransitionBeginAnimation() {
    this->UpdateWorldObject(0);
    CM2Model* model = this->m_worldModel;
    CM2Scene* scene = model->m_scene;
    CM2Model* transitionModel = this->m_displayTransition ? this->m_displayTransition->m_model : NULL;

    if (scene && transitionModel && (this->m_currentClientRaceFlags & 0x4) && !(this->m_currentClientRaceFlags & 0x1)) {
        uint32_t ids[2] = { 5, 6 };
        uint32_t attachments[2] = { 51, 52 };

        for (uint32_t i = 0; i < 2; i++) {
            uint32_t attachment = attachments[i];

            if (!transitionModel->HasAttachment(attachment)) {
                continue;
            }

            for (CM2Model* child = model->m_attachList; child; child = child->m_attachNext) {
                if (child->m_attachmentId == ids[i]) {
                    scene->DuplicateModel(child, 0)->AttachToParent(transitionModel, attachment, NULL, 0);
                }
            }
        }
    }

    if (!this->m_soundData) {
        return;
    }

    int32_t soundKitID = (this->m_currentClientRaceFlags & 0x4) ? this->m_soundData->unk_009C : this->m_soundData->unk_0098;

    if (!soundKitID) {
        return;
    }

    C3Vector position = this->GetPosition();
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit(soundKitID, &position, &properties, &this->m_displayTransitionSound, __FILE__, __LINE__, "CGUnit_C::OnDisplayTransitionBeginAnimation");
}

// OFFSET: 0x005B5580/0x140228D80
void CGUnit_C::StopDisplayTransition() {
    CUnitDisplayTransition_C* transition = this->m_displayTransition;

    if (!transition) {
        return;
    }

    if ((this->m_currentClientRaceFlags & 0x4) && transition->m_state != 5) {
        SE3::StopSound(this->m_displayTransitionSound, 500, __FILE__, __LINE__, "CGUnit_C::StopDisplayTransition");
    }

    transition->Cancel();
    this->m_currentClientRaceFlags &= 0xFFFFFFF9;
    this->m_displayTransition = NULL;
}

// OFFSET: 0x005BC330
void CGUnit_C::ExpediteDisplayTransition(int32_t a2) {
    if (!this->m_displayTransition) {
        return;
    }

    if (!this->m_displayTransition->UpdateState(1)) {
        this->StopDisplayTransition();
    }

    if (this->m_displayTransition) {
        this->m_displayTransition->Expedite(a2);
        this->StopDisplayTransition();
    }
}

// OFFSET: 0x005B9510/0x14022D390
int32_t CGUnit_C::Animate(float elapsed) {
    C44Matrix matrix;

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->UpdateModel();
    } else {
        CMovement_C* movement = this->m_movement;

        if ((movement->m_flags & 0x1100000) && ((movement->m_flags & 0x3) || (this->unk_09FC & 0x7))) {
            if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
                this->ComputeSmoothedSwimMatrix(&matrix, elapsed, movement->m_flags & 0x1000000);

                if (this->m_movement->m_flags & 0x3) {
                    this->unk_09FC |= 0x1;
                }
            } else {
                this->GetSwimMatrix(matrix);
            }

            CM2Model* model = this->GetObjectModel();
            model->m_fFlags |= 0x4000;
            model->m_worldTransform = matrix;
        } else {
            this->ResetSmoothFacing();

            if (this->m_mountTransition && this->m_mountTransitionEffect) {
                this->MountTransitionSetCharacterTransform(elapsed);
            } else {
                this->UpdateWorldTransform(elapsed);
            }
        }
    }

    this->UpdateMountTransitionModel();

    if (this->m_displayTransition) {
        this->m_displayTransition->MirrorTransformToSwapModel();
    }

    return 1;
}

// OFFSET: 0x005D10A0
void CGUnit_C::ShouldRender(uint32_t flags, int32_t& culled, int32_t& hidden) {
    this->CGObject_C::ShouldRender(flags, culled, hidden);

    if ((this->m_clientFlags & 0x400000) && !this->InitializeComponent()) {
        hidden = 1;
    } else if (culled || hidden) {
        if (this->m_characterComponent) {
            this->m_characterComponent->Prep();
        }

        if (this->m_displayTransition) {
            this->m_displayTransition->HandleComponentPrep();
        }

        return;
    }

    uint32_t hideFlag = 0;
    uint32_t parentHidden = 0;

    if (this->m_displayTransition) {
        this->m_displayTransition->HandleComponentRenderPrep();
    }

    if (this->m_characterComponent && !this->m_characterComponent->RenderPrep(0)) {
        hidden = 1;

        if (!(this->m_characterComponent->m_data.m_flags & 0x1)) {
            C3Vector position = this->GetPosition();
            const C3Vector& guard = World::GetGuardBandPosition();
            float dy = guard.y - position.y;
            float dz = guard.z - position.z;
            CCharacterComponent::SetClosestComponent(this->m_characterComponent, culled == 0, dy * dy + dz * dz + (guard.x - position.x) * (guard.x - position.x));
        }
    } else {
        CM2Model* model = this->m_worldModel;
        bool childHidden = false;

        if (model && (model->m_fFlags & 0x2000)) {
            CM2Model* child;

            for (child = model->m_attachList; child; child = child->m_attachNext) {
                if ((child->m_attachParent ? child->m_fFlags >> 6 : child->m_fFlags >> 3) & 0x1) {
                    break;
                }
            }

            if (!child) {
                hidden = 1;
                childHidden = true;
            }
        }

        if (!childHidden) {
            CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

            if (passenger && passenger->m_state && (passenger->m_flags & 0x400)) {
                parentHidden = 1;
                hidden = 1;
            }
        }
    }

    if (this->m_unit->m_unitFieldFlags2 & 0x2) {
        hideFlag = 1;
    }

    if ((this->m_mountModel || this->m_vehiclePassenger) && hidden) {
        parentHidden = 1;
        hidden = 0;
    }

    if (this->m_worldModel) {
        this->m_worldModel->m_fFlags ^= (this->m_worldModel->m_fFlags ^ (hideFlag << 13)) & 0x2000;
        this->m_worldModel->m_fFlags ^= (this->m_worldModel->m_fFlags ^ (parentHidden << 23)) & 0x800000;
    }
}

// OFFSET: 0x005D70B0/0x140251ED0
void CGUnit_C::ModelLoaded(CM2Model* model) {
    if (model == this->m_pendingDisplayModel) {
        this->m_pendingDisplayModel = NULL;
        this->OnDisplayModelLoaded(model);
    }

    this->CGObject_C::ModelLoaded(model);

    if (this->GetTransportGUID()) {
        C44Matrix matrix;
        MovementGetTransportMtxX(WOWGUID::Make(this->GetTransportGUID()), &matrix, this->m_obj->m_guid, __FILE__, __LINE__);

        if (model->m_fFlags & 0x1) {
            model->m_frameOfReferenceMatrix = matrix;
            model->ChangeFrameOfReference(&model->m_frameOfReferenceMatrix);
        }
    }

    if (model == this->m_worldModel) {
        this->m_animKitManager.SetModel(model, 0);
        this->UpdateObjectEffectAnimationStates();
        this->m_animationState &= 0xFFFFFFF9;

        if (model->HasBone(4)) {
            this->m_animationState |= 0x2;
        }

        if (model->HasBone(6)) {
            this->m_animationState |= 0x4;
        }

        if (this->m_animationState & 0x2) {
            this->m_torsoKeyBone = 4;
        } else {
            this->m_torsoKeyBone = (this->m_animationState & 0x4) ? 6 : -1;
        }

        this->UpdateInteractIconAttach();
    } else if (model == this->m_mountModel && !this->m_mountModel->HasAttachment(0)) {
        SysMsgPrintf(STATUS_ERROR, 0x10, "MOUNTDISPLAYIDNOMOUNTATTACHMENT|%d", this->m_mountDisplayId);
    }

    if (this->m_modelFlags & 0x40000) {
        AnimationDataRec* animRec = g_animationDataDB.GetRecord(this->GetCurrentTorsoAnimId());

        if (animRec && animRec->m_behaviorID == 127) {
            this->m_fadeDuration = 0;
            this->m_alpha = this->m_fadeEndAlpha;
        }
    }

    if (model == this->m_worldModel) {
        this->UpdateDeathAnimKit();
        this->m_animKitManager.OnModelLoaded();
    }

    if (model == this->m_mountModel && this->m_mountAnimKitManager) {
        this->m_mountAnimKitManager->OnModelLoaded();
    }

    if (model == this->GetObjectModel()) {
        this->InitWheels();
    }

    uint32_t emoteState = this->m_unit->m_unitNpcEmotestate;

    if (emoteState && model == this->m_worldModel) {
        EmotesRec* emote = g_emotesDB.GetRecord(emoteState);

        if (emote && (emote->m_emoteFlags & 0x80000)) {
            this->UpdateEmoteAnimKit();
        }
    }

    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state == 3) {
        CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

        if (vehicle) {
            if (vehicle->m_vehicle && vehicle->m_vehicle->m_vehicleRec) {
                vehicle->m_vehicle->UpdateLargestPassengerBoundsRadius();
            }

            vehicle->UpdateWorldObject(0);
        }
    }

    int32_t spellId = this->m_pendingStateKitSpell;

    if (spellId) {
        if (this->HasAura(spellId, WOWGUID())) {
            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (spell) {
                const SpellVisualRec* visual = GetSpellVisual(spell, this);

                if (visual) {
                    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);

                    if (CanPlaySpellVisualKit(0, this->m_obj->m_guid.m_guid, kit, visual)) {
                        PlaySpellVisuaKitData data(spell, kit, 2);
                        this->PlaySpellVisualKit(data);
                    }
                }
            }
        }

        this->m_pendingStateKitSpell = 0;
    }

    if (model == this->GetObjectModel()) {
        if (this->m_vehicle && this->m_vehicle->m_vehicleRec && this->m_vehicle->IsMirroringAnimsToPassengers()) {
            this->m_vehicle->StartMirroringAnimsToPassengers();
        }

        if (this->m_vehiclePassenger && this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
            this->m_vehiclePassenger->StartMirroringVehicleAnims(this->m_vehiclePassenger->MirrorsVehicleAnimTier());
        }
    }
}

// OFFSET: 0x005C6CC0/0x14023DF30
void CGUnit_C::PreAnimate(CGWorldFrame* frame) {
    float elapsed = frame->m_elapsedSec;

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->CheckForVehicleTransitionAnimTimeout();
    }

    uint32_t time = CGWorldFrame::s_currentWorldFrame->GetSceneTime();

    if (static_cast<int32_t>(time - this->m_nextBreathCheckTime) >= 0) {
        this->UpdateBreathState(time);
    }

    this->UpdateModelTint(time);

    if (this->m_proceduralFadeStart && static_cast<int32_t>(time - this->m_proceduralFadeDuration - this->m_proceduralFadeStart) >= 0) {
        this->DoFade(this->GetBaseAlpha(), this->m_proceduralFadeEndAlpha);
        this->m_proceduralFadeStart = 0;
    }

    this->CGObject_C::PreAnimate(frame);

    while (this->m_pendingMissiles && static_cast<int32_t>(time - this->m_pendingMissiles->m_destroyTime) >= 0) {
        this->m_pendingMissiles->DeleteSelf();
    }

    for (CMissile* missile = this->m_pendingMissiles; missile; missile = missile->m_next) {
        float t = static_cast<float>(missile->m_destroyTime - time) * 0.0005f;
        float alpha;

        if (t < 0.0f) {
            alpha = 0.0f;
        } else if (t <= 1.0f) {
            alpha = t * t * t;
        } else {
            alpha = 1.0f;
        }

        if (missile->m_model) {
            missile->m_model->m_fadeAlpha = alpha;
        }
    }

    if (this->m_animationState & 0x80000) {
        this->m_animationState &= ~0x80000;
        this->UpdateDeathAnimKit();
    }

    this->ProcessEmoteQueue();
    this->CreateRipple(0);

    const VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;
    uint32_t pitchBone;
    int32_t fixedFacing;

    if (vehicleRec) {
        pitchBone = vehicleRec->m_flags & 0x200;
        fixedFacing = (pitchBone || (vehicleRec->m_flags & 0x1000)) ? 1 : 0;
    } else {
        pitchBone = 0;
        fixedFacing = 0;
    }

    uint32_t moveFlags;

    if (this->m_unit->m_unitFieldHealth <= 0 || (this->m_vehiclePassenger && (this->m_vehiclePassenger->m_state == 2 || this->m_vehiclePassenger->m_state == 5)) || ((moveFlags = this->m_movement->m_flags) & 0x1100000) || fixedFacing || !(this->m_animationState & 0x6)) {
        this->m_facingVelocity = 0.0f;
        this->m_facingBlend = 0.0f;
        this->m_renderFacing = this->m_smoothTarget.m_value;
    } else if (moveFlags & 0xC) {
        float offset = (this->m_movementData.m_flags & 0x3) ? 0.7853982f : 1.5707964f;
        uint32_t direction = this->m_movementData.m_flags & 0x6;

        if (direction == 6 || !direction) {
            offset = -offset;
        }

        this->m_facingBlend = 1.0f;
        this->SmoothFacingAngle(CMath::normalizeAngleNegPiToPi_(CMath::normalizeAngleNegPiToPi_(this->m_smoothTarget.m_value - this->m_renderFacing + offset) + this->m_renderFacing));
    } else if (moveFlags & 0x803) {
        if (this->m_facingBlend > 0.0f) {
            this->m_facingBlend = this->m_facingBlend - CGWorldFrame::s_currentWorldFrame->m_elapsedSec * 2.5f;

            if (this->m_facingBlend > 1.0f) {
                this->m_facingBlend = 1.0f;
            }

            this->SmoothFacingAngle(this->m_smoothTarget.m_value);
        } else {
            this->m_facingVelocity = 0.0f;
            this->m_renderFacing = this->m_smoothTarget.m_value;
        }
    } else if (this->m_facingBlend < 1.0f) {
        this->m_facingBlend = CGWorldFrame::s_currentWorldFrame->m_elapsedSec * 2.5f + this->m_facingBlend;
    }

    float delta = CMath::normalizeAngleNegPiToPi_(this->m_smoothTarget.m_value - this->m_renderFacing);
    float absDelta = fabsf(delta);
    float turn = 0.0f;
    this->UpdateActiveHeadLook();
    ActiveHeadLook* headLook = this->m_activeHeadLook;
    float headOffset = 0.0f;
    float spineOffset = 0.0f;
    int32_t hasHeadLook;

    if (headLook) {
        headOffset = headLook->m_head;
        spineOffset = headLook->m_spine;
        hasHeadLook = 1;
    } else {
        hasHeadLook = 0;
    }

    if (absDelta < 0.001f && !hasHeadLook) {
        this->m_worldModel->SetBoneFlags(4, 0, 0x80);
        this->m_worldModel->SetBoneFlags(6, 0, 0x80);
    } else {
        if (absDelta > 1.5707964f) {
            turn = absDelta - 1.5707964f;
            turn = delta < 0.0f ? -fabsf(turn) : fabsf(turn);
        }

        if (!(this->m_movement->m_flags & 0xC) && !(this->m_animationState & 0x1)) {
            float step = this->m_movementData.m_turnRate * elapsed;

            if (step > absDelta) {
                step = absDelta;
            }

            turn = (delta < 0.0f ? -fabsf(step) : fabsf(step)) + turn;
        }

        this->m_renderFacing = CMath::normalizeAngleNegPiToPi_(this->m_renderFacing + turn);
        float remaining = fabsf(CMath::normalizeAngleNegPiToPi_(this->m_smoothTarget.m_value - this->m_renderFacing));
        M2BoneSequenceInfo info;
        AnimationDataRec* animRec;

        if ((remaining < s_twistEpsilon && !hasHeadLook) || (this->GetObjectModel()->IsLoaded() && this->GetObjectModel()->GetBoneSequenceInfo(0xFFFFFFFF, info) && (animRec = g_animationDataDB.GetRecord(info.m_sequenceId)) && (animRec->m_flags & 0x100))) {
            this->m_worldModel->SetBoneFlags(4, 0, 0x80);
            this->m_worldModel->SetBoneFlags(6, 0, 0x80);
        } else {
            if (!this->m_mountModel && (this->m_animationState & 0x2)) {
                float twist = remaining;

                if (this->m_obj->m_guid != CGUnit_C::s_activeMover || CGUnit_C::s_trackingType == 13) {
                    twist = twist * 0.5f;
                }

                if (twist > 0.7853982f) {
                    twist = 0.7853982f;
                }

                float angle = delta < 0.0f ? -fabsf(twist) : fabsf(twist);
                this->m_worldModel->SetBoneFlags(4, 0x80, 0x80);
                CM2Model* model = this->m_worldModel;
                C3Vector axis(0.0f, 0.0f, 1.0f);
                model->SetBoneProceduralTransform(4, C44Matrix::Rotation(angle + spineOffset, axis, true));
                remaining = remaining - twist;
            }

            if (this->m_animationState & 0x4) {
                float twist = remaining < 0.7853982f ? remaining : 0.7853982f;
                float angle = delta < 0.0f ? -fabsf(twist) : fabsf(twist);
                this->m_worldModel->SetBoneFlags(6, 0x80, 0x80);
                CM2Model* model = this->m_worldModel;
                C3Vector axis(0.0f, 0.0f, 1.0f);
                model->SetBoneProceduralTransform(6, C44Matrix::Rotation(angle + headOffset, axis, true));
            }
        }
    }

    if (pitchBone) {
        this->m_worldModel->SetBoneFlags(4, 0x80, 0x80);
        C3Vector axis(0.0f, 1.0f, 0.0f);
        CM2Model* model = this->m_worldModel;
        model->SetBoneProceduralTransform(4, C44Matrix::Rotation(-this->GetPitch(), axis, true));
    }

    this->RotateWheels();
    uint32_t turnState = this->m_animationState & 0x60;

    if (!(this->m_movementData.m_flags & 0x170080F) && (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !static_cast<CGPlayer_C*>(this)->m_lootTarget) && !this->GetClientStandState()) {
        if (static_cast<int32_t>(time - this->m_turnAnimTime - 100) > 0) {
            this->m_animationState &= ~0x60;
        }

        uint32_t bit;

        if ((turn > 0.00001f && (bit = 0x20, turn > 0.01f || (turnState & bit))) || (turn < -0.00001f && (bit = 0x40, turn < -0.01f || (turnState & bit)))) {
            this->m_animationState |= bit;
            this->m_turnAnimTime = time;
        }

        if ((this->m_animationState & 0x60) != turnState) {
            this->UpdateMovementAnimKit(0);
        }

        if (this->GetObjectModel()->IsLoaded()) {
            this->GetObjectModel()->GetBoneSequenceId(0xFFFFFFFF);
        }
    } else if (turnState) {
        this->m_animationState &= ~0x60;
        this->UpdateMovementAnimKit(0);
    }

    this->UpdateDelayedSpellVisualKits();

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->UpdateAttackIndicatorList();
    }
}

// OFFSET: 0x005B5270
void CGUnit_C::UpdateActiveHeadLook() {
    float yawSum = 0.0f;
    float pitchSum = 0.0f;
    int32_t found = 0;

    for (CEffect* effect = this->m_effectList; effect; ) {
        CEffect* next = effect->m_next;
        float yaw = 0.0f;
        float pitch = 0.0f;

        if (effect->GetOrientationAngles(yaw, pitch)) {
            yawSum = yaw + yawSum;
            pitchSum = pitch + pitchSum;
            found = 1;
        }

        effect = next;
    }

    if (found && !this->m_activeHeadLook) {
        ActiveHeadLook* headLook = static_cast<ActiveHeadLook*>(s_activeHeadLookAllocator.GetData(0, typeid(ActiveHeadLook).raw_name(), -2));

        if (headLook) {
            headLook->m_targetSpine = 0.0f;
            headLook->m_targetHead = 0.0f;
            headLook->m_spine = 0.0f;
            headLook->m_head = 0.0f;
        }

        this->m_activeHeadLook = headLook;
    }

    if (this->m_activeHeadLook) {
        float dt = FrameTime::GetTickTimeSec();
        float diff = CMath::normalizeAngleNegPiToPi_(pitchSum - this->m_activeHeadLook->m_targetSpine);
        float maxStep = s_headLookSpineRate * dt;
        float step = maxStep > fabsf(diff) ? diff : (diff < 0.0f ? -fabsf(maxStep) : fabsf(maxStep));
        this->m_activeHeadLook->m_targetSpine = CMath::normalizeAngleNegPiToPi_(this->m_activeHeadLook->m_targetSpine + step);

        diff = CMath::normalizeAngleNegPiToPi_(yawSum - this->m_activeHeadLook->m_targetHead);
        maxStep = s_headLookHeadRate * dt;
        step = maxStep > fabsf(diff) ? diff : (diff < 0.0f ? -fabsf(maxStep) : fabsf(maxStep));
        this->m_activeHeadLook->m_targetHead = CMath::normalizeAngleNegPiToPi_(this->m_activeHeadLook->m_targetHead + step);

        float headBlend = 1.0f - static_cast<float>(CMath::exp2_(-(dt / s_headLookHeadTime) * 1.442695f));
        float spineBlend = 1.0f - static_cast<float>(CMath::exp2_(-(dt / s_headLookSpineTime) * 1.442695f));
        this->m_activeHeadLook->m_spine = CMath::normalizeAngleNegPiToPi_(CMath::normalizeAngleNegPiToPi_(this->m_activeHeadLook->m_targetSpine - this->m_activeHeadLook->m_spine) * spineBlend + this->m_activeHeadLook->m_spine);
        this->m_activeHeadLook->m_head = CMath::normalizeAngleNegPiToPi_(CMath::normalizeAngleNegPiToPi_(this->m_activeHeadLook->m_targetHead - this->m_activeHeadLook->m_head) * headBlend + this->m_activeHeadLook->m_head);
    }

    if (!found) {
        ActiveHeadLook* headLook = this->m_activeHeadLook;

        if (headLook && s_headLookEpsilon > fabsf(headLook->m_targetSpine) && s_headLookEpsilon > fabsf(headLook->m_targetHead) && s_headLookEpsilon > fabsf(headLook->m_spine) && s_headLookEpsilon > fabsf(headLook->m_head)) {
            s_activeHeadLookAllocator.PutData(headLook, NULL, 0);
            this->m_activeHeadLook = NULL;
        }
    }
}

// OFFSET: #0x0071D940
int32_t IsEmoteAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);
    int32_t behavior = rec ? rec->m_behaviorID : 506;

    switch (behavior) {
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
        case 76:
        case 77:
        case 78:
        case 80:
        case 81:
        case 82:
        case 83:
        case 84:
        case 113:
        case 136:
        case 137:
        case 138:
        case 178:
        case 185:
        case 186:
        case 195:
            return 1;
        default:
            return 0;
    }
}

// OFFSET: #0x0071D380
int32_t IsSpellCastAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);
    int32_t behavior = rec ? rec->m_behaviorID : 506;

    switch (behavior) {
        case 2:
        case 32:
        case 33:
        case 53:
        case 54:
            return 1;
        default:
            return 0;
    }
}

// OFFSET: #0x0071DA20
int32_t IsThrownWeaponAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior == 107 || (behavior > 110 && behavior <= 112)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071DA60
int32_t IsBowAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior == 46 || behavior == 105 || behavior == 109) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071DAA0
int32_t IsRifleAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior == 49 || behavior == 106 || behavior == 110) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005B8F90/0x14022CEA0
void CGUnit_C::UpdateBreathState(uint32_t time) {
    uint32_t liquidType;
    float height;
    int32_t liquidFlags;
    this->m_clientFlags &= ~0x60;

    if (World::QueryObjectLiquid(this->m_worldObject, liquidType, height, liquidFlags)) {
        float headHeight = this->m_height * this->m_scale;

        if (height - this->GetPosition().z > headHeight + 5.0f) {
            this->m_clientFlags |= 0x20;
            this->m_nextBreathCheckTime = time + 10000;
            return;
        }
    }

    if (AreaListZoneHasBreathParticles(this->m_worldObject)) {
        this->m_clientFlags |= 0x40;
    }

    this->m_nextBreathCheckTime = time + 10000;
}

// OFFSET: 0x005BA6B0
void CGUnit_C::UpdateModelTint(uint32_t time) {
    CImVector color(0xFFFFFFFF);

    if (!this->CalculateVertexColor(time, &color) && this->m_procEffects) {
        color = CImVector(this->m_procEffects->m_tintColor);
    }

    float r = color.r * 0.0039215689f;
    float g = color.g * 0.0039215689f;
    float b = color.b * 0.0039215689f;
    CM2Model* model = this->GetObjectModel();
    model->m_diffuseTintR = r;
    model->m_diffuseTintG = g;
    model->m_diffuseTintB = b;
}

// OFFSET: #0x0073ADC0
void CGUnit_C::ProcessEmoteQueue() {
    if (this->m_emoteQueue[0].m_emote == -1) {
        return;
    }

    uint32_t index;

    for (index = 0; index < 4; index++) {
        if (this->m_emoteQueue[index].m_emote) {
            break;
        }
    }

    if (index == 4) {
        this->m_animationState |= 0x8000;
        this->m_emoteQueue[0].m_emote = -1;
        return;
    }

    if (this->m_emoteQueue[index].m_delay > OsGetAsyncTimeMs()) {
        return;
    }

    EmotesRec* emote = g_emotesDB.GetRecord(this->m_emoteQueue[index].m_emote);
    this->PlayBaseAnimation(emote->m_animID, 0);
    this->m_emoteQueue[index].m_emote = 0;

    if (index < 3 && this->m_emoteQueue[index + 1].m_emote) {
        M2SequenceInfo info;
        info.Clear();
        this->m_worldModel->GetSequenceInfo(emote->m_animID, 0, info);
        this->m_emoteQueue[index + 1].m_delay += OsGetAsyncTimeMs() + info.m_duration + 500;
    } else {
        this->m_animationState |= 0x8000;
        this->m_emoteQueue[0].m_emote = -1;
    }
}

// OFFSET: 0x005B9340
void CGUnit_C::SmoothFacingAngle(float facing) {
    float target = this->m_smoothTarget.m_value;

    if (this->m_renderFacing > target + 3.1415927f) {
        target = target + 6.2831855f;
    } else if (target - 3.1415927f > this->m_renderFacing) {
        target = target - 6.2831855f;
    }

    if (target > this->m_renderFacing + 1.5704823f) {
        this->m_renderFacing = target - 1.5704823f;
    } else if (this->m_renderFacing - 1.5704823f > target) {
        this->m_renderFacing = target + 1.5704823f;
    }

    float render = CMath::normalizeAngleNegPiToPi_(this->m_renderFacing);
    this->m_renderFacing = render;

    if (render > facing + 3.1415927f) {
        facing = facing + 6.2831855f;
    } else if (facing - 3.1415927f > render) {
        facing = facing - 6.2831855f;
    }

    float elapsed = CGWorldFrame::s_currentWorldFrame->m_elapsedSec;
    float offset = this->m_renderFacing - facing;
    float k = elapsed * 20.0f;
    float k2 = k * k;
    float damping = 1.0f / (k2 * k * 0.235f + k2 * 0.48f + k + 1.0f);

    if (fabsf(offset) > 0.00000011920929f) {
        float change = (offset * 20.0f + this->m_facingVelocity) * elapsed;
        this->m_renderFacing = (change + offset) * damping + facing;
        this->m_facingVelocity = (this->m_facingVelocity - change * 20.0f) * damping;
    } else {
        this->m_renderFacing = facing;
        this->m_facingVelocity = 0.0f;
    }

    this->m_renderFacing = (1.0f - this->m_facingBlend) * facing + this->m_facingBlend * this->m_renderFacing;
}

// OFFSET: 0x005B4A80
void CGUnit_C::RotateWheels() {
    if (!this->m_wheelMask) {
        return;
    }

    CM2Model* model = this->GetObjectModel();
    uint32_t now = FrameTime::GetCurTimeMs();
    int32_t elapsedMs = now - this->m_lastWheelTime;

    if (elapsedMs <= 1) {
        elapsedMs = 1;
    } else if (elapsedMs >= 500) {
        elapsedMs = 500;
    }

    float elapsed = static_cast<float>(elapsedMs) * 0.001f;
    this->m_lastWheelTime = now;
    CGUnit_C* mover = this;
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    while (passenger) {
        VehicleSeatRec* seat = passenger->m_seatRec;

        if (!seat || !(seat->m_flags & 0x4000000)) {
            break;
        }

        CGUnit_C* vehicle = passenger->GetVehicleUnit();

        if (!vehicle) {
            break;
        }

        mover = vehicle;
        passenger = vehicle->m_vehiclePassenger;
    }

    CMovement_C* movement = mover->m_movement;
    uint32_t flags = movement->m_flags;
    float distance;

    if (flags & 0xD) {
        distance = movement->m_currentSpeed * elapsed;
    } else if (flags & 0x2) {
        distance = -(movement->m_currentSpeed * elapsed);
    } else {
        distance = 0.0f;
    }

    float turn;

    if (flags & 0x10) {
        turn = (-movement->m_turnRate - this->m_smoothTarget.m_velocity) * elapsed;
    } else if (flags & 0x20) {
        turn = (movement->m_turnRate - this->m_smoothTarget.m_velocity) * elapsed;
    } else {
        turn = -(this->m_smoothTarget.m_velocity * elapsed);
    }

    CMoveSpline* spline = movement->m_spline;

    if ((spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x200)) || (flags & 0x1100000)) {
        for (int32_t i = 0; i < 8; i++) {
            if ((1 << i) & this->m_wheelMask) {
                model->SetBoneFlags(i + 27, 0x80, 0x80);
                C3Vector axis(0.0f, 1.0f, 0.0f);
                model->SetBoneProceduralTransform(i + 27, C44Matrix::Rotation(this->m_wheelAngles[i], axis, true));
            }
        }
    } else {
        for (int32_t i = 0; i < 8; i++) {
            if ((1 << i) & this->m_wheelMask) {
                C3Vector pivot = model->GetBonePivot(i + 27);

                if (fabs(pivot.z) > 0.001f) {
                    const C44Matrix& transform = model->m_worldTransform;
                    float scale = sqrt(transform.a2 * transform.a2 + transform.a1 * transform.a1 + transform.a0 * transform.a0);
                    pivot.x = scale * pivot.x;
                    pivot.y = scale * pivot.y;
                    float angle = (pivot.y * turn + distance) / (scale * pivot.z) + this->m_wheelAngles[i];
                    pivot.z = scale * pivot.z;
                    this->m_wheelAngles[i] = angle;
                    this->m_wheelAngles[i] = CMath::normalizeAngleNegPiToPi_(angle);
                }

                model->SetBoneFlags(i + 27, 0x80, 0x80);
                C3Vector axis(0.0f, 1.0f, 0.0f);
                model->SetBoneProceduralTransform(i + 27, C44Matrix::Rotation(this->m_wheelAngles[i], axis, true));
            }
        }
    }
}

// OFFSET: 0x005B6A60
void CGUnit_C::CreateRipple(uint32_t a2) {
    if (this->m_liquidPerturbator) {
        this->m_liquidPerturbator->m_amplitude = 0.0f;
    }

    if (Liquid::HasDynamicDisplMap() && this->m_unit->m_unitFieldHealth <= 0) {
        return;
    }

    uint32_t liquidType;
    float liquidHeight;
    int32_t liquidFlags;

    if (!World::QueryObjectLiquid(this->m_worldObject, liquidType, liquidHeight, liquidFlags)) {
        return;
    }

    int32_t flag8;
    int32_t flag9;
    World::QueryObjectLiquidFlags(this->m_worldObject, flag8, flag9);

    if (!flag8) {
        return;
    }

    if (a2) {
        this->m_nextRippleTime = 0;
    }

    if (Liquid::HasDynamicDisplMap()) {
        uint32_t flags;

        if (!this->IsInCombat() && !((flags = this->m_movement->m_flags) & 0x3F) && !(flags & 0x800) && !this->CGUnit::IsJumping() && !(flags & 0x100000) && !this->CGUnit::IsOnFallingSpline()) {
            return;
        }

        if (!this->GetObjectModel()) {
            return;
        }

        CAaBox box = this->GetObjectModel()->GetCurrentBoundingBox();

        if (box.IsEmpty()) {
            return;
        }

        C3Vector position = this->GetObjectModel()->HasBone(0xFFFFFFFF) ? this->GetObjectModel()->GetBonePosition(0xFFFFFFFF) : this->GetPosition();
        float t = (liquidHeight - box.b.z) / (box.t.z - box.b.z);

        if (t > 1.0f) {
            t = 1.0f;
        } else if (0.0f >= t) {
            return;
        }

        float amplitude = sin(t * 3.1415927f);
        CMovement_C* movement = this->m_movement;
        uint32_t moveFlags = movement->m_flags;

        if (!(moveFlags & 0x800) && !this->CGUnit::IsJumping()) {
            if ((moveFlags & 0x100000) && !(moveFlags & 0x3F) && !this->IsInCombat()) {
                amplitude = 0.0f;
            } else {
                float scale = movement->m_currentSpeed * 0.083333336f + 0.5f;

                if (0.0f > scale) {
                    amplitude = 0.0f * amplitude;
                } else {
                    if (scale >= 1.0f) {
                        scale = 1.0f;
                    }

                    amplitude = scale * amplitude;
                }
            }
        }

        if (!this->m_liquidPerturbator) {
            if (amplitude <= 0.0f) {
                return;
            }

            this->m_liquidPerturbator = Liquid::CreatePerturbator(Liquid::PertSurface_Water, this->IsActivePlayer());
        }

        if (!this->m_liquidPerturbator) {
            return;
        }

        CAaSphere sphere = this->GetObjectModel()->GetCurrentBoundingSphere();
        this->m_liquidPerturbator->m_pos = position;
        this->m_liquidPerturbator->m_radius = sphere.r * 0.1f + 0.2f;
        this->m_liquidPerturbator->m_amplitude = amplitude;
        this->m_liquidPerturbator->m_period = 800;
        return;
    }

    uint32_t type = 0;

    if (a2) {
        if (a2 == 1) {
            type = 3;
        }
    } else {
        uint32_t flags = this->m_movement->m_flags;

        if (flags & 0xF) {
            type = 2;
        } else if (flags & 0x30) {
            type = 1;
        }
    }

    float maxDepth = this->m_movementData.m_collisionHeight * 2.0f;

    if (maxDepth <= 1.0f) {
        maxDepth = 1.0f;
    }

    float halfDepth = maxDepth * 0.5f;
    float depth = liquidHeight - this->GetPosition().z;

    if (depth >= maxDepth) {
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();

    if (this->m_nextRippleTime && static_cast<int32_t>(time - this->m_nextRippleTime) < 0) {
        return;
    }

    C3Vector position = this->GetPosition();
    position.z = liquidHeight;
    float speedFactor = 1.0f;
    float speed = this->m_movement->m_currentSpeed;

    if (s_rippleDirectional[type] && speed > 0.0001f) {
        if (speed >= 20.0f) {
            speed = 20.0f;
        }

        speedFactor = 2.5f / speed;
    }

    float invSpeed = 1.0f / speedFactor;
    float facing = this->GetFacing();
    float scale = this->GetScale() * 0.33333334f;
    float size = (CRandom::ufloat_(g_rndSeed) * 0.2f - 0.1f + 1.0f) * scale;

    if (size < 0.33333334f) {
        size = 0.33333334f;
    } else if (size >= 1.6666666f) {
        size = 1.6666666f;
    }

    float alpha = CRandom::ufloat_(g_rndSeed) * 0.1f - 0.05f + 0.65f;
    float rippleSpeed = (CRandom::ufloat_(g_rndSeed) * 1.5f - 0.75f + 3.75f) * 12.0f * 0.027777778f * invSpeed;
    float width = 0.16666667f;
    float depthFactor = 1.0f;

    if (depth > halfDepth) {
        depthFactor = (maxDepth - depth) / (maxDepth - halfDepth) * 0.5f + 0.5f;
        width = depthFactor * 0.16666667f;
        alpha = depthFactor * alpha;
        size = depthFactor * size;
    }

    if (!type) {
        width = width * 0.8f;
        rippleSpeed = rippleSpeed * 0.25f;
        size = size * 0.6f;
    }

    uint8_t directional = s_rippleDirectional[type];
    float angle;

    if (directional) {
        angle = facing;
        uint32_t flags = this->m_movement->m_flags;

        if (flags & 0x4) {
            if (flags & 0x1) {
                angle = angle + 0.7853982f;
            } else if (flags & 0x2) {
                angle = angle + 2.3561945f;
            } else {
                angle = angle + 1.5707964f;
            }
        } else if (flags & 0x8) {
            if (flags & 0x1) {
                angle = angle - 0.7853982f;
            } else if (flags & 0x2) {
                angle = angle - 2.3561945f;
            } else {
                angle = angle - 1.5707964f;
            }
        } else if (flags & 0x2) {
            angle = angle + 3.1415927f;
        }
    } else {
        angle = CRandom::ufloat_(g_rndSeed) * 6.2831855f;
    }

    World::CreateWaterRipple(position, angle, size, alpha, width, rippleSpeed, directional, this->IsActiveMover());

    if (s_rippleDirectional[type]) {
        this->m_nextRippleTime = time - static_cast<int32_t>(depthFactor * speedFactor * 0.25f * -1000.0f);
    } else {
        this->m_nextRippleTime = CRandom::dice_(50, g_rndSeed) + time + 400;
    }
}

// OFFSET: 0x005C2C30/0x140238F40
void CGUnit_C::UpdateDelayedSpellVisualKits() {
    uint32_t count = this->m_delayedSpellVisualKits.m_count;
    int32_t pending = 0;

    if (!count) {
        return;
    }

    for (uint32_t i = 0; i < count; i++) {
        DelayedSpellVisualKit* delayed = &this->m_delayedSpellVisualKits.m_data[i];

        if (!(delayed->m_flags & 0x8)) {
            continue;
        }

        if (static_cast<int32_t>(FrameTime::GetCurTimeMs() - delayed->m_startTime) < 0) {
            pending++;
            continue;
        }

        if (delayed->m_delayType == 1) {
            const SpellRec* spell = g_spellDB.GetRecord(delayed->m_spellId);

            if (spell) {
                SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(delayed->m_kitId);

                if (kit) {
                    PlaySpellVisuaKitData data(spell, kit, delayed->m_kitType);
                    uint32_t flags = delayed->m_flags;

                    if (flags & 0x1) {
                        data.m_position = &delayed->m_position;
                        data.m_transportGUID = delayed->m_targetGuid;
                    }

                    data.m_displayId = delayed->m_displayId;
                    data.m_inventoryType = delayed->m_inventoryType;
                    data.unk_0024 = (flags >> 2) & 0x1;
                    data.m_animParam = (flags >> 1) & 0x1;
                    data.unk_0028 = 1;
                    data.unk_002C = delayed->m_delay;
                    this->PlaySpellVisualKit(data);
                }
            }
        }

        delayed->m_flags &= ~0x8;
    }

    if (!pending && count) {
        this->m_delayedSpellVisualKits.SetCount(0);
    }
}

// OFFSET: 0x005C6AA0/0x14023DC70
void CGUnit_C::RefreshInteractIcon() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (this->UnitReaction(player) < 3) {
        this->m_questStatus = 0;
        this->m_interactIconType = 0;
        this->RemoveInteractIcon();
        return;
    }

    if (this->IsQuestGiver()) {
        player->UpdateQuestStatus(this);
    } else {
        this->m_questStatus = 0;
    }

    if (this->m_unit->m_unitNpcFlags & 0x2000) {
        player->UpdateTaxiStatus(this);
    } else {
        this->m_interactIconType = 0;
    }

    if (!this->IsQuestGiver() && !(this->m_unit->m_unitNpcFlags & 0x2000) && !this->GetNPCInteractIcon()) {
        this->RemoveInteractIcon();
    }
}

// OFFSET: 0x005BDD20
void CGUnit_C::OnPvPFlagChanged(uint8_t oldFlags) {
    if (oldFlags == this->m_unit->m_unitFieldBytes2[1]) {
        return;
    }

    this->UpdateReaction(false);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && (this->m_unit->m_unitFieldBytes2[1] & 0x1)) {
        CGTutorial::TriggerTutorial(31);
    }
}

// PROVEN: e0e66b78 IDENTICAL/NONE
// OFFSET: 0x005C5820/0x14023C760
void CGUnit_C::ScaleChangeUpdate() {
    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();
    this->UpdateInteractIconScale();

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateLargestPassengerBoundsRadius();
    }
}

// PROVEN: dfc5db38 IDENTICAL/NONE
// OFFSET: 0x005C5850/0x14023C7B0
void CGUnit_C::ScaleChangeFinished() {
    if (this->m_nameDesc) {
        PlayerNameTriggerNameRegenerate(this->m_nameDesc);
    }

    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();
    this->UpdateInteractIconScale();

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateLargestPassengerBoundsRadius();
    }
}

// OFFSET: #0x007275C0
void CGUnit_C::RenderAutoTrackCursor() {
    if (this->m_obj->m_guid != CGUnit_C::s_activeMover || CGUnit_C::s_trackingType == 13 || (CGUnit_C::s_trackingType != 4 && CGUnit_C::s_trackingType != 10)) {
        return;
    }

    int32_t phase = OsGetAsyncTimeMs() % s_trackCursorPeriod;
    float size = (sinf(phase * 6.2831855f / static_cast<int32_t>(s_trackCursorPeriod)) + 1.0f) * 0.5f * (s_trackCursorMaxSize - s_trackCursorMinSize) + s_trackCursorMinSize;
    C3Vector position = this->m_movementData.GetPosition(CGUnit_C::s_trackingPosition);
    CAaBox box(C3Vector(position.x - size, position.y - size, position.z - size), C3Vector(position.x + size, position.y + size, position.z + size));
    C44Matrix matrix;
    this->BuildSelectionRotMatrix(matrix, CGUnit_C::s_trackingPosition);
    g_theGxDevicePtr->RsPush();

    if (this->SetCircleRenderStates()) {
        CImVector color(0x7F, 0xFF, 0xFF, 0xFF);
        ProjectTex2d(box, color, &matrix, 0.5f, 0x200122, 1, GxPolygonOffset_Default);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: %0x005CD740
void CGUnit_C::Render() {
    this->RenderAutoTrackCursor();
}

// OFFSET: 0x005AFC70/0x140222AC0
void CGUnit_C::PlayFallLandAnimation(uint32_t oldFlags, int32_t jumping) {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;
    uint32_t landing = this->m_animationState & 0x1000;
    this->m_animationState &= ~0x1000;

    if (passenger && (passenger->m_state == 4 || passenger->m_state == 5)) {
        this->UpdateMovementAnimKit(0);
        return;
    }

    if (this->m_animationState & 0x2000) {
        this->UpdateMovementAnimKit(0);
        this->PlayDeathAnimKit(4);
        return;
    }

    if ((!jumping && !(oldFlags & 0x801000) && !landing) || (this->m_movement->m_flags & 0x1100000)) {
        this->UpdateMovementAnimKit(0);
        return;
    }

    uint32_t flags = this->m_movementData.m_flags;

    if (!(flags & 0xF)) {
        this->PlayUnitSound(12, 1, 0);
        this->UpdateMovementAnimKit(0);
        this->PlayJumpEndAnimKit();
        return;
    }

    if ((flags & 0x2) || (flags & 0x100) || this->IsRunning()) {
        this->UpdateMovementAnimKit(0);
        return;
    }

    this->PlayUnitSound(12, 1, 0);
    this->PlayMovementAnimKit(6, this->SelectAnimationSet()->m_jumpLandRun, 0xFFFFFFFF, 0);
}

// OFFSET: 0x005B85A0/0x14022C340
void CGUnit_C::OnCollideFallLand(uint32_t a1, int32_t a2) {
    this->PlayFallLandAnimation(a1, a2);

    if (!this->IsClientControlled()) {
        return;
    }

    CMoveSpline* spline = this->m_movementData.m_spline;

    if (!spline || (spline->m_flags & 0x80) || !(spline->m_flags & 0x40)) {
        if (spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x200)) {
            return;
        }

        if (this->m_movementData.m_flags & 0x20000200) {
            return;
        }
    }

    if (this->m_movementData.m_flags & 0x8100000) {
        return;
    }

    float distance = this->m_movementData.GetDistanceFallen();

    if (distance >= 70.0f || (distance > 13.0f && !this->IsDeadOrGhost())) {
        this->HandleEnvironmentDamage(2, 0, 0, 0);
    }
}

// OFFSET: 0x005B8650
int32_t CGUnit_C::OnCollideFallLandNotify(uint32_t time, uint32_t oldFlags, int32_t jumping) {
    this->OnCollideFallLand(oldFlags, jumping);
    int32_t sent = 0;

    if ((this->m_clientFlags & 0x80) || this->m_movementData.GetDistanceFallen() > 0.027777778f) {
        PlayerMoveFallLand msg;

        if (this->m_movementData.SendMoveStatus(time, MSG_MOVE_FALL_LAND, &msg.m_status, &msg)) {
            this->m_clientFlags &= ~0x80;
            sent = 1;
        }
    }

    if (this->m_clientFlags & 0x100000) {
        this->m_clientFlags &= ~0x100000;
        static_cast<CGPlayer_C*>(this)->HandleRepopRequest(true);
    }

    return sent;
}

// PROVEN: e4525aee IDENTICAL/NONE
// OFFSET: 0x005ACBD0
int32_t CMovement_C::SendFallResetMessage(uint32_t time) {
    PlayerMoveFallReset msg;
    return this->SendMoveStatus(time, CMSG_MOVE_FALL_RESET, &msg.m_status, &msg);
}

// PROVEN: 577e070f IDENTICAL/NONE
// OFFSET: 0x005AFD80
int32_t CGUnit_C::OnCollideFallReset(uint32_t time) {
    if (!(this->m_clientFlags & 0x80)) {
        return 0;
    }

    return this->m_movementData.SendFallResetMessage(time);
}

// OFFSET: 0x005AEBE0/0x140221580
void CGUnit_C::SendSplineDone(uint32_t time, uint32_t splineId) {
    PlayerMoveSplineDone msg;
    this->m_movementData.GetMoveStatus(CMSG_MOVE_SPLINE_DONE, time, &msg.m_status);
    msg.m_splineId = splineId;
    this->m_movementData.SendMoveMessage(time, &msg);
    CGInputControl* input = CGInputControl::GetActive();
    input->UpdateHalted();
    input->UpdatePlayer(time, 1);
}

// OFFSET: #0x00714CE0
void ApplyKitAnimFlags(uint32_t kitFlags, uint32_t* animFlags) {
    if (!animFlags) {
        return;
    }

    if (kitFlags & 0x4) {
        *animFlags |= 0x2;
    }

    if (kitFlags & 0x8) {
        *animFlags |= 0x4;
    }

    if (kitFlags & 0x10) {
        *animFlags |= 0x8;
    }

    if (kitFlags & 0x100) {
        *animFlags |= 0x20;
    }
}

// OFFSET: #0x0071DFC0
int32_t CGUnit_C::HasAnim466() {
    uint32_t animId = this->ResolveAnimForModel(ANIM_DEATH_START, NULL);
    return this->m_worldModel && this->m_worldModel->HasSequence(animId);
}

// OFFSET: #0x00724060
int32_t CGUnit_C::ChooseDeathAnim(uint32_t mask, uint32_t* anim, uint32_t* animFlags) {
    if (!this->AppearsDead(1)) {
        return (mask & ~0x1) == 0;
    }

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_state == 3 && !(this->m_unit->m_unitFieldFlags2 & 0x20000)) {
        return (mask & ~0x1) == 0;
    }

    uint32_t flags = 0;

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        const SpellVisualKitRec* kit = effect->m_kitRec;

        if (kit && IsDeadOrDyingAnim(kit->m_animID)) {
            if (kit->m_flags & 0x4) {
                flags |= 0x2;
            }

            if (kit->m_flags & 0x8) {
                flags |= 0x4;
            }

            if (kit->m_flags & 0x10) {
                flags |= 0x8;
            }

            if (kit->m_flags & 0x100) {
                flags |= 0x20;
            }

            if (flags & 0x2) {
                *anim = effect->m_kitRec->m_animID;

                if (animFlags) {
                    *animFlags |= 0x2;
                }

                return 1;
            }
        }
    }

    uint32_t torsoAnim = this->GetCurrentTorsoAnimId();

    if ((this->m_animationState & 0x4000000) && IsDeadOrDyingAnim(torsoAnim)) {
        return 1;
    }

    if (this->HasAnim466()) {
        if ((this->m_movement->m_flags & MOVEMENTFLAG_FALLING) || this->m_movementData.IsOnFallingSpline() || this->m_movementData.IsOnParabolicSpline()) {
            *anim = ANIM_DEATH_START;
        } else {
            *anim = ANIM_DEATH_END_HOLD;
        }

        return 1;
    }

    uint32_t moveFlags = this->m_movementData.m_flags;

    if ((!(moveFlags & MOVEMENTFLAG_FALLING) || !(moveFlags & MOVEMENTFLAG_FALLING_FAR)) && !IsDyingAnim(torsoAnim) && (mask & 0x1)) {
        *anim = (moveFlags & MOVEMENTFLAG_SWIMMING) ? 132 : 6;
    }

    return 1;
}

// OFFSET: #0x00724330
int32_t CGUnit_C::ChooseSpellVisualKitAnim(uint32_t mask, uint32_t* anim, uint32_t* animFlags) {
    if ((!this->m_unit->m_unitChannelSpell && !(this->m_animationState & 0x400)) || !(this->m_animationState & 0x60) || this->m_mountModel) {
        return (mask & ~0x1F) == 0;
    }

    uint32_t kitAnim;
    uint32_t kitFlags = 0;

    if (!this->GetSpellVisualKitAnim(&kitAnim, &kitFlags)) {
        return (mask & ~0x1F) == 0;
    }

    ApplyKitAnimFlags(kitFlags, animFlags);

    if (kitAnim == this->GetCurrentTorsoAnimId() && this->GetCurrentTorsoAnimId() != this->GetCurrentBaseAnimId()) {
        *anim = kitAnim;
        return 1;
    }

    if (kitAnim != this->GetCurrentTorsoAnimId()) {
        if (mask & 0x10) {
            *anim = kitAnim;
        }

        return 1;
    }

    if ((*animFlags & 0x10) && (kitFlags & 0x40)) {
        *anim = kitAnim;
    }

    return 1;
}

// OFFSET: #0x0071E180
int32_t CGUnit_C::ChooseShuffleAnim(uint32_t mask, uint32_t* anim) {
    if (!this->CanPlayTurnAnim()) {
        return (mask & ~0x7F) == 0;
    }

    if ((this->m_animationState & 0x70) && (mask & 0x40)) {
        if (!(this->m_movementData.m_flags & MOVEMENTFLAG_TURN_LEFT) && !(this->m_animationState & 0x800)) {
            *anim = ANIM_SHUFFLE_RIGHT;
            return 1;
        }

        *anim = ANIM_SHUFFLE_LEFT;
    }

    return 1;
}

// OFFSET: 0x005C82E0/0x14023F850
void CGUnit_C::PlayDeathAnim(int32_t force) {
    if (!force && this->m_deathAnimKit) {
        return;
    }

    uint32_t anim = 466;
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if ((this->m_movement->m_flags & 0x100000) || (passenger && passenger->m_seatRec && (passenger->m_seatRec->m_flagsC & 0x2)) || (this->IsHovering() && (!this->HasDeathSequence() || this->IsDeepInLiquid()) && this->IsSplineDestinationInLiquid())) {
        anim = 131;
    }

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->IsMirroringVehicleAnims()) {
        this->m_vehiclePassenger->UpdateVehicleAnimMirroring();
    }

    if (!this->m_worldModel->IsLoaded()) {
        return;
    }

    if (anim == 466 && !this->HasDeathSequence()) {
        anim = 1;
    }

    uint32_t type = 1;

    if (anim == 131) {
        type = 7;
    } else if (anim == 466) {
        this->PlayDeathAnimKit(2);
        return;
    }

    this->PlayDeathAnimKit(type);
}

// OFFSET: 0x005D2ED0
void CGUnit_C::UpdateClientStandState(uint32_t oldStandState, uint32_t standState) {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        static_cast<CGPlayer_C*>(this)->SetClientStandState(standState);
    }

    if (this->m_sheatheState && this->GetClientStandState() && this->GetClientStandState() != 2) {
        this->SetSheatheState(0, 1, 0);
    }

    this->UpdateStandStateAnimKit(oldStandState, 0);

    if (this->m_obj->m_guid == CGWorldFrame::GetActiveCamera()->m_targetGUID) {
        CGGameUI::ResetCamera(this->m_obj->m_guid);
    }
}

// OFFSET: 0x005B8BF0/0x14022CA20
int32_t CGUnit_C::UnitReaction(int32_t factionTemplate, const CGUnit_C* unit) {
    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(factionTemplate);
    CGUnitData* data = unit->m_unit;
    FactionTemplateRec* other = g_factionTemplateDB.GetRecord(data->m_unitFieldFactiontemplate);

    if (!faction || !other) {
        return 3;
    }

    if (data->m_unitFieldFlags & 0x8) {
        CGUnit_C* controller = const_cast<CGUnit_C*>(unit)->GetControllingPlayer();

        if (controller && controller->IsActivePlayer()) {
            if ((faction->m_flags & 0x1000) && (static_cast<CGPlayer_C*>(controller)->m_player->m_playerFlags & 0x100)) {
                return 1;
            }

            if (CGReputationInfo::GetForcedReaction(faction->m_faction, &factionTemplate)) {
                return factionTemplate;
            }

            if (!(unit->m_unit->m_unitFieldFlags2 & 0x4)) {
                int32_t factionId = faction->m_faction;

                if (CGUnit_C::FactionHasReputation(factionId)) {
                    return CGReputationInfo::GetFactionStandingReaction(factionId);
                }
            }
        }
    }

    return FactionTemplateReaction(faction, other);
}

// OFFSET: 0x005BEB70/0x1402340D0
int32_t CGUnit_C::UnitReaction(const CGUnit_C* unit) const {
    if (unit == this) {
        return 4;
    }

    CGUnit_C* self = const_cast<CGUnit_C*>(this);
    CGUnit_C* other = const_cast<CGUnit_C*>(unit);
    CGUnitData* data = this->m_unit;

    if ((data->m_unitFieldFlags & 0x8) && (unit->m_unit->m_unitFieldFlags & 0x8)) {
        CGUnit_C* controller = self->GetControllingPlayer();
        CGUnit_C* otherController = other->GetControllingPlayer();

        if (controller && otherController) {
            CGPlayerLocalData* player = static_cast<CGPlayer_C*>(controller)->m_player;

            if (player->m_playerDuelTeam) {
                CGPlayerLocalData* otherPlayer = static_cast<CGPlayer_C*>(otherController)->m_player;

                if (otherPlayer->m_playerDuelTeam && player->m_playerDuelArbiter == otherPlayer->m_playerDuelArbiter) {
                    return player->m_playerDuelTeam != otherPlayer->m_playerDuelTeam ? 1 : 4;
                }
            }

            if (controller == otherController) {
                return 4;
            }

            if (controller->IsActivePlayer()) {
                WOWGUID guid = otherController->m_obj->m_guid;

                if (CGGameUI::IsInParty(guid)) {
                    return CGUnit_C::UnitReaction(this->m_unit->m_unitFieldFactiontemplate, unit);
                }

                guid = otherController->m_obj->m_guid;

                if (CGGameUI::IsPartyMember(guid)) {
                    return CGUnit_C::UnitReaction(this->m_unit->m_unitFieldFactiontemplate, unit);
                }
            }

            if (otherController->IsActivePlayer()) {
                WOWGUID guid = controller->m_obj->m_guid;

                if (CGGameUI::IsInParty(guid)) {
                    return CGUnit_C::UnitReaction(this->m_unit->m_unitFieldFactiontemplate, unit);
                }

                guid = controller->m_obj->m_guid;

                if (CGGameUI::IsPartyMember(guid)) {
                    return CGUnit_C::UnitReaction(this->m_unit->m_unitFieldFactiontemplate, unit);
                }
            }
        } else if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !otherController) {
            if (self->IsDuelingWith(other)) {
                return 1;
            }
        } else if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !controller) {
            if (other->IsDuelingWith(self)) {
                return 1;
            }
        }

        data = this->m_unit;

        if ((data->m_unitFieldBytes2[1] & 0x4) && (unit->m_unit->m_unitFieldBytes2[1] & 0x4)) {
            return 1;
        }
    }

    if (data->m_unitFieldFlags & 0x8) {
        CGUnit_C* controller = self->GetControllingPlayer();

        if (controller && controller->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(unit->m_unit->m_unitFieldFactiontemplate);

            if (faction) {
                int32_t reaction;

                if (CGReputationInfo::GetForcedReaction(faction->m_faction, &reaction)) {
                    return reaction;
                }

                if (!(controller->m_unit->m_unitFieldFlags2 & 0x4) && CGUnit_C::FactionHasReputation(faction->m_faction)) {
                    if ((faction->m_flags & 0x1000) && (static_cast<CGPlayer_C*>(controller)->m_player->m_playerFlags & 0x100)) {
                        return 1;
                    }

                    return CGReputationInfo::IsAtWar(faction->m_faction) ? 1 : 4;
                }
            }
        }
    }

    int32_t reaction = CGUnit_C::UnitReaction(this->m_unit->m_unitFieldFactiontemplate, unit);

    if (reaction >= 7) {
        return 7;
    }

    return reaction;
}

// OFFSET: 0x005C2550/0x1402387B0
float CGUnit_C::GetPlayerDiscount(const CGUnit_C* unit) const {
    if (!unit) {
        return 0.0f;
    }

    FactionTemplateRec* factionTemplate = g_factionTemplateDB.GetRecord(this->m_unit->m_unitFieldFactiontemplate);

    if (!factionTemplate || !CGUnit_C::FactionHasReputation(factionTemplate->m_faction)) {
        return 0.0f;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[0]);

    if (race && (race->m_flags & 0x100)) {
        return 0.2f;
    }

    switch (this->UnitReaction(unit)) {
        case 4:
            return 0.05f;

        case 5:
            return 0.1f;

        case 6:
            return 0.15f;

        case 7:
            return 0.2f;

        default:
            return 0.0f;
    }
}
// OFFSET: 0x005BFB50/0x140235420
const char* CGUnit_C::GetVisibleUnitName(const char** realm, int32_t a2) {
    uint32_t high = static_cast<uint32_t>(this->m_obj->m_guid.m_guid >> 32);

    if (!(this->m_unit->m_unitFieldFlags & 0x2000000) || (this->m_obj->m_type & TYPEMASK_PLAYER) || this->IsClientControlled() || ((high & 0xF0F00000) == 0xF0500000 && this->HasAPlayerAsPassenger())) {
        return this->GetUnitName(realm, a2);
    }

    return NULL;
}

// OFFSET: 0x0044E390/0x14005C630
bool CGUnit_C::IsFriendly(const CGUnit_C* unit) const {
    return this->UnitReaction(unit) >= 4 || unit->UnitReaction(this) >= 4 || this->IsTreatAsRaidUnit();
}

// PROVEN: 346264f5 IDENTICAL/NONE
// OFFSET: 0x0044E3D0
bool CGUnit_C::IsHatedTowards(const CGUnit_C* unit) const {
    return this->UnitReaction(unit) <= 1;
}

// PROVEN: a712832d IDENTICAL/NONE
// OFFSET: 0x0056E080/0x1401CEC70
const char* CGUnit_C::GetObjectName() const {
    return const_cast<CGUnit_C*>(this)->GetUnitName(NULL, 1);
}

// PROVEN: 5fb81b4e IDENTICAL/NONE
// OFFSET: 0x0056E140/0x1401CED30
int32_t CGUnit_C::UpdateObjectNameString(int32_t a1, char* buffer, uint32_t size) {
    return CGUnit_C::UpdateObjectNameStringForUnit(this, a1, buffer, size, 1);
}

// PROVEN: 10a371f5 IDENTICAL/NONE
// OFFSET: 0x005AB9C0/0x14021DEA0
void CGUnit_C::ObjectNameVisibilityChanged() {
    if (!this->m_interactIcon) {
        return;
    }

    if (PlayerNameShowing(this->m_nameDesc) || this->m_namePlateFrame) {
        this->m_interactIcon->SetBoneSequence(0xFFFFFFFF, 190, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        this->m_interactIcon->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x005C7BD0/0x14023F0B0
int32_t CGUnit_C::CanHighlight() const {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (this->m_unit->m_unitFieldFlags2 & 0x10000000) && !player->IsFriendly(this)) {
        return 0;
    }

    CGUnitData* data = this->m_unit;

    if (data->m_unitFieldFlags & 0x2000000) {
        if (data->m_unitFieldCreatedby != ClntObjMgrGetActivePlayer()) {
            return 0;
        }

        if (this->m_obj->m_guid != CGPetInfo::GetPet(0)) {
            return 0;
        }
    }

    return 1;
}

// PROVEN: 0aca3875 IDENTICAL/NONE
// OFFSET: 0x005AFFD0/0x140222E60
int32_t CGUnit_C::GetSelectionHighlightColor(CImVector* color) const {
    if (this->m_clientFlags & 0x10) {
        *color = *reinterpret_cast<const CImVector*>(s_attackTargetColor);
    } else {
        WOWGUID guid = this->m_obj->m_guid;
        CGGameUI::GetSelectionHighlightColor(guid, color, 1);
    }

    return 1;
}

// OFFSET: 0x00A27BF0/0x1407CF3A0
float CalculateFacingTo(const C3Vector* from, const C3Vector* to) {
    float dx = to->x - from->x;
    float dy = to->y - from->y;

    if (2.3841858e-7f <= fabsf(dx)) {
        if (fabsf(dy) < 2.3841858e-7f) {
            return from->x <= to->x ? 0.0f : 3.1415927f;
        }

        return static_cast<float>(atan2(dy, dx));
    }

    return 0.0f <= dy ? 0.5f * 3.1415927f : 1.5f * 3.1415927f;
}

// OFFSET: 0x005B4210/0x140227770
uint64_t CGUnit_C::GetComboPointTarget() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return static_cast<const CGPlayer_C*>(this)->m_lootTarget.m_guid;
    }

    return 0;
}

// OFFSET: 0x005BE100/0x140233510
void CGUnit_C::UpdatePassengerSmoothFacing(const C44Matrix* parentMatrix, float* facingOffset) {
    float facing;

    if (facingOffset) {
        facing = this->m_smoothTarget.m_value + *facingOffset;
    } else {
        facing = this->GetSmoothFacing();
    }

    C44Matrix matrix;

    if (parentMatrix) {
        matrix = C44Matrix::RotationAroundZ(this->m_smoothTarget.m_value);
        C3Vector position = this->GetRawPosition();
        matrix.d0 = position.x;
        matrix.d1 = position.y;
        matrix.d2 = position.z;
        matrix *= *parentMatrix;
    } else {
        this->BuildSmoothMatrix(matrix);
    }

    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->UpdateSmoothFacing(&matrix, &facing);
        }
    }
}

// OFFSET: 0x005B8A30
bool CGUnit_C::IsDuelingWith(CGUnit_C* unit) {
    if (!unit || !(unit->m_obj->m_type & TYPEMASK_UNIT)) {
        return false;
    }

    bool dueling = false;

    if (!(this->m_unit->m_unitFieldFlags & 0x8) || !(unit->m_unit->m_unitFieldFlags & 0x8)) {
        return dueling;
    }

    CGUnit_C* controller = (this->m_obj->m_type & TYPEMASK_PLAYER) ? this : this->GetControllingPlayer();
    CGUnit_C* otherController = (unit->m_obj->m_type & TYPEMASK_PLAYER) ? unit : unit->GetControllingPlayer();

    if (controller && otherController) {
        CGPlayerLocalData* player = static_cast<CGPlayer_C*>(controller)->m_player;
        uint32_t team = player->m_playerDuelTeam;

        if (team) {
            CGPlayerLocalData* otherPlayer = static_cast<CGPlayer_C*>(otherController)->m_player;
            uint32_t otherTeam = otherPlayer->m_playerDuelTeam;

            if (otherTeam && player->m_playerDuelArbiter == otherPlayer->m_playerDuelArbiter && team != otherTeam) {
                dueling = true;
            }
        }

        return dueling;
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !otherController) {
        if (unit->GetCharmerOrCreatorGuid()->m_guid && *unit->GetCharmerOrCreatorGuid() == CGDuelInfo::m_dueler) {
            dueling = true;
        }
    } else if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !controller) {
        if (this->GetCharmerOrCreatorGuid()->m_guid && *this->GetCharmerOrCreatorGuid() == CGDuelInfo::m_dueler) {
            dueling = true;
        }
    }

    return dueling;
}

// OFFSET: #0x007160B0
void CSmoothAngle::Chase(const float* target, float rate, float elapsed) {
    float k = rate * elapsed;
    float damping = 1.0f / (k + (k * k * k * 0.235f + k * k * 0.48f) + 1.0f);
    float offset = this->m_value - *target;
    float change = elapsed * (offset * rate + this->m_velocity);
    this->m_value = (offset + change) * damping + *target;
    this->m_velocity = (this->m_velocity - rate * change) * damping;
}

// OFFSET: 0x005BE260/0x140233740
void CGUnit_C::UpdateSmoothFacing(const C44Matrix* matrix, float* facingOffset) {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && (passenger->m_flags & 0x2000) && matrix && facingOffset) {
        C3Vector position = this->m_movementData.m_position * *matrix;
        float facing = passenger->ComputeTrainCarRawFacing(position, *facingOffset);
        this->m_movementData.m_facing = facing;
        this->m_smoothTarget.m_value = facing;
        this->m_smoothTarget.m_velocity = 0.0f;

        if (this->IsVehicle()) {
            this->UpdatePassengerSmoothFacing(matrix, facingOffset);
        }

        return;
    }

    float facing = this->m_movementData.m_facing;

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        this->m_smoothTarget.m_value = facing;
        this->m_smoothTarget.m_velocity = 0.0f;

        if ((this->m_movementData.m_flags & 0x30) || CGInputControl::GetActive()->CameraCanTurnPlayer()) {
            this->m_animationState |= 0x1;
        } else {
            this->m_animationState &= ~0x1;
        }

        if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
            this->UpdatePassengerSmoothFacing(matrix, facingOffset);
        }

        return;
    }

    bool emoteFacing = false;
    uint32_t emoteState = this->m_unit->m_unitNpcEmotestate;

    if (emoteState) {
        EmotesRec* emote = g_emotesDB.GetRecord(emoteState);

        if (emote && !(emote->m_emoteFlags & 0x2000)) {
            emoteFacing = true;
        }
    }

    if (!(this->m_movement->m_flags & 0xF)) {
        CGUnitData* data = this->m_unit;

        if (!this->GetClientStandState() && !emoteFacing && !(data->m_unitFieldFlags & 0x40000) && !((data->m_unitFieldFlags2 >> 15) & 1)) {
            if (this->m_clientFlags & 0x1) {
                facing = this->m_scriptedFacing;
            } else if (!((data->m_unitFieldFlags >> 24) & 1) && !(this->m_obj->m_type & TYPEMASK_PLAYER) && this->CanTurnWhileCasting()) {
                bool seatAllows = true;

                if (this->HasVehicleTransport()) {
                    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (transport) {
                        VehicleSeatRec* seat = transport->GetVehicleSeatRec(this->m_movementData.m_transportSeat);

                        if (seat && !(seat->m_flags & 0x400)) {
                            seatAllows = false;
                        }
                    }
                }

                if (seatAllows) {
                    WOWGUID target;

                    if (this->IsVehicle() && this->m_vehicle->m_forceFacingGuid.m_guid) {
                        target = this->m_vehicle->m_forceFacingGuid;
                    } else {
                        if (this->m_unit->m_unitChannelSpell && (this->m_clientFlags & 0x8000) && !this->IsClientControlled()) {
                            target = this->m_unit->m_unitFieldChannelObject;
                        } else {
                            target = this->m_unit->m_unitFieldTarget;
                        }

                        if (!target.m_guid) {
                            target = WOWGUID::Make(this->GetComboPointTarget());
                        }
                    }

                    bool haveTarget = true;

                    if (!target.m_guid) {
                        if (CGGameUI::m_interactTarget != this->m_obj->m_guid) {
                            haveTarget = false;
                        } else {
                            target = ClntObjMgrGetActivePlayer();

                            if (!target.m_guid) {
                                haveTarget = false;
                            }
                        }
                    }

                    CGUnit_C* targetUnit = haveTarget ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__)) : NULL;

                    if (targetUnit) {
                        C3Vector position = this->GetPosition();
                        C3Vector targetPosition = targetUnit->GetPosition();
                        float dx = targetPosition.x - position.x;
                        float dy = targetPosition.y - position.y;

                        if (dx * dx + dy * dy < 0.0001f) {
                            facing = this->m_smoothTarget.m_value;
                        } else {
                            float angle = CalculateFacingTo(&position, &targetPosition);

                            if (facingOffset) {
                                facing = angle - *facingOffset;
                            } else if (this->HasVehicleTransport()) {
                                CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

                                if (transport) {
                                    facing = angle - transport->GetSmoothFacing();
                                }
                            } else if (this->GetTransportGUID()) {
                                facing = angle - MovementGetTransportFacing(WOWGUID::Make(this->GetTransportGUID()));
                            } else {
                                facing = angle;
                            }

                            this->ClampRawAngleToLegalFacingRange(facing);
                        }
                    }
                }
            }
        }
    }

    CreatureMovementInfoRec* movementInfo = g_creatureMovementInfoDB.GetRecord(this->m_creatureCacheEntry ? this->m_creatureCacheEntry->m_movementId : 0);
    float rate;

    if (movementInfo && (rate = movementInfo->m_smoothFacingChaseRate) > 0.00001f) {
        if (this->m_smoothTarget.m_value > facing + 3.1415927f) {
            facing = facing + 6.2831855f;
        } else if (facing - 3.1415927f > this->m_smoothTarget.m_value) {
            facing = facing - 6.2831855f;
        }

        this->m_smoothTarget.Chase(&facing, rate, CGWorldFrame::s_currentWorldFrame->m_elapsedSec);
        this->m_smoothTarget.m_value = CMath::normalizeangle0to2pi_(this->m_smoothTarget.m_value);
    } else if (((this->m_unit->m_unitFieldFlags >> 3) & 1) && !(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        if (this->m_smoothTarget.m_value > facing + 3.1415927f) {
            facing = facing + 6.2831855f;
        } else if (facing - 3.1415927f > this->m_smoothTarget.m_value) {
            facing = facing - 6.2831855f;
        }

        this->m_smoothTarget.Chase(&facing, this->IsInCombat() ? 30.0f : 20.0f, CGWorldFrame::s_currentWorldFrame->m_elapsedSec);
        this->m_smoothTarget.m_value = CMath::normalizeangle0to2pi_(this->m_smoothTarget.m_value);
    } else {
        float diff = facing - this->m_smoothTarget.m_value;

        if (diff > 3.1415927f) {
            diff = diff - 6.2831855f;
        } else if (-3.1415927f > diff) {
            diff = diff + 6.2831855f;
        }

        if (fabsf(diff) <= 0.01f) {
            this->m_turnDelta[0] = 0.0f;
            this->m_smoothTarget.m_value = facing;

            if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
                this->UpdatePassengerSmoothFacing(matrix, facingOffset);
            }

            return;
        }

        float step = diff;

        if (0.0f > diff) {
            if (this->m_turnDelta[0] > 0.0f) {
                this->m_turnDelta[0] = 0.0f;
            }
        } else if (0.0f > this->m_turnDelta[0]) {
            this->m_turnDelta[0] = 0.0f;
        }

        if (this->m_turnDelta[0] == 0.0f) {
            this->m_turnDelta[0] = diff;
            this->m_turnDelta[1] = diff;
            this->m_turnDelta[2] = diff;
            this->m_turnDelta[3] = diff;
        } else {
            memmove(&this->m_turnDelta[1], &this->m_turnDelta[0], sizeof(float) * 3);
            this->m_turnDelta[0] = diff;
            float average = (diff + this->m_turnDelta[1] + this->m_turnDelta[2] + this->m_turnDelta[3]) * 0.25f;

            if (diff > 0.0f) {
                if (!(average > diff)) {
                    step = average;
                }
            } else if (!(diff > average)) {
                step = average;
            }
        }

        this->m_smoothTarget.m_value = CMath::normalizeangle0to2pi_(step * 0.5f + this->m_smoothTarget.m_value);
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->UpdatePassengerSmoothFacing(matrix, facingOffset);
    }
}

// OFFSET: #0x0071DD30
int32_t IsMeleeOrReadyAnim(uint32_t animId) {
    if (IsMeleeAttackAnim(animId)) {
        return 1;
    }

    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (!rec) {
        return 0;
    }

    int32_t behavior = rec->m_behaviorID;

    if (behavior < 25 || behavior > 29) {
        return 0;
    }

    return 1;
}

// OFFSET: #0x0071DC70
int32_t IsSpellCastOrPrecastAnim(uint32_t animId) {
    if (IsSpellCastAnim(animId)) {
        return 1;
    }

    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (!rec) {
        return 0;
    }

    int32_t behavior = rec->m_behaviorID;

    if (behavior < 51 || behavior > 52) {
        return 0;
    }

    return 1;
}

// OFFSET: #0x0071D410
int32_t IsSpellPrecastAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 51 && behavior <= 52) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071DAE0
int32_t IsWeaponReadyAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 25 && behavior <= 29) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071D7C0
int32_t IsJumpAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 37 && (behavior <= 39 || behavior == 187)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071D2A0
int32_t IsJumpLandAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior == 39 || behavior == 187) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071DE50
int32_t IsFishingAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 133 && behavior <= 134) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005ADF70/0x140220760
int32_t AnimationDataHasFlags(uint32_t anim, uint32_t flags) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(anim);

    if (!rec) {
        return 0;
    }

    uint32_t behavior = rec->m_behaviorID;

    if (anim != behavior) {
        AnimationDataRec* behaviorRec = g_animationDataDB.GetRecord(behavior);

        if (behaviorRec) {
            rec = behaviorRec;
        }
    }

    return (flags & rec->m_flags) != 0;
}

// OFFSET: 0x005ADFE0/0x1402207E0
int32_t CGUnit_C::IsSplitbodyAnim(uint32_t anim) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(anim);

    if (!rec) {
        return 0;
    }

    uint32_t behavior = rec->m_behaviorID;

    if (anim != behavior) {
        AnimationDataRec* behaviorRec = g_animationDataDB.GetRecord(behavior);

        if (behaviorRec) {
            rec = behaviorRec;
        }
    }

    if (rec->m_id == 14) {
        uint32_t high = static_cast<uint32_t>(this->m_obj->m_guid.m_guid >> 32);

        if ((high & 0xF0F00000) != 0xF0300000 && (high & 0xF0F00000) != 0xF0500000) {
            return 1;
        }

        uint32_t entry = high & 0xFFFFF;
        return entry != 9554 && entry != 52327;
    }

    return (rec->m_flags >> 16) & 1;
}

// OFFSET: #0x004F6250
bool CGUnit_C::IsVehiclePassengerSeated() const {
    return this->m_vehiclePassenger && this->m_vehiclePassenger->m_state == 3;
}

// OFFSET: #0x005140C0
const VehicleSeatRec* CGUnit_C::GetPassengerSeatRec() const {
    if (this->m_vehiclePassenger) {
        return this->m_vehiclePassenger->m_seatRec;
    }

    return NULL;
}

// PROVEN: 98d4afc9 IDENTICAL/NONE
// OFFSET: 0x005AC1B0
void CGUnit_C::UpdateWeaponTrails(const uint32_t* a2, uint32_t a3) {
    if (this->m_sheatheState != 1) {
        return;
    }

    for (int32_t i = 0; i < 2; i++) {
        if (this->m_weaponTrails[i]) {
            WeaponTrailSetDrawTrail(this->m_weaponTrails[i], a2, a3);
        }
    }
}

// OFFSET: #0x0071E400
int32_t CGUnit_C::ApplyAttachmentAnimOverride(const M2BoneSequenceInfo* mount, const M2BoneSequenceInfo* base, const M2BoneSequenceInfo* torso, int32_t hasTorso, M2BoneSequenceInfo* mountOut, M2BoneSequenceInfo* baseOut, M2BoneSequenceInfo* torsoOut, int32_t* keepBase, int32_t* keepTorso) {
    CEffect* effect = this->m_effectList;

    while (true) {
        if (!effect) {
            return 0;
        }

        CEffect* next = effect->m_next;

        if (!(effect->m_flags & 0x800000) && effect->m_kitRec && effect->m_kitRec->m_animID > 0 && (effect->m_flags & 0x1000)) {
            break;
        }

        effect = next;
    }

    if (mount && mountOut) {
        *mountOut = *mount;

        if (mountOut->m_sequenceId != static_cast<uint32_t>(effect->m_kitRec->m_animID)) {
            mountOut->m_time = 0;
        }

        if (effect->m_flags & 0x1000000) {
            mountOut->m_sequenceId = effect->m_kitRec->m_animID;
        } else {
            mountOut->m_sequenceId = this->ResolveAnimForModel(effect->m_kitRec->m_animID, this->m_mountModel);
        }

        mountOut->m_variationIndex = 0xFFFFFFFF;
    }

    if (base && baseOut) {
        *baseOut = *base;

        if (baseOut->m_sequenceId != static_cast<uint32_t>(effect->m_kitRec->m_animID)) {
            baseOut->m_time = 0;
        }

        if (effect->m_flags & 0x1000000) {
            baseOut->m_sequenceId = effect->m_kitRec->m_animID;
        } else {
            baseOut->m_sequenceId = this->ResolveAnimForModel(effect->m_kitRec->m_animID, this->m_worldModel);
        }

        baseOut->m_variationIndex = 0xFFFFFFFF;

        if (keepBase) {
            *keepBase = 0;
        }
    }

    if (hasTorso && torso && torsoOut) {
        *torsoOut = *torso;

        if (torsoOut->m_sequenceId != static_cast<uint32_t>(effect->m_kitRec->m_animID)) {
            torsoOut->m_time = 0;
        }

        if (effect->m_flags & 0x1000000) {
            torsoOut->m_sequenceId = effect->m_kitRec->m_animID;
        } else {
            torsoOut->m_sequenceId = this->ResolveAnimForModel(effect->m_kitRec->m_animID, this->m_worldModel);
        }

        torsoOut->m_variationIndex = 0xFFFFFFFF;

        if (keepTorso) {
            *keepTorso = 0;
        }
    }

    return 1;
}

// OFFSET: #0x00737EF0
void CGUnit_C::ProcessAnimationEvent(M2BoneSequenceInfo* slot, uint32_t currentAnim, int32_t torso, uint32_t previousAnim, int32_t blend, int32_t keepVariation) {
    uint32_t anim = slot->m_sequenceId;
    uint32_t playing = anim;
    bool casting = this->m_unit->m_unitChannelSpell || (this->m_animationState & 0x400);

    if (this->m_combat.m_attacking.m_guid || casting || IsSpellCastOrPrecastAnim(previousAnim) || IsMeleeOrReadyAnim(previousAnim)) {
        AnimationDataRec* rec = g_animationDataDB.GetRecord(anim);

        if ((rec && rec->m_behaviorID >= 25 && rec->m_behaviorID <= 29) || !anim) {
            slot->m_variationIndex = 0;
        }
    }

    if (IsDeadOrDyingAnim(anim)) {
        slot->m_speed = 1.0f;
    }

    if (!(this->m_clientFlags & 0x80000)) {
        blend = 0;
    }

    if (!keepVariation && anim != 0xFFFFFFFF && slot->m_variationIndex != 0xFFFFFFFF) {
        M2SequenceInfo info;
        info.Clear();
        this->m_worldModel->GetSequenceInfo(anim, 0, info);
        playing = slot->m_sequenceId;

        if (info.m_animId != slot->m_sequenceId) {
            slot->m_variationIndex = 0;
        }
    }

    uint32_t bone = torso ? this->m_torsoKeyBone : 0xFFFFFFFF;

    if (currentAnim == playing) {
        if (!(fabsf(this->m_worldModel->GetBoneSequenceSpeed(bone) - slot->m_speed) < 2.3841858e-7f)) {
            this->SetKeyBoneSequenceSpeed(this->m_worldModel, bone, slot->m_speed, 0);
        }

        return;
    }

    if (playing == 0xFFFFFFFF || (torso && this->m_torsoKeyBone == -1)) {
        return;
    }

    this->SetKeyBoneSequence(this->m_worldModel, bone, playing, slot->m_variationIndex, slot->m_time, slot->m_speed, blend, 1, 0);

    switch (GetAnimBehaviorId(anim)) {
        case 39:
            this->m_animationState |= 0x4;
            break;
        case 121:
            this->m_animationState |= 0x80000;
            break;
        case 127:
            this->m_animationState |= 0x8;
            break;
        case 192:
        case 200:
            this->m_animationState |= 0x40000;
            break;
        case 201:
            this->m_animationState |= 0x400000;
            break;
        case 458:
        case 459:
        case 460:
            this->m_animationState |= 0x2000000;
            break;
        default:
            break;
    }

    if (IsSubmergedDeathAnim(anim) && this->m_animTier == 3 && (this->m_movementData.m_flags & 0x2201000)) {
        this->m_animationState |= 0x4000000;
    } else {
        this->m_animationState &= ~0x4000000;
    }

    if ((this->m_animationState & 0x20000) && (IsMeleeAttackAnim(anim) || IsSpellCastAnim(anim))) {
        this->m_animationState |= 0x2000;
    }
}

// OFFSET: 0x005D3A20
void CGUnit_C::UpdateHandItemAnimEffects(uint32_t anim) {
    uint32_t state = this->m_animationState;
    this->m_animationState = state & ~0x4000;
    uint32_t keepSheathed = state & 0x4000;
    AnimationDataRec* rec = g_animationDataDB.GetRecord(anim);

    if (rec && (rec->m_flags & 0x8) && !keepSheathed) {
        this->SetSheatheState(0, 1, 0);

        if (this->m_clientFlags & 0x10000) {
            this->m_clientFlags &= ~0x10000;
            this->ShowHandItemSpellEffects(0, 0);
        }

        return;
    }

    if (this->m_sheatheState == 2) {
        if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
            this->SetSheatheState(2, 1, 0);
        }

        if (!AnimationDataHasFlags(anim, 0xE0000) && rec) {
            if (rec->m_flags & 0x800) {
                this->SetSheatheState(0, 1, 0);
            } else if (rec->m_flags & 0x1000) {
                this->SetSheatheState(1, 1, 0);
            }
        }

        return;
    }

    if (this->m_castSpellId) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);

        if (spell) {
            if ((spell->m_attributes & 0x2) || (spell->m_attributesExJ & 0x4)) {
                this->SetSheatheState(2, 1, 0);
            } else if (!(spell->m_attributes & 0x40000)) {
                this->SetSheatheState(0, 1, 0);
            }
        }

        if (this->m_clientFlags & 0x10000) {
            this->m_clientFlags &= ~0x10000;
            this->ShowHandItemSpellEffects(0, 0);
        }

        return;
    }

    if (this->m_combat.m_attacking && AnimationDataHasFlags(anim, 0x200000)) {
        this->SetSheatheState(1, 1, 0);

        if (this->m_clientFlags & 0x10000) {
            this->m_clientFlags &= ~0x10000;
            this->ShowHandItemSpellEffects(0, 0);
        }

        return;
    }

    if (rec) {
        uint32_t flags = rec->m_flags;

        if ((flags & 0x800) && !keepSheathed) {
            this->SetSheatheState(0, 1, 0);
            return;
        }

        if (flags & 0x1000) {
            this->SetSheatheState(1, 1, 0);
            return;
        }
    }

    if (this->m_combat.m_attacking) {
        this->SetSheatheState(1, 1, 0);
        return;
    }

    if (this->IsActivePlayer()) {
        this->SetHandItemSpellEffectsVisible(1);
        return;
    }

    uint32_t sheatheState = this->m_unit->m_unitFieldBytes2[0];

    if (this->m_sheatheState == sheatheState) {
        this->SetHandItemSpellEffectsVisible(1);
    } else {
        this->SetSheatheState(sheatheState, 1, 0);
    }
}

// OFFSET: 0x005C6710/0x14023D8E0
int32_t CGUnit_C::ShouldRenderObjectName(uint32_t flags) const {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const WOWGUID* controllerGuid = this->m_unit->m_unitFieldCharmedby.m_guid ? &this->m_unit->m_unitFieldCharmedby : &this->m_unit->m_unitFieldCreatedby;
    CGUnit_C* controller = NULL;

    if (controllerGuid->m_guid) {
        controller = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controllerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (!this->CanBeTargetted() || (CGGameUI::m_visible && (this->m_namePlateFrame || this->m_chatBubble))) {
        return 0;
    }

    if (this->IsControlledByViewer()) {
        return flags & 0x1;
    }

    if (this->m_obj->m_guid == CGGameUI::m_lockedTarget) {
        return 1;
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        if (player->CanCooperate(this)) {
            return (flags >> 7) & 0x1;
        }

        if ((flags & 0x10) && !(this->m_unit->m_unitFieldBytes1[2] & 0x2)) {
            return 1;
        }

        return 0;
    }

    if (controller && (controller->m_obj->m_type & TYPEMASK_PLAYER)) {
        bool friendly = !player->CanAttack(this);

        if (!friendly && (this->m_unit->m_unitFieldBytes1[2] & 0x2)) {
            return 0;
        }

        if ((this->HasCreator() && !this->HasSummoner() && (this->m_unit->m_unitFieldFlags & 0x200)) || this->GetCreatureType() == 8 || this->GetCreatureType() == 12) {
            return (flags >> 10) & 0x1;
        }

        if (this->GetCreatureType() == 11) {
            return friendly ? (flags >> 9) & 0x1 : (flags >> 6) & 0x1;
        }

        if (this->GetCreatorGuid()) {
            return (flags >> 12) & 0x1;
        }

        CGUnit_C* charmer = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit->m_unitFieldCharmedby, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (player->CanCooperate(this)) {
            return (flags >> 8) & 0x1;
        }

        if (charmer && charmer->GetTransportGUID() == this->m_obj->m_guid.m_guid && (player == charmer || player->CanCooperate(charmer))) {
            return (flags >> 8) & 0x1;
        }

        return (flags >> 5) & 0x1;
    }

    if ((this->m_unit->m_unitFieldBytes1[2] & 0x2) && player->CanAttack(this)) {
        return 0;
    }

    if (this->GetCreatureType() == 8 || this->GetCreatureType() == 12) {
        return (flags >> 10) & 0x1;
    }

    return (flags >> 1) & 0x1;
}

// OFFSET: 0x005B3BE0/0x140227160
bool CGUnit_C::CanAutoStand() const {
    return this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && g_autoStandCVar->m_intValue;
}

// OFFSET: 0x005BB6F0/0x14022FFC0
bool CGUnit_C::CanAutoInteract() const {
    return this->m_unit->m_unitFieldHealth > 0 && this->m_obj->m_guid == CGUnit_C::s_activeMover && g_cvAutoInteract->m_intValue;
}

// OFFSET: 0x005BA0C0
int32_t CGUnit_C::IsLowPrioritySelection(uint32_t time) {
    if (this->m_unit->m_unitFieldFlags & 0x2000000) {
        return 1;
    }

    if (this->m_obj->m_type != (TYPEMASK_OBJECT | TYPEMASK_UNIT) || this->m_unit->m_unitFieldHealth > 0) {
        return 0;
    }

    if (this->CanBeLooted(time) && ClntObjMgrGetActivePlayerObj()->CanLoot(this)) {
        return 0;
    }

    if ((this->m_unit->m_unitFieldFlags & 0x4000000) && CGSpellBook::GetSkinningSpell(this)) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x005B4400
int32_t CGUnit_C::GetCursorOverride(int32_t unable) {
    int32_t mode = this->GetQuestGiverCursor(unable);

    if (mode) {
        return mode;
    }

    CreatureStats_C* stats = this->m_creatureCacheEntry;

    if (!stats || !stats->m_iconName || !*stats->m_iconName) {
        return 0;
    }

    char name[40];

    if (unable) {
        SStrPrintf(name, sizeof(name), "Unable%s", stats->m_iconName);
    } else {
        SStrCopy(name, stats->m_iconName, sizeof(name));
    }

    mode = CursorGetModeFromName(name);
    return mode ? mode : 53;
}

// OFFSET: 0x005C1D20/0x140237DE0
bool CGUnit_C::CanAutoUnshift() const {
    if (this->m_unit->m_unitFieldFlags & 0x100000) {
        return false;
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !g_autoUnshiftCVar->m_intValue) {
        return false;
    }

    if ((this->m_movement->m_flags & MOVEMENTFLAG_FLYING) && !g_autoDismountFlyingCVar->m_intValue) {
        return false;
    }

    SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());
    return !form || !(form->m_flags & 0x100);
}

// OFFSET: 0x005B6140/0x140229AE0
int32_t CGUnit_C::IsInPartyOrRaid() const {
    if (!(this->m_unit->m_unitNpcFlags & 0x1000000) || (this->m_unit->m_unitFieldFlags2 & 0x2000)) {
        return 0;
    }

    if (!(this->m_unit->m_unitFieldFlags2 & 0x1000)) {
        return 1;
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (CGPartyInfo::IsPlayerOrPetInParty(guid)) {
        return 1;
    }

    WOWGUID raidGuid = this->m_obj->m_guid;
    return CGRaidInfo::IsPlayerOrPetInRaid(raidGuid) ? 1 : 0;
}

// OFFSET: 0x005BCBD0/0x140231770
bool CGUnit_C::CanBeSpellClickedByLocalPlayer(const CGPlayer_C* player) const {
    if (!(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        return false;
    }

    if (!this->m_vehicle || !this->m_vehicle->m_vehicleRec || !this->m_vehicle->m_freeSeatMask) {
        return false;
    }

    if (player->GetTransportGUID() == this->m_obj->m_guid.m_guid) {
        return false;
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (CGPartyInfo::IsPlayerOrPetInParty(guid)) {
        return true;
    }

    WOWGUID raidGuid = this->m_obj->m_guid;
    return CGRaidInfo::IsPlayerOrPetInRaid(raidGuid) != 0;
}

// OFFSET: 0x005C5FE0
bool CGUnit_C::CanInteractNow(const CGUnit_C* unit) const {
    CGUnitData* data = this->m_unit;

    if ((data->m_unitFieldFlags & 0x100000) || data->m_unitFieldCharmedby || data->m_unitFieldHealth <= 0) {
        return false;
    }

    if (this->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (!(form && (form->m_flags & 0x8)) && !this->CanAutoUnshift()) {
            return false;
        }
    }

    CGUnitData* target = unit->m_unit;

    if (target->m_unitFieldHealth <= 0 && !(unit->m_creatureCacheEntry && (unit->m_creatureCacheEntry->m_typeFlags[0] & 0x80))) {
        return false;
    }

    if (target->m_unitFieldCharmedby && !unit->IsVehicle()) {
        return false;
    }

    CreatureStats_C* stats = unit->m_creatureCacheEntry;

    if (unit->IsInCombat() && !(stats && (stats->m_typeFlags[0] & 0x40000)) && !(this->m_unit->m_unitFieldFlags2 & 0x4000)) {
        return false;
    }

    if (((target->m_unitNpcFlags & 0x1000000) && !unit->IsInPartyOrRaid()) || this->IsAlteredFormTransitionPreventingAction()) {
        return false;
    }

    return static_cast<const CGPlayer_C*>(this)->CanInteract(unit);
}

// OFFSET: 0x005D1DF0/0x14024B810
void CGUnit_C::OnRightClick(bool interactive) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if (player->m_unit->m_unitFieldHealth <= 0 && !player->GetPossessedUnit()) {
        return;
    }

    bool vehicleAllows = false;

    if (!interactive || !(player->m_combatModeFlags & 0x1) || player->GetFarSightGuid() == player->GetTransportGUID()) {
        vehicleAllows = player->VehicleAllowsInteraction();

        if (vehicleAllows) {
            CGUnitData* data = this->m_unit;

            if (data->m_unitFieldHealth <= 0 && !((data->m_unitDynamicFlags >> 5) & 1) && player->IsActiveMover()) {
                if (this->CanBeLooted(FrameTime::GetCurTimeMs())) {
                    if (player->GetClientStandState()) {
                        if (!player->CanAutoStand()) {
                            CGGameUI::DisplayError(ERR_LOOT_NOTSTANDING);
                            return;
                        }

                        player->ChangeStandState(0);
                    }

                    player->SetAutoLoot(CGGameUI::IsAutoLooting());
                    player->LootUnit(this, player->CanAutoInteract());
                    return;
                }

                if (!interactive || !((data->m_unitFieldFlags >> 26) & 1) || !CGSpellBook::GetSkinningSpell(this)) {
                    return;
                }

                if (this->m_obj->m_type & TYPEMASK_PLAYER) {
                    if (!player->CanAssist(this, false)) {
                        player->HandleSkinning(this, player->CanAutoInteract(), true);
                    }

                    return;
                }

                int32_t points = 0;
                const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetSkinningSpell(this));

                if (!spell) {
                    return;
                }

                uint32_t count;
                const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

                for (uint32_t i = 0; i < count; i++) {
                    const SpellEffectRec* effect = effects->m_data[i];

                    if (effect && effect->m_effect == 95) {
                        int32_t minPoints;
                        Spell_C_GetMinMaxPoints(spell, i, &minPoints, &points, 0, 0, 0, 0);
                        points = minPoints;
                        break;
                    }
                }

                int32_t required = GetSkinningDifficulty(this->m_unit->m_unitFieldLevel);

                if (points < required) {
                    CGGameUI::DisplayError(ERR_USE_LOCKED_WITH_SPELL_KNOWN_SI, spell->m_name, required);
                    return;
                }

                player->HandleSkinning(this, player->CanAutoInteract(), true);
                return;
            }

            if (player->CanInteract(this)) {
                player->HandleOnRightClick(this, player->CanAutoInteract());
                return;
            }
        }

        WOWGUID target = this->m_obj->m_guid;

        if (!interactive || !player->CanAttackNow(this) || !player->VerifyAttackIconPressed(&target, 1, 0, false, true)) {
            if (vehicleAllows) {
                this->HandleNPCFeedback();
            }

            return;
        }

        target = this->m_obj->m_guid;
        player->CombatModeEnter(target, 0, player->CanAutoInteract(), 0, 0);
        CGPlayer_C* mover = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (mover && mover->m_obj->m_guid == player->m_obj->m_guid && !mover->IsInVehicle()) {
            mover->CTMAttackUnit(this);
        }
    }

    CGUnit_C* possessed = player->GetPossessedUnit();

    if (this->m_unit->m_unitFieldHealth > 0 && possessed && possessed->CanAttackNow(this)) {
        CGPetInfo::PetAttackTarget(this->m_obj->m_guid, 1);
    }
}

// OFFSET: 0x005D4E20/0x14024F2B0
void CGUnit_C::OnRightClick() {
    this->OnRightClick(true);
}

// OFFSET: 0x005ACE50/0x14021F220
static void CacheEmoteRecords() {
    int32_t index = g_emotesDB.m_numRecords;

    while (index--) {
        EmotesRec* rec = g_emotesDB.GetRecordByIndex(index);

        if (rec->m_emoteFlags & 0x8) {
            s_talkEmotes[0] = rec;
        } else if (rec->m_emoteFlags & 0x10) {
            s_talkEmotes[1] = rec;
        } else if (rec->m_emoteFlags & 0x20) {
            s_talkEmotes[2] = rec;
        } else if (rec->m_emoteFlags & 0x40) {
            s_talkEmotes[3] = rec;
        } else if (rec->m_emoteFlags & 0x100) {
            s_talkEmotes[4] = rec;
        }
    }
}

// OFFSET: 0x005DA150/0x140255C80
void CGUnit_C::RegisterMirrorHandlers() {
    for (uint32_t offset = offsetof(CGUnitData, m_unitVirtualItemSlotId); offset < offsetof(CGUnitData, m_unitVirtualItemSlotId) + sizeof(((CGUnitData*)0)->m_unitVirtualItemSlotId); offset += sizeof(uint32_t)) {
        ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offset, sizeof(uint32_t), VirtualItemIDMirrorHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldLevel), sizeof(uint32_t), UnitLevelUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_entryID), sizeof(uint32_t), UnitModeUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), sizeof(int32_t), UnitHealthUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);

    for (uint32_t offset = offsetof(CGUnitData, m_unitFieldMaxpower); offset < offsetof(CGUnitData, m_unitFieldMaxpower) + sizeof(((CGUnitData*)0)->m_unitFieldMaxpower); offset += sizeof(uint32_t)) {
        ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offset - (offsetof(CGUnitData, m_unitFieldMaxpower) - offsetof(CGUnitData, m_unitFieldPower)), sizeof(uint32_t), UnitPowerUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
        ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offset, sizeof(uint32_t), UnitMaxPowerUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes0) + 3, sizeof(uint8_t), UnitDisplayPowerUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldAurastate), sizeof(uint32_t), AuraStateUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldFlags), sizeof(uint32_t), UnitFlagUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldFlags2), sizeof(uint32_t), UnitFlag2UpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes1) + 2, sizeof(uint8_t), UnitVisFlagUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes2) + 1, sizeof(uint8_t), UnitPvPFlagUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes1) + 3, sizeof(uint8_t), AnimTierChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldMountdisplayid), sizeof(uint32_t), MountDisplayIDUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldFactiontemplate), sizeof(uint32_t), UnitFactionUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldCharmedby), 2 * sizeof(WOWGUID), UnitCharmedUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldDisplayid), sizeof(uint32_t), DisplayIDUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(1), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes1), sizeof(uint8_t), StandStateUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitNpcFlags), sizeof(uint32_t), NPCFlagsHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitNpcEmotestate), sizeof(uint32_t), EmoteStateUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldPetNameTimestamp), sizeof(uint32_t), PetNameChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitDynamicFlags), sizeof(uint32_t), DynamicFlagsChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldChannelObject), sizeof(WOWGUID) + sizeof(uint32_t), ChannelSpellOrObjectChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 1);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldPetnumber), sizeof(uint32_t), PetNumberChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_scale), sizeof(float), UnitScaleUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes2), sizeof(uint8_t), SheatheStateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldTarget), sizeof(WOWGUID), TargetChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldHoverheight), sizeof(float), HoverHeightChangeHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x005DA450
void CGUnit_C::UnregisterMirrorHandlers() {
    for (uint32_t offset = offsetof(CGUnitData, m_unitVirtualItemSlotId); offset < offsetof(CGUnitData, m_unitVirtualItemSlotId) + sizeof(((CGUnitData*)0)->m_unitVirtualItemSlotId); offset += sizeof(uint32_t)) {
        ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offset, VirtualItemIDMirrorHandler, NULL);
    }

    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldLevel), UnitLevelUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_entryID), UnitModeUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), UnitHealthUpdateHandler, NULL);

    for (uint32_t offset = offsetof(CGUnitData, m_unitFieldMaxpower); offset < offsetof(CGUnitData, m_unitFieldMaxpower) + sizeof(((CGUnitData*)0)->m_unitFieldMaxpower); offset += sizeof(uint32_t)) {
        ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offset - (offsetof(CGUnitData, m_unitFieldMaxpower) - offsetof(CGUnitData, m_unitFieldPower)), UnitPowerUpdateHandler, NULL);
        ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offset, UnitMaxPowerUpdateHandler, NULL);
    }

    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes0) + 3, UnitDisplayPowerUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldAurastate), AuraStateUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldFlags), UnitFlagUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldFlags2), UnitFlag2UpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes1) + 2, UnitVisFlagUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes2) + 1, UnitPvPFlagUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes1) + 3, AnimTierChangeHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldMountdisplayid), MountDisplayIDUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldFactiontemplate), UnitFactionUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldCharmedby), UnitCharmedUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldDisplayid), DisplayIDUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldBytes1), StandStateUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitNpcFlags), NPCFlagsHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitNpcEmotestate), EmoteStateUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldPetNameTimestamp), PetNameChangeHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitDynamicFlags), DynamicFlagsChangeHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldChannelObject), ChannelSpellOrObjectChangeHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldPetnumber), PetNumberChangeHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_scale), UnitScaleUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, offsetof(CGUnitData, m_unitFieldBytes2), SheatheStateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_UNIT, offsetof(CGUnitData, m_unitFieldHoverheight), HoverHeightChangeHandler, NULL);
}

// PROVEN: fd861d1f IDENTICAL/NONE
// OFFSET: 0x005AC190
void CGUnit_C::OnStopFlying() {
    this->PlayUnitSound(12, 1, 0);
    this->UpdateMovementAnimKit(0);
}

// OFFSET: 0x005AFE10
int32_t CGUnit_C::OnTeleportUpdate(uint32_t time, const CMovementStatus& status) {
    if (this->m_vehiclePassenger && (status.m_moveExtraFlags & 0x200)) {
        if (status.m_hasTransport) {
            this->VehiclePassengerStartWaitingForVehicleTransitionAnim(status.m_transport.m_guid.m_guid, status.m_transport.m_seat);
        } else {
            this->VehiclePassengerStartWaitingForVehicleTransitionAnim(0, 0xFF);
        }
    }

    return this->m_movementData.OnTeleport(time, status);
}

// PROVEN: 3a13cf4a IDENTICAL/NONE
// OFFSET: 0x005BB310
void CGUnit_C::OnStartFlying() {
    this->ResetSmoothFacing();

    if (this->GetCurrentTorsoAnimId() == ANIM_FALL || this->GetCurrentTorsoAnimId() == 648 || this->GetCurrentMountAnimId() == ANIM_FALL) {
        this->UpdateMovementAnimKit(0);
        return;
    }

    this->PlayUnitSound(11, 1, 0);

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec && this->m_vehicle->HasHighPriorityAnim()) {
        this->UpdateMovementAnimKit(0);
        return;
    }

    const UnitAnimationSet* set = this->SelectAnimationSet();
    this->PlayMovementAnimKit(6, set->m_jumpStart, 0xFFFFFFFF, 0);
}

// OFFSET: 0x005CD980
void CGUnit_C::OnMovementFlagsChanged(uint32_t time, uint32_t oldFlags, int32_t teleport, int32_t a4, int32_t knockBack) {
    uint32_t flags = this->m_movement->m_flags;
    uint32_t added = flags & ~oldFlags;
    uint32_t changed = oldFlags ^ flags;
    uint32_t removed = oldFlags & ~flags;
    int32_t jumped = (added & MOVEMENTFLAG_FALLING) && this->m_movementData.m_fallVelocity != 0.0f;

    if (added & 0x60080F) {
        this->CancelRangedMode();

        if ((added & 0x60000F) && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
            static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
        }
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        if (changed & 0x1100400) {
            CGInputControl::GetActive()->UpdatePlayer(time, 1);
        }

        if (removed & 0x60080F) {
            this->HandlePendingTrackEvents();
        }

        if (teleport) {
            CGInputControl::GetActive()->UpdateHalted();
            CGInputControl::GetActive()->UpdatePlayer(time, 1);
            this->m_nextBreathCheckTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();

            if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13) {
                this->ClearTrackingTarget(false, true);
            }
        }
    }

    if (knockBack) {
        const UnitAnimationSet* set = this->SelectAnimationSet();
        this->PlayMovementAnimKit(2, set->m_fall, 0xFFFFFFFF, 0);
    } else if (jumped) {
        this->PlayUnitSound(11, 1, 0);
        const UnitAnimationSet* set = this->SelectAnimationSet();
        this->PlayMovementAnimKit(3, set->m_jump, set->m_jumpStart, 0);
    } else if (added & MOVEMENTFLAG_FLYING) {
        this->OnStartFlying();
    } else if (removed & MOVEMENTFLAG_FLYING) {
        this->OnStopFlying();
    } else if (!teleport && !(changed & 0x3110073F) && (!a4 || !(this->m_movement->m_flags & 0x60080F))) {
        if (added & MOVEMENTFLAG_FALLING) {
            this->OnCollideFalling(oldFlags);
        }
    } else {
        this->UpdateMovementAnimKit(0);
    }

    if (teleport || (changed & 0x3170063F)) {
        this->UpdateDeathAnimKit();
    }
}

// PROVEN: ccbb6c15 IDENTICAL/NONE
// OFFSET: 0x005C2AD0
void CGUnit_C::SetAuraEffectAmount(uint32_t auraIndex, uint32_t effectIndex, int32_t amount) {
    if (auraIndex < this->m_auras.Count() && effectIndex < 3) {
        this->m_auras[auraIndex].m_effectAmounts[effectIndex] = amount;
    }
}

// OFFSET: 0x005B7A20
static bool ClientMoveUpdateRunSpeedHandler(ClientMoveUpdateRunSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnRunSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnRunSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7AB0
static bool ClientMoveUpdateRunBackSpeedHandler(ClientMoveUpdateRunBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnRunBackSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnRunBackSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7B30
static bool ClientMoveUpdateWalkSpeedHandler(ClientMoveUpdateWalkSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnWalkSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnWalkSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7BB0
static bool ClientMoveUpdateSwimSpeedHandler(ClientMoveUpdateSwimSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnSwimSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnSwimSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7C40
static bool ClientMoveUpdateSwimBackSpeedHandler(ClientMoveUpdateSwimBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnSwimBackSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnSwimBackSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7CD0
static bool ClientMoveUpdateFlightSpeedHandler(ClientMoveUpdateFlightSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnFlightSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnFlightSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7D60
static bool ClientMoveUpdateFlightBackSpeedHandler(ClientMoveUpdateFlightBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnFlightBackSpeedChange(time, msg->m_speed);
            return true;
        }

        unit->m_movementData.OnFlightBackSpeedChange(time, msg->m_status, msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005B7DF0
static bool ClientMoveUpdateTurnRateHandler(ClientMoveUpdateTurnRate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnTurnRateChange(time, msg->m_rate);
            return true;
        }

        unit->m_movementData.OnTurnRateChange(time, msg->m_status, msg->m_rate);
    }

    return true;
}

// OFFSET: 0x005B7E70
static bool ClientMoveUpdatePitchRateHandler(ClientMoveUpdatePitchRate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnPitchRateChange(time, msg->m_rate);
            return true;
        }

        unit->m_movementData.OnPitchRateChange(time, msg->m_status, msg->m_rate);
    }

    return true;
}

// OFFSET: 0x005B7EF0
static bool ClientMoveUpdateCollisionHeightHandler(ClientMoveUpdateCollisionHeight* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();

        if (unit->m_obj->m_guid == CGUnit_C::s_activeMover) {
            unit->m_movementData.OnSetCollisionHeight(time, msg->m_height);
            return true;
        }

        unit->m_movementData.OnSetCollisionHeight(time, msg->m_status, msg->m_height);
    }

    return true;
}

// OFFSET: 0x005D0640
static bool ClientMoveUpdateHandler(ClientMoveUpdate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();
        int32_t root;

        if (!(unit->m_movement->m_flags & MOVEMENTFLAG_ROOT) || (msg->m_status.m_moveFlags & MOVEMENTFLAG_ROOT)) {
            root = 0;
        } else {
            unit->m_clientFlags |= 0x20000000;
            root = 1;
        }

        uint32_t oldFlags = unit->m_movement->m_flags;

        if (unit->m_movementData.OnHeartBeat(time, msg->m_status)) {
            unit->OnMovementFlagsChanged(time, oldFlags, 0, 0, 0);
        }

        if (root) {
            unit->m_clientFlags &= ~0x20000000;
        }
    }

    return true;
}

// OFFSET: 0x005D06F0
static bool ClientMoveUpdateTeleportHandler(ClientMoveUpdateTeleport* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();
        uint32_t oldFlags = unit->m_movement->m_flags;

        if (unit->OnTeleportUpdate(time, msg->m_status)) {
            unit->OnMovementFlagsChanged(time, oldFlags, 1, 0, 0);
        }
    }

    return true;
}

// OFFSET: 0x005D0760
static bool ClientMoveUpdateKnockBackHandler(ClientMoveUpdateKnockBack* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_status.m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t time = FrameTime::GetCurTimeMs();
        uint32_t oldFlags = unit->m_movement->m_flags;

        if (unit->m_movementData.OnLaunch(time, msg->m_status)) {
            unit->OnMovementFlagsChanged(time, oldFlags, 0, 0, 1);
        }
    }

    return true;
}

// OFFSET: 0x005AF000
static bool ClientSplineMoveSetRunSpeedHandler(ClientSplineMoveSetRunSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnRunSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF040
static bool ClientSplineMoveSetRunBackSpeedHandler(ClientSplineMoveSetRunBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnRunBackSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF080
static bool ClientSplineMoveSetSwimSpeedHandler(ClientSplineMoveSetSwimSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSwimSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF0C0
static bool ClientSplineMoveSetSwimBackSpeedHandler(ClientSplineMoveSetSwimBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSwimBackSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF100
static bool ClientSplineMoveSetFlightSpeedHandler(ClientSplineMoveSetFlightSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnFlightSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF140
static bool ClientSplineMoveSetFlightBackSpeedHandler(ClientSplineMoveSetFlightBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnFlightBackSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF180
static bool ClientSplineMoveSetWalkSpeedHandler(ClientSplineMoveSetWalkSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnWalkSpeedChange(msg->m_speed);
    }

    return true;
}

// OFFSET: 0x005AF1C0
static bool ClientSplineMoveSetTurnRateHandler(ClientSplineMoveSetTurnRate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnTurnRateChange(msg->m_rate);
    }

    return true;
}

// OFFSET: 0x005AF200
static bool ClientSplineMoveSetPitchRateHandler(ClientSplineMoveSetPitchRate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnPitchRateChange(msg->m_rate);
    }

    return true;
}

// PROVEN: c774f946 IDENTICAL/NONE
// OFFSET: 0x005D07D0
static bool ClientSplineMoveRootHandler(ClientSplineMoveRoot* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnRoot();
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 6af9a8d7 IDENTICAL/NONE
// OFFSET: 0x005D0840
static bool ClientSplineMoveUnrootHandler(ClientSplineMoveUnroot* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnUnRoot();
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 63d35215 IDENTICAL/NONE
// OFFSET: 0x005D08B0
static bool ClientSplineMoveGravityDisableHandler(ClientSplineMoveGravityDisable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnSplineEnableGravity(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 651d41f9 IDENTICAL/NONE
// OFFSET: 0x005D0920
static bool ClientSplineMoveGravityEnableHandler(ClientSplineMoveGravityEnable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnSplineEnableGravity(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 7a8e506d IDENTICAL/NONE
// OFFSET: 0x005D0990
static bool ClientSplineMoveCollisionDisableHandler(ClientSplineMoveCollisionDisable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.EnableCollision(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 252d31a2 IDENTICAL/NONE
// OFFSET: 0x005D0A00
static bool ClientSplineMoveCollisionEnableHandler(ClientSplineMoveCollisionEnable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.EnableCollision(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 601e57c3 IDENTICAL/NONE
// OFFSET: 0x005D0A70
static bool ClientSplineMoveSetFeatherFallHandler(ClientSplineMoveSetFeatherFall* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnFeatherFall(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 8af139c5 IDENTICAL/NONE
// OFFSET: 0x005D0AE0
static bool ClientSplineMoveSetNormalFallHandler(ClientSplineMoveSetNormalFall* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnFeatherFall(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: b83d603d IDENTICAL/NONE
// OFFSET: 0x005D0B50
static bool ClientSplineMoveSetHoverHandler(ClientSplineMoveSetHover* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnHover(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 6dc7e8ee IDENTICAL/NONE
// OFFSET: 0x005D0BC0
static bool ClientSplineMoveUnsetHoverHandler(ClientSplineMoveUnsetHover* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnHover(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 5a033751 IDENTICAL/NONE
// OFFSET: 0x005D0C30
static bool ClientSplineMoveSetWaterWalkHandler(ClientSplineMoveSetWaterWalk* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnWaterWalk(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 1a626143 IDENTICAL/NONE
// OFFSET: 0x005D0CA0
static bool ClientSplineMoveSetLandWalkHandler(ClientSplineMoveSetLandWalk* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnWaterWalk(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 86ae1e4e IDENTICAL/NONE
// OFFSET: 0x005D0D10
static bool ClientSplineMoveStartSwimHandler(ClientSplineMoveStartSwim* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnSplineStartSwim();
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 0c813382 IDENTICAL/NONE
// OFFSET: 0x005D0D80
static bool ClientSplineMoveStopSwimHandler(ClientSplineMoveStopSwim* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnSplineStopSwim();
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 03dd9046 IDENTICAL/NONE
// OFFSET: 0x005D0DF0
static bool ClientSplineMoveSetRunModeHandler(ClientSplineMoveSetRunMode* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnSetRunMode(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: a6bf780c IDENTICAL/NONE
// OFFSET: 0x005D0E60
static bool ClientSplineMoveSetWalkModeHandler(ClientSplineMoveSetWalkMode* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.OnSetRunMode(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: bb35eda8 IDENTICAL/NONE
// OFFSET: 0x005D0ED0
static bool ClientSplineMoveSetFlyingHandler(ClientSplineMoveSetFlying* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.SplineToggleEnableFlying(1);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: 336d6046 IDENTICAL/NONE
// OFFSET: 0x005D0F40
static bool ClientSplineMoveUnsetFlyingHandler(ClientSplineMoveUnsetFlying* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t oldFlags = unit->m_movement->m_flags;
        unit->m_movementData.FlushMoveQueue(0, 0);
        unit->m_movementData.SplineToggleEnableFlying(0);
        unit->OnMovementFlagsChanged(FrameTime::GetCurTimeMs(), oldFlags, 0, 0, 0);
    }

    return true;
}

// PROVEN: bd7fc221 IDENTICAL/NONE
// OFFSET: 0x005AF280
static bool ClientMoveSetRunSpeedHandler(ClientMoveSetRunSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnRunSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: b99b959f IDENTICAL/NONE
// OFFSET: 0x005AF2D0
static bool ClientMoveSetRunBackSpeedHandler(ClientMoveSetRunBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnRunBackSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: b4c2a2a9 IDENTICAL/NONE
// OFFSET: 0x005AF320
static bool ClientMoveSetSwimSpeedHandler(ClientMoveSetSwimSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSwimSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: c3b86c40 IDENTICAL/NONE
// OFFSET: 0x005AF370
static bool ClientMoveSetSwimBackSpeedHandler(ClientMoveSetSwimBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSwimBackSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: 12704cc0 IDENTICAL/NONE
// OFFSET: 0x005AF3C0
static bool ClientMoveSetFlightSpeedHandler(ClientMoveSetFlightSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnFlightSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: b7a1eb7b IDENTICAL/NONE
// OFFSET: 0x005AF410
static bool ClientMoveSetFlightBackSpeedHandler(ClientMoveSetFlightBackSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnFlightBackSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: 9651f1e5 IDENTICAL/NONE
// OFFSET: 0x005AF460
static bool ClientMoveSetWalkSpeedHandler(ClientMoveSetWalkSpeed* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnWalkSpeedChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_speed);
    }

    return true;
}

// PROVEN: ee84fa16 IDENTICAL/NONE
// OFFSET: 0x005AF4B0
static bool ClientMoveSetTurnRateHandler(ClientMoveSetTurnRate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnTurnRateChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_rate);
    }

    return true;
}

// PROVEN: 98f1f876 IDENTICAL/NONE
// OFFSET: 0x005AF500
static bool ClientMoveSetPitchRateHandler(ClientMoveSetPitchRate* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnPitchRateChangeLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_rate);
    }

    return true;
}

// PROVEN: af7c5942 IDENTICAL/NONE
// OFFSET: 0x005AF550
static bool ClientMoveRootHandler(ClientMoveRoot* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnRootLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: 9067b9c8 IDENTICAL/NONE
// OFFSET: 0x005AF5A0
static bool ClientMoveUnrootHandler(ClientMoveUnroot* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnRootLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: 16bd7159 IDENTICAL/NONE
// OFFSET: 0x005AF5F0
static bool ClientMoveSetWaterWalkHandler(ClientMoveSetWaterWalk* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnWaterWalkLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: 93a1f1ba IDENTICAL/NONE
// OFFSET: 0x005AF640
static bool ClientMoveSetLandWalkHandler(ClientMoveSetLandWalk* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnWaterWalkLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: 6dc07c63 IDENTICAL/NONE
// OFFSET: 0x005AF690
static bool ClientMoveSetFeatherFallHandler(ClientMoveSetFeatherFall* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnFeatherFallLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: c6690961 IDENTICAL/NONE
// OFFSET: 0x005AF6E0
static bool ClientMoveSetNormalFallHandler(ClientMoveSetNormalFall* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnFeatherFallLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: a0bb37fe IDENTICAL/NONE
// OFFSET: 0x005AF730
static bool ClientMoveSetHoverHandler(ClientMoveSetHover* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnHoverLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: b318231b IDENTICAL/NONE
// OFFSET: 0x005AF780
static bool ClientMoveUnsetHoverHandler(ClientMoveUnsetHover* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnHoverLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: f6a05696 IDENTICAL/NONE
// OFFSET: 0x005AF7D0
static bool ClientMoveKnockBackHandler(ClientMoveKnockBack* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnLaunchLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_direction, msg->m_horizontalSpeed, msg->m_verticalSpeed);
    }

    return true;
}

// PROVEN: 7c72a22e IDENTICAL/NONE
// OFFSET: 0x005AF830
static bool ClientMoveSetCanFlyHandler(ClientMoveSetCanFly* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnCanFlyLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: aec22db8 IDENTICAL/NONE
// OFFSET: 0x005AF880
static bool ClientMoveUnsetCanFlyHandler(ClientMoveUnsetCanFly* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnCanFlyLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: aca37df4 IDENTICAL/NONE
// OFFSET: 0x005AF8D0
static bool ClientMoveSetCanTransitionBetweenSwimAndFlyHandler(ClientMoveSetCanTransitionBetweenSwimAndFly* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnCanTransitionBetweenSwimAndFlyLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: 7180eac4 IDENTICAL/NONE
// OFFSET: 0x005AF920
static bool ClientMoveUnsetCanTransitionBetweenSwimAndFlyHandler(ClientMoveUnsetCanTransitionBetweenSwimAndFly* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnCanTransitionBetweenSwimAndFlyLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: fd20b778 IDENTICAL/NONE
// OFFSET: 0x005AF970
static bool ClientMoveGravityDisableHandler(ClientMoveGravityDisable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnDisableGravityLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// PROVEN: 9a0e14d7 IDENTICAL/NONE
// OFFSET: 0x005AF9C0
static bool ClientMoveGravityEnableHandler(ClientMoveGravityEnable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnDisableGravityLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: 8a96c69e IDENTICAL/NONE
// OFFSET: 0x005AFA10
static bool ClientMoveCollisionDisableHandler(ClientMoveCollisionDisable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnEnableCollisionLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 0);
    }

    return true;
}

// PROVEN: fd083b5a IDENTICAL/NONE
// OFFSET: 0x005AFA60
static bool ClientMoveCollisionEnableHandler(ClientMoveCollisionEnable* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnEnableCollisionLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, 1);
    }

    return true;
}

// OFFSET: 0x005AFAB0
static bool ClientMoveSetCollisionHeightHandler(ClientMoveSetCollisionHeight* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSetCollisionHeightLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_height, msg->m_reason);
    }

    return true;
}

// PROVEN: 00a0c21b IDENTICAL/NONE
// OFFSET: 0x005AFB00
static bool ClientMoveSetVehicleRecIdHandler(ClientMoveSetVehicleRecId* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSetVehicleRecIdLocal(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_vehicleRecId);
    }

    return true;
}

// PROVEN: d3141a07 IDENTICAL/NONE
// OFFSET: 0x005AFB50
static bool ClientMoveSkipTimeHandler(ClientMoveSkipTime* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.OnSkipTime(msg->m_time);
    }

    return true;
}

// OFFSET: 0x005ADB00
static bool ClientSuppressNPCGreetingsHandler(ClientSuppressNPCGreetings* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return true;
    }

    bool suppressed = msg->m_suppressed != 0;
    unit->SetFootstepSoundsSuppressed(suppressed);

    if (suppressed) {
        s_npcFeedbackSuppressCount++;
    } else {
        s_npcFeedbackSuppressCount--;
    }

    return true;
}

// PROVEN: 654d2276 IDENTICAL/NONE
// OFFSET: 0x005C4340
static bool ClientAuraPointsDepletedHandler(ClientAuraPointsDepleted* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->SetAuraEffectAmount(msg->m_slot, msg->m_effectIndex, 0);
    }

    return true;
}

// OFFSET: 0x005CC200
static bool ClientMoveTeleportHandler(ClientMoveTeleport* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return true;
    }

    if (ClntObjMgrGetPlayerType()) {
        PlayerMoveTeleportAck ack;
        ack.m_time = FrameTime::GetCurTimeMs();
        ack.m_guid = msg->m_guid;
        ack.m_ackIndex = msg->m_ackIndex;
        ClientServices::Send(&ack);
        return true;
    }

    unit->OnMovementInitiated(true, false);

    if (msg->m_hasVehicle) {
        unit->m_movementData.AddPlayerTeleportEvent(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_hasTransport ? msg->m_transportGuid : WOWGUID(), msg->m_position, msg->m_facing, msg->m_seat, msg->m_exitVoluntary, msg->m_awaitTransition);
        return true;
    }

    unit->m_movementData.AddPlayerTeleportEvent(FrameTime::GetCurTimeMs(), msg->m_ackIndex, msg->m_hasTransport ? msg->m_transportGuid : WOWGUID(), msg->m_position, msg->m_facing, 0xFF, 0, 0);
    return true;
}

// OFFSET: [0x005B6500<ClientMoveSetRunSpeed>, 0x005B6550<ClientMoveSetRunBackSpeed>, 0x005B65A0<ClientMoveSetSwimSpeed>, 0x005B65F0<ClientMoveSetSwimBackSpeed>, 0x005B6640<ClientMoveSetFlightSpeed>, 0x005B6690<ClientMoveSetFlightBackSpeed>, 0x005B66E0<ClientMoveSetWalkSpeed>, 0x005B6730<ClientMoveSetTurnRate>, 0x005B6780<ClientMoveSetPitchRate>]
template <class T>
static bool HandleCompoundSpeedChange(WOWGUID guid, const JamMoveStateChange& change) {
    T msg(guid, change.m_ackIndex, *(change.m_hasSpeed ? &change.m_speed : NULL));
    return msg.Handle();
}

// OFFSET: 0x005AFB90
static bool HandleCompoundKnockBack(WOWGUID guid, const JamMoveStateChange& change) {
    const JamMoveKnockBack* knockBack = change.m_hasKnockBack ? &change.m_knockBack : NULL;
    ClientMoveKnockBack msg(guid, change.m_ackIndex, knockBack->m_direction, knockBack->m_horizontalSpeed, knockBack->m_verticalSpeed);
    return msg.Handle();
}

// OFFSET: 0x005AFBE0
static bool HandleCompoundCollisionHeight(WOWGUID guid, const JamMoveStateChange& change) {
    const JamMoveCollisionHeight* height = change.m_hasCollisionHeight ? &change.m_collisionHeight : NULL;
    ClientMoveSetCollisionHeight msg(guid, change.m_ackIndex, height->m_height, height->m_reason);
    return msg.Handle();
}

// OFFSET: 0x005AFC30
static bool HandleCompoundVehicleRecId(WOWGUID guid, const JamMoveStateChange& change) {
    ClientMoveSetVehicleRecId msg(guid, change.m_ackIndex, *(change.m_hasVehicleRecId ? &change.m_vehicleRecId : NULL));
    return msg.Handle();
}

// OFFSET: 0x005B7F70
static bool ClientMoveSetCompoundStateHandler(ClientMoveSetCompoundState* msg) {
    if (!ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        return true;
    }

    for (uint32_t i = 0; i < msg->m_changes.Count(); i++) {
        const JamMoveStateChange& change = msg->m_changes[i];

        switch (change.m_messageId) {
            case 0x3DB5:
                HandleCompoundSpeedChange<ClientMoveSetRunSpeed>(msg->m_guid, change);
                break;

            case 0x71B1:
                HandleCompoundSpeedChange<ClientMoveSetRunBackSpeed>(msg->m_guid, change);
                break;

            case 0x15A7:
                HandleCompoundSpeedChange<ClientMoveSetSwimSpeed>(msg->m_guid, change);
                break;

            case 0x5CA6:
                HandleCompoundSpeedChange<ClientMoveSetSwimBackSpeed>(msg->m_guid, change);
                break;

            case 0x71A6:
                HandleCompoundSpeedChange<ClientMoveSetFlightSpeed>(msg->m_guid, change);
                break;

            case 0x30A2:
                HandleCompoundSpeedChange<ClientMoveSetFlightBackSpeed>(msg->m_guid, change);
                break;

            case 0x1DA4:
                HandleCompoundSpeedChange<ClientMoveSetWalkSpeed>(msg->m_guid, change);
                break;

            case 0x30A5:
                HandleCompoundSpeedChange<ClientMoveSetTurnRate>(msg->m_guid, change);
                break;

            case 0x75B0:
                HandleCompoundSpeedChange<ClientMoveSetPitchRate>(msg->m_guid, change);
                break;

            case 0x5CB4:
                HandleCompoundKnockBack(msg->m_guid, change);
                break;

            case 0x11B0:
                HandleCompoundCollisionHeight(msg->m_guid, change);
                break;

            case 0x70A1:
                HandleCompoundVehicleRecId(msg->m_guid, change);
                break;

            case 0x7DA0: {
                ClientMoveRoot move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x7DB4: {
                ClientMoveUnroot move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x75B1: {
                ClientMoveSetWaterWalk move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x34B7: {
                ClientMoveSetLandWalk move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x79B0: {
                ClientMoveSetFeatherFall move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x51B6: {
                ClientMoveSetNormalFall move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x5CB3: {
                ClientMoveSetHover move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x51B3: {
                ClientMoveUnsetHover move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x3DA1: {
                ClientMoveSetCanFly move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x15A2: {
                ClientMoveUnsetCanFly move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x59A2: {
                ClientMoveSetCanTransitionBetweenSwimAndFly move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x7DB2: {
                ClientMoveUnsetCanTransitionBetweenSwimAndFly move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x75B2: {
                ClientMoveGravityDisable move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x30B3: {
                ClientMoveGravityEnable move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x31B0: {
                ClientMoveCollisionDisable move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }

            case 0x11A7: {
                ClientMoveCollisionEnable move(msg->m_guid, change.m_ackIndex);
                move.Handle();
                break;
            }
        }
    }

    return true;
}

// OFFSET: 0x005DB500/0x140257560
void CGUnit_C::Initialize() {
    ClientServices::SetMessageHandler(SMSG_ON_MONSTER_MOVE, CGUnit_C::HandleMonsterMovePacket, NULL);
    ClientServices::SetMessageHandler(SMSG_MONSTER_MOVE_TRANSPORT, CGUnit_C::HandleMonsterMovePacket, NULL);
    ClientServices::SetMessageHandler(SMSG_MOUNT_SPECIAL_ANIM, Packet_SMSG_MOUNTSPECIAL_ANIM, NULL);
    ClientServices::SetMessageHandler(SMSG_AI_REACTION, Packet_SMSG_AI_REACTION, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_ACTION_SOUND, Packet_SMSG_PET_ACTION_SOUND, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_DISMISS_SOUND, Packet_SMSG_PET_DISMISS_SOUND, NULL);
    ClientServices::SetMessageHandler(SMSG_STAND_STATE_UPDATE, Packet_SMSG_STANDSTATE_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_MULTIPLE_MOVES, Packet_SMSG_MULTIPLE_MOVES, NULL);
    ClientServices::SetMessageHandler(SMSG_CONTROL_UPDATE, Packet_SMSG_CLIENT_CONTROL_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_FLIGHT_SPLINE_SYNC, Packet_SMSG_FLIGHT_SPLINE_SYNC, NULL);
    ClientServices::SetMessageHandler(SMSG_AURA_UPDATE_ALL, Packet_AuraUpdate, NULL);
    ClientServices::SetMessageHandler(SMSG_AURA_UPDATE, Packet_AuraUpdate, NULL);
    ClientServices::SetMessageHandler(SMSG_DISMOUNT, Packet_SMSG_DISMOUNT, NULL);
    ClientServices::SetMessageHandler(SMSG_LOOT_LIST, Packet_SMSG_LOOT_LIST, NULL);
    ClientServices::SetMessageHandler(SMSG_MIRROR_IMAGE_COMPONENTED_DATA, Packet_SMSG_MIRRORIMAGE_DATA, NULL);
    ClientServices::SetMessageHandler(SMSG_FORCE_DISPLAY_UPDATE, Packet_SMSG_FORCE_DISPLAY_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_CANCEL_AUTO_REPEAT, Packet_SMSG_CANCEL_AUTO_REPEAT, NULL);
    ClientServices::SetMessageHandler(SMSG_HEALTH_UPDATE, Packet_SMSG_HEALTH_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_POWER_UPDATE, Packet_SMSG_POWER_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_HIGHEST_THREAT_UPDATE, Packet_ThreatUpdate, NULL);
    ClientServices::SetMessageHandler(SMSG_THREAT_UPDATE, Packet_ThreatUpdate, NULL);
    ClientServices::SetMessageHandler(SMSG_THREAT_REMOVE, Packet_SMSG_THREAT_REMOVE, NULL);
    ClientServices::SetMessageHandler(SMSG_THREAT_CLEAR, Packet_SMSG_THREAT_CLEAR, NULL);
    ClientServices::SetMessageHandler(SMSG_PRE_RESSURECT, Packet_SMSG_PRE_RESURRECT, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAYER_VEHICLE_DATA, Packet_SMSG_PLAYER_VEHICLE_DATA, NULL);
    ClientServices::SetMessageHandler(SMSG_MULTIPLE_PACKETS_2, Packet_SMSG_MULTIPLE_PACKETS, NULL);
    ClientServices::SetMessageHandler(SMSG_FORCE_ANIM, Packet_SMSG_FORCE_ANIM, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_AI_ANIM_KIT, Packet_SMSG_SET_AI_ANIM_KIT, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAY_ONE_SHOT_ANIM_KIT, Packet_SMSG_PLAY_ONE_SHOT_ANIM_KIT, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_MOVEMENT_ANIM_KIT, Packet_SMSG_SET_MOVEMENT_ANIM_KIT, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_MELEE_ANIM_KIT, Packet_SMSG_SET_MELEE_ANIM_KIT, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_ANIM_TIER, Packet_SMSG_SET_ANIM_TIER, NULL);
    UnitSoundInitialize();
    Spell_C_Initialize();
    UnitCombatClientInitialize();
    CGUnit_C::RegisterMirrorHandlers();
    CacheEmoteRecords();
    CGUnit_C::InitPowerTypeSlots();
    ClientSuppressNPCGreetings::s_handler = ClientSuppressNPCGreetingsHandler;
    ClientAuraPointsDepleted::s_handler = ClientAuraPointsDepletedHandler;
    ClientPlaySpellVisual::s_handler = ClientPlaySpellVisualHandler;
    ClientSetPlayHoverAnim::s_handler = ClientSetPlayHoverAnimHandler;
    ClientMoveUpdateRunSpeed::s_handler = ClientMoveUpdateRunSpeedHandler;
    ClientMoveUpdateRunBackSpeed::s_handler = ClientMoveUpdateRunBackSpeedHandler;
    ClientMoveUpdateWalkSpeed::s_handler = ClientMoveUpdateWalkSpeedHandler;
    ClientMoveUpdateSwimSpeed::s_handler = ClientMoveUpdateSwimSpeedHandler;
    ClientMoveUpdateSwimBackSpeed::s_handler = ClientMoveUpdateSwimBackSpeedHandler;
    ClientMoveUpdateFlightSpeed::s_handler = ClientMoveUpdateFlightSpeedHandler;
    ClientMoveUpdateFlightBackSpeed::s_handler = ClientMoveUpdateFlightBackSpeedHandler;
    ClientMoveUpdateTurnRate::s_handler = ClientMoveUpdateTurnRateHandler;
    ClientMoveUpdatePitchRate::s_handler = ClientMoveUpdatePitchRateHandler;
    ClientMoveUpdateCollisionHeight::s_handler = ClientMoveUpdateCollisionHeightHandler;
    ClientMoveUpdate::s_handler = ClientMoveUpdateHandler;
    ClientMoveUpdateTeleport::s_handler = ClientMoveUpdateTeleportHandler;
    ClientMoveUpdateKnockBack::s_handler = ClientMoveUpdateKnockBackHandler;
    ClientSplineMoveSetRunSpeed::s_handler = ClientSplineMoveSetRunSpeedHandler;
    ClientSplineMoveSetRunBackSpeed::s_handler = ClientSplineMoveSetRunBackSpeedHandler;
    ClientSplineMoveSetSwimSpeed::s_handler = ClientSplineMoveSetSwimSpeedHandler;
    ClientSplineMoveSetSwimBackSpeed::s_handler = ClientSplineMoveSetSwimBackSpeedHandler;
    ClientSplineMoveSetFlightSpeed::s_handler = ClientSplineMoveSetFlightSpeedHandler;
    ClientSplineMoveSetFlightBackSpeed::s_handler = ClientSplineMoveSetFlightBackSpeedHandler;
    ClientSplineMoveSetWalkSpeed::s_handler = ClientSplineMoveSetWalkSpeedHandler;
    ClientSplineMoveSetTurnRate::s_handler = ClientSplineMoveSetTurnRateHandler;
    ClientSplineMoveSetPitchRate::s_handler = ClientSplineMoveSetPitchRateHandler;
    ClientSplineMoveRoot::s_handler = ClientSplineMoveRootHandler;
    ClientSplineMoveUnroot::s_handler = ClientSplineMoveUnrootHandler;
    ClientSplineMoveGravityDisable::s_handler = ClientSplineMoveGravityDisableHandler;
    ClientSplineMoveGravityEnable::s_handler = ClientSplineMoveGravityEnableHandler;
    ClientSplineMoveCollisionDisable::s_handler = ClientSplineMoveCollisionDisableHandler;
    ClientSplineMoveCollisionEnable::s_handler = ClientSplineMoveCollisionEnableHandler;
    ClientSplineMoveSetFeatherFall::s_handler = ClientSplineMoveSetFeatherFallHandler;
    ClientSplineMoveSetNormalFall::s_handler = ClientSplineMoveSetNormalFallHandler;
    ClientSplineMoveSetHover::s_handler = ClientSplineMoveSetHoverHandler;
    ClientSplineMoveUnsetHover::s_handler = ClientSplineMoveUnsetHoverHandler;
    ClientSplineMoveSetWaterWalk::s_handler = ClientSplineMoveSetWaterWalkHandler;
    ClientSplineMoveSetLandWalk::s_handler = ClientSplineMoveSetLandWalkHandler;
    ClientSplineMoveStartSwim::s_handler = ClientSplineMoveStartSwimHandler;
    ClientSplineMoveStopSwim::s_handler = ClientSplineMoveStopSwimHandler;
    ClientSplineMoveSetRunMode::s_handler = ClientSplineMoveSetRunModeHandler;
    ClientSplineMoveSetWalkMode::s_handler = ClientSplineMoveSetWalkModeHandler;
    ClientSplineMoveSetFlying::s_handler = ClientSplineMoveSetFlyingHandler;
    ClientSplineMoveUnsetFlying::s_handler = ClientSplineMoveUnsetFlyingHandler;
    ClientMoveSetRunSpeed::s_handler = ClientMoveSetRunSpeedHandler;
    ClientMoveSetRunBackSpeed::s_handler = ClientMoveSetRunBackSpeedHandler;
    ClientMoveSetSwimSpeed::s_handler = ClientMoveSetSwimSpeedHandler;
    ClientMoveSetSwimBackSpeed::s_handler = ClientMoveSetSwimBackSpeedHandler;
    ClientMoveSetFlightSpeed::s_handler = ClientMoveSetFlightSpeedHandler;
    ClientMoveSetFlightBackSpeed::s_handler = ClientMoveSetFlightBackSpeedHandler;
    ClientMoveSetWalkSpeed::s_handler = ClientMoveSetWalkSpeedHandler;
    ClientMoveSetTurnRate::s_handler = ClientMoveSetTurnRateHandler;
    ClientMoveSetPitchRate::s_handler = ClientMoveSetPitchRateHandler;
    ClientMoveRoot::s_handler = ClientMoveRootHandler;
    ClientMoveUnroot::s_handler = ClientMoveUnrootHandler;
    ClientMoveSetWaterWalk::s_handler = ClientMoveSetWaterWalkHandler;
    ClientMoveSetLandWalk::s_handler = ClientMoveSetLandWalkHandler;
    ClientMoveSetFeatherFall::s_handler = ClientMoveSetFeatherFallHandler;
    ClientMoveSetNormalFall::s_handler = ClientMoveSetNormalFallHandler;
    ClientMoveSetHover::s_handler = ClientMoveSetHoverHandler;
    ClientMoveUnsetHover::s_handler = ClientMoveUnsetHoverHandler;
    ClientMoveKnockBack::s_handler = ClientMoveKnockBackHandler;
    ClientMoveTeleport::s_handler = ClientMoveTeleportHandler;
    ClientMoveSetCanFly::s_handler = ClientMoveSetCanFlyHandler;
    ClientMoveUnsetCanFly::s_handler = ClientMoveUnsetCanFlyHandler;
    ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler = ClientMoveSetCanTransitionBetweenSwimAndFlyHandler;
    ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler = ClientMoveUnsetCanTransitionBetweenSwimAndFlyHandler;
    ClientMoveGravityDisable::s_handler = ClientMoveGravityDisableHandler;
    ClientMoveGravityEnable::s_handler = ClientMoveGravityEnableHandler;
    ClientMoveCollisionDisable::s_handler = ClientMoveCollisionDisableHandler;
    ClientMoveCollisionEnable::s_handler = ClientMoveCollisionEnableHandler;
    ClientMoveSetCollisionHeight::s_handler = ClientMoveSetCollisionHeightHandler;
    ClientMoveSetVehicleRecId::s_handler = ClientMoveSetVehicleRecIdHandler;
    ClientMoveSkipTime::s_handler = ClientMoveSkipTimeHandler;
    ClientMoveSetCompoundState::s_handler = ClientMoveSetCompoundStateHandler;

    memset(g_environmentalDamageVisualKits, 0, sizeof(g_environmentalDamageVisualKits));

    int32_t index = g_environmentalDamageDB.m_numRecords;

    while (index--) {
        EnvironmentalDamageRec* rec = g_environmentalDamageDB.GetRecordByIndex(index);

        if (static_cast<uint32_t>(rec->m_enumID) < sizeof(g_environmentalDamageVisualKits) / sizeof(g_environmentalDamageVisualKits[0])) {
            g_environmentalDamageVisualKits[rec->m_enumID] = rec->m_visualkitID;
        }
    }

    CGUnit_C::s_trackingType = 13;
    CGUnit_C::s_unk_CA11D4 = 0;
    s_pendingAttackTarget = WOWGUID();
    s_pendingLootTarget = WOWGUID();
    s_pendingUseObject = WOWGUID();
    s_pendingSkinTarget = WOWGUID();
    s_pendingInteractTarget = WOWGUID();
    CGUnit_C::s_activeMover = WOWGUID();
    CMissile::Initialize();
    CUnitDisplayTransition_C::InitSystem();
    CVehicle_C::InitSystem();
    CVehiclePassenger_C::InitSystem();
    CVehicleCamera_C::InitSystem();
    CGUnit_C::InitMissileTrajectorySystem();
    CGUnit_C::VehiclePassengerInitWorldCameraState();
    void* pool = SMemAlloc(sizeof(TSObjectAllocPool<UnitThreat>), __FILE__, __LINE__, 0);
    s_unitThreatPool = pool ? new (pool) TSObjectAllocPool<UnitThreat>("Unit Threat", 16) : NULL;
    CGUnit_C::s_deferredClientControlUpdateGUID = WOWGUID();
    CGUnit_C::s_deferredClientControlUpdateState = 0;
    CGUnit_C::s_initialized = 1;
}

// OFFSET: 0x005DA690
void CGUnit_C::Shutdown() {
    CGUnit_C::s_initialized = 0;
    CMissile::RemoveMissiles();
    ClientServices::ClearMessageHandler(SMSG_ON_MONSTER_MOVE);
    ClientServices::ClearMessageHandler(SMSG_MONSTER_MOVE_TRANSPORT);
    ClientServices::ClearMessageHandler(SMSG_MOUNT_SPECIAL_ANIM);
    ClientServices::ClearMessageHandler(SMSG_AI_REACTION);
    ClientServices::ClearMessageHandler(SMSG_PET_ACTION_SOUND);
    ClientServices::ClearMessageHandler(SMSG_PET_DISMISS_SOUND);
    ClientServices::ClearMessageHandler(SMSG_STAND_STATE_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_MULTIPLE_MOVES);
    ClientServices::ClearMessageHandler(SMSG_CONTROL_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_FLIGHT_SPLINE_SYNC);
    ClientServices::ClearMessageHandler(SMSG_AURA_UPDATE_ALL);
    ClientServices::ClearMessageHandler(SMSG_AURA_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_DISMOUNT);
    ClientServices::ClearMessageHandler(SMSG_LOOT_LIST);
    ClientServices::ClearMessageHandler(SMSG_MIRROR_IMAGE_COMPONENTED_DATA);
    ClientServices::ClearMessageHandler(SMSG_FORCE_DISPLAY_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_CANCEL_AUTO_REPEAT);
    ClientServices::ClearMessageHandler(SMSG_HEALTH_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_POWER_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_HIGHEST_THREAT_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_THREAT_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_THREAT_REMOVE);
    ClientServices::ClearMessageHandler(SMSG_THREAT_CLEAR);
    ClientServices::ClearMessageHandler(SMSG_PRE_RESSURECT);
    ClientServices::ClearMessageHandler(SMSG_PLAYER_VEHICLE_DATA);
    ClientServices::ClearMessageHandler(SMSG_MULTIPLE_PACKETS_2);
    ClientServices::ClearMessageHandler(SMSG_FORCE_ANIM);
    ClientServices::ClearMessageHandler(SMSG_SET_AI_ANIM_KIT);
    ClientServices::ClearMessageHandler(SMSG_PLAY_ONE_SHOT_ANIM_KIT);
    ClientServices::ClearMessageHandler(SMSG_SET_MOVEMENT_ANIM_KIT);
    ClientServices::ClearMessageHandler(SMSG_SET_MELEE_ANIM_KIT);
    ClientServices::ClearMessageHandler(SMSG_SET_ANIM_TIER);
    ClientAuraPointsDepleted::s_handler = NULL;
    ClientPlaySpellVisual::s_handler = NULL;
    ClientMoveUpdateRunSpeed::s_handler = NULL;
    ClientMoveUpdateRunBackSpeed::s_handler = NULL;
    ClientMoveUpdateWalkSpeed::s_handler = NULL;
    ClientMoveUpdateSwimSpeed::s_handler = NULL;
    ClientMoveUpdateSwimBackSpeed::s_handler = NULL;
    ClientMoveUpdateFlightSpeed::s_handler = NULL;
    ClientMoveUpdateFlightBackSpeed::s_handler = NULL;
    ClientMoveUpdateTurnRate::s_handler = NULL;
    ClientMoveUpdatePitchRate::s_handler = NULL;
    ClientMoveUpdateCollisionHeight::s_handler = NULL;
    ClientMoveUpdate::s_handler = NULL;
    ClientMoveUpdateTeleport::s_handler = NULL;
    ClientMoveUpdateKnockBack::s_handler = NULL;
    ClientSplineMoveSetRunSpeed::s_handler = NULL;
    ClientSplineMoveSetRunBackSpeed::s_handler = NULL;
    ClientSplineMoveSetSwimSpeed::s_handler = NULL;
    ClientSplineMoveSetSwimBackSpeed::s_handler = NULL;
    ClientSplineMoveSetFlightSpeed::s_handler = NULL;
    ClientSplineMoveSetFlightBackSpeed::s_handler = NULL;
    ClientSplineMoveSetWalkSpeed::s_handler = NULL;
    ClientSplineMoveSetTurnRate::s_handler = NULL;
    ClientSplineMoveSetPitchRate::s_handler = NULL;
    ClientSplineMoveRoot::s_handler = NULL;
    ClientSplineMoveUnroot::s_handler = NULL;
    ClientSplineMoveGravityDisable::s_handler = NULL;
    ClientSplineMoveGravityEnable::s_handler = NULL;
    ClientSplineMoveCollisionDisable::s_handler = NULL;
    ClientSplineMoveCollisionEnable::s_handler = NULL;
    ClientSplineMoveSetFeatherFall::s_handler = NULL;
    ClientSplineMoveSetNormalFall::s_handler = NULL;
    ClientSplineMoveSetHover::s_handler = NULL;
    ClientSplineMoveUnsetHover::s_handler = NULL;
    ClientSplineMoveSetWaterWalk::s_handler = NULL;
    ClientSplineMoveSetLandWalk::s_handler = NULL;
    ClientSplineMoveStartSwim::s_handler = NULL;
    ClientSplineMoveStopSwim::s_handler = NULL;
    ClientSplineMoveSetRunMode::s_handler = NULL;
    ClientSplineMoveSetWalkMode::s_handler = NULL;
    ClientSplineMoveSetFlying::s_handler = NULL;
    ClientSplineMoveUnsetFlying::s_handler = NULL;
    ClientMoveSetRunSpeed::s_handler = NULL;
    ClientMoveSetRunBackSpeed::s_handler = NULL;
    ClientMoveSetSwimSpeed::s_handler = NULL;
    ClientMoveSetSwimBackSpeed::s_handler = NULL;
    ClientMoveSetFlightSpeed::s_handler = NULL;
    ClientMoveSetFlightBackSpeed::s_handler = NULL;
    ClientMoveSetWalkSpeed::s_handler = NULL;
    ClientMoveSetTurnRate::s_handler = NULL;
    ClientMoveSetPitchRate::s_handler = NULL;
    ClientMoveRoot::s_handler = NULL;
    ClientMoveUnroot::s_handler = NULL;
    ClientMoveSetWaterWalk::s_handler = NULL;
    ClientMoveSetLandWalk::s_handler = NULL;
    ClientMoveSetFeatherFall::s_handler = NULL;
    ClientMoveSetNormalFall::s_handler = NULL;
    ClientMoveSetHover::s_handler = NULL;
    ClientMoveUnsetHover::s_handler = NULL;
    ClientMoveKnockBack::s_handler = NULL;
    ClientMoveTeleport::s_handler = NULL;
    ClientMoveSetCanFly::s_handler = NULL;
    ClientMoveUnsetCanFly::s_handler = NULL;
    ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler = NULL;
    ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler = NULL;
    ClientMoveGravityDisable::s_handler = NULL;
    ClientMoveGravityEnable::s_handler = NULL;
    ClientMoveCollisionDisable::s_handler = NULL;
    ClientMoveCollisionEnable::s_handler = NULL;
    ClientMoveSetCollisionHeight::s_handler = NULL;
    ClientMoveSetVehicleRecId::s_handler = NULL;
    ClientMoveSkipTime::s_handler = NULL;
    ClientMoveSetCompoundState::s_handler = NULL;
    UnitSoundShutdown();
    Spell_C_Destroy();
    UnitCombatClientShutdown();
    CGUnit_C::UnregisterMirrorHandlers();

    if (s_unitThreatPool) {
        delete s_unitThreatPool;
        s_unitThreatPool = NULL;
    }
}

// PROVEN: f73cb84f IDENTICAL/NONE
// OFFSET: 0x005ABD90/0x14021E220
const char* CGUnit_C::GetDisplayRaceNameFromRecord(const ChrRacesRec* rec, int32_t sex, int32_t* displaySex) {
    if (displaySex) {
        *displaySex = sex;
    }

    if (!rec) {
        return NULL;
    }

    if (sex == 0) {
        if (*rec->m_nameMale) {
            return rec->m_nameMale;
        }

        if (*rec->m_nameFemale) {
            if (displaySex) {
                *displaySex = 1;
            }

            return rec->m_nameFemale;
        }
    } else if (sex == 1) {
        if (*rec->m_nameFemale) {
            return rec->m_nameFemale;
        }

        if (*rec->m_nameMale) {
            if (displaySex) {
                *displaySex = 0;
            }

            return rec->m_nameMale;
        }
    }

    return rec->m_name;
}

// PROVEN: e0fd9d4e IDENTICAL/NONE
// OFFSET: 0x005ABE00/0x14021E2E0
const char* CGUnit_C::GetDisplayClassNameFromRecord(const ChrClassesRec* rec, int32_t sex, int32_t* displaySex) {
    if (displaySex) {
        *displaySex = sex;
    }

    if (!rec) {
        return NULL;
    }

    if (sex == 0) {
        if (*rec->m_nameMale) {
            return rec->m_nameMale;
        }

        if (*rec->m_nameFemale) {
            if (displaySex) {
                *displaySex = 1;
            }

            return rec->m_nameFemale;
        }
    } else if (sex == 1) {
        if (*rec->m_nameFemale) {
            return rec->m_nameFemale;
        }

        if (*rec->m_nameMale) {
            if (displaySex) {
                *displaySex = 0;
            }

            return rec->m_nameMale;
        }
    }

    return rec->m_name;
}

// OFFSET: 0x0043F410
int32_t CGUnit_C::IsActiveMover() const {
    return this->m_obj->m_guid == s_activeMover;
}

// PROVEN: 79770932 IDENTICAL/NONE
// OFFSET: 0x0043F440
int32_t CGUnit_C::IsVehicle() const {
    return (this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL) != NULL;
}

// OFFSET: 0x00554CF0/0x1401B1170
float CGUnit_C::GetDistanceToPos(const C3Vector& pos) const {
    C3Vector position = this->GetPosition();
    float dy = position.y - pos.y;
    float dz = position.z - pos.z;
    float dx = position.x - pos.x;
    return dx * dx + dz * dz + dy * dy;
}

// OFFSET: #0x004F6210
VehicleRec* CGUnit_C::GetVehicleRec() const {
    if (this->m_vehicle) {
        return this->m_vehicle->m_vehicleRec;
    }

    return NULL;
}

// PROVEN: 9569db51 IDENTICAL/NONE
// OFFSET: 0x00459560/0x140069590
uint32_t CGUnit_C::GetAuraCount() const {
    if (this->m_auras.m_count == 0xFFFFFFFF) {
        return this->m_auras.Dynamic().m_count;
    }

    return this->m_auras.m_count;
}

// OFFSET: #0x004F8870
int32_t CGUnit_C::GetAuraSpellId(uint32_t index) {
    return this->m_auras[index].m_spellId;
}

// PROVEN: 344f5756 IDENTICAL/NONE
// OFFSET: 0x0044ED60
int32_t CGUnit_C::IsMounted() const {
    return this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000);
}

// OFFSET: 0x00459580/0x1400695B0
AuraData* CGUnit_C::GetAura(uint32_t index) {
    if (index >= this->GetAuraCount()) {
        return NULL;
    }

    return &this->m_auras[index];
}

// OFFSET: #0x00565510
uint8_t CGUnit_C::GetAuraFlags(uint32_t index) {
    return this->m_auras[index].m_flags;
}

// OFFSET: 0x00950240/0x1406C05F0
int32_t CGUnit_C::IsControllable() const {
    uint32_t flags = this->m_unit->m_unitFieldFlags;
    return !(flags & 0x40000) && !(flags & 0x800000) && !(flags & 0x400000);
}

// OFFSET: #0x005D3340
VehicleSeatRec* CGUnit_C::GetVehicleSeatRec(uint8_t seat) const {
    if (this->m_vehicle) {
        return this->m_vehicle->GetVehicleSeatRec(seat);
    }

    return NULL;
}

// OFFSET: 0x00557260/0x1401B4070
int32_t __stdcall CGUnit_C::UnitCanGossip(CGUnit_C* unit) {
    return unit->IsQuestGiver() && unit->m_questStatus >= 2;
}

// PROVEN: d51cdaaf IDENTICAL/NONE
// OFFSET: 0x0056E060/0x1401CEC50
int32_t CGUnit_C::CanBeTargetted() const {
    return this->CanHighlight();
}

// PROVEN: e0e59ad5 IDENTICAL/NONE
// OFFSET: 0x0056E070/0x1401CEC60
int32_t CGUnit_C::IsHighlightSuppressed() const {
    return (this->m_clientFlags >> 25) & 1;
}

// OFFSET: 0x0056E0B0/0x1401CECB0
C3Vector CGUnit_C::GetRawPosition() const {
    return this->m_movement->m_position;
}

// PROVEN: 964abdc7 IDENTICAL/NONE
// OFFSET: 0x0056E100/0x1401CECF0
float CGUnit_C::GetRawFacing() const {
    return this->m_movement->m_facing;
}

// PROVEN: 6ea7a310 IDENTICAL/NONE
// OFFSET: 0x0056E110/0x1401CED00
uint64_t CGUnit_C::GetTransportGUID() const {
    return this->m_movementData.m_transportGuid.m_guid;
}

// PROVEN: baa57048 IDENTICAL/NONE
// OFFSET: 0x0056E120/0x1401CED10
CM2Model* CGUnit_C::GetObjectModel() const {
    if (this->m_mountModel) {
        return this->m_mountModel;
    }

    return this->m_worldModel;
}

// PROVEN: bd653a62 IDENTICAL/NONE
// OFFSET: 0x0056E160/0x1401CED50
float CGUnit_C::GetPitch() const {
    return this->m_movementData.m_pitch;
}

// OFFSET: #0x006FBE30
void CGUnit_C::PreStartUpdatingMissileTrajectory(int32_t spellId, uint32_t castId) {
    this->m_missileTrajectorySpell = spellId;
    this->m_missileTrajectoryCastId = castId;
}

// OFFSET: #0x006FBE50
void CGUnit_C::StartUpdatingMissileTrajectory(int32_t spellId, uint32_t start, uint32_t end) {
    if (spellId == this->m_missileTrajectorySpell) {
        this->m_missileTrajectoryStart = start;
        this->m_missileTrajectoryEnd = end;
    }
}

// OFFSET: #0x006FBE80
void CGUnit_C::StopUpdatingMissileTrajectory(int32_t spellId) {
    if (spellId == this->m_missileTrajectorySpell) {
        this->m_missileTrajectorySpell = 0;
        this->m_missileTrajectoryCastId = 0;
        this->m_missileTrajectoryStart = 0;
        this->m_missileTrajectoryReleased = 0;
    }
}

// PROVEN: 1a6419f5 IDENTICAL/NONE
// OFFSET: 0x0057CCB0/0x1401E2E40
int32_t CGUnit_C::IsUpdatingMissileTrajectory() const {
    return this->m_missileTrajectoryCastId && this->m_missileTrajectoryStart;
}

// OFFSET: #0x006FBED0
int32_t CGUnit_C::IsMissileTrajectoryUpdateWaitingForMissileRelease() const {
    return this->m_missileTrajectoryCastId && this->m_missileTrajectoryStart && this->m_missileTrajectoryReleased;
}

// PROVEN: 7991db9c IDENTICAL/NONE
// OFFSET: 0x0057CD90/0x1401E2F50
void CGUnit_C::PitchMissileTrajectoryTargetingNextFrame() {
    s_pitchMissileTrajectoryTargetingNextFrame = 1;
}

// PROVEN: 606268f1 IDENTICAL/NONE
// OFFSET: 0x0057CDA0/0x1401E2F60
void CGUnit_C::SetMissileTrajectoryTargetingSpell(uint32_t spellId) {
    s_missileTrajectoryTargetingSpell = spellId;
}

// PROVEN: de992491 IDENTICAL/NONE
// OFFSET: 0x005AA530/0x14021C9A0
bool CGUnit_C::IsLocalClientControlled() const {
    return (this->m_clientFlags >> 10) & 1;
}

// PROVEN: d00494b9 IDENTICAL/NONE
// OFFSET: 0x005AA600
void CGUnit_C::FreeComponent() {
    if (this->m_characterComponent) {
        CCharacterComponent::FreeComponent(this->m_characterComponent);
        this->m_characterComponent = NULL;
        this->m_clientFlags = (this->m_clientFlags & 0xFFBDFFFF) | 0x400000;
    }
}

// PROVEN: ab948951 IDENTICAL/NONE
// OFFSET: 0x005AA680/0x14021CB30
void CGUnit_C::SetupFootprints() {
    CreatureModelDataRec* modelData = this->m_modelData;
    this->m_footprintTextureId = modelData->m_footprintTextureID;
    this->m_footprintLength = modelData->m_footprintTextureWidth * 0.027777778f;
    this->m_footprintWidth = modelData->m_footprintTextureLength * 0.027777778f;
    this->m_footprintParticleScale = modelData->m_footprintParticleScale;
}

// PROVEN: 8aeeb380 IDENTICAL/NONE
// OFFSET: 0x005AA640/0x14021CAC0
int32_t CGUnit_C::ShouldFadeout() {
    int32_t render = 0;
    int32_t fade = 0;
    this->ShouldRender(1, render, fade);
    return fade == 0;
}

// PROVEN: d2a132ca IDENTICAL/NONE
// OFFSET: 0x005AA6D0
int32_t __stdcall CGUnit_C::IsTwoHanded(const CGUnitVirtualItem* item) {
    if (!item || item->m_classID != 2) {
        return 0;
    }

    switch (item->m_subclassID) {
        case 1:
        case 5:
        case 6:
        case 8:
        case 10:
        case 20:
            return 1;

        default:
            return 0;
    }
}

// OFFSET: 0x005AA730/0x14021CBE0
uint32_t CGUnit_C::GetCombatReadyAnim() const {
    const CGUnitVirtualItem* mainHand = this->GetVirtualItem(0, false);

    if (!mainHand || mainHand->m_classID != 2) {
        const CGUnitVirtualItem* offHand = this->GetVirtualItem(1, false);
        return offHand && offHand->m_classID == 2 ? ANIM_READY_FIST : ANIM_READY_UNARMED;
    }

    const CGUnitVirtualItem* offHand = this->GetVirtualItem(1, false);
    uint32_t anim = ANIM_READY_2H - (offHand != NULL);

    switch (mainHand->m_subclassID) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 7:
        case 8:
        case 14:
        case 15:
        case 20:
            return anim;

        case 6:
        case 10:
            return ANIM_READY_2HL;

        case 13:
            if (!offHand || offHand->m_subclassID == 13) {
                return ANIM_READY_FIST;
            }

            return ANIM_READY_1H;

        default:
            return ANIM_READY_UNARMED;
    }
}

// OFFSET: 0x005AA7F0/0x14021CD00
float CGUnit_C::AddSquelchedEffect() {
    uint32_t now = FrameTime::GetCurTimeMs();
    uint32_t elapsed = now - this->m_lastSquelchedEffectTime;

    if (static_cast<int32_t>(elapsed - 5000) < 0) {
        this->m_squelchedEffectRate = static_cast<float>(static_cast<double>(elapsed) * 0.001 * 0.10000002 + this->m_squelchedEffectRate * 0.89999998);
    } else {
        this->m_squelchedEffectRate = 1.0f;
    }

    float rate = this->m_squelchedEffectRate;
    this->m_lastSquelchedEffectTime = now;

    if (rate < 0.25f) {
        return 0.85000002f;
    }

    if (rate >= 0.5f) {
        return 0.0f;
    }

    return 0.69999999f;
}

// PROVEN: af008cb1 IDENTICAL/NONE
// OFFSET: 0x005AA8A0
void CGUnit_C::RetargetEffects(uint32_t spellId, uint64_t oldTarget, uint64_t newTarget) {
    CGObject_C::RetargetEffects(this->m_effectList, spellId, oldTarget, newTarget);
    CGObject_C::RetargetEffects(this->m_procEffects, spellId, oldTarget, newTarget);
    CGObject_C::RetargetEffects(this->m_auraEffects, spellId, oldTarget, newTarget);
}

// OFFSET: #0x00714F90
int32_t CGUnit_C::ChooseRangedLoadAnim(uint32_t flags, uint32_t* anim) const {
    if (this->m_sheatheState != 2 || !(this->m_animationState & 0x200)) {
        return (flags & 0xFFFFFF00) == 0;
    }

    if (!(flags & 0x80) || (this->m_animationState & 0x4000)) {
        return 1;
    }

    *anim = ANIM_READY_UNARMED;
    const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

    if (!item || item->m_classID != 2) {
        return 1;
    }

    switch (item->m_subclassID) {
        case 2: {
            *anim = 105;
            return 1;
        }

        case 3:
        case 18: {
            *anim = 106;
            return 1;
        }

        case 16: {
            *anim = 112;
            return 1;
        }

        case 19: {
            *anim = 111;
            return 1;
        }

        default: {
            return 1;
        }
    }
}

// PROVEN: e3eb58e4 IDENTICAL/NONE
// OFFSET: 0x005AA910/0x14021CDB0
void CGUnit_C::RemoveMountModel() {
    if (this->m_mountModel) {
        this->UpdateAnimKitsBeforeMountModelChange(true);
        this->m_mountModel->SetLoadedCallback(NULL, NULL);
        this->m_mountModel->SetSequenceCallback(NULL, 0);
        this->m_mountModel->SetEventCallback(NULL, 0);
        this->m_mountModel->Release();
        this->m_mountModel = NULL;
        this->UpdateAnimKitsAfterMountModelChange(true);
    }
}

// OFFSET: 0x005BDD80
void CGUnit_C::SetMountModel(CM2Model* model) {
    if (model == this->m_mountModel) {
        return;
    }

    this->CleanUpVehicleBoneAnimsBeforeObjectModelChange();
    int32_t dismounting = 1;

    if (model) {
        model->m_refCount++;
    }

    if (!this->m_mountModel || model) {
        dismounting = 0;
    }

    this->UpdateAnimKitsBeforeMountModelChange(dismounting);

    if (this->m_mountModel) {
        if (this->m_mountModel->m_attachParent) {
            this->m_mountModel->DetachFromParent();
        }

        this->m_mountModel->SetLoadedCallback(NULL, NULL);
        this->m_mountModel->Release();
    }

    this->m_mountModel = model;
    this->Animate(0.0f);
    this->UpdateAnimKitsAfterMountModelChange(dismounting);

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->UpdateAnimKitAfterObjectModelChange();
    }

    if (this->m_mountModel) {
        this->m_mountModel->SetLoadedCallback(CGObject_C::ModelLoadedCallback, this);
    } else if (this->GetObjectModel()->IsLoaded()) {
        this->ModelChanged();
    } else {
        this->SetBoundsDirty();
    }

    if (this->m_worldObject) {
        World::ObjectSetModel(this->m_worldObject, this->GetObjectModel());
    }

    this->UpdateAlphaEffectAlpha();
    this->m_fadeDuration = 0;
    this->m_alpha = this->m_fadeEndAlpha;
    this->m_mountFadeDuration = 0;
    this->m_mountAlpha = this->m_mountFadeEndAlpha;
}

// PROVEN: 8511de85 IDENTICAL/NONE
// OFFSET: 0x005AA980/0x14021CE30
void CGUnit_C::RotateFacing(float delta) {
    this->m_smoothTarget.m_value = CMath::normalizeangle0to2pi_(delta + this->m_smoothTarget.m_value);
    this->m_renderFacing = CMath::normalizeangle0to2pi_(delta + this->m_renderFacing);
}

// PROVEN: f184d2ce IDENTICAL/NONE
// OFFSET: 0x009E72F0/0x14077FCB0
void CGUnit_C::NamePlateSetResortAll(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x1;
    } else {
        s_namePlateFlags &= ~0x1;
    }
}

// PROVEN: 453941c5 IDENTICAL/NONE
// OFFSET: 0x009E7310/0x14077FCD0
void CGUnit_C::NamePlateUpdateLevels(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x2;
    } else {
        s_namePlateFlags &= ~0x2;
    }
}

// PROVEN: 8c91925f IDENTICAL/NONE
// OFFSET: 0x009E7330/0x14077FCF0
void CGUnit_C::NamePlateUpdateColor(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x4;
    } else {
        s_namePlateFlags &= ~0x4;
    }
}

// PROVEN: ca975137 IDENTICAL/NONE
// OFFSET: 0x005AB840/0x14021DD20
uint32_t CGUnit_C::GetCreatureFamily() const {
    if (this->m_creatureCacheEntry) {
        return this->m_creatureCacheEntry->m_family;
    }

    return 0;
}

// PROVEN: 90a2d557 IDENTICAL/NONE
// OFFSET: 0x005AB860
void CGUnit_C::SaveChainSpellLocation(const C3Vector* location) {
    if (location) {
        this->m_chainSpellLocation = *location;
        this->m_clientFlags |= 0x800000;
    } else {
        this->m_clientFlags &= ~0x800000;
    }
}

// OFFSET: #0x00715440
int32_t FactionTemplateReaction(const FactionTemplateRec* faction, const FactionTemplateRec* other) {
    int32_t otherGroup = other->m_factionGroup;

    if (otherGroup & faction->m_enemyGroup) {
        return 1;
    }

    for (uint32_t i = 0; i < 4 && faction->m_enemies[i]; i++) {
        if (faction->m_enemies[i] == other->m_faction) {
            return 1;
        }
    }

    if (otherGroup & faction->m_friendGroup) {
        return 4;
    }

    for (uint32_t i = 0; i < 4 && faction->m_friend[i]; i++) {
        if (faction->m_friend[i] == other->m_faction) {
            return 4;
        }
    }

    if (faction->m_factionGroup & other->m_friendGroup) {
        return 4;
    }

    for (uint32_t i = 0; i < 4 && other->m_friend[i]; i++) {
        if (other->m_friend[i] == faction->m_faction) {
            return 4;
        }
    }

    return (~(static_cast<uint32_t>(faction->m_flags) >> 12) & 2) | 1;
}

// PROVEN: 5b1fc146 IDENTICAL/NONE
// OFFSET: 0x005ABAE0
void CGUnit_C::StartMountTransition(const MountTransitionObject* transition, CEffect* effect) {
    this->m_mountTransition = transition;
    this->m_mountTransitionEffect = effect;
}

// PROVEN: d92bc3ec IDENTICAL/NONE
// OFFSET: 0x005ABB00
void CGUnit_C::StopMountTransition() {
    this->m_mountTransition = NULL;
    this->m_mountTransitionEffect = NULL;
}

// OFFSET: #0x00715880
bool CGUnit_C::IsStandStateSplitBody() const {
    bool result = false;

    switch (this->GetClientStandState()) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 6:
            result = true;
            break;
    }

    return result;
}

// PROVEN: d6b179d8 IDENTICAL/NONE
// OFFSET: 0x005ABD50
const char* CGUnit_C::GetDisplayBakeName() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_bakeName;
    }

    return NULL;
}

// PROVEN: a3f6279b IDENTICAL/NONE
// OFFSET: 0x005ABD70
const char* CGUnit_C::GetPortraitTextureName() const {
    if (this->m_displayInfo) {
        return this->m_displayInfo->m_portraitTextureName;
    }

    return NULL;
}

// OFFSET: 0x005ABE70
void CGUnit_C::SetAnimEventPosition(const C3Vector* position) {
    this->m_clientFlags |= 0x80000000;
    this->m_animEventPosition = *position;
}

// OFFSET: 0x005ABEA0
void CGUnit_C::SetCastingSpell(const PendingSpellCast* cast) {
    uint8_t castCount = 0;
    int32_t spellId = 0;

    if (cast) {
        castCount = cast->m_data.m_castCount;
        spellId = cast->m_data.m_spellId;
    }

    this->m_castCount = castCount;

    if (this->m_castSpellId == spellId) {
        return;
    }

    this->m_castSpellId = spellId;

    if (cast && (cast->m_data.m_targetFlags & 0x2)) {
        this->UpdatePrecastAnimKit(0, cast->m_data.m_objectTarget);
    } else {
        this->UpdatePrecastAnimKit(0, WOWGUID());
    }
}

// PROVEN: 1c8aeaac IDENTICAL/NONE
// OFFSET: 0x005ABF00
void CGUnit_C::SetCastSchoolImmunities(int32_t immunities) {
    this->m_castSchoolImmunities = immunities;
}

// PROVEN: 5f524bc2 IDENTICAL/NONE
// OFFSET: 0x005ABF10
void CGUnit_C::SetCastImmunities(int32_t immunities) {
    this->m_castImmunities = immunities;
}

// PROVEN: 733e62af IDENTICAL/NONE
// OFFSET: 0x005ABF70
void CGUnit_C::SetCastingAutoRepeat(bool enable) {
    if (enable) {
        this->m_animationState |= 0x10;
    } else {
        this->m_animationState &= ~0x10;
    }
}

// PROVEN: 7d64e0b4 IDENTICAL/NONE
// OFFSET: 0x005ABFC0/0x14021E4D0
int32_t CGUnit_C::GetCastTimeRemaining() const {
    if (!this->m_castingSpellId) {
        return 0;
    }

    FrameTime::GetCurTimeMs();
    return this->m_castEndTime - FrameTime::GetCurTimeMs();
}

// PROVEN: 746babf9 IDENTICAL/NONE
// OFFSET: 0x005ABF20/0x14021E410
void CGUnit_C::SetCastingAnim(bool enable) {
    uint32_t state = this->m_animationState;

    if (!enable == !(state & 0x8)) {
        return;
    }

    this->m_animationState = enable ? state | 0x8 : state & ~0x8;
    this->UpdateCombatReadyAnimKit(0);
}

// OFFSET: 0x005ABFF0/0x14021E510
float CGUnit_C::GetBaseAlpha() const {
    if (this->m_displayInfo) {
        return this->m_displayInfo->m_creatureModelAlpha * (1.0f / 255.0f);
    }

    return 1.0f;
}

// PROVEN: ee465b15 IDENTICAL/NONE
// OFFSET: 0x005AC240/0x14021E790
uint32_t CGUnit_C::GetTrackingType() {
    return s_trackingType;
}

// PROVEN: 7d26eb92 IDENTICAL/DIFFERENT
// OFFSET: 0x005AC2F0/0x140101EF0
float CGUnit_C::GetTrackingTurn() {
    return s_trackingFacing;
}

// OFFSET: #0x00715D70
bool CGUnit_C::IsBossMob() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 2) & 1;
    }

    return false;
}

// PROVEN: 41090a61 IDENTICAL/NONE
// OFFSET: 0x005AC370/0x14021E8C0
bool CGUnit_C::IsLinkAll() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 22) & 1;
    }

    return false;
}

// PROVEN: 74debba2 IDENTICAL/NONE
// OFFSET: 0x005AC390/0x14021E8E0
bool CGUnit_C::HasNoShadowBlob() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 25) & 1;
    }

    return false;
}

// OFFSET: 0x005AC3B0/0x14021E900
bool CGUnit_C::ShowFactionTooltip() const {
    return !this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags[0] & 0x10);
}

// PROVEN: 5ed0e98f IDENTICAL/NONE
// OFFSET: 0x005AC3D0/0x14021E920
bool CGUnit_C::IsTreatAsRaidUnit() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 26) & 1;
    }

    return false;
}

// PROVEN: 6629b01b IDENTICAL/NONE
// OFFSET: 0x005AC3F0/0x14021E940
bool CGUnit_C::IsForceGossip() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 27) & 1;
    }

    return false;
}

// OFFSET: 0x005AC410
bool CGUnit_C::CanInteractWhileDead() const {
    return this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x80);
}

// OFFSET: 0x005AC430/0x14021E980
uint32_t CGUnit_C::GetSkinningType() const {
    if (!this->m_creatureCacheEntry) {
        return 0;
    }

    if (this->m_creatureCacheEntry->m_typeFlags[0] & 0x100) {
        return 1;
    }

    if (this->m_creatureCacheEntry->m_typeFlags[0] & 0x200) {
        return 2;
    }

    if (this->m_creatureCacheEntry->m_typeFlags[0] & 0x8000) {
        return 3;
    }

    return 0;
}

// OFFSET: 0x005AC480
void CGUnit_C::GetCursorFile(int32_t unable, char* buffer, uint32_t size) const {
    if (unable) {
        SStrPrintf(buffer, size, "Interface\\Cursor\\Unable%s.blp", this->m_creatureCacheEntry->m_iconName);
    } else {
        SStrPrintf(buffer, size, "Interface\\Cursor\\%s.blp", this->m_creatureCacheEntry->m_iconName);
    }
}

// PROVEN: ce3011b8 IDENTICAL/NONE
// OFFSET: 0x005AC4D0/0x14021E9F0
bool CGUnit_C::MountedCombatAllowed() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 11) & 1;
    }

    return false;
}

// PROVEN: b368ba0f IDENTICAL/NONE
// OFFSET: 0x005AC4F0
bool CGUnit_C::CanAidPlayers() const {
    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 12) & 1;
    }

    return false;
}

// OFFSET: #0x00715FB0
uint64_t CGUnit_C::GetLooter() const {
    return this->m_looter.m_guid;
}

// PROVEN: 88d2d1f0 IDENTICAL/NONE
// OFFSET: 0x005AC570
uint64_t CGUnit_C::GetMasterLooter() const {
    return this->m_masterLooter.m_guid;
}

// OFFSET: 0x005AC590/0x14021EA40
void CGUnit_C::InitWheels() {
    this->m_wheelMask = 0;
    CM2Model* model = this->GetObjectModel();

    for (uint32_t bone = 27; bone <= 34; bone++) {
        if (model->HasBone(bone)) {
            this->m_wheelMask |= 1 << (bone - 27);
            this->m_wheelAngles[bone - 27] = 0.0f;
        }
    }
}

// OFFSET: #0x00716050
void CGUnit_C::SetPredictedHealth(int32_t health) {
    this->m_predictedHealth = health;
}

// OFFSET: 0x005B7970/0x14022B330
float CGUnit_C::GetMaxCameraHeight(int32_t adjust) const {
    float height = this->m_movementData.m_collisionHeight;

    if (this->m_currentClientRace && (this->m_auraEffectMask[0x2C] & 0x40) && this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        height = this->m_worldModel->GetBoundingBox().t.z;
    }

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER) && height > 2.0277777f) {
        height = 2.0277777f;
    }

    return adjust ? height - 0.16666667f : height - 0.0f;
}

// PROVEN: 9bbbc25e IDENTICAL/NONE
// OFFSET: 0x005AFE80/0x140222CF0
uint32_t CGUnit_C::GetCreatureClassification() const {
    if (!this->m_creatureCacheEntry || this->m_unit->m_unitFieldPetnumber) {
        return 0;
    }

    return this->m_creatureCacheEntry->m_rank;
}

// OFFSET: 0x005AFEA0
int32_t CGUnit_C::IsDeepInLiquid() const {
    float depth = 0.0f;
    uint32_t liquidType;
    float height;
    int32_t flags;

    if (World::QueryObjectLiquid(this->m_worldObject, liquidType, height, flags)) {
        depth = height - this->GetPosition().z;
    }

    return depth > this->m_movementData.m_collisionHeight * 0.5f;
}

// OFFSET: 0x005AFFA0/0x140222E30
bool CGUnit_C::DoNotLogDeath() const {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return false;
    }

    if (this->m_creatureCacheEntry) {
        return (this->m_creatureCacheEntry->m_typeFlags[0] >> 10) & 1;
    }

    return true;
}

// PROVEN: 687e1a1e IDENTICAL/NONE
// OFFSET: 0x005B0190
uint32_t CGUnit_C::GetVirtualItemDisplayID(uint32_t slot) const {
    return this->m_virtualItemDisplayId[slot];
}

// OFFSET: 0x005B0200/0x1402230A0
bool CGUnit_C::FactionHasReputation(int32_t factionId) {
    FactionRec* faction = g_factionDB.GetRecord(factionId);
    return faction && faction->m_reputationIndex >= 0;
}

// OFFSET: 0x005B0240/0x1402230E0
CGUnit_C* CGUnit_C::GetControllingPlayer() {
    CGUnit_C* unit = this;
    const WOWGUID* controller = this->GetCharmerOrCreatorGuid();

    if (*controller) {
        controller = this->GetCharmerOrCreatorGuid();
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (!unit) {
        return NULL;
    }

    if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        controller = unit->GetCharmerOrCreatorGuid();
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return NULL;
        }
    }

    return unit;
}

// OFFSET: 0x005B0710/0x140223650
int32_t CGUnit_C::IsDisarmed(int32_t hand) const {
    if (hand == 2) {
        return 0;
    }

    if (this->m_unit->m_unitFieldFlags & 0x200000) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(0, true);

        if (!hand) {
            return item && item->m_classID == 2;
        }

        if (!this->IsDisarmed(0)) {
            const CGUnitVirtualItem* offhand = this->GetVirtualItem(1, true);

            if (offhand && offhand->m_classID == 2) {
                return 1;
            }
        }
    }

    return hand == 1 && (this->m_unit->m_unitFieldFlags2 & 0x80);
}

// PROVEN: 84d65699 IDENTICAL/NONE
// OFFSET: 0x005B0AE0/0x140223A70
int32_t CGUnit_C::IsQuestGiver() const {
    return (this->m_unit->m_unitNpcFlags >> 1) & 1;
}

// PROVEN: 11cbd21c IDENTICAL/NONE
// OFFSET: 0x005B0B00/0x140223A90
void CGUnit_C::UpdateInteractIcon() {
    if (this->m_unit->m_unitFieldHealth > 0 || (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x80))) {
        CGObject_C::UpdateInteractIcon();
    }
}

// PROVEN: 6f668816 IDENTICAL/NONE
// OFFSET: 0x005B0B30/0x140223AC0
void CGUnit_C::UpdateInteractIconAttach() {
    if (!this->m_interactIcon || !this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return;
    }

    if (this->m_interactIcon->m_attachParent) {
        this->m_interactIcon->DetachFromParent();
    }

    uint32_t attachment;

    if (this->m_mountModel && this->m_worldModel->HasAttachment(29)) {
        attachment = 29;
    } else {
        if (!this->m_worldModel->HasAttachment(18)) {
            return;
        }

        attachment = 18;
    }

    this->m_interactIcon->AttachToParent(this->m_worldModel, attachment, NULL, 0);
    this->UpdateInteractIconScale();
    this->ObjectNameVisibilityChanged();
}

// PROVEN: 59ecc7e4 IDENTICAL/NONE
// OFFSET: 0x005B0BD0/0x140223B90
void CGUnit_C::UpdateInteractIconScale() {
    if (!this->m_mountModel || this->m_mountModel->IsLoaded()) {
        CGObject_C::UpdateInteractIconScale();
    }
}

// PROVEN: ab2d33e9 IDENTICAL/NONE
// OFFSET: 0x005B0E10
CreatureSoundDataRec* CGUnit_C::GetMountSoundDataRec() const {
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);

    if (!displayInfo) {
        return NULL;
    }

    CreatureSoundDataRec* soundData = g_creatureSoundDataDB.GetRecord(displayInfo->m_soundID);

    if (soundData) {
        return soundData;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);
    return g_creatureSoundDataDB.GetRecord(modelData->m_soundID);
}

// OFFSET: #0x00719900
int32_t __stdcall CGUnit_C::GetRecordForTitle(int32_t maskId) {
    for (int32_t i = 0; i < g_charTitlesDB.m_numRecords; i++) {
        CharTitlesRec* rec = g_charTitlesDB.GetRecordByIndex(i);

        if (rec && rec->m_maskID == maskId) {
            return rec->m_id;
        }
    }

    return 0;
}

// OFFSET: 0x005B11B0/0x140224400
const char* CGUnit_C::GetUnitTitle() const {
    if (!this->m_creatureCacheEntry) {
        return NULL;
    }

    const char* title = this->m_creatureCacheEntry->m_subName;

    if (!*title || this->m_unit->m_unitFieldPetnumber) {
        return NULL;
    }

    return title;
}

// OFFSET: 0x005B19F0
int32_t CGUnit_C::IsSplashing(const C3Vector& position) const {
    uint32_t liquidType;
    float height;
    int32_t flags;

    if (!World::QueryObjectLiquid(this->m_worldObject, liquidType, height, flags)) {
        return 0;
    }

    int32_t flag8;
    int32_t flag9;
    World::QueryObjectLiquidFlags(this->m_worldObject, flag8, flag9);

    if (flag9 && World::GetWaterEpsilon() + height > position.z && !(this->m_movement->m_flags & 0x100000)) {
        return 1;
    }

    return 0;
}

// PROVEN: cd593bf8 IDENTICAL/NONE
// OFFSET: 0x005B22C0
uint32_t CGUnit_C::GetClientStandState() const {
    return this->m_unit->m_unitFieldBytes1[0];
}

// OFFSET: 0x005B2340
int32_t CGUnit_C::HasBakedNpcTexture() const {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1)) {
        return 1;
    }

    return 0;
}

// PROVEN: 38c34a4d IDENTICAL/NONE
// OFFSET: 0x005BA400/0x1402257C0
uint32_t CGUnit_C::GetDisplayRace() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_displayRaceID;
    }

    uint32_t race = this->m_currentClientRace;

    if (!race || (this->m_currentClientRaceFlags & 0x1)) {
        return this->m_unit->m_unitFieldBytes0[0];
    }

    return race;
}

// PROVEN: ac01fca3 IDENTICAL/NONE
// OFFSET: 0x005B2380
uint32_t CGUnit_C::GetDisplaySex() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_displaySexID;
    }

    return this->m_unit->m_unitFieldBytes0[2];
}

// OFFSET: 0x005B2B70
int32_t CGUnit_C::GetSpellRank(const SpellRec* spell) const {
    int32_t rank = 5 * this->m_unit->m_unitFieldLevel;

    if (spell->m_spellLevelsID) {
        SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

        if (levels && levels->m_maxLevel > 0 && rank >= 5 * levels->m_maxLevel) {
            rank = 5 * levels->m_maxLevel;
        }
    }

    if (rank < 0) {
        return 0;
    }

    return rank;
}

// OFFSET: 0x005B2FE0/0x1402263C0
void CGUnit_C::ApplyAlpha(uint32_t time) {
    CM2Model* model = this->GetObjectModel();
    CM2Model* worldModel = this->m_worldModel;
    float scale = static_cast<float>(this->m_alphaScale);
    float mountAlpha = 1.0f;

    if (model != worldModel && model) {
        if (this->m_mountFadeDuration) {
            if (static_cast<int32_t>(time - this->m_mountFadeStartTime - this->m_mountFadeDuration) < 0) {
                this->m_mountAlpha = this->m_mountFadeStartAlpha + (time - this->m_mountFadeStartTime) * (this->m_mountFadeEndAlpha - this->m_mountFadeStartAlpha) / this->m_mountFadeDuration;
            } else {
                this->m_mountFadeDuration = 0;
                this->m_mountAlpha = this->m_mountFadeEndAlpha;
            }
        }

        mountAlpha = static_cast<float>(this->m_mountAlpha) * scale * (1.0f / 65025.0f);
        model->m_fadeAlpha = mountAlpha;
    }

    if (!worldModel) {
        return;
    }

    if (this->m_fadeDuration) {
        if (static_cast<int32_t>(time - this->m_fadeStartTime - this->m_fadeDuration) < 0) {
            this->m_alpha = this->m_fadeStartAlpha + static_cast<int32_t>((time - this->m_fadeStartTime) * (this->m_fadeEndAlpha - this->m_fadeStartAlpha)) / static_cast<int32_t>(this->m_fadeDuration);
        } else {
            this->m_alpha = this->m_fadeEndAlpha;
            this->m_fadeDuration = 0;
        }
    }

    float alpha = static_cast<float>(this->m_alpha) * scale * (1.0f / 65025.0f);

    if (mountAlpha > 0.00000011920929f) {
        alpha = alpha / mountAlpha;

        if (alpha < 0.0f) {
            worldModel->m_fadeAlpha = 0.0f;
            return;
        }

        if (alpha >= 1.0f) {
            alpha = 1.0f;
        }
    }

    worldModel->m_fadeAlpha = alpha;
}

// OFFSET: 0x005B3470
int32_t CGUnit_C::GetSpellCastingTime(const SpellRec* spell) const {
    if (!spell) {
        return 0;
    }

    int32_t time = 0;
    SpellScalingRec* scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID);

    if (scaling) {
        if (!Spell_C_SpellScalingCastTime(this, scaling, time)) {
            return 0;
        }
    } else {
        SpellCastTimesRec* castTime = g_spellCastTimesDB.GetRecord(spell->m_castingTimeIndex);

        if (!castTime) {
            return 0;
        }

        int32_t rank = this->GetSpellRank(spell);
        int32_t baseLevel = 0;

        if (spell->m_spellLevelsID) {
            SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

            if (levels) {
                baseLevel = levels->m_baseLevel;
            }
        }

        time = castTime->m_base + castTime->m_perLevel * (rank / 5 - baseLevel);

        if (time < castTime->m_minimum) {
            time = castTime->m_minimum;
        }
    }

    int32_t attributes = spell->m_attributes;

    if (!(attributes & 0x30) && !(spell->m_attributesExC & 0x20000000) && time > 0) {
        float modCastSpeed = this->m_unit->m_unitModCastSpeed;

        if (fnotequal(modCastSpeed, 1.0f)) {
            time = static_cast<int32_t>(static_cast<float>(time) * modCastSpeed);
        }
    }

    if (attributes & 0x2) {
        time = 0x7FFFFFFF;
    }

    return time <= 0 ? 0 : time;
}

// PROVEN: bc5674eb IDENTICAL/NONE
// OFFSET: 0x005B3A60/0x140226FC0
uint32_t CGUnit_C::GetShapeshiftForm() const {
    if (this->m_shapeshiftSuppressed) {
        return 0;
    }

    return this->m_unit->m_unitFieldBytes2[3];
}

// OFFSET: 0x005B3AC0/0x140227020
int32_t CGUnit_C::IsInMelee() const {
    return static_cast<bool>(this->m_combat.m_attacking);
}

// OFFSET: 0x005B3AE0/0x140227030
int32_t CGUnit_C::IsInCombat() const {
    if (this->m_unit->m_unitFieldFlags & 0x800) {
        return 1;
    }

    return static_cast<bool>(this->m_combat.m_attacking);
}

// PROVEN: 250b1269 IDENTICAL/NONE
// OFFSET: 0x005B3B60
void CGUnit_C::UnRootEffects() {
    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        effect->UnrootFromCaster();
    }
}

// OFFSET: 0x005B4130
uint64_t CGUnit_C::GetCreatorGuid() const {
    CGUnitData* unit = this->m_unit;

    if (unit->m_unitFieldCharmedby || unit->m_unitFieldSummonedby) {
        return 0;
    }

    return unit->m_unitFieldCreatedby.m_guid;
}

// OFFSET: 0x005B4160/0x1402276C0
bool CGUnit_C::CanGainXP() const {
    CGUnitData* unit = this->m_unit;

    if (!unit->m_unitFieldPetnumber) {
        return false;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unitFieldCreatedby, TYPEMASK_PLAYER, __FILE__, __LINE__));
    return owner && owner->m_unit->m_unitFieldBytes0[1] == 3;
}

// OFFSET: 0x005B41E0/0x140227740
int32_t CGUnit_C::IsPVPLeader() const {
    if (!(this->m_unit->m_unitFieldBytes2[1] & 0x1)) {
        return 0;
    }

    if (!this->m_creatureCacheEntry || !this->m_creatureCacheEntry->m_racialLeader) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x005B42A0
void CGUnit_C::GetRangedAttackSkillRank(int32_t& rank, int32_t& bonus) {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        static_cast<CGPlayer_C*>(this)->GetRangedAttackSkillRank(rank, bonus);
        return;
    }

    rank = 0;
    bonus = 0;
}

// OFFSET: #0x0071B770
uint32_t CGUnit_C::GetBasePowerForType(int32_t powerType) const {
    switch (powerType) {
        case -2:
            return this->m_unit->m_unitFieldBaseHealth;

        case 0:
            return this->m_unit->m_unitFieldBaseMana;

        case 1:
        case 6:
            return 1000;

        case 2:
        case 3:
            return 100;

        case 5:
            return 1;

        default:
            return 0;
    }
}

// OFFSET: #0x0071B810
uint32_t CGUnit_C::GetCanFly() const {
    return this->m_movementData.m_flags & MOVEMENTFLAG_CAN_FLY;
}

// OFFSET: 0x005B44A0/0x140227A40
int32_t CGUnit_C::IsInMeleeRange(CGUnit_C* target) const {
    CGUnit* targetUnit = target ? static_cast<CGUnit*>(target) : NULL;
    float range = this->m_unit->m_unitFieldCombatreach + targetUnit->m_unit->m_unitFieldCombatreach + 1.3333334f;

    if (range < 5.0f) {
        range = 5.0f;
    }

    C3Vector targetPosition = target->GetPosition();
    C3Vector position = this->GetPosition();
    float dx = targetPosition.x - position.x;
    float dy = targetPosition.y - position.y;
    float dz = targetPosition.z - position.z;
    return range * range > dx * dx + dz * dz + dy * dy;
}

// OFFSET: 0x005AD100/0x14021F600
bool CGUnit_C::IsClientControlled() const {
    if (!(this->m_unit->m_unitFieldFlags & 0x2) && (this->m_unit->m_unitFieldFlags & 0xC00004)) {
        return false;
    }

    CGUnitData* unit = this->m_unit;
    uint32_t flags = unit->m_unitFieldFlags;

    if (flags & 0x1000000) {
        const WOWGUID* controller = this->GetCharmerOrCreatorGuid();
        CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*controller, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!owner || !(owner->m_obj->m_type & TYPEMASK_PLAYER)) {
            return false;
        }

        return !(owner->m_unit->m_unitFieldFlags & 0x1);
    }

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER) || unit->m_unitFieldCharmedby) {
        return false;
    }

    return !(flags & 0x1);
}

// OFFSET: 0x005AD1B0/0x14005C680
bool CGUnit_C::IsActivePlayer() {
    return this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
}

// PROVEN: 0fed68fd IDENTICAL/NONE
// OFFSET: 0x005B5050/0x140228770
uint32_t CGUnit_C::GetCurrentClientRace() const {
    if (!this->m_currentClientRace || (this->m_currentClientRaceFlags & 1)) {
        return this->m_unit->m_unitFieldBytes0[0];
    }

    return this->m_currentClientRace;
}

// OFFSET: 0x005B5070/0x140228790
uint32_t CGUnit_C::GetCurrentClientNativeDisplayID() const {
    if (!this->m_currentClientRace || (this->m_currentClientRaceFlags & 1)) {
        return this->m_unit->m_unitFieldNativedisplayid;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_currentClientRace);
    return this->m_unit->m_unitFieldBytes0[2] ? race->m_femaleDisplayID : race->m_maleDisplayID;
}

// OFFSET: 0x005ADDF0
bool CGUnit_C::IsMirrorImage() const {
    return (this->m_unit->m_unitFieldFlags2 & 0x10) || (this->m_clientFlags & 0x8020000);
}

// OFFSET: 0x005ADE20
void CGUnit_C::RequestMirrorImageDataFromServer() {
    this->m_clientFlags |= 0x20000;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GET_MIRROR_IMAGE_DATA));
    msg.Put(this->m_obj->m_guid.m_guid);
    msg.Put(static_cast<uint32_t>(this->m_unit->m_unitFieldDisplayid));
    ClientServices::Send2(&msg);
}

// OFFSET: #0x007171C0
int32_t CGUnit_C::ChooseEmoteAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t state = this->m_animationState;

    if ((!(state & 0x40) || this->m_mountModel) && !(state & 0x20)) {
        return (flags & 0x200) == 0;
    }

    int32_t emoteState = this->m_unit->m_unitNpcEmotestate;

    if (!emoteState) {
        return (flags & 0x200) == 0;
    }

    EmotesRec* emote = g_emotesDB.GetRecord(emoteState);

    if (!emote || (CGGameUI::m_interactTarget == this->m_obj->m_guid && (emote->m_emoteFlags & 0x2000))) {
        return (flags & 0x200) == 0;
    }

    if (flags & 0x200) {
        *anim = emote->m_animID;
    }

    return 1;
}

// OFFSET: 0x005AE270/0x140220AC0
uint32_t CGUnit_C::GetCurrentTorsoAnimId() const {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return 0xFFFFFFFF;
    }

    if (this->m_torsoKeyBone == -1) {
        return const_cast<CAnimKitManager&>(this->m_animKitManager).GetExpressingAnim(0xFFFFFFFF, 1, 1);
    }

    uint32_t anim = const_cast<CAnimKitManager&>(this->m_animKitManager).GetExpressingAnim(this->m_torsoKeyBone, 1, 1);

    if (anim == 0xFFFFFFFF) {
        return const_cast<CAnimKitManager&>(this->m_animKitManager).GetExpressingAnim(0xFFFFFFFF, 1, 1);
    }

    return anim;
}

// OFFSET: 0x005AE220/0x140220A50
int32_t CGUnit_C::HasDeathSequence() {
    uint32_t anim = this->m_animKitManager.FilterAnim(466, 0);

    if (anim == 0xFFFFFFFF) {
        anim = 466;
    }

    return this->m_worldModel && this->m_worldModel->HasSequence(anim);
}

// OFFSET: #0x007172B0
uint32_t CGUnit_C::GetCurrentBaseAnimId() const {
    if (this->m_worldModel && this->m_worldModel->IsLoaded()) {
        return this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    return 0xFFFFFFFF;
}

// OFFSET: #0x007172E0
uint32_t CGUnit_C::GetCurrentMountAnimId() const {
    if (this->m_mountModel && this->m_mountModel->IsLoaded()) {
        return this->m_mountModel->GetBoneSequenceId(0xFFFFFFFF);
    }

    return 0xFFFFFFFF;
}

// OFFSET: #0x007173F0
void CGUnit_C::GetCurrentAnimationInfo(M2BoneSequenceInfo* mount, M2BoneSequenceInfo* base, M2BoneSequenceInfo* torso, int32_t keepFinished) const {
    mount->m_sequenceId = 0;
    mount->m_variationIndex = 0;
    mount->m_time = 0;
    mount->m_speed = 0.0f;
    mount->m_startTime = 0;
    mount->m_endTime = 0;
    mount->m_finished = 0;
    mount->m_complete = 0;

    if (this->m_mountModel && this->m_mountModel->IsLoaded()) {
        this->m_mountModel->GetBoneSequenceInfo(0xFFFFFFFF, *mount);
    } else {
        mount->m_finished = 1;
    }

    if (mount->m_complete) {
        mount->m_finished = 1;
    }

    if (mount->m_finished) {
        mount->m_sequenceId = 0xFFFFFFFF;
        mount->m_speed = 1.0f;
        mount->m_variationIndex = 0xFFFFFFFF;
        mount->m_time = 0;
    }

    base->m_sequenceId = 0;
    base->m_variationIndex = 0;
    base->m_time = 0;
    base->m_speed = 0.0f;
    base->m_startTime = 0;
    base->m_endTime = 0;
    base->m_finished = 0;
    base->m_complete = 0;
    this->m_worldModel->GetBoneSequenceInfo(0xFFFFFFFF, *base);

    if (base->m_complete) {
        base->m_finished = 1;
    }

    if (base->m_finished && !keepFinished) {
        base->m_sequenceId = 0xFFFFFFFF;
        base->m_speed = 1.0f;
        base->m_variationIndex = 0xFFFFFFFF;
        base->m_time = 0;
    }

    torso->m_sequenceId = 0;
    torso->m_variationIndex = 0;
    torso->m_time = 0;
    torso->m_speed = 0.0f;
    torso->m_startTime = 0;
    torso->m_endTime = 0;
    torso->m_finished = 0;
    torso->m_complete = 0;

    if (this->m_torsoKeyBone == -1) {
        torso->m_finished = 1;
    } else {
        this->m_worldModel->GetBoneSequenceInfo(this->m_torsoKeyBone, *torso);
    }

    if (torso->m_complete) {
        torso->m_finished = 1;
    }

    if (torso->m_finished && !keepFinished) {
        torso->m_sequenceId = 0xFFFFFFFF;
        torso->m_speed = 1.0f;
        torso->m_variationIndex = 0xFFFFFFFF;
        torso->m_time = 0;
    }
}

// OFFSET: 0x005BDEC0/0x140233240
CreatureModelDataRec* CGUnit_C::GetModelData() const {
    CGUnitData* unit = this->m_unit;
    uint32_t displayId = unit->m_unitFieldNativedisplayid == unit->m_unitFieldDisplayid ? this->GetCurrentClientNativeDisplayID() : unit->m_unitFieldDisplayid;
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!displayInfo) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOCREATUREDISPLAYIDFOUND|%d", displayId);
        return NULL;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

    if (!modelData) {
        SysMsgPrintf(STATUS_WARNING, 16, "INVALIDDISPLAYMODELRECORD|%d|%d", displayInfo->m_modelID, displayInfo->m_id);
        return NULL;
    }

    return modelData;
}

// OFFSET: 0x005AE8C0/0x140221210
int32_t CGUnit_C::IsOwnedByActiveMover(void* unit) {
    if (!unit) {
        return 0;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGUnitData* data = static_cast<CGUnit_C*>(unit)->m_unit;
    const WOWGUID& owner = data->m_unitFieldCharmedby ? data->m_unitFieldCharmedby : data->m_unitFieldCreatedby;
    return mover->m_obj->m_guid == owner;
}

// PROVEN: 98c61116 IDENTICAL/NONE
// OFFSET: 0x005AE780
void CGUnit_C::PlayMountVisualKit() {
    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(2110);

    if (kit) {
        PlaySpellVisuaKitData data(NULL, kit, 1);
        data.m_someGUID = this->m_obj->m_guid;
        this->PlaySpellVisualKit(data);
    }
}

// PROVEN: f0a7dd91 IDENTICAL/NONE
// OFFSET: 0x005AE930/0x140221280
int32_t CGUnit_C::CanBeLooted(uint32_t time) const {
    CGUnitData* unit = this->m_unit;
    return unit->m_unitFieldHealth <= 0 && (!this->m_lootableTime || static_cast<int32_t>(time - this->m_lootableTime) >= 0) && (unit->m_unitDynamicFlags & 0x1);
}

// OFFSET: 0x005B4E80/0x1402285D0
float CGUnit_C::GetTrueScale() const {
    float scale = this->m_modelScale * this->m_scaleModifier * this->m_scale;

    if (this->m_mountModel) {
        return scale * this->m_mountScale;
    }

    return scale;
}

// OFFSET: #0x0071C260
void CGUnit_C::ModifyPredictedHealth(int32_t delta) {
    if (delta < 0 && (this->m_unit->m_unitFieldFlags2 & 0x100)) {
        return;
    }

    int32_t health = delta + this->m_predictedHealth;

    if (health < 1) {
        this->m_predictedHealth = 1;
    } else {
        if (health > static_cast<int32_t>(this->m_unit->m_unitFieldMaxhealth)) {
            health = this->m_unit->m_unitFieldMaxhealth;
        }

        this->m_predictedHealth = health;
    }
}

// OFFSET: 0x005BC610/0x1402310E0
void CGUnit_C::InitPredictedPower() {
    for (int32_t slot = 0; slot < 5; slot++) {
        int32_t powerType;

        if (this->m_obj->m_type & TYPEMASK_PLAYER) {
            powerType = CGUnit_C::s_powerTypeBySlot[this->m_unit->m_unitFieldBytes0[1]][slot];
        } else if (slot == 0) {
            powerType = this->m_unit->m_unitFieldBytes0[3];
        } else if (slot == 1) {
            powerType = 10;
        } else {
            continue;
        }

        if (powerType == 11) {
            continue;
        }

        this->m_predictedPower[slot] = this->GetPower(powerType);
    }
}

// OFFSET: #0x0071C390
void CGUnit_C::SetManaRegenInterrupt() {
    this->m_manaRegenInterruptTime = FrameTime::s_curTimeMs + 5000;
}

// OFFSET: 0x005B60D0
int32_t CGUnit_C::UsesNpcBakedTexture() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1)) {
        return 1;
    }

    if (!this->m_displayInfoExtra && (this->m_obj->m_type & TYPEMASK_PLAYER) && (this->m_modelData->m_flags & 0x4)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x005B61D0
int32_t __stdcall CGUnit_C::MaybeDeferTeleport(const WOWGUID* transport) {
    return *transport && !ClntObjMgrObjectPtr(*transport, TYPEMASK_OBJECT, __FILE__, __LINE__);
}

// OFFSET: #0x0071DF30
int32_t CGUnit_C::OverrideAnimForFlying(uint32_t* anim) const {
    if (anim[0] == 0xFFFFFFFF) {
        return 0;
    }

    M2SequenceInfo info = {};
    this->m_worldModel->GetSequenceInfo(anim[0], anim[1], info);

    if (info.m_animId && info.m_animId != 8 && info.m_animId != 25) {
        return 0;
    }

    anim[0] = 193;
    anim[1] = 0xFFFFFFFF;
    return 1;
}

// PROVEN: e3ac226c IDENTICAL/NONE
// OFFSET: 0x005B8800/0x14022C5E0
int32_t CGUnit_C::GetCreatureType() const {
    uint32_t form = this->m_shapeshiftSuppressed ? 0 : this->m_unit->m_unitFieldBytes2[3];
    SpellShapeshiftFormRec* shapeshift = g_spellShapeshiftFormDB.GetRecord(form);

    if (shapeshift && shapeshift->m_creatureType > 0) {
        return shapeshift->m_creatureType;
    }

    if (this->m_creatureCacheEntry) {
        return this->m_creatureCacheEntry->m_type;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    if (!race || race->m_creatureType <= 0) {
        return 0;
    }

    return race->m_creatureType;
}

// OFFSET: #0x0071F390
PetPersonalityRec* CGUnit_C::GetPetPersonality() const {
    CGUnitData* unit = this->m_unit;

    if (!unit->m_unitFieldPetnumber) {
        return NULL;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unitFieldCreatedby, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!owner || owner->m_unit->m_unitFieldBytes0[1] != 3) {
        return NULL;
    }

    return g_petPersonalityDB.GetRecord(1);
}

// OFFSET: 0x005B9060
void CGUnit_C::GetFootprintInfo(int32_t* textureId, float* size) const {
    float length;
    float width;

    if (this->m_mountDisplayId <= 0 || (this->m_clientFlags & 0x10000000) || !this->m_mountModel) {
        *textureId = this->m_footprintTextureId;
        float scale = this->GetTrueScale();
        length = this->m_footprintLength * scale;
        width = scale * this->m_footprintWidth;
    } else {
        *textureId = this->m_mountFootprintTextureId;
        float scale = this->GetTrueScale();
        length = this->m_mountFootprintLength * scale;
        width = scale * this->m_mountFootprintWidth;
    }

    size[1] = width;
    size[0] = length;
}

// OFFSET: 0x005B97B0
int32_t CGUnit_C::IsControlledByViewer() const {
    return this->m_obj->m_guid == ClntObjMgrGetActivePlayer() || this->m_obj->m_guid == s_activeMover || (this->m_clientFlags & 0x1000);
}

// OFFSET: 0x005B9D30/0x14022DE60
int32_t CGUnit_C::CanEquipItemsInThisForm() const {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_modelData && (this->m_modelData->m_flags & 0x4) && this->m_displayInfoExtra && (this->m_displayInfoExtra->m_flags & 0x1)) {
        return 1;
    }

    if (this->m_shapeshiftSuppressed) {
        return 1;
    }

    return this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid;
}

// OFFSET: 0x005BA430/0x14022E7A0
uint32_t CGUnit_C::GetVoiceSex() const {
    if (this->m_displayInfoExtra) {
        return this->m_displayInfoExtra->m_displaySexID;
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return static_cast<const CGPlayer_C*>(this)->m_player->m_playerBytes3[0];
    }

    return this->m_unit->m_unitFieldBytes0[2];
}

// OFFSET: 0x005BB3E0/0x14022FBB0
bool CGUnit_C::IsShapeShifted() const {
    if (this->m_shapeshiftSuppressed) {
        return false;
    }

    uint32_t form = this->m_unit->m_unitFieldBytes2[3];

    if (!form) {
        return false;
    }

    return !(g_spellShapeshiftFormDB.GetRecord(form)->m_flags & 0x1);
}

// OFFSET: 0x005B3A30
void CGUnit_C::PreemptivelyCancelShapeshift() {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        this->m_shapeshiftSuppressed = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORM, NULL);
    }
}

// OFFSET: 0x005BB690/0x14022FF60
bool CGUnit_C::IsCancelShapeshiftAtFlightMaster() const {
    if (this->m_shapeshiftSuppressed) {
        return false;
    }

    uint32_t form = this->m_unit->m_unitFieldBytes2[3];

    if (!form) {
        return false;
    }

    int32_t flags = g_spellShapeshiftFormDB.GetRecord(form)->m_flags;
    return !(flags & 0x1) || (flags & 0x800);
}

// PROVEN: 9559f55a IDENTICAL/NONE
// OFFSET: 0x005BB730
int32_t CGUnit_C::IsAutoTracking() const {
    return this->m_obj->m_guid == s_activeMover && s_trackingType != 13;
}

// PROVEN: 6b990ca5 IDENTICAL/NONE
// OFFSET: 0x005BB760
bool CGUnit_C::IsActiveMoverTracking(uint32_t typeMask) const {
    if (s_trackingType == 13) {
        return false;
    }

    return this->m_obj->m_guid == s_activeMover && ((1 << s_trackingType) & typeMask);
}

// OFFSET: #0x00722010
void CGUnit_C::TrackingMakeRelativeTo(const C44Matrix* transform, float facing) {
    if (this->m_obj->m_guid != s_activeMover) {
        return;
    }

    uint32_t type = s_trackingType;

    if (type == 13) {
        return;
    }

    if (type == 4) {
        s_trackingPosition *= *transform;
        type = s_trackingType;
    }

    if (type == 2 || type == 8) {
        s_trackingDirection = CMath::normalizeangle0to2pi_(s_trackingDirection + facing);
    }
}

// OFFSET: 0x005BBBF0/0x140230500
const char* CGUnit_C::GetFrameScriptText(const char* token, int32_t count) const {
    uint32_t sex;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        sex = static_cast<const CGPlayer_C*>(this)->m_player->m_playerBytes3[0];
    } else {
        sex = this->m_unit->m_unitFieldBytes0[2];
    }

    return FrameScript_GetText(token, count, sex == 1 ? GENDER_FEMALE : GENDER_MALE);
}

// OFFSET: 0x005BBFA0/0x140230980
C44Matrix CGUnit_C::GetMatrix() const {
    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        return this->m_vehicle->m_worldMatrix;
    }

    C44Matrix matrix;
    C3Vector position = this->GetRawPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;
    matrix.RotateAroundZ(this->GetRawFacing());

    if (this->GetTransportGUID()) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);
        matrix *= transport->GetMatrix();
    }

    return matrix;
}

// PROVEN: a1d4345f IDENTICAL/NONE
// OFFSET: 0x005BCEF0
uint64_t CGUnit_C::GetAuraCaster(uint32_t index) {
    const WOWGUID& creator = this->m_auras[index].m_creator;
    return creator.m_guid;
}

// OFFSET: #0x0056C220
bool CGUnit_C::IsInVehicle() const {
    return this->m_vehiclePassenger && this->m_vehiclePassenger->m_state;
}

// PROVEN: 0d32b8ce IDENTICAL/NONE
// OFFSET: 0x005AA540
int32_t Packet_SMSG_MULTIPLE_PACKETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    while (!msg->IsRead() && msg->m_read <= msg->m_size) {
        ClientServices::Connection()->ProcessMessage(time, msg, 1);
    }

    return 1;
}

// PROVEN: e5b069cb IDENTICAL/NONE
// OFFSET: 0x005AA590
int32_t Packet_SMSG_FORCE_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    char text[512];
    msg->Get(guid.m_guid);
    msg->GetString(text, sizeof(text));
    return 1;
}

// PROVEN: 1cc53f77 IDENTICAL/NONE
// OFFSET: 0x005AD900
int32_t Packet_SMSG_SET_AI_ANIM_KIT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    uint16_t animKitId = 0;
    msg->Get(animKitId);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->SetAIAnimKitID(animKitId);
    }

    return 1;
}

// OFFSET: 0x005AD960
int32_t Packet_SMSG_PLAY_ONE_SHOT_ANIM_KIT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint16_t animKitId = 0;
        msg->Get(animKitId);
        CAnimKitManager* manager = unit->m_mountAnimKitManager ? unit->m_mountAnimKitManager : &unit->m_animKitManager;
        manager->PlayAndForget(animKitId, 0xFFFFFFFF, NULL);
    }

    return 1;
}

// PROVEN: d35e75c2 IDENTICAL/NONE
// OFFSET: 0x005AD9D0
int32_t Packet_SMSG_SET_MOVEMENT_ANIM_KIT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint16_t animKitId = 0;
        msg->Get(animKitId);
        unit->SetMovementAnimKitID(animKitId);
    }

    return 1;
}

// PROVEN: 19ce8399 IDENTICAL/NONE
// OFFSET: 0x005ADA30
int32_t Packet_SMSG_SET_MELEE_ANIM_KIT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint16_t animKitId = 0;
        msg->Get(animKitId);
        unit->SetMeleeAnimKitID(animKitId);
    }

    return 1;
}

// OFFSET: 0x005ADA90
int32_t Packet_SMSG_SET_ANIM_TIER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        int32_t animTier = 0;
        msg->Get(animTier);

        if (animTier != unit->m_animTier) {
            unit->m_animTier = animTier;
            unit->UpdateAnimKitMgrTier();
        }
    }

    return 1;
}

// PROVEN: bf5853b5 IDENTICAL/NONE
// OFFSET: 0x005AA5D0
void CGUnit_C::DestroyLiquidPerturbator() {
    if (this->m_liquidPerturbator) {
        Liquid::DestroyPerturbator(static_cast<Liquid::EPertSurface>(0), this->m_liquidPerturbator);
        this->m_liquidPerturbator = NULL;
    }
}

// OFFSET: #0x00714C10
float CopySign(float value, float sign) {
    uint32_t bits = (*reinterpret_cast<uint32_t*>(&sign) ^ *reinterpret_cast<uint32_t*>(&value)) & 0x7FFFFFFF;
    bits ^= *reinterpret_cast<uint32_t*>(&sign);
    return *reinterpret_cast<float*>(&bits);
}

// PROVEN: 45db7ad2 IDENTICAL/NONE
// OFFSET: 0x005AD1E0
int32_t AnimTierChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        int32_t tier = unit->m_unit->m_unitFieldBytes1[3];

        if (tier != unit->m_animTier) {
            unit->m_animTier = tier;
            unit->UpdateAnimKitMgrTier();
        }
    }

    return 1;
}

// PROVEN: 866c792f IDENTICAL/NONE
// OFFSET: 0x005AD400
int32_t TargetChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_targetChangeTime = FrameTime::GetCurTimeMs();
    }

    return 1;
}

// PROVEN: cc1fa3e7 IDENTICAL/NONE
// OFFSET: 0x005AD440
int32_t UnitMaxItemLevelUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__);
    return 1;
}

// OFFSET: 0x005AD6E0
int32_t Packet_SMSG_PET_DISMISS_SOUND(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t modelId;
    msg->Get(modelId);
    C3Vector position(0.0f, 0.0f, 0.0f);
    msg->Get(position.x);
    msg->Get(position.y);
    msg->Get(position.z);
    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(modelId);

    if (!modelData) {
        return 1;
    }

    CreatureSoundDataRec* soundData = g_creatureSoundDataDB.GetRecord(modelData->m_soundID);

    if (soundData && soundData->m_soundPetDismissID) {
        position.z = position.z + 1.0f;
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit(soundData->m_soundPetDismissID, &position, NULL, NULL, __FILE__, __LINE__, "OnUnitPetDismissSound");
    }

    return 1;
}

// OFFSET: #0x00717310
float CGUnit_C::GetTorsoAnimProgress() const {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return 0.0f;
    }

    M2BoneSequenceInfo info;
    info.m_sequenceId = 0;
    info.m_variationIndex = 0;
    info.m_time = 0;
    info.m_speed = 0.0f;
    info.m_startTime = 0;
    info.m_finished = 0;
    info.m_complete = 0;
    info.m_endTime = 0;

    if (this->m_torsoKeyBone != -1) {
        this->m_worldModel->GetBoneSequenceInfo(this->m_torsoKeyBone, info);
    }

    if (info.m_sequenceId == 0xFFFFFFFF) {
        this->m_worldModel->GetBoneSequenceInfo(0xFFFFFFFF, info);
    }

    float duration = static_cast<float>(static_cast<int32_t>(info.m_endTime - info.m_startTime));

    if (duration < 0.00000011920929f) {
        return 0.0f;
    }

    float progress = info.m_time / duration;

    if (progress < 0.0f) {
        return 0.0f;
    }

    if (progress >= 1.0f) {
        return 1.0f;
    }

    return progress;
}

// OFFSET: #0x00717EC0
void CGUnit_C::BuildSmoothMatrix(C44Matrix& matrix) const {
    matrix = C44Matrix::RotationAroundZ(this->m_smoothTarget.m_value);

    C3Vector position = this->GetRawPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;

    CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (transport) {
        C44Matrix transportMatrix;

        if (transport->m_obj->m_type & TYPEMASK_UNIT) {
            static_cast<CGUnit_C*>(transport)->BuildSmoothMatrix(transportMatrix);
        } else {
            transportMatrix = transport->GetMatrix();
        }

        matrix *= transportMatrix;
    }
}

// OFFSET: 0x005B11E0
bool __stdcall CGUnit_C::CheckSpellPvPUsability(const SpellRec* spell, int32_t* error) {
    int32_t categoryRecoveryTime = 0;
    int32_t recoveryTime = 0;

    if (spell->m_spellCooldownsID) {
        SpellCooldownsRec* cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);

        if (cooldowns) {
            categoryRecoveryTime = cooldowns->m_categoryRecoveryTime;
            recoveryTime = cooldowns->m_recoveryTime;
        }
    }

    int32_t category = 0;

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories) {
            category = categories->m_category;
        }
    }

    Spell_C_AdjustCategoryCooldown(&categoryRecoveryTime, category, 0);
    bool arena = false;

    if (!CGGameUI::IsInArenaOrRatedBattleground(&arena)) {
        return true;
    }

    if (arena) {
        if (spell->m_attributesExD & 0x20000) {
            return true;
        }

        if ((spell->m_attributesExI & 0x200) || (spell->m_attributesExD & 0x10000)) {
            if (error) {
                *error = 199;
            }

            return false;
        }
    } else {
        if (spell->m_attributesExI & 0x4000) {
            return true;
        }

        if (spell->m_attributesExD & 0x10000) {
            if (error) {
                *error = 154;
            }

            return false;
        }
    }

    if (recoveryTime > 900000 || categoryRecoveryTime > 900000) {
        if (error) {
            *error = arena ? 199 : 154;
        }

        return false;
    }

    return true;
}

// OFFSET: 0x005B1A90/0x140224DB0
void CGUnit_C::PreloadNpcHelloSound() {
    NPCSoundsRec* npcSounds = g_npcSoundsDB.GetRecord(this->m_displayInfo->m_npcsoundID);

    if (!npcSounds) {
        return;
    }

    SoundEntriesRec* sound = g_soundEntriesDB.GetRecord(npcSounds->m_soundID[0]);

    if (!sound) {
        return;
    }

    int32_t fallback = SI3::GetFallbackSoundKitID(npcSounds->m_soundID[0]);

    if (fallback) {
        sound = g_soundEntriesDB.GetRecord(fallback);

        if (!sound) {
            return;
        }
    }

    char path[260];
    SStrCopy(path, sound->m_directoryBase, sizeof(path));
    SStrPack(path, "\\", sizeof(path));
    SStrPack(path, sound->m_file[0], sizeof(path));
    OsPathReplaceExtension(path, sizeof(path), path, "ogg");
    AsyncFileReadCreateObject(path, 2, 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
}

// PROVEN: c63f467b IDENTICAL/NONE
// OFFSET: 0x005B07A0
void CGUnit_C::RefreshNameDesc() {
    if (this->m_modelFlags & 0x10000) {
        return;
    }

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    WOWGUID guid = this->m_obj->m_guid;
    this->m_nameDesc = PlayerNameCreate(guid);
}

// OFFSET: #0x00719A90
void CGUnit_C::GetSwimMatrix(C44Matrix& matrix) const {
    C33Matrix rotation(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    rotation.FromEulerAnglesZYX(-(6.2831855f - this->GetDisplayFacing()), 6.2831855f - this->m_movementData.m_pitch, 0.0f);

    matrix.a0 = rotation.a0;
    matrix.a1 = rotation.a1;
    matrix.a2 = rotation.a2;
    matrix.a3 = 0.0f;
    matrix.b0 = rotation.b0;
    matrix.b1 = rotation.b1;
    matrix.b2 = rotation.b2;
    matrix.b3 = 0.0f;
    matrix.c0 = rotation.c0;
    matrix.c1 = rotation.c1;
    matrix.c2 = rotation.c2;
    matrix.c3 = 0.0f;
    matrix.d0 = 0.0f;
    matrix.d1 = 0.0f;
    matrix.d2 = 0.0f;
    matrix.d3 = 1.0f;
    matrix.Scale(this->GetTrueScale());

    C3Vector position = this->GetPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;
}

// OFFSET: 0x005B4250/0x1402277B0
int32_t CGUnit_C::IsLooting() const {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        const WOWGUID& target = static_cast<const CGPlayer_C*>(this)->m_lootTarget;
        return static_cast<bool>(target);
    }

    return (this->m_unit->m_unitFieldFlags >> 10) & 1;
}

// OFFSET: #0x0071BFF0
void CGUnit_C::ClearPrecastMissileModel(uint8_t castCount) {
    if (castCount && castCount != this->m_precastCastCount) {
        return;
    }

    CM2Model* model = this->m_precastMissileModel;

    if (model) {
        if (model->m_attachParent) {
            model->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = NULL;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000;
}

// PROVEN: 94fefe90 IDENTICAL/NONE
// OFFSET: 0x005B60A0/0x140229A20
uint64_t CGUnit_C::GetCameraRelativeTo() const {
    if (this->m_vehicleCamera && this->m_vehicleCamera->m_state) {
        return this->m_vehicleCamera->GetRelativeTo();
    }

    return this->GetTransportGUID();
}

// PROVEN: 78e4c042 IDENTICAL/NONE
// OFFSET: 0x005B6930
int32_t HoverHeightChangeHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.m_hoverHeight = unit->m_unit->m_unitFieldHoverheight;
    }

    return 1;
}

// OFFSET: #0x0071DB70
int32_t IsSubmergedDeathAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 466 && (behavior <= 468 || behavior == 472)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071DC20
int32_t IsAirborneAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec) {
        int32_t behavior = rec->m_behaviorID;

        if (behavior >= 37 && (behavior <= 40 || behavior == 467)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: #0x0071DD80
int32_t IsDyingAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (!rec) {
        return 0;
    }

    int32_t behavior = rec->m_behaviorID;

    if (behavior > 131) {
        if (behavior < 466 || behavior > 467) {
            return 0;
        }
    } else if (behavior != 131 && behavior != 1) {
        return 0;
    }

    return 1;
}

// PROVEN: 38ee30d1 IDENTICAL/NONE
// OFFSET: 0x005D6390
void CGUnit_C::GetDefenseSkillRank(int32_t& rank, int32_t& bonus) const {
    rank = 5 * this->m_unit->m_unitFieldLevel;
    bonus = 0;
}

// PROVEN: 1e3b350a IDENTICAL/NONE
// OFFSET: 0x005D63C0
void CGUnit_C::GetAttackSkillRank(int32_t hand, int32_t& rank, int32_t& bonus) const {
    rank = 5 * this->m_unit->m_unitFieldLevel;
    bonus = 0;
}

// OFFSET: #0x00737390
void CGUnit_C::SetMaxAlpha(float alpha) {
    CGUnit_C* transport;

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_state == 3 && (transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__)))) {
        CM2Model* model = this->GetObjectModel();

        if (model && model->m_attachParent == transport->GetObjectModel()) {
            this->m_alphaScale = static_cast<uint8_t>(CMath::fint_(255.0f));
        } else {
            this->m_alphaScale = static_cast<uint8_t>(CMath::fint_(alpha * 255.0f));
        }
    } else {
        this->m_alphaScale = static_cast<uint8_t>(CMath::fint_(alpha * 255.0f));
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec) {
        for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                unit->SetMaxAlpha(alpha);
            }
        }
    }
}

// OFFSET: 0x005AD040/0x14021F510
int32_t CGUnit_C::ShouldFadeIn() const {
    CGUnitData* unit = this->m_unit;

    if ((unit->m_unitFieldFlags2 & 0x20) || (unit->m_unitFieldFlags & 0x2) || (unit->m_unitFieldBytes1[2] & 0x2)) {
        return 0;
    }

    if (!this->HasVehicleTransport()) {
        return 1;
    }

    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!transport) {
        return 0;
    }

    VehicleSeatRec* seat = transport->GetVehicleSeatRec(static_cast<uint8_t>(this->m_movementData.m_transportSeat));
    return (seat && seat->m_attachmentID < 0) || transport->m_fadeDuration == 0;
}

// OFFSET: 0x005ADF40/0x140220720
int32_t CGUnit_C::IsRunning() const {
    return this->m_movementData.m_walkSpeed + this->m_movementData.m_walkSpeed >= this->m_movementData.GetBaseSpeed(0);
}

// OFFSET: #0x00717050
int32_t CGUnit_C::ChooseMovementAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t moveFlags = this->m_movementData.m_flags;

    if (!(moveFlags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))) {
        return (flags & 0xFFFFFFF8) == 0;
    }

    uint32_t state = this->m_animationState;

    if (!(state & 0x70) || !(flags & 0x4) || (state & 0x800000) || (state & 0x2000000)) {
        return 1;
    }

    if (moveFlags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) {
        if (moveFlags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            *anim = ANIM_SWIM_RIGHT - ((moveFlags & MOVEMENTFLAG_STRAFE_LEFT) != 0);
            return 1;
        }

        *anim = (moveFlags & MOVEMENTFLAG_BACKWARD) ? ANIM_SWIM_BACKWARDS : ANIM_SWIM;
        return 1;
    }

    if (this->IsOnFlyingSpline()) {
        *anim = ANIM_FLY;
        return 1;
    }

    if (moveFlags & MOVEMENTFLAG_BACKWARD) {
        *anim = ANIM_WALKBACKWARDS;
        return 1;
    }

    if (this->m_unit->m_unitFieldBytes1[2] & 0x2) {
        *anim = ANIM_STEALTH_WALK;
        return 1;
    }

    float speed = this->m_movementData.GetBaseSpeed(0);

    if (speed >= 11.0f) {
        *anim = ANIM_SPRINT;
        return 1;
    }

    if (this->m_movementData.m_walkSpeed + this->m_movementData.m_walkSpeed < speed) {
        *anim = ANIM_RUN;
        return 1;
    }

    *anim = ANIM_WALK;
    return 1;
}

// OFFSET: #0x00717AD0
float CGUnit_C::GetStandHeight() const {
    CreatureModelDataRec* modelData = this->m_modelData;

    if (!modelData) {
        modelData = this->GetModelData();

        if (!modelData) {
            return 0.0f;
        }
    }

    float height = modelData->m_geoBoxMaxZ - modelData->m_geoBoxMinZ;
    return this->GetTrueScale() * height;
}

// OFFSET: 0x005BE0C0/0x1402334B0
int32_t CGUnit_C::GetModelFileName(const char** name) const {
    CreatureModelDataRec* modelData = this->GetModelData();

    if (modelData) {
        *name = modelData->m_modelName;
        return *name != NULL;
    }

    *name = "Spells\\ErrorCube.mdx";
    return 1;
}

// OFFSET: 0x005AEEA0/0x140221980
C3Vector CGUnit_C::BuildSmoothPosition() const {
    C3Vector position = this->GetRawPosition();
    CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (transport) {
        C44Matrix transportMatrix;

        if (transport->m_obj->m_type & TYPEMASK_UNIT) {
            static_cast<CGUnit_C*>(transport)->BuildSmoothMatrix(transportMatrix);
        } else {
            transportMatrix = transport->GetMatrix();
        }

        position *= transportMatrix;
    }

    return position;
}

// OFFSET: 0x005B2740
C3Vector CGUnit_C::GetMissileFirePosition() const {
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded()) {
        if (model->HasEvent(0x4C534324)) {
            return model->GetEventPosition(0x4C534324);
        }

        if (model->HasEvent(0x52534324)) {
            return model->GetEventPosition(0x52534324);
        }

        if (model->HasEvent(0x54534324)) {
            return model->GetEventPosition(0x54534324);
        }

        if (this->m_vehiclePassenger) {
            return const_cast<CGUnit_C*>(this)->GetModelPosition();
        }
    }

    return this->GetPosition();
}

// OFFSET: 0x005B3B90/0x1402270F0
bool CGUnit_C::AnimSuppressesMovement() const {
    if (this->GetCurrentTorsoAnimId() > 659) {
        return false;
    }

    return (g_animationDataDB.GetRecord(this->GetCurrentTorsoAnimId())->m_flags >> 2) & 1;
}

// PROVEN: afab70b2 IDENTICAL/NONE
// OFFSET: 0x005B6970
int32_t Packet_SMSG_MOUNTSPECIAL_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && !unit->IsDeepInLiquid() && guid != ClntObjMgrGetActivePlayer()) {
        unit->OnSpecialMountAnim();
    }

    return 1;
}

// OFFSET: 0x005BDF60/0x140233300
CAaBox CGUnit_C::GetStandBox() const {
    CAaBox box;
    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.b.z = 0.0f;
    box.t.x = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;

    CreatureModelDataRec* modelData = this->m_modelData;

    if (modelData || (modelData = this->GetModelData())) {
        box = *reinterpret_cast<const CAaBox*>(&modelData->m_geoBoxMinX);
    }

    if (this->m_unit->m_unitFieldMountdisplayid > 0) {
        CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);

        if (displayInfo) {
            CreatureModelDataRec* mountModel = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

            if (mountModel) {
                float offset = mountModel->m_mountHeight - (box.t.z - box.b.z) * 0.5f;
                box.b.z = box.b.z + offset;
                box.t.z = offset + box.t.z;
                box |= *reinterpret_cast<const CAaBox*>(&mountModel->m_geoBoxMinX);
            }
        }
    }

    return box;
}

// OFFSET: #0x0071F400
uint32_t CGUnit_C::GetHappinessIndex() const {
    PetPersonalityRec* personality = this->GetPetPersonality();

    if (!personality) {
        return 0;
    }

    uint32_t index = 1;

    while (this->m_unit->m_unitFieldPower[4] >= static_cast<uint32_t>(personality->m_happinessThreshold[index])) {
        if (++index >= 3) {
            break;
        }
    }

    return index - 1;
}

// OFFSET: 0x005B8890
const CGUnitVirtualItem* CGUnit_C::GetVirtualItem(uint32_t slot, bool ignoreDisarm) const {
    if (!this->GetVirtualItemDisplayID(slot)) {
        return NULL;
    }

    if (ignoreDisarm) {
        return &this->m_virtualItem[slot];
    }

    if (slot == 0) {
        if (this->m_unit->m_unitFieldFlags & 0x200000) {
            const CGUnitVirtualItem* item = this->GetVirtualItem(0, true);

            if (item) {
                if (item->m_classID != 2) {
                    return &this->m_virtualItem[0];
                }

                return NULL;
            }
        }
    } else if (slot == 1) {
        if (this->m_unit->m_unitFieldFlags & 0x200000) {
            this->GetVirtualItem(0, true);

            if (!this->IsDisarmed(0)) {
                const CGUnitVirtualItem* offhand = this->GetVirtualItem(1, true);

                if (offhand && offhand->m_classID == 2) {
                    return NULL;
                }
            }
        }

        if (this->m_unit->m_unitFieldFlags2 & 0x80) {
            return NULL;
        }
    } else if (slot == 2) {
        if (this->m_unit->m_unitFieldFlags2 & 0x400) {
            return NULL;
        }
    }

    return &this->m_virtualItem[slot];
}

// PROVEN: 201c1b0c IDENTICAL/NONE
// OFFSET: 0x005B96C0/0x14022D620
C3Vector CGUnit_C::GetNamePosition() const {
    if (this->m_worldModel->IsLoaded()) {
        uint32_t anim = const_cast<CAnimKitManager&>(this->m_animKitManager).GetExpressingAnim(0xFFFFFFFF, 1, 0);

        if (AnimationDataHasFlags(anim, 0x2000000) && this->m_worldModel->HasAttachment(29)) {
            return this->m_worldModel->GetAttachmentPosition(29);
        }

        if (this->m_worldModel->HasAttachment(18)) {
            return this->m_worldModel->GetAttachmentPosition(18);
        }
    }

    C3Vector position = this->m_vehiclePassenger ? const_cast<CGUnit_C*>(this)->GetModelPosition() : this->GetPosition();
    position.z = this->m_height * this->m_scale * 1.25f + position.z;
    return position;
}

// OFFSET: 0x005C2510/0x140238760
bool CGUnit_C::DoNotPlayWoundAnim() const {
    return (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x8)) || this->GetCurrentTorsoAnimId() == 121 || const_cast<CGUnit_C*>(this)->IsMountSelf(0);
}

// OFFSET: 0x005BB8F0/0x140230170
void CGUnit_C::CreateFishingLineEffect() {
    CGObject_C* bobber = ClntObjMgrObjectPtr(this->m_unit->m_unitFieldChannelObject, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!bobber || !this->m_unit->m_unitChannelSpell || static_cast<CGGameObject_C*>(bobber)->m_gameObject->m_typeID != 17) {
        return;
    }

    for (CM2Model* attached = this->m_worldModel->m_attachList; attached; attached = attached->m_attachNext) {
        if (attached->m_attachmentId == 1) {
            if (!this->m_fishingLineEffect) {
                this->m_fishingLineEffect = CEffect::Create();
                WOWGUID target = this->m_unit->m_unitFieldChannelObject;
                this->m_fishingLineEffect->AddFishingLine(this, target);
            }

            return;
        }
    }
}

// OFFSET: #0x00724200
int32_t CGUnit_C::ChooseFallAnim(uint32_t* anim) const {
    int32_t falling = this->m_movementData.IsFalling();

    if (!falling) {
        return falling;
    }

    uint32_t current = (this->m_worldModel && this->m_worldModel->IsLoaded()) ? this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF) : 0xFFFFFFFF;
    *anim = IsAirborneAnim(current) ? ANIM_COUNT : ANIM_FALL;
    return 1;
}

// OFFSET: #0x00724F50
void CGUnit_C::SaveChainSpellTargets(const TSStackArray<WOWGUID>& targets) {
    this->m_chainSpellTargets.m_count = 0;
    uint32_t i = targets.m_count;

    while (i) {
        i--;
        const WOWGUID& target = targets.m_data[i];

        if (target != this->m_obj->m_guid) {
            this->m_chainSpellTargets.New(target);
        }
    }
}

// OFFSET: 0x005C2D70/0x140239120
bool CGUnit_C::HasAura(int32_t spellId, WOWGUID caster) {
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_auras[i].m_spellId == spellId && (!caster || this->m_auras[i].m_creator == caster)) {
            return true;
        }
    }

    return false;
}

// PROVEN: 623fb597 IDENTICAL/NONE
// OFFSET: 0x005BEF80/0x140234500
int32_t CGUnit_C::FindMountTypeID(int32_t& spellId) {
    spellId = 0;
    uint32_t effectCount = 0;
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        int32_t auraSpellId = this->m_auras[i].m_spellId;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(auraSpellId, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect || effect->m_effect != 6) {
                continue;
            }

            int32_t mountTypeID;

            if (effect->m_effectAura == 78) {
                mountTypeID = effect->m_effectMiscValueB;
            } else if (effect->m_effectAura == 36) {
                SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(effect->m_effectMiscValue);

                if (!form) {
                    continue;
                }

                mountTypeID = form->m_mountTypeID;
            } else {
                continue;
            }

            if (mountTypeID) {
                spellId = auraSpellId;
                return mountTypeID;
            }
        }
    }

    return 0;
}

// OFFSET: 0x005C22F0/0x1402384D0
uint32_t CGUnit_C::GetMountFlags() {
    uint32_t flags = 0;
    int32_t found = 0;
    uint32_t effectCount = 0;
    uint32_t count = this->GetAuraCount();

    if (count) {
        for (uint32_t i = 0; i < count; i++) {
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(this->m_auras[i].m_spellId, effectCount);

            for (uint32_t j = 0; j < effectCount; j++) {
                const SpellEffectRec* effect = effects->m_data[j];

                if (effect && effect->m_effectAura == 336) {
                    flags |= effect->m_effectMiscValue;
                    found = 1;
                }
            }
        }

        if (found) {
            return flags;
        }
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return flags;
    }

    AreaTableRec* area = CGGameUI::GetCurrentSubzoneRec();
    return area ? area->m_mountFlags : 0xFFFFFFFF;
}

// OFFSET: 0x005C9580/0x140240FC0
bool CGUnit_C::CheckMountRules(int32_t mountTypeID, SpellCastResult* reason) {
    if (!mountTypeID) {
        return true;
    }

    MountTypeRec* mountType = g_mountTypeDB.GetRecord(mountTypeID);

    if (!mountType) {
        return true;
    }

    int32_t ridingSkill = 5000;

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        ridingSkill = static_cast<CGPlayer_C*>(this)->GetSkillRank(762);
    }

    uint32_t mountFlags = this->GetMountFlags();

    for (int32_t i = 23; i >= 0; i--) {
        MountCapabilityRec* capability = g_mountCapabilityDB.GetRecord(mountType->m_capability[i]);

        if (!capability || ridingSkill < capability->m_requiredRidingSkill) {
            continue;
        }

        uint32_t capFlags = capability->m_flags;

        if ((capFlags & 0x1) && !(mountFlags & 0x1)) {
            continue;
        }

        if ((capFlags & 0x4) && !(mountFlags & 0x4)) {
            continue;
        }

        if ((capFlags & 0x8) && !(mountFlags & 0x8)) {
            continue;
        }

        if ((capFlags & 0x2) && !(mountFlags & 0x2)) {
            continue;
        }

        if (this->m_clientFlags & 0x4000000) {
            if (!(capFlags & 0x4)) {
                continue;
            }
        } else if (this->m_clientFlags & 0x200000) {
            if (!(capFlags & 0x8)) {
                continue;
            }
        } else if (!(capFlags & 0x1) && !(capFlags & 0x2)) {
            continue;
        }

        if (capability->m_requiredMap >= 0 && CGGameUI::m_iCurrentMapID != capability->m_requiredMap) {
            continue;
        }

        if (capability->m_requiredArea && this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !CGGameUI::IsInArea(capability->m_requiredArea)) {
            continue;
        }

        if (capability->m_requiredAura && !this->HasAura(capability->m_requiredAura, WOWGUID())) {
            continue;
        }

        if (capability->m_requiredSpell && !this->IsSpellKnown(capability->m_requiredSpell)) {
            continue;
        }

        return true;
    }

    if (reason) {
        *reason = SPELL_FAILED_NOT_HERE;
    }

    return false;
}

// OFFSET: 0x005CE740/0x1402473E0
bool CGUnit_C::CheckCurrentMountRules() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return true;
    }

    AreaTableRec* area = AreaListGetObjectArea(this->m_worldObject);
    int32_t noMount = 0;

    if (!area) {
        return true;
    }

    while (!noMount) {
        noMount = area->m_mountFlags & 0x10;
        area = g_areaTableDB.GetRecord(area->m_parentAreaID);

        if (!area) {
            break;
        }
    }

    if (!noMount) {
        return true;
    }

    int32_t spellId = 0;
    int32_t mountTypeID = this->FindMountTypeID(spellId);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell && (spell->m_attributes & 0xC000)) {
        if (World::QueryOutdoors(this->m_worldObject)) {
            if (spell->m_attributes & 0x4000) {
                return false;
            }
        } else if (spell->m_attributes & 0x8000) {
            return false;
        }
    }

    this->CheckMountRules(mountTypeID, NULL);
    return true;
}

// OFFSET: 0x005C2E00
bool CGUnit_C::HasAuraActive(int32_t spellId, WOWGUID caster) {
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        AuraData* aura = this->GetAura(i);

        if (aura->m_spellId == spellId && (!caster || aura->m_creator == caster)) {
            for (uint32_t effect = 0; effect < 3; effect++) {
                if ((1 << effect) & aura->m_flags) {
                    return true;
                }
            }
        }
    }

    return false;
}

// OFFSET: #0x007289C0
int32_t CGUnit_C::GetAuraIndex(int32_t spellId) {
    uint32_t count = this->GetAuraCount();

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_auras[i].m_spellId == spellId) {
            return i;
        }
    }

    return -1;
}

// OFFSET: #0x007226B0
bool CGUnit_C::IsPetBarUsed() const {
    if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x2000)) {
        return false;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->GetTransportGUID() != this->m_obj->m_guid.m_guid || !this->m_vehicle) {
        return true;
    }

    VehicleSeatRec* seat = this->m_vehicle->GetVehicleSeatRec(player->m_movementData.m_transportSeat);
    return !seat || seat->m_vehicleAbilityDisplay != 0;
}

// PROVEN: 2d613366 IDENTICAL/NONE
// OFFSET: 0x0056E0E0/0x1401CECD0
float CGUnit_C::GetFacing() const {
    return this->m_movement->GetFacing(this->m_movement->m_facing);
}

// PROVEN: 8a021453 IDENTICAL/NONE
// OFFSET: 0x005ABB10/0x140224040
float CGUnit_C::GetDisplayFacing() const {
    return this->m_movementData.GetFacing(this->m_renderFacing);
}

// OFFSET: #0x00715810
void CGUnit_C::ResetSmoothFacing() {
    this->unk_08D4 = 0.0f;
    this->unk_09FC &= ~0x7u;

    float facing = this->m_movementData.GetFacing(this->m_renderFacing);
    this->m_smoothFacing = facing;
    this->m_smoothFacingTarget = facing;
    this->unk_08D8 = 0.0f;
}

// OFFSET: 0x005AECD0/0x140221690
float CGUnit_C::GetSmoothFacing() const {
    float facing = 0.0f;
    const CGUnit_C* unit = this;

    while (unit) {
        facing += unit->m_smoothTarget.m_value;

        if (!unit->HasVehicleTransport()) {
            break;
        }

        unit = static_cast<const CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(unit->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    return unit->m_movement->GetFacing(facing);
}

// OFFSET: #0x0071E340
void CGUnit_C::ChooseDefaultAnim(uint32_t& anim, int32_t force) const {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state == 3 && passenger->GetLowerAnim(passenger->m_seatRec) != 506) {
        return;
    }

    if (this->m_animationState & 0x800004) {
        return;
    }

    uint32_t torso = this->GetCurrentTorsoAnimId();

    if (!force && GetAnimBehaviorId(torso) == 464) {
        anim = torso;
        return;
    }

    if (this->m_movementData.m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) {
        anim = ANIM_SWIM_IDLE;
        return;
    }

    if (this->m_unit->m_unitFieldBytes1[2] & 0x2) {
        anim = ANIM_STEALTH_STAND;
        return;
    }

    anim = this->m_movement->IsSplineFlyerOrHovering() ? ANIM_HOVER : ANIM_STAND;
}

// OFFSET: 0x005C0130/0x140235BB0
void CGUnit_C::UpdateSelectionRadius() {
    CAaBox box = this->GetStandBox();
    float dx = box.t.x - box.b.x;
    float dy = box.t.y - box.b.y;

    if (fabs(dx) < 0.00000023841858f && fabs(dy) < 0.00000023841858f) {
        this->m_selectionRadius = 1.2f;
        return;
    }

    this->m_selectionRadius = sqrt(dx * dx + dy * dy) * 0.5f;
    float scaled = this->GetTrueScale() * this->m_selectionRadius;
    this->m_selectionRadius = sqrt(scaled);

    if (scaled > 5.0f) {
        this->m_selectionRadius = this->m_selectionRadius + (scaled - 5.0f) * (scaled - 5.0f) * 0.059999999f;
    }

    if (this->m_selectionRadius > 10.0f) {
        this->m_selectionRadius = 10.0f;
    }
}

// OFFSET: #0x006FC290
void CGUnit_C::OnRemoteMissileTrajectoryCollision(MissileCollisionData* data) {
    for (CMissile* missile = this->m_missiles; missile; missile = missile->m_next) {
        if (missile->m_casterGuid == data->m_casterGuid && missile->m_castID == data->m_castID) {
            missile->SetRemoteCollisionPos(data->m_position);
        }
    }
}

// PROVEN: 7ff5622b IDENTICAL/NONE
// OFFSET: 0x005B0130
int32_t CGUnit_C::GetVirtualItemDisplayRec(uint32_t slot, ItemDisplayInfoRec& rec) const {
    return g_itemDisplayInfoDB.GetRecord(this->m_virtualItemDisplayId[slot], &rec);
}

// OFFSET: #0x00718C20
int32_t CGUnit_C::UnitReaction(const CGCorpse_C* corpse) const {
    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(this->m_unit->m_unitFieldFactiontemplate);
    FactionTemplateRec* other = g_factionTemplateDB.GetRecord(corpse->GetFactionTemplate());

    if (faction && other) {
        return FactionTemplateReaction(faction, other);
    }

    return 3;
}

// PROVEN: 09b619c0 IDENTICAL/NONE
// OFFSET: 0x005ACC40
bool CGUnit_C::IsDeadOrGhost() const {
    return this->m_unit->m_unitFieldHealth <= 0;
}

// PROVEN: 97be03e0 IDENTICAL/NONE
// OFFSET: 0x005ABCC0/0x14021E130
CreatureSoundDataRec* CGUnit_C::GetSoundData(int32_t ignoreOverride) const {
    if (this->m_soundDataOverride && !ignoreOverride) {
        return this->m_soundDataOverride;
    }

    return this->m_soundData;
}

// OFFSET: 0x005BA140/0x14022E420
void CGUnit_C::PlayNPCSound(bool greeting) {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return;
    }

    CGUnit_C::s_lastNPCSoundGUID = WOWGUID();
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->IsDeadOrGhost() || SE3::IsPlaying(this->m_npcInteractSound)) {
        return;
    }

    if (this->m_unit->m_unitFieldHealth <= 0 || (this->m_unit->m_unitDynamicFlags & 0x20) || !this->m_displayInfo) {
        return;
    }

    NPCSoundsRec* sounds = g_npcSoundsDB.GetRecord(this->m_displayInfo->m_npcsoundID);

    if (!sounds) {
        return;
    }

    C3Vector position = this->GetPosition();
    SE3SoundKitProperties properties;
    properties.m_highPriorityDownload = 1;

    if (greeting) {
        SE3::PlaySoundKit(sounds->m_soundID[0], &position, &properties, &this->m_npcInteractSound, __FILE__, __LINE__, "CGUnit_C::PlayNPCSound");
    } else {
        SE3::PlaySoundKit(sounds->m_soundID[1], &position, &properties, &this->m_npcInteractSound, __FILE__, __LINE__, "CGUnit_C::PlayNPCSound");
    }
}

// OFFSET: #0x00721DA0
bool CGUnit_C::CanCurrentFormMount() const {
    if (this->IsShapeShifted()) {
        return false;
    }

    if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        return true;
    }

    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldDisplayid);
    CreatureDisplayInfoExtraRec* extra = displayInfo ? g_creatureDisplayInfoExtraDB.GetRecord(displayInfo->m_extendedDisplayInfoID) : NULL;

    if (!extra) {
        return false;
    }

    if (displayInfo) {
        CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

        if (modelData && (modelData->m_flags & 0x80)) {
            return true;
        }
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(extra->m_displayRaceID);
    return race && (race->m_flags & 0x4);
}

// OFFSET: 0x005ADEC0/0x140220690
SummonPropertiesRec* CGUnit_C::GetSummonProperties() const {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(this->m_unit->m_unitCreatedBySpell, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && effect->m_effect == 28) {
            return g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);
        }
    }

    return NULL;
}

// PROVEN: 4232bb42 IDENTICAL/NONE
// OFFSET: 0x005B43C0
bool CGUnit_C::CanTurnWhileCasting() const {
    if (this->m_castSpellId) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);

        if (spell && (spell->m_attributesExE & 0x80000)) {
            return false;
        }
    }

    return true;
}

// PROVEN: 45d64cc0 IDENTICAL/NONE
// OFFSET: 0x0056E090/0x1401CEC80
C3Vector CGUnit_C::GetPosition() const {
    return this->m_movement->GetPosition(this->m_movement->m_position);
}

// PROVEN: c1911aae IDENTICAL/NONE
// OFFSET: 0x005AF240/0x140221F90
void CGUnit_C::OnFlightLocal(uint32_t eventTime, int32_t flying) {
    this->m_movementData.OnFlightLocal(eventTime, flying);

    if (flying) {
        CGUnit_C::s_flightStatusTime = eventTime;
    } else {
        CGUnit_C::s_flightEndTime = eventTime;
    }
}

// PROVEN: d452a30c IDENTICAL/NONE
// OFFSET: 0x005B3650/0x140226A90
void CGUnit_C::ToggleRunModeLocal(uint32_t eventTime) {
    this->m_movementData.OnSetRunModeLocal(eventTime, this->m_movementData.m_flags & MOVEMENTFLAG_WALKING);
}

// OFFSET: 0x005B8D30/0x14022CBC0
bool CGUnit_C::CanAssist(const CGCorpse_C* corpse) const {
    return this->UnitReaction(corpse) >= 4;
}

// OFFSET: 0x005C28E0
void CGUnit_C::UpdateAuraEffectMask() {
    memset(this->m_auraEffectMask, 0, sizeof(this->m_auraEffectMask));

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData* aura = this->GetAura(i);
        const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            if (!((1 << j) & aura->m_flags)) {
                continue;
            }

            const SpellEffectRec* effect = effects->m_data[j];

            if (effect) {
                int32_t auraType = effect->m_effectAura;
                this->m_auraEffectMask[auraType / 8] |= 1 << (auraType % 8);
            }
        }
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return;
    }

    for (PASSIVESPELLNODE* node = CGSpellBook::s_passiveSpells.Head(); node; node = CGSpellBook::s_passiveSpells.Next(node)) {
        const SpellRec* spell = g_spellDB.GetRecord(node->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect) {
                int32_t auraType = effect->m_effectAura;
                this->m_auraEffectMask[auraType / 8] |= 1 << (auraType % 8);
            }
        }
    }
}

// OFFSET: 0x005B7520/0x14022ADC0
void CGUnit_C::UpdateObjectEffectAnimationStates() {
    if (!this->m_effectManager) {
        return;
    }

    M2BoneSequenceInfo sequences[3];
    this->GetCurrentAnimationInfo(&sequences[0], &sequences[1], &sequences[2], 1);

    if (this->m_mountModel) {
        if (!this->m_mountModel->IsLoaded()) {
            return;
        }

        if (sequences[0].m_sequenceId < 506) {
            sequences[0].m_sequenceId = this->m_mountModel->GetFallbackSequenceIdFromSequenceId(sequences[0].m_sequenceId, NULL);
        }
    }

    if (this->m_worldModel) {
        if (!this->m_worldModel->IsLoaded()) {
            return;
        }

        if (sequences[1].m_sequenceId < 506) {
            sequences[1].m_sequenceId = this->m_worldModel->GetFallbackSequenceIdFromSequenceId(sequences[1].m_sequenceId, NULL);
        }

        if (sequences[2].m_sequenceId < 506) {
            sequences[2].m_sequenceId = this->m_worldModel->GetFallbackSequenceIdFromSequenceId(sequences[2].m_sequenceId, NULL);
        }
    }

    struct {
        uint32_t m_state;
        int32_t m_slot;
    } states[3];

    int32_t count = 0;

    for (int32_t slot = 0; slot < 3; slot++) {
        if (sequences[slot].m_sequenceId != 0xFFFFFFFF) {
            states[count].m_state = FindObjectEffectStateFromAnimID(sequences[slot].m_sequenceId, sequences[slot].m_variationIndex);
            states[count].m_slot = slot;
            count++;
        }
    }

    uint32_t previous[3];
    previous[0] = this->m_animEffectStates[0];
    previous[1] = this->m_animEffectStates[1];
    previous[2] = this->m_animEffectStates[2];

    for (int32_t i = 0; i < 3; i++) {
        if (this->m_animEffectStates[i]) {
            int32_t j;

            for (j = 0; j < count; j++) {
                if (this->m_animEffectStates[i] == states[j].m_state) {
                    break;
                }
            }

            if (j == count) {
                this->m_animEffectStates[i] = 0;
            }
        }
    }

    for (int32_t i = 0; i < count; i++) {
        uint32_t state = states[i].m_state;
        int32_t j;

        for (j = 0; j < 3; j++) {
            if (this->m_animEffectStates[j] == state) {
                break;
            }
        }

        if (j != 3) {
            continue;
        }

        if (state) {
            this->m_effectManager->EnterState(state, 1, 0);

            if (state == 15 || state == 81) {
                this->m_effectManager->ExitState(37, 1);
                this->m_effectManager->ExitState(38, 1);
                this->m_effectManager->ExitState(6, 1);
                this->m_effectManager->ExitState(7, 1);
                this->m_effectManager->ExitState(47, 1);
                this->m_effectManager->ExitState(48, 1);
                this->m_effectManager->ExitState(49, 1);
                this->m_effectManager->ExitState(50, 1);
                this->m_effectManager->ExitState(51, 1);
                this->m_effectManager->ExitState(52, 1);
                this->m_effectManager->ExitState(53, 1);
                this->m_effectManager->ExitState(54, 1);
                this->m_effectManager->ExitState(55, 1);
                this->m_effectManager->ExitState(56, 1);
                this->m_effectManager->ExitState(57, 1);
                this->m_effectManager->ExitState(58, 1);
                this->m_effectManager->ExitState(59, 1);
                this->m_effectManager->ExitState(60, 1);
                this->m_effectManager->ExitState(61, 1);
                this->m_effectManager->ExitState(62, 1);
                this->m_effectManager->ExitState(63, 1);
                this->m_effectManager->ExitState(64, 1);
                this->m_effectManager->ExitState(65, 1);
                this->m_effectManager->ExitState(66, 1);
            }
        }

        this->m_animEffectStates[states[i].m_slot] = state;
    }

    for (int32_t i = 0; i < 3; i++) {
        if (previous[i]) {
            int32_t j;

            for (j = 0; j < count; j++) {
                if (previous[i] == states[j].m_state) {
                    break;
                }
            }

            if (j == count) {
                this->m_effectManager->ExitState(previous[i], 1);
            }
        }
    }
}

// PROVEN: 222dbd51 IDENTICAL/NONE
// OFFSET: 0x0044E3F0
bool CGUnit_C::HasAuraEffect(int32_t auraType) const {
    return (this->m_auraEffectMask[auraType / 8] & (1 << (auraType % 8))) != 0;
}

// PROVEN: f714a36c IDENTICAL/NONE
// OFFSET: 0x005AC300/0x14021E850
int32_t __stdcall CGUnit_C::ResolveSheathState(int32_t hand, const CGUnitVirtualItem* mainHand, const CGUnitVirtualItem* offHand) {
    if (hand == 1) {
        if (!mainHand && (!offHand || offHand->m_inventoryType == 23)) {
            return 0;
        }
    } else if (hand == 0) {
        if ((offHand && offHand->m_inventoryType == 14) || (mainHand && mainHand->m_classID == 2) || (offHand && offHand->m_classID == 2)) {
            return 1;
        }
    }

    return hand;
}

// OFFSET: #0x00716040
void CGUnit_C::RequestRepopOnLanding() {
    this->m_clientFlags |= 0x100000;
}

// PROVEN: 3589697a IDENTICAL/NONE
// OFFSET: 0x005AE5F0
C3Spline_CatmullRom* CGUnit_C::GetSpline() {
    CMoveSpline* spline = this->m_movementData.m_spline;
    return spline ? &spline->m_spline : NULL;
}

// OFFSET: #0x00717890
float CGUnit_C::GetSplineElapsedTime() {
    CMoveSpline* spline = this->m_movementData.m_spline;
    return spline ? static_cast<float>(static_cast<int32_t>(spline->m_start)) : 0.0f;
}

// OFFSET: #0x007178B0
float CGUnit_C::GetSplineDuration() {
    CMoveSpline* spline = this->m_movementData.m_spline;
    return spline ? static_cast<float>(spline->m_duration) : 0.0f;
}

// OFFSET: 0x005B1320
bool __stdcall CGUnit_C::CheckIsInArea(int32_t areaId, int32_t areaGroupId, int32_t* checked) {
    if (checked) {
        *checked = 0;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (!area) {
        return false;
    }

    while (areaGroupId) {
        AreaGroupRec* group = g_areaGroupDB.GetRecord(areaGroupId);

        if (!group) {
            return false;
        }

        for (uint32_t i = 0; i < 6; i++) {
            if (!group->m_areaID[i]) {
                break;
            }

            if (checked) {
                (*checked)++;
            }

            if (area->m_id == group->m_areaID[i] || area->m_parentAreaID == group->m_areaID[i]) {
                return true;
            }
        }

        areaGroupId = group->m_nextAreaID;
    }

    return false;
}

// OFFSET: 0x005B2810
int32_t CGUnit_C::HasAttachmentForEffect(uint32_t id, int32_t raw) const {
    CM2Model* model = this->m_worldModel;

    if (!model || !model->IsLoaded()) {
        return 0;
    }

    if (raw) {
        return model->HasAttachment(id);
    }

    if (id == 13) {
        return model->HasAttachment(s_effectAttachmentIds[4]);
    }

    return model->HasAttachment(s_effectAttachmentIds[id]);
}

// OFFSET: #0x0071A860
C3Vector CGUnit_C::GetAttachmentPositionForEffect(uint32_t id, const C3Vector& offset, int32_t raw) {
    if (raw) {
        return this->GetAttachmentPosition(id, &offset);
    }

    if (id != 13) {
        return this->GetAttachmentPosition(s_effectAttachmentIds[id], &offset);
    }

    for (CM2Model* model = this->m_worldModel->m_attachList; model; model = model->m_attachNext) {
        if (model->m_attachmentId == 1) {
            for (int32_t attach = 4; attach >= 0; attach--) {
                if (model->HasAttachment(attach)) {
                    return offset * model->GetAttachmentWorldTransform(attach);
                }
            }

            break;
        }
    }

    return this->GetAttachmentPosition(s_effectAttachmentIds[4], &offset);
}

// OFFSET: 0x005B2AA0
bool CGUnit_C::CalculateVertexColor(int32_t time, CImVector* color) {
    if (!this->m_flashStartTime) {
        return false;
    }

    uint32_t start = this->m_flashStartTime + this->m_flashDelay;
    int32_t elapsed = time - start;

    if (elapsed < 0) {
        *color = this->m_flashColor;
        return true;
    }

    uint32_t duration = this->m_flashDuration;

    if (static_cast<int32_t>(time - duration - start) >= 0) {
        this->m_flashStartTime = 0;
        return false;
    }

    uint32_t t = static_cast<uint32_t>(static_cast<int64_t>((1.0 - static_cast<double>(elapsed) / static_cast<double>(duration)) * 255.0));
    CImVector flash = this->m_flashColor;

    if (t) {
        color->InterpColor(t, &flash);
    }

    return true;
}

// OFFSET: 0x005AC060
int32_t CGUnit_C::IsFacingUnchangedSinceSent() const {
    return fabs(this->GetRawFacing() - this->m_lastSentFacing) < 0.1f;
}

// OFFSET: #0x0071B580
float __fastcall CGUnit_C::ComputeTrackingElevationAngle(const C3Vector& position, const C3Vector& target) {
    float dx = position.x - target.x;
    float dy = position.y - target.y;
    float dz = position.z - target.z;
    float scale = 1.0f / sqrt(dz * dz + dy * dy + dx * dx);
    dx *= scale;
    dy *= scale;
    dz *= scale;

    if (fabs(dx) < 0.001f && fabs(dy) < 0.001f) {
        return target.z >= position.z ? -1.5707964f : 1.5707964f;
    }

    return asin(dz);
}

// OFFSET: 0x005BB9A0/0x140230230
bool CGUnit_C::ShouldKneelForLoot() const {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return !(this->m_unit->m_unitFieldFlags & 0x10000000);
    }

    CGObject_C* target = ClntObjMgrObjectPtr(static_cast<const CGPlayer_C*>(this)->m_lootTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!target) {
        return false;
    }

    uint16_t type = target->m_obj->m_type;

    if ((type & TYPEMASK_GAMEOBJECT) && static_cast<CGGameObject_C*>(target)->m_gameObject->m_typeID == 17) {
        return false;
    }

    if ((type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(target)->m_unit->m_unitFieldHealth > 0) {
        return false;
    }

    return !(type & TYPEMASK_ITEM);
}

// PROVEN: 95bbb636 IDENTICAL/NONE
// OFFSET: 0x005BB8D0/0x140230140
uint32_t CGUnit_C::GetLocalDisplayID() {
    if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        return this->GetCurrentClientNativeDisplayID();
    }

    return this->m_unit->m_unitFieldDisplayid;
}

// PROVEN: 46abadea IDENTICAL/NONE
// OFFSET: 0x005BEEA0
int32_t CGUnit_C::IsMountSelf(int32_t checkCasting) {
    if (checkCasting && this->m_castSpellId) {
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(this->m_castSpellId, count);

        for (uint32_t i = 0; i < effects->m_count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (effect && effect->m_effectAura == 78 && !effect->m_effectMiscValue) {
                return 1;
            }
        }
    }

    int32_t mountDisplayId = this->m_mountDisplayId;

    if (!mountDisplayId) {
        return 0;
    }

    if (this->m_displayTransition) {
        int32_t targetDisplayId = this->m_displayTransition->GetTargetDisplayID();

        if (targetDisplayId && targetDisplayId == mountDisplayId) {
            return 1;
        }
    }

    if (this->m_currentClientRace && mountDisplayId == static_cast<int32_t>(this->m_unit->m_unitFieldNativedisplayid)) {
        return 1;
    }

    return mountDisplayId == static_cast<int32_t>(this->GetLocalDisplayID());
}

// OFFSET: 0x005B2010
void CGUnit_C::SetEmoteQueue(const QUESTGIVEREMOTENODE* nodes, uint32_t count) {
    this->m_emoteQueue[0].m_emote = -1;

    if (!count) {
        return;
    }

    uint32_t queued = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (nodes[i].m_emote) {
            EmotesRec* emote = g_emotesDB.GetRecord(nodes[i].m_emote);

            if (emote && !emote->m_emoteSpecProc) {
                this->m_emoteQueue[queued].m_delay = nodes[i].m_delay;
                this->m_emoteQueue[queued].m_emote = nodes[i].m_emote;
                queued++;
            }
        }

        EmotesRec* current = g_emotesDB.GetRecord(this->m_emoteQueue[i].m_emote);

        if (current && current->m_animID) {
            this->m_animationState &= ~0x8000u;
            this->m_previousSheatheState = this->m_sheatheState;
        }
    }

    if (queued < 4) {
        this->m_emoteQueue[queued].m_emote = 0;
    }

    this->m_emoteQueue[0].m_delay += FrameTime::GetCurTimeMs();
}

// OFFSET: #0x00716470
C3Vector CGUnit_C::GetGroundNormal() const {
    return this->m_movement->GetGroundNormal();
}

// OFFSET: 0x005B4990
bool CGUnit_C::IsMovingToward(CGObject_C* target) const {
    C3Vector position = this->GetPosition();
    C3Vector targetPosition = target->GetPosition();
    C3Vector delta(targetPosition.x - position.x, targetPosition.y - position.y, targetPosition.z - position.z);
    C3Vector direction = this->m_movement->GetDirection();
    return direction.x * delta.x + direction.y * delta.y + direction.z * delta.z >= 0.0f;
}

// OFFSET: #0x00724280
int32_t CGUnit_C::ChooseLootAnim(uint32_t flags, uint32_t* animId) {
    if (this->m_mountModel || !(this->m_animationState & 0x40) || !this->ShouldKneelForLoot()) {
        return (flags & 0xFFFFFFF0) == 0;
    }

    uint32_t torso = this->GetCurrentTorsoAnimId();

    if (!this->IsLooting()) {
        if (torso != 188) {
            return (flags & 0xFFFFFFF0) == 0;
        }

        if (flags & 0x8) {
            *animId = 189;
        }

        return 1;
    }

    if (!(flags & 0x8) || torso == 188 || torso == 50) {
        return 1;
    }

    *animId = 50;
    return 1;
}

// OFFSET: 0x005B4A40
int32_t CGUnit_C::IsInFrontOfTargetWithGuid(const WOWGUID& guid) const {
    CGObject_C* target = ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__);
    return target && this->IsMovingToward(target);
}

// OFFSET: 0x005B0EA0
void CGUnit_C::UpdateWorldTransform(float elapsed) {
    C3Vector normal = this->m_movement->GetGroundNormal();

    if (normal.z >= 0.35721236f) {
        C3Vector delta(this->m_groundNormal.x - normal.x, this->m_groundNormal.y - normal.y, this->m_groundNormal.z - normal.z);
        float decay = pow(0.0018f, elapsed);
        this->m_groundNormal.x = delta.x * decay + normal.x;
        this->m_groundNormal.y = decay * delta.y + normal.y;
        this->m_groundNormal.z = delta.z * decay + normal.z;
    }

    CM2Model* model = this->GetObjectModel();

    if (!model) {
        return;
    }

    uint32_t animFlags = 0;
    M2BoneSequenceInfo info;

    if (model->IsLoaded() && model->GetBoneSequenceInfo(0xFFFFFFFF, info)) {
        const AnimationDataRec* animation = g_animationDataDB.GetRecord(info.m_sequenceId);

        if (animation) {
            animFlags = animation->m_flags & 0x200;
        }
    }

    float scale = this->GetTrueScale();
    float facing = this->GetDisplayFacing();
    model->SetWorldTransform(this->GetPosition(), facing, scale, this->m_groundNormal, animFlags ? 3 : 0xFFFFFFFF);
}

// OFFSET: #0x007178E0
void CGUnit_C::ReleaseEffectsBySpell(int32_t spellId) {
    CEffect* effect = this->m_procEffects;

    while (effect) {
        CEffect* next = effect->m_next;

        if (effect->m_spellId == spellId) {
            effect->Release();
        }

        effect = next;
    }
}

// OFFSET: #0x00735A60
int32_t CGUnit_C::UnsetKeyBoneSequence(CM2Model* model, uint32_t boneId, int32_t blend, int32_t primary, int32_t force) {
    if (!model) {
        return 0;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
        return 0;
    }

    int32_t result;

    if (model->IsLoaded() && model->BoneHasParent(boneId)) {
        result = 1;
        model->UnsetBoneSequence(boneId, blend, primary);
    } else {
        result = 0;
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec && vehicle->IsMirroringAnimsToPassengers() && model == this->GetObjectModel()) {
        for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
                unit->UnsetKeyBoneSequence(unit->GetObjectModel(), boneId, blend, primary, 1);
            }
        }
    }

    return result;
}

// OFFSET: #0x00735BB0
void CGUnit_C::SetBoneSequenceTimeRecursive(CM2Model* model, uint32_t boneId, uint32_t time, int32_t force) {
    if (!model) {
        return;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
        return;
    }

    model->SetBoneSequenceTime(boneId, time);

    CVehicle_C* vehicle = this->m_vehicle;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->IsMirroringAnimsToPassengers() || model != this->GetObjectModel()) {
        return;
    }

    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
            unit->SetBoneSequenceTimeRecursive(unit->GetObjectModel(), boneId, time, 1);
        }
    }
}

// OFFSET: #0x00735DD0
void CGUnit_C::UpdateBoneSequenceFlags(CM2Model* model, int32_t freeze, int32_t force) {
    if (!model) {
        return;
    }

    if (!force && this->m_vehiclePassenger && this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
        return;
    }

    if (freeze) {
        uint32_t time = model->m_scene->m_time;
        model->m_lastAnimTime = time ? time : 1;
    } else {
        model->m_lastAnimTime = 0;
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->IsMirroringAnimsToPassengers() || model != this->GetObjectModel()) {
        return;
    }

    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
            unit->UpdateBoneSequenceFlags(unit->GetObjectModel(), freeze, 1);
        }
    }
}

// OFFSET: #0x00738440
void CGUnit_C::ResetKeyBoneSequences() {
    CM2Model* model = this->m_worldModel;

    if (model) {
        if (this->m_animationState & 0x80) {
            uint32_t torsoBone = this->m_torsoKeyBone;

            if (!this->m_vehiclePassenger || !this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
                if (model->IsLoaded() && model->BoneHasParent(torsoBone)) {
                    model->UnsetBoneSequence(torsoBone, 1, 1);
                }

                CVehicle_C* vehicle = this->m_vehicle;

                if (vehicle && vehicle->m_vehicleRec && vehicle->IsMirroringAnimsToPassengers() && model == this->GetObjectModel()) {
                    for (CPassenger* passenger = this->m_vehicle->m_passengers.Head(); passenger; passenger = this->m_vehicle->m_passengers.Next(passenger)) {
                        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
                            unit->UnsetKeyBoneSequence(unit->GetObjectModel(), torsoBone, 1, 1, 1);
                        }
                    }
                }
            }
        }

        this->SetKeyBoneSequence(model, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0);
    }

    if (this->m_mountModel) {
        this->SetKeyBoneSequence(this->m_mountModel, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0);
    }
}

// OFFSET: #0x0061F890
uint8_t CGUnit_C::GetAuraLevel(uint32_t index) {
    return this->m_auras[index].m_level;
}

// OFFSET: #0x0061F8D0
uint8_t CGUnit_C::GetAuraStackCount(uint32_t index) {
    return this->m_auras[index].m_stackCount;
}

// PROVEN: 4d921ef3 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGUnit_C::GetAFKText(char* buffer, uint32_t size) const {
}

// PROVEN: 4f552ebd IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGUnit_C::GetDNDText(char* buffer, uint32_t size) const {
}

// PROVEN: 98c5ab58 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGUnit_C::GetGMText(char* buffer, uint32_t size) const {
}

// PROVEN: 391de47a IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGUnit_C::GetDevText(char* buffer, uint32_t size) const {
}

// PROVEN: 728231bd IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
const CGUnitVirtualItem* CGUnit_C::GetDefendingItem() const {
    return NULL;
}

// OFFSET: 0x005D5B30/0x1402504C0
CGUnit_C::CGUnit_C(uint32_t time, const CClientObjCreate& create)
    : CGObject_C(time, create),
      m_movementData(create.m_move.m_status.m_position, create.m_move.m_status.m_facing, this->m_obj->m_guid, this),
      m_smoothFacing(0.0f),
      unk_08D4(0.0f),
      unk_08D8(0.0f),
      m_smoothFacingTarget(0.0f),
      unk_08E0(-1),
      unk_08E4(),
      m_splashSoundKitId(0),
      m_npcInteractSound(0),
      m_npcSound(0),
      m_reactionSoundPriority(0),
      m_creatureCacheEntry(NULL),
      m_displayInfo(NULL),
      m_displayInfoExtra(NULL),
      m_modelData(NULL),
      m_soundData(NULL),
      m_soundDataOverride(NULL),
      m_bloodlevels(NULL),
      m_lastSquelchedEffectTime(0),
      m_squelchedEffectRate(0.0f),
      m_fishingLineEffect(NULL),
      m_auraEffects(NULL),
      unk_0948(NULL),
      m_mountModel(NULL),
      m_pendingDisplayModel(NULL),
      m_mountScale(1.0f),
      m_mountFadeStartTime(0),
      m_mountFadeDuration(0),
      m_mountAlpha(0xFF),
      m_nextBreathCheckTime(CGWorldFrame::s_currentWorldFrame->GetSceneTime()),
      m_mountDisplayId(this->m_unit->m_unitFieldMountdisplayid),
      m_mountTransition(NULL),
      m_mountTransitionEffect(NULL),
      m_mountTransitionSpellId(0),
      m_groundNormal(0.0f, 0.0f, 1.0f),
      m_lastMissileTime(0),
      m_missiles(NULL),
      m_pendingMissiles(NULL),
      m_shapeshiftSuppressed(0),
      unk_09BC(0),
      m_nextBloodPoolTime(0),
      m_combat(),
      m_stopAttackSent(0),
      m_animationState(0),
      m_footprintTextureId(-1),
      m_terrainTypeId(-1),
      m_footprintLength(0.27777779f),
      m_footprintWidth(0.27777779f),
      m_footprintParticleScale(1.0f),
      m_lastSentFacing(-10.0f),
      m_lastSentPitch(3.402823466e38f),
      m_nextRippleTime(0),
      m_castCount(0),
      m_castSpellId(0),
      m_castSchoolImmunities(0),
      m_castImmunities(0),
      m_castingSpellId(0),
      m_castingObject(),
      m_castStartTime(0),
      m_castEndTime(0),
      m_channelSpellId(0),
      m_channelStartTime(0),
      m_channelEndTime(0),
      m_pendingStateKitSpell(0),
      m_scriptRegistration(0),
      m_renderFacing(m_movementData.m_facing),
      m_facingVelocity(0.0f),
      m_facingBlend(1.0f),
      m_scriptedFacing(0.0f),
      m_lootableTime(0),
      m_loopSound(0),
      m_mountFootprintTextureId(0),
      m_mountFootprintLength(0.0f),
      m_mountFootprintWidth(0.0f),
      m_animEventPosition(0.0f, 0.0f, 0.0f),
      m_procEffects(NULL),
      unk_0AB0(),
      m_selectionRadius(0.0f),
      m_flashStartTime(0),
      m_flashColor(0),
      m_flashDelay(0),
      m_flashDuration(0),
      m_proceduralFadeAlpha(0xFF),
      m_proceduralFadeType(0),
      m_proceduralFadeStart(0),
      m_proceduralFadeDuration(0),
      m_proceduralFadeEndAlpha(0),
      unk_0AE4(),
      m_modelScale(1.0f),
      m_rangedWeaponModel(NULL),
      m_ammoModel(NULL),
      m_ammoDisplayId(0),
      m_characterComponent(NULL),
      m_previousSheatheState(this->m_unit->m_unitFieldBytes2[0]),
      m_sheatheState(this->m_unit->m_unitFieldBytes2[0]),
      m_chainSpellLocation(0.0f, 0.0f, 0.0f),
      m_mountAnimBehaviorId(91),
      m_mountSpellVisualKitID(0),
      m_animTier(this->m_unit->m_unitFieldBytes1[3]),
      m_torsoKeyBone(-1),
      m_pendingCombatActionAnim(-1),
      m_pendingCombatActionType(0),
      m_pendingCombatActionFlags(0),
      m_turnAnimTime(0),
      m_namePlateFrame(NULL),
      m_chatBubble(NULL),
      m_masterLooter(),
      m_looter(),
      unk_1020(0),
      m_vehicle(NULL),
      m_vehiclePassenger(NULL),
      m_vehicleCamera(NULL),
      m_missileTrajectorySpell(0),
      m_missileTrajectoryCastId(0),
      m_missileTrajectoryStart(0),
      m_missileTrajectoryEnd(0),
      m_missileTrajectoryReleased(0),
      m_wheelMask(0),
      m_lastWheelTime(0),
      m_specialIdleAnim(-1),
      m_precastMissileModel(NULL),
      m_precastCastCount(0),
      m_mountAnimKitManager(NULL),
      m_deathAnimKit(NULL),
      m_standStateAnimKit(NULL),
      m_movementAnimKit(NULL),
      m_lootAnimKit(NULL),
      m_precastAnimKit(NULL),
      m_castAnimKit(NULL),
      m_combatReadyAnimKit(NULL),
      m_combatActionAnimKit(NULL),
      m_woundAnimKit(NULL),
      m_mountedAnimKit(NULL),
      m_effectAnimKit(NULL),
      m_emoteAnimKit(NULL),
      m_aiAnimKit(NULL),
      m_jumpEndAnimKit(NULL),
      m_aiAnimKitID(0),
      m_movementAnimKitID(0),
      m_meleeAnimKitID(0),
      m_currentClientRace(0),
      m_currentClientRaceFlags(0),
      m_displayTransitionSound(0),
      m_camouflage(0.0f),
      m_camouflageFadeIn(0.0f),
      m_camouflageFadeOut(0.0f),
      m_activeHeadLook(NULL),
      m_displayTransition(NULL),
      m_predictedHealth(this->m_unit->m_unitFieldHealth),
      m_highestThreatGuid(),
      m_liquidPerturbator(NULL) {
    this->m_movement = &this->m_movementData;
    this->m_floodsafeTime[0] = 0;
    this->m_floodsafeTime[1] = 0;
    this->m_floodsafeOpcode[0] = 8229;
    this->m_floodsafeOpcode[1] = 8229;
    this->m_mountFadeStartAlpha = 0xFF;
    this->m_mountFadeEndAlpha = 0xFF;
    this->m_targetChangeTime = FrameTime::GetCurTimeMs() - 1000;
    this->unk_1041[0] = 0;

    this->m_manaRegenInterruptTime = 0;
    this->m_powerBarID = 0;

    this->m_animKitManager.SetGlueMode(1);
    this->unk_09FC = 0;
    this->m_clientFlags = 0x400000;
    this->SetClientInitData(create);
    memset(this->m_auraEffectMask, 0, sizeof(this->m_auraEffectMask));
    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);
    this->m_currentClientRace = race ? race->m_alteredFormRaceID : 0;
    this->m_currentClientRaceFlags = this->HasAuraEffect(358);

    if (static_cast<float>(this->m_unit->m_unitFieldHealth) / static_cast<float>(static_cast<int32_t>(this->m_unit->m_unitFieldMaxhealth)) < 0.2f && this->m_unit->m_unitFieldHealth > 0 && this->m_bloodlevels) {
        this->m_clientFlags |= 0x2;
    }

    this->m_weaponTrails[0] = NULL;
    this->m_weaponTrails[1] = NULL;
    this->m_emoteQueue[0].m_delay = 0;
    this->m_emoteQueue[1].m_delay = 0;
    this->m_emoteQueue[1].m_emote = 0;
    this->m_emoteQueue[2].m_delay = 0;
    this->m_emoteQueue[2].m_emote = 0;
    this->m_emoteQueue[3].m_delay = 0;
    this->m_emoteQueue[3].m_emote = 0;
    this->m_emoteQueue[0].m_emote = -1;
    this->InitPredictedPower();
    this->m_powerRegenRemainder[0] = 0;
    this->m_powerRegenRemainder[1] = 0;
    this->m_powerRegenRemainder[2] = 0;
    this->m_powerRegenRemainder[3] = 0;
    this->m_powerRegenRemainder[4] = 0;

    for (int32_t i = 0; i < 3; i++) {
        ItemRec* rec = g_itemDB.GetRecord(this->m_unit->m_unitVirtualItemSlotId[i], WOWGUID(), NULL, NULL, false);
        CGUnitVirtualItem item;

        if (rec) {
            this->m_virtualItemDisplayId[i] = rec->m_displayInfoID;
            item.m_classID = rec->m_classID;
            item.m_subclassID = rec->m_subclassID;
            item.m_soundOverrideSubclassID = rec->m_soundOverrideSubclassID;
            item.m_material = rec->m_material;
            item.m_inventoryType = rec->m_inventoryType;
            item.m_sheatheType = rec->m_sheatheType;
            item.m_pad[0] = 0;
            item.m_pad[1] = 0;
        } else {
            item.m_classID = 0;
            item.m_subclassID = 0;
            item.m_soundOverrideSubclassID = 0;
            item.m_material = 0;
            item.m_inventoryType = 0;
            item.m_sheatheType = 0;
            item.m_pad[0] = 0;
            item.m_pad[1] = 0;
            this->m_virtualItemDisplayId[i] = 0;
        }

        this->m_virtualItem[i] = item;
    }

    this->RefreshDataPointers();

    if (create.m_isActivePlayer) {
        this->InitializeActivePlayerComponent();
    }
}

// OFFSET: 0x005BDA50
void CGUnit_C::SetClientInitData(const CClientObjCreate& create) {
    this->m_combat.SetClientInitData(create);

    if (create.m_hasVehicle && create.m_vehicleId) {
        this->CreateVehicleData(&create, create.m_vehicleId);
    }

    if (create.m_hasAnimKits) {
        this->UpdateAnimKitIDsFromClientInitData(create.m_animKits);
    }

    if ((this->m_modelFlags & 0x20000) && create.m_isActivePlayer) {
        return;
    }

    this->m_movementData.SetUpdateInfo(FrameTime::GetCurTimeMs(), &create.m_move, create.m_isActivePlayer);

    if (this->m_movementData.m_flags & 0x1000) {
        this->OnCollideFalling(0);
    }

    if (create.m_isActivePlayer) {
        this->m_clientFlags |= 0x80;
    }

    if (create.unk_0001) {
        this->m_animationState |= 0x8000;
    }

    if (create.unk_0003) {
        this->m_animationState |= 0x40000;
    } else {
        this->m_animationState &= ~0x40000;
    }

    if (create.unk_0004) {
        this->unk_09FC |= 0x40;
        s_npcFeedbackSuppressCount++;
    } else {
        this->unk_09FC &= ~0x40;
    }
}

// OFFSET: 0x005B8470
void CGUnit_C::OnCollideFalling(uint32_t moveFlags) {
    if ((this->m_movementData.m_flags & 0x1000) && this->m_unit->m_unitFieldHealth > 0) {
        CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

        if (!passenger || !passenger->m_state) {
            const UnitAnimationSet* animSet = this->SelectAnimationSet();
            this->PlayMovementAnimKit(2, animSet->m_fall, -1, 0);
        }
    }

    if ((moveFlags & 0x100000) && (this->m_movement->m_flags & 0x800)) {
        C3Vector position = this->GetPosition();
        this->PlaySplashSound(&position);
        this->CreateRipple(1);
    }
}

// OFFSET: 0x005CC430/0x140244970
void CGUnit_C::RefreshDataPointers() {
    uint32_t displayId = this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid ? this->GetCurrentClientNativeDisplayID() : this->m_unit->m_unitFieldDisplayid;
    this->m_displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!this->m_displayInfo) {
        SysMsgPrintf(STATUS_ERROR, 2, "NOUNITDISPLAYID|%d|%s", displayId, this->GetUnitName(NULL, 1));
        this->m_displayInfo = g_creatureDisplayInfoDB.m_numRecords > 0 ? reinterpret_cast<CreatureDisplayInfoRec*>(g_creatureDisplayInfoDB.m_records) : NULL;

        if (!this->m_displayInfo) {
            OsOutputDebugString("Error, NO creature display records found");
        }
    }

    CreatureDisplayInfoRec* displayInfo = this->m_displayInfo;
    this->m_displayInfoExtra = g_creatureDisplayInfoExtraDB.GetRecord(displayInfo->m_extendedDisplayInfoID);
    this->m_modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);
    this->RefreshSoundData();
    this->m_bloodlevels = g_unitBloodLevelsDB.GetRecord(this->m_displayInfo->m_bloodID);

    if (!this->m_bloodlevels) {
        this->m_bloodlevels = g_unitBloodLevelsDB.GetRecord(this->m_modelData->m_bloodID);

        if (!this->m_bloodlevels) {
            this->m_bloodlevels = g_unitBloodLevelsDB.m_numRecords > 0 ? reinterpret_cast<UnitBloodLevelsRec*>(g_unitBloodLevelsDB.m_records) : NULL;
        }
    }

    if (this->m_obj->m_type == (TYPEMASK_UNIT | TYPEMASK_OBJECT)) {
        this->m_creatureCacheEntry = g_creatureDBCache.GetRecord(this->m_obj->m_entryID, this->m_obj->m_guid, &CreatureQueryCallback, NULL, false);
        this->PreloadNpcHelloSound();
    }

    if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
        this->m_clientFlags |= 0x100;
    } else {
        this->m_clientFlags &= ~0x100u;
    }

    this->UpdateAnimKitSplitBody();

    if ((this->m_modelData->m_flags & 0x40) || (this->m_unit->m_unitFieldFlags2 & 0x80000)) {
        this->m_clientFlags |= 0x2000000;
    } else {
        this->m_clientFlags &= ~0x2000000u;
    }
}

// OFFSET: #0x0071CFA0
void CGUnit_C::InitializeActivePlayerComponent() {
    CCharacterComponent* component = CCharacterComponent::m_activePlayerComponent;

    if (!component) {
        return;
    }

    if (this->m_modelData && (this->m_modelData->m_flags & 0x4) && CGPlayer_C::s_displayId == this->m_unit->m_unitFieldDisplayid) {
        this->SetCharacterModel(CCharacterComponent::m_activePlayerComponent->m_data.m_model);
        this->m_characterComponent = component;
        CCharacterComponent::m_activePlayerComponent = NULL;
    } else {
        CCharacterComponent::FreeComponent(CCharacterComponent::m_activePlayerComponent);
        CCharacterComponent::m_activePlayerComponent = NULL;
    }
}

// OFFSET: 0x005DAB60/0x140256A80
void CGUnit_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_animKitManager.SetModel(this->m_worldModel, 0);
    this->m_movementData.UpdateTransportStatus(create.m_move.m_status);
    this->m_modelScale = this->GetModelScale(this->m_unit->m_unitFieldDisplayid);

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        this->OnMoveUpdate(time, 1, 1);
    }

    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();
    CGObject_C::PostInit(time, create, reenable);
    this->VehiclePassengerInit();
    MovementUpdateCameraYaw(this->m_obj->m_guid, this->m_movementData.m_transportGuid);
    this->SetModelCallbacks(this->m_worldModel);

    if (this->m_displayInfo) {
        CCharacterComponent::ApplyMonsterGeosets(this->m_worldModel, this->m_displayInfo);
        CCharacterComponent::ReplaceMonsterSkin(this->m_worldModel, this->m_displayInfo, this->m_modelData);

        if (this->m_modelData) {
            this->m_worldModel->m_fFlags ^= (this->m_worldModel->m_fFlags ^ (this->m_modelData->m_flags >> 7)) & 0x4;
        }
    }

    CreatureModelDataRec* modelData = this->m_modelData;
    this->m_footprintTextureId = modelData->m_footprintTextureID;
    this->m_footprintLength = modelData->m_footprintTextureWidth * 0.027777778f;
    this->m_footprintWidth = modelData->m_footprintTextureLength * 0.027777778f;
    this->m_footprintParticleScale = modelData->m_footprintParticleScale;

    if (create.m_isActivePlayer && modelData && (modelData->m_flags & 0x4) && CGPlayer_C::s_displayId == (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid ? this->GetCurrentClientNativeDisplayID() : this->m_unit->m_unitFieldDisplayid) && this->m_characterComponent) {
        this->m_clientFlags &= 0xFFBDFFFF;
        static_cast<CGPlayer_C*>(this)->RefreshVisibleItems();
    } else {
        if (this->m_characterComponent) {
            CCharacterComponent::FreeComponent(this->m_characterComponent);
            this->m_characterComponent = NULL;
            this->m_clientFlags = (this->m_clientFlags & 0xFFBDFFFF) | 0x400000;
        }

        this->m_clientFlags = (this->m_clientFlags & 0xFFBDFFFF) | 0x400000;
    }

    this->UpdateUnitCollisionBox(1, 1);
    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);

    if (race) {
        this->m_splashSoundKitId = race->m_splashSoundID;
    }

    float facing = this->GetRawFacing();
    this->m_smoothTarget.m_value = CMath::normalizeangle0to2pi_(facing);
    this->m_smoothTarget.m_velocity = 0.0f;
    this->m_turnDelta[0] = 0.0f;
    this->m_turnDelta[1] = 0.0f;
    this->m_turnDelta[2] = 0.0f;
    this->m_turnDelta[3] = 0.0f;
    this->Animate(0.0f);

    if (this->m_unit->m_unitFieldHealth > 0) {
        if (static_cast<int32_t>(this->m_unit->m_unitFieldMountdisplayid) > 0) {
            this->CreateUnitMount();

            if (!this->IsMountSelf(0) && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
                CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldMountdisplayid);

                if (displayInfo) {
                    CreatureModelDataRec* mountData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

                    if (mountData) {
                        static_cast<CGPlayer_C*>(this)->UpdateMountedCollisionBox(mountData->m_mountHeight, 1);
                    }
                }
            }
        }
    } else {
        this->m_lootableTime = time;
        this->ClearEffectList(0);
        this->ShowLootEffect();
    }

    this->CheckLoopSound();
    uint32_t groundType;

    if (!ClntObjMgrGetPlayerType() && World::QueryGroundType(this->m_worldObject, groundType)) {
        this->m_terrainTypeId = groundType;
    }

    this->UpdateScriptRegistration();
    this->RefreshNameDesc();
    this->UpdateBreathState(CGWorldFrame::s_currentWorldFrame->GetSceneTime());
    this->UpdateChannelEffects();

    if (!this->ShouldFadeIn()) {
        this->m_fadeDuration = 0;
        this->m_alpha = this->m_fadeEndAlpha;
    }

    this->InitAnims();

    if (reenable && !(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        this->unk_09BC = 10;
        this->PlaySpawnAnimKit();
    }

    AnimationDataRec* anim = g_animationDataDB.GetRecord(this->GetCurrentTorsoAnimId());

    if (anim && anim->m_behaviorID == 127) {
        this->m_fadeDuration = 0;
        this->m_alpha = this->m_fadeEndAlpha;
    }

    const WOWGUID& charmedBy = this->m_unit->m_unitFieldCharmedby;

    if (charmedBy) {
        Script_SendUnitSignal(charmedBy, 0);
    }

    const WOWGUID& summonedBy = this->m_unit->m_unitFieldSummonedby;

    if (summonedBy) {
        Script_SendUnitSignal(summonedBy, 2);
    }

    this->UpdatePartyMemberPetState();
    TotemInfo* totem = CGGameUI::GetTotemInfo(this->m_obj->m_guid);

    if (totem) {
        totem->m_name = this->GetUnitName(NULL, 1);
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TOTEM_UPDATE, "%d", totem->m_slot + 1);
    }

    this->m_movementData.SnapToGroundIfCloseEnough();
    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;

    if (vehicleRec && (vehicleRec->m_flags & 0x10000000)) {
        WOWGUID vehicleGuid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddVehicle(vehicleGuid);
    }

    this->m_clientFlags |= 0x80000;
    this->CreateOrDestroyObjectEffectManager();

    if (CGUnit_C::s_deferredClientControlUpdateGUID == this->m_obj->m_guid) {
        this->ExecuteClientControlUpdate(CGUnit_C::s_deferredClientControlUpdateGUID, CGUnit_C::s_deferredClientControlUpdateState);
        CGUnit_C::s_deferredClientControlUpdateGUID = WOWGUID();
    }

    CMissileCollision::MaybeAddUnitToSystem(this);
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_unit->m_unitFieldPetnumber && player && player->UnitReaction(this) <= 1) {
        WOWGUID petGuid = this->m_obj->m_guid;
        const WOWGUID* owner = this->m_unit->m_unitFieldCharmedby ? &this->m_unit->m_unitFieldCharmedby : &this->m_unit->m_unitFieldCreatedby;
        CGBattlefieldInfo::AddArenaOpponentPet(petGuid, *owner);
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->SignalPlayerGainsVehicleDataEvent();
    }
}

// PROVEN: b7807bb2 IDENTICAL/NONE
// OFFSET: 0x005BBF30/0x140239B10
float CGUnit_C::GetModelScale(uint32_t displayId) {
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!displayInfo) {
        return 1.0f;
    }

    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

    if (!modelData) {
        return 1.0f;
    }

    return this->GetModelScale(displayInfo, modelData);
}

// OFFSET: 0x005DAAB0/0x1402569B0
void CGUnit_C::OnMoveUpdate(uint32_t time, int32_t a3, int32_t a4) {
    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateWorldMatrix();
        this->m_vehicle->UpdatePassengerSoundPositions();
    }

    this->UpdateWorldObject(0);

    if (!this->m_worldObject || !World::QueryGroundType(this->m_worldObject, reinterpret_cast<uint32_t&>(this->m_terrainTypeId))) {
        this->m_terrainTypeId = -1;
    }

    this->UpdateFlightStatus(time);
    this->UpdateSwimmingStatus(time, a3);
    this->UpdateSoundPosition();

    if (static_cast<int32_t>(this->m_mountDisplayId) > 0 && !(this->m_clientFlags & 0x10000000) && !this->CheckCurrentMountRules()) {
        this->Dismount();
    }
}

// OFFSET: #0x007179D0
void CGUnit_C::SetCharacterModel(CM2Model* model) {
    if (this->m_worldModel == model) {
        return;
    }

    if (!this->m_mountModel) {
        this->CleanUpVehicleBoneAnimsBeforeObjectModelChange();
    }

    CM2Model* oldModel = this->SetModelStart(model);
    CMissile::UpdateAttachments(this->m_obj->m_guid);
    this->SetModelFinish(oldModel);
}

// OFFSET: 0x005B3A80
void CGUnit_C::SetAttacking(WOWGUID guid) {
    this->m_combat.SetAttacking(guid);
    this->m_combat.m_attackSent = 0;
    this->UpdateCombatReadyAnimKit(0);
}

// OFFSET: 0x005D3790/0x14024D970
void CGUnit_C::Reenable(const CClientObjCreate& create) {
    CGObject_C::Reenable(create);
    this->RefreshNameDesc();
    float facing = this->m_movementData.m_facing;
    this->m_smoothTarget.m_value = CMath::normalizeangle0to2pi_(facing);
    this->m_smoothTarget.m_velocity = 0.0f;
    this->m_turnDelta[0] = 0.0f;
    this->m_turnDelta[1] = 0.0f;
    this->m_turnDelta[2] = 0.0f;
    this->m_turnDelta[3] = 0.0f;
    this->m_facingVelocity = 0.0f;
    this->m_renderFacing = facing;
    this->m_facingBlend = 1.0f;
    this->m_predictedHealth = this->m_unit->m_unitFieldHealth;
    this->InitPredictedPower();
    this->m_powerRegenRemainder[0] = 0;
    this->m_powerRegenRemainder[1] = 0;
    this->m_powerRegenRemainder[2] = 0;
    this->m_powerRegenRemainder[3] = 0;
    this->m_powerRegenRemainder[4] = 0;
    this->m_manaRegenInterruptTime = 0;

    if (static_cast<float>(this->m_unit->m_unitFieldHealth) / static_cast<float>(static_cast<int32_t>(this->m_unit->m_unitFieldMaxhealth)) < 0.2f && !this->IsDeadOrGhost() && this->m_bloodlevels) {
        this->m_clientFlags |= 0x2;
    }

    this->m_animationState &= ~0x10u;

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->ClearEffectList(3);
    this->ResetAIAnimKit();

    if (this->m_worldModel) {
        this->m_animKitManager.Unfreeze();
    }

    this->ShowLootEffect();

    for (int32_t i = 0; i < 3; i++) {
        ItemRec* rec = g_itemDB.GetRecord(this->m_unit->m_unitVirtualItemSlotId[i], WOWGUID(), NULL, NULL, false);
        CGUnitVirtualItem item;

        if (rec) {
            this->m_virtualItemDisplayId[i] = rec->m_displayInfoID;
            item.m_classID = rec->m_classID;
            item.m_subclassID = rec->m_subclassID;
            item.m_soundOverrideSubclassID = rec->m_soundOverrideSubclassID;
            item.m_material = rec->m_material;
            item.m_inventoryType = rec->m_inventoryType;
            item.m_sheatheType = rec->m_sheatheType;
            item.m_pad[0] = 0;
            item.m_pad[1] = 0;
        } else {
            item.m_classID = 0;
            item.m_subclassID = 0;
            item.m_soundOverrideSubclassID = 0;
            item.m_material = 0;
            item.m_inventoryType = 0;
            item.m_sheatheType = 0;
            item.m_pad[0] = 0;
            item.m_pad[1] = 0;
            this->m_virtualItemDisplayId[i] = 0;
        }

        this->m_virtualItem[i] = item;
    }

    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;

    if (vehicleRec && (vehicleRec->m_flags & 0x10000000)) {
        WOWGUID vehicleGuid = this->m_obj->m_guid;
        CGBattlefieldInfo::AddVehicle(vehicleGuid);
    }

    this->m_previousSheatheState = this->m_sheatheState;
    this->UpdateObjectEffectMovementStates();
}

// OFFSET: 0x005D8340
void CGUnit_C::PostReenable(const CClientObjCreate& create) {
    this->SetClientInitData(create);
    CGObject_C::PostReenable(create);
    this->CheckLoopSound();
    this->UpdateChannelEffects();

    if (!this->ShouldFadeIn()) {
        this->m_fadeDuration = 0;
        this->m_alpha = this->m_fadeEndAlpha;
    }

    ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_unit->m_unitFieldBytes0[0]);
    this->m_currentClientRace = race ? race->m_alteredFormRaceID : 0;
    this->m_currentClientRaceFlags = this->HasAuraEffect(358);
    this->InitAnims();
    this->DestroyUnitMount(1);
    this->UpdateDisplayInfo(0);

    if (static_cast<int32_t>(this->m_unit->m_unitFieldMountdisplayid) > 0) {
        this->CreateUnitMount();
    }

    this->UpdateScriptRegistration();
    this->UpdatePartyMemberPetState();
    this->UpdateBreathState(CGWorldFrame::s_currentWorldFrame->GetSceneTime());

    if (this->m_unit->m_unitFieldHealth <= 0 && !this->m_lootableTime) {
        this->m_lootableTime = FrameTime::GetCurTimeMs();
        this->ShowLootEffect();
    }

    this->m_movementData.SnapToGroundIfCloseEnough();

    if (!this->m_vehiclePassenger) {
        this->VehiclePassengerInit();
    }

    if (CGUnit_C::s_deferredClientControlUpdateGUID == this->m_obj->m_guid) {
        this->ExecuteClientControlUpdate(CGUnit_C::s_deferredClientControlUpdateGUID, CGUnit_C::s_deferredClientControlUpdateState);
        CGUnit_C::s_deferredClientControlUpdateGUID = WOWGUID();
    }

    this->unk_09FC &= ~0x80u;
    this->m_floodsafeOpcode[0] = 8229;
    this->m_floodsafeOpcode[1] = 8229;
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (CGBattlefieldInfo::m_instanceType == 4 && this->m_unit->m_unitFieldPetnumber && player && player->UnitReaction(this) <= 1) {
        WOWGUID petGuid = this->m_obj->m_guid;
        const WOWGUID* owner = this->m_unit->m_unitFieldCharmedby ? &this->m_unit->m_unitFieldCharmedby : &this->m_unit->m_unitFieldCreatedby;
        CGBattlefieldInfo::AddArenaOpponentPet(petGuid, *owner);
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateSeatVacancy();
    }
}

// OFFSET: #0x0070CBA0
void CGUnit_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_unit = reinterpret_cast<CGUnitData*>(storage + OBJECT_END);
    this->m_unitMirror = saved + CGObject::TotalFieldsSaved();
}

// OFFSET: 0x005C5A60/0x14023C9F0
void CGUnit_C::OnDeath() {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        static_cast<CGPlayer_C*>(this)->OnDeath();
    }

    if (this->m_liquidPerturbator) {
        Liquid::DestroyPerturbator(Liquid::PertSurface_Water, this->m_liquidPerturbator);
        this->m_liquidPerturbator = NULL;
    }

    this->ExpediteDisplayTransition(1);
    this->m_combat.m_attacking = WOWGUID();
    this->m_combat.m_attackSent = 0;
    this->m_stopAttackSent = 0;
    this->UpdateCombatReadyAnimKit(0);

    if (this->m_worldModel && this->m_worldModel->IsLoaded()) {
        this->m_animKitManager.Unfreeze();
    }

    if (SE3::IsStoppable(this->m_npcInteractSound)) {
        SE3::StopSound(this->m_npcInteractSound, 0, __FILE__, __LINE__, "CGUnit_C::OnDeath");
    }

    if (SE3::IsStoppable(this->m_npcSound)) {
        SE3::StopSound(this->m_npcSound, 0, __FILE__, __LINE__, "CGUnit_C::OnDeath");
    }

    if (!this->m_vehiclePassenger || this->m_vehiclePassenger->m_state != 3 || (this->m_unit->m_unitFieldFlags2 & 0x20000)) {
        this->OnDeathAnimate();
    }

    WOWGUID guid;

    if (!(this->m_unit->m_unitDynamicFlags & 0x20) || (guid = this->m_obj->m_guid, !CGGameUI::IsRaidMemberOrPet(guid))) {
        guid = this->m_obj->m_guid;
        Spell_C_CancelSpellByTarget(guid, SPELL_FAILED_TARGETS_DEAD);
    }

    this->DisengageFromActivePlayer();

    if (!this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags[0] & 0x80)) {
        this->RemoveInteractIcon();
    }

    PlayerNameTriggerColorUpdate(this->m_nameDesc);

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateColorDisplay(this);
    }

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->ClearEffectList(0);
    this->ClearTrackingTarget(false, true);
    guid = this->m_obj->m_guid;
    CGGameUI::ClearInteractTarget(guid, 1);

    for (int32_t powerType = 0; powerType < 11; powerType++) {
        if (powerType == 10) {
            if (this->m_powerBarID) {
                UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(this->m_powerBarID);

                if (bar && (bar->m_flags & 0x10)) {
                    continue;
                }
            }
        } else if (powerType == 7) {
            continue;
        }

        int32_t slot;

        if (this->m_obj->m_type & TYPEMASK_PLAYER) {
            slot = CGUnit_C::s_powerSlotByType[this->m_unit->m_unitFieldBytes0[1]][powerType];

            if (slot == -1) {
                continue;
            }
        } else if (powerType == this->m_unit->m_unitFieldBytes0[3]) {
            slot = 0;
        } else if (powerType == 10) {
            slot = 1;
        } else {
            continue;
        }

        this->SetPredictedPowerSlot(slot, powerType, 0, 1);
    }
}

// OFFSET: 0x005C1DE0/0x140237EB0
void CGUnit_C::ClearTrackingTarget(bool a2, bool a3) {
    if (this->m_obj->m_guid != CGUnit_C::s_activeMover || CGUnit_C::s_trackingType == 13) {
        return;
    }

    if (CGUnit_C::s_trackingType == 3) {
        FrameScript_SignalEvent(SCRIPT_EVENT_AUTOFOLLOW_END, NULL);
    }

    uint32_t time = FrameTime::GetCurTimeMs();

    if (a2) {
        CGObject_C* target = ClntObjMgrObjectPtr(CGUnit_C::s_trackingGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (target) {
            C3Vector targetPosition = target->GetPosition();
            C3Vector position = this->GetPosition();
            CGUnit_C::s_trackingFacing = CalculateFacingTo(&position, &targetPosition);
            CGInputControl::GetActive()->ToggleControlBit(0x1000000, 1, time, 0);
        }
    }

    if (a3) {
        if (s_clickToMoveActions[CGUnit_C::s_trackingType].m_enabled) {
            this->TrackingStopMove(time);
        }

        CGInputControl::GetActive()->ToggleControlBit(0x1000000, 0, time, 0);
        CGInputControl::GetActive()->ToggleControlBit(0x400000, 0, time, 0);
    }

    CGUnit_C::s_trackingGUID = WOWGUID();
    CGUnit_C::s_unk_CA11D0 = 0.0f;
    CGUnit_C::s_trackingFlags = 0;
    CGUnit_C::s_unk_CA11D4 = 0;
    CGUnit_C::s_trackingType = 13;
}

// PROVEN: b0e3709d IDENTICAL/NONE
// OFFSET: 0x005AC030/0x1401207B0
void CGUnit_C::ClearWeaponTrailHandles() {
    for (int32_t i = 0; i < 2; i++) {
        if (this->m_weaponTrails[i]) {
            WeaponTrailClose(this->m_weaponTrails[i]);
            this->m_weaponTrails[i] = NULL;
        }
    }
}

// OFFSET: 0x005D63F0/0x140250F10
CGUnit_C::~CGUnit_C() {
    if (this->m_liquidPerturbator) {
        Liquid::DestroyPerturbator(Liquid::PertSurface_Water, this->m_liquidPerturbator);
        this->m_liquidPerturbator = NULL;
    }

    if (this->m_pendingDisplayModel) {
        this->m_pendingDisplayModel->SetLoadedCallback(NULL, NULL);
        this->m_pendingDisplayModel->Release();
        this->m_pendingDisplayModel = NULL;
    }

    this->ShutdownAnims();

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->RemoveMountModel();
    SE3::StopSound(this->m_npcInteractSound, 0, __FILE__, __LINE__, "CGUnit_C::~CGUnit_C");
    SE3::StopSound(this->m_displayTransitionSound, 0, __FILE__, __LINE__, "CGUnit_C::~CGUnit_C");
    this->KillCreatureLoopSound();
    this->DestroyNamePlate();

    if (this->m_chatBubble) {
        CGChatBubbleFrame::Remove(this->m_chatBubble);
        this->m_chatBubble = NULL;
    }

    if (!(this->m_modelFlags & 0x10000)) {
        if (this->m_nameDesc) {
            PlayerNameDelete(this->m_nameDesc);
        }

        this->m_nameDesc = NULL;
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGUnit_C::s_trackingType = 13;
        CGUnit_C::s_unk_CA11D4 = 0;
        CGUnit_C::s_activeMover = WOWGUID();
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && this->m_worldModel) {
        this->m_worldModel->m_lastAnimTime = 0;
    }

    if (this->m_characterComponent) {
        CCharacterComponent::FreeComponent(this->m_characterComponent);
        this->m_characterComponent = NULL;
        this->m_clientFlags = (this->m_clientFlags & 0xFFBDFFFF) | 0x400000;
    }

    this->ClearWeaponTrailHandles();

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_flags & 0x8) {
            effect->MoveToDestroyList();
            break;
        }
    }

    while (this->m_auraEffects) {
        this->m_auraEffects->Release();
    }

    while (this->m_procEffects) {
        this->m_procEffects->Release();
    }

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = NULL;
    }

    if (this->unk_0948) {
        this->unk_0948->DeleteSelf();
    }

    if (this->m_rangedWeaponModel) {
        this->m_rangedWeaponModel->SetLoadedCallback(NULL, NULL);
        this->m_rangedWeaponModel->m_drawCallback = NULL;
        this->m_rangedWeaponModel->Release();
    }

    if (this->m_ammoModel) {
        this->m_ammoModel->Release();
    }

    if (this->m_activeHeadLook) {
        s_activeHeadLookAllocator.PutData(this->m_activeHeadLook, NULL, 0);
        this->m_activeHeadLook = NULL;
    }

    g_nameDBCache.CancelCallback(this->m_obj->m_guid.m_guid, &NameQueryCallback, NULL);
    g_petNameCache.CancelCallback(this->m_unit->m_unitFieldPetnumber, &NameQueryCallback, NULL);

    if (this->m_precastMissileModel) {
        if (this->m_precastMissileModel->m_attachParent) {
            this->m_precastMissileModel->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = NULL;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000;
    this->DestroyVehicleData(0);

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->Free();
    }

    this->StopDisplayTransition();
    this->ClearEffectList(1);
}

// OFFSET: 0x005D7A70/0x140252DA0
void CGUnit_C::Disable() {
    if (this->m_liquidPerturbator) {
        Liquid::DestroyPerturbator(Liquid::PertSurface_Water, this->m_liquidPerturbator);
        this->m_liquidPerturbator = NULL;
    }

    this->ExpediteDisplayTransition(1);
    CGObject_C::Disable();
    this->ShutdownAnims();
    CMissileCollision::RemoveUnitFromSystem(this);
    VehicleRec* vehicleRec = this->m_vehicle ? this->m_vehicle->m_vehicleRec : NULL;

    if (vehicleRec && (vehicleRec->m_flags & 0x10000000)) {
        WOWGUID guid = this->m_obj->m_guid;
        CGBattlefieldInfo::RemoveVehicle(guid);
    }

    this->DestroyVehicleCamera();

    if (this->m_vehicle) {
        this->m_vehicle->RemoveAllPassengersLocally(1);
    }

    this->m_chainSpellTargets.Clear();
    this->KillCreatureLoopSound();

    if (this->m_effectManager) {
        for (int32_t i = 1; i < 106; i++) {
            this->m_effectManager->ExitState(i, 0);
        }
    }

    this->m_clientFlags &= ~0x2;
    this->ClearThreatList();

    if (this->m_namePlateFrame) {
        if (CGNamePlateFrame::GetNamePlateFocus() == this->m_namePlateFrame) {
            CGUnit_C::ClearNamePlateFocus();
        }

        CGNamePlateFrame::ReleaseNameplateFrame(this->m_namePlateFrame);
        this->m_namePlateFrame->Hide();
        this->m_namePlateFrame = NULL;
        this->ObjectNameVisibilityChanged();
    }

    if (this->m_chatBubble) {
        CGChatBubbleFrame::Remove(this->m_chatBubble);
        this->m_chatBubble = NULL;
    }

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    this->m_nameDesc = NULL;
    WOWGUID guid = this->m_obj->m_guid;

    if (!CGGameUI::IsRaidMemberOrPet(guid) || this->IsDuelingWith(static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)))) {
        CGGameUI::BreakTarget(this->m_obj->m_guid);
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera->m_targetGUID == this->m_obj->m_guid) {
        camera->MakeRelativeTo(WOWGUID());
    }

    this->m_movementData.OnUnitDisable();

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        if (CGUnit_C::s_trackingType != 13) {
            this->ClearTrackingTarget(false, true);
        }

        CGUnit_C::s_activeMover = WOWGUID();
    }

    guid = this->m_obj->m_guid;
    Spell_C_CancelSpellByCaster(guid);

    if (this->m_castSpellId) {
        this->m_castCount = 0;
        this->m_castSpellId = 0;
        this->UpdatePrecastAnimKit(0, WOWGUID());
        this->m_castingSpellId = 0;
        this->m_castStartTime = 0;
        this->m_castEndTime = 0;
        this->UpdateSpellCastBars();
        const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);

        if (spell) {
            guid = this->m_obj->m_guid;
            Script_SendUnitSpellSignal(guid, this->m_castCount, spell, SCRIPT_EVENT_UNIT_SPELLCAST_STOP);
        }
    }

    if (this->m_unit->m_unitChannelSpell) {
        this->OnChannelUpdate(0);
        this->m_channelSpellId = 0;
        this->m_channelStartTime = 0;
        this->m_channelEndTime = 0;
        this->UpdateSpellCastBars();
        const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

        if (spell) {
            guid = this->m_obj->m_guid;
            Script_SendUnitSpellSignal(guid, 0, spell, SCRIPT_EVENT_UNIT_SPELLCAST_CHANNEL_STOP);
        }
    }

    int32_t index;
    guid = this->m_obj->m_guid;

    if (CGPartyInfo::IsMemberPet(guid, &index) && index >= 0) {
        CGPartyMemberState* member = CGPartyInfo::m_members[index] ? &s_partyMemberStates[index] : NULL;
        this->FillPartyMemberPetState(&member->m_pet);
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
        msg.Put(member->m_guid.m_guid);
        ClientServices::Send2(&msg);
    }

    guid = this->m_obj->m_guid;

    if (CGRaidInfo::IsPetInRaid(guid, &index) && index >= 0) {
        CGRaidMember* raidMember = CGRaidInfo::m_members[index];
        CGPartyMemberState* member = raidMember->m_guid ? &raidMember->m_stats : NULL;
        this->FillPartyMemberPetState(&member->m_pet);
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
        msg.Put(member->m_guid.m_guid);
        ClientServices::Send2(&msg);
    }

    if (this->m_scriptRegistration) {
        this->m_scriptRegistration = 0;
        ScriptEventsUnregisterUnit(this);
    }

    if (this->m_activeHeadLook) {
        s_activeHeadLookAllocator.PutData(this->m_activeHeadLook, NULL, 0);
        this->m_activeHeadLook = NULL;
    }

    while (this->m_auraEffects) {
        this->m_auraEffects->Release();
    }

    while (this->m_procEffects) {
        this->m_procEffects->Release();
    }

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = NULL;
    }

    if (this->unk_0948) {
        this->unk_0948->DeleteSelf();
    }

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_flags & 0x8) {
            effect->MoveToDestroyList();
            break;
        }
    }

    while (this->m_missiles) {
        this->m_missiles->DeleteSelf();
    }

    while (this->m_pendingMissiles) {
        this->m_pendingMissiles->DeleteSelf();
    }

    this->ClearEffectList(2);

    for (uint32_t i = 0; i < (this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count); i++) {
        this->unk_0F9C[i] = 0;
    }

    if (this->m_animationState & 0x8) {
        this->m_animationState &= ~0x8;
        this->UpdateCombatReadyAnimKit(0);
    }

    for (int32_t i = 0; i < 2; i++) {
        if (this->m_weaponTrails[i]) {
            WeaponTrailClose(this->m_weaponTrails[i]);
            this->m_weaponTrails[i] = NULL;
        }
    }

    SE3::StopSound(this->m_npcInteractSound, 2000, __FILE__, __LINE__, "CGUnit_C::Disable");
    SE3::StopSound(this->m_displayTransitionSound, 2000, __FILE__, __LINE__, "CGUnit_C::Disable");
    this->ClearTrackingTarget(false, true);
    this->unk_0AE4[1] = 0;
    this->m_emoteQueue[0].m_delay = 0;
    this->m_emoteQueue[1].m_delay = 0;
    this->m_emoteQueue[1].m_emote = 0;
    this->m_emoteQueue[2].m_delay = 0;
    this->m_emoteQueue[2].m_emote = 0;
    this->m_emoteQueue[3].m_delay = 0;
    this->m_emoteQueue[3].m_emote = 0;
    this->m_animationState &= ~0x100;
    this->m_emoteQueue[0].m_emote = -1;
    TotemInfo* totem = CGGameUI::GetTotemInfo(this->m_obj->m_guid);

    if (totem) {
        CGGameUI::DestroyTotem(totem->m_slot);
    }

    g_nameDBCache.CancelCallback(this->m_obj->m_guid.m_guid, &NameQueryCallback, NULL);
    g_petNameCache.CancelCallback(this->m_unit->m_unitFieldPetnumber, &NameQueryCallback, NULL);

    if (this->m_precastMissileModel) {
        if (this->m_precastMissileModel->m_attachParent) {
            this->m_precastMissileModel->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = NULL;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000;
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (this->GetControllingPlayer() == player) {
        Spell_C_RemovePeriodicClientTrigger(this->m_obj->m_guid, 0);
    }

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->Uninit();
        this->m_vehiclePassenger = NULL;
    }

    if (this->unk_09FC & 0x40) {
        s_npcFeedbackSuppressCount--;
    }
}

// PROVEN: b83e445a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WeaponTrailsInitialize() {
}

// OFFSET: 0x005CD6B0
void CGUnit_C::OnSetRawFacingLocal(uint32_t eventTime, float facing) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    this->m_movementData.OnSetRawFacingLocal(eventTime, facing);

    if (this->GetClientStandState() == 3 && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
        static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
    }
}

// OFFSET: 0x005CD730
void CGUnit_C::OnSetPitchLocal(uint32_t eventTime, float pitch) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnSetPitchLocal(eventTime, pitch);
}

// OFFSET: 0x005B4FC0
int32_t CGUnit_C::ClampRawAngleToLegalFacingRange(float& angle) const {
    float min;
    float max;

    if (!this->m_movementData.ComputeLegalRawFacingRange(min, max)) {
        return 0;
    }

    float wrapped = angle;
    CMovement_C::WrapFacingToRange(wrapped, min, max);

    if (min > wrapped) {
        angle = CMath::normalizeangle0to2pi_(min);
        return 1;
    }

    if (wrapped > max) {
        angle = CMath::normalizeangle0to2pi_(max);
        return 1;
    }

    return 0;
}

// OFFSET: #0x0072D3F0
void CGUnit_C::OnTurnToAngleLocal(uint32_t eventTime, float facing) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    this->m_movementData.OnTurnToAngleLocal(eventTime, facing);

    if (this->GetClientStandState() == 3 && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
        static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
    }
}

// OFFSET: #0x0072D470
void CGUnit_C::OnPitchToAngleLocal(uint32_t eventTime, float pitch) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    this->m_movementData.OnPitchToAngleLocal(eventTime, pitch);

    if (this->GetClientStandState() == 3 && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
        static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
    }
}

// OFFSET: #0x007189F0
void CGUnit_C::OnFallLocal(uint32_t eventTime) {
    this->m_movementData.OnFallLocal(eventTime);
}

// OFFSET: 0x005B22A0/0x1402256D0
void CGUnit_C::ChangeStandState(uint32_t standState) {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        static_cast<CGPlayer_C*>(this)->ChangeStandState(standState);
    }
}

// PROVEN: 9a3954dc IDENTICAL/NONE
// OFFSET: 0x005B35E0/0x140226A20
void CGUnit_C::OnMoveStopLocal(uint32_t eventTime) {
    this->m_movementData.OnMoveStopLocal(eventTime);
}

// PROVEN: c271e722 IDENTICAL/NONE
// OFFSET: 0x005B35F0/0x140226A30
void CGUnit_C::OnStrafeStopLocal(uint32_t eventTime) {
    this->m_movementData.OnStrafeStopLocal(eventTime);
}

// PROVEN: 6edee790 IDENTICAL/NONE
// OFFSET: 0x005B3600/0x140226A40
void CGUnit_C::OnAscendDescendStopLocal(uint32_t eventTime) {
    this->m_movementData.OnAscendDescendStopLocal(eventTime);
}

// PROVEN: 82074ab6 IDENTICAL/NONE
// OFFSET: 0x005B3610/0x140226A50
void CGUnit_C::OnTurnStopLocal(uint32_t eventTime) {
    this->m_movementData.OnTurnStopLocal(eventTime);
}

// OFFSET: 0x005CD230
void CGUnit_C::OnMoveStartLocal(uint32_t eventTime, int32_t forward) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnMoveStartLocal(eventTime, forward);
}

// OFFSET: 0x005CD2E0/0x140245CE0
void CGUnit_C::OnStrafeStartLocal(uint32_t eventTime, int32_t left) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnStrafeStartLocal(eventTime, left);
}

// OFFSET: 0x005CD390
void CGUnit_C::OnAscendDescendStartLocal(uint32_t eventTime, int32_t ascend) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnAscendDescendStartLocal(eventTime, ascend);
}

// OFFSET: 0x005CD440
void CGUnit_C::OnTurnStartLocal(uint32_t eventTime, int32_t left) {
    int32_t channelSpell = this->m_unit->m_unitChannelSpell;
    const SpellRec* spell = g_spellDB.GetRecord(channelSpell);

    if (spell && spell->m_spellInterruptsID) {
        SpellInterruptsRec* interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID);

        if (interrupts && (interrupts->m_channelInterruptFlags & 0x10) && (spell->m_attributesEx & 0x4000) && this->IsAutoTracking()) {
            Spell_C_CancelChannelSpell(channelSpell);
        }
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnTurnStartLocal(eventTime, left);
}

// OFFSET: 0x005CD560
void CGUnit_C::OnPitchStartLocal(uint32_t eventTime, int32_t up) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnPitchStartLocal(eventTime, up);
}

// OFFSET: 0x005CD610/0x140245EF0
void CGUnit_C::OnPitchStopLocal(uint32_t eventTime) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    this->m_movementData.OnPitchStopLocal(eventTime);
}

// OFFSET: 0x005CD7E0/0x1402460F0
void CGUnit_C::TryJumpOrAscend(uint32_t eventTime) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }

    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);
    CreatureModelDataRec* modelRec = displayRec ? g_creatureModelDataDB.GetRecord(displayRec->m_modelID) : NULL;

    if ((modelRec && (modelRec->m_flags & 0x400)) || this->m_mountDisplayId <= 0 || (this->m_clientFlags & 0x10000000) || (this->m_movementData.m_flags & MOVEMENTFLAG_CAN_FLY) || ((this->m_movement->m_flags & 0xF) && !this->m_movementData.IsGravityDisabled())) {
        this->m_movementData.OnJumpLocal(eventTime);
        return;
    }

    if ((this->m_movement->m_flags & 0x30) || this->IsDeepInLiquid() || !this->IsActivePlayer()) {
        return;
    }

    CAnimKitManager* manager = this->m_mountAnimKitManager ? this->m_mountAnimKitManager : &this->m_animKitManager;
    uint32_t anim = manager->GetExpressingAnim(0xFFFFFFFF, 1, 1);

    if (anim == 94 || anim == 636) {
        return;
    }

    this->OnSpecialMountAnim();
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MOUNT_SPECIAL_ANIM));
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005AEC60/0x140221620
void CGUnit_C::SendTimeSkip(uint32_t skip) {
    if (this->m_movement->IsOnSpline() || (this->m_movementData.m_flags & MOVEMENTFLAG_UNK_40000000) || !skip) {
        return;
    }

    PlayerMoveTimeSkipped msg;
    msg.m_guid = this->m_obj->m_guid;
    msg.m_timeSkipped = skip;
    ClientServices::Send(&msg);
}

// OFFSET: 0x005B36D0
int32_t CGUnit_C::HandleFloodsafeTurnEvent(uint32_t time, uint32_t msgId) {
    if (msgId != MSG_MOVE_STOP_TURN) {
        this->m_floodsafeOpcode[0] = msgId;
        this->m_floodsafeTime[0] = time;
        return 0;
    }

    if (this->m_floodsafeOpcode[0] == 8229 || (this->unk_09FC & 0x100)) {
        if (!this->m_movementData.SendStopTurn(time)) {
            return 0;
        }

        this->unk_09FC &= ~0x100u;
    } else {
        if (this->IsFacingUnchangedSinceSent()) {
            this->m_floodsafeOpcode[0] = 8229;
            return 0;
        }

        if (!this->m_movementData.SendSetFacing(time)) {
            return 0;
        }
    }

    this->m_floodsafeOpcode[0] = 8229;
    return 1;
}

// OFFSET: 0x005B3770
int32_t CGUnit_C::HandleFloodsafePitchEvent(uint32_t time, uint32_t msgId) {
    if (msgId != MSG_MOVE_STOP_PITCH) {
        this->m_floodsafeOpcode[1] = msgId;
        this->m_floodsafeTime[1] = time;
        return 0;
    }

    if (this->m_floodsafeOpcode[1] == 8229 || (this->unk_09FC & 0x200)) {
        if (!this->m_movementData.SendStopPitch(time)) {
            return 0;
        }

        this->unk_09FC &= ~0x200u;
    } else {
        if (this->IsPitchUnchangedSinceSent()) {
            this->m_floodsafeOpcode[1] = 8229;
            return 0;
        }

        if (!this->m_movementData.SendSetPitch(time)) {
            return 0;
        }
    }

    this->m_floodsafeOpcode[1] = 8229;
    return 1;
}

// PROVEN: 8e937d58 IDENTICAL/NONE
// OFFSET: 0x005B3810
int32_t CGUnit_C::SendFloodsafeTurnEvent(uint32_t time) {
    int32_t sent = 0;

    switch (this->m_floodsafeOpcode[0]) {
        case MSG_MOVE_STOP_TURN: {
            sent = this->m_movementData.SendStopTurn(time);
            break;
        }

        case MSG_MOVE_START_TURN_RIGHT: {
            sent = this->m_movementData.SendStartTurnRight(time);
            break;
        }

        case MSG_MOVE_START_TURN_LEFT: {
            sent = this->m_movementData.SendStartTurnLeft(time);
            break;
        }
    }

    this->m_floodsafeOpcode[0] = 8229;
    return sent;
}

// PROVEN: 9b7d6510 IDENTICAL/NONE
// OFFSET: 0x005B3890
int32_t CGUnit_C::SendFloodsafePitchEvent(uint32_t time) {
    int32_t sent = 0;

    switch (this->m_floodsafeOpcode[1]) {
        case MSG_MOVE_START_PITCH_UP: {
            sent = this->m_movementData.SendStartPitchUp(time);
            break;
        }

        case MSG_MOVE_START_PITCH_DOWN: {
            sent = this->m_movementData.SendStartPitchDown(time);
            break;
        }

        case MSG_MOVE_STOP_PITCH: {
            sent = this->m_movementData.SendStopPitch(time);
            break;
        }
    }

    this->m_floodsafeOpcode[1] = 8229;
    return sent;
}

// OFFSET: 0x005B3910/0x140226E40
void CGUnit_C::UpdateFloodsafeMoveEvents(uint32_t time) {
    if (time - this->m_floodsafeTime[0] >= 200 && !this->SendFloodsafeTurnEvent(time) && (this->unk_09FC & 0x80) && fnotequalz(this->GetFacing(), this->m_lastSentFacing, 0.0049999999f)) {
        this->m_movementData.SendSetFacing(time);
    }

    if (time - this->m_floodsafeTime[1] >= 200) {
        this->SendFloodsafePitchEvent(time);
    }
}

// OFFSET: 0x005B67D0/0x140229CA0
int32_t CGUnit_C::IsBelowHalfBaseSpeed() const {
    return this->m_movementData.m_walkSpeed + this->m_movementData.m_walkSpeed < this->m_movementData.GetBaseSpeed(0);
}

// OFFSET: 0x005BAC20/0x14022F0E0
void CGUnit_C::UpdateObjectEffectMovementStates() {
    if (!this->m_effectManager) {
        return;
    }

    this->m_effectManager->EnterState((this->m_movement->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) ? 0x25 : 0x26, 1, 0);

    if (this->m_movement->m_flags & MOVEMENTFLAG_ASCENDING) {
        this->m_effectManager->EnterState(0x6, 1, 0);
    } else if (this->m_movement->m_flags & MOVEMENTFLAG_DESCENDING) {
        this->m_effectManager->EnterState(0x7, 1, 0);
    }

    this->m_effectManager->EnterState((this->m_movement->m_flags & (MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT)) ? 0x2F : 0x30, 1, 0);

    if (this->m_movement->m_flags & MOVEMENTFLAG_FLYING) {
        this->m_effectManager->EnterState(0x3D, 1, 0);

        if (this->m_movement->m_flags & MOVEMENTFLAG_FORWARD) {
            this->m_effectManager->EnterState(0x3E, 1, 0);
        }

        if (this->m_movement->m_flags & MOVEMENTFLAG_BACKWARD) {
            this->m_effectManager->EnterState(0x3F, 1, 0);
        }

        if (this->m_movement->m_flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            this->m_effectManager->EnterState(0x42, 1, 0);

            if (this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_LEFT) {
                this->m_effectManager->EnterState(0x40, 1, 0);
            }

            if (this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_RIGHT) {
                this->m_effectManager->EnterState(0x41, 1, 0);
            }
        }
    } else if (this->IsBelowHalfBaseSpeed()) {
        this->m_effectManager->EnterState(0x31, 1, 0);

        if (this->m_movement->m_flags & MOVEMENTFLAG_FORWARD) {
            this->m_effectManager->EnterState(0x32, 1, 0);
        }

        if (this->m_movement->m_flags & MOVEMENTFLAG_BACKWARD) {
            this->m_effectManager->EnterState(0x33, 1, 0);
        }

        if (this->m_movement->m_flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            this->m_effectManager->EnterState(0x36, 1, 0);

            if (this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_LEFT) {
                this->m_effectManager->EnterState(0x34, 1, 0);
            }

            if (this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_RIGHT) {
                this->m_effectManager->EnterState(0x35, 1, 0);
            }
        }
    } else if (this->IsRunning()) {
        this->m_effectManager->EnterState(0x37, 1, 0);

        if (this->m_movement->m_flags & MOVEMENTFLAG_FORWARD) {
            this->m_effectManager->EnterState(0x38, 1, 0);
        }

        if (this->m_movement->m_flags & MOVEMENTFLAG_BACKWARD) {
            this->m_effectManager->EnterState(0x39, 1, 0);
        }

        if (this->m_movement->m_flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            this->m_effectManager->EnterState(0x3C, 1, 0);

            if (this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_LEFT) {
                this->m_effectManager->EnterState(0x3A, 1, 0);
            }

            if (this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_RIGHT) {
                this->m_effectManager->EnterState(0x3B, 1, 0);
            }
        }
    }

    uint32_t flags = this->m_movement->m_flags;

    if (!(flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD))) {
        this->m_effectManager->ExitState(0x31, 1);
        this->m_effectManager->ExitState(0x32, 1);
        this->m_effectManager->ExitState(0x33, 1);
        this->m_effectManager->ExitState(0x37, 1);
        this->m_effectManager->ExitState(0x38, 1);
        this->m_effectManager->ExitState(0x39, 1);
        this->m_effectManager->ExitState(0x3D, 1);
        this->m_effectManager->ExitState(0x3E, 1);
        this->m_effectManager->ExitState(0x3F, 1);
    } else if (flags & MOVEMENTFLAG_FLYING) {
        this->m_effectManager->ExitState(0x37, 1);
        this->m_effectManager->ExitState(0x38, 1);
        this->m_effectManager->ExitState(0x39, 1);
        this->m_effectManager->ExitState(0x31, 1);
        this->m_effectManager->ExitState(0x32, 1);
        this->m_effectManager->ExitState(0x33, 1);
        this->m_effectManager->ExitState((this->m_movement->m_flags & MOVEMENTFLAG_FORWARD) ? 0x3F : 0x3E, 1);
    } else if (this->IsBelowHalfBaseSpeed()) {
        this->m_effectManager->ExitState(0x37, 1);
        this->m_effectManager->ExitState(0x38, 1);
        this->m_effectManager->ExitState(0x39, 1);
        this->m_effectManager->ExitState(0x3D, 1);
        this->m_effectManager->ExitState(0x3E, 1);
        this->m_effectManager->ExitState(0x3F, 1);
        this->m_effectManager->ExitState((this->m_movement->m_flags & MOVEMENTFLAG_FORWARD) ? 0x33 : 0x32, 1);
    } else if (this->IsRunning()) {
        this->m_effectManager->ExitState(0x3D, 1);
        this->m_effectManager->ExitState(0x3E, 1);
        this->m_effectManager->ExitState(0x3F, 1);
        this->m_effectManager->ExitState(0x31, 1);
        this->m_effectManager->ExitState(0x32, 1);
        this->m_effectManager->ExitState(0x33, 1);
        this->m_effectManager->ExitState((this->m_movement->m_flags & MOVEMENTFLAG_FORWARD) ? 0x39 : 0x38, 1);
    }

    flags = this->m_movement->m_flags;

    if (!(flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))) {
        this->m_effectManager->ExitState(0x34, 1);
        this->m_effectManager->ExitState(0x35, 1);
        this->m_effectManager->ExitState(0x36, 1);
        this->m_effectManager->ExitState(0x3A, 1);
        this->m_effectManager->ExitState(0x3B, 1);
        this->m_effectManager->ExitState(0x3C, 1);
        this->m_effectManager->ExitState(0x40, 1);
        this->m_effectManager->ExitState(0x41, 1);
        this->m_effectManager->ExitState(0x42, 1);
    } else if (flags & MOVEMENTFLAG_FLYING) {
        this->m_effectManager->ExitState(0x34, 1);
        this->m_effectManager->ExitState(0x35, 1);
        this->m_effectManager->ExitState(0x36, 1);
        this->m_effectManager->ExitState(0x3A, 1);
        this->m_effectManager->ExitState(0x3B, 1);
        this->m_effectManager->ExitState(0x3C, 1);
        this->m_effectManager->ExitState((this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_LEFT) ? 0x41 : 0x40, 1);
    } else if (this->IsBelowHalfBaseSpeed()) {
        this->m_effectManager->ExitState(0x3A, 1);
        this->m_effectManager->ExitState(0x3B, 1);
        this->m_effectManager->ExitState(0x3C, 1);
        this->m_effectManager->ExitState(0x40, 1);
        this->m_effectManager->ExitState(0x41, 1);
        this->m_effectManager->ExitState(0x42, 1);
        this->m_effectManager->ExitState((this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_LEFT) ? 0x35 : 0x34, 1);
    } else if (this->IsRunning()) {
        this->m_effectManager->ExitState(0x34, 1);
        this->m_effectManager->ExitState(0x35, 1);
        this->m_effectManager->ExitState(0x36, 1);
        this->m_effectManager->ExitState(0x40, 1);
        this->m_effectManager->ExitState(0x41, 1);
        this->m_effectManager->ExitState(0x42, 1);
        this->m_effectManager->ExitState((this->m_movement->m_flags & MOVEMENTFLAG_STRAFE_LEFT) ? 0x3B : 0x3A, 1);
    }

    this->m_effectManager->ExitState((this->m_movement->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) ? 0x26 : 0x25, 1);
    this->m_effectManager->ExitState((this->m_movement->m_flags & (MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT)) ? 0x30 : 0x2F, 1);

    if (!(this->m_movement->m_flags & MOVEMENTFLAG_ASCENDING)) {
        this->m_effectManager->ExitState(0x6, 1);
    }

    if (!(this->m_movement->m_flags & MOVEMENTFLAG_DESCENDING)) {
        this->m_effectManager->ExitState(0x7, 1);
    }
}

// OFFSET: 0x005C85D0/0x14023FC50
void CGUnit_C::CancelRangedMode() {
    this->m_animationState &= ~0x80u;

    if (!CGUnit_C::s_trackingType) {
        this->ClearTrackingTarget(false, true);
    }

    CM2Model* ammoModel = this->m_ammoModel;

    if (ammoModel && ammoModel->m_attachParent) {
        ammoModel->DetachFromParent();
    }

    CM2Model* model = this->m_rangedWeaponModel;

    if (model && model->IsLoaded() && model->GetBoneSequenceId(0xFFFFFFFF) != 161) {
        this->m_rangedWeaponModel->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: #0x0073AF00
void CGUnit_C::SetAnimType(uint8_t animTier) {
    if (!this->m_animTier && (animTier == 3 || animTier == 2)) {
        this->PlayBaseAnimation(ANIM_TO_FLY, 0);
    } else if ((this->m_animTier == 3 || this->m_animTier == 2) && !animTier) {
        this->PlayBaseAnimation(ANIM_TO_GROUND, 0);
    }

    this->m_animTier = animTier;
}

// PROVEN: 335fb0a9 IDENTICAL/NONE
// OFFSET: 0x005ABCE0/0x14021E150
void CGUnit_C::RegisterScript(uint32_t flags) {
    if (!this->m_scriptRegistration) {
        ScriptEventsRegisterUnit(this);
    }

    this->m_scriptRegistration |= flags;
}

// PROVEN: 9360abd5 IDENTICAL/NONE
// OFFSET: 0x005ABD20/0x14021E190
void CGUnit_C::UnregisterScript(uint32_t flags) {
    if (this->m_scriptRegistration) {
        this->m_scriptRegistration &= ~flags;

        if (!this->m_scriptRegistration) {
            ScriptEventsUnregisterUnit(this);
        }
    }
}

// PROVEN: 05ef5c5b IDENTICAL/NONE
// OFFSET: 0x005ABF90/0x14021E490
void CGUnit_C::StopNamePlateSpellbar() {
    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->SetCastBarValues(0.0f, 0.0f, 0.0f, 0, false, false);
    }
}

// OFFSET: 0x005B2F20/0x140226360
bool CGUnit_C::IsCastNotInterruptible() const {
    if (!(this->unk_09FC & 0x8)) {
        return false;
    }

    bool isPlayer = (this->m_obj->m_type & TYPEMASK_PLAYER) != 0;
    uint32_t interruptSpecial = isPlayer ? 0 : CGSpellBook::m_interruptSpecialSchoolMask;
    uint32_t silenceSpecial = isPlayer ? 0 : CGSpellBook::m_silenceSpecialSchoolMask;

    return CGSpellBook::m_interruptSchoolMask || interruptSpecial || CGSpellBook::m_silenceSchoolMask || silenceSpecial;
}

// OFFSET: 0x005B3C10/0x1402271A0
bool CGUnit_C::CanAutoDismount() const {
    if (this->m_unit->m_unitFieldFlags & 0x100000) {
        return false;
    }

    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !g_autoDismountCVar->m_intValue) {
        return false;
    }

    return !(this->m_movement->m_flags & MOVEMENTFLAG_FLYING) || g_autoDismountFlyingCVar->m_intValue;
}

// PROVEN: 382c2141 IDENTICAL/NONE
// OFFSET: 0x005BA7E0
void CGUnit_C::UpdateSpellCastBars() {
    uint32_t time = FrameTime::GetCurTimeMs();
    CGNamePlateFrame* namePlate = this->m_namePlateFrame;

    if (!namePlate || this->m_obj->m_guid != CGGameUI::m_lockedTarget) {
        return;
    }

    int32_t channelSpell = this->m_channelSpellId;

    if (channelSpell) {
        uint32_t end = this->m_channelEndTime;
        bool notInterruptible = this->IsCastNotInterruptible();
        namePlate->SetCastBarValues(0.0f, (end - this->m_channelStartTime) * 0.001f, (end - time) * 0.001f, channelSpell, true, notInterruptible);
        return;
    }

    int32_t castSpell = this->m_castingSpellId;

    if (castSpell) {
        uint32_t start = this->m_castStartTime;
        bool notInterruptible = this->IsCastNotInterruptible();
        namePlate->SetCastBarValues(0.0f, (this->m_castEndTime - start) * 0.001f, (time - start) * 0.001f, castSpell, false, notInterruptible);
        return;
    }

    this->StopNamePlateSpellbar();
}

// OFFSET: 0x005C07C0
void CGUnit_C::OnChannelUpdate(uint32_t remaining) {
    uint32_t duration = this->m_channelEndTime - this->m_channelStartTime;
    uint32_t now = FrameTime::GetCurTimeMs();

    if (remaining <= duration) {
        uint32_t end = remaining + now;
        this->m_channelEndTime = end;
        this->m_channelStartTime = end - duration;
    } else {
        this->m_channelStartTime = now;
        this->m_channelEndTime = remaining + now;
    }

    this->m_castingSpellId = 0;
    this->m_castStartTime = 0;
    this->m_castEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005C0830
bool CGUnit_C::ProcessCastNotInterruptible(const SpellRec* spell) {
    this->unk_09FC &= ~0x8u;

    if (spell->m_attributesExG & 0x2000) {
        return false;
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) && spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories && (categories->m_preventionType & 0x1)) {
            bool isPlayer = (this->m_obj->m_type & TYPEMASK_PLAYER) != 0;
            uint32_t interrupt = CGSpellBook::m_interruptSchoolMask;
            uint32_t interruptSpecial = isPlayer ? 0 : CGSpellBook::m_interruptSpecialSchoolMask;
            uint32_t silence = CGSpellBook::m_silenceSchoolMask;
            uint32_t silenceSpecial = !isPlayer ? CGSpellBook::m_silenceSpecialSchoolMask : 0;

            if (!interrupt && !interruptSpecial && !silence && !silenceSpecial) {
                return true;
            }

            uint32_t immunities = this->m_castImmunities;
            uint32_t schoolImmunities = this->m_castSchoolImmunities;

            if (immunities & 0x4000000) {
                interrupt = 0;
                interruptSpecial = 0;
            } else if (schoolImmunities) {
                interrupt &= ~schoolImmunities;
                interruptSpecial &= ~schoolImmunities;
            }

            if (immunities & 0x200) {
                silence = 0;
                silenceSpecial = 0;
            } else if (schoolImmunities) {
                silence &= ~schoolImmunities;
                silenceSpecial &= ~schoolImmunities;
            }

            if (interrupt || interruptSpecial || silence || silenceSpecial) {
                if (this->HasAuraEffect(39) || this->HasAuraEffect(77) || this->HasAuraEffect(37) || this->HasAuraEffect(38)) {
                    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
                        if (!interrupt && !interruptSpecial && !silence && !silenceSpecial) {
                            break;
                        }

                        const SpellRec* auraSpell = g_spellDB.GetRecord(this->GetAura(i)->m_spellId);

                        if (!auraSpell) {
                            continue;
                        }

                        uint32_t effectCount;
                        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(auraSpell->m_id, effectCount);

                        for (uint32_t j = 0; j < effectCount; j++) {
                            const SpellEffectRec* effect = effects->m_data[j];

                            if (!effect) {
                                continue;
                            }

                            int32_t miscValue = effect->m_effectMiscValue;

                            switch (effect->m_effectAura) {
                                case 37:
                                    if (miscValue == 68) {
                                        interrupt = 0;
                                        interruptSpecial = 0;
                                    }

                                    break;

                                case 38:
                                    if (miscValue == 27) {
                                        silence = 0;
                                        silenceSpecial = 0;
                                    }

                                    break;

                                case 39:
                                    if (miscValue & interrupt) {
                                        interrupt &= ~miscValue;
                                    }

                                    if (miscValue & interruptSpecial) {
                                        interruptSpecial &= ~miscValue;
                                    }

                                    if (miscValue & silence) {
                                        silence &= ~miscValue;
                                    }

                                    if (miscValue & silenceSpecial) {
                                        silenceSpecial &= ~miscValue;
                                    }

                                    break;

                                case 77:
                                    if (miscValue == 9) {
                                        silence = 0;
                                        silenceSpecial = 0;
                                    } else if (miscValue == 26) {
                                        interrupt = 0;
                                        interruptSpecial = 0;
                                    }

                                    break;

                                default:
                                    break;
                            }
                        }
                    }
                }

                if (interrupt || interruptSpecial || silence || silenceSpecial) {
                    return false;
                }
            }
        }
    }

    this->unk_09FC |= 0x8;
    return true;
}

// PROVEN: 7cf40646 IDENTICAL/NONE
// OFFSET: 0x005C4050
bool CGUnit_C::IsNotHostileTo(const CGUnit_C* unit) const {
    return this->UnitReaction(unit) >= 3;
}

// OFFSET: 0x005C5D40/0x14023CD20
bool CGUnit_C::CanAssist(const CGUnit_C* unit, bool ignoreFlags) const {
    uint32_t flags = unit->m_unit->m_unitFieldFlags;

    if (flags & 0x2000000) {
        return false;
    }

    CUnitDisplayTransition_C* transition = this->m_displayTransition;

    if (transition && (this->m_clientFlags & 0x400) && (this->m_currentClientRaceFlags & 0x4) && (transition->m_state == 4 || transition->m_state == 5)) {
        return false;
    }

    if (!ignoreFlags && (((this->m_unit->m_unitFieldFlags & 0x8) && (flags & 0x100)) || (!(this->m_unit->m_unitFieldFlags & 0x8) && (flags & 0x200)))) {
        return false;
    }

    if (!this->IsFriendly(unit)) {
        return false;
    }

    if (unit->m_unit->m_unitFieldFlags & 0x8) {
        CGPlayer_C* mine = static_cast<CGPlayer_C*>(const_cast<CGUnit_C*>(this)->GetControllingPlayer());
        CGPlayer_C* theirs = static_cast<CGPlayer_C*>(const_cast<CGUnit_C*>(unit)->GetControllingPlayer());

        if (mine && theirs && theirs->m_player->m_playerDuelTeam) {
            if (mine->m_player->m_playerDuelArbiter != theirs->m_player->m_playerDuelArbiter || mine->m_player->m_playerDuelTeam != theirs->m_player->m_playerDuelTeam) {
                return false;
            }
        }

        if ((this->m_unit->m_unitFieldFlags & 0x8) && (unit->m_unit->m_unitFieldBytes2[1] & 0x4) && !(this->m_unit->m_unitFieldBytes2[1] & 0x4)) {
            return false;
        }

        if ((this->m_unit->m_unitFieldFlags & 0x8) && (this->m_unit->m_unitFieldBytes2[1] & 0x8)) {
            uint8_t bytes = unit->m_unit->m_unitFieldBytes2[1];

            if (!(bytes & 0x8) && (bytes & 0x1)) {
                return false;
            }
        }

        return true;
    }

    if (!(this->m_unit->m_unitFieldFlags & 0x8)) {
        return true;
    }

    return ignoreFlags || (unit->m_unit->m_unitFieldBytes2[1] & 0x1) || unit->CanAidPlayers() || unit->IsTreatAsRaidUnit();
}
// OFFSET: 0x005C64B0/0x14023D610
bool CGUnit_C::CanAttackNow(const CGUnit_C* unit) const {
    if (this->IsDeadOrGhost()) {
        if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
            return false;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player || !player->GetPossessedUnit()) {
            return false;
        }
    }

    if (this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) && !(this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x800)) && !this->CanAutoDismount()) {
        return false;
    }

    if (unit->m_unit->m_unitFieldHealth <= 0 && !(unit->m_unit->m_unitDynamicFlags & 0x20)) {
        return false;
    }

    return this->CanAttack(unit);
}

// PROVEN: 4a5ed8cd IDENTICAL/NONE
// OFFSET: 0x005C65C0/0x14023D740
bool CGUnit_C::CanCooperate(const CGUnit_C* unit) const {
    if (unit == this) {
        return false;
    }

    if (this->m_unit->m_unitFieldCharmedby || unit->m_unit->m_unitFieldCharmedby) {
        return false;
    }

    FactionTemplateRec* mine = g_factionTemplateDB.GetRecord(this->m_unit->m_unitFieldFactiontemplate);
    FactionTemplateRec* theirs = g_factionTemplateDB.GetRecord(unit->m_unit->m_unitFieldFactiontemplate);

    if (mine && theirs && mine->m_factionGroup != theirs->m_factionGroup) {
        return false;
    }

    return !this->CanAttack(unit);
}

// OFFSET: 0x005C7C70/0x14023F160
void CGUnit_C::HandleNPCFeedback() {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return;
    }

    if (s_npcFeedbackSuppressCount) {
        return;
    }

    if (SE3::IsPlaying(this->m_npcInteractSound) || this->AppearsDead(1) || this->GetClientStandState() == 3) {
        return;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover || !this->IsNotHostileTo(mover) || !mover->IsNotHostileTo(this)) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->IsDeadOrGhost()) {
        return;
    }

    if (this->m_obj->m_guid != CGUnit_C::s_lastNPCSoundGUID) {
        s_lastNPCSoundCount = 0;
    }

    CGUnit_C::s_lastNPCSoundGUID = this->m_obj->m_guid;
    int32_t soundKitId;
    int32_t fileIndex;
    int32_t count = this->GetNewNPCSoundCount(s_lastNPCSoundCount, &soundKitId, &fileIndex);

    if (count == -1) {
        return;
    }

    C3Vector position = this->GetPosition();
    SE3SoundKitProperties properties;
    properties.m_highPriorityDownload = 1;
    properties.m_nIndexOverride = fileIndex;
    SE3::PlaySoundKit(soundKitId, &position, &properties, &this->m_npcInteractSound, __FILE__, __LINE__, "CGUnit_C::HandleNPCFeedback", 0, NULL);

    if (SE3::IsPlaying(this->m_npcInteractSound)) {
        s_lastNPCSoundCount = count;
    }
}

// OFFSET: 0x005B4390/0x140227900
bool CGUnit_C::IsOutdoors() const {
    return World::QueryOutdoors(this->m_worldObject) != 0;
}

// OFFSET: 0x005B5820/0x140229140
int32_t CGUnit_C::GetPredictedHealth() const {
    if (g_predictedHealthCVar->m_intValue) {
        return this->m_predictedHealth;
    }

    return this->m_unit->m_unitFieldHealth;
}

// OFFSET: 0x005BC5A0/0x140231060
int32_t CGUnit_C::GetPredictedPower(int32_t powerType) const {
    if (g_predictedPowerCVar->m_intValue) {
        int32_t slot = this->GetPowerTypeSlot(powerType);

        if (slot != -1) {
            return this->m_predictedPower[slot];
        }
    } else {
        if (powerType == -2) {
            return this->m_unit->m_unitFieldHealth;
        }

        int32_t slot = this->GetPowerTypeSlot(powerType);

        if (slot != -1) {
            return this->m_unit->m_unitFieldPower[slot];
        }
    }

    return 0;
}

// OFFSET: 0x005C7E10/0x14023F3F0
bool CGUnit_C::HasIgnoreAuraState(const SpellRec* spell) const {
    const CGUnit_C* unit = this;

    while (unit->m_unit->m_unitFieldCreatedby) {
        const CGUnit_C* creator = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unit->m_unitFieldCreatedby, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!creator) {
            break;
        }

        unit = creator;
    }

    return unit->HasAuraMatchingSpellClass(262, spell);
}

// OFFSET: 0x005C9530
bool CGUnit_C::CheckStealthRules(const SpellRec* spell, int32_t* reason) const {
    if (!(spell->m_attributes & 0x20000) || (this->m_unit->m_unitFieldBytes1[2] & 0x2) || this->HasAuraMatchingSpellClass(275, spell)) {
        return true;
    }

    *reason = 95;
    return false;
}

// OFFSET: 0x005BF9C0/0x140235250
const char* CGUnit_C::GetUnitName(const char** realm, int32_t followOverride) {
    if ((followOverride && this->HasAuraEffect(279)) || this->HasAuraEffect(56)) {
        const char* name = this->GetAuraOverrideName(realm);

        if (name) {
            return name;
        }
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        WOWGUID requester = this->m_obj->m_guid;
        NameCache* name = g_nameDBCache.GetRecord(this->m_obj->m_guid.m_guid, requester, NameQueryCallback, NULL, true);

        if (name) {
            if (realm && name->m_realmName[0]) {
                *realm = name->m_realmName;
            }

            return name->m_name;
        }
    } else if (this->m_unit->m_unitFieldPetnumber) {
        WOWGUID requester = this->m_obj->m_guid;
        PetNameCache* pet = g_petNameCache.GetRecord(this->m_unit->m_unitFieldPetnumber, requester, NameQueryCallback, NULL, true);

        if (pet) {
            if (pet->m_timestamp == this->m_unit->m_unitFieldPetNameTimestamp) {
                return pet->m_name;
            }

            UnitCombatLogInvalidateName(pet->m_name);
            g_petNameCache.Invalidate(this->m_unit->m_unitFieldPetnumber);
            requester = this->m_obj->m_guid;
            g_petNameCache.GetRecord(this->m_unit->m_unitFieldPetnumber, requester, NameQueryCallback, NULL, true);
        }
    } else if (this->m_creatureCacheEntry) {
        if (this->m_unit->m_unitFieldBytes0[2] == 1 && this->m_creatureCacheEntry->m_nameAlt[0]) {
            return this->m_creatureCacheEntry->m_nameAlt[0];
        }

        if (this->m_creatureCacheEntry->m_name[0]) {
            return this->m_creatureCacheEntry->m_name[0];
        }
    }

    const char* text = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);

    if (!text || !*text) {
        return "Unknown Being";
    }

    return text;
}

// OFFSET: 0x005BFBC0/0x1402354D0
char* CGUnit_C::GetUnitChosenTitle(char* buffer, uint32_t size, int32_t showTitle, int32_t* lines) {
    if (lines) {
        *lines = 1;
    }

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER) || !showTitle || static_cast<int32_t>(static_cast<CGPlayer_C*>(this)->m_player->m_playerChosenTitle) <= 0) {
        SStrCopy(buffer, this->GetUnitName(NULL, 1), size);
        return buffer;
    }

    CharTitlesRec* title = g_charTitlesDB.GetRecord(CGUnit_C::GetRecordForTitle(static_cast<CGPlayer_C*>(this)->m_player->m_playerChosenTitle));

    if (!title) {
        SStrCopy(buffer, this->GetUnitName(NULL, 1), size);
        return buffer;
    }

    uint32_t sex = this->IsActivePlayer() ? static_cast<CGPlayer_C*>(this)->m_player->m_playerBytes3[0] : this->m_unit->m_unitFieldBytes0[2];
    char format[400];

    if (sex == 1) {
        strcpy(format, strcmp(title->m_name1, "") ? title->m_name1 : title->m_name);
    } else {
        strcpy(format, strcmp(title->m_name, "") ? title->m_name : title->m_name1);
    }

    SStrPrintf(buffer, size, format, this->GetUnitName(NULL, 1));

    uint8_t medal = static_cast<CGPlayer_C*>(this)->m_player->m_playerBytes3[2];

    if (medal) {
        char token[32];
        SStrPrintf(token, sizeof(token), "PVP_MEDAL%d", medal);
        SStrPack(buffer, "\n", size);
        SStrPack(buffer, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), size);

        if (lines) {
            *lines = 2;
        }
    }

    return buffer;
}

// OFFSET: 0x005B9D90
void CGUnit_C::ShowHandItemSpellEffects(int32_t show, int32_t force) {
    uint32_t flags = this->m_clientFlags;
    int32_t visible = ((flags >> 16) & 0x1) & (this->m_sheatheState == 0) & (this->m_castSpellId == 0) & show;

    if (!force && (visible == static_cast<int32_t>((flags >> 14) & 0x1) || !(flags & 0x2000))) {
        return;
    }

    flags &= ~0x2000;
    this->m_clientFlags = flags;

    if (visible) {
        for (uint32_t i = 0; i < (this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count); i++) {
            if (!this->unk_0F9C[i]) {
                continue;
            }

            const SpellRec* spell = g_spellDB.GetRecord(this->unk_0F9C[i]);

            if (!spell) {
                continue;
            }

            SpellVisualRec visualRec;
            const SpellVisualRec* visual = this->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

            if (visual && visual->m_stateKit && (visual->m_flags & 0x8)) {
                SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);

                if (kit) {
                    if (!(this->m_clientFlags & 0x4000)) {
                        PlaySpellVisuaKitData data(spell, kit, 2);
                        data.m_animParam = 0;
                        this->PlaySpellVisualKit(data);
                        this->m_clientFlags |= 0x4000;
                    }

                    this->m_clientFlags |= 0x2000;
                }
            }
        }
    } else {
        this->m_clientFlags = flags & ~0x4000;

        for (uint32_t i = 0; i < (this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count); i++) {
            if (!this->unk_0F9C[i]) {
                continue;
            }

            const SpellRec* spell = g_spellDB.GetRecord(this->unk_0F9C[i]);

            if (!spell) {
                continue;
            }

            SpellVisualRec visualRec;
            const SpellVisualRec* visual = this->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

            if (visual && visual->m_stateKit && (visual->m_flags & 0x8) && g_spellVisualKitDB.GetRecord(visual->m_stateKit)) {
                this->ClearSpellEffect(this->unk_0F9C[i], 1, 0);
                this->m_clientFlags |= 0x2000;
            }
        }
    }
}

// OFFSET: 0x005C0340
void CGUnit_C::SetHandItemSpellEffectsVisible(int32_t visible) {
    uint32_t flags = this->m_clientFlags;

    if (visible == static_cast<int32_t>((flags >> 16) & 0x1)) {
        return;
    }

    if (visible) {
        this->m_clientFlags = flags | 0x10000;
        this->ShowHandItemSpellEffects(visible, 0);
    } else {
        this->m_clientFlags = flags & ~0x10000;
        this->ShowHandItemSpellEffects(visible, 0);
    }
}

// OFFSET: 0x005AA9D0
C3Vector* ReadPackedVector3(CDataStore* msg, const C3Vector& base, C3Vector* out) {
    uint32_t packed;
    msg->Get(packed);
    int32_t z = static_cast<int32_t>(packed) >> 22;
    float y = static_cast<float>(static_cast<int32_t>((packed >> 11) << 21) >> 21) * 0.25f;
    out->x = base.x - static_cast<float>(static_cast<int32_t>(packed << 21) >> 21) * 0.25f;
    out->y = base.y - y;
    out->z = base.z - 0.25f * static_cast<float>(z);
    return out;
}

// OFFSET: 0x005AAA60
C3Vector* ComputeTransportRelativeMovement(const C3Vector& position, uint32_t& count, WOWGUID transport, C3Vector* points, int32_t use3D) {
    C3Vector pos = position;

    if (transport) {
        C44Matrix matrix;
        MovementGetTransportMtxX(transport, &matrix, WOWGUID(), __FILE__, __LINE__);
        pos = pos * matrix.AffineInverse();
    }

    if (count == 1) {
        if ((points[0].x - pos.x) * (points[0].x - pos.x) + (points[0].z - pos.z) * (points[0].z - pos.z) + (points[0].y - pos.y) * (points[0].y - pos.y) >= 0.00077160494f) {
            points[-1] = pos;
            count++;
            return points - 1;
        }

        count = 0;
        return NULL;
    }

    int32_t segments = count - 1;
    int32_t behind = 0;
    int32_t ahead = 0;

    for (int32_t i = 0; i < segments; i++) {
        C3Vector dir = points[i + 1] - points[i];
        float d = -(dir.y * pos.y + dir.x * pos.x + dir.z * pos.z);
        float start = dir.z * points[i].z + dir.y * points[i].y + dir.x * points[i].x + d;
        float end = dir.y * points[i + 1].y + dir.x * points[i + 1].x + dir.z * points[i + 1].z + d;

        if (start < 0.0f || end < 0.0f) {
            if (start > 0.0f || end > 0.0f) {
                break;
            }

            behind++;
        } else {
            ahead++;
        }
    }

    if (ahead == segments) {
        int32_t skip = 0;

        if (segments > 0) {
            for (; skip < static_cast<int32_t>(count - 1); skip++) {
                float distSq = (points[skip].x - pos.x) * (points[skip].x - pos.x);

                if (use3D) {
                    distSq = distSq + (points[skip].z - pos.z) * (points[skip].z - pos.z);
                }

                if (distSq + (points[skip].y - pos.y) * (points[skip].y - pos.y) > 1.0f) {
                    break;
                }
            }
        }

        if (!skip) {
            points[-1] = pos;
            count++;
            return points - 1;
        }

        C3Vector* result = &points[skip];

        if (skip == segments && (pos.x - result->x) * (pos.x - result->x) + (pos.z - result->z) * (pos.z - result->z) + (pos.y - result->y) * (pos.y - result->y) < 0.00077160494f) {
            count = 1;
            return result;
        }

        result = &points[skip - 1];
        *result = pos;
        count -= skip - 1;
        return result;
    }

    if (behind == segments) {
        C3Vector& last = points[count - 1];

        if ((last.x - pos.x) * (last.x - pos.x) + (last.z - pos.z) * (last.z - pos.z) + (last.y - pos.y) * (last.y - pos.y) < 0.00077160494f) {
            count = 0;
            return NULL;
        }

        points[0] = pos;
        points[1] = last;
        count = 2;
        return points;
    }

    if (segments <= 0) {
        return points;
    }

    uint32_t i = 0;

    while (true) {
        C3Vector& p0 = points[i];
        C3Vector& p1 = points[i + 1];
        C3Vector delta = p1 - p0;
        float inv = 1.0f / sqrt(delta.x * delta.x + delta.z * delta.z + delta.y * delta.y);
        C3Vector dir(inv * delta.x, delta.y * inv, delta.z * inv);
        float d = dir.y * pos.y + dir.x * pos.x + dir.z * pos.z;
        float start = p0.z * dir.z + p0.y * dir.y + p0.x * dir.x - d;
        float end = p1.z * dir.z + p1.y * dir.y + p1.x * dir.x - d;

        if (start <= 0.0f && end >= 0.0f) {
            float t = start / (start - end);
            C3Vector projected(delta.x * t + p0.x, (p1.y - p0.y) * t + p0.y, (p1.z - p0.z) * t + p0.z);

            if ((p1.x - projected.x) * (p1.x - projected.x) + (p1.z - projected.z) * (p1.z - projected.z) + (p1.y - projected.y) * (p1.y - projected.y) >= 0.00077160494f) {
                uint32_t skip = i + 1;

                for (; skip < count - 1; skip++) {
                    float distSq = (points[skip].x - pos.x) * (points[skip].x - pos.x);

                    if (use3D) {
                        distSq = distSq + (points[skip].z - pos.z) * (points[skip].z - pos.z);
                    }

                    if (distSq + (points[skip].y - pos.y) * (points[skip].y - pos.y) > 1.0f) {
                        break;
                    }
                }

                if (skip) {
                    C3Vector* result = &points[skip];

                    if (skip == count - 1 && (pos.x - result->x) * (pos.x - result->x) + (pos.z - result->z) * (pos.z - result->z) + (pos.y - result->y) * (pos.y - result->y) < 0.00077160494f) {
                        count = 1;
                        return result;
                    }

                    result = &points[skip - 1];
                    *result = pos;
                    count -= skip - 1;
                    return result;
                }
            } else if ((p1.x - pos.x) * (p1.x - pos.x) + (p1.z - pos.z) * (p1.z - pos.z) + (p1.y - pos.y) * (p1.y - pos.y) >= 0.00077160494f) {
                p0 = pos;
            } else {
                i++;

                if (i == count - 1) {
                    count = 0;
                    return NULL;
                }
            }
        }

        if (start >= 0.0f && end >= 0.0f) {
            break;
        }

        i++;

        if (i >= count - 1) {
            return points;
        }
    }

    count -= i;
    C3Vector* result = &points[i];

    if ((result->x - pos.x) * (result->x - pos.x) + (result->y - pos.y) * (result->y - pos.y) < 0.00077160494f) {
        return result;
    }

    result--;
    *result = pos;
    count++;
    return result;
}

// OFFSET: 0x005AFDA0
void CGUnit_C::FaceTarget(const WOWGUID& target, int32_t flush) {
    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        C3Vector targetPosition = object->GetPosition();
        C3Vector position = this->GetPosition();
        this->m_movementData.OnSetFacing(CalculateFacingTo(&position, &targetPosition), flush);
    }
}

// OFFSET: 0x005CF680
int32_t CGUnit_C::HandleMonsterMovePacket(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        msg->m_read = msg->m_size;
        return 0;
    }

    SmartGUID transport;
    uint8_t seat;

    if (msgId == SMSG_MONSTER_MOVE_TRANSPORT) {
        *msg >> transport;
        msg->Get(seat);
    } else {
        transport.m_guid = 0;
        seat = 0xFF;
    }

    uint8_t exitVoluntary = 0;
    msg->Get(exitVoluntary);
    unit->SetVehicleExitVoluntary(exitVoluntary);

    if (!unit->VehiclePassengerStartWaitingForVehicleTransitionAnimFromSpline(msg, transport.m_guid, seat)) {
        unit->OnMonsterMove(msg, msgId, transport.m_guid, seat, 1);
    }

    return 1;
}

// OFFSET: 0x005CB460
void CGUnit_C::OnMonsterMove(CDataStore* msg, NETMESSAGE msgId, uint64_t transport, uint8_t seat, int32_t flush) {
    this->m_clientFlags |= 0x20000000;

    if (flush) {
        this->m_movementData.FlushMoveQueue(0, 0);
    }

    this->m_movementData.ForceSetTransport(WOWGUID::Make(transport), seat, 1);
    this->m_clientFlags &= ~0x20000000;

    if (this->m_movementData.m_transportGuid.m_guid != transport) {
        msg->m_read = msg->m_size;
        return;
    }

    this->m_movementData.SetAnimTierInTrans(0);
    C3Vector start(0.0f, 0.0f, 0.0f);
    msg->Get(start.x);
    msg->Get(start.y);
    msg->Get(start.z);
    uint32_t splineId;
    msg->Get(splineId);
    uint8_t faceType;
    msg->Get(faceType);
    C3Vector facePoint(0.0f, 0.0f, 0.0f);
    WOWGUID faceTarget = WOWGUID();
    float faceAngle = 0.0f;

    switch (faceType) {
        case 1: {
            C3Vector position = this->GetRawPosition();

            if ((start.z - position.z) * (start.z - position.z) + (start.y - position.y) * (start.y - position.y) + (start.x - position.x) * (start.x - position.x) < CGUnit_C::s_cvPathingDistTolerance->m_floatValue * CGUnit_C::s_cvPathingDistTolerance->m_floatValue) {
                this->m_movementData.OnSplineSnap(splineId, start, 0, 1);
                this->UpdateMovementAnimKit(0);
                return;
            }

            break;
        }

        case 2: {
            *msg >> facePoint;
            break;
        }

        case 3: {
            msg->Get(faceTarget.m_guid);
            break;
        }

        case 4: {
            msg->Get(faceAngle);
            break;
        }
    }

    float verticalAcceleration = 0.0f;
    uint32_t effectStartTime = 0;
    uint32_t animTime = 0;
    uint8_t animType = 0;
    uint32_t splineFlags;
    uint32_t duration;
    uint32_t waypoints;

    if (faceType == 1) {
        splineFlags = 0;
        duration = 0;
        waypoints = 1;
    } else {
        msg->Get(splineFlags);

        if (splineFlags & 0x1000000) {
            msg->Get(animType);
            msg->Get(animTime);
        }

        msg->Get(duration);

        if (splineFlags & 0x2000000) {
            msg->Get(verticalAcceleration);
            msg->Get(effectStartTime);
        }

        msg->Get(waypoints);
    }

    if (!CGUnit_C::s_cvPathSmoothing->m_intValue) {
        splineFlags &= ~0x100000u;
    }

    C3Vector* buffer = static_cast<C3Vector*>(alloca(sizeof(C3Vector) * (waypoints + 5)));
    C3Vector direction(0.0f, 0.0f, 0.0f);
    C3Vector end(0.0f, 0.0f, 0.0f);
    float facing = this->GetRawFacing();
    direction.y = sin(facing);
    direction.x = cos(facing);
    C3Vector* points;
    uint32_t pointCount;

    if (faceType == 1) {
        C3Vector position = this->GetRawPosition();
        buffer[0] = position - direction;
        buffer[1] = position;
        buffer[2] = start;
        buffer[3] = start;
        end = start;
        points = buffer;
        pointCount = 4;
    } else {
        C3Vector* path = &buffer[2];
        uint32_t count = 0;

        if (splineFlags & 0x400000) {
            C3Vector point(0.0f, 0.0f, 0.0f);
            *msg >> point;

            if ((point.z - start.z) * (point.z - start.z) + (point.y - start.y) * (point.y - start.y) + (point.x - start.x) * (point.x - start.x) > 0.00077160494f) {
                path[0] = start;
                count = 1;
            }

            path[count] = point;
            count++;

            for (uint32_t i = 1; i < waypoints; i++) {
                msg->Get(point.x);
                msg->Get(point.y);
                msg->Get(point.z);
                path[count] = point;
                count++;
            }

            end = path[count - 1];
        } else {
            *msg >> end;

            if (waypoints == 1) {
                if ((end.z - start.z) * (end.z - start.z) + (end.y - start.y) * (end.y - start.y) + (end.x - start.x) * (end.x - start.x) > 0.00077160494f) {
                    path[0] = start;
                    count = 1;
                }

                path[count] = end;
                count++;
            } else {
                C3Vector middle((end.x + start.x) * 0.5f, (end.y + start.y) * 0.5f, (end.z + start.z) * 0.5f);
                C3Vector point;
                ReadPackedVector3(msg, middle, &point);

                if ((point.z - start.z) * (point.z - start.z) + (point.y - start.y) * (point.y - start.y) + (point.x - start.x) * (point.x - start.x) > 0.0f) {
                    path[0] = start;
                    count = 1;
                }

                path[count] = point;
                count++;

                for (uint32_t i = 1; i < waypoints - 1; i++) {
                    path[count] = *ReadPackedVector3(msg, middle, &point);
                    count++;
                }

                path[count] = end;
                count++;
            }
        }

        C3Vector position = this->GetPosition();

        if (!(splineFlags & 0x1840)) {
            path = ComputeTransportRelativeMovement(position, count, WOWGUID::Make(transport), path, (splineFlags & 0x240) != 0);
        }

        if (path && count) {
            if (splineFlags & 0x1000) {
                uint32_t first = 1;
                C3Vector last = path[count - 1];

                for (; first < count; first++) {
                    C3Vector diff = path[first] - last;

                    if (fabs(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) >= 2.3841858e-7f) {
                        break;
                    }
                }

                if (first != count) {
                    path[count] = path[first];
                    count++;
                }

                uint32_t next = first + 1;
                path[count] = next < count ? path[next] : path[count - 1];

                if (splineFlags & 0x100000) {
                    C3Vector seg = path[count - 1] - path[count - 2];
                    float length = sqrt(seg.x * seg.x + seg.y * seg.y + seg.z * seg.z);

                    if (length > 0.0099999998f) {
                        if (length > 2.0f) {
                            length = 2.0f;
                        }

                        C3Vector tail = path[count] - path[count - 1];
                        tail.NormalizeSafe();
                        path[count].x = tail.x * length + path[count - 1].x;
                        path[count].y = path[count - 1].y + tail.y * length;
                        path[count].z = path[count - 1].z + tail.z * length;
                    }
                }
            } else {
                path[count] = path[count - 1];
            }

            C3Vector lead = path[1] - path[0];

            if (splineFlags & 0x100000) {
                lead.NormalizeSafe();

                if (lead.z * direction.z + lead.y * direction.y + lead.x * direction.x <= 0.69999999f) {
                    lead = C3Vector(lead.x * 2.0f, lead.y * 2.0f, lead.z * 2.0f);
                } else {
                    lead = C3Vector(direction.x * 2.0f, direction.y * 2.0f, direction.z * 2.0f);
                }

                if (splineFlags & 0x80000) {
                    lead = C3Vector(lead.x * -1.0f, lead.y * -1.0f, lead.z * -1.0f);
                }
            } else if (splineFlags & 0x800) {
                lead.NormalizeSafe();
                lead = C3Vector(lead.x * 10.0f, lead.y * 10.0f, lead.z * 10.0f);
            }

            path[-1] = C3Vector(path[0].x - lead.x, path[0].y - lead.y, path[0].z - lead.z);
            points = path - 1;
            pointCount = count + 2;
        } else {
            points = path;
            pointCount = 0;
        }
    }

    float speed = 0.0f;

    if (pointCount > 3) {
        float length = 0.0f;

        for (uint32_t i = pointCount - 3; i > 0; i--) {
            C3Vector seg = points[i + 1] - points[i];
            length += sqrt(seg.x * seg.x + seg.y * seg.y + seg.z * seg.z);
        }

        if (length > 0.16666667f) {
            float maxSpeed = this->GetMaxSplineSpeed(splineFlags);
            float required;

            if (duration && (required = length / (duration * 0.001f), maxSpeed > required)) {
                speed = required;
            } else {
                speed = maxSpeed;
            }

            if (speed > 9.5367432e-7f) {
                duration = static_cast<int32_t>(length / speed * 1000.0f);

                if (duration <= 1) {
                    duration = 1;
                }
            }
        }
    }

    MoveSplineParams params;
    params.m_points = points;
    params.m_pointCount = pointCount;
    params.m_duration = duration;
    params.m_flags = splineFlags;
    params.m_id = splineId;
    params.m_faceTarget = WOWGUID();
    params.m_facePoint = C3Vector(0.0f, 0.0f, 0.0f);
    params.m_faceAngle = 0.0f;
    params.m_verticalAcceleration = 0.0f;
    params.m_effectStartTime = 0;
    params.m_animTime = 0;

    switch (faceType) {
        case 2: {
            params.m_facePoint = facePoint;
            params.m_flags = splineFlags | 0x4000000;
            break;
        }

        case 3: {
            params.m_faceTarget = faceTarget;
            params.m_flags = splineFlags | 0x8000000;
            break;
        }

        case 4: {
            params.m_flags = splineFlags | 0x10000000;
            params.m_faceAngle = faceAngle;
            break;
        }
    }

    if (splineFlags & 0x2000000) {
        params.m_verticalAcceleration = verticalAcceleration;
        params.m_effectStartTime = effectStartTime;
    } else if (splineFlags & 0x1000000) {
        params.m_flags |= animType;
        params.m_animTime = animTime;
    }

    if (speed <= 9.5367432e-7f || !this->m_movementData.OnSpline(params)) {
        if (faceType == 2) {
            C3Vector target = this->m_movementData.CPassenger::GetPosition(facePoint);
            C3Vector position = this->GetPosition();
            this->m_movementData.OnSetFacing(CalculateFacingTo(&position, &target), flush);
        } else if (faceType == 3) {
            this->FaceTarget(faceTarget, flush);
        } else if (faceType == 4) {
            this->m_movementData.OnSetFacing(this->m_movementData.CPassenger::GetFacing(faceAngle), flush);
        }

        this->m_movementData.OnSplineSnap(splineId, end, splineFlags, flush);
    } else if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGInputControl::GetActive()->UpdatePlayer(FrameTime::GetCurTimeMs(), 1);
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateWorldMatrix();
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
    }

    this->CancelRangedMode();

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = NULL;
    }

    this->UpdateMovementAnimKit(0);
}

// OFFSET: 0x005AEB90/0x140221520
void CGUnit_C::SendMovementHeartbeat() {
    PlayerMoveHeartbeat msg;
    this->m_movementData.SendMoveStatus(MovementGetIdleTime(), MSG_MOVE_HEARTBEAT, &msg.m_status, &msg);
}

// OFFSET: 0x005C58A0
void CGUnit_C::OnUnsetActiveMover() {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13) {
        this->ClearTrackingTarget(false, true);
    }

    this->m_movementData.OnUnsetActiveMover();

    if (!this->m_movement->IsOnSpline() && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        this->m_movementData.SendNotActiveMover(MovementGetIdleTime());
    }
}

// PROVEN: fe95367c IDENTICAL/NONE
// OFFSET: 0x005C5920
void CGUnit_C::SetActiveMover(uint64_t guid) {
    if (CGUnit_C::s_activeMover.m_guid == guid) {
        return;
    }

    CGUnit_C* oldMover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (oldMover) {
        oldMover->OnUnsetActiveMover();
    }

    CGUnit_C::s_activeMover = WOWGUID::Make(guid);

    if (guid) {
        PlayerSetActiveMover msg;
        msg.m_guid = WOWGUID::Make(guid);
        ClientServices::Send(&msg);
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(guid), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        mover->OnSetActiveMover();
    } else {
        CVehiclePassenger_C::OnCancelControlEnd();
    }
}

// OFFSET: 0x005C8240
void CGUnit_C::OnMovementInitiated(bool clearTracking, bool keepLoot) {
    if (clearTracking && this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13 && !(CGUnit_C::s_trackingFlags & 0x1)) {
        this->ClearTrackingTarget(false, true);
    }

    if (!keepLoot && this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->IsLooting()) {
            CGGameUI::CloseLoot(true, true, false);
        }
    }
}

// OFFSET: 0x005CF610
int32_t Packet_AuraUpdate(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        msg->m_read = msg->m_size;
        return 0;
    }

    unit->OnAuraUpdate(msg, msgId == SMSG_AURA_UPDATE_ALL);
    return 1;
}

// OFFSET: 0x005B2560/0x1402259C0
SkillLineAbilityRec* CGUnit_C::LookupAbility(int32_t spellId) {
    if (!this->m_creatureCacheEntry) {
        return SpellTableLookupAbility(this->m_unit->m_unitFieldBytes0[0], this->m_unit->m_unitFieldBytes0[1], spellId);
    }

    SkillLineAbilityRec* ability = NULL;
    CreatureFamilyRec* family = g_creatureFamilyDB.GetRecord(this->m_creatureCacheEntry->m_family);

    if (family) {
        for (uint32_t i = 0; i < 2; i++) {
            ability = SpellTableLookupPetAbility(family->m_skillLine[i], spellId);

            if (ability) {
                break;
            }
        }
    }

    return ability;
}

// OFFSET: 0x005CDC70/0x140246640
void CGUnit_C::SpellInterrupted(uint8_t castCount, int32_t spellId) {
    if (!this->m_castSpellId || spellId != this->m_castSpellId || castCount != this->m_castCount) {
        return;
    }

    this->m_animationState &= ~0x400;

    if (this->m_sheatheState == 2) {
        this->CancelRangedMode();
    }

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = NULL;
    }

    this->ReleaseMissiles();

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && this->IsActiveMoverTracking(s_spellInterruptTrackingMask)) {
        this->ClearTrackingTarget(false, true);
    }
}

// OFFSET: 0x005ACD50
void AuraData::Unpack(CGUnit_C* unit, uint32_t time, CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(this->m_spellId));

    if (!this->m_spellId) {
        return;
    }

    msg->Get(this->m_flags);
    msg->Get(this->m_level);
    msg->Get(this->m_stackCount);
    SmartGUID creator;

    if (this->m_flags & 0x8) {
        creator.m_guid = unit->m_obj->m_guid.m_guid;
    } else {
        *msg >> creator;
    }

    this->m_creator = creator;

    if (this->m_flags & 0x20) {
        msg->Get(reinterpret_cast<uint32_t&>(this->m_duration));
        uint32_t remaining;
        msg->Get(remaining);
        this->m_endTime = remaining + time;

        if (!this->m_endTime) {
            this->m_endTime = 1;
        }
    } else {
        this->m_duration = 0;
        this->m_endTime = 0;
    }

    this->m_effectAmounts[0] = 0;
    this->m_effectAmounts[1] = 0;
    this->m_effectAmounts[2] = 0;

    if (this->m_flags & 0x40) {
        for (uint32_t i = 0; i < 3; i++) {
            if (this->m_flags & (1 << i)) {
                msg->Get(reinterpret_cast<uint32_t&>(this->m_effectAmounts[i]));
            }
        }
    }
}

// OFFSET: #0x0072F360
void CGUnit_C::AddAuraSlot(uint8_t slot) {
    uint32_t count = this->m_auras.m_count == 0xFFFFFFFF ? this->m_auras.Dynamic().m_count : this->m_auras.m_count;

    if (count > slot) {
        return;
    }

    this->m_auras.SetCount(slot + 1);

    for (uint32_t i = count; i <= slot; i++) {
        this->m_auras[i].m_spellId = 0;
    }

    for (uint32_t j = 0; j < sizeof(this->m_sortedAuras) / sizeof(this->m_sortedAuras[0]); j++) {
        this->m_sortedAuras[j].SetCount(slot + 1);

        for (uint32_t i = count; i <= slot; i++) {
            this->m_sortedAuras[j][i].m_auraIndex = 256;
            this->m_sortedAuras[j][i].m_spellId = 0;
        }
    }

    this->unk_0F9C.SetCount(slot + 1);

    for (uint32_t i = count; i <= slot; i++) {
        this->unk_0F9C[i] = 0;
    }
}

// OFFSET: 0x005D2D30/0x14024CB20
void CGUnit_C::SetSheatheState(uint32_t showWeapons, int32_t instant, int32_t noSend) {
    if (this->IsPlayingWeaponAnim()) {
        showWeapons = CGUnit_C::ResolveSheathState(showWeapons, this->GetVirtualItem(0, false), this->GetVirtualItem(1, false));
    }

    if (showWeapons == 2 && (this->m_obj->m_type & TYPEMASK_PLAYER) && !this->FindVirtualItemEffect(2)) {
        ChrClassesRec* classRec = g_chrClassesDB.GetRecord(this->m_unit->m_unitFieldBytes0[1]);

        if (!classRec || (classRec->m_flags & 0x8)) {
            return;
        }
    }

    if (showWeapons == this->m_sheatheState) {
        return;
    }

    if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x10000000) && !noSend) {
        return;
    }

    if (!this->m_worldModel) {
        return;
    }

    this->m_previousSheatheState = this->m_sheatheState;
    this->m_sheatheState = showWeapons;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !noSend) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_SHEATHED));
        msg.Put(this->m_sheatheState);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (showWeapons) {
        this->ShowHandItemSpellEffects(0, 0);
    }

    if (instant) {
        this->SetSheatheStateInstant();
    } else {
        this->SetSheatheStateAnimate();
    }

    this->UpdateCombatReadyAnimKit(0);
}

// OFFSET: 0x005BC4D0
void CGUnit_C::ModifyPredictedPower(int32_t powerType, int32_t delta) {
    if (delta < 0 && (this->m_unit->m_unitFieldFlags2 & 0x100) && powerType != 7 && powerType != 8) {
        return;
    }

    int32_t slot = this->GetPowerTypeSlot(powerType);

    if (slot == -1) {
        return;
    }

    int32_t current = this->m_predictedPower[slot];
    int32_t value = current + delta;
    int32_t maximum = this->GetMaxPower(powerType);

    if (current > 0 && value < 0) {
        value = 0;
    } else if (current < 0 && value > 0) {
        value = 0;
    } else if (current >= 0 && value > maximum) {
        value = maximum;
    } else if (value < 0 && value < -maximum) {
        value = -maximum;
    }

    this->SetPredictedPowerSlot(slot, powerType, value, 0);
}

// OFFSET: 0x005AE9F0
void CGUnit_C::InitActiveMover(const WOWGUID& guid) {
    CGUnit_C::s_activeMover = guid;
    PlayerSetActiveMover msg;
    msg.m_guid = guid;
    ClientServices::Send(&msg);
    uint32_t time = FrameTime::GetCurTimeMs();
    CGInputControl::GetActive()->UpdatePlayer(time, 1);
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover->m_movement->m_flags & 0x60080F) {
        mover->m_movementData.UpdateHeartbeatTimerA(time);
    }

    WOWGUID transportGuid = WOWGUID::Make(mover->GetTransportGUID());

    if (transportGuid) {
        CGGameObject_C* transport = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(transportGuid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

        if (transport) {
            MovementSetTransportUpdateTime(transport->m_typeData->GetTransportUpdateTime());
        }
    }

    CVehiclePassenger_C::OnSetActiveMover(mover);
}

// OFFSET: 0x005AEAE0
void CGUnit_C::OnSetActiveMover() {
    this->m_movementData.OnSetActiveMover();
    CGInputControl::GetActive()->UpdatePlayer(FrameTime::GetCurTimeMs(), 1);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        AreaTriggerCheckNow();
    }

    CVehiclePassenger_C::OnSetActiveMover(this);
}

// OFFSET: 0x005B6810
void CGUnit_C::SetLocalClientControl(bool control) {
    if (control) {
        this->m_clientFlags |= 0xC000400;
    } else {
        this->m_clientFlags &= ~0x400;

        if (this->m_unit->m_unitFieldCharmedby) {
            this->m_clientFlags |= 0x1000;
        }

        this->m_movementData.ResetFallOnControlLoss();
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGGameUI::OnClientControlChanged((this->m_clientFlags >> 10) & 1);

        if (this->m_clientFlags & 0x400) {
            this->VehiclePassengerUpdateWorldCameraStateInstantly();
        }
    }
}

// OFFSET: #0x0071D010
int32_t CGUnit_C::InitializeExtendedDisplay(CGPlayer_C* player, int32_t hasDisplayInfo) {
    this->m_characterComponent = CCharacterComponent::AllocComponent();
    ComponentData data;

    if (hasDisplayInfo) {
        data.m_preferences.m_raceID = this->m_displayInfoExtra->m_displayRaceID;
        data.m_preferences.m_sexID = this->m_displayInfoExtra->m_displaySexID;
    } else {
        data.m_preferences.m_raceID = this->m_unit->m_unitFieldBytes0[0];
        data.m_preferences.m_sexID = this->m_unit->m_unitFieldBytes0[2];
    }

    data.m_preferences.m_classId = this->m_unit->m_unitFieldBytes0[1];

    if (player) {
        data.m_preferences.m_skinID = player->m_player->m_playerBytes[0];
        data.m_preferences.m_faceId = player->m_player->m_playerBytes[1];
        data.m_preferences.m_hairStyleId = player->m_player->m_playerBytes[2];
        data.m_preferences.m_hairColorId = player->m_player->m_playerBytes[3];
        data.m_preferences.m_facialHairId = player->m_player->m_playerBytes2[0];
    } else {
        data.m_preferences.m_skinID = this->m_displayInfoExtra->m_skinID;
        data.m_preferences.m_faceId = this->m_displayInfoExtra->m_faceID;
        data.m_preferences.m_hairStyleId = this->m_displayInfoExtra->m_hairStyleID;
        data.m_preferences.m_hairColorId = this->m_displayInfoExtra->m_hairColorID;
        data.m_preferences.m_facialHairId = this->m_displayInfoExtra->m_facialHairID;

        if (!CGPlayer_C::IsXRayVisionActive() || !this->UsesNpcBakedTexture()) {
            if (!this->m_displayInfoExtra) {
                return 0;
            }

            const char* bakeName = this->m_displayInfoExtra->m_bakeName;

            if (!bakeName || !*bakeName) {
                return 0;
            }

            data.m_flags |= 0x1;
            SStrPrintf(data.m_npcSkinTexture, sizeof(data.m_npcSkinTexture), "%s%s", "Textures\\BakedNpcTextures\\", bakeName);
        }
    }

    data.m_model = this->m_worldModel;
    data.m_flags ^= (data.m_flags ^ (static_cast<uint32_t>(this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) << 1)) & 0x2;
    data.m_model->m_refCount++;

    if (player) {
        CCharacterComponent::ValidateComponentData(&data, COMPONENT_CONTEXT_1);
    } else {
        CCharacterComponent::ValidateComponentData(&data, COMPONENT_CONTEXT_2);
    }

    this->m_characterComponent->Init(&data, NULL);
    return 1;
}

// PROVEN: 10d68d3b IDENTICAL/NONE
// OFFSET: 0x005ADB60/0x140220230
void CGUnit_C::ApplyComponentItemsFromEffects() {
    for (CEffect* effect = this->m_effectList; effect;) {
        CEffect* next = effect->m_next;

        if (effect->m_flags & 0x80000) {
            effect->ApplyItemVisual();
        }

        effect = next;
    }
}

// OFFSET: 0x005CF750
int32_t CGUnit_C::InitializeComponent() {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded() || this->m_pendingDisplayModel) {
        return 0;
    }

    if (this->IsMirrorImage()) {
        this->m_clientFlags &= ~0x8000000;

        if (!(this->m_clientFlags & 0x20000)) {
            this->RequestMirrorImageDataFromServer();
        }

        this->m_clientFlags |= 0x400000;
        return 0;
    }

    this->m_clientFlags &= ~0x400000;

    if (this->m_characterComponent) {
        CCharacterComponent::FreeComponent(this->m_characterComponent);
        this->m_characterComponent = NULL;
    }

    if (this->HasBakedNpcTexture()) {
        this->InitializeExtendedDisplay(static_cast<CGPlayer_C*>(this), 1);
    } else if (this->m_displayInfoExtra) {
        int32_t result = this->InitializeExtendedDisplay(NULL, 1);

        if (!result) {
            return result;
        }
    } else if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (this->m_modelData->m_flags & 0x4)) {
        this->InitializeExtendedDisplay(static_cast<CGPlayer_C*>(this), 0);
    }

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER) || !static_cast<CGPlayer_C*>(this)->RefreshVisibleItems()) {
        if ((!CGPlayer_C::IsXRayVisionActive() || !this->UsesNpcBakedTexture()) && this->m_characterComponent && this->m_displayInfoExtra) {
            for (uint32_t i = 0; i < 11; i++) {
                int32_t displayId = this->m_displayInfoExtra->m_npcitemDisplay[i];

                if (displayId) {
                    this->m_characterComponent->AddItem(static_cast<ITEM_SLOT>(i), displayId, 0);
                }
            }
        }

        this->AddHandItem(0);
        this->AddHandItem(1);
        this->AddHandItem(2);
    }

    this->ApplyComponentItemsFromEffects();
    CGGameUI::UnitModelUpdate(this->m_obj->m_guid, 3);
    this->CreateFishingLineEffect();
    return 1;
}

// OFFSET: 0x005BA760/0x14022EBC0
void CGUnit_C::BowModelLoaded(CM2Model* model, void* arg) {
    if (model->HasEvent('TTW$') && model->HasEvent('BTW$')) {
        model->m_fFlags |= 0x20;
        model->m_drawCallback = &CDrawBowString::UnitDrawBowStringCB;
        model->m_drawArg = arg;
        return;
    }

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    SysMsgPrintf(STATUS_WARNING, "Bow model '%s' equipped without proper events.  Bowstring won't draw.", model->m_shared->m_data->m_name.Data());
}

// OFFSET: 0x005C8E20
int32_t CGUnit_C::ShowRangedWeapon(bool sheathed, bool* rightHand) {
    if (this->m_modelData->m_flags & 0x10) {
        return 0;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

    if (!item) {
        return 0;
    }

    this->CancelRangedMode();
    bool isRightHand = item->m_inventoryType == INVTYPE_RANGEDRIGHT || item->m_inventoryType == INVTYPE_THROWN;

    if (rightHand) {
        *rightHand = isRightHand;
    }

    if (this->m_rangedWeaponModel) {
        this->m_rangedWeaponModel->m_drawCallback = NULL;
        this->m_rangedWeaponModel->Release();
        this->m_rangedWeaponModel = NULL;
    }

    if (!sheathed) {
        this->m_worldModel->DetachAllChildrenById(ATTACH_HANDRIGHT);
        this->m_worldModel->DetachAllChildrenById(ATTACH_HANDLEFT);
        this->m_worldModel->DetachAllChildrenById(ATTACH_SHIELD);
    }

    ItemDisplayInfoRec displayRec;
    const ItemDisplayInfoRec* displayRecPtr = NULL;

    if (this->GetVirtualItemDisplayRec(2, displayRec)) {
        displayRecPtr = &displayRec;
    }

    int32_t link = this->AddHandItemModel(displayRecPtr, INVSLOT_RANGED, item->m_sheatheType, sheathed, false, isRightHand, 0);
    CM2Model* child = this->m_worldModel->m_attachList;

    while (child && child->m_attachmentId != static_cast<uint32_t>(link)) {
        child = child->m_attachNext;
    }

    this->m_rangedWeaponModel = child;

    if (child) {
        child->m_refCount++;
        this->m_rangedWeaponModel->SetSequenceCallback(NULL, 0);

        if (item->m_classID == 2 && item->m_subclassID == 2) {
            this->m_rangedWeaponModel->SetLoadedCallback(&CGUnit_C::BowModelLoaded, this);
        }

        this->UpdateRangedWeaponOrientation();
    }

    bool track = false;

    if (this->m_castSpellId && (this->m_animationState & 0x10)) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);

        if (spell) {
            bool ignoreFacing = true;

            if (spell->m_spellCastingRequirementsID) {
                SpellCastingRequirementsRec* requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID);

                if (requirements) {
                    ignoreFacing = !(requirements->m_facingCasterFlags & 0x1);
                }
            }

            if ((spell->m_attributes & 0x400000) && (ignoreFacing || this->IsInFrontOfTargetWithGuid(CGGameUI::m_lockedTarget))) {
                track = true;
            }
        }
    } else if (this->m_unit->m_unitChannelSpell) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

        if (spell && (spell->m_attributesEx & 0x4000)) {
            track = true;
        }
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && track) {
        this->SetTrackingTarget(CGGameUI::m_lockedTarget, 0);
    }

    if (sheathed) {
        this->ShowHandItemSpellEffects(1, 0);
    }

    return 1;
}

// OFFSET: 0x005D1C30
int32_t CGUnit_C::SheatheHandItem(uint32_t hand, int32_t sheathe) {
    CM2Model* model = this->m_worldModel;

    if (!model) {
        return 0;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(hand, false);

    if (!item) {
        return 0;
    }

    bool mainHand = !hand || item->m_inventoryType == INVTYPE_RANGEDRIGHT || item->m_inventoryType == INVTYPE_THROWN;
    GEOCOMPONENTLINKS handLink;

    switch (hand) {
        case 0: {
            handLink = ATTACH_HANDRIGHT;
            break;
        }

        case 1: {
            handLink = item->m_inventoryType != INVTYPE_SHIELD ? ATTACH_HANDLEFT : ATTACH_SHIELD;
            break;
        }

        case 2: {
            handLink = mainHand ? ATTACH_HANDRIGHT : ATTACH_HANDLEFT;
            break;
        }

        default: {
            return 0;
        }
    }

    GEOCOMPONENTLINKS from;
    GEOCOMPONENTLINKS to;

    if (sheathe) {
        from = handLink;
        to = CCharacterComponent::GetSheatheLink(static_cast<SHEATHE_TYPE>(item->m_sheatheType), mainHand);
    } else {
        to = handLink;
        from = CCharacterComponent::GetSheatheLink(static_cast<SHEATHE_TYPE>(item->m_sheatheType), mainHand);
    }

    bool skipAdd = false;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && CGBarberShop::m_barberShopEnabled && item->m_inventoryType == INVTYPE_SHIELD) {
        skipAdd = true;
        to = ATTACH_NONE;
    }

    CM2Model* child = model->m_attachList;

    while (child && child->m_attachmentId != static_cast<uint32_t>(from)) {
        child = child->m_attachNext;
    }

    if (child) {
        child->m_refCount++;
        child->DetachFromParent();

        if (to != ATTACH_NONE) {
            child->AttachToParent(model, to, NULL, 0);
        }

        child->Release();

        if (handLink) {
            COMP_HAND_SLOT slot = static_cast<COMP_HAND_SLOT>(handLink != ATTACH_HANDRIGHT);

            if (to == handLink) {
                CCharacterComponent::ComponentCloseFingers(model, slot);
            } else {
                CCharacterComponent::ComponentOpenFingers(model, slot);
            }
        }

        if (hand == 2) {
            if (!sheathe) {
                return 1;
            }

            this->AddHandItem(0);
            this->AddHandItem(1);
        }
    } else if (!skipAdd) {
        this->AddHandItem(hand);
    }

    if (sheathe && mainHand) {
        this->ShowHandItemSpellEffects(1, 0);
    }

    return 1;
}

// OFFSET: 0x005D2B70
void CGUnit_C::SetSheatheStateInstant() {
    switch (this->m_sheatheState) {
        case 0: {
            if (this->m_previousSheatheState == 1) {
                this->SheatheHandItem(0, 1);
                this->SheatheHandItem(1, 1);
            } else if (this->m_previousSheatheState == 2) {
                this->m_worldModel->DetachAllChildrenById(35);

                if (this->m_characterComponent && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
                    this->m_characterComponent->RemoveItem(ITEMSLOT_11);
                }

                if (!CGUnit_C::s_trackingType) {
                    this->ClearTrackingTarget(false, true);
                }

                const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

                if (item) {
                    if (item->m_inventoryType == INVTYPE_RANGEDRIGHT || item->m_inventoryType == INVTYPE_THROWN) {
                        this->m_worldModel->DetachAllChildrenById(ATTACH_HANDRIGHT);
                        CCharacterComponent::ComponentOpenFingers(this->m_worldModel, COMP_HAND_RIGHT);
                    } else {
                        this->m_worldModel->DetachAllChildrenById(ATTACH_HANDLEFT);
                        CCharacterComponent::ComponentOpenFingers(this->m_worldModel, COMP_HAND_LEFT);
                    }

                    this->m_animationState &= ~0x600;
                    return;
                }
            }

            this->m_animationState &= ~0x600;
            return;
        }

        case 1: {
            if (this->m_previousSheatheState == 2) {
                this->ShowRangedWeapon(true, NULL);
            }

            if (this->SheatheHandItem(0, 0)) {
                this->m_animationState |= 0x200;
            }

            if (this->SheatheHandItem(1, 0)) {
                this->m_animationState |= 0x400;
            }

            return;
        }

        case 2: {
            if (this->m_previousSheatheState == 1) {
                this->SheatheHandItem(0, 1);
                this->SheatheHandItem(1, 1);
            }

            this->m_animationState &= ~0x600;
            bool rightHand;

            if (this->ShowRangedWeapon(false, &rightHand)) {
                if (rightHand) {
                    this->m_animationState |= 0x200;
                } else {
                    this->m_animationState |= 0x400;
                }
            }

            return;
        }

        default: {
            return;
        }
    }
}

// OFFSET: 0x005CE2F0
int32_t CGUnit_C::UnsheatheRightWeapon() {
    this->m_animationState |= 0x200;

    if (!this->m_previousSheatheState) {
        return 0;
    }

    if (this->m_sheatheState == 1) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(0, false);

        if (!item) {
            return 0;
        }

        this->PlaySheatheAnimKit(((1 << item->m_sheatheType) & 0x88) ? 90 : 89, 0);
        return 1;
    }

    if (this->m_sheatheState == 2) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

        if (item && (item->m_inventoryType == INVTYPE_RANGEDRIGHT || item->m_inventoryType == INVTYPE_THROWN)) {
            this->ShowRangedWeapon(true, NULL);
            this->PlaySheatheAnimKit(((1 << item->m_sheatheType) & 0x88) ? 90 : 89, 0);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005CE3B0
int32_t CGUnit_C::UnsheatheLeftWeapon() {
    this->m_animationState |= 0x400;

    if (!this->m_previousSheatheState) {
        return 0;
    }

    if (this->m_sheatheState == 1) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(1, false);

        if (!item) {
            return 0;
        }

        this->PlaySheatheAnimKit(((1 << item->m_sheatheType) & 0x88) ? 90 : 89, 1);
        return 1;
    }

    if (this->m_sheatheState == 2) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

        if (item && item->m_inventoryType != INVTYPE_RANGEDRIGHT && item->m_inventoryType != INVTYPE_THROWN) {
            this->ShowRangedWeapon(true, NULL);
            this->PlaySheatheAnimKit(((1 << item->m_sheatheType) & 0x88) ? 90 : 89, 1);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005CE5C0
void CGUnit_C::SetSheatheStateAnimate() {
    switch (this->m_previousSheatheState) {
        case 0: {
            break;
        }

        case 1: {
            const CGUnitVirtualItem* right = this->GetVirtualItem(0, false);
            const CGUnitVirtualItem* left = this->GetVirtualItem(1, false);

            if (right) {
                this->PlaySheatheAnimKit(((1 << right->m_sheatheType) & 0x88) ? 90 : 89, 0);
                this->m_animationState &= ~0x200;
            } else {
                this->UnsheatheRightWeapon();
            }

            if (!left) {
                this->UnsheatheLeftWeapon();
                return;
            }

            this->PlaySheatheAnimKit(((1 << left->m_sheatheType) & 0x88) ? 90 : 89, 1);
            this->m_animationState &= ~0x400;
            return;
        }

        case 2: {
            this->m_worldModel->DetachAllChildrenById(35);

            if (this->m_characterComponent && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
                this->m_characterComponent->RemoveItem(ITEMSLOT_11);
            }

            if (!CGUnit_C::s_trackingType) {
                this->ClearTrackingTarget(false, true);
            }

            const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

            if (!item) {
                break;
            }

            if (item->m_inventoryType == INVTYPE_RANGEDRIGHT || item->m_inventoryType == INVTYPE_THROWN) {
                this->PlaySheatheAnimKit(89, 0);
                this->m_animationState &= ~0x200;
                this->UnsheatheLeftWeapon();
            } else {
                this->PlaySheatheAnimKit(89, 1);
                this->m_animationState &= ~0x400;
                this->UnsheatheRightWeapon();
            }

            return;
        }

        default: {
            return;
        }
    }

    if (this->m_sheatheState == 1) {
        const CGUnitVirtualItem* right = this->GetVirtualItem(0, false);
        const CGUnitVirtualItem* left = this->GetVirtualItem(1, false);

        if (right) {
            this->PlaySheatheAnimKit(((1 << right->m_sheatheType) & 0x88) ? 90 : 89, 0);
            this->m_animationState |= 0x200;
        }

        if (left) {
            this->PlaySheatheAnimKit(((1 << left->m_sheatheType) & 0x88) ? 90 : 89, 1);
            this->m_animationState |= 0x400;
        }
    } else if (this->m_sheatheState == 2) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

        if (item) {
            this->ShowRangedWeapon(true, NULL);

            if (item->m_inventoryType == INVTYPE_RANGEDRIGHT || item->m_inventoryType == INVTYPE_THROWN) {
                this->PlaySheatheAnimKit(((1 << item->m_sheatheType) & 0x88) ? 90 : 89, 0);
                this->m_animationState |= 0x200;
            } else {
                this->PlaySheatheAnimKit(((1 << item->m_sheatheType) & 0x88) ? 90 : 89, 1);
                this->m_animationState |= 0x400;
            }
        }
    }
}

// OFFSET: 0x005BD890
void CGUnit_C::AnimEndCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        if (boneId != 0xFFFFFFFF && boneId != 26 && !interrupted && model) {
            model->UnsetBoneSequence(boneId, 1, 1);
        }

        return;
    }

    if (model == unit->m_mountModel && unit->m_mountAnimKitManager) {
        unit->m_mountAnimKitManager->OnModelSequenceCallback(boneId, sequenceId, interrupted, a7);
    } else if (model == unit->m_worldModel) {
        unit->m_animKitManager.OnModelSequenceCallback(boneId, sequenceId, interrupted, a7);
    }

    if (!unit->m_mountModel && unit->m_vehicle && unit->m_vehicle->m_vehicleRec && unit->m_vehicle->IsBoneAnimPlaying(boneId)) {
        return;
    }

    unit->AnimEndHandler(model, boneId, sequenceId, interrupted, time);
}

// OFFSET: 0x005BD710
void CGUnit_C::AnimEndHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time) {
    if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_state) {
        this->m_vehiclePassenger->OnAnimEnd(boneId);
    }

    AnimationDataRec* anim = g_animationDataDB.GetRecord(sequenceId);
    int32_t behavior = anim ? anim->m_behaviorID : 660;

    if (behavior == 1 || behavior == 131 || behavior == 468) {
        if (!this->m_lootableTime) {
            this->m_lootableTime = FrameTime::GetCurTimeMs();
        }

        this->ShowLootEffect();
    }

    if (this->m_specialIdleAnim != -1 && this->m_worldModel && this->m_worldModel->IsLoaded()) {
        int32_t torso = this->m_torsoKeyBone != -1 && static_cast<int32_t>(this->m_worldModel->GetBoneSequenceId(this->m_torsoKeyBone)) == this->m_specialIdleAnim;
        int32_t special = this->m_specialIdleAnim;
        int32_t base = static_cast<int32_t>(this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF)) == special;

        if ((static_cast<int32_t>(sequenceId) == special && (!torso || !base)) || (!torso && !base)) {
            this->m_specialIdleAnim = -1;
        }
    }

    if (AnimationDataHasFlags(sequenceId, 0x200000) && !AnimationDataHasFlags(this->m_pendingCombatActionAnim, 0x200000)) {
        this->m_pendingCombatActionAnim = -1;
    }

    if (interrupted) {
        return;
    }

    this->UpdateObjectEffectAnimationStates();

    if (this->m_mountTransition && SpellVisualsIsMountTransitionFinished(this->m_mountTransition)) {
        return;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(this)->m_dance) {
        static_cast<CGPlayer_C*>(this)->DanceMoveAnimComplete();
    }
}

// OFFSET: #0x0073B510
void CGUnit_C::OnBaseAnimEnd(CM2Model* model, uint32_t boneId, uint32_t sequenceId) {
    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state == 3 && this->m_unit->m_unitFieldHealth > 0) {
        const VehicleSeatRec* seat = passenger->m_seatRec;

        if (seat) {
            bool ride;

            if (passenger->GetLowerAnim(seat) != 506) {
                ride = (seat->m_flags & 0x2) && (static_cast<int32_t>(sequenceId) == seat->m_rideAnimStart || static_cast<int32_t>(sequenceId) == seat->m_rideAnimLoop);
            } else {
                ride = (seat->m_flags & 0x4) && (static_cast<int32_t>(sequenceId) == seat->m_rideUpperAnimStart || static_cast<int32_t>(sequenceId) == seat->m_rideUpperAnimLoop);
            }

            if (ride) {
                this->UpdateBaseAnimation(0, -1);
                return;
            }
        }
    }

    uint32_t next = 506;
    AnimationDataRec* anim = g_animationDataDB.GetRecord(sequenceId);
    int32_t behavior = anim ? anim->m_behaviorID : 506;

    if (behavior >= 458 && behavior <= 460) {
        this->PlayBaseAnimation(464, 16);
        return;
    }

    switch (behavior) {
        case 1: {
            if (!this->AppearsDead(1)) {
                break;
            }

            M2BoneSequenceInfo info;
            model->GetBoneSequenceInfo(boneId, info);
            this->SetKeyBoneSequence(model, 0xFFFFFFFF, 6, info.m_variationIndex, 0, 1.0f, 1, 1, 0);
            this->UpdateObjectEffectAnimationStates();
            return;
        }

        case 6:
        case 472: {
            if (!this->AppearsDead(1)) {
                break;
            }

            this->UpdateObjectEffectAnimationStates();
            return;
        }

        case 37:
        case 38: {
            if (this->m_movement->m_flags & 0x2000000) {
                break;
            }

            this->PlayBaseAnimation(38, 16);
            return;
        }

        case 39: {
            this->m_animationState &= ~0x4;
            this->UpdateBaseAnimation(16, -1);
            return;
        }

        case 40: {
            this->PlayBaseAnimation(40, 16);
            return;
        }

        case 50: {
            this->PlayBaseAnimation(188, 16);
            return;
        }

        case 69: {
            if (this->IsInCombat() || this->m_combat.m_attacking.m_guid) {
                break;
            }

            this->PlayBaseAnimation(69, 16);
            return;
        }

        case 96:
        case 97: {
            next = (this->GetClientStandState() != 1) + 97;

            if (next == 506) {
                break;
            }

            this->PlayBaseAnimation(next, 16);
            return;
        }

        case 99:
        case 100: {
            next = (this->GetClientStandState() != 3) + 100;

            if (next == 506) {
                break;
            }

            this->PlayBaseAnimation(next, 16);
            return;
        }

        case 102: {
            if (this->GetClientStandState() != 4) {
                break;
            }

            this->PlayBaseAnimation(102, 16);
            return;
        }

        case 103: {
            if (this->GetClientStandState() != 5) {
                break;
            }

            this->PlayBaseAnimation(103, 16);
            return;
        }

        case 104: {
            if (this->GetClientStandState() != 6) {
                break;
            }

            this->PlayBaseAnimation(104, 16);
            return;
        }

        case 105: {
            this->PlayBaseAnimation(109, 16);
            return;
        }

        case 106: {
            this->PlayBaseAnimation(110, 16);
            return;
        }

        case 109: {
            if (!this->m_rangedWeaponModel) {
                break;
            }

            if (this->m_unit->m_unitChannelSpell || (this->m_animationState & 0x600)) {
                this->PlayBaseAnimation(109, 16);
                return;
            }

            if (!this->m_rangedWeaponModel || !this->m_ammoModel || !this->m_ammoModel->m_attachParent) {
                break;
            }

            this->m_ammoModel->DetachFromParent();
            this->m_animationState &= ~0x4000;
            this->m_rangedWeaponModel->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
            this->UpdateBaseAnimation(16, -1);
            return;
        }

        case 110: {
            if (!this->m_rangedWeaponModel || (!this->m_unit->m_unitChannelSpell && !(this->m_animationState & 0x400))) {
                break;
            }

            this->PlayBaseAnimation(110, 16);
            return;
        }

        case 111:
        case 112: {
            if (!this->m_rangedWeaponModel || (!this->m_unit->m_unitChannelSpell && !(this->m_animationState & 0x400))) {
                break;
            }

            this->PlayBaseAnimation(111, 16);
            return;
        }

        case 114:
        case 115: {
            next = (this->GetClientStandState() != 8) + 115;

            if (next == 506) {
                break;
            }

            this->PlayBaseAnimation(next, 16);
            return;
        }

        case 127: {
            this->m_animationState &= ~0x8;
            this->UpdateBaseAnimation(16, -1);
            return;
        }

        case 131: {
            if (!this->AppearsDead(1)) {
                break;
            }

            M2BoneSequenceInfo info;
            model->GetBoneSequenceInfo(boneId, info);
            this->SetKeyBoneSequence(model, 0xFFFFFFFF, 132, info.m_variationIndex, 0, 1.0f, 1, 1, 0);
            this->UpdateObjectEffectAnimationStates();
            return;
        }

        case 132: {
            if (!this->AppearsDead(1)) {
                break;
            }

            return;
        }

        case 133: {
            this->CreateFishingLineEffect();
            this->PlayBaseAnimation(134, 16);
            return;
        }

        case 201: {
            this->m_animationState &= ~0x400000;
            this->PlayBaseAnimation(202, 16);
            return;
        }

        case 464: {
            this->ChooseDefaultAnim(next, 1);

            if (next == 506) {
                break;
            }

            this->PlayBaseAnimation(next, 16);
            return;
        }

        case 466:
        case 467: {
            if (!this->AppearsDead(1)) {
                break;
            }

            this->PlayBaseAnimation((this->m_animationState & 0x4000000) ? 467 : 468, 16);
            return;
        }

        case 468: {
            if (!this->AppearsDead(1)) {
                break;
            }

            M2BoneSequenceInfo info;
            model->GetBoneSequenceInfo(boneId, info);
            this->SetKeyBoneSequence(model, 0xFFFFFFFF, this->ResolveAnimForModel(472, NULL), info.m_variationIndex, 0, 1.0f, 1, 1, 0);
            this->UpdateObjectEffectAnimationStates();
            return;
        }

        default: {
            break;
        }
    }

    this->UpdateBaseAnimation(16, -1);
}

// OFFSET: #0x00737BD0
void CGUnit_C::OnKeyBoneAnimEnd(CM2Model* model, uint32_t boneId, uint32_t sequenceId) {
    if (sequenceId == 15) {
        return;
    }

    if (sequenceId > 88 && sequenceId <= 90) {
        if (boneId == 2) {
            if (!(this->m_animationState & 0x200000) && this->UnsheatheLeftWeapon()) {
                return;
            }
        } else if (boneId == 3) {
            if (!(this->m_animationState & 0x100000) && this->UnsheatheRightWeapon()) {
                return;
            }
        }
    }

    M2BoneSequenceInfo torso;
    torso.m_sequenceId = 0xFFFFFFFF;
    torso.m_variationIndex = 0xFFFFFFFF;
    torso.m_time = 0;
    torso.m_speed = 1.0f;
    torso.m_startTime = 0;
    torso.m_endTime = 0;
    torso.m_finished = 0;
    torso.m_complete = 0;
    uint32_t anim = 0xFFFFFFFF;
    int32_t unsetTorso = 0;
    int32_t usedDefault = 0;

    if (this->m_worldModel && this->m_worldModel->IsLoaded()) {
        uint32_t animFlags = 0;
        uint32_t chosen = this->ChooseAnimation(0xFFFFFFFF, &animFlags, &usedDefault);
        anim = this->ResolveAnimForModel(chosen, NULL);
        torso.m_sequenceId = anim;
        CVehiclePassenger_C* passenger = this->m_vehiclePassenger;
        const VehicleSeatRec* seat = passenger ? passenger->m_seatRec : NULL;
        int32_t ridingLower = passenger && passenger->m_state == 3 && passenger->GetLowerAnim(seat) != 506;

        if (static_cast<int32_t>(boneId) == this->m_torsoKeyBone) {
            if (!usedDefault && chosen == 506 && ridingLower) {
                this->UpdateObjectEffectAnimationStates();
                return;
            }
        }

        if (static_cast<int32_t>(boneId) != this->m_torsoKeyBone || !ridingLower || this->m_vehiclePassenger->GetUpperAnim(seat) == 506) {
            uint32_t current = this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);

            if (!IsUpperBodyAnim(anim) || !this->ShouldSplitTorsoAnimation(anim, current) || anim == current) {
                unsetTorso = 1;
            }
        }
    }

    M2BoneSequenceInfo overrideTorso;

    if (this->ApplyAttachmentAnimOverride(NULL, NULL, &torso, 1, NULL, NULL, &overrideTorso, NULL, NULL)) {
        this->SetKeyBoneSequence(model, boneId, overrideTorso.m_sequenceId, overrideTorso.m_variationIndex, overrideTorso.m_time, overrideTorso.m_speed, 1, 1, 0);
    } else if (!unsetTorso) {
        this->SetKeyBoneSequence(model, boneId, anim, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0);
    } else if (model && (!this->m_vehiclePassenger || !this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims())) {
        if (model->IsLoaded() && model->BoneHasParent(boneId)) {
            model->UnsetBoneSequence(boneId, 1, 1);
        }

        CVehicle_C* vehicle = this->m_vehicle;

        if (vehicle && vehicle->m_vehicleRec && vehicle->IsMirroringAnimsToPassengers() && model == this->GetObjectModel()) {
            for (CPassenger* rider = vehicle->m_passengers.Head(); rider; rider = vehicle->m_passengers.Next(rider)) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*rider->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

                if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
                    unit->UnsetKeyBoneSequence(unit->GetObjectModel(), boneId, 1, 1, 1);
                }
            }
        }
    }

    this->UpdateObjectEffectAnimationStates();
}

// OFFSET: 0x005D3C70
void CGUnit_C::ExchangeSheathedWeapon(const C3Vector* position, uint32_t hand, uint32_t flag) {
    if (hand == 0) {
        if (!(this->m_animationState & 0x10000)) {
            return;
        }

        this->m_animationState &= ~0x10000;
    } else if (hand == 1) {
        if (!(this->m_animationState & 0x20000)) {
            return;
        }

        this->m_animationState &= ~0x20000;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(hand, false);
    const CGUnitVirtualItem* ranged = this->GetVirtualItem(2, false);
    uint32_t state;
    bool sheathe;

    if ((flag & this->m_animationState) && (state = this->m_sheatheState) != 0) {
        sheathe = false;
    } else {
        state = this->m_previousSheatheState;
        sheathe = true;
    }

    if (state != 2) {
        this->SheatheHandItem(hand, sheathe);
        SI3::PlaySheatheSound(item, sheathe, *position, this->m_obj->m_guid == CGUnit_C::s_activeMover);
    } else if (!ranged || (ranged->m_inventoryType == INVTYPE_RANGED ? hand != 0 : (static_cast<uint8_t>(ranged->m_inventoryType - INVTYPE_THROWN) > 1 || hand != 1))) {
        this->ShowRangedWeapon(sheathe, NULL);
        SI3::PlaySheatheSound(ranged, sheathe, *position, this->m_obj->m_guid == CGUnit_C::s_activeMover);
    }

    if (this->m_previousSheatheState == 2) {
        this->AddHandItem(0);
    }

    WOWGUID guid = this->m_obj->m_guid;
    CGGameUI::UnitModelUpdate(guid, 1);
}

// OFFSET: 0x005C59D0
static int32_t UpdateAllSmoothFacingCallback(WOWGUID guid, void* param) {
    CGCamera* camera = static_cast<CGCamera*>(param);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT) && unit->m_obj->m_guid != camera->m_targetGUID && !unit->HasVehicleTransport()) {
        unit->UpdateSmoothFacing(NULL, NULL);
    }

    return 1;
}

// PROVEN: 6bcd46aa IDENTICAL/NONE
// OFFSET: 0x005C5A40
void CGUnit_C::UpdateAllSmoothFacing() {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera) {
        ClntObjMgrEnumVisibleObjects(UpdateAllSmoothFacingCallback, camera);
    }
}

// OFFSET: #0x007190A0
static bool IsOwnedByActivePlayer(const WOWGUID& guid, bool* isPlayer) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        *isPlayer = true;
        return true;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->GetCharmerOrCreatorGuid()->m_guid) {
        return false;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*unit->GetCharmerOrCreatorGuid(), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (*unit->GetCharmerOrCreatorGuid() == ClntObjMgrGetActivePlayer() || (owner && *owner->GetCharmerOrCreatorGuid() == ClntObjMgrGetActivePlayer())) {
        *isPlayer = false;
        return true;
    }

    return false;
}

// OFFSET: 0x005B8E80
void CGUnit_C::AddWorldText(const WOWGUID& source, int32_t reason, const SpellRec* spell, bool periodic) {
    bool melee = !spell || (spell->m_attributesExC & 0x8000);
    bool isPlayer;

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover || !IsOwnedByActivePlayer(source, &isPlayer) || !g_combatDamageCVar->m_intValue || (periodic && !s_cvCombatLogPeriodicSpells->m_intValue)) {
        return;
    }

    const CImVector* color;

    if (melee) {
        if (isPlayer) {
            color = NULL;
        } else {
            if (!g_petMeleeDamageCVar->m_intValue) {
                return;
            }

            color = reinterpret_cast<const CImVector*>(&s_petMeleeDamageColor);
        }
    } else {
        if (!isPlayer && !g_petSpellDamageCVar->m_intValue) {
            return;
        }

        color = reinterpret_cast<const CImVector*>(&s_petSpellDamageColor);
    }

    const char* text = FrameScript_GetText(s_combatTextTokens[reason], -1, GENDER_NOT_APPLICABLE);

    if (text && *text) {
        PlayerNameCreateText(this->m_nameDesc, s_combatTextTypes[reason], text, color, NULL);
    }
}

// OFFSET: 0x005BBA40
void CGUnit_C::AddWorldDamageText(const WOWGUID& source, int32_t amount, const SpellRec* spell, bool crit, bool periodic, bool spellColor) {
    bool melee = !spellColor && (!spell || (spell->m_attributesExC & 0x8000));
    bool isPlayer;

    if (!amount || this->m_obj->m_guid == CGUnit_C::s_activeMover || !IsOwnedByActivePlayer(source, &isPlayer) || !g_combatDamageCVar->m_intValue || (periodic && !s_cvCombatLogPeriodicSpells->m_intValue)) {
        return;
    }

    char text[32];
    SStrPrintf(text, sizeof(text), "%d", amount);
    const CImVector* color;

    if (melee) {
        if (isPlayer) {
            color = NULL;
        } else {
            if (!g_petMeleeDamageCVar->m_intValue) {
                return;
            }

            color = reinterpret_cast<const CImVector*>(&s_petMeleeDamageColor);
        }
    } else {
        if (!isPlayer && !g_petSpellDamageCVar->m_intValue) {
            return;
        }

        color = reinterpret_cast<const CImVector*>(&s_petSpellDamageColor);
    }

    PlayerNameCreateText(this->m_nameDesc, crit ? 2 : 0, text, color, NULL);
}

// OFFSET: #0x0073C220
void CGUnit_C::SetUpdateInfo(const CClientMoveUpdate& update, int32_t activeMover) {
    this->m_movementData.SetUpdateInfo(OsGetAsyncTimeMs(), &update, activeMover);

    if (this->m_movementData.m_flags & MOVEMENTFLAG_FALLING_FAR) {
        this->OnCollideFalling();
    }
}

// OFFSET: 0x005CA7E0/0x140242760
void CGUnit_C::UpdateModelScale(const CreatureModelDataRec* modelRec) {
    float oldScale = this->m_modelScale;
    this->m_modelScale = this->GetModelScale(this->m_unit->m_unitFieldDisplayid);
    float scale;

    if (modelRec) {
        scale = this->m_scale;
    } else {
        scale = oldScale / this->m_modelScale * this->m_scale;
    }

    this->CGObject_C::HandleScaleUpdate(scale);
    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->UpdateLargestPassengerBoundsRadius();
    }
}

// OFFSET: #0x004F6250
int32_t CGUnit_C::IsSeatedInVehicle() const {
    return this->m_vehiclePassenger && this->m_vehiclePassenger->m_state == 3;
}

// PROVEN: 4444db38 IDENTICAL/NONE
// OFFSET: 0x005AD7F0
int32_t Packet_SMSG_FORCE_DISPLAY_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    uint32_t displayId = 0;
    *msg >> guid;
    msg->Get(displayId);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_unit->m_unitFieldDisplayid == displayId) {
        unit->UpdateDisplayInfo(1);
        unit->m_clientFlags |= 0x8000000;
    }

    return 1;
}

// PROVEN: c367eb9d IDENTICAL/NONE
// OFFSET: 0x005AD4D0
int32_t Packet_SMSG_FLIGHT_SPLINE_SYNC(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    float progress;
    SmartGUID guid;
    msg->Get(progress);
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->m_movementData.ComputeSplineDurationModifier(progress);
    }

    return 1;
}

// PROVEN: 25ee5d06 IDENTICAL/NONE
// OFFSET: 0x005B69E0
int32_t Packet_SMSG_HEALTH_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    uint32_t health;
    *msg >> guid;
    msg->Get(health);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_predictedHealth != health) {
        unit->m_predictedHealth = health;
        WOWGUID target = unit->m_obj->m_guid;
        Script_SendUniqueSignal(target, SCRIPT_EVENT_UNIT_HEALTH_FREQUENT);
    }

    return 1;
}

// OFFSET: 0x005C4260
int32_t Packet_SMSG_POWER_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    uint32_t count;
    msg->Get(count);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        msg->m_read = msg->m_size;
        return 1;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        uint8_t powerType;
        int32_t value;
        msg->Get(powerType);
        msg->Get(value);
        int32_t slot;

        if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
            slot = CGUnit_C::s_powerSlotByType[unit->m_unit->m_unitFieldBytes0[1]][powerType];

            if (slot == -1) {
                continue;
            }
        } else if (powerType == unit->m_unit->m_unitFieldBytes0[3]) {
            slot = 0;
        } else if (powerType == 10) {
            slot = 1;
        } else {
            continue;
        }

        unit->SetPredictedPowerSlot(slot, powerType, value, false);
    }

    return 1;
}

// OFFSET: 0x005B04C0/0x1402233C0
bool CGUnit_C::IsInMyParty(const CGUnit_C* unit) const {
    if (unit == this) {
        return true;
    }

    if (unit->m_creatureCacheEntry && (unit->m_creatureCacheEntry->m_typeFlags[0] & 0x4000000)) {
        return true;
    }

    if (!(this->m_unit->m_unitFieldFlags & 8) || !(unit->m_unit->m_unitFieldFlags & 8)) {
        return false;
    }

    CGUnit_C* controller = const_cast<CGUnit_C*>(this)->GetControllingPlayer();
    CGUnit_C* otherController = const_cast<CGUnit_C*>(unit)->GetControllingPlayer();

    if (!controller || !otherController) {
        return false;
    }

    if (controller->IsActivePlayer()) {
        WOWGUID guid = otherController->m_obj->m_guid;

        if (CGGameUI::IsPartyMember(guid)) {
            return true;
        }
    }

    if (otherController->IsActivePlayer()) {
        WOWGUID guid = controller->m_obj->m_guid;

        if (CGGameUI::IsPartyMember(guid)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005CAB80
void CGUnit_C::ExecuteClientControlUpdate(WOWGUID guid, uint8_t state) {
    this->SetLocalClientControl(state != 0);
    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    WOWGUID target = camera->m_targetGUID;

    if (target == guid) {
        if (this->m_clientFlags & 0x400) {
            CGUnit_C::SetActiveMover(guid.m_guid);
            return;
        }

        if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
            CGUnit_C::SetActiveMover(0);
        }
    } else if (this->m_obj->m_guid == CGUnit_C::s_activeMover && !(this->m_clientFlags & 0x400)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__));

        if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT) && (unit->m_clientFlags & 0x400)) {
            CGUnit_C::SetActiveMover(target.m_guid);
            return;
        }

        CGUnit_C::SetActiveMover(0);
    }
}

// OFFSET: 0x005B03F0/0x1402232C0
bool CGUnit_C::IsInMyPartyOrRaid(const CGUnit_C* unit) const {
    if (unit == this) {
        return true;
    }

    if (!(this->m_unit->m_unitFieldFlags & 8) || !(unit->m_unit->m_unitFieldFlags & 8)) {
        return false;
    }

    CGUnit_C* controller = const_cast<CGUnit_C*>(this)->GetControllingPlayer();
    CGUnit_C* otherController = const_cast<CGUnit_C*>(unit)->GetControllingPlayer();

    if (!controller || !otherController) {
        return false;
    }

    if (controller->IsActivePlayer()) {
        WOWGUID guid = otherController->m_obj->m_guid;

        if (CGGameUI::IsInParty(guid)) {
            return true;
        }
    }

    if (otherController->IsActivePlayer()) {
        WOWGUID guid = controller->m_obj->m_guid;

        if (CGGameUI::IsInParty(guid)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005AE6D0
void CGUnit_C::OnVisFlagChanged(uint8_t oldFlags) {
    uint8_t changed = oldFlags ^ this->m_unit->m_unitFieldBytes1[2];

    if (changed & 2) {
        this->UpdateMovementAnimKit(0);

        if (this->m_nameDesc) {
            PlayerNameTriggerNameRegenerate(this->m_nameDesc);
        }

        if (this->m_namePlateFrame) {
            this->m_namePlateFrame->UpdateNameDisplay(this);
        }

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_STEALTH, NULL);
            CGSpellBook::UpdateUsable();
        }
    }

    if (changed & 1) {
        if (this->m_nameDesc) {
            PlayerNameTriggerNameRegenerate(this->m_nameDesc);
        }

        if (this->m_namePlateFrame) {
            this->m_namePlateFrame->UpdateNameDisplay(this);
        }
    }
}

// OFFSET: 0x005C0650/0x140236200
bool CGUnit_C::IsSpellKnown(int32_t spellId) {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        if (static_cast<uint32_t>(spellId) <= static_cast<uint32_t>(g_spellDB.m_maxID)) {
            return (s_knownSpellBits.m_bits.m_data[static_cast<uint32_t>(spellId) >> 5] & (1 << (spellId & 0x1F))) != 0;
        }
    } else if (this->m_obj->m_guid == CGPetInfo::GetPet(0)) {
        return HasPetSpell(spellId);
    }

    return false;
}

// OFFSET: #0x0071A4B0
const char* CGUnit_C::GetDisplayRaceName(const CGUnit_C* unit, const NameCache* name, int32_t* displaySex) {
    int32_t sex = 2;
    ChrRacesRec* rec;

    if (unit) {
        rec = g_chrRacesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[0]);
        sex = unit->m_unit->m_unitFieldBytes0[2];
    } else {
        if (!name) {
            if (displaySex) {
                *displaySex = sex;
            }

            return NULL;
        }

        rec = g_chrRacesDB.GetRecord(name->m_race);
        sex = name->m_sex;
    }

    if (rec) {
        return CGUnit_C::GetDisplayRaceNameFromRecord(rec, sex, displaySex);
    }

    if (displaySex) {
        *displaySex = sex;
    }

    return NULL;
}

// OFFSET: #0x0071A540
int32_t CGUnit_C::GetDisplayRaceGender(const CGUnit_C* unit, const NameCache* name) {
    if (unit) {
        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return unit->m_unit->m_unitFieldBytes0[2];
        }
    } else if (!name) {
        return 2;
    }

    int32_t displaySex = 0;
    CGUnit_C::GetDisplayRaceName(unit, name, &displaySex);
    return displaySex;
}

// OFFSET: 0x005B2480
const char* CGUnit_C::GetDisplayClassName(const CGUnit_C* unit, const NameCache* name, int32_t* displaySex) {
    int32_t sex = 2;
    ChrClassesRec* rec;

    if (unit) {
        rec = g_chrClassesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[1]);
        sex = unit->m_unit->m_unitFieldBytes0[2];
    } else {
        if (!name) {
            if (displaySex) {
                *displaySex = sex;
            }

            return NULL;
        }

        rec = g_chrClassesDB.GetRecord(name->m_class);
        sex = name->m_sex;
    }

    if (rec) {
        return CGUnit_C::GetDisplayClassNameFromRecord(rec, sex, displaySex);
    }

    if (displaySex) {
        *displaySex = sex;
    }

    return NULL;
}

// OFFSET: #0x0071A620
int32_t CGUnit_C::GetDisplayClassGender(const CGUnit_C* unit, const NameCache* name) {
    if (unit) {
        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return unit->m_unit->m_unitFieldBytes0[2];
        }
    } else if (!name) {
        return 2;
    }

    int32_t displaySex = 0;
    CGUnit_C::GetDisplayClassName(unit, name, &displaySex);
    return displaySex;
}

// OFFSET: 0x005C2EB0/0x140239290
bool CGUnit_C::HasAuraMatchingSpellClass(int32_t auraType, const SpellRec* spell) const {
    if (!this->HasAuraEffect(auraType)) {
        return false;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        const AuraData* aura = i < this->GetAuraCount() ? &const_cast<CGUnit_C*>(this)->m_auras[i] : NULL;
        const SpellRec* auraSpell = g_spellDB.GetRecord(aura->m_spellId);

        if (!auraSpell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(auraSpell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == auraType && (aura->m_flags & (1 << j)) && SpellRec::IsAffectedBy(spell, auraSpell, j)) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x005C0700/0x1402362F0
bool CGUnit_C::IsSpellSuperceded(int32_t spellId) {
    SkillLineAbilityRec* ability = this->LookupAbility(spellId);

    if (!ability) {
        return false;
    }

    while (ability->m_supercededBySpell > 0) {
        if (this->IsSpellKnown(ability->m_supercededBySpell)) {
            return true;
        }

        ability = this->LookupAbility(ability->m_supercededBySpell);

        if (!ability) {
            return false;
        }
    }

    return false;
}

// OFFSET: 0x005B1FC0
static bool IsEmoteWithoutSpecProc(uint32_t emote) {
    if (!emote) {
        return false;
    }

    const EmotesRec* rec = g_emotesDB.GetRecord(emote);
    return rec && rec->m_emoteSpecProc == 0;
}

// OFFSET: 0x005B7320/0x14022AB70
void CGUnit_C::PlayEmoteAnimation(uint32_t emote, int32_t force, int32_t ignoreChannel) {
    EmotesRec* rec = g_emotesDB.GetRecord(emote);

    if (!rec) {
        return;
    }

    uint32_t anim = rec->m_animID;

    if (anim > 659 || this->GetCurrentTorsoAnimId() == anim) {
        return;
    }

    bool flight = anim == ANIM_LIFT_OFF || anim == ANIM_LAND;

    if ((this->m_unit->m_unitChannelSpell && !ignoreChannel) || (this->m_animationState & 0x10)) {
        return;
    }

    if (!flight && !force && this->IsInCombat()) {
        return;
    }

    if (this->m_unit->m_unitFieldMountdisplayid > 0 && (rec->m_emoteFlags & 0x2)) {
        CAnimKitManager* manager = this->m_mountAnimKitManager ? this->m_mountAnimKitManager : &this->m_animKitManager;

        if (manager->GetExpressingAnim(0xFFFFFFFF, 1, 1) != anim) {
            this->PlayMountSpecialAnimKit(anim);
        }
    } else {
        this->PlayEmoteAnimKit(anim, !IsEmoteWithoutSpecProc(emote), rec->m_emoteFlags & 0x20000);
    }
}

// OFFSET: #0x0073AD70
void CGUnit_C::RequestTalkEmote(int32_t index) {
    if (!s_talkEmotes[index]) {
        return;
    }

    if (this->m_movement->m_flags & 0x200000) {
        return;
    }

    if (this->GetClientStandState() == 3) {
        return;
    }

    this->PlayEmoteAnimation(s_talkEmotes[index]->m_id, 0, 0);
}

// PROVEN: c6b81281 IDENTICAL/NONE
// OFFSET: 0x005BA0A0
void CGUnit_C::PlayEmotes(const QUESTGIVEREMOTEQUEUE& emotes) {
    this->SetEmoteQueue(emotes.m_nodes, emotes.m_count);
}

// OFFSET: #0x0072AA70
const char* CGUnit_C::GetDisplayRaceName(const CGUnit_C* unit, const NameCache* name) {
    if (unit) {
        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return const_cast<CGUnit_C*>(unit)->GetUnitName(NULL, 1);
        }
    } else if (!name) {
        return NULL;
    }

    return CGUnit_C::GetDisplayRaceName(unit, name, NULL);
}

// OFFSET: 0x005C0610/0x140022960
const char* CGUnit_C::GetDisplayClassName(const CGUnit_C* unit, const NameCache* name) {
    if (unit) {
        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            return const_cast<CGUnit_C*>(unit)->GetUnitName(NULL, 1);
        }
    } else if (!name) {
        return NULL;
    }

    return CGUnit_C::GetDisplayClassName(unit, name, NULL);
}

// OFFSET: #0x00726220
void CGUnit_C::ClearUIChannel() {
    this->m_channelSpellId = 0;
    this->m_channelStartTime = 0;
    this->m_channelEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: #0x00726240
void CGUnit_C::DelayCast(int32_t delay) {
    this->m_castStartTime += delay;
    this->m_castEndTime += delay;
    this->m_channelSpellId = 0;
    this->m_channelStartTime = 0;
    this->m_channelEndTime = 0;
    this->UpdateSpellCastBars();
}

// OFFSET: 0x005B78A0/0x14022B240
void WriteMovementPacket(CGUnit_C* unit, CDataStore* msg) {
    if (!unit || unit->m_obj->m_guid != CGUnit_C::s_activeMover || unit->m_movement->IsOnSpline()) {
        msg->Put(static_cast<uint8_t>(0));
        return;
    }

    msg->Put(static_cast<uint8_t>(1));
    CMovementStatus status;
    unit->m_movementData.GetMoveStatus(static_cast<NETMESSAGE>(0x781E), MovementGetIdleTime(), &status);
    status.WriteEmbedded(msg);
}

// OFFSET: #0x0071C8F0
uint8_t CGUnit_C::GetActivePlayerFieldBytes0() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    return static_cast<CGPlayer_C*>(this)->m_player->m_playerFieldBytes[0];
}

// OFFSET: #0x00727C00
void CGUnit_C::InsertSortedAura(uint32_t list, uint32_t auraIndex, int32_t position) {
    AuraData* aura = auraIndex < this->GetAuraCount() ? &this->m_auras[auraIndex] : NULL;
    WOWGUID player = ClntObjMgrGetActivePlayer();
    TSCGrowableArray<SortedAuraInfo, 16>& sorted = this->m_sortedAuras[list];

    if (position) {
        if (CGSpellBook::IsAuraCaster(player, &aura->m_creator)) {
            int32_t i = position - 1;

            for (; i >= 0; i--) {
                uint32_t otherIndex = sorted[i].m_auraIndex;
                AuraData* other = otherIndex < this->GetAuraCount() ? &this->m_auras[otherIndex] : NULL;

                if (CGSpellBook::IsAuraCaster(player, &other->m_creator)) {
                    break;
                }

                sorted[i + 1] = sorted[i];
            }

            position = i + 1;
        } else if (CGSpellBook::CanApplyAura(aura)) {
            int32_t i = position - 1;

            for (; i >= 0; i--) {
                AuraData* other = this->GetAura(sorted[i].m_auraIndex);

                if (CGSpellBook::IsAuraCaster(player, &other->m_creator) || CGSpellBook::CanApplyAura(other)) {
                    break;
                }

                sorted[i + 1] = sorted[i];
            }

            position = i + 1;
        }
    }

    sorted[position].m_auraIndex = auraIndex;
    sorted[position].m_spellId = aura->m_spellId;
}

// OFFSET: 0x005CA190/0x140242020
void CGUnit_C::SortAuras() {
    for (uint32_t list = 0; list < 2; list++) {
        TSCGrowableArray<SortedAuraInfo, 16>& sorted = this->m_sortedAuras[list];
        uint32_t i = 0;

        while (i < this->GetAuraCount() && sorted[i].m_spellId) {
            AuraData& aura = this->m_auras[sorted[i].m_auraIndex];

            if (aura.m_spellId == sorted[i].m_spellId && (aura.m_flags & 0x7)) {
                i++;
                continue;
            }

            uint32_t last = this->GetAuraCount() - 1;

            if (i < last) {
                memcpy(&sorted[i], &sorted[i + 1], sizeof(SortedAuraInfo) * (last - i));
            }

            sorted[last].m_auraIndex = 256;
            sorted[last].m_spellId = 0;
        }
    }

    for (uint32_t index = this->GetAuraCount(); index; ) {
        index--;

        AuraData* aura = index < this->GetAuraCount() ? &this->m_auras[index] : NULL;

        const SpellRec* spell;

        if (!aura->m_spellId || !(aura->m_flags & 0x7) || !(spell = g_spellDB.GetRecord(aura->m_spellId))) {
            continue;
        }

        if (!CGSpellBook::CanShowAura(spell, &aura->m_creator, (this->m_auras[index].m_flags & 0x8) != 0)) {
            continue;
        }

        for (uint32_t j = 0; j < this->GetAuraCount(); j++) {
            if (this->m_sortedAuras[0][j].m_spellId <= 0) {
                this->InsertSortedAura(0, index, j);
                break;
            }

            if (this->m_sortedAuras[0][j].m_spellId == aura->m_spellId && this->m_sortedAuras[0][j].m_auraIndex == index) {
                break;
            }
        }

        int32_t dispelType = 0;

        if (spell->m_spellCategoriesID) {
            SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

            if (categories) {
                dispelType = categories->m_dispelType;
            }
        }

        if ((!(aura->m_flags & 0x80) && CGSpellBook::FindRaidBuff(aura->m_spellId)) || ((aura->m_flags & 0x80) && CGSpellBook::FindRaidDispel(dispelType))) {
            for (uint32_t j = 0; j < this->GetAuraCount(); j++) {
                if (this->m_sortedAuras[1][j].m_spellId <= 0) {
                    this->InsertSortedAura(1, index, j);
                    break;
                }

                if (this->m_sortedAuras[1][j].m_spellId == aura->m_spellId && this->m_sortedAuras[1][j].m_auraIndex == index) {
                    break;
                }
            }
        }
    }

    this->m_clientFlags |= 0x40000;
}

// OFFSET: 0x005CA600/0x1402424E0
uint32_t CGUnit_C::GetSortedAuraSlot(int32_t index, const char* name, const char* rank, uint32_t filter) {
    if (index < 0 && !name) {
        return 0xFFFFFFFF;
    }

    if (!(this->m_clientFlags & 0x40000)) {
        this->SortAuras();
    }

    WOWGUID player = ClntObjMgrGetActivePlayer();
    bool helpful = filter & 0x1;
    bool playerOnly = (filter & 0x4) != 0;
    int32_t found = 0;

    if (!this->GetAuraCount()) {
        return 0xFFFFFFFF;
    }

    TSCGrowableArray<SortedAuraInfo, 16>& sorted = this->m_sortedAuras[(filter & 0x8) != 0];

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        uint32_t auraIndex = sorted[i].m_auraIndex;
        AuraData* aura = auraIndex < this->GetAuraCount() ? &this->m_auras[auraIndex] : NULL;

        if (!aura || !aura->m_spellId || helpful != !(aura->m_flags & 0x80)) {
            continue;
        }

        if (playerOnly && !CGSpellBook::IsAuraCaster(player, &aura->m_creator)) {
            continue;
        }

        if ((aura->m_flags & 0x10) ? (filter & 0x20) : (filter & 0x10)) {
            continue;
        }

        if (index >= 0) {
            if (index == found) {
                return auraIndex;
            }

            found++;
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(this->GetAuraSpellId(auraIndex));

        if (spell && !SStrCmpUTF8I(name, spell->m_name, STORM_MAX_STR) && (!rank || !SStrCmpUTF8I(rank, spell->m_nameSubtext, STORM_MAX_STR))) {
            return auraIndex;
        }
    }

    return 0xFFFFFFFF;
}

// OFFSET: 0x005B47C0/0x140227D60
float CGUnit_C::GetSpellChanceToCrit() const {
    float base = GetGameTableValue(5, 0, this->m_unit->m_unitFieldBytes0[1] - 1);
    float ratio = GetGameTableValue(4, this->m_unit->m_unitFieldLevel - 1, this->m_unit->m_unitFieldBytes0[1] - 1);

    if (ratio == 0.0f) {
        return 0.0f;
    }

    int32_t intellect = this->m_unit->m_unitFieldStat[3];
    return (ratio * (intellect < 0 ? 0 : intellect) + base) * 100.0f;
}

// OFFSET: #0x00715D60
void CGUnit_C::TriggerDamageFeedback() {
    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->TriggerDamageFeedback();
    }
}

// OFFSET: 0x005C99E0/0x1402415E0
int32_t CGUnit_C::VerifyAttackIconPressed(WOWGUID* target, int32_t useGiven, int32_t checkRange, bool autoTarget, bool showErrors) {
    if (this->IsDeadOrGhost()) {
        CGPlayer_C* player;

        if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !(player = ClntObjMgrGetActivePlayerObj()) || !player->GetPossessedUnit()) {
            if (showErrors) {
                CGGameUI::DisplayError(ERR_ATTACK_DEAD);
            }

            return 0;
        }
    } else {
        if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(this)->m_player->m_playerFlags & 0x1000000)) {
            if (showErrors) {
                CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            }

            return 0;
        }

        int32_t mechanic;

        if (this->m_unit->m_unitFieldCharmedby && this->m_unit->m_unitFieldCharmedby != ClntObjMgrGetActivePlayer()) {
            Spell_C_CancelsCharm(this, NULL, &mechanic);

            if (!showErrors) {
                return 0;
            }

            if (!mechanic) {
                CGGameUI::DisplayError(ERR_ATTACK_CHARMED);
                return 0;
            }
        } else {
            uint32_t flags = this->m_unit->m_unitFieldFlags;

            if (flags & 0x40000) {
                Spell_C_CancelsStun(this, NULL, &mechanic);

                if (!showErrors) {
                    return 0;
                }

                if (!mechanic) {
                    CGGameUI::DisplayError(ERR_ATTACK_STUNNED);
                    return 0;
                }
            } else if (flags & 0x20000) {
                Spell_C_CancelsPacify(this, NULL, &mechanic);

                if (!showErrors) {
                    return 0;
                }

                if (!mechanic) {
                    CGGameUI::DisplayError(ERR_ATTACK_PACIFIED);
                    return 0;
                }
            } else if (this->m_unit->m_unitFieldFlags2 & 0x800000) {
                Spell_C_CancelsNoActions(this, NULL, &mechanic);

                if (!showErrors) {
                    return 0;
                }

                if (!mechanic) {
                    CGGameUI::DisplayError(ERR_ATTACK_NO_ACTIONS);
                    return 0;
                }
            } else if (flags & 0x800000) {
                Spell_C_CancelsFear(this, NULL, &mechanic);

                if (!showErrors) {
                    return 0;
                }

                if (!mechanic) {
                    CGGameUI::DisplayError(ERR_ATTACK_FLEEING);
                    return 0;
                }
            } else if (flags & 0x400000) {
                Spell_C_CancelsConfuse(this, NULL, &mechanic);

                if (!showErrors) {
                    return 0;
                }

                if (!mechanic) {
                    CGGameUI::DisplayError(ERR_ATTACK_CONFUSED);
                    return 0;
                }
            } else {
                mechanic = -1;
            }
        }

        if (mechanic != -1) {
            SpellMechanicRec* mechanicRec = g_spellMechanicDB.GetRecord(mechanic);
            CGGameUI::DisplayError(ERR_ATTACK_PREVENTED_BY_MECHANIC_S, mechanicRec ? mechanicRec->m_stateName : "UNKNOWN");
            return 0;
        }

        if (this->m_unit->m_unitChannelSpell) {
            const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

            if (spell && spell->m_spellInterruptsID) {
                SpellInterruptsRec* interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID);

                if (interrupts && (interrupts->m_channelInterruptFlags & 0x1000)) {
                    if (showErrors) {
                        CGGameUI::DisplayError(ERR_ATTACK_CHANNEL);
                    }

                    return 0;
                }
            }
        }

        if (this->IsMounted() && !this->MountedCombatAllowed() && !this->CanAutoDismount()) {
            if (showErrors) {
                CGGameUI::DisplayError(ERR_ATTACK_MOUNTED);
            }

            return 0;
        }
    }

    if (!useGiven) {
        bool haveTarget = true;

        if (!*target) {
            *target = CGGameUI::m_lockedTarget;
            haveTarget = static_cast<bool>(*target);
        }

        if (haveTarget) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*target, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit && this->IsFriendly(unit)) {
                *target = WOWGUID();
            } else if (*target) {
                goto checkTarget;
            }
        }

        if (autoTarget) {
            TargetNearestEnemy(0, 1);
        }

        *target = CGGameUI::m_lockedTarget;
    }

checkTarget:
    if (!*target) {
        if (showErrors) {
            CGGameUI::DisplayError(ERR_NO_ATTACK_TARGET);
        }

        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    if (this->IsMounted() && !this->MountedCombatAllowed() && checkRange) {
        float minRange;
        float maxRange;
        int32_t spellId;

        if (this->IsActivePlayer() && (spellId = GetModalNextSpell())) {
            Spell_C::GetSpellRange(this, spellId, &minRange, &maxRange, unit);
        } else {
            maxRange = this->GetMeleeRange(unit);
        }

        if (maxRange * maxRange < this->GetDistanceToPos(unit->GetPosition())) {
            if (showErrors) {
                CGGameUI::DisplayError(ERR_OUT_OF_RANGE);
            }

            return 0;
        }
    }

    if (this->CanAttackNow(unit)) {
        return 1;
    }

    if (showErrors) {
        CGGameUI::DisplayError(ERR_INVALID_ATTACK_TARGET);
    }

    return 0;
}

// OFFSET: 0x005C9F00/0x140241DB0
int32_t CGUnit_C::OnAttackIconPressed(WOWGUID target, int32_t a3, int32_t a4, int32_t a5) {
    WOWGUID previousTarget = CGGameUI::m_lockedTarget;
    int32_t targetChanged = 0;

    if (!this->VerifyAttackIconPressed(&target, a3, 1, 1, 1)) {
        return 0;
    }

    if (previousTarget != target) {
        targetChanged = 1;

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGGameUI::Target(target);
        }
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t controlsPet = 0;

    if (player && player->GetTransportGUID() == CGPetInfo::GetPet(0).m_guid) {
        const WOWGUID& charm = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (charm == CGPetInfo::GetPet(0)) {
            controlsPet = 1;
        }
    }

    int32_t isPlayer = (ClntObjMgrGetActivePlayer() == this->m_obj->m_guid) || controlsPet;
    int32_t petAttack = (CGPetInfo::GetPet(0) == this->m_obj->m_guid) || controlsPet;

    if (isPlayer) {
        if (((player->m_combatModeFlags & 2) || player->m_combat.m_attacking) && CGGameUI::m_hasControl) {
            player->SetCombatMode();
        } else {
            player->CombatModeEnter(target, targetChanged, 0, a4, a5);
        }

        if (petAttack) {
            CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
            CGUnit_C* victim = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!pet || !victim || !pet->CanAttackNow(victim)) {
                petAttack = 0;
            }
        }

        if (player->m_obj->m_guid == CGUnit_C::s_activeMover && player->HasVehicleTransport()) {
            player->CheckVehicleExitOnCast();
        }
    }

    if (petAttack) {
        CGPetInfo::PetAttackTarget(target, a3);

        if (CGPetInfo::GetPet(0) == CGUnit_C::s_activeMover) {
            CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (mover && mover->HasVehicleTransport()) {
                mover->CheckVehicleExitOnCast();
            }
        }
    }

    return 1;
}

// PROVEN: e806479d IDENTICAL/NONE
// OFFSET: 0x005C93C0
bool CGUnit_C::CheckShapeshiftRules(const SpellRec* spell, int32_t* reason) {
    int32_t form = this->m_shapeshiftSuppressed ? 0 : this->m_unit->m_unitFieldBytes2[3];

    if (SpellRec::NotUsableInShapeshiftForm(spell, form - 1)) {
        if (!this->HasAuraMatchingSpellClass(275, spell)) {
            *reason = SPELL_FAILED_NOT_SHAPESHIFT;
            return false;
        }

        return true;
    }

    form = this->m_shapeshiftSuppressed ? 0 : this->m_unit->m_unitFieldBytes2[3];

    if (Spell_C_SpellHasShapeshiftMask(spell, form - 1)) {
        return true;
    }

    if (this->IsShapeShifted()) {
        if (spell->m_attributes & 0x10000) {
            if (!this->HasAuraMatchingSpellClass(275, spell)) {
                *reason = SPELL_FAILED_NOT_SHAPESHIFT;
                return false;
            }

            return true;
        }

        form = this->m_shapeshiftSuppressed ? 0 : this->m_unit->m_unitFieldBytes2[3];
        SpellShapeshiftFormRec* shapeshift = g_spellShapeshiftFormDB.GetRecord(form);

        if (shapeshift && (shapeshift->m_flags & 0x400)) {
            if (!this->HasAuraMatchingSpellClass(275, spell)) {
                *reason = SPELL_FAILED_NOT_SHAPESHIFT;
                return false;
            }

            return true;
        }
    } else if (spell->m_attributesExB & 0x80000) {
        return true;
    }

    if (spell->m_spellShapeshiftID) {
        SpellShapeshiftRec* shapeshift = g_spellShapeshiftDB.GetRecord(spell->m_spellShapeshiftID);

        if (shapeshift && (shapeshift->m_shapeshiftMask[0] || shapeshift->m_shapeshiftMask[1]) && !this->HasAuraMatchingSpellClass(275, spell)) {
            *reason = SPELL_FAILED_ONLY_SHAPESHIFT;
            return false;
        }
    }

    return true;
}

// OFFSET: 0x005B08F0/0x140223800
void CGUnit_C::AddWorldXPGainText(int32_t amount) {
    char text[64] = {};
    char label[64];
    SStrCopy(label, FrameScript_GetText("XP", -1, GENDER_NOT_APPLICABLE), sizeof(label));
    SStrPrintf(text, sizeof(text), "%s: %d", label, amount);
    PlayerNameCreateText(this->m_nameDesc, 4, text, NULL, NULL);
}

// OFFSET: 0x005CA870/0x1402427E0
bool CGUnit_C::CalculateThreat(const WOWGUID& guid, uint8_t* status, uint8_t* rawPercent, float* percent, int32_t* threat) {
    UnitThreat* entry = this->m_threats.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!this->m_highestThreatGuid || !entry) {
        if (status) {
            *status = entry != NULL;
        }

        if (rawPercent) {
            *rawPercent = 0;
        }

        if (percent) {
            *percent = 0.0f;
        }

        if (threat) {
            *threat = 0;
        }

        return false;
    }

    if (status) {
        *status = entry->m_status;
    }

    if (rawPercent) {
        *rawPercent = entry->m_percent;
    }

    if (percent) {
        if (guid == this->m_highestThreatGuid) {
            *percent = 100.0f;
        } else {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
            float range;

            if (unit && (range = this->GetMeleeRange(unit), range * range >= this->GetDistanceToPos(unit->GetPosition()))) {
                *percent = entry->m_percent * 0.90909088f;
            } else {
                *percent = entry->m_percent * 0.76923078f;
            }
        }
    }

    if (threat) {
        *threat = entry->m_threat;
    }

    return true;
}

// OFFSET: #0x007345C0
void CGUnit_C::ClearThreatList() {
    if (!this->m_threats.m_fulllist.Head()) {
        return;
    }

    UnitThreat* entry;

    while ((entry = this->m_threats.m_fulllist.Head())) {
        WOWGUID target = entry->m_key.m_guid;
        Script_SendUniqueSignal(target, SCRIPT_EVENT_UNIT_THREAT_SITUATION_UPDATE);
        CGGameUI::ThreatMapRemove(this->m_obj->m_guid, target);
        this->m_threats.Unlink(entry);
        s_unitThreatPool->Delete(entry);
    }

    this->m_highestThreatGuid = WOWGUID();
    Script_SendUniqueSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_THREAT_LIST_UPDATE);
    CGGameUI::UnitTooltipUpdate(this->m_obj->m_guid);
}

// OFFSET: 0x005B0990/0x1402238E0
void CGUnit_C::AddWorldThreatUpdateText(int32_t oldStatus, int32_t newStatus) {
    if (!CGGameUI::m_threatWarningEnabled || !g_threatWorldTextCVar->m_intValue) {
        return;
    }

    if ((oldStatus == 2 && newStatus == 1) || (oldStatus == 3 && newStatus == 4)) {
        return;
    }

    char token[32];
    SStrPrintf(token, sizeof(token), "COMBAT_THREAT_%s_%d", newStatus > oldStatus ? "INCREASE" : "DECREASE", newStatus);
    const char* text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);

    if (*text) {
        PlayerNameDeleteText(this->m_nameDesc, 10);
        PlayerNameCreateText(this->m_nameDesc, 10, text, NULL, NULL);
    }
}

// OFFSET: #0x007416F0
UnitThreat* CGUnit_C::CreateThreatEntry(const WOWGUID& guid) {
    UnitThreat* entry = this->m_threats.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (entry) {
        return entry;
    }

    entry = s_unitThreatPool->New("", 0);
    entry->m_status = 1;
    entry->m_percent = 0xFF;
    entry->m_threat = 0;
    this->m_threats.Insert(entry, static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (guid == ClntObjMgrGetActivePlayer()) {
        this->m_clientFlags |= 0x8;
    }

    CGGameUI::ThreatMapAdd(this->m_obj->m_guid, guid);
    return entry;
}

// OFFSET: 0x005B5F80
bool CGUnit_C::UpdateThreatEntry(UnitThreat* entry, int32_t highestThreat, bool activePlayer) {
    bool changed = false;
    uint8_t percent;

    if (!highestThreat) {
        percent = 100;
    } else if (entry->m_threat <= 0) {
        percent = 0;
    } else {
        int64_t scaled = 100ll * entry->m_threat / highestThreat;
        percent = scaled >= 250 ? 250 : static_cast<uint8_t>(scaled);
    }

    if (percent == entry->m_percent) {
        return changed;
    }

    entry->m_percent = percent;
    uint8_t status = percent < 100 ? 1 : 2;

    if (status != entry->m_status) {
        if (activePlayer) {
            if (s_cvThreatPlaySounds->m_intValue && status == 2 && entry->m_status == 1) {
                SI3::PlayUISound(15262, NULL, true);
            }

            if (!(this->m_clientFlags & 0x8)) {
                this->AddWorldThreatUpdateText(entry->m_status, status);
            }
        }

        entry->m_status = status;
        Script_SendUniqueSignal(entry->m_key.m_guid, SCRIPT_EVENT_UNIT_THREAT_SITUATION_UPDATE);
        changed = true;
    }

    if (activePlayer) {
        this->m_clientFlags &= ~0x8u;
        CGGameUI::UnitTooltipUpdate(this->m_obj->m_guid);
    }

    return changed;
}

// OFFSET: 0x005CAA40
void CGUnit_C::UpdateThreatStatus() {
    UnitThreat* highest = this->m_threats.Ptr(static_cast<uint32_t>(this->m_highestThreatGuid.m_guid), CHashKeyGUID(this->m_highestThreatGuid.m_guid));

    if (!highest) {
        return;
    }

    uint8_t status = 4;

    for (UnitThreat* entry = this->m_threats.m_fulllist.Head(); entry; entry = this->m_threats.m_fulllist.Next(entry)) {
        if (entry->m_key.m_guid != this->m_highestThreatGuid && entry->m_status == 2) {
            status = 3;
        }
    }

    if (status != highest->m_status) {
        if (this->m_highestThreatGuid == ClntObjMgrGetActivePlayer() && !(this->m_clientFlags & 0x8)) {
            this->AddWorldThreatUpdateText(highest->m_status, status);

            if (s_cvThreatPlaySounds->m_intValue) {
                SI3::PlayUISound(15263, NULL, true);
            }
        }

        highest->m_status = status;
        Script_SendUniqueSignal(this->m_highestThreatGuid, SCRIPT_EVENT_UNIT_THREAT_SITUATION_UPDATE);
    }

    if (this->m_highestThreatGuid == ClntObjMgrGetActivePlayer()) {
        this->m_clientFlags &= ~0x8u;
        CGGameUI::UnitTooltipUpdate(this->m_obj->m_guid);
    }
}

// OFFSET: #0x007417A0
void CGUnit_C::UpdateThreat(CDataStore* msg, bool highest) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();
    bool highestInList = highest;

    if (highest) {
        SmartGUID highestGuid;
        *msg >> highestGuid;
        this->m_highestThreatGuid = highestGuid;

        if (highestGuid) {
            this->CreateThreatEntry(this->m_highestThreatGuid);
        }
    }

    uint32_t count;
    msg->Get(count);
    UnitThreat** entries = static_cast<UnitThreat**>(alloca(count * sizeof(UnitThreat*)));

    for (uint32_t i = 0; i < count; i++) {
        SmartGUID guid;
        *msg >> guid;
        uint32_t threat;
        msg->Get(threat);
        WOWGUID target = guid;
        UnitThreat* entry = this->CreateThreatEntry(target);
        entry->m_threat = threat;

        if (guid == this->m_highestThreatGuid) {
            highestInList = true;
        }

        entries[i] = entry;
    }

    bool changed = false;
    UnitThreat* top = this->m_threats.Ptr(static_cast<uint32_t>(this->m_highestThreatGuid.m_guid), CHashKeyGUID(this->m_highestThreatGuid.m_guid));

    if (this->m_highestThreatGuid && top) {
        int32_t highestThreat = top->m_threat;

        if (highestInList) {
            for (UnitThreat* entry = this->m_threats.m_fulllist.Head(); entry; entry = this->m_threats.m_fulllist.Next(entry)) {
                if (entry->m_key.m_guid != this->m_highestThreatGuid && this->UpdateThreatEntry(entry, highestThreat, entry->m_key.m_guid == activePlayer)) {
                    changed = true;
                }
            }
        } else {
            for (uint32_t i = 0; i < count; i++) {
                UnitThreat* entry = entries[i];

                if (entry->m_key.m_guid != this->m_highestThreatGuid && this->UpdateThreatEntry(entry, highestThreat, entry->m_key.m_guid == activePlayer)) {
                    changed = true;
                }
            }
        }
    }

    if (highest || changed) {
        this->UpdateThreatStatus();
    }

    Script_SendUniqueSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_THREAT_LIST_UPDATE);
}

// OFFSET: #0x00737750
void CGUnit_C::RemoveThreat(const WOWGUID& guid) {
    UnitThreat* entry = this->m_threats.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!entry) {
        return;
    }

    bool update = false;

    if (guid == this->m_highestThreatGuid) {
        this->m_highestThreatGuid = WOWGUID();
    } else if (entry->m_status > 1) {
        update = true;
    }

    CGGameUI::ThreatMapRemove(this->m_obj->m_guid, guid);
    this->m_threats.Unlink(entry);
    s_unitThreatPool->Delete(entry);
    Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_THREAT_SITUATION_UPDATE);

    if (update) {
        this->UpdateThreatStatus();
    }

    Script_SendUniqueSignal(this->m_obj->m_guid, SCRIPT_EVENT_UNIT_THREAT_LIST_UPDATE);

    if (guid == ClntObjMgrGetActivePlayer()) {
        CGGameUI::UnitTooltipUpdate(this->m_obj->m_guid);
    }
}

// OFFSET: 0x005DB490
int32_t Packet_ThreatUpdate(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateThreat(msg, msgId == SMSG_HIGHEST_THREAT_UPDATE);
    } else {
        msg->m_read = msg->m_size;
    }

    return 1;
}

// OFFSET: 0x005D79A0
int32_t Packet_SMSG_THREAT_REMOVE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        *msg >> guid;
        WOWGUID target = guid;
        unit->RemoveThreat(target);
    } else {
        msg->m_read = msg->m_size;
    }

    return 1;
}

// OFFSET: 0x005D7A20
int32_t Packet_SMSG_THREAT_CLEAR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->ClearThreatList();
    }

    return 1;
}

// OFFSET: 0x005AD5F0
int32_t Packet_SMSG_AI_REACTION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    uint32_t reaction;
    msg->Get(reaction);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!reaction) {
            unit->PlayUnitSound(8, 0, 0);
        } else if (reaction == 2) {
            unit->PlayUnitEventSound(0);
        }
    }

    return 1;
}

// PROVEN: 42c8511a IDENTICAL/NONE
// OFFSET: 0x005AD670
int32_t Packet_SMSG_PET_ACTION_SOUND(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    uint32_t action;
    msg->Get(action);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!action) {
            unit->PlayUnitEventSound(1);
        } else if (action == 1) {
            unit->PlayUnitEventSound(2);
        }
    }

    return 1;
}

// OFFSET: 0x005BC760
void CGUnit_C::DoPowerRegen(int32_t powerType, uint32_t elapsed) {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && ((1 << powerType) & static_cast<CGPlayer_C*>(this)->m_player->m_playerFieldBytes2[2])) {
        return;
    }

    if (powerType == 9) {
        return;
    }

    int32_t slot = this->GetPowerTypeSlot(powerType);

    if (slot == -1) {
        return;
    }

    float regen;
    bool interrupted = false;

    if (powerType) {
        interrupted = this->IsPowerRegenInterrupted();
    } else {
        if (this->IsActivePlayer()) {
            interrupted = this->IsPowerRegenInterrupted();
        } else if (this->m_manaRegenInterruptTime) {
            if (this->IsPowerRegenInterrupted() && static_cast<int32_t>(FrameTime::GetCurTimeMs() - this->m_manaRegenInterruptTime) < 0) {
                interrupted = true;
            } else {
                this->m_manaRegenInterruptTime = 0;
            }
        }

        if (this->m_unit->m_unitChannelSpell) {
            const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

            if (spell && !(spell->m_attributesExB & 0x2000000) && !spell->m_powerType && Spell_C_GetPowerCost(spell, this)) {
                interrupted = true;
            }
        }
    }

    regen = this->GetPowerRegen(powerType, interrupted);

    if (fabs(regen) < 0.00000023841858f) {
        return;
    }

    float delta = regen * static_cast<float>(elapsed) * 0.001f;
    int32_t predicted = this->GetPredictedPower(powerType);
    int32_t maximum = this->GetMaxPower(powerType);
    int32_t center = 0;
    int32_t minimum = 0;

    if (powerType == 10 && this->m_powerBarID) {
        UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(this->m_powerBarID);

        if (bar) {
            minimum = bar->m_minPower;
            center = bar->m_centerPower;

            if (center > minimum && center < maximum) {
                goto balance;
            }
        }
    }

    if (powerType != 8) {
        if (delta >= 0.0f && predicted < maximum) {
            this->m_powerRegenRemainder[slot] += static_cast<int32_t>(delta * 1000.0f);
            this->SetPredictedPowerSlot(slot, powerType, predicted + this->m_powerRegenRemainder[slot] / 1000, 0);
            this->m_powerRegenRemainder[slot] %= 1000;
        } else if (delta < 0.0f && predicted > minimum && !(this->m_unit->m_unitFieldFlags2 & 0x100)) {
            float value = static_cast<float>(this->m_powerRegenRemainder[slot]) * 0.001f + static_cast<float>(predicted) + delta;

            if (static_cast<float>(minimum) > value) {
                value = static_cast<float>(minimum);
            }

            this->SetPredictedPowerSlot(slot, powerType, static_cast<int32_t>(value), 0);
            this->m_powerRegenRemainder[slot] = static_cast<int32_t>(value * 1000.0f - static_cast<float>(1000 * static_cast<uint32_t>(static_cast<int32_t>(value))));
        }

        return;
    }

balance:
    if (!minimum) {
        minimum = -maximum;
    }

    if (delta < 0.0f && predicted != center && ((predicted < center && predicted >= minimum) || (predicted > center && predicted <= maximum))) {
        float target = static_cast<float>(center);
        float value;
        bool inRange;

        if (predicted <= center) {
            value = static_cast<float>(predicted) - static_cast<float>(this->m_powerRegenRemainder[slot]) * 0.001f - delta;
            inRange = value <= target;
        } else {
            value = static_cast<float>(this->m_powerRegenRemainder[slot]) * 0.001f + static_cast<float>(predicted) + delta;
            inRange = target <= value;
        }

        if (!inRange) {
            value = target;
        }

        this->SetPredictedPowerSlot(slot, powerType, static_cast<int32_t>(value), 0);
        int32_t whole = abs(static_cast<int32_t>(value));

        if (target > value) {
            value = -value;
        }

        this->m_powerRegenRemainder[slot] = static_cast<int32_t>(value * 1000.0f - static_cast<float>(1000 * whole));
    }
}

// OFFSET: 0x005C36B0
void CGUnit_C::DoPowerRegen(uint32_t elapsed) {
    if (this->IsDeadOrGhost() || !(this->m_unit->m_unitFieldFlags2 & 0x800)) {
        return;
    }

    int32_t powerType = this->m_unit->m_unitFieldBytes0[3];
    this->DoPowerRegen(powerType, elapsed);

    if (this->m_unit->m_unitFieldBytes0[1] == 11) {
        if (powerType) {
            this->DoPowerRegen(0, elapsed);
        }

        if (powerType != 3) {
            this->DoPowerRegen(3, elapsed);
        }

        this->DoPowerRegen(8, elapsed);
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && powerType == 6) {
        uint32_t runes = CGRuneInfo::m_predictedRunes;
        uint32_t previous = CGRuneInfo::m_predictedRunes;
        uint32_t now = FrameTime::GetCurTimeMs();

        for (int32_t i = 0; i < 6; i++) {
            uint32_t bit = 1 << i;

            if (!(bit & CGRuneInfo::m_predictedRunes) && static_cast<int32_t>(now - CGRuneInfo::GetRuneRechargeTime(i)) >= 0) {
                runes |= bit;
            }
        }

        if (runes != previous) {
            CGRuneInfo::SetPredictedRunes(previous, runes, NULL);
        }
    } else if (powerType == 10) {
        return;
    }

    this->DoPowerRegen(10, elapsed);
}

// OFFSET: #0x0072E3A0
void CGUnit_C::OnLevelChange() {
    CEffect* effect = CEffect::Create();
    WOWGUID guid = this->m_obj->m_guid;
    effect->AddSpecialEffect(5, guid, CGObject_C::EffectOnceHandler);
    effect->m_flags |= 0x20;
    effect->Release();

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        static_cast<CGPlayer_C*>(this)->OnLevelChange();
    } else {
        this->UpdateModelScale(NULL);
    }

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateLevelDisplay(this);
    }
}

// PROVEN: 484aaec5 IDENTICAL/NONE
// OFFSET: 0x005B9C90
void CGUnit_C::HandleBloodPool(uint32_t time) {
    if (!this->m_bloodlevels) {
        return;
    }

    if (!g_unitBloodDB.GetRecord(this->m_bloodlevels->m_violencelevel[ViolenceGetLevel()])) {
        return;
    }

    if (static_cast<int32_t>(time - this->m_nextBloodPoolTime) < 0) {
        return;
    }

    this->m_nextBloodPoolTime = static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 3000) >> 32) + time + 2000;
    CRandom::sfloat_(g_rndSeed);
    CRandom::sfloat_(g_rndSeed);
}

// PROVEN: 2741e209 IDENTICAL/NONE
// OFFSET: 0x005AC520
void CGUnit_C::ReadLootList(CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    this->m_masterLooter = guid;
    *msg >> guid;
    this->m_looter = guid;
}

// PROVEN: bffb38ee IDENTICAL/NONE
// OFFSET: 0x005AD470
int32_t Packet_SMSG_LOOT_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->ReadLootList(msg);
    } else {
        msg->m_read = msg->m_size;
    }

    return 1;
}

// PROVEN: 49e9ada2 IDENTICAL/NONE
// OFFSET: 0x005AE9C0
void CGUnit_C::RemoveLootEffect() {
    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_flags & 0x8) {
            effect->MoveToDestroyList();
            return;
        }
    }
}

// OFFSET: 0x005AFF20
void CGUnit_C::OnLife(bool a1) {
    this->m_clientFlags &= ~0x1u;
    this->CheckLoopSound();
    this->m_lootableTime = 0;
    PlayerNameTriggerColorUpdate(this->m_nameDesc);

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateColorDisplay(this);
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !a1) {
        SendQuestGiverStatusMultipleQuery();
        CGGameUI::UnlockAllItems();
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ALIVE, NULL);
    }

    this->UpdateDeathAnimKit();
}

// OFFSET: 0x005CB300
void CGUnit_C::OnDynamicFlagsChanged(uint32_t oldFlags) {
    uint32_t flags = this->m_unit->m_unitDynamicFlags;

    if ((oldFlags & 1) && !(flags & 1)) {
        this->RemoveLootEffect();
    }

    if ((oldFlags ^ this->m_unit->m_unitDynamicFlags) & 0x20) {
        Script_SendUnitSignal(this->m_obj->m_guid, 18);
        Script_SendUnitSignal(this->m_obj->m_guid, 20);
        Script_SendUnitSignal(this->m_obj->m_guid, 231, "%s", Spell_C_GetPowerTypeToken(this->m_unit->m_unitFieldBytes0[3]));
        Script_SendUnitSignal(this->m_obj->m_guid, 232, "%s", Spell_C_GetPowerTypeToken(this->m_unit->m_unitFieldBytes0[3]));

        if (this->m_unit->m_unitDynamicFlags & 0x20) {
            this->OnDeath();
        } else {
            this->OnLife(true);
        }
    }

    if (static_cast<int32_t>(this->m_unit->m_unitFieldHealth) <= 0 && (this->m_unit->m_unitDynamicFlags & 1)) {
        if (this->m_lootableTime) {
            this->ClearEffectList(0);
        }

        this->ShowLootEffect();
    }

    if ((oldFlags ^ this->m_unit->m_unitDynamicFlags) & 4) {
        Script_SendUnitSignal(this->m_obj->m_guid, 41);
    }
}

// OFFSET: 0x005B0650
static int32_t PetColorUpdateProc(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    const WOWGUID& owner = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldCreatedby;

    if (owner == static_cast<CGUnit_C*>(param)->m_obj->m_guid && !(unit->m_unit->m_unitFieldFlags & 0x100)) {
        PlayerNameTriggerColorUpdate(unit->m_nameDesc);

        if (unit->m_namePlateFrame) {
            unit->m_namePlateFrame->UpdateColorDisplay(unit);
        }

        Script_SendUnitSignal(unit->m_obj->m_guid, 49);
    }

    return 1;
}

// PROVEN: 52269791 IDENTICAL/NONE
// OFFSET: 0x005B0700/0x140223640
void CGUnit_C::UpdatePetReaction() {
    ClntObjMgrEnumVisibleObjects(PetColorUpdateProc, this);
}

// PROVEN: 01c64cd9 IDENTICAL/NONE
// OFFSET: 0x005AE970/0x1402212B0
void CGUnit_C::ShowLootEffect() {
    if (!(this->m_unit->m_unitDynamicFlags & 1)) {
        return;
    }

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_flags & 0x8) {
            return;
        }
    }

    CEffect* effect = CEffect::Create();
    effect->AddLootEffect(this, NULL);
    effect->Release();
}

// OFFSET: 0x005D8680
void CGUnit_C::CompleteMountTransition() {
    if (this->m_mountTransitionEffect) {
        this->OnMountDisplayChanged(this->m_mountTransitionEffect->m_displayId);
    }

    this->m_mountTransitionEffect = NULL;
    this->m_mountTransition = NULL;
}

// OFFSET: #0x00723730
void CGUnit_C::RefreshComponentItem(int32_t itemSlot) {
    if (this->CanEquipItemsInThisForm() && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
        if (!this->GetBag()) {
            return;
        }

        INVENTORY_SLOTS invSlot = CCharacterComponent::GetInvSlotForItemSlot(static_cast<ITEM_SLOT>(itemSlot));

        if (invSlot == -1) {
            return;
        }

        this->m_characterComponent->RemoveItem(static_cast<ITEM_SLOT>(itemSlot));
        const VisibleItem_C* item = static_cast<CGPlayer_C*>(this)->GetVisibleItem(invSlot);

        if (item && abs(item->m_itemId)) {
            static_cast<CGPlayer_C*>(this)->EquipVisibleItem(item, invSlot);
        }
    } else if (this->m_characterComponent && this->m_displayInfoExtra) {
        this->m_characterComponent->RemoveItem(static_cast<ITEM_SLOT>(itemSlot));
        int32_t displayId = this->m_displayInfoExtra->m_npcitemDisplay[itemSlot];

        if (displayId) {
            this->m_characterComponent->AddItem(static_cast<ITEM_SLOT>(itemSlot), displayId, 0);
        }
    }
}

// OFFSET: 0x005CCE80
void CGUnit_C::ClearCastingSpell(int32_t spellId, bool clearChannel, bool a4) {
    if (spellId != this->m_castSpellId) {
        return;
    }

    const SpellRec* castSpell = g_spellDB.GetRecord(spellId);
    this->m_castCount = 0;
    this->m_castSpellId = 0;

    if ((!castSpell || !(castSpell->m_attributesEx & 0x4)) && !this->m_unit->m_unitChannelSpell) {
        this->m_castSchoolImmunities = 0;
        this->m_castImmunities = 0;
    }

    this->m_animationState &= ~0x10u;
    this->UpdatePrecastAnimKit(0, WOWGUID());

    if (!a4) {
        return;
    }

    if (clearChannel) {
        if (this->IsAutoTracking() && !CGUnit_C::s_trackingType) {
            CGObject_C* target = ClntObjMgrObjectPtr(CGUnit_C::s_trackingGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (target) {
                C3Vector targetPosition = target->GetPosition();
                C3Vector position = this->GetPosition();
                float facing = CalculateFacingTo(&position, &targetPosition);
                C3Vector none = C3Vector();
                WOWGUID guid = WOWGUID();
                static_cast<CGPlayer_C*>(this)->ClickToMove(2, &guid, &none, facing);
            }
        }

        const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

        if (spell && (spell->m_attributesEx & 0x4000) && this->IsActiveMoverTracking(2)) {
            this->ClearTrackingTarget(false, true);
        }
    }

    this->m_chainSpellTargets.SetCount(0);

    if (spellId != Spell_C_GetAutoRepeatingSpell()) {
        this->CancelRangedMode();
    }

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = NULL;
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGGameUI::CloseLoot(true, (this->m_movement->m_flags & 0x60080F) != 0, false);
    }
}

// OFFSET: 0x005CCA40
void CGUnit_C::ReleaseMissiles(const C3Vector& position) {
    if (this->m_missiles) {
        this->CancelRangedMode();
    }

    while (this->m_missiles) {
        CM2Model* model = this->m_missiles->m_model;

        if (model) {
            if (!model->IsLoaded()) {
                this->m_clientFlags |= 0x800;
                break;
            }

            this->m_missiles->m_model->m_fadeAlpha = 1.0f;
        }

        if (this->IsMissileTrajectoryUpdateWaitingForMissileRelease() && this->m_missileTrajectoryReleased == this->m_missiles->m_castID) {
            this->SendFinalMissileTrajectoryUpdate();
        }

        PendingSpellCast* cast = Spell_C_GetDelayedTrajectoryCast();

        if (cast && this->m_missiles->m_castID == cast->m_data.m_castCount) {
            if (!Spell_C_SendDelayedMissileTrajectoryCast(this)) {
                if (!this->m_missiles) {
                    break;
                }

                if (this->m_precastMissileModel == this->m_missiles->m_model) {
                    if (this->m_precastMissileModel) {
                        if (this->m_precastMissileModel->m_attachParent) {
                            this->m_precastMissileModel->DetachFromParent();
                        }

                        this->m_precastMissileModel->Release();
                        this->m_precastMissileModel = NULL;
                        this->m_precastCastCount = 0;
                    }

                    this->m_clientFlags &= ~0x1000000u;
                }

                this->m_missiles->DeleteSelf();
                continue;
            }

            if (!this->m_missiles) {
                break;
            }
        }

        CM2Model* precast = this->m_precastMissileModel;

        if (precast && precast == this->m_missiles->m_model) {
            precast->Animate();
            C44Matrix transform = precast->m_matrixF4 * precast->m_scene->m_viewInv;

            if (this->m_precastMissileModel) {
                if (this->m_precastMissileModel->m_attachParent) {
                    this->m_precastMissileModel->DetachFromParent();
                }

                this->m_precastMissileModel->Release();
                this->m_precastMissileModel = NULL;
                this->m_precastCastCount = 0;
            }

            this->m_clientFlags &= ~0x1000000u;
            this->m_missiles->m_flags |= 0x2000000;
            CM2Model* missileModel = this->m_missiles->m_model;
            missileModel->m_fFlags |= 0x4000;
            missileModel->m_worldTransform = transform;
            C3Vector firePosition(transform.d0, transform.d1, transform.d2);
            this->m_missiles->FireMissiles(&firePosition);
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(this->m_missiles->m_spellID);
        C3Vector overridePosition = C3Vector();

        if (spell && this->OverrideMissileFirePos(&overridePosition, spell)) {
            this->m_missiles->FireMissiles(&overridePosition);
        } else {
            this->m_missiles->FireMissiles(&position);
        }
    }

    if (this->m_precastMissileModel) {
        if (this->m_precastMissileModel->m_attachParent) {
            this->m_precastMissileModel->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = NULL;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000u;
}

// OFFSET: 0x005D4FD0
void CGUnit_C::OnChannelObjectChanged(WOWGUID oldObject, int32_t oldSpell) {
    CGObject_C* object = ClntObjMgrObjectPtr(oldObject, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && oldSpell) {
        object->ClearSpellEffect(oldSpell, 1, this->m_obj->m_guid.m_guid);
    }

    if (this->m_fishingLineEffect) {
        this->m_fishingLineEffect->Release();
        this->m_fishingLineEffect = NULL;
    }

    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(this->m_unit->m_unitFieldChannelObject, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (gameObject && this->m_unit->m_unitChannelSpell && gameObject->m_gameObject->m_typeID == 17) {
        if (this->m_sheatheState == 1) {
            this->CreateFishingLineEffect();
        } else {
            this->SetSheatheState(1, 1, 0);
        }

        return;
    }

    if (!ClntObjMgrObjectPtr(this->m_unit->m_unitFieldChannelObject, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(this->m_unit->m_unitChannelSpell);

    if (spell && (spell->m_attributesEx & 0x4000) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        WOWGUID target = this->m_unit->m_unitFieldChannelObject;
        this->SetTrackingTarget(target, 1);
    }
}

// OFFSET: 0x005B8D50/0x14022CBE0
bool CGUnit_C::CanInteract(const CGGameObject_C* gameObject) const {
    return (!(this->m_obj->m_type & TYPEMASK_PLAYER) || !(static_cast<const CGPlayer_C*>(this)->m_player->m_playerFlags & 0x10)) && gameObject->ObjectReaction(this) >= 3;
}

// OFFSET: 0x005B05A0
static int32_t UnitColorUpdateProc(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return 1;
    }

    const WOWGUID& owner = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldCreatedby;

    if (owner != ClntObjMgrGetActivePlayer()) {
        PlayerNameTriggerColorUpdate(unit->m_nameDesc);

        if (unit->m_namePlateFrame) {
            unit->m_namePlateFrame->UpdateColorDisplay(unit);
        }

        Script_SendUnitSignal(unit->m_obj->m_guid, 49);
    }

    return 1;
}

// OFFSET: 0x005B8D90/0x14022CC30
void CGUnit_C::UpdateReaction(bool a1) {
    PlayerNameTriggerColorUpdate(this->m_nameDesc);

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateColorDisplay(this);
    }

    Script_SendUnitSignal(this->m_obj->m_guid, 49);

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        SendQuestGiverStatusMultipleQuery();
        ClntObjMgrEnumVisibleObjects(UnitColorUpdateProc, NULL);
        return;
    }

    if (a1) {
        ClntObjMgrEnumVisibleObjects(PetColorUpdateProc, this);
    }

    this->RefreshInteractIcon();
}

// OFFSET: 0x005C5670
void CGUnit_C::OnFlagChangedActivePlayer(uint32_t oldFlags, uint32_t changed) {
    if (changed & 0x40000) {
        uint32_t time = FrameTime::GetCurTimeMs();
        CGInputControl::GetActive()->UpdatePlayer(time, 1);
    }

    if (changed & 0x1C20000) {
        CGSpellBook::UpdateUsable();
    }

    if (changed & 0x80000) {
        CGSpellBook::UpdateUsable();
        CGGameUI::SetRegenEnabled(!(this->m_unit->m_unitFieldFlags & 0x80000));
    }

    if (changed & 0x100000) {
        if (this->m_unit->m_unitFieldFlags & 0x100000) {
            this->ClearTrackingTarget(false, true);
        } else if (!CGGameUI::m_inCinematic) {
            CGChat::ZoneUpdate();
        }
    }
}

// OFFSET: 0x005D59D0/0x1402502F0
void CGUnit_C::AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (std::isnan(static_cast<double>(position->x)) || std::isnan(static_cast<double>(position->y)) || std::isnan(static_cast<double>(position->z))) {
        unit->GetUnitName(NULL, 1);
    }

    unit->HandleAnimEvent(boneId, eventId, data, position, time);
}

// OFFSET: 0x005D3E00/0x14024E060
void CGUnit_C::HandleAnimEvent(uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time) {
    SE3SoundKitProperties properties;
    properties.m_conditionArg = this;
    properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;

    switch (eventId) {
        case 0x48414324:
        case 0x48544424:
        case 0x50504324:
        case 0x50574224:
        case 0x53534324:
        case 0x30484124:
        case 0x31484124:
        case 0x32484124:
        case 0x33484124:
            this->HandleCombatAnimEvent(eventId, data, position, time);
            return;

        case 0x31444624:
        case 0x32444624:
        case 0x33444624:
        case 0x34444624:
        case 0x35444624:
        case 0x36444624:
        case 0x37444624:
        case 0x38444624:
        case 0x39444624:
        case 0x58444624:
            this->HandlePlayStandSound(eventId, data);
            return;

        case 0x4C534324:
        case 0x52534324:
        case 0x54534324:
            if (this->m_missiles) {
                this->SetAnimEventPosition(position);
            }
            return;

        case 0x304C4224:
        case 0x304C4624:
        case 0x304C5224:
        case 0x304C5324:
        case 0x304C5724:
        case 0x314C4224:
        case 0x314C4624:
        case 0x314C5224:
        case 0x314C5324:
        case 0x314C5724:
        case 0x324C4224:
        case 0x324C4624:
        case 0x324C5224:
        case 0x324C5324:
        case 0x324C5724:
        case 0x334C4224:
        case 0x334C4624:
        case 0x334C5224:
        case 0x334C5324:
        case 0x334C5724:
            this->FootstepAnimEventHit(position, 1);
            return;

        case 0x30524224:
        case 0x30524624:
        case 0x30525224:
        case 0x30525324:
        case 0x30525724:
        case 0x31524224:
        case 0x31524624:
        case 0x31525224:
        case 0x31525324:
        case 0x31525724:
        case 0x32524224:
        case 0x32524624:
        case 0x32525224:
        case 0x32525324:
        case 0x32525724:
        case 0x33524224:
        case 0x33524624:
        case 0x33525224:
        case 0x33525324:
        case 0x33525724:
            this->FootstepAnimEventHit(position, 0);
            return;

        case 0x30475624:
        case 0x31475624:
        case 0x32475624:
        case 0x33475624:
        case 0x34475624:
        case 0x35475624:
        case 0x36475624:
        case 0x37475624:
        case 0x38475624:
            if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
                this->m_vehicle->HandleBoneAnimGrabEvent(boneId, eventId);
            }
            return;

        case 0x30545624:
        case 0x31545624:
        case 0x32545624:
        case 0x33545624:
        case 0x34545624:
        case 0x35545624:
        case 0x36545624:
        case 0x37545624:
        case 0x38545624:
            if (this->GetVehicleRec()) {
                this->m_vehicle->HandleBoneAnimThrowEvent(boneId, eventId);
            }
            return;

        case 0x44435324:
            if (this->m_soundData) {
                SE3::PlaySoundKit(this->m_soundData->m_spellCastDirectedSoundID, position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleAnimEvent", 0, NULL);
            }
            return;

        case 0x4C485324:
            this->ExchangeSheathedWeapon(position, 1, 0x200000);
            return;

        case 0x52485324:
            this->ExchangeSheathedWeapon(position, 0, 0x100000);
            return;

        case 0x48544224: {
            if (ClntObjMgrGetPlayerType() == 1 || !this->m_modelData || (this->m_modelData->m_flags & 2)) {
                return;
            }

            int32_t type;

            if ((this->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(this)->GetDrunknessTurnValue() >= 0.5f) {
                type = 7;
            } else if (this->m_clientFlags & 0x20) {
                type = 2;
            } else if (this->m_clientFlags & 0x40) {
                type = 3;
            } else {
                return;
            }

            CEffect* effect = CEffect::Create();
            WOWGUID guid = this->m_obj->m_guid;
            effect->AddSpecialEffect(type, guid, CGObject_C::EffectOnceHandler);
            effect->m_flags |= 0x20;
            effect->Release();
            return;
        }

        case 0x474D5324:
            if (this->m_soundData) {
                SE3::PlaySoundKit(this->m_soundData->m_submergeSoundID, position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleAnimEvent", 0, NULL);
            }
            return;

        case 0x444D5324:
            if (this->m_soundData) {
                SE3::PlaySoundKit(this->m_soundData->m_submergedSoundID, position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleAnimEvent", 0, NULL);
            }
            return;

        case 0x474E5724:
            this->PlayUnitSound(7, 0, 0);
            return;

        case 0x47475724:
            this->PlayUnitSound(10, 0, 0);
            return;

        case 0x44534524:
            this->PlayEmoteStateSound(position);
            return;

        case 0x44534624:
            this->HandleFootfallAnimEvent(*position);
            return;

        case 0x44534324:
            this->PlayEmoteSound(data, true, -1, 0);
            return;

        case 0x44525424:
            if (this->m_obj->m_type & TYPEMASK_PLAYER) {
                static_cast<CGPlayer_C*>(this)->HandleSpellEventSound();
            }
            return;

        case 0x54524224:
            if (this->m_soundData) {
                SE3::PlaySoundKit(this->m_soundData->m_birthSoundID, position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleAnimEvent", 0, NULL);
            }
            return;

        case 0x52574224: {
            if (!CGUnit_C::s_trackingType) {
                this->ClearTrackingTarget(false, true);
            }

            if (this->m_animationState & 0x4000) {
                this->m_animationState &= ~0x4000u;
            }

            if (this->m_missiles) {
                C3Vector eventPosition;

                if (IsBowAnim(this->GetCurrentTorsoAnimId()) && this->m_worldModel->HasAttachment(35)) {
                    eventPosition = this->m_worldModel->GetAttachmentPosition(35);

                    if (this->m_rangedWeaponModel) {
                        this->m_rangedWeaponModel->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
                    }
                } else if (IsRifleAnim(this->GetCurrentTorsoAnimId()) && this->m_worldModel->HasAttachment(36)) {
                    eventPosition = this->m_worldModel->GetAttachmentPosition(36);

                    if (this->m_rangedWeaponModel) {
                        this->m_rangedWeaponModel->SetBoneSequence(0xFFFFFFFF, 161, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
                    }
                } else {
                    eventPosition = *position;
                }

                this->SetAnimEventPosition(&eventPosition);
            }

            if (this->m_ammoModel && this->m_ammoModel->m_attachParent) {
                this->m_ammoModel->DetachFromParent();
            }

            return;
        }

        default:
            return;
    }
}

// OFFSET: #0x0073B050
void CGUnit_C::PlayParryAnim() {
    if (this->AppearsDead(1) || !this->m_worldModel->IsLoaded() || this->DoNotPlayWoundAnim()) {
        return;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(0, false);

    if (!item || item->m_classID != 2 || !this->m_unit->m_unitFieldBytes2[0]) {
        this->PlayBaseAnimation(ANIM_PARRY_UNARMED, 0);
        return;
    }

    uint32_t twoHandAnim = ANIM_PARRY_2H - (this->GetVirtualItem(1, false) != NULL);

    switch (item->m_subclassID) {
        case 0:
        case 4:
        case 7:
        case 11:
        case 14:
        case 15:
        case 20:
            this->PlayBaseAnimation(ANIM_PARRY_1H, 0);
            break;

        case 1:
        case 5:
        case 8:
        case 12:
            this->PlayBaseAnimation(twoHandAnim, 0);
            break;

        case 6:
        case 10:
        case 17:
            this->PlayBaseAnimation(ANIM_PARRY_2HL, 0);
            break;

        case 13:
            this->PlayBaseAnimation(ANIM_PARRY_UNARMED, 0);
            break;

        default:
            break;
    }
}

// OFFSET: 0x005D12C0/0x14024AB30
void CGUnit_C::CreateMissile(const CreateMissileParams& params) {
    float scale = 1.0f;
    float alpha = 1.0f;
    CM2Model* model = this->m_precastMissileModel;

    if (model && !(this->m_clientFlags & 0x1000000)) {
        model->m_refCount++;
        this->m_clientFlags |= 0x1000000;
        const SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(params.m_visual->m_missileModel);
        model = this->m_precastMissileModel;

        if (effectName && *effectName->m_fileName) {
            scale = effectName->m_scale;
        }
    } else if (params.m_visual->m_missileModel) {
        model = CEffect::CreateModelFromEffect(params.m_visual->m_missileModel, &scale, &alpha, this->m_obj->m_guid, params.m_ammoDisplayId, params.m_ammoInventoryType);
    } else {
        ItemDisplayInfoRec displayRec;
        model = NULL;

        if (params.m_ammoDisplayId && g_itemDisplayInfoDB.GetRecord(params.m_ammoDisplayId, &displayRec)) {
            model = CCharacterComponent::MakeAmmoModel(this->m_worldModel, &displayRec, params.m_ammoInventoryType);
        }
    }

    if (!model) {
        model = World::GetScene()->CreateModel("Spells\\ErrorCube.mdx", 0, 3);
    }

    if (AnimationDataHasFlags(this->GetCurrentTorsoAnimId(), 0x40000)) {
        model->m_fadeAlpha = 0.0f;
    }

    int32_t pointMode = params.m_flags & 1;
    CMissile* missile = CMissile::NewMissile();
    missile->m_model = model;
    missile->m_casterGuid = this->m_obj->m_guid;
    missile->m_sourceGuid = this->m_obj->m_guid;
    missile->m_targetGuid = pointMode ? WOWGUID() : params.m_target;
    missile->m_spellID = params.m_spell ? params.m_spell->m_id : 0;
    missile->m_spellVisualID = params.m_visual->m_id;
    missile->m_castID = params.m_data ? params.m_data->m_castCount : 0;
    float speed = params.m_spell ? params.m_spell->m_speed : params.m_speed;
    missile->m_speed = speed;
    missile->m_baseSpeed = speed;
    missile->m_launchSpeed = speed;
    missile->m_hitResult = pointMode ? 0 : params.m_missReason;
    missile->m_missReason = pointMode ? 0 : params.m_reflectResult;

    if (pointMode || params.m_hit) {
        missile->m_flags |= 1;
    }

    if (params.m_spell && (params.m_spell->m_attributes & 2)) {
        const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

        if (item && item->m_classID == 2 && (item->m_subclassID == 2 || item->m_subclassID == 18 || item->m_subclassID == 16)) {
            missile->m_flags |= 4;
        }
    }

    if (!pointMode && params.m_spell && params.m_missReason == 11 && (params.m_spell->m_attributesExG & 0x20000000)) {
        missile->m_flags |= 0x8000000;
    }

    missile->m_soundKitID = params.m_visual->m_missileSound;
    missile->m_scale = scale;
    missile->m_alpha = alpha;
    const SpellVisualRec* spellVisual = params.m_spell ? GetSpellVisual(params.m_spell, this) : params.m_visual;

    if (spellVisual) {
        float height = spellVisual->m_missileFollowGroundHeight * 0.001f;

        if (height <= 0.25f) {
            height = 0.25f;
        }

        missile->m_groundOffset = height;
        float rate = spellVisual->m_missileFollowGroundDropSpeed * 0.001f;

        if (rate <= 0.0099999998f) {
            rate = 0.0099999998f;
        }

        missile->m_followGroundRate = rate;
        float approach = spellVisual->m_missileFollowGroundApproach * 0.001f;

        if (approach < 0.0f) {
            approach = 0.0f;
        } else if (approach >= 1.0f) {
            approach = 1.0f;
        }

        missile->m_followGroundStart = approach;
        missile->m_motionFlags = spellVisual->m_missileFollowGroundFlags;

        if (spellVisual->m_missileFollowGroundFlags & 1) {
            missile->m_flags |= 0x7800;
        }
    }

    const SpellVisualRec* visual = params.m_visual;
    int32_t attachment = (visual->m_flags & 0x200) ? visual->m_missileDestinationAttachment : s_effectAttachmentIds[visual->m_missileDestinationAttachment];
    C3Vector offset(visual->m_missileImpactOffset.x, -visual->m_missileImpactOffset.y, visual->m_missileImpactOffset.z);
    int32_t hasOffset = 0.0f != offset.x || 0.0f != offset.y || 0.0f != offset.z;

    if (pointMode) {
        if (params.m_points) {
            const SpellCastTargetPoint* point = params.m_points->m_data;
            missile->m_impactPos = point->m_position;
            missile->unk_0074 = point->unk_0024;
            missile->m_transportGuid = point->m_transport;
            missile->m_flags |= 1;
            missile->m_attachmentID = attachment;
            missile->m_attachOffset = offset;
            missile->m_targetPoints.Set(params.m_points->m_count, params.m_points->m_data);
            missile->m_flags |= 0x200;
            goto flagsDone;
        }

        const SpellCastTargetPoint* point = params.m_point;
        missile->m_impactPos = point->m_position;
        missile->unk_0074 = point->unk_0024;
        missile->m_transportGuid = point->m_transport;
    } else {
        CGObject_C* object;

        if (params.m_target) {
            missile->m_attachmentID = attachment;
            missile->m_attachOffset = offset;
            object = ClntObjMgrObjectPtr(params.m_target, TYPEMASK_OBJECT, __FILE__, __LINE__);
        } else {
            PendingSpellCastData* data = params.m_data;
            uint32_t targetFlags = data ? data->m_targetFlags : 0;

            if (data && (targetFlags & 0x60) == 0x60) {
                missile->m_position = data->m_sourcePos;
                missile->m_impactPos = data->m_destPos;
                missile->m_transportGuid = data->m_destTarget;
                goto locationTarget;
            }

            if (data && (targetFlags & 0x40)) {
                missile->m_impactPos = data->m_destPos;
                missile->m_transportGuid = data->m_destTarget;
                goto locationTarget;
            }

            if (data && (targetFlags & 0x20)) {
                missile->m_impactPos = data->m_sourcePos;
                missile->m_transportGuid = data->m_sourceTarget;
                goto locationTarget;
            }

            if (!params.m_targets->m_count) {
                missile->DeleteSelf();
                return;
            }

            missile->m_targetGuid = params.m_targets->m_data[0];
            missile->m_attachmentID = attachment;
            missile->m_attachOffset = offset;
            object = ClntObjMgrObjectPtr(params.m_targets->m_data[0], TYPEMASK_OBJECT, __FILE__, __LINE__);
        }

        if (object) {
            missile->m_impactPos = object->GetPosition();
        }

        goto unitTarget;
    }

locationTarget:
    missile->m_flags |= 1;
    missile->m_attachmentID = -1;

    if (hasOffset) {
        C44Matrix matrix = this->m_vehiclePassenger ? this->GetModelMatrix() : this->GetMatrix();
        float x = matrix.c0 * offset.z + matrix.b0 * offset.y + matrix.a0 * offset.x;
        float y = matrix.c1 * offset.z + matrix.a1 * offset.x + matrix.b1 * offset.y;
        float z = matrix.c2 * offset.z + matrix.a2 * offset.x + matrix.b2 * offset.y;
        missile->m_impactPos.x = missile->m_impactPos.x + x;
        missile->m_impactPos.y = y + missile->m_impactPos.y;
        missile->m_impactPos.z = z + missile->m_impactPos.z;
    }

    if (pointMode) {
        missile->m_flags &= ~0x200u;
        goto flagsDone;
    }

unitTarget:
    {
        int32_t multiTarget = params.m_spell && (params.m_spell->m_attributesExD & 0x40000);
        int32_t areaTarget = params.m_data && (params.m_data->m_targetFlags & 0x60);

        if (!params.m_target && params.m_targets->m_count) {
            if (!multiTarget && !areaTarget) {
                missile->m_flags &= ~0x200u;
                goto flagsDone;
            }

            missile->m_targets.Set(params.m_targets->m_count, reinterpret_cast<const uint64_t*>(params.m_targets->m_data));

            if (params.m_missReasons && params.m_missReasons->m_count) {
                missile->m_targetHitResults.Set(params.m_missReasons->m_count, params.m_missReasons->m_data);
            }

            if (params.m_reflectResults && params.m_reflectResults->m_count) {
                missile->m_targetMissReasons.Set(params.m_reflectResults->m_count, params.m_reflectResults->m_data);
            }
        }

        if (multiTarget) {
            missile->m_flags |= 0x200;
        } else {
            missile->m_flags &= ~0x200u;
        }
    }

flagsDone:
    missile->m_currentTarget = 0;
    int32_t missileCount = 1;

    if (spellVisual && spellVisual->m_missileMotion) {
        missile->m_flags |= 0x3800;
        missile->InstallMotionScriptFunction(spellVisual->m_missileMotion);
        const SpellMissileMotionRec* motion = g_spellMissileMotionDB.GetRecord(spellVisual->m_missileMotion);

        if (motion) {
            missileCount = 1;

            if (motion->m_missileCount >= 1) {
                missileCount = motion->m_missileCount;
            }
        }
    }

    missile->m_castTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    missile->SetReadyToFire(&this->m_missiles);
    this->m_lastMissileTime = missile->m_castTime;
    PendingSpellCastData* data = params.m_data;

    if (data && (data->m_castFlags & 0x20000)) {
        missile->m_pitch = data->m_elevation;
        missile->m_trajectorySpeed = data->unk_0120;
        missile->m_flags |= 0x10800;
        missile->m_arrivalTime = missile->m_castTime + params.m_delay;
    }

    if ((this->m_clientFlags & 0x400) || (this->m_obj->m_guid == CGPetInfo::GetPet(0) && !this->m_unit->m_unitFieldCharmedby)) {
        missile->m_flags |= 0x200000;
    }

    missile->m_missileCount = missileCount;

    if (missileCount > 1) {
        missile->m_missileIndex = 0;

        for (int32_t i = 1; i < missileCount; i++) {
            missile = CMissile::CopyMissile(missile);
            missile->m_missileIndex = i;
            missile->SetReadyToFire(&this->m_missiles);
        }
    }

    if (params.m_visual->m_flags & 4) {
        C3Vector position = this->GetMissileFirePosition();
        this->ReleaseMissiles(position);
    }
}

// PROVEN: 0c268722 IDENTICAL/NONE
// OFFSET: 0x005ABB90/0x14021E0B0
void CGUnit_C::ChatBubbleRemove() {
    if (this->m_chatBubble) {
        CGChatBubbleFrame::Remove(this->m_chatBubble);
        this->m_chatBubble = NULL;
    }
}

// OFFSET: 0x005B9800/0x14022D7D0
void CGUnit_C::ChatBubbleShow(int32_t type, const char* text) {
    if (!text || !*text || !this->m_worldObject) {
        return;
    }

    if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[1] & 0x8)) {
        return;
    }

    switch (type) {
        case 1:
        case 2:
        case 6:
        case 12:
        case 14:
        case 51: {
            CVar* cvar = (type == 2 || type == 51) ? g_chatBubblesPartyCVar : g_chatBubblesCVar;

            if (!cvar->m_intValue && !StaticSingleton<CCameraManager>::Instance()->m_legacyActive) {
                return;
            }

            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (!player) {
                return;
            }

            if (this->m_obj->m_type & TYPEMASK_PLAYER) {
                C3Vector delta = player->GetPosition() - this->GetPosition();

                if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x > 625.0f) {
                    return;
                }
            }

            CImVector color = CGChat::GetChatColor(type);
            this->ChatBubbleRemove();
            this->m_chatBubble = CGChatBubbleFrame::GetNewChatBubble(CGWorldFrame::s_currentWorldFrame);
            WOWGUID guid = this->m_obj->m_guid;
            this->m_chatBubble->Initialize(guid, text, color);
            break;
        }

        default:
            break;
    }
}

// OFFSET: #0x0071ACF0
bool CGUnit_C::IsTracked() const {
    return CGMinimapFrame::s_trackingInfo && CGMinimapFrame::s_trackingInfo->m_type == 1 && (this->m_unit->m_unitNpcFlags & CGMinimapFrame::s_trackingInfo->m_npcFlags);
}

// OFFSET: #0x0071D450
int32_t IsUnarmedAnim(uint32_t animId) {
    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);
    int32_t behavior = rec ? rec->m_behaviorID : 506;

    switch (behavior) {
        case 16:
        case 20:
        case 25:
        case 117:
        case 118:
            return 1;

        default:
            return 0;
    }
}

// PROVEN: 581d65ff IDENTICAL/NONE
// OFFSET: 0x005ABC90
void CGUnit_C::ClearWeaponTrail(int32_t hand) {
    if (this->m_weaponTrails[hand]) {
        WeaponTrailClose(this->m_weaponTrails[hand]);
        this->m_weaponTrails[hand] = NULL;
    }
}

// OFFSET: #0x00721ED0
int32_t CGUnit_C::IsPlayingUnarmedAnim() {
    uint32_t sequence = 0xFFFFFFFF;

    if (this->m_worldModel && this->m_worldModel->IsLoaded()) {
        if (this->m_torsoKeyBone == -1 || (sequence = this->m_worldModel->GetBoneSequenceId(this->m_torsoKeyBone)) == 0xFFFFFFFF) {
            sequence = this->m_worldModel->GetBoneSequenceId(0xFFFFFFFF);
        }
    }

    return IsUnarmedAnim(sequence) || (IsMeleeAttackAnim(sequence) && !this->GetVirtualItem(0, false));
}

// OFFSET: 0x005CC660
void CGUnit_C::AddHandItem(int32_t hand) {
    if ((this->m_modelData->m_flags & 0x10) || !this->m_worldModel) {
        return;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(hand, false);

    if (!item) {
        return;
    }

    ItemDisplayInfoRec displayRec;

    if (!this->GetVirtualItemDisplayRec(hand, displayRec)) {
        return;
    }

    SHEATHE_TYPE sheatheType = static_cast<SHEATHE_TYPE>(item->m_sheatheType);
    bool sheathed;

    if (this->m_sheatheState == 2) {
        sheathed = hand != 2;
    } else if (hand == 1 && this->IsPlayingWeaponAnim()) {
        sheathed = CGUnit_C::ResolveSheathState(this->m_sheatheState, this->GetVirtualItem(0, false), item) == 0;
    } else {
        sheathed = this->m_sheatheState == 0;
    }

    bool shield = item->m_inventoryType == 14;
    bool ranged = item->m_inventoryType == 26 || item->m_inventoryType == 25;
    uint32_t attachId;
    INVENTORY_SLOTS invSlot;
    bool mainHand;

    if (hand == 0) {
        const CGUnitVirtualItem* mainItem;

        if ((this->m_unit->m_unitFieldFlags & 0x200000) && (mainItem = this->GetVirtualItem(0, true)) && mainItem->m_classID == 2) {
            return;
        }

        attachId = 1;
        invSlot = static_cast<INVENTORY_SLOTS>(15);
        mainHand = true;
    } else if (hand == 1) {
        const CGUnitVirtualItem* offItem;

        if (this->m_unit->m_unitFieldFlags & 0x200000) {
            this->GetVirtualItem(0, true);

            if (!this->IsDisarmed(0) && (offItem = this->GetVirtualItem(1, true)) && offItem->m_classID == 2) {
                return;
            }
        }

        if (this->m_unit->m_unitFieldFlags2 & 0x80) {
            return;
        }

        const CGUnitVirtualItem* mainItem;

        if (!(this->m_obj->m_type & TYPEMASK_PLAYER) && (mainItem = this->GetVirtualItem(0, false)) && mainItem->m_classID == 2) {
            uint8_t subclass = mainItem->m_subclassID;

            if (subclass == 1 || subclass == 5 || subclass == 8 || subclass == 10 || subclass == 6 || subclass == 20) {
                return;
            }
        }

        attachId = 2;
        invSlot = static_cast<INVENTORY_SLOTS>(16);
        mainHand = false;
    } else {
        if (this->m_sheatheState == 2) {
            if (!this->IsDisarmed(!ranged)) {
                this->ShowRangedWeapon(false, NULL);
            }
        } else if (ranged) {
            const CGUnitVirtualItem* offItem;

            if (this->m_sheatheState != 1 || !this->GetVirtualItem(1, false) || ((this->m_unit->m_unitFieldFlags & 0x200000) && (this->GetVirtualItem(0, true), !this->IsDisarmed(0)) && (offItem = this->GetVirtualItem(1, true)) && offItem->m_classID == 2) || (this->m_unit->m_unitFieldFlags2 & 0x80)) {
                this->RemoveHandItemModel(16, 0, false);
            }
        } else {
            const CGUnitVirtualItem* mainItem;

            if (this->m_sheatheState != 1 || !this->GetVirtualItem(0, false) || ((this->m_unit->m_unitFieldFlags & 0x200000) && (mainItem = this->GetVirtualItem(0, true)) && mainItem->m_classID == 2)) {
                this->RemoveHandItemModel(15, 0, false);
            }
        }

        return;
    }

    CM2Model* detached = NULL;

    if (this->m_sheatheState == 2) {
        for (CM2Model* attached = this->m_worldModel->m_attachList; attached; attached = attached->m_attachNext) {
            if (attached->m_attachmentId == attachId) {
                attached->m_refCount++;
                detached = attached;
                attached->DetachFromParent();
                break;
            }
        }
    }

    this->AddHandItemModel(&displayRec, invSlot, sheatheType, sheathed, shield, ranged, 0);

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        GEOCOMPONENTLINKS visualLink = sheathed ? CCharacterComponent::GetSheatheLink(static_cast<SHEATHE_TYPE>(item->m_sheatheType), mainHand) : static_cast<GEOCOMPONENTLINKS>(attachId);
        static_cast<CGPlayer_C*>(this)->UpdateHandItemVisual(visualLink, hand);
    }

    if (detached) {
        detached->AttachToParent(this->m_worldModel, attachId, NULL, 0);
        detached->Release();
    }
}

// PROVEN: 8e4b7e2e IDENTICAL/NONE
// OFFSET: 0x0044E430
CGUnit_C* CGUnit_C::GetVehicleUnit() const {
    return this->m_vehiclePassenger ? this->m_vehiclePassenger->GetVehicleUnit() : NULL;
}

// OFFSET: 0x005B4730/0x140227CD0
float CGUnit_C::GetMeleeChanceToCrit() const {
    float base = GetGameTableValue(3, 0, this->m_unit->m_unitFieldBytes0[1] - 1);
    float ratio = GetGameTableValue(2, this->m_unit->m_unitFieldLevel - 1, this->m_unit->m_unitFieldBytes0[1] - 1);

    if (ratio == 0.0f) {
        return 0.0f;
    }

    int32_t agility = this->m_unit->m_unitFieldStat[1] < 0 ? 0 : this->m_unit->m_unitFieldStat[1];
    return (ratio * agility + base) * 100.0f;
}

// OFFSET: 0x005B4580/0x140227B20
double CGUnit_C::GetHealthModifier() {
    if (this->m_unit->m_unitFieldPetnumber) {
        CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit->m_unitFieldCreatedby, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner && owner->m_unit->m_unitFieldBytes0[1] == 3) {
            return 1.0;
        }
    }

    WOWGUID none = WOWGUID();
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(this->m_obj->m_entryID, none, NULL, NULL, false);

    if (stats) {
        return stats->m_hpModifier;
    }

    return 0.0;
}

// OFFSET: 0x005B4620/0x140227BC0
double CGUnit_C::GetPowerModifier() {
    if (this->m_unit->m_unitFieldPetnumber) {
        CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit->m_unitFieldCreatedby, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner && owner->m_unit->m_unitFieldBytes0[1] == 3) {
            return 1.0;
        }
    }

    WOWGUID none = WOWGUID();
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(this->m_obj->m_entryID, none, NULL, NULL, false);

    if (stats) {
        return stats->m_mpModifier;
    }

    return 0.0;
}

// OFFSET: 0x005B46C0/0x140227C60
float CGUnit_C::GetManaRegenRateFromSpirit() const {
    int32_t spirit = this->m_unit->m_unitFieldStat[4] < 0 ? 0 : this->m_unit->m_unitFieldStat[4];
    int32_t intellect = this->m_unit->m_unitFieldStat[3] < 0 ? 0 : this->m_unit->m_unitFieldStat[3];
    return GetGameTableValue(10, this->m_unit->m_unitFieldLevel - 1, this->m_unit->m_unitFieldBytes0[1] - 1) * sqrt(static_cast<double>(intellect)) * spirit;
}

// OFFSET: #0x0071BA60
float CGUnit_C::GetHealthRegenRateFromSpirit() const {
    int32_t spirit = this->m_unit->m_unitFieldStat[4] < 0 ? 0 : this->m_unit->m_unitFieldStat[4];
    int32_t baseSpirit = spirit <= s_baseSpiritRegenCap ? spirit : s_baseSpiritRegenCap;
    int32_t classIndex = this->m_unit->m_unitFieldBytes0[1] - 1;
    int32_t levelIndex = this->m_unit->m_unitFieldLevel - 1;
    float baseRatio = GetGameTableValue(7, levelIndex, classIndex);
    float ratio = GetGameTableValue(9, levelIndex, classIndex);
    return ratio * (spirit - baseSpirit) + baseSpirit * baseRatio;
}

// PROVEN: 70afced5 IDENTICAL/NONE
// OFFSET: 0x005AB960/0x14021DE30
void CGUnit_C::UpdateUnitNameText() {
    if (this->m_nameDesc) {
        PlayerNameTriggerNameRegenerate(this->m_nameDesc);
    }

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateNameDisplay(this);
    }
}

// PROVEN: 527b14fd IDENTICAL/NONE
// OFFSET: 0x005ACF60/0x14021F430
void NameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->DestroyNamePlate();
    }

    CGGameUI::UnitNameUpdate(guid);

    if (!unit) {
        return;
    }

    PlayerNameTriggerNameRegenerate(unit->m_nameDesc);

    if (unit->m_unit->m_unitFieldCharm) {
        CGUnit_C* charm = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unit->m_unitFieldCharm, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (charm) {
            PlayerNameTriggerNameRegenerate(charm->m_nameDesc);
        }
    }

    TotemInfo* totem = CGGameUI::GetTotemInfo(guid);

    if (totem) {
        totem->m_name = unit->GetUnitName(NULL, 1);
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TOTEM_UPDATE, "%d", totem->m_slot + 1);
    }
}

// STATIC
// PROVEN: f5c49047 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
static void ObjectNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
}

// OFFSET: 0x005F3C10/0x1402768B0
const char* GetCombatLogObjectName(const WOWGUID& guid) {
    const char* name = NULL;
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid))) {
        NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, guid, ObjectNameCacheCallback, NULL, true);

        if (record) {
            record->FullName(s_objectNameBuffers[s_objectNameIndex], sizeof(s_objectNameBuffers[0]));
            name = s_objectNameBuffers[s_objectNameIndex++];

            if (s_objectNameIndex >= 2) {
                s_objectNameIndex = 0;
            }
        }
    } else {
        uint32_t type = high & 0xF0F00000;

        if (type == 0xF0300000 || type == 0xF0500000) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                name = unit->GetUnitName(NULL, 1);
            } else {
                CreatureStats_C* stats = g_creatureDBCache.GetRecord(static_cast<int32_t>(guid.m_guid >> 24) & 0xFFFFFFF, guid, ObjectNameCacheCallback, NULL, true);

                if (stats) {
                    name = stats->m_name[0];
                }
            }
        } else if (type == 0xF0400000) {
            PetNameCache* pet = g_petNameCache.GetRecord(static_cast<int32_t>(guid.m_guid >> 24) & 0xFFFFFFF, guid, ObjectNameCacheCallback, NULL, true);

            if (pet) {
                name = pet->m_name;
            }
        } else if (type == 0xF0100000) {
            GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(static_cast<int32_t>(guid.m_guid >> 24) & 0xFFFFFFF, guid, ObjectNameCacheCallback, NULL, true);

            if (stats) {
                name = stats->GetName(0);
            }
        }
    }

    if (!name || !*name) {
        name = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
    }

    if (!name || !*name) {
        return "UKNOWNOBJECT";
    }

    return name;
}

// OFFSET: #0x00715760
static float NamePlateScreenCenterDistanceSquared(C3Vector position) {
    static C3Vector s_screenCenter(0.4f, 0.3f, 0.0f);

    C3Vector delta(position.x - s_screenCenter.x, position.y - s_screenCenter.y, position.z - s_screenCenter.z);
    C2Vector delta2(delta);

    return delta2.x * delta2.x + delta2.y * delta2.y;
}

// OFFSET: 0x009E8FD0/0x140781DD0
void CGUnit_C::ClearNamePlateFocus() {
    if (CGUnit_C::s_namePlateFocus) {
        CGUnit_C::s_namePlateFocus->OnLoseFocus();
    }

    CGUnit_C::s_namePlateFocus = NULL;
}

// OFFSET: 0x005B1020
void CGUnit_C::NamePlateUpdateRaidTarget() {
    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateRaidTargetDisplay(GetRaidTargetIndexFromGuid(this->m_obj->m_guid));
    }
}

// OFFSET: 0x009E8FF0
void CGNamePlateFrame::ReleaseNameplateFrame(CGNamePlateFrame* frame) {
    if (frame) {
        s_unusedNamePlates.LinkToTail(frame);
    }
}

// OFFSET: #0x007254E0
CGNamePlateFrame* CGNamePlateFrame::GetNewNameplateFrame(CSimpleFrame* parent) {
    CGNamePlateFrame* frame = s_unusedNamePlates.Head();

    if (!frame) {
        void* data = CGNamePlateFrame::s_namePlateHeap.GetData(0, typeid(CGNamePlateFrame).raw_name(), SLOG_OBJECT);
        return data ? new (data) CGNamePlateFrame(parent) : NULL;
    }

    s_unusedNamePlates.UnlinkNode(frame);

    return frame;
}

// OFFSET: #0x00725530
static void NamePlateSort(CGNamePlateFrame* frame) {
    s_namePlates.UnlinkNode(frame);

    float distance = NamePlateScreenCenterDistanceSquared(frame->m_position);

    for (CGNamePlateFrame* node = s_namePlates.Head(); node; node = s_namePlates.Next(node)) {
        if (NamePlateScreenCenterDistanceSquared(node->m_position) <= distance) {
            s_namePlates.LinkNode(frame, 2, node);
            return;
        }
    }

    s_namePlates.LinkToTail(frame);
}

// OFFSET: 0x005BF090
void CGUnit_C::NamePlateAddOrUpdate(const C3Vector& position, CGWorldFrame* worldFrame) {
    if (!this->m_namePlateFrame) {
        this->m_namePlateFrame = CGNamePlateFrame::GetNewNameplateFrame(worldFrame);
        this->m_namePlateFrame->Initialize(this);
        this->m_namePlateFrame->Show();
        this->m_namePlateFrame->m_position = position;
        NamePlateSort(this->m_namePlateFrame);
        worldFrame->SetNamePlateUpdate();

        if (this->m_chatBubble) {
            CGChatBubbleFrame::Remove(this->m_chatBubble);
            this->m_chatBubble = NULL;
        }

        this->UpdateSpellCastBars();
        this->ObjectNameVisibilityChanged();
        return;
    }

    if (CGUnit_C::s_namePlateFlags & 0x2) {
        this->m_namePlateFrame->UpdateLevelDisplay(this);
    }

    if (CGUnit_C::s_namePlateFlags & 0x4) {
        this->m_namePlateFrame->UpdateColorDisplay(this);
    }

    C3Vector oldPosition = this->m_namePlateFrame->m_position;
    C3Vector delta(position.x - oldPosition.x, position.y - oldPosition.y, position.z - oldPosition.z);
    C2Vector delta2(delta);

    if ((CGUnit_C::s_namePlateFlags & 0x1) || delta2.y * delta2.y + delta2.x * delta2.x != 0.0f) {
        this->m_namePlateFrame->m_position = position;
        NamePlateSort(this->m_namePlateFrame);
        worldFrame->SetNamePlateUpdate();
    }
}

// OFFSET: #0x00725890
void CGUnit_C::UnitNameplatesReposition(CGWorldFrame* worldFrame) {
    CGNamePlateFrame* frame = s_namePlates.Head();

    while (frame) {
        CGNamePlateFrame* next = s_namePlates.Next(frame);

        if (s_cvNameplateAllowOverlap && s_cvNameplateAllowOverlap->m_intValue) {
            C3Vector position = frame->m_position;
            frame->SetPoint(FRAMEPOINT_TOP, worldFrame, FRAMEPOINT_BOTTOMLEFT, position.x, position.y, 1);
            frame->SetFrameDepth(position.z - worldFrame->m_layoutDepth, true);
        } else {
            C3Vector position = frame->m_position;
            SmartScreenRectPositionFrame(0, frame, worldFrame, position, 0);
        }

        frame = next;
    }

    CLayoutFrame::ResizePending();
}

// OFFSET: #0x00727130
void CGUnit_C::NamePlateClearAll() {
    CGNamePlateFrame* frame = s_namePlates.Head();

    while (frame) {
        CGNamePlateFrame* next = s_namePlates.Next(frame);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(frame->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_namePlateFrame) {
            if (CGUnit_C::s_namePlateFocus == unit->m_namePlateFrame) {
                CGUnit_C::s_namePlateFocus = NULL;
            }

            s_unusedNamePlates.LinkToTail(unit->m_namePlateFrame);
            unit->m_namePlateFrame->Hide();
            unit->m_namePlateFrame = NULL;
            unit->ObjectNameVisibilityChanged();
        }

        frame = next;
    }
}

// OFFSET: #0x007271D0
void CGUnit_C::SetNamePlateFocus(const C2Vector& position) {
    if (CGInputControl::GetActive()->m_flags & 0x6000003) {
        return;
    }

    CGNamePlateFrame* focus = NULL;

    for (CGNamePlateFrame* frame = s_namePlates.Head(); frame; frame = s_namePlates.Next(frame)) {
        if (frame->m_guid == CGGameUI::m_lockedTarget) {
            continue;
        }

        if (!frame->PtInFrameRect(position)) {
            continue;
        }

        if (!focus || focus->m_position.z > frame->m_position.z) {
            focus = frame;
        }
    }

    if (focus == CGUnit_C::s_namePlateFocus) {
        return;
    }

    if (CGUnit_C::s_namePlateFocus) {
        CGUnit_C::s_namePlateFocus->OnLoseFocus();
    }

    CGUnit_C::s_namePlateFocus = focus;

    if (focus) {
        focus->OnGainFocus();
    }
}

// OFFSET: 0x005C8650
int32_t CGUnit_C::ShouldShowNamePlate(CGWorldFrame* worldFrame, C3Vector* position) {
    if (this->m_unit->m_unitFieldHealth <= 0) {
        return 0;
    }

    if (this->m_unit->m_unitFieldFlags & 0x2000000) {
        return 0;
    }

    if (!this->m_worldObject || !World::QueryObjectVisible(this->m_worldObject)) {
        return 0;
    }

    if (!CGGameUI::m_visible || StaticSingleton<CCameraManager>::Instance()->m_legacyActive) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return 0;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!target || target == this) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*this->GetCharmerOrCreatorGuid(), TYPEMASK_UNIT, __FILE__, __LINE__));
    bool friendly;

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        friendly = player->CanCooperate(this);
    } else if (owner && (owner->m_obj->m_type & TYPEMASK_PLAYER)) {
        friendly = player->CanCooperate(owner);
    } else {
        friendly = !player->CanAttack(this, false);
    }

    if (!friendly) {
        if (!player->CanAttack(this, false)) {
            return 0;
        }

        if ((this->m_unit->m_unitFieldBytes1[2] & 0x2) || (this->m_unit->m_unitFieldFlags2 & 0x10000000)) {
            return 0;
        }
    }

    if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x100000)) {
        return 0;
    }

    if (!g_nameplateShowEnemiesCVar->m_intValue && !friendly) {
        return 0;
    }

    if (!g_nameplateShowFriendsCVar->m_intValue && friendly) {
        return 0;
    }

    if (this->GetCreatureType() == 8) {
        return 0;
    }

    if (this->GetCreatureType() == 12) {
        return 0;
    }

    if (this->m_unit->m_unitDynamicFlags & 0x20) {
        WOWGUID guid = this->m_obj->m_guid;

        if (!CGGameUI::IsRaidMemberOrPet(guid)) {
            return 0;
        }
    }

    if (owner && (owner->m_obj->m_type & TYPEMASK_PLAYER)) {
        if (this->HasCreator() && !this->HasSummoner() && (this->m_unit->m_unitFieldFlags & 0x200)) {
            return 0;
        }

        if (this->GetCreatureType() == 11) {
            if (friendly ? !g_nameplateShowFriendlyTotemsCVar->m_intValue : !g_nameplateShowEnemyTotemsCVar->m_intValue) {
                return 0;
            }
        } else if (this->GetCreatorGuid()) {
            if (friendly ? !g_nameplateShowFriendlyGuardiansCVar->m_intValue : !g_nameplateShowEnemyGuardiansCVar->m_intValue) {
                return 0;
            }
        } else if (friendly ? !g_nameplateShowFriendlyPetsCVar->m_intValue : !g_nameplateShowEnemyPetsCVar->m_intValue) {
            return 0;
        }
    }

    if (!World::QueryObjectVisible(this->m_worldObject)) {
        return 0;
    }

    C3Vector namePosition = this->GetNamePosition();
    namePosition.z = namePosition.z + 0.36666667f;

    if (!worldFrame->GetScreenCoordinates(namePosition, position, NULL)) {
        return 0;
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map || map->m_instanceType != 4) {
        C3Vector delta = this->GetPosition() - target->GetPosition();

        if (s_namePlateMaxDistanceSquared < delta.x * delta.x + delta.z * delta.z + delta.y * delta.y) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x005ABA40/0x14021DF30
void CGUnit_C::AddWorldHonorGainText(int32_t honor, int32_t pvpRank, const char* victimName) {
    char text[64];
    memset(text, 0, sizeof(text));
    const char* label;

    if (honor <= 0) {
        label = FrameScript_GetText("DK", -1, GENDER_NOT_APPLICABLE);

        if (!label || !*label) {
            label = "DK";
        }
    } else {
        label = FrameScript_GetText("HK", -1, GENDER_NOT_APPLICABLE);

        if (!label || !*label) {
            label = "HK";
        }
    }

    SStrPrintf(text, sizeof(text), "%s: %s", label, victimName);
    PlayerNameCreateText(this->m_nameDesc, 5, text, NULL, &pvpRank);
}

// OFFSET: 0x005BBB50/0x140230440
void CGUnit_C::AddWorldHealingText(const WOWGUID& source, int32_t amount, const SpellRec* spell, bool crit, int32_t a6) {
    bool isPlayer;

    if (!amount || this->m_obj->m_guid == CGUnit_C::s_activeMover || !IsOwnedByActivePlayer(source, &isPlayer) || !g_combatHealingCVar->m_intValue) {
        return;
    }

    char text[32];
    SStrPrintf(text, sizeof(text), "+%d", amount);

    if (isPlayer) {
        PlayerNameCreateText(this->m_nameDesc, crit + 6, text, NULL, NULL);
    }
}

// OFFSET: #0x00714B60
float CGUnit_C::GetHeightAboveGround() {
    C3Vector start = this->GetPosition();
    C3Vector end = start;
    start.z += 1.0f;
    end.z -= 1.0f;
    C3Vector hit = end;
    float distance = 1.0f;
    World::Intersect(start, end, &hit, &distance, 0x100111, NULL);

    return start.z - hit.z;
}

// OFFSET: 0x005BAA70
void CGUnit_C::RemoveChannelEffects(int32_t spellId) {
    for (uint32_t i = 0; i < (this->unk_0F9C.m_count == 0xFFFFFFFF ? this->unk_0F9C.Dynamic().m_count : this->unk_0F9C.m_count); i++) {
        if (this->unk_0F9C[i] == static_cast<uint32_t>(spellId)) {
            return;
        }
    }

    CEffect* effect = this->m_effectList;

    while (effect) {
        CEffect* current = effect;
        effect = effect->m_next;

        if (current->m_spellId == static_cast<uint32_t>(spellId) && (current->m_flags & 0x3040)) {
            current->MoveToDestroyList();
        }
    }

    this->UpdateEffectAnimKit();
    this->UpdateDeathAnimKit();
}

// OFFSET: 0x005D9F10/0x1402559E0
void CGUnit_C::UpdateSwimmingStatus(uint32_t time, int32_t spline) {
    if (this->m_movement->m_flags & MOVEMENTFLAG_DISABLE_COLLISION) {
        return;
    }

    if ((this->m_vehiclePassenger && this->m_vehiclePassenger->m_state) || (this->m_movementData.m_flags2 & 0x1000)) {
        return;
    }

    CMoveSpline* moveSpline = this->m_movement->m_spline;
    int32_t canSwim = moveSpline && !(moveSpline->m_flags & 0x80) ? this->m_movementData.IsOnSwimSpline() : this->CanSwim();

    if (!canSwim) {
        if (this->m_movement->m_flags & MOVEMENTFLAG_SWIMMING) {
            this->OnSwimStop(time, spline);
        }

        uint32_t wasUnder = this->m_clientFlags & 0x200000;
        this->m_clientFlags &= ~0x200000;

        if (this->IsActivePlayer() && wasUnder != (this->m_clientFlags & 0x200000)) {
            CGSpellBook::UpdateUsable();
        }

        return;
    }

    uint32_t liquidType;
    float height;
    int32_t liquidFlags;
    int32_t inLiquid = World::QueryObjectLiquid(this->m_worldObject, liquidType, height, liquidFlags);
    float depth = 0.0f;

    if (inLiquid) {
        depth = height - this->GetPosition().z;
    }

    float swimDepth = this->m_movementData.m_collisionHeight * 0.75f;
    float exitDepth = swimDepth - 0.027777778f;

    if (!(this->m_clientFlags & 0x80000000)) {
        this->UpdateSwimmingStatusDefault(time, spline, depth, swimDepth, exitDepth);
    } else {
        this->UpdateSwimmingStatusFlightStyle(time, depth, swimDepth, exitDepth);
    }

    uint32_t flags = this->m_clientFlags;
    uint32_t wasUnder = flags & 0x200000;

    if ((this->m_movement->m_flags & MOVEMENTFLAG_SWIMMING) || depth > swimDepth) {
        flags |= 0x200000;
    } else {
        flags &= ~0x200000;
    }

    uint32_t wasSurface = flags & 0x4000000;
    flags &= ~0x4000000;
    this->m_clientFlags = flags;

    if (inLiquid && depth - swimDepth <= 0.33333334f) {
        this->m_clientFlags = flags | 0x4000000;
    }

    if (this->IsActivePlayer() && (this->m_modelFlags & 0x40000) && (wasUnder != (this->m_clientFlags & 0x200000) || wasSurface != (this->m_clientFlags & 0x4000000))) {
        this->DismountIfMountRulesFail();
        CGSpellBook::UpdateUsable();
    }
}

// OFFSET: 0x005C4550/0x14023B030
void CGUnit_C::UpdateFlightStatus(uint32_t time) {
    if (!(this->m_movementData.m_flags & MOVEMENTFLAG_CAN_FLY)) {
        return;
    }

    if (this->m_movementData.m_flags & MOVEMENTFLAG_FLYING) {
        this->PlayFlightBoundsFeedback(time);
    }

    if (this->m_obj->m_guid != CGUnit_C::s_activeMover) {
        return;
    }

    uint32_t delay = (this->m_unit->m_unitFieldFlags & 0x8) ? s_flightTransitionDelayFlag : s_flightTransitionDelay;

    if (this->m_movementData.m_flags & MOVEMENTFLAG_FLYING) {
        if (time - s_swimStartTime > delay && (this->m_movementData.m_flags2 & 0x800) && (!(this->m_obj->m_type & TYPEMASK_PLAYER) || !static_cast<CGPlayer_C*>(this)->IsCommentatorUberOrInArena())) {
            this->m_movementData.OnFlightLocal(time, 0);
            s_swimStopTime = time;
        }
    } else if (time - s_swimStopTime > delay && !this->m_movementData.IsJumpingUp() && !(this->m_movementData.m_flags2 & 0x800) && ((this->m_movementData.m_flags & (MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR)) || this->m_movementData.HasFallVelocity())) {
        this->m_movementData.OnFlightLocal(time, 1);
        s_swimStartTime = time;
    }
}

// OFFSET: 0x005B99E0
void CGUnit_C::SetWeaponTrail(CM2Model* model, uint32_t hand) {
    if (hand > 1) {
        return;
    }

    if (this->m_weaponTrails[hand]) {
        WeaponTrailClose(this->m_weaponTrails[hand]);
    }

    this->m_weaponTrails[hand] = NULL;
    const CGUnitVirtualItem* item = this->GetVirtualItem(hand, false);

    if (!item) {
        return;
    }

    uint32_t link = this->m_unit->m_unitFieldBytes2[0] ? (hand != 0) + 1 : CCharacterComponent::GetSheatheLink(static_cast<SHEATHE_TYPE>(item->m_sheatheType), hand == 0);

    for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
        if (attached->m_attachmentId == link) {
            this->m_weaponTrails[hand] = WeaponTrailCreate(attached);
            return;
        }
    }
}

// OFFSET: #0x006CF350
float CGUnit_C::GetScaledModelScale(const CreatureDisplayInfoRec* displayInfo, const CreatureModelDataRec* modelData, float* modelScale) {
    float scale = this->GetScale();

    if (scale < 1.0f) {
        scale = 1.0f;
    }

    float model = this->GetModelScale(displayInfo, modelData);

    if (modelScale) {
        *modelScale = model;
    }

    return model * scale;
}

// OFFSET: 0x005C0200/0x140235C80
int32_t CGUnit_C::UpdateUnitCollisionBox(int32_t a2, int32_t a3) {
    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        return static_cast<CGPlayer_C*>(this)->UpdateUnitCollisionBox(a2, a3);
    }

    CreatureDisplayInfoRec* displayInfo;

    if (!this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags[0] & 0x20000) || !(displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldNativedisplayid))) {
        this->m_movementData.SetCollisionBox(0.6666667f, 2.0277777f, 1.0f, 1.0f, 0);
        return 1;
    }

    float width;
    float height;
    float scale;
    float modelScale;
    CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

    if (modelData) {
        float objectScale = this->GetScale();

        if (objectScale < 1.0f) {
            objectScale = 1.0f;
        }

        modelScale = this->GetModelScale(this->m_unit->m_unitFieldNativedisplayid);
        scale = modelScale * objectScale;
        height = modelData->m_collisionHeight;
        width = modelData->m_collisionWidth;
    } else {
        modelScale = 1.0f;
        scale = 1.0f;
        height = 2.0277777f;
        width = 0.6666667f;
    }

    this->m_movementData.SetCollisionBox(width, height, scale, modelScale, 0);
    return 1;
}

// OFFSET: 0x005B90F0
void CGUnit_C::UpdateMountTransitionModel() {
    if (!this->m_mountTransition || !this->m_mountTransitionEffect || !this->m_mountTransitionEffect->m_model) {
        return;
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    C3Vector up(0.0f, 0.0f, 0.0f);
    C44Matrix inverse;
    float facing;
    SpellVisualsGetMountTransitionTransform(this->m_mountTransition, &position, &up, &facing);

    C44Matrix world = this->m_worldModel->m_worldTransform;
    inverse = world.AffineInverse(this->GetTrueScale());

    this->m_mountTransitionEffect->m_model->SetWorldTransform(position, facing, this->GetTrueScale(), up, 0xFFFFFFFF);

    C44Matrix transform = this->m_mountTransitionEffect->m_model->m_worldTransform;
    CM2Model* model = this->m_mountTransitionEffect->m_model;
    model->SetWorldTransform(transform * inverse);

    if (model->m_attachParent) {
        model->m_fFlags |= 0x80;
    } else {
        model->m_fFlags |= 0x8;
    }

    if (model->m_attachParent) {
        model->m_fFlags |= 0x20000;
    } else {
        model->m_fFlags |= 0x10000;
    }

    float alpha = this->m_worldModel->m_fadeAlpha;
    this->m_mountTransitionEffect->m_model->m_fadeAlpha = SpellVisualsGetMountTransitionAlpha(this->m_mountTransition) * alpha;
}

// OFFSET: 0x005AA480
static int32_t IsArenaOpponent(const WOWGUID& guid) {
    for (int32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponents[i] == guid) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005BA270/0x14022E600
void CGUnit_C::UpdateScriptRegistration() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t flags = 0;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        flags = 0x1;
    }

    if (player) {
        const WOWGUID& pet = player->m_unit->m_unitFieldCharm.m_guid ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet == this->m_obj->m_guid) {
            flags |= 0x2;
        }
    }

    if (!(flags & 0x1)) {
        WOWGUID guid = this->m_obj->m_guid;

        if (CGPartyInfo::IsMember(guid)) {
            flags |= 0x10;
        }
    }

    if (!(flags & 0x2)) {
        WOWGUID guid = this->m_obj->m_guid;

        if (CGPartyInfo::IsMemberPet(guid, NULL)) {
            flags |= 0x20;
        }
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (IsGuidInMyRaid(guid)) {
        flags |= 0x100;
    }

    guid = this->m_obj->m_guid;

    if (CGRaidInfo::IsPetInRaid(guid, NULL)) {
        flags |= 0x200;
    }

    if (CGGameUI::m_lockedTarget == this->m_obj->m_guid) {
        flags |= 0x1000;
    }

    guid = this->m_obj->m_guid;

    if (IsArenaOpponent(guid)) {
        flags |= 0x4000;
    }

    if (flags == this->m_scriptRegistration) {
        return;
    }

    if (!this->m_scriptRegistration) {
        ScriptEventsRegisterUnit(this);
    } else if (!flags) {
        ScriptEventsUnregisterUnit(this);
    }

    this->m_scriptRegistration = flags;
}

// OFFSET: 0x005BFE80/0x140235880
void CGUnit_C::CreateOrDestroyObjectEffectManager() {
    int32_t displayId = this->m_mountDisplayId;

    if (!displayId) {
        if (this->m_unit->m_unitFieldNativedisplayid == this->m_unit->m_unitFieldDisplayid) {
            displayId = this->GetCurrentClientNativeDisplayID();
        } else {
            displayId = this->m_unit->m_unitFieldDisplayid;
        }
    }

    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);
    int32_t packageId = displayInfo ? displayInfo->m_objectEffectPackageID : 0;

    if (this->m_effectManager && this->m_effectManager->m_packageId != packageId) {
        this->m_animEffectStates[0] = 0;
        this->m_animEffectStates[1] = 0;
        this->m_animEffectStates[2] = 0;
        delete this->m_effectManager;
        this->m_effectManager = NULL;
    }

    if (!packageId || this->m_effectManager) {
        return;
    }

    void* mem = SMemAlloc(sizeof(CObjectEffectManager), __FILE__, __LINE__, 0);
    this->m_effectManager = mem ? new (mem) CObjectEffectManager() : NULL;

    if (this->m_effectManager->Init(displayInfo->m_objectEffectPackageID, this)) {
        this->m_animEffectStates[0] = 0;
        this->m_animEffectStates[1] = 0;
        this->m_animEffectStates[2] = 0;
        this->UpdateObjectEffectMovementStates();
    } else {
        delete this->m_effectManager;
        this->m_effectManager = NULL;
    }
}

// OFFSET: 0x005D4E30/0x14024F2C0
void CGUnit_C::UpdatePartyMemberPetState() {
    WOWGUID guid = this->m_obj->m_guid;
    CGPartyMemberState* partyState = NULL;
    CGPartyMemberState* raidState = NULL;
    int32_t index;

    if (CGPartyInfo::IsMemberPet(guid, &index) && index >= 0) {
        partyState = CGPartyInfo::m_members[index] ? &s_partyMemberStates[index] : NULL;
    }

    guid = this->m_obj->m_guid;

    if (CGRaidInfo::IsPetInRaid(guid, &index) && index >= 0) {
        raidState = CGRaidInfo::m_members[index]->m_guid ? &CGRaidInfo::m_members[index]->m_stats : NULL;
    }

    CGPartyMemberState oldState;
    CGPartyMemberState* source;

    if (partyState) {
        oldState = *partyState;
        source = partyState;
    } else {
        if (!raidState) {
            return;
        }

        oldState = *raidState;
        source = &oldState;
    }

    CGPartyMemberState newState = *source;
    this->FillPartyMemberPetState(&newState.m_pet);
    uint32_t changes = CGPlayer_C::BuildPartyMemberStatsChanges(&oldState, &newState);
    CGPlayer_C::DispatchPartyMemberStatsChanges(newState.m_guid, changes, &oldState, &newState);
}

// OFFSET: #0x0072CC30
int32_t CGUnit_C::IsHostileCritter() {
    if (this->GetCreatureType() != 7) {
        return 0;
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    if (!(GetVisibleAuraEffectFlags() & 0x1)) {
        return 1;
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player || player->IsHatedTowards(this)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0060C030/0x140295730
int32_t CreatureSoundDataGetFidgetSound(const CreatureSoundDataRec* rec, uint32_t index) {
    if (rec && index < 5) {
        return rec->m_soundFidget[index];
    }

    return 0;
}

// OFFSET: #0x007462A0
float CreatureSoundDataGetFidgetDelayMin(const CreatureSoundDataRec* rec) {
    if (rec) {
        return rec->m_fidgetDelaySecondsMin;
    }

    return 0.0f;
}

// OFFSET: #0x007462C0
float CreatureSoundDataGetFidgetDelayMax(const CreatureSoundDataRec* rec) {
    if (rec) {
        return rec->m_fidgetDelaySecondsMax;
    }

    return 0.0f;
}

// OFFSET: #0x0071D450
static int32_t IsWeaponAnimBehavior(uint32_t animId) {
    AnimationDataRec* animation = g_animationDataDB.GetRecord(animId);
    int32_t behavior = animation ? animation->m_behaviorID : 506;

    switch (behavior) {
        case 16:
        case 20:
        case 25:
        case 117:
        case 118:
            return 1;

        default:
            return 0;
    }
}

// OFFSET: 0x005B3B10
bool CGUnit_C::IsPlayingWeaponAnim() const {
    uint32_t anim = this->GetCurrentTorsoAnimId();
    return AnimationDataHasFlags(anim, 0x1000000) || (AnimationDataHasFlags(anim, 0x200000) && !this->GetVirtualItem(0, false));
}

// OFFSET: 0x005C5710/0x14023C670
void CGUnit_C::DisengageFromActivePlayer() {
    if (this->m_obj->m_guid == CGGameUI::m_lockedTarget) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->m_combat.m_attacking) {
            player->SetCombatMode();
        }

        Spell_C_CancelAutoRepeat(true);

        if (!(this->m_unit->m_unitDynamicFlags & 0x20)) {
            CGGameUI::Target(WOWGUID());
        } else {
            WOWGUID guid = this->m_obj->m_guid;

            if (!CGGameUI::IsRaidMemberOrPet(guid)) {
                CGGameUI::Target(WOWGUID());
            }
        }
    }

    if (this->m_obj->m_guid == CGGameUI::m_focusTarget) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player || !player->IsFriendly(this)) {
            WOWGUID guid = WOWGUID();
            CGGameUI::SetFocusTarget(guid);
        }
    }
}
// OFFSET: 0x005B3C80
void CGUnit_C::HandlePendingTrackEvents() {
    if (s_pendingAttackTarget) {
        if (ClntObjMgrObjectPtr(s_pendingAttackTarget, TYPEMASK_UNIT, __FILE__, __LINE__) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            static_cast<CGPlayer_C*>(this)->CombatModeEnter(s_pendingAttackTarget, 0, 0, 0, 0);
        }

        s_pendingAttackTarget = WOWGUID();
    } else if (s_pendingLootTarget) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pendingLootTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            static_cast<CGPlayer_C*>(this)->LootUnit(unit, false);
        }

        s_pendingLootTarget = WOWGUID();
    } else if (s_pendingUseObject) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(s_pendingUseObject, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

        if (gameObject) {
            gameObject->UseObject();
        }

        s_pendingUseObject = WOWGUID();
    } else if (s_pendingSkinTarget) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pendingSkinTarget, TYPEMASK_OBJECT, __FILE__, __LINE__));

        if (unit && this->IsActivePlayer()) {
            static_cast<CGPlayer_C*>(this)->HandleSkinning(unit, false, false);
        }

        s_pendingSkinTarget = WOWGUID();
    } else if (s_pendingInteractTarget) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pendingInteractTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && this->IsActivePlayer()) {
            static_cast<CGPlayer_C*>(this)->HandleOnRightClick(unit, false);
        }

        s_pendingInteractTarget = WOWGUID();
    }
}

// OFFSET: 0x005C6B70
void CGUnit_C::DestroyUnitMount(int32_t a2) {
    if (this->m_mountModel) {
        CM2Model* parent = this->m_worldModel->m_attachParent;

        if (parent) {
            CM2Model* grandParent = parent->m_attachParent;
            uint32_t attachmentId = parent->m_attachmentId;
            this->m_worldModel->m_fFlags ^= (this->m_worldModel->m_fFlags ^ parent->m_fFlags) & 0x40000;
            this->m_worldModel->m_drawFlags ^= (this->m_worldModel->m_drawFlags ^ parent->m_drawFlags) & 0x8;
            this->m_worldModel->DetachFromParent();

            if (grandParent) {
                this->m_worldModel->AttachToParent(grandParent, attachmentId, NULL, 0);
            }
        }

        CM2Model* mount = this->m_mountModel;
        this->m_worldModel->m_diffuseTintR = mount->m_diffuseTintR;
        this->m_worldModel->m_diffuseTintG = mount->m_diffuseTintG;
        this->m_worldModel->m_diffuseTintB = mount->m_diffuseTintB;
        this->SetMountModel(NULL);
    }

    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();
    this->m_soundDataOverride = NULL;

    if (!a2 || (this->UpdateUnitCollisionBox(1, 0), !(this->m_obj->m_type & TYPEMASK_PLAYER))) {
        if (this->m_obj->m_guid == CGWorldFrame::GetActiveCamera()->m_targetGUID) {
            CGGameUI::ResetCamera(this->m_obj->m_guid);
        }
    }

    if (this->GetObjectModel()->IsLoaded()) {
        this->InitWheels();
    } else {
        this->m_wheelMask = 0;
    }

    this->UpdateObjectEffectAnimationStates();
    this->m_clientFlags &= ~0x10000000;
}

// OFFSET: 0x005D6950
void CGUnit_C::CreateUnitMount() {
    this->m_wheelMask = 0;

    if (!this->IsMountSelf(0) && !this->m_mountModel) {
        CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);

        if (!displayInfo) {
            return;
        }

        CreatureModelDataRec* modelData = g_creatureModelDataDB.GetRecord(displayInfo->m_modelID);

        if (!modelData) {
            return;
        }

        this->m_mountScale = displayInfo->m_creatureModelScale;
        CM2Model* mount = World::GetScene()->CreateModel(modelData->m_modelName, 0, 3);
        mount->SetSequenceCallback(reinterpret_cast<void*>(&CGUnit_C::MountAnimEndCallback), this->m_obj->m_guid.m_guid);
        mount->SetSequenceStartCallback(&CGUnit_C::SequenceStartCallback, this->m_obj->m_guid.m_guid);
        mount->SetEventCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEventCallback), this->m_obj->m_guid.m_guid);
        mount->m_fFlags |= 0x4000;
        mount->m_worldTransform = this->m_worldModel->m_worldTransform;
        CCharacterComponent::ApplyMonsterGeosets(mount, displayInfo);
        CCharacterComponent::ReplaceMonsterSkin(mount, displayInfo, modelData);
        this->SetMountModel(mount);
        mount->Release();
        this->m_mountFootprintTextureId = modelData->m_footprintTextureID;
        this->m_mountFootprintWidth = modelData->m_footprintTextureLength * 0.027777778f;
        this->m_mountFootprintLength = modelData->m_footprintTextureWidth * 0.027777778f;
        this->m_soundDataOverride = this->GetMountSoundDataRec();
        this->m_mountModel->m_fFlags ^= (this->m_mountModel->m_fFlags ^ this->m_worldModel->m_fFlags) & 0x40000;
        this->m_worldModel->m_fFlags &= ~0x40000;
        this->m_mountModel->m_drawFlags ^= (this->m_mountModel->m_drawFlags ^ this->m_worldModel->m_drawFlags) & 0x8;
        this->m_worldModel->m_drawFlags &= ~0x8;
        CM2Model* parent = this->m_worldModel->m_attachParent;
        uint32_t attachmentId = this->m_worldModel->m_attachmentId;

        if (parent) {
            this->m_worldModel->DetachFromParent();
        }

        this->m_worldModel->AttachToParent(this->m_mountModel, 0, NULL, 0);

        if (parent) {
            if (this->m_mountModel->m_attachParent) {
                this->m_mountModel->DetachFromParent();
            }

            this->m_mountModel->AttachToParent(parent, attachmentId, NULL, 0);
        }

        float scale = 1.0f / this->m_mountScale;
        this->m_worldModel->m_fFlags |= 0x4000;
        this->m_worldModel->m_worldTransform = C44Matrix(scale);
        mount->m_diffuseTintR = this->m_worldModel->m_diffuseTintR;
        mount->m_diffuseTintG = this->m_worldModel->m_diffuseTintG;
        mount->m_diffuseTintB = this->m_worldModel->m_diffuseTintB;
        this->m_worldModel->m_fadeAlpha = 1.0f;
        this->m_worldModel->m_diffuseTintR = 1.0f;
        this->m_worldModel->m_diffuseTintG = 1.0f;
        this->m_worldModel->m_diffuseTintB = 1.0f;
        this->m_worldModel->m_emissiveTintR = 0.0f;
        this->m_worldModel->m_emissiveTintG = 0.0f;
        this->m_worldModel->m_emissiveTintB = 0.0f;
    }

    CEffect* effect = this->m_mountTransitionEffect;

    if (effect && !(effect->m_flags & 0x100)) {
        effect->MoveToDestroyList();
    }

    if (this->m_obj->m_guid == CGWorldFrame::GetActiveCamera()->m_targetGUID) {
        CGGameUI::ResetCamera(this->m_obj->m_guid);
    }

    this->UpdateObjectEffectAnimationStates();
}

// PROVEN: 93a563ce IDENTICAL/NONE
// OFFSET: 0x005D68F0/0x1402515A0
void CGUnit_C::SetModelCallbacks(CM2Model* model) {
    model->SetSequenceCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEndCallback), this->m_obj->m_guid.m_guid);
    model->SetEventCallback(reinterpret_cast<void*>(&CGUnit_C::AnimEventCallback), this->m_obj->m_guid.m_guid);
    model->SetSequenceStartCallback(&CGUnit_C::SequenceStartCallback, this->m_obj->m_guid.m_guid);
}

// PROVEN: 35213eec IDENTICAL/NONE
// OFFSET: 0x005AE160
void CGUnit_C::MountAnimEndCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && model == unit->m_mountModel && unit->m_mountAnimKitManager) {
        unit->m_mountAnimKitManager->OnModelSequenceCallback(boneId, sequenceId, interrupted, a7);
    }
}

// OFFSET: 0x005AE1C0
void CGUnit_C::SequenceStartCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, uint32_t variation, uint32_t time, uint64_t param, int32_t primary) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    CAnimKitManager* manager;

    if (model == unit->m_mountModel && (manager = unit->m_mountAnimKitManager)) {
        manager->OnModelSequenceStartCallback(boneId);
    } else if (model == unit->m_worldModel && (manager = &unit->m_animKitManager)) {
        manager->OnModelSequenceStartCallback(boneId);
    }
}

// OFFSET: #0x00724EA0
void CGUnit_C::PostShutdown() {
    CMissile::Destroy();
    CGNamePlateFrame* plate = s_unusedNamePlates.Head();

    while (!s_unusedNamePlates.IsEnd(plate)) {
        CGNamePlateFrame* next = s_unusedNamePlates.RawNext(plate);
        s_unusedNamePlates.UnlinkNode(plate);
        delete plate;
        plate = next;
    }

    CGUnit_C::s_lastNPCSoundGUID = WOWGUID();
    CVehicle_C::UninitSystem();
    CVehiclePassenger_C::UninitSystem();
    CVehicleCamera_C::UninitSystem();
    CGUnit_C::ShutdownMissileTrajectorySystem();
}

// OFFSET: 0x005BD220
void CGUnit_C::FootstepAnimEventHit(const C3Vector* position, int32_t left) {
    if (this->m_unit->m_unitFieldPetnumber || (this->m_movement->m_flags & MOVEMENTFLAG_HOVER) || (this->m_unit->m_unitFieldBytes1[2] & 0x2) || this->GetTransportGUID()) {
        return;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(this)->m_player->m_playerFlags & 0x10)) {
        return;
    }

    const C3Vector& cameraPos = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    float dy = position->y - cameraPos.y;
    float dx = position->x - cameraPos.x;
    float dz = position->z - cameraPos.z;
    float distanceSquared = dz * dz + dy * dy + dx * dx;

    if (distanceSquared > 2500.0f) {
        return;
    }

    if (!(this->m_modelData->m_flags & 0x20)) {
        C2Vector size(0.0f, 0.0f);
        int32_t textureId;
        this->GetFootprintInfo(&textureId, &size.x);
        int32_t activePlayer = this->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? 1 : 0;
        float facing = this->GetFacing();
        uint32_t moveFlags = this->m_movement->m_flags;

        if (moveFlags & MOVEMENTFLAG_STRAFE_LEFT) {
            if (moveFlags & MOVEMENTFLAG_FORWARD) {
                facing = facing + 0.78539819f;
            } else if (moveFlags & MOVEMENTFLAG_BACKWARD) {
                facing = facing - 0.78539819f;
            } else {
                facing = facing + 1.5707964f;
            }
        } else if (moveFlags & MOVEMENTFLAG_STRAFE_RIGHT) {
            if (moveFlags & MOVEMENTFLAG_FORWARD) {
                facing = facing - 0.78539819f;
            } else if (moveFlags & MOVEMENTFLAG_BACKWARD) {
                facing = facing + 0.78539819f;
            } else {
                facing = facing - 1.5707964f;
            }
        }

        World::NewFootprint(textureId, size, *position, facing, left == 0, this->m_terrainTypeId, activePlayer);
    }

    if (this->m_modelData->m_footstepShakeSize) {
        StaticSingleton<CCameraManager>::Instance()->AddShakeById(this->m_modelData->m_footstepShakeSize, *position);
    }

    if (distanceSquared > 625.0f || (this->m_movement->m_flags & MOVEMENTFLAG_BACKWARD) || (this->m_modelData->m_flags & 0x1) || !CGUnit_C::s_cvShowFootPrintParticles->m_intValue) {
        return;
    }

    const TerrainTypeRec* terrain = g_terrainTypeDB.GetRecord(this->m_terrainTypeId);
    uint32_t liquidType;
    float liquidHeight;
    int32_t liquidFlags;
    int32_t spray;

    if (!World::QueryObjectLiquid(this->m_worldObject, liquidType, liquidHeight, liquidFlags)) {
        if (!terrain) {
            return;
        }

        spray = terrain->m_flags & 0x1;
    } else {
        int32_t flag8;
        int32_t flag9;
        World::QueryObjectLiquidFlags(this->m_worldObject, flag8, flag9);
        float depth = liquidHeight - this->GetPosition().z;

        if (flag9 && this->m_movementData.m_collisionHeight * 0.5f > depth) {
            C3Vector splash(position->x, position->y, position->z + depth);
            CEffect* effect = CEffect::Create();
            effect->AddSpecialByPosition(!this->IsRunning(), splash, this->m_obj->m_guid, 0x220, CGObject_C::EffectOnceHandler);
            effect->Release();
        }

        if (!terrain || !(terrain->m_flags & 0x2) || depth <= 1.1920929e-7f) {
            return;
        }

        spray = this->m_movement->m_flags2 & 0x800;
    }

    if (!spray) {
        return;
    }

    const SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(this->IsRunning() ? terrain->m_footstepSprayWalk : terrain->m_footstepSprayRun);

    if (!effectName) {
        return;
    }

    C3Vector effectPosition = *position;

    if (this->GetTransportGUID()) {
        C44Matrix transport;
        MovementGetTransportMtxX(WOWGUID::Make(this->GetTransportGUID()), &transport, this->m_obj->m_guid, __FILE__, __LINE__);
        effectPosition = effectPosition * transport.AffineInverse();
    }

    CEffect* effect = CEffect::Create();
    effect->AddEffectByPosition(effectPosition, NULL, CGWorldFrame::s_currentWorldFrame->GetSceneTime(), 0, NULL, effectName, this->m_obj->m_guid, WOWGUID(), 0x220, CGObject_C::EffectOnceHandler, 0, NULL, WOWGUID::Make(this->GetTransportGUID()), -1, 0, 0);
    effect->Release();
}

// OFFSET: #0x0071DCC0
int32_t IsSpecialIdleAnim(uint32_t animId) {
    if (IsSpellCastAnim(animId)) {
        return 1;
    }

    AnimationDataRec* rec = g_animationDataDB.GetRecord(animId);

    if (rec && rec->m_behaviorID == 107) {
        return 1;
    }

    rec = g_animationDataDB.GetRecord(animId);

    if (rec && rec->m_behaviorID == 46) {
        return 1;
    }

    return GetAnimBehaviorId(animId) == 49;
}

// OFFSET: #0x0072CDE0
void CreatureQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(id, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->DestroyNamePlate();
        unit->m_creatureCacheEntry = stats;

        if (stats->m_family) {
            unit->UpdateModelScale(NULL);
        }

        unit->UpdateUnitCollisionBox(1, 0);
    }

    CGGameUI::UnitNameUpdate(guid);
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_CLASSIFICATION_CHANGED);
    TotemInfo* totem = CGGameUI::GetTotemInfo(guid);

    if (totem) {
        totem->m_name = unit->GetUnitName(NULL, 1);
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TOTEM_UPDATE, "%d", totem->m_slot + 1);
    }
}

// OFFSET: #0x00720BF0
int32_t CGUnit_C::OverrideMissileFirePos(C3Vector* position, const SpellRec* spell) {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return 0;
    }

    const SpellVisualRec* visual = GetSpellVisual(spell);

    if (!visual) {
        return 0;
    }

    C3Vector offset;

    if (visual->m_missileAttachment == -1) {
        if (0.0f == visual->m_missileCastOffset.x && 0.0f == visual->m_missileCastOffset.y && 0.0f == visual->m_missileCastOffset.z) {
            return 0;
        }

        CM2Model* model = this->m_worldModel;

        if (model && model->IsLoaded() && model->HasAttachment(s_effectAttachmentIds[12])) {
            offset.x = visual->m_missileCastOffset.x;
            offset.y = -visual->m_missileCastOffset.y;
            offset.z = visual->m_missileCastOffset.z;
            *position = this->GetAttachmentPosition(s_effectAttachmentIds[12], &offset);
            return 1;
        }

        C44Matrix matrix = this->GetModelMatrix();
        matrix.d0 = 0.0f;
        matrix.d1 = 0.0f;
        matrix.d2 = 0.0f;
        offset.x = visual->m_missileCastOffset.x;
        offset.y = -visual->m_missileCastOffset.y;
        offset.z = visual->m_missileCastOffset.z;
        C3Vector rotated = offset * matrix;
        *position = this->GetModelPosition() + rotated;
        return 1;
    }

    int32_t raw = visual->m_flags & 0x200;

    if (!this->HasAttachmentForEffect(visual->m_missileAttachment, raw)) {
        return 0;
    }

    offset.x = visual->m_missileCastOffset.x;
    offset.y = -visual->m_missileCastOffset.y;
    offset.z = visual->m_missileCastOffset.z;
    *position = this->GetAttachmentPositionForEffect(visual->m_missileAttachment, offset, raw);
    return 1;
}

// OFFSET: 0x005D2F70/0x14024CEA0
CM2Model* CGUnit_C::CreateItemModel(uint32_t displayId, uint32_t inventoryType, float* scale) {
    if (scale) {
        *scale = 1.0f;
    }

    const char* path;

    switch (inventoryType) {
        case 1:
            path = "Item\\ObjectComponents\\Head\\";
            break;
        case 3:
            path = "Item\\ObjectComponents\\Shoulder\\";
            break;
        case 14:
            path = "Item\\ObjectComponents\\Shield\\";
            break;
        case 24:
            path = "Item\\ObjectComponents\\Ammo\\";
            break;
        case 27:
            path = "Item\\ObjectComponents\\Quiver\\";
            break;
        default:
            path = "Item\\ObjectComponents\\Weapon\\";
            break;
    }

    ItemDisplayInfoRec rec;

    if (!g_itemDisplayInfoDB.GetRecordByIndex(displayId, &rec)) {
        return NULL;
    }

    uint32_t index;

    if (*rec.m_modelName[0]) {
        index = 0;
    } else if (*rec.m_modelName[1]) {
        index = 1;
    } else {
        return NULL;
    }

    char fileName[260];

    if (inventoryType == 1) {
        int32_t sex = this->m_displayInfoExtra ? this->m_displayInfoExtra->m_displaySexID : this->m_unit->m_unitFieldBytes0[2];
        ChrRacesRec* race = g_chrRacesDB.GetRecord(sex);

        if (!race) {
            return NULL;
        }

        SStrPrintf(fileName, sizeof(fileName), "%s%s_%s%s.mdx", path, rec.m_modelName[index], race->m_clientPrefix, s_genderSuffix[sex]);
    } else {
        SStrPrintf(fileName, sizeof(fileName), "%s%s", path, rec.m_modelName[index]);
    }

    CM2Model* model = World::GetScene()->CreateModel(fileName, 0, 3);

    if (!model) {
        return NULL;
    }

    if (*rec.m_modelTexture[index]) {
        SStrPrintf(fileName, sizeof(fileName), "%s%s", path, rec.m_modelTexture[index]);
        CStatus status;
        CTexture* texture = CreateTexture(fileName, &status);

        if (texture) {
            model->ReplaceTexture(2, texture);
            TextureRelease(texture);
        }
    }

    CCharacterComponent::ComponentUtilAddItemVisual(model, rec.m_itemVisual);
    CM2Model* worldModel = this->m_worldModel;

    if (scale && worldModel && worldModel->IsLoaded()) {
        uint32_t attachId;

        switch (inventoryType) {
            case 1:
                attachId = 11;
                break;
            case 14:
                attachId = 0;
                break;
            case 22:
                attachId = 2;
                break;
            case 24:
            case 25:
                attachId = 0xFFFFFFFF;
                break;
            default:
                attachId = 1;
                break;
        }

        if (worldModel->HasAttachment(attachId)) {
            C44Matrix transform = worldModel->GetAttachmentWorldTransform(attachId);
            *scale = sqrtf(transform.a1 * transform.a1 + transform.a2 * transform.a2 + transform.a0 * transform.a0);
        }
    }

    return model;
}

// PROVEN: a35410e7 IDENTICAL/NONE
// OFFSET: 0x005B19B0
UnitBloodRec* CGUnit_C::GetBloodRecord() {
    if (!this->m_bloodlevels) {
        return NULL;
    }

    return g_unitBloodDB.GetRecord(this->m_bloodlevels->m_violencelevel[ViolenceGetLevel()]);
}

// OFFSET: 0x005D48B0
void CGUnit_C::OnVirtualItemChanged(uint32_t slot, int32_t oldItemId) {
    if (this->m_displayTransition) {
        if (!this->m_displayTransition->UpdateState(1)) {
            this->StopDisplayTransition();
        }

        if (this->m_displayTransition) {
            this->m_displayTransition->Expedite(1);
            this->StopDisplayTransition();
        }
    }

    ItemRec* oldRec = g_itemDB.GetRecord(oldItemId, WOWGUID(), NULL, NULL, false);
    ItemRec* rec = g_itemDB.GetRecord(this->m_unit->m_unitVirtualItemSlotId[slot], WOWGUID(), NULL, NULL, false);
    this->m_virtualItemDisplayId[slot] = rec ? rec->m_displayInfoID : 0;
    CGUnitVirtualItem item;

    if (rec) {
        item.m_classID = static_cast<uint8_t>(rec->m_classID);
        item.m_subclassID = static_cast<uint8_t>(rec->m_subclassID);
        item.m_soundOverrideSubclassID = static_cast<uint8_t>(rec->m_soundOverrideSubclassID);
        item.m_material = static_cast<uint8_t>(rec->m_material);
        item.m_inventoryType = static_cast<uint8_t>(rec->m_inventoryType);
        item.m_sheatheType = static_cast<uint8_t>(rec->m_sheatheType);
    } else {
        item.m_classID = 0;
        item.m_subclassID = 0;
        item.m_soundOverrideSubclassID = 0;
        item.m_material = 0;
        item.m_inventoryType = 0;
        item.m_sheatheType = 0;
    }

    item.m_pad[0] = 0;
    item.m_pad[1] = 0;
    this->m_virtualItem[slot] = item;
    int32_t sheatheType = oldRec ? oldRec->m_sheatheType : 0;
    this->RemoveHandItemModel(s_virtualItemInvSlots[slot], sheatheType, false);

    if (slot == 1) {
        this->RemoveHandItemModel(s_virtualItemInvSlots[1], sheatheType, true);
    }

    const CGUnitVirtualItem* current = this->GetVirtualItem(slot, false);
    ItemDisplayInfoRec displayRec;

    if (current && this->GetVirtualItemDisplayRec(slot, displayRec)) {
        this->AddHandItemModel(&displayRec, s_virtualItemInvSlots[slot], current->m_sheatheType, this->m_sheatheState == 0, current->m_inventoryType == 14, false, 0);
        this->SetWeaponTrail(this->m_worldModel, slot);
    }
}

// OFFSET: #0x00719B80
void CGUnit_C::ComputeSmoothedSwimMatrix(C44Matrix* matrix, float elapsed, int32_t a3) {
    if (elapsed < 0.011111111f) {
        elapsed = 0.011111111f;
    } else if (elapsed >= 0.050000001f) {
        elapsed = 0.050000001f;
    }

    uint32_t moveFlags = this->m_movement->m_flags;
    float pitch;

    if (moveFlags & 0x3) {
        uint32_t flags = this->m_movementData.m_flags;

        if (flags & 0x400000) {
            pitch = 0.78539819f;
        } else if (flags & 0x800000) {
            pitch = -0.78539819f;
        } else {
            pitch = this->m_movementData.m_pitch;
        }
    } else {
        pitch = 0.0f;
    }

    float basePitch = 0.0f;
    float targetPitch = pitch;

    if (this->m_movementData.m_flags & 0xC) {
        targetPitch = 0.0f;
    } else {
        basePitch = pitch;
    }

    if ((moveFlags & 0x2) && (this->m_movementData.m_flags & 0xC00000)) {
        targetPitch = basePitch * -1.0f;
    }

    float facing = this->GetDisplayFacing();
    float facingDelta = facing - this->m_smoothFacing;
    this->m_smoothFacing = facing;

    if (facingDelta > 3.1415927f) {
        facingDelta = facingDelta - 6.2831855f;
    } else if (facingDelta < -3.1415927f) {
        facingDelta = facingDelta + 6.2831855f;
    }

    float pitchDelta = targetPitch - this->unk_08D4;

    if (fabs(pitchDelta) >= 0.0099999998f) {
        if (pitchDelta > 3.1415927f) {
            pitchDelta = pitchDelta - 6.2831855f;
        } else if (pitchDelta < -3.1415927f) {
            pitchDelta = pitchDelta + 6.2831855f;
        }

        float rate = elapsed * 12.566371f;

        if (rate < 0.0f) {
            rate = 0.0f;
        } else if (rate >= 1.0f) {
            rate = 1.0f;
        }

        float smoothed = pitchDelta * rate + this->unk_08D4;
        this->unk_08D4 = smoothed;

        if (smoothed > 3.1415927f) {
            this->unk_08D4 = smoothed - 6.2831855f;
        } else if (smoothed < -3.1415927f) {
            this->unk_08D4 = smoothed + 6.2831855f;
        }
    } else {
        this->unk_08D4 = targetPitch;
    }

    float targetRoll;

    if (a3) {
        targetRoll = facingDelta / elapsed * -0.15000001f;
        float rollDelta = targetRoll - this->unk_08D8;
        float rate = elapsed * 1.5707964f;

        if (!(this->m_movement->m_flags & 0x3)) {
            rate = rate * 4.0f;
        }

        if (rate < 0.0f) {
            rate = 0.0f;
        } else if (rate >= 1.0f) {
            rate = 1.0f;
        }

        float roll = rollDelta * rate + this->unk_08D8;

        if (roll < -0.78539819f) {
            this->unk_08D8 = -0.78539819f;
        } else if (roll >= 0.78539819f) {
            this->unk_08D8 = 0.78539819f;
        } else {
            this->unk_08D8 = roll;
        }
    } else {
        targetRoll = 0.0f;
    }

    if (fabs(facingDelta) > 1.5707964f) {
        this->unk_09FC |= 0x4;
    }

    if (this->unk_09FC & 0x4) {
        float yawDelta = facing - this->m_smoothFacingTarget;

        if (yawDelta > 3.1415927f) {
            yawDelta = yawDelta - 6.2831855f;
        } else if (yawDelta < -3.1415927f) {
            yawDelta = yawDelta + 6.2831855f;
        }

        float rate = elapsed * 12.566371f;
        float clamped = 0.0f;

        if (rate >= 0.0f) {
            clamped = rate;

            if (rate >= 1.0f) {
                clamped = 1.0f;
            }
        }

        this->m_smoothFacingTarget = CMath::normalizeangle0to2pi_(clamped * yawDelta + this->m_smoothFacingTarget);
    } else {
        this->m_smoothFacingTarget = facing;
    }

    C33Matrix rotation;
    rotation.FromEulerAnglesZYX(-(6.2831855f - this->m_smoothFacingTarget), 6.2831855f - this->unk_08D4, this->unk_08D8);
    matrix->a0 = rotation.a0;
    matrix->a1 = rotation.a1;
    matrix->a2 = rotation.a2;
    matrix->a3 = 0.0f;
    matrix->b0 = rotation.b0;
    matrix->b1 = rotation.b1;
    matrix->b2 = rotation.b2;
    matrix->b3 = 0.0f;
    matrix->c0 = rotation.c0;
    matrix->c1 = rotation.c1;
    matrix->c2 = rotation.c2;
    matrix->c3 = 0.0f;
    matrix->d0 = 0.0f;
    matrix->d1 = 0.0f;
    matrix->d2 = 0.0f;
    matrix->d3 = 1.0f;
    matrix->Scale(this->GetTrueScale());
    C3Vector position = this->GetPosition();
    matrix->d0 = position.x;
    matrix->d1 = position.y;
    matrix->d2 = position.z;
    uint32_t flags = this->unk_09FC & 0xFFFFFFF8;
    flags |= fabs(this->unk_08D4 - targetPitch) > 0.0099999998f;
    this->unk_09FC = flags;
    flags |= fabs(this->unk_08D8 - targetRoll) <= 0.0099999998f ? 0 : 2;
    this->unk_09FC = flags;

    if (fabs(this->m_smoothFacingTarget - facing) <= 0.0099999998f) {
        this->unk_09FC = flags;
    } else {
        this->unk_09FC = flags | 0x4;
    }
}

// OFFSET: 0x005AD530
int32_t Packet_SMSG_MULTIPLE_MOVES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msg->IsRead()) {
        return 1;
    }

    while (true) {
        uint8_t size;
        msg->Get(size);
        uint8_t* data;
        msg->GetDataInSitu(reinterpret_cast<void*&>(data), size);

        if (!data) {
            break;
        }

        CDataStore move(data, size);
        ClientServices::Connection()->ProcessMessage(time, &move, 0);

        if (msg->IsRead()) {
            return 1;
        }
    }

    return 0;
}

// PROVEN: c004bcbc IDENTICAL/NONE
// OFFSET: 0x005AD870
int32_t Packet_SMSG_PRE_RESURRECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CMissile::ClearInFlightMissilesWithTarget(guid);
    return 1;
}

// OFFSET: 0x005B1CE0
void CGUnit_C::NPCFlagChanged(uint32_t oldFlags) {
    uint32_t flags = this->m_unit->m_unitNpcFlags;
    uint32_t changed = oldFlags ^ flags;

    if ((changed & 0x1) && !(flags & 0x1) && CGGossipInfo::m_gossip == this->m_obj->m_guid) {
        CGGossipInfo::SetGossip(WOWGUID());
    }

    if (changed & 0x2) {
        this->RefreshInteractIcon();

        if (!(flags & 0x2) && CGQuestInfo::m_npc == this->m_obj->m_guid) {
            CGQuestInfo::QuestGiverFinished(0, 0, 0);
        }
    }

    if (changed & 0x2000) {
        this->RefreshInteractIcon();

        if (!(flags & 0x2000) && CGTaxiMap::m_unit == this->m_obj->m_guid) {
            CGTaxiMap::CloseMap();
        }
    }

    if ((changed & 0x10) && !(flags & 0x10) && CGClassTrainer::m_trainer == this->m_obj->m_guid) {
        CGClassTrainer::SetTrainer(WOWGUID(), 0);
    }

    if ((changed & 0x20000) && !(flags & 0x20000) && CGBankInfo::m_banker == this->m_obj->m_guid) {
        CGBankInfo::SetBanker(WOWGUID());
    }

    if ((changed & 0x800000) && !(flags & 0x800000) && CGGuildBankInfo::m_banker == this->m_obj->m_guid) {
        CGGuildBankInfo::SetGuildBanker(WOWGUID());
    }

    if ((changed & 0x4000000) && !(flags & 0x4000000) && CGMailInfo::m_object == this->m_obj->m_guid) {
        CGPlayer_C::TalkToMailbox(WOWGUID());
    }
}

// OFFSET: 0x005D8550
void CGUnit_C::OnMountDisplayChanged(int32_t displayId) {
    int32_t current = this->m_mountDisplayId;

    if (displayId == current) {
        return;
    }

    if (!displayId && this->m_vehicle && this->m_vehicle->m_passengers.Head()) {
        this->m_clientFlags |= 0x10000000;
        return;
    }

    this->m_clientFlags &= ~0x10000000;

    if (current) {
        this->DestroyUnitMount(displayId == 0);
    }

    bool playKit = !this->m_mountTransition && !this->IsMountSelf(0);
    this->m_mountDisplayId = displayId;

    if (this->m_mountDisplayId) {
        this->CreateUnitMount();

        if (playKit) {
            this->PlayMountVisualKit();
        }
    } else if (playKit) {
        const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(this->m_mountSpellVisualKitID);

        if (kit) {
            PlaySpellVisuaKitData data(NULL, kit, 8);
            this->PlaySpellVisualKit(data);
        }

        this->m_mountSpellVisualKitID = 0;
    }

    this->UpdateMovementAnimKit(0);
    this->CheckLoopSound();
    this->UpdateInteractIconAttach();
    PlayerNameTriggerNameRegenerate(this->m_nameDesc);
    this->CreateOrDestroyObjectEffectManager();
    this->UpdateMountAnimReplacementSets();
}

// OFFSET: 0x005D86B0/0x140253BA0
void CGUnit_C::Dismount() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || (this->m_unit->m_unitFieldFlags & 0x100000)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_MOUNT_AURA));
    msg.Finalize();
    ClientServices::Send2(&msg);

    if (this->m_mountDisplayId) {
        this->OnMountDisplayChanged(0);
    }
}

// OFFSET: 0x005D9DB0
int32_t Packet_SMSG_DISMOUNT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->m_mountDisplayId) {
        return 1;
    }

    if (unit->m_vehicle && unit->m_vehicle->m_passengers.Head()) {
        unit->m_clientFlags |= 0x10000000;
        return 1;
    }

    unit->m_clientFlags &= ~0x10000000;
    unit->DestroyUnitMount(1);
    bool noTransition = !unit->m_mountTransition;
    unit->m_mountDisplayId = 0;
    int32_t playKit = noTransition && !unit->IsMountSelf(0);

    if (unit->m_mountDisplayId) {
        unit->CreateUnitMount();

        if (playKit) {
            unit->PlayMountVisualKit();
        }
    } else if (playKit) {
        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(unit->m_mountSpellVisualKitID);

        if (kit) {
            PlaySpellVisuaKitData data(NULL, kit, 8);
            unit->PlaySpellVisualKit(data);
        }

        unit->m_mountSpellVisualKitID = 0;
    }

    unit->UpdateMovementAnimKit(0);
    unit->CheckLoopSound();
    unit->UpdateInteractIconAttach();
    PlayerNameTriggerNameRegenerate(unit->m_nameDesc);
    unit->CreateOrDestroyObjectEffectManager();
    unit->UpdateMountAnimReplacementSets();
    return 1;
}

// PROVEN: ad08a36b IDENTICAL/NONE
// OFFSET: 0x005AC650
void CGUnit_C::SetDeferredClientControl(WOWGUID guid, uint8_t state) {
    if (!CGUnit_C::s_deferredClientControlUpdateGUID || CGUnit_C::s_deferredClientControlUpdateGUID == guid || !CGUnit_C::s_deferredClientControlUpdateState || state) {
        CGUnit_C::s_deferredClientControlUpdateGUID = guid;
        CGUnit_C::s_deferredClientControlUpdateState = state;
    }
}

// OFFSET: 0x005CB150
int32_t Packet_SMSG_CLIENT_CONTROL_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    uint8_t state;
    *msg >> guid;
    msg->Get(state);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->ExecuteClientControlUpdate(guid, state);
    } else {
        CGUnit_C::SetDeferredClientControl(guid, state);
    }

    return 1;
}

// OFFSET: 0x005CB1D0
int32_t Packet_SMSG_CANCEL_AUTO_REPEAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            Spell_C_CancelAutoRepeat(false);
        }

        if (unit->m_animationState & 0x8) {
            unit->m_animationState &= ~0x8;
            unit->UpdateCombatReadyAnimKit(0);
        }

        unit->CancelRangedMode();
    }

    return 1;
}

// PROVEN: 82302426 IDENTICAL/NONE
// OFFSET: 0x005AB990
void CGUnit_C::UpdateUnitNameColor() {
    PlayerNameTriggerColorUpdate(this->m_nameDesc);

    if (this->m_namePlateFrame) {
        this->m_namePlateFrame->UpdateColorDisplay(this);
    }
}

// PROVEN: ee3ea77b IDENTICAL/NONE
// OFFSET: 0x005B2720/0x140225C20
int32_t CGUnit_C::GetAbilitySkillLine(int32_t spellId) {
    SkillLineAbilityRec* ability = this->LookupAbility(spellId);
    return ability ? ability->m_skillLine : 0;
}

// OFFSET: 0x005AC200
static void __stdcall EnableTrackingPitch(uint32_t time, float pitch) {
    CGInputControl::GetActive()->ToggleControlBit(0x400000, 1, time, 0);
}

// OFFSET: 0x005AC220
static void __stdcall DisableTrackingPitch(uint32_t time) {
    CGInputControl::GetActive()->ToggleControlBit(0x400000, 0, time, 0);
}

// OFFSET: 0x005AC250/0x14021E7A0
bool CGUnit_C::CanMoveTo(int32_t action, const C3Vector& position) {
    if (!s_clickToMoveActions[action].m_enabled || 0.0f == s_clickToMoveActions[action].m_stopDistanceSq) {
        return true;
    }

    C3Vector current = this->GetPosition();
    float dx = position.x - current.x;
    float dy = position.y - current.y;
    float dz = position.z - current.z;

    if (dx * dx + dz * dz + dy * dy < s_clickToMoveActions[action].m_stopDistanceSq) {
        return true;
    }

    CGGameUI::DisplayError(ERR_AUTOFOLLOW_TOO_FAR);
    return false;
}

// OFFSET: 0x005B39A0/0x140226EE0
void CGUnit_C::TrackingStopMove(uint32_t time) {
    if ((this->m_unit->m_unitFieldFlags2 & 0x40) || this->IsVehicleCurrentlyUnstoppable()) {
        return;
    }

    CGInputControl::GetActive()->ToggleControlBit(0x200000, 0, time, 0);

    if (CGInputControl::GetActive()->CanControl(this) && (this->m_movement->m_flags & 0x3)) {
        CGUnit_C::s_trackingFlags |= 0x1;
        this->m_movementData.OnMoveStopLocal(time);
        CGInputControl::GetActive()->UpdateMoveStopped();
        CGUnit_C::s_trackingFlags &= ~0x1;
    }
}

// OFFSET: 0x005B3E50/0x140227420
bool CGUnit_C::GetInteractDistanceOfAction(int32_t action, const WOWGUID& guid, float* distance) {
    if (!s_clickToMoveActions[action].m_enabled || s_clickToMoveActions[action].unk_0009) {
        return true;
    }

    float minRange;
    float maxRange;

    switch (action) {
        case 3:
            *distance = s_trackingFollowDistanceSq;
            return true;

        case 4:
            *distance = s_trackingTerrainDistanceSq;
            return true;

        case 5: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                float range = unit->m_unit->m_unitFieldCombatreach * 0.5f + 2.0f;
                *distance = range * range;
            } else {
                *distance = s_trackingDefaultInteractDistanceSq;
            }

            return true;
        }

        case 6:
        case 10: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
            float range = unit ? this->CGUnit::GetMeleeRange(unit) - 1.3333334f : 5.0f;
            *distance = range * range;
            return true;
        }

        case 7:
            *distance = s_trackingObjectRange * s_trackingObjectRange;
            return true;

        case 9: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__));

            if (!unit) {
                return false;
            }

            int32_t spellId = CGSpellBook::GetSkinningSpell(unit);

            if (!spellId) {
                return false;
            }

            Spell_C::GetSpellRange(this, spellId, &minRange, &maxRange, unit);
            break;
        }

        case 11: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                return false;
            }

            Spell_C::GetSpellRange(this, CGSpellBook::m_autoRangedSpell, &minRange, &maxRange, unit);
            break;
        }

        default:
            return false;
    }

    float range = maxRange * 0.9f;
    *distance = range * range;
    return true;
}

// OFFSET: 0x005C1F20/0x140238030
bool CGPlayer_C::ClickToMove(int32_t type, const WOWGUID* guid, const C3Vector* position, float facing) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::s_trackingType != 13) {
        if (CGUnit_C::s_trackingType == 3) {
            FrameScript_SignalEvent(SCRIPT_EVENT_AUTOFOLLOW_END, NULL);
        }

        FrameTime::GetCurTimeMs();
        CGUnit_C::s_unk_CA11D0 = 0.0f;
        CGUnit_C::s_trackingGUID = WOWGUID();
        CGUnit_C::s_trackingFlags = 0;
        CGUnit_C::s_unk_CA11D4 = 0;
        CGUnit_C::s_trackingType = 13;
    }

    if (!this->CanMoveTo(type, *position) || !this->GetInteractDistanceOfAction(type, *guid, &s_trackingInteractDistanceSq)) {
        return false;
    }

    CGUnit_C::s_trackingType = type;
    s_trackingLastTime = FrameTime::GetCurTimeMs();
    s_trackingStartPosition = this->m_movementData.GetPosition(this->GetPosition());
    CGUnit_C::s_trackingDirection = facing;
    CGUnit_C::s_trackingGUID = *guid;
    CGUnit_C::s_trackingPosition = *position;
    s_trackingTurnRate = (this->m_movement->m_flags2 & 0x8) ? this->m_movement->m_turnRate : 3.1415927f;
    CGUnit_C::s_unk_CA11D4 = 0;
    s_trackingInteractDistance = sqrt(s_trackingInteractDistanceSq);
    CGUnit_C::s_unk_CA11D0 = 0.0f;

    if (0.0f == s_forcedRunSpeed) {
        s_forcedRunSpeed = this->m_movementData.m_runSpeed;
    }

    if (this->GetClientStandState() && (this->m_obj->m_type & TYPEMASK_PLAYER)) {
        this->ChangeStandState(0);
    }

    return true;
}

// OFFSET: 0x005C6670/0x14023D810
bool CGUnit_C::IsValidFollowTarget(CGUnit_C* unit) {
    if (unit == this || !(unit->m_obj->m_type & TYPEMASK_PLAYER) || unit->m_unit->m_unitFieldCharmedby) {
        return false;
    }

    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(this->m_unit->m_unitFieldFactiontemplate);
    FactionTemplateRec* other = g_factionTemplateDB.GetRecord(unit->m_unit->m_unitFieldFactiontemplate);

    if (faction && other && faction->m_factionGroup != other->m_factionGroup) {
        return false;
    }

    return !this->CanAttack(unit);
}

// OFFSET: 0x005C89C0
void CGUnit_C::HandleTrackingMoveThreshold() {
    switch (CGUnit_C::s_trackingType) {
        case 4:
            this->ClearTrackingTarget(true, true);
            break;

        case 5:
            s_pendingInteractTarget = CGUnit_C::s_trackingGUID;
            this->ClearTrackingTarget(true, true);
            break;

        case 6:
            s_pendingLootTarget = CGUnit_C::s_trackingGUID;
            this->ClearTrackingTarget(true, true);
            break;

        case 7:
            s_pendingUseObject = CGUnit_C::s_trackingGUID;
            this->ClearTrackingTarget(true, true);
            break;

        case 9:
            s_pendingSkinTarget = CGUnit_C::s_trackingGUID;
            this->ClearTrackingTarget(true, true);
            break;

        case 10:
        case 11:
            s_pendingAttackTarget = CGUnit_C::s_trackingGUID;
            this->ClearTrackingTarget(true, true);
            break;

        default:
            this->TrackingStopMove(FrameTime::GetCurTimeMs());
            break;
    }
}

// OFFSET: 0x005C8AC0/0x140240320
bool CGUnit_C::SetTrackingTarget(const WOWGUID& target, int32_t type) {
    if (target == this->m_obj->m_guid || this->m_obj->m_guid != CGUnit_C::s_activeMover) {
        return false;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return false;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(object);

    switch (type) {
        case 0:
        case 1:
        case 5:
            if (!(object->m_obj->m_type & TYPEMASK_UNIT)) {
                return false;
            }

            break;

        case 3:
            if (!this->IsValidFollowTarget(unit)) {
                CGGameUI::DisplayError(ERR_INVALID_FOLLOW_TARGET);
                return false;
            }

            if (this->m_unit->m_unitFieldHealth <= 0) {
                CGGameUI::DisplayError(ERR_PLAYER_DEAD);
                return false;
            }

            if (this->m_unit->m_unitFieldFlags & 0x40000) {
                CGGameUI::DisplayError(ERR_GENERIC_STUNNED);
                return false;
            }

            if (this->m_unit->m_unitChannelSpell) {
                CGGameUI::DisplayError(ERR_TOOBUSYTOFOLLOW);
                return false;
            }

            break;

        case 6:
        case 9:
            if (!(object->m_obj->m_type & TYPEMASK_CORPSE) && (!(object->m_obj->m_type & TYPEMASK_UNIT) || unit->m_unit->m_unitFieldHealth > 0)) {
                return false;
            }

            break;

        default:
            return false;
    }

    if (this->m_unit->m_unitFieldHealth <= 0) {
        return false;
    }

    C3Vector position = object->GetPosition();

    if (!static_cast<CGPlayer_C*>(this)->ClickToMove(type, &target, &position, 0.0f)) {
        return false;
    }

    if (type == 3) {
        FrameScript_SignalEvent(SCRIPT_EVENT_AUTOFOLLOW_BEGIN, "%s", unit->GetUnitName(NULL, 1));
    }

    return true;
}

// INLINED
bool CGPlayer_C::CTMFace(float facing) {
    if (!this->CanAutoInteract()) {
        return false;
    }

    C3Vector position = C3Vector();
    WOWGUID guid = WOWGUID();
    return this->ClickToMove(2, &guid, &position, facing);
}

// OFFSET: 0x005C8D10
bool CGPlayer_C::CTMClickTerrain(const C3Vector& position) {
    if (!this->CanAutoInteract()) {
        return false;
    }

    WOWGUID guid = WOWGUID();
    return this->ClickToMove(4, &guid, &position, 0.0f);
}

// OFFSET: #0x0072B730
bool CGPlayer_C::CTMInteractGO(const WOWGUID& guid, float range) {
    if (!this->CanAutoInteract()) {
        return false;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!object) {
        return false;
    }

    C3Vector position = object->GetPosition();

    if (!this->ClickToMove(7, &guid, &position, 0.0f)) {
        return false;
    }

    s_trackingObjectRange = range * 0.9f;
    return true;
}

// OFFSET: 0x005CDB90
void CGUnit_C::TrackingStartMove(uint32_t time) {
    CGInputControl::GetActive()->ToggleControlBit(0x200000, 1, time, 0);

    if (CGInputControl::GetActive()->CanControl(this)) {
        CGUnit_C::s_trackingFlags |= 0x1;
        this->OnMoveStartLocal(time, 1);
        CGUnit_C::s_trackingFlags &= ~0x1;
    }
}

// OFFSET: 0x005CDBE0
void CGUnit_C::TrackingSetPitch(uint32_t time, float pitch) {
    if (CGInputControl::GetActive()->CanControl(this)) {
        CGUnit_C::s_trackingFlags |= 0x1;
        this->OnSetPitchLocal(time, pitch);
        CGUnit_C::s_trackingFlags &= ~0x1;
    }
}

// OFFSET: 0x005CDD00
bool CGPlayer_C::CTMAttackUnit(CGUnit_C* unit) {
    int32_t modalSpell = 0;
    float range = static_cast<CGUnit*>(unit)->m_unit->m_unitFieldCombatreach + this->m_unit->m_unitFieldCombatreach + 1.3333334f;

    if (range < 5.0f) {
        range = 5.0f;
    }

    C3Vector target = unit->GetPosition();
    C3Vector position = this->GetPosition();
    float dz = position.z - target.z;
    float dy = position.y - target.y;
    float dx = position.x - target.x;

    if (range * range > dx * dx + (dy * dy + dz * dz)) {
        C3Vector current = this->GetPosition();
        return this->CTMFace(CalculateFacingTo(&current, &target));
    }

    if (this->m_unit->m_unitFieldHealth <= 0 || this->m_obj->m_guid != CGUnit_C::s_activeMover || !g_cvAutoInteract->m_intValue) {
        return false;
    }

    CAaBox box;
    box.b = target;
    box.t = target;

    // STATIC: 0x00DD62D8
    static World::FacetData s_facets;

    s_facets.facets.SetCount(0);
    s_facets.facetIds.m_count = 0;
    s_facets.facetFlags.m_count = 0;
    s_facets.m_status = 0;

    if (World::GetFacets(box, s_facets, 0x100111) && s_facets.facets.m_count && s_facets.facets.m_data[0].plane.n.z > 0.0000099999997f) {
        const C4Plane& plane = s_facets.facets.m_data[0].plane;
        target.z = -((plane.n.y * target.y + plane.n.x * target.x + plane.d) / plane.n.z);
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        modalSpell = GetModalNextSpell();
    }

    WOWGUID guid = unit->m_obj->m_guid;
    return this->ClickToMove((modalSpell != 0) + 10, &guid, &target, 0.0f);
}

// OFFSET: 0x005CDFA0
void CGUnit_C::SetTrackingDirection(const C3Vector& direction) {
    if (!this->CanAutoInteract()) {
        return;
    }

    C3Vector none = C3Vector();

    if (!this->CanMoveTo(8, none)) {
        return;
    }

    C3Vector dir = direction;
    float scale = 1.0f / sqrt(dir.y * dir.y + dir.x * dir.x + dir.z * dir.z);
    dir.x *= scale;
    dir.y *= scale;
    dir.z *= scale;

    bool flying = (this->m_movement->m_flags & 0x2200000) != 0;
    float up = fabs((dir.x + dir.y) * 0.0f + dir.z);

    if (flying) {
        float pitch = 0.0f;

        if (up > 0.86602002f) {
            pitch = CopySign(1.5707964f, dir.z);
        } else if (up > 0.17364f) {
            pitch = asin(dir.z);
        }

        this->OnSetPitchLocal(FrameTime::GetCurTimeMs(), pitch);
    }

    C3Vector current = this->GetPosition();
    C3Vector target;
    target.x = current.x + dir.x;
    target.y = current.y + dir.y;
    target.z = current.z + dir.z;

    C3Vector position = this->GetPosition();
    float facing = CalculateFacingTo(&position, &target);
    C3Vector zero = C3Vector();
    WOWGUID guid = WOWGUID();
    static_cast<CGPlayer_C*>(this)->ClickToMove(8, &guid, &zero, facing);
}

// OFFSET: 0x005CE180
bool CGUnit_C::HandleTrackingPitch(float targetZ, float z, float pitch, float maxStep, uint32_t time, bool* overshoot) {
    uint32_t liquidType;
    int32_t liquidFlags;

    if (z < targetZ && World::QueryObjectLiquid(this->m_worldObject, liquidType, targetZ, liquidFlags) && targetZ > z && fabs(targetZ - z - this->m_movementData.m_collisionHeight * 0.75f) < 0.001f) {
        if (0.0f != this->GetPitch()) {
            EnableTrackingPitch(time, 0.0f);
            this->TrackingSetPitch(time, 0.0f);
        }

        return false;
    }

    CGInputControl::GetActive()->ToggleControlBit(0x400000, 1, time, 0);

    float current = this->m_movementData.m_pitch;

    if (fabs(pitch - current) < 0.001f) {
        return false;
    }

    float newPitch = pitch;

    if (pitch <= current) {
        float step = current - maxStep;

        if (step > newPitch) {
            newPitch = step;
        }
    } else {
        float step = current + maxStep;

        if (step < newPitch) {
            newPitch = step;
        }
    }

    this->TrackingSetPitch(time, newPitch);
    *overshoot = fabs(newPitch - pitch) > 0.39269909f;
    return true;
}

// OFFSET: 0x005D21B0
void CGUnit_C::TrackingSetTurn(uint32_t time, float facing) {
    if (CGInputControl::GetActive()->CanControl(this)) {
        CGUnit_C::s_trackingFlags |= 0x1;
        this->OnSetRawFacingLocal(time, this->m_movement->GetRawFacing(facing));
        CGUnit_C::s_trackingFlags &= ~0x1;
    }
}

// OFFSET: #0x00731650
bool CGUnit_C::HandleTrackingFacing(float facing, float maxTurn, uint32_t time) {
    CGUnit_C::s_trackingFacing = facing;
    CGInputControl::GetActive()->ToggleControlBit(0x1000000, 1, time, 0);

    float delta = CMath::normalizeangle0to2pi_(facing - this->GetFacing());

    if (fabs(delta) < 0.001f || fabs(delta - 6.2831855f) < 0.001f) {
        return false;
    }

    CMovementShared* movement = this->m_movement;

    if (movement->m_flags2 & 0x8) {
        s_trackingTurnRate = movement->m_turnRate;
    } else if (3.1415927f == s_trackingTurnRate) {
        s_trackingTurnRate = (delta <= 2.0943952f || delta >= 4.1887903f ? 800.0f : 1200.0f) * 0.017453292f;
    }

    float turn = delta <= 3.1415927f ? delta : 6.2831855f - delta;

    if (maxTurn <= turn) {
        turn = maxTurn;
    }

    float newFacing = delta <= 3.1415927f ? this->GetFacing() + turn : this->GetFacing() - turn;
    this->TrackingSetTurn(time, CMath::normalizeangle0to2pi_(newFacing));
    return true;
}

// OFFSET: 0x005D2350
void CGUnit_C::HandleTracking(float drunkness, float turnOffset) {
    C3Vector target = C3Vector();
    bool useTarget = true;
    bool stuck = false;

    switch (CGUnit_C::s_trackingType) {
        case 2:
        case 8:
            useTarget = false;
            break;

        case 4:
        case 10:
            target = this->m_movementData.GetPosition(CGUnit_C::s_trackingPosition);
            break;

        default: {
            CGObject_C* object = ClntObjMgrObjectPtr(CGUnit_C::s_trackingGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (!object) {
                this->ClearTrackingTarget(false, true);
                return;
            }

            if (object->m_obj->m_type & TYPEMASK_UNIT) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(object);

                if ((unit->m_unit->m_unitFieldFlags & 0x100000) || (unit->m_unit->m_unitFieldHealth <= 0 && CGUnit_C::s_trackingType != 9 && CGUnit_C::s_trackingType != 6)) {
                    this->ClearTrackingTarget(false, true);
                    return;
                }
            }

            target = object->GetPosition();
            break;
        }
    }

    C3Vector position = this->GetPosition();
    C3Vector start = this->m_movementData.GetPosition(s_trackingStartPosition);
    s_trackingStartPosition = this->GetRawPosition();

    float dx = position.x - target.x;
    float dy = position.y - target.y;
    float dz = position.z - target.z;
    float distSq2 = dx * dx + dy * dy;
    float distSq3 = dz * dz + distSq2;
    float distSq = ((this->m_movement->m_flags & 0x2200000) || (this->m_movementData.m_flags & 0x1000000)) ? distSq3 : distSq2;

    if (s_clickToMoveActions[CGUnit_C::s_trackingType].m_enabled && 0.0f != s_clickToMoveActions[CGUnit_C::s_trackingType].m_stopDistanceSq && !(distSq < s_clickToMoveActions[CGUnit_C::s_trackingType].m_stopDistanceSq)) {
        this->ClearTrackingTarget(false, true);
        return;
    }

    float speedScale = this->m_movement->GetBaseSpeed(0) * 0.14285715f < 1.0f ? 1.0f : this->m_movement->GetBaseSpeed(0) * 0.14285715f;

    if (s_clickToMoveActions[CGUnit_C::s_trackingType].m_enabled && (CGUnit_C::s_trackingFlags & 0x2)) {
        switch (CGUnit_C::s_trackingType) {
            case 2:
            case 3:
            case 5:
            case 8:
                break;

            default: {
                float sx = start.x - target.x;
                float sy = start.y - target.y;
                float sz = start.z - target.z;

                if (sx * sx + sy * sy + sz * sz < distSq3) {
                    stuck = true;

                    if (++CGUnit_C::s_unk_CA11D4 >= 5) {
                        this->ClearTrackingTarget(false, true);
                        return;
                    }
                }

                break;
            }
        }
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    float elapsed = fabs(static_cast<float>(time - s_trackingLastTime)) * 0.001f;
    s_trackingLastTime = time;

    float facingTo = CMath::normalizeangle0to2pi_(useTarget ? CalculateFacingTo(&position, &target) : CGUnit_C::s_trackingDirection);
    float desired = facingTo;
    bool moving = false;

    if (drunkness > 0.0f) {
        moving = true;

        if ((this->m_movement->m_flags & 0xF) && (CGUnit_C::s_trackingFlags & 0x2)) {
            desired = this->GetFacing() + turnOffset;
        }
    }

    bool turning = this->HandleTrackingFacing(desired, static_cast<float>(fmod(static_cast<double>(s_trackingTurnRate * elapsed), 6.283185307179586)), time);

    if (!turning) {
        CGInputControl::GetActive()->ToggleControlBit(0x1000000, 0, time, 0);

        if (CGUnit_C::s_trackingType == 2) {
            this->ClearTrackingTarget(false, true);
            return;
        }

        CGUnit_C::s_trackingFlags |= 0x2;
    } else if (moving && CGUnit_C::s_trackingType == 3 && CRandom::ufloat_(g_rndSeed) * drunkness < 0.07f) {
        CGUnit_C::s_trackingFlags &= ~0x2;
    }

    bool overshoot = false;
    float pitchStep = static_cast<float>(fmod(static_cast<double>(elapsed * 3.1415927f), 6.283185307179586));
    bool pitching = false;
    uint32_t moveFlags = this->m_movement->m_flags;

    if ((moveFlags & 0x2200000) && useTarget && s_trackingStuckDistanceSq < distSq3) {
        float pitch;

        if (moving && (moveFlags & 0x3) && (CGUnit_C::s_trackingFlags & 0x4)) {
            pitch = this->GetPitch() + turnOffset;
        } else {
            pitch = CGUnit_C::ComputeTrackingElevationAngle(target, position);
        }

        pitching = this->HandleTrackingPitch(target.z, position.z, pitch, pitchStep, time, &overshoot);

        if (!pitching) {
            DisableTrackingPitch(time);
            CGUnit_C::s_trackingFlags |= 0x4;
        }
    }

    if (!s_clickToMoveActions[CGUnit_C::s_trackingType].m_enabled) {
        return;
    }

    if (moving && (CGUnit_C::s_trackingFlags & 0x2)) {
        float diff = CMath::normalizeangle0to2pi_(facingTo - this->GetFacing());

        if (diff > 2.7925267f && diff < 3.8397243f) {
            this->ClearTrackingTarget(false, true);
            return;
        }
    }

    if ((this->m_movement->m_flags & 0x3) && !turning && !pitching && s_clickToMoveActions[CGUnit_C::s_trackingType].unk_0008) {
        float mx = position.x - start.x;
        float my = position.y - start.y;
        float mz = position.z - start.z;

        if (mx * mx + my * my + mz * mz < s_trackingStuckDistanceSq) {
            stuck = true;

            if (++CGUnit_C::s_unk_CA11D4 >= 5) {
                this->ClearTrackingTarget(false, true);
                return;
            }
        }
    }

    if (!s_clickToMoveActions[CGUnit_C::s_trackingType].unk_0009) {
        float range = s_trackingInteractDistance;

        if (CGUnit_C::s_trackingType == 4) {
            range = (this->m_movement->GetBaseSpeed(0) * 0.14285715f < 1.0f ? 1.0f : this->m_movement->GetBaseSpeed(0) * 0.14285715f) * range;
        }

        if (!(range * range < distSq)) {
            this->HandleTrackingMoveThreshold();
            return;
        }
    }

    if (!stuck) {
        CGUnit_C::s_unk_CA11D4 = 0;
    }

    // STATIC: #0x00CA12A8
    static float s_flightAngleCos = cos(0.7853981852531433);

    if (!pitching && useTarget && !(this->m_movement->m_flags & 0x2200000) && (this->m_movementData.m_flags & 0x1000000)) {
        C3Vector dir;
        dir.x = target.x - position.x;
        dir.y = target.y - position.y;
        dir.z = target.z - position.z;

        if (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z > 2.0f) {
            dir.Normalize();

            if (fabs((dir.y + dir.x) * 0.0f + dir.z) > s_flightAngleCos) {
                this->m_movementData.OnFlightLocal(time, 1);
                CGUnit_C::s_flightStatusTime = time;
                return;
            }
        }
    }

    float speed;

    if (this->m_movementData.m_flags & 0x100) {
        speed = this->m_movementData.m_walkSpeed;
    } else {
        speed = this->m_movementData.m_runSpeed;
    }

    int32_t canMove = (!pitching || !overshoot) && this->CGUnit::CanMove() && this->IsActiveMover() && this->m_unit->m_unitFieldHealth > 0 && !this->CGUnit::IsOnSpline() && !this->AnimSuppressesMovement();

    if (canMove == static_cast<int32_t>(this->m_movement->m_flags & 0x3)) {
        return;
    }

    if (!canMove) {
        this->TrackingStopMove(time);
        return;
    }

    float scale = speed * 0.14285715f;

    if (scale < 1.0f) {
        scale = 1.0f;
    }

    scale *= 4.5f;

    if (CGUnit_C::s_trackingType != 3 || scale * scale <= distSq3) {
        this->TrackingStartMove(time);
    }
}

// OFFSET: 0x005BF210/0x140234850
void CGUnit_C::RenderTargetSelection() const {
    if (!CGGameUI::m_visible) {
        return;
    }

    C3Vector position = const_cast<CGUnit_C*>(this)->GetModelPosition();
    float radius = this->m_selectionRadius;
    CAaBox box;
    box.b.x = position.x - radius;
    box.b.y = position.y - radius;
    box.b.z = position.z - radius;
    box.t.x = position.x + radius;
    box.t.y = position.y + radius;
    box.t.z = position.z + radius;

    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0;
    this->GetSelectionHighlightColor(&color);

    C44Matrix matrix;
    CGObject_C::BuildSelectionRotMatrix(matrix, position);
    g_theGxDevicePtr->RsPush();
    int32_t states = CGObject_C::SetCircleRenderStates();

    if (states) {
        ProjectTex2d(box, color, &matrix, 0.5f, 0x200122, 1, GxPolygonOffset_Default);
    }

    if (this->m_clientFlags & 0x10) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
        uint32_t now = FrameTime::s_curTimeMs;
        uint32_t start = player->m_combatModeTime;
        uint32_t end = start + CGPlayer_C::GetCombatModeFlashDuration();

        if (static_cast<int32_t>(now - start) >= 0 && static_cast<int32_t>(now - end) < 0) {
            float t = static_cast<float>(static_cast<int32_t>(now - start)) / CGPlayer_C::GetCombatModeFlashDuration();
            float shrink = (1.0f - t) * ((1.0f - t) * (1.0f - t));
            float grow = (1.6f * radius + 2.3f - radius) * shrink;
            float fade = 1.0f - shrink;

            if (t > 0.5f) {
                float half = t - 0.5f + t - 0.5f;
                fade = (1.0f - half * half) * fade;
            }

            color.a = static_cast<uint8_t>(fade * color.a);
            C3Vector amount(grow, grow, grow);
            box.Expand(amount);

            if (states) {
                ProjectTex2d(box, color, &matrix, 0.5f, 0x200122, 1, GxPolygonOffset_Default);
            }
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x005BF560/0x140234CD0
void CGUnit_C::RenderPetTargetSelection() const {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    const WOWGUID& petGuid = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(petGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return;
    }

    uint32_t elapsed = FrameTime::s_curTimeMs - pet->m_targetChangeTime;

    if (static_cast<int32_t>(elapsed) < 0 || static_cast<int32_t>(elapsed - 1000) >= 0) {
        return;
    }

    C3Vector position = const_cast<CGUnit_C*>(this)->GetModelPosition();
    float radius = this->m_selectionRadius;
    float t = elapsed * 0.001f;
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0xFF605FFF;
    float size = radius + (1.0f - t) * ((1.0f - t) * (1.0f - t)) * (radius * 1.5f + 2.3f - radius);

    if (t > 0.75f) {
        float x = (t - 0.75f) * 4.0f;
        color.a = static_cast<uint8_t>((1.0f - x * x) * 255.0f);
    } else if (t < 0.25f) {
        float x = t * 4.0f;
        color.a = static_cast<uint8_t>(x * x * 255.0f);
    }

    C3Vector amount(size, size, size + size);
    CAaBox box;
    box.b = position;
    box.t = position;
    box.Expand(amount);

    C44Matrix matrix;
    CGObject_C::BuildSelectionRotMatrix(matrix, position);
    g_theGxDevicePtr->RsPush();

    if (CGObject_C::SetCircleRenderStates()) {
        ProjectTex2d(box, color, &matrix, 0.5f, 0x200122, 1, GxPolygonOffset_Default);
    }

    g_theGxDevicePtr->RsPop();
}

// PROVEN: efa95363 IDENTICAL/NONE
// OFFSET: 0x005ABBC0/0x14021E0E0
void CGUnit_C::OnSpecialMountAnim() {
    this->PlayMountSpecialAnimKit(0xFFFFFFFF);
}

// OFFSET: 0x005B3230/0x1402265F0
const SpellVisualRec* CGUnit_C::GetAppropriateSpellVisual(const SpellRec* spell, SpellVisualRec& visual, int32_t a3, int32_t a4) const {
    int32_t id = a3;
    const SpellVisualRec* weapon = NULL;
    const SpellVisualRec* base;
    const SpellVisualRec* primary = NULL;

    if (a3) {
        if (ViolenceGetLevel() < 2 && a4) {
            id = a4;
        }

        base = g_spellVisualDB.GetRecord(id);
        primary = GetSpellVisual(spell);
    } else {
        base = GetSpellVisual(spell);
    }

    if (spell->m_attributes & 0x2) {
        uint32_t displayId = this->GetVirtualItemDisplayID(2);
        ItemDisplayInfoRec itemRec;

        if (g_itemDisplayInfoDB.GetRecord(displayId, &itemRec) && itemRec.m_spellVisualID) {
            weapon = g_spellVisualDB.GetRecord(itemRec.m_spellVisualID);
        }
    }

    if (base) {
        visual = *base;

        if (primary) {
            visual.m_precastKit = primary->m_precastKit;
            visual.m_castKit = primary->m_castKit;
            visual.m_impactKit = primary->m_impactKit;
            visual.m_casterImpactKit = primary->m_casterImpactKit;
            visual.m_targetImpactKit = primary->m_targetImpactKit;
        }
    } else {
        if (!weapon) {
            return NULL;
        }

        memset(&visual, 0, sizeof(visual));
    }

    if (weapon) {
        if (!visual.m_precastKit) {
            visual.m_precastKit = weapon->m_precastKit;
        }

        if (!visual.m_castKit) {
            visual.m_castKit = weapon->m_castKit;
        }

        if (!visual.m_impactKit) {
            visual.m_impactKit = weapon->m_impactKit;
        }

        if (!visual.m_casterImpactKit) {
            visual.m_casterImpactKit = weapon->m_casterImpactKit;
        }

        if (!visual.m_targetImpactKit) {
            visual.m_targetImpactKit = weapon->m_targetImpactKit;
        }

        if (!visual.m_hasMissile && weapon->m_hasMissile) {
            visual.m_hasMissile = 1;
            visual.m_missileModel = weapon->m_missileModel;
        }

        if (!visual.m_missileAttachment) {
            visual.m_missileAttachment = weapon->m_missileAttachment;
        }

        if (!visual.m_missileDestinationAttachment) {
            visual.m_missileDestinationAttachment = weapon->m_missileDestinationAttachment;
        }

        if (!visual.m_missileFollowGroundHeight) {
            visual.m_missileFollowGroundHeight = weapon->m_missileFollowGroundHeight;
        }

        if (!visual.m_missileFollowGroundDropSpeed) {
            visual.m_missileFollowGroundDropSpeed = weapon->m_missileFollowGroundDropSpeed;
        }

        if (!visual.m_missileFollowGroundApproach) {
            visual.m_missileFollowGroundApproach = weapon->m_missileFollowGroundApproach;
        }

        if (!visual.m_missileFollowGroundFlags) {
            visual.m_missileFollowGroundFlags = weapon->m_missileFollowGroundFlags;
        }

        if (!visual.m_missileMotion) {
            visual.m_missileMotion = weapon->m_missileMotion;
        }

        if (!visual.m_missileSound) {
            visual.m_missileSound = weapon->m_missileSound;
        }

        if (!visual.m_animEventSoundID) {
            visual.m_animEventSoundID = weapon->m_animEventSoundID;
        }
    }

    return &visual;
}

// PROVEN: d07a19c8 IDENTICAL/NONE
// OFFSET: 0x005AD8A0
int32_t Packet_SMSG_PLAYER_VEHICLE_DATA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    uint32_t vehicleId;
    msg->Get(vehicleId);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->SetVehicleRecID(vehicleId);
    }

    return 1;
}

// OFFSET: 0x005B0BF0
void CGUnit_C::MountTransitionSetCharacterTransform(float elapsed) {
    if (SpellVisualsGetMountTransitionHeight(this->m_mountTransition) == 0.0f) {
        this->CGObject_C::Animate(elapsed);
        return;
    }

    C3Vector normal = this->m_movement->GetGroundNormal();

    if (normal.z >= 0.35721236f) {
        float dx = this->m_groundNormal.x - normal.x;
        float dy = this->m_groundNormal.y - normal.y;
        float dz = this->m_groundNormal.z - normal.z;
        float decay = static_cast<float>(pow(0.0018f, elapsed));
        this->m_groundNormal.x = dx * decay + normal.x;
        this->m_groundNormal.y = dy * decay + normal.y;
        this->m_groundNormal.z = dz * decay + normal.z;
    }

    CM2Model* mountModel = this->m_mountTransitionEffect->m_model;

    if (!(mountModel->m_fFlags & 0x1)) {
        mountModel->WaitForLoad(NULL);
    }

    uint32_t flags = mountModel->m_shared->m_data->m_flags;
    CM2Model* model = this->GetObjectModel();

    if (model) {
        model->SetWorldTransform(this->GetPosition(), this->GetDisplayFacing(), this->GetTrueScale(), this->m_groundNormal, flags & 0x3);
    }

    C44Matrix transform = this->GetObjectModel()->m_worldTransform;
    C3Vector jump(0.0f, 0.0f, 0.0f);
    SpellVisualsComputeMountTransitionJumpVec(this->m_mountTransition, &jump);
    transform.d0 = transform.d0 + jump.x;
    transform.d1 = transform.d1 + jump.y;
    transform.d2 = transform.d2 + jump.z;

    CM2Model* objectModel = this->GetObjectModel();
    objectModel->m_fFlags |= 0x8000;
    objectModel->m_worldTransform = transform;

    if (this->m_mountTransitionEffect) {
        this->m_mountTransitionSpellId = this->m_mountTransitionEffect->m_spellId;
    }
}

// OFFSET: 0x005BBD90
void CGUnit_C::AttachPrecastMissileModel(const SpellVisualRec* visual, uint8_t castCount) {
    CM2Model* precast = this->m_precastMissileModel;

    if (precast) {
        if (precast->m_attachParent) {
            precast->DetachFromParent();
        }

        this->m_precastMissileModel->Release();
        this->m_precastMissileModel = NULL;
        this->m_precastCastCount = 0;
    }

    this->m_clientFlags &= ~0x1000000;

    if (!visual || visual->m_missileAttachment == -1) {
        return;
    }

    int32_t raw = visual->m_flags & 0x200;

    if (!this->GetObjectModel() || !this->GetObjectModel()->IsLoaded() || !this->HasAttachmentForEffect(visual->m_missileAttachment, raw)) {
        return;
    }

    SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(visual->m_missileModel);

    if (!effectName || !*effectName->m_fileName) {
        return;
    }

    uint32_t attachId = raw ? visual->m_missileAttachment : s_effectAttachmentIds[visual->m_missileAttachment];
    float attachScale = this->GetObjectModel()->GetAttachmentWorldScale(attachId);
    this->m_precastMissileModel = World::GetScene()->CreateModel(effectName->m_fileName, 32, 3);
    C3Vector position;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    this->m_precastMissileModel->SetWorldTransform(position, -1.5707964f, effectName->m_scale / (attachScale <= 0.0f ? 1.0f : attachScale));
    this->m_precastMissileModel->AttachToParent(this->GetObjectModel(), attachId, NULL, 0);
    this->m_precastCastCount = castCount;
}

// OFFSET: 0x005B50D0
bool CGUnit_C::IsAlteredFormTransitionPreventingAction() const {
    CUnitDisplayTransition_C* transition = this->m_displayTransition;
    return transition && (this->m_clientFlags & 0x400) && (this->m_currentClientRaceFlags & 0x4) && (transition->m_state == 4 || transition->m_state == 5);
}

// OFFSET: 0x005B6210
void CGUnit_C::CancelGrowthAura() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_GROWTH_AURA));
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005C7B00
void CGUnit_C::HandleScaleUpdate(float scale) {
    int32_t growing = scale >= this->GetScale();
    int32_t updated = 0;

    if (this->m_unit->m_unitFieldMountdisplayid <= 0 || this->IsMountSelf(0) || !(this->m_obj->m_type & TYPEMASK_PLAYER)) {
        updated = this->UpdateUnitCollisionBox(growing, 0);
    } else {
        CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldMountdisplayid);

        if (displayRec) {
            CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

            if (modelRec) {
                updated = static_cast<CGPlayer_C*>(this)->UpdateMountedCollisionBox(modelRec->m_mountHeight, growing);
            }
        }
    }

    if (!updated) {
        this->CancelGrowthAura();
    }

    this->UpdateInteractIconScale();
    this->UpdateSelectionRadius();
    this->UpdateEffectAttachments();
}

// OFFSET: #0x0072A7F0
void CGUnit_C::FillPartyMemberPetState(CGPartyPetState* state) {
    state->m_guid = this->m_obj->m_guid;
    SStrCopy(state->m_name, this->GetUnitName(NULL, 1), sizeof(state->m_name));
    state->m_displayId = this->m_unit->m_unitFieldDisplayid;
    state->m_health = this->m_unit->m_unitFieldHealth;
    state->m_maxHealth = this->m_unit->m_unitFieldMaxhealth;
    uint8_t powerType = this->m_unit->m_unitFieldBytes0[3];
    state->m_powerType = powerType;
    state->m_power = this->m_unit->m_unitFieldPower[powerType];
    state->m_maxPower = this->m_unit->m_unitFieldMaxpower[powerType];

    uint32_t count = 0;

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData* aura = this->GetAura(i);

        if (aura->m_spellId && (aura->m_flags & 0x7)) {
            state->m_auras[count] = aura->m_spellId;
            state->m_auraFlags[count] = aura->m_flags;
            count++;

            if (count == 64) {
                return;
            }
        }
    }

    memset(&state->m_auras[count], 0, sizeof(state->m_auras[0]) * (64 - count));
}

// OFFSET: 0x005CFA00
void CGUnit_C::OnReceiveMirrorImageDataFromServer(CDataStore* msg) {
    uint32_t clientFlags = this->m_clientFlags;

    if (!(clientFlags & 0x20000)) {
        return;
    }

    ComponentData data;
    this->m_clientFlags = clientFlags & ~0x20000;

    if (!this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return;
    }

    uint32_t displayId;
    msg->Get(displayId);

    if (displayId != this->m_unit->m_unitFieldDisplayid) {
        msg->m_read = msg->m_size;
        return;
    }

    this->FreeComponent();
    uint8_t value;
    msg->Get(value);
    data.m_preferences.m_raceID = value;

    if (value) {
        msg->Get(value);
        data.m_preferences.m_sexID = value;
        msg->Get(value);
        data.m_preferences.m_classId = value;
        msg->Get(value);
        data.m_preferences.m_skinID = value;
        msg->Get(value);
        data.m_preferences.m_faceId = value;
        msg->Get(value);
        data.m_preferences.m_hairStyleId = value;
        msg->Get(value);
        data.m_preferences.m_hairColorId = value;
        msg->Get(value);
        data.m_preferences.m_facialHairId = value;

        uint64_t guildGuid;
        msg->Get(guildGuid);

        this->m_characterComponent = CCharacterComponent::AllocComponent();
        data.m_model = this->m_worldModel;
        data.m_flags ^= (data.m_flags ^ (static_cast<uint32_t>(this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) << 1)) & 0x2;
        data.m_model->m_refCount++;
        CCharacterComponent::ValidateComponentData(&data, COMPONENT_CONTEXT_1);
        this->m_characterComponent->Init(&data, NULL);

        uint32_t itemDisplayId;

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_HEAD, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_SHOULDER, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_BODY, itemDisplayId, 0);
            ItemDisplayInfoRec itemRec;
            int32_t emblemStyle;
            int32_t emblemColor;
            int32_t borderStyle;
            int32_t borderColor;
            int32_t background;

            if (g_itemDisplayInfoDB.GetRecord(itemDisplayId, &itemRec) && (itemRec.m_flags & 0x19) && GuildGetGuildTabard(this->m_obj->m_guid, WOWGUID::Make(guildGuid), &CGUnit_C::ShirtTabardDrawCallback, reinterpret_cast<void*>(itemRec.m_flags & 0x19), emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                this->m_characterComponent->ApplyShirtGuildColor(itemRec.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
            }
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_CHEST, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_WAIST, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_LEGS, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_FEET, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_WRIST, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_HAND, itemDisplayId, 0);
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_BACK, itemDisplayId, 0);
            ItemDisplayInfoRec itemRec;

            if (g_itemDisplayInfoDB.GetRecord(itemDisplayId, &itemRec) && (itemRec.m_flags & 0x19)) {
                this->LoadGuildTextures(guildGuid);
            }
        }

        msg->Get(itemDisplayId);

        if (itemDisplayId) {
            this->m_characterComponent->AddItemBySlot(INVSLOT_TABARD, itemDisplayId, 0);
            ItemDisplayInfoRec itemRec;
            int32_t emblemStyle;
            int32_t emblemColor;
            int32_t borderStyle;
            int32_t borderColor;
            int32_t background;

            if (g_itemDisplayInfoDB.GetRecord(itemDisplayId, &itemRec) && (itemRec.m_flags & 0x19) && GuildGetGuildTabard(this->m_obj->m_guid, WOWGUID::Make(guildGuid), &CGUnit_C::TabardDrawCallback, reinterpret_cast<void*>(itemRec.m_flags & 0x19), emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                this->m_characterComponent->ApplyGuildColor(itemRec.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
            }
        }
    } else if (this->m_displayInfoExtra) {
        this->m_characterComponent = CCharacterComponent::AllocComponent();
        const char* bakeName = this->GetDisplayBakeName();

        if (!bakeName || !*bakeName) {
            return;
        }

        CreatureDisplayInfoExtraRec* extra = this->m_displayInfoExtra;
        data.m_preferences.m_raceID = extra->m_displayRaceID;
        data.m_preferences.m_sexID = extra->m_displaySexID;
        data.m_preferences.m_classId = 0;
        data.m_preferences.m_skinID = extra->m_skinID;
        data.m_preferences.m_faceId = extra->m_faceID;
        data.m_preferences.m_hairStyleId = extra->m_hairStyleID;
        data.m_preferences.m_hairColorId = extra->m_hairColorID;
        data.m_flags |= 0x1;
        data.m_preferences.m_facialHairId = extra->m_facialHairID;
        SStrPrintf(data.m_npcSkinTexture, sizeof(data.m_npcSkinTexture), "%s%s", "Textures\\BakedNpcTextures\\", bakeName);
        data.m_model = this->m_worldModel;
        data.m_model->m_refCount++;
        this->m_characterComponent->Init(&data, NULL);

        if (this->m_characterComponent && this->m_displayInfoExtra) {
            for (uint32_t i = 0; i < 11; i++) {
                int32_t itemDisplay = this->m_displayInfoExtra->m_npcitemDisplay[i];

                if (itemDisplay) {
                    this->m_characterComponent->AddItem(static_cast<ITEM_SLOT>(i), itemDisplay, 0);
                }
            }
        }
    }

    this->AddHandItem(0);
    this->AddHandItem(1);
    this->AddHandItem(2);
    this->ApplyComponentItemsFromEffects();
    WOWGUID guid = this->m_obj->m_guid;
    CGGameUI::UnitModelUpdate(guid, 3);
    this->m_clientFlags &= ~0x400000;
}

// PROVEN: 6eb97f16 IDENTICAL/NONE
// OFFSET: 0x005D3280
CM2Model* CGUnit_C::CreateItemModel(uint32_t hand, float* scale) {
    const CGUnitVirtualItem* item = this->GetVirtualItem(hand, true);

    if (!item) {
        return NULL;
    }

    return this->CreateItemModel(this->GetVirtualItemDisplayID(hand), item->m_inventoryType, scale);
}

// PROVEN: cc173495 IDENTICAL/NONE
// OFFSET: 0x005D34A0
int32_t Packet_SMSG_MIRRORIMAGE_DATA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->OnReceiveMirrorImageDataFromServer(msg);
    }

    return 1;
}

// OFFSET: 0x005D34F0
bool ClientPlaySpellVisualHandler(ClientPlaySpellVisual* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return true;
    }

    const SpellVisualRec* visual = g_spellVisualDB.GetRecord(msg->m_spellVisualId);

    if (!CanPlaySpellVisual(msg->m_guid.m_guid, 0, visual)) {
        return true;
    }

    if (visual->m_castKit) {
        const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_castKit);

        if (kit && CanPlaySpellVisualKit(msg->m_guid.m_guid, msg->m_targetGuid.m_guid, kit, NULL)) {
            PlaySpellVisuaKitData data(NULL, kit, 0);
            unit->PlaySpellVisualKit(data);
        }
    }

    if (!visual->m_hasMissile || msg->m_travelSpeed <= 0.0f) {
        return true;
    }

    C3Vector position = msg->m_targetPosition;
    C3Vector normal(0.0f, 0.0f, 1.0f);

    if (msg->m_targetGuid) {
        CGObject_C* target = ClntObjMgrObjectPtr(msg->m_targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__);

        if (!target) {
            return true;
        }

        position = target->GetPosition();
    } else if (visual->m_flags & 0x400) {
        C3Vector ground;
        position = *MissileFindGroundPosition(&ground, position, normal);
    }

    float speed = 0.0f;
    int32_t time = 0;
    unit->CalcMissileTravel(position, msg->m_travelSpeed, msg->m_speedAsTime, &speed, &time);
    CreateMissileParams params;
    memset(&params, 0, sizeof(params));
    SpellCastTargetPoint point;
    TSStackArray<WOWGUID> targets(&msg->m_targetGuid, 1, 0);
    params.m_visual = visual;
    params.m_speed = speed;

    if (msg->m_targetGuid) {
        params.m_target = msg->m_targetGuid;
        params.m_targets = &targets;
        params.m_hit = 1;
        params.m_missReason = msg->m_reflectStatus;
        params.m_reflectResult = msg->m_missReason;
    } else {
        params.m_flags |= 1;
        point.m_position = position;
        point.m_worldPosition = position;
        point.unk_0024 = normal;
        point.m_transport = WOWGUID();
        params.m_point = &point;
    }

    unit->CreateMissile(params);
    return true;
}

// OFFSET: 0x005D0320
void CGUnit_C::OnFlag2Changed(uint32_t oldFlags) {
    uint32_t flags = this->m_unit->m_unitFieldFlags2;
    uint32_t changed = oldFlags ^ flags;

    if (changed & 0x1) {
        if (flags & 0x1) {
            if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
                CGInputControl::GetActive()->UpdatePlayer(FrameTime::GetCurTimeMs(), 0);
            }

            this->CheckLoopSound();
            this->PlayUnitEventSound(4);
            this->PlayDeathAnim(1);
        } else {
            this->CheckLoopSound();
            this->UpdateDeathAnimKit();
        }
    }

    if ((changed & 0x8) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGChat::UpdateLanguages();
    }

    if ((changed & 0x10) && !(flags & 0x10)) {
        this->UpdateDisplayInfo(1);
    }

    if (changed & 0x80) {
        const CGUnitVirtualItem* offHand = this->GetVirtualItem(1, true);

        if (offHand) {
            bool hide = false;

            if ((this->m_unit->m_unitFieldFlags & 0x200000)) {
                this->GetVirtualItem(0, true);

                if (!this->IsDisarmed(0)) {
                    const CGUnitVirtualItem* item = this->GetVirtualItem(1, true);

                    if (item && item->m_classID == 2) {
                        hide = true;
                    }
                }
            }

            if (!hide && (this->m_unit->m_unitFieldFlags2 & 0x80)) {
                hide = true;
            }

            if (hide && this->m_sheatheState <= 1) {
                this->RemoveHandItemModel(INVSLOT_OFFHAND, this->m_sheatheState ? 0 : offHand->m_sheatheType, offHand->m_inventoryType == 14);
            } else {
                this->AddHandItem(1);
            }
        }

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGSpellBook::UpdateUsable();
        }
    }

    if (changed & 0x400) {
        if ((this->m_unit->m_unitFieldFlags2 & 0x400) && this->m_sheatheState == 2) {
            const CGUnitVirtualItem* ranged = this->GetVirtualItem(2, true);

            if (ranged) {
                if (ranged->m_inventoryType == 26 || ranged->m_inventoryType == 25) {
                    this->m_worldModel->DetachAllChildrenById(1);
                } else {
                    this->m_worldModel->DetachAllChildrenById(2);
                }
            }
        } else {
            this->AddHandItem(2);
        }

        if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGSpellBook::UpdateUsable();
        }
    }

    if ((changed & 0x40) && this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        CGInputControl::GetActive()->UpdatePlayer(FrameTime::GetCurTimeMs(), 1);
    }

    if (changed & 0x10000000) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if ((this->m_unit->m_unitFieldFlags2 & 0x10000000) && player && !player->IsFriendly(this)) {
            CGGameUI::BreakTarget(this->m_obj->m_guid);
            Script_SendUniqueSignal(this->m_obj->m_guid, 146);
        }
    }

    if (changed & 0x80000) {
        if ((this->m_modelData->m_flags & 0x40) || (this->m_unit->m_unitFieldFlags2 & 0x80000)) {
            this->m_clientFlags |= 0x2000000;
        } else {
            this->m_clientFlags &= ~0x2000000;
        }

        CGGameUI::UpdateUnitHighlights(1);
    }
}

// PROVEN: 23efb777 IDENTICAL/NONE
// OFFSET: 0x005B01B0
void CGUnit_C::UpdateEffectVirtualItemFlag() {
    this->unk_09FC &= ~0x400;

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if ((effect->m_flags & 0x80000) && effect->m_data.m_virtualItem) {
            this->unk_09FC |= 0x400;
            return;
        }
    }
}
