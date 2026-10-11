#include "Spell_C.h"
#include "WoW/Source/Magic/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"
#include "WoW/Source/DB/Rec/SpellPowerRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/UnitPowerBarRec.h"
#include "WoW/Source/DB/Rec/SpellDifficultyRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "WoW/Source/DB/Rec/SpellMissileRec.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/DB/Rec/SpellCategoryRec.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include <cmath>
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"
#include "WoW/Source/DB/Rec/SpellDurationRec.h"
#include "WoW/Source/DB/Rec/SpellRangeRec.h"
#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/Corpse.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/DB/Rec/SpellRadiusRec.h"
#include "WoW/Source/Magic/Spell.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"
#include "WoW/Source/DB/Rec/PowerDisplayRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"
#include "WoW/Source/DB/Rec/SpellMechanicRec.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/GlyphSlotRec.h"
#include "SpellWorldObject.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"
#include "WoW/Source/DB/Rec/SpellInterruptsRec.h"
#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"
#include "WoW/Source/DB/Rec/SpellTotemsRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/SpellLevelsRec.h"
#include "WoW/Source/DB/Rec/SpellScalingRec.h"
#include "WoW/Source/DB/Rec/SpellAuraRestrictionsRec.h"
#include "WoW/Source/DB/Rec/SpellAuraVisibilityRec.h"
#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#if defined(_WIN32)
#include <malloc.h>
#else
#include <alloca.h>
#endif

// STATIC: 0x00EC7620
static int32_t s_maxStartRecoveryOffset;

// STATIC: 0x00EC7624
static CVar* s_enableErrorSpeechCVar;

// STATIC: 0x00EC7628
static int32_t s_spellClassSet;

// STATIC: 0x00EC762C
static int32_t s_feedPetSpell;

// STATIC: 0x00EC7630
static uint32_t s_lastFailedTime;

// STATIC: 0x00EC7634
static int32_t s_lastFailedReason;

// STATIC: 0x00EC7638
static uint32_t s_lastFailedSpellId;

// STATIC: 0x00EC763C
static int32_t s_lastAutoRangedCombatError;

// STATIC: 0x00EC7640
static int32_t s_autoRangedCombatSpell;

// STATIC: 0x00EC7644
static int32_t s_autoRepeatingSpell;

// STATIC: 0x00EC7648
static float s_spellPctModifiers[96][32];

// STATIC: 0x00ECA648
static float s_spellFlatModifiers[96][32];

// STATIC: 0x00ECD648
uint32_t s_needTargets;

// STATIC: 0x00ECD64C
static uint8_t s_nextCastCount;

// STATIC: 0x00ECD650
static SpellWorldObject* s_cursorWorldObject;

// STATIC: 0x00ECD654
PendingSpellCast* s_targetingSpell;

// STATIC: 0x00ECD658
static PendingSpellCast* s_castingSpellCast;

// STATIC: 0x00ECD65C
static PendingSpellCast* s_delayedTrajectoryCast;

// STATIC: 0x00ECD660
static uint32_t s_missileTrajectorySequenceIndex;

// STATIC: 0x00ECD668
static char s_targetString[128];

// STATIC: 0x00ECD6E8
static TSHashTable<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE> s_itemCooldowns;

// STATIC: 0x00ECD714
static SpellHistory s_spellHistory[2];

// STATIC: 0x00ECD744
static TSHashTable<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID> s_destLocSpellCasts;

// STATIC: 0x00ECD770
static uint32_t s_numPendingSpellCasts;

// STATIC: 0x00ECD778
static PendingSpellCast s_pendingSpellCasts[100];

// STATIC: 0x00D0ADD4
static uint32_t s_powerDisplayMod[11] = { 1, 10, 1, 1, 1, 1, 10, 1, 1, 1, 1 };

// STATIC: 0x00D0AE7C
static TSList<PeriodicClientTrigger, TSGetLink<PeriodicClientTrigger>> s_periodicClientTriggers;

// STATIC: 0x00D0AE98
static TSList<WEEKLYSPELLUSE, TSGetLink<WEEKLYSPELLUSE>> s_weeklySpellUses;

// STATIC: 0x00D0AEA8
static TSList<SpellInTransit, TSGetLink<SpellInTransit>> s_spellsInTransit;

// STATIC: 0x00D0AE88
static ThrottleObject s_castSpellThrottle(30, 1);

// STATIC: 0x00D0AFF4
static uint8_t s_meleeTutorialActive = 1;

// STATIC: 0x00ED6738
static uint32_t s_meleeTutorialCount;

// STATIC: 0x00ED673C
static uint32_t s_nextHistoryCleanup;

// STATIC: 0x0100A3E0
static const CSoftFloat s_softPercent(0x3C23D70A);

// STATIC: 0x00BE0EA8
static const UIErrors s_powerErrors[] = { ERR_OUT_OF_MANA, ERR_OUT_OF_RAGE, ERR_OUT_OF_FOCUS, ERR_OUT_OF_ENERGY, ERR_MAX_VALUE, ERR_OUT_OF_RUNES, ERR_OUT_OF_RUNIC_POWER, ERR_OUT_OF_SOUL_SHARDS, ERR_OUT_OF_BALANCE_POSITIVE, ERR_OUT_OF_HOLY_POWER, ERR_OUT_OF_MANA };

// STATIC: 0x00BE0ED4
static const UIErrors s_eclipseErrors[] = { ERR_OUT_OF_BALANCE_POSITIVE, ERR_OUT_OF_BALANCE_NEGATIVE };

// INLINED
uint32_t SetTrajectoryCheckBit(uint32_t value, uint32_t shift) {
    uint32_t masked = value & ~(1u << shift);
    uint32_t fold = masked ^ (masked >> 16);
    fold ^= fold >> 8;
    fold = (fold ^ (fold >> 4)) & 0xF;
    return masked | (((0x28D7 >> fold) & 1) << shift);
}

// OFFSET: 0x008A47A0
bool Spell_C_IsRestrictedAbility(const SpellRec* spell) {
    if (!spell) {
        return false;
    }

    return (spell->m_attributes & 0x404) || (spell->m_attributesEx & 0x200) || (spell->m_attributesExB & 0x100000);
}

// OFFSET: 0x008A47D0
bool SpellIsWeaponDamageEffect(int32_t effect) {
    return effect == 58 || effect == 121 || effect == 17 || effect == 31;
}

// OFFSET: 0x008A47F0/0x1405E7150
const char* GetSpellFailedResult(int32_t reason) {
    switch (reason) {
        case SPELL_FAILED_SUCCESS:
            return "SPELL_FAILED_SUCCESS";

        case SPELL_FAILED_AFFECTING_COMBAT:
            return "SPELL_FAILED_AFFECTING_COMBAT";

        case SPELL_FAILED_ALREADY_AT_FULL_HEALTH:
            return "SPELL_FAILED_ALREADY_AT_FULL_HEALTH";

        case SPELL_FAILED_ALREADY_AT_FULL_MANA:
            return "SPELL_FAILED_ALREADY_AT_FULL_MANA";

        case SPELL_FAILED_ALREADY_AT_FULL_POWER:
            return "SPELL_FAILED_ALREADY_AT_FULL_POWER";

        case SPELL_FAILED_ALREADY_BEING_TAMED:
            return "SPELL_FAILED_ALREADY_BEING_TAMED";

        case SPELL_FAILED_ALREADY_HAVE_CHARM:
            return "SPELL_FAILED_ALREADY_HAVE_CHARM";

        case SPELL_FAILED_ALREADY_HAVE_SUMMON:
            return "SPELL_FAILED_ALREADY_HAVE_SUMMON";

        case SPELL_FAILED_ALREADY_HAVE_PET:
            return "SPELL_FAILED_ALREADY_HAVE_PET";

        case SPELL_FAILED_ALREADY_OPEN:
            return "SPELL_FAILED_ALREADY_OPEN";

        case SPELL_FAILED_AURA_BOUNCED:
            return "SPELL_FAILED_AURA_BOUNCED";

        case SPELL_FAILED_AUTOTRACK_INTERRUPTED:
            return "SPELL_FAILED_AUTOTRACK_INTERRUPTED";

        case SPELL_FAILED_BAD_IMPLICIT_TARGETS:
            return "SPELL_FAILED_BAD_IMPLICIT_TARGETS";

        case SPELL_FAILED_BAD_TARGETS:
            return "SPELL_FAILED_BAD_TARGETS";

        case SPELL_FAILED_CANT_BE_CHARMED:
            return "SPELL_FAILED_CANT_BE_CHARMED";

        case SPELL_FAILED_CANT_BE_DISENCHANTED:
            return "SPELL_FAILED_CANT_BE_DISENCHANTED";

        case SPELL_FAILED_CANT_BE_DISENCHANTED_SKILL:
            return "SPELL_FAILED_CANT_BE_DISENCHANTED_SKILL";

        case SPELL_FAILED_CANT_BE_MILLED:
            return "SPELL_FAILED_CANT_BE_MILLED";

        case SPELL_FAILED_CANT_BE_PROSPECTED:
            return "SPELL_FAILED_CANT_BE_PROSPECTED";

        case SPELL_FAILED_CANT_CAST_ON_TAPPED:
            return "SPELL_FAILED_CANT_CAST_ON_TAPPED";

        case SPELL_FAILED_CANT_DUEL_WHILE_INVISIBLE:
            return "SPELL_FAILED_CANT_DUEL_WHILE_INVISIBLE";

        case SPELL_FAILED_CANT_DUEL_WHILE_STEALTHED:
            return "SPELL_FAILED_CANT_DUEL_WHILE_STEALTHED";

        case SPELL_FAILED_CANT_STEALTH:
            return "SPELL_FAILED_CANT_STEALTH";

        case SPELL_FAILED_CASTER_AURASTATE:
            return "SPELL_FAILED_CASTER_AURASTATE";

        case SPELL_FAILED_CASTER_DEAD:
            return "SPELL_FAILED_CASTER_DEAD";

        case SPELL_FAILED_CHARMED:
            return "SPELL_FAILED_CHARMED";

        case SPELL_FAILED_CHEST_IN_USE:
            return "SPELL_FAILED_CHEST_IN_USE";

        case SPELL_FAILED_CONFUSED:
            return "SPELL_FAILED_CONFUSED";

        case SPELL_FAILED_DONT_REPORT:
            return "SPELL_FAILED_DONT_REPORT";

        case SPELL_FAILED_EQUIPPED_ITEM:
            return "SPELL_FAILED_EQUIPPED_ITEM";

        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
            return "SPELL_FAILED_EQUIPPED_ITEM_CLASS";

        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
            return "SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND";

        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
            return "SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND";

        case SPELL_FAILED_ERROR:
            return "SPELL_FAILED_ERROR";

        case SPELL_FAILED_FALLING:
            return "SPELL_FAILED_FALLING";

        case SPELL_FAILED_FIZZLE:
            return "SPELL_FAILED_FIZZLE";

        case SPELL_FAILED_FLEEING:
            return "SPELL_FAILED_FLEEING";

        case SPELL_FAILED_FOOD_LOWLEVEL:
            return "SPELL_FAILED_FOOD_LOWLEVEL";

        case SPELL_FAILED_HIGHLEVEL:
            return "SPELL_FAILED_HIGHLEVEL";

        case SPELL_FAILED_HUNGER_SATIATED:
            return "SPELL_FAILED_HUNGER_SATIATED";

        case SPELL_FAILED_IMMUNE:
            return "SPELL_FAILED_IMMUNE";

        case SPELL_FAILED_INCORRECT_AREA:
            return "SPELL_FAILED_INCORRECT_AREA";

        case SPELL_FAILED_INTERRUPTED:
            return "SPELL_FAILED_INTERRUPTED";

        case SPELL_FAILED_INTERRUPTED_COMBAT:
            return "SPELL_FAILED_INTERRUPTED_COMBAT";

        case SPELL_FAILED_ITEM_ALREADY_ENCHANTED:
            return "SPELL_FAILED_ITEM_ALREADY_ENCHANTED";

        case SPELL_FAILED_ITEM_GONE:
            return "SPELL_FAILED_ITEM_GONE";

        case SPELL_FAILED_ITEM_NOT_FOUND:
            return "SPELL_FAILED_ITEM_NOT_FOUND";

        case SPELL_FAILED_ITEM_NOT_READY:
            return "SPELL_FAILED_ITEM_NOT_READY";

        case SPELL_FAILED_LEVEL_REQUIREMENT:
            return "SPELL_FAILED_LEVEL_REQUIREMENT";

        case SPELL_FAILED_LINE_OF_SIGHT:
            return "SPELL_FAILED_LINE_OF_SIGHT";

        case SPELL_FAILED_LOWLEVEL:
            return "SPELL_FAILED_LOWLEVEL";

        case SPELL_FAILED_LOW_CASTLEVEL:
            return "SPELL_FAILED_LOW_CASTLEVEL";

        case SPELL_FAILED_MAINHAND_EMPTY:
            return "SPELL_FAILED_MAINHAND_EMPTY";

        case SPELL_FAILED_MOVING:
            return "SPELL_FAILED_MOVING";

        case SPELL_FAILED_NEED_AMMO:
            return "SPELL_FAILED_NEED_AMMO";

        case SPELL_FAILED_NEED_AMMO_POUCH:
            return "SPELL_FAILED_NEED_AMMO_POUCH";

        case SPELL_FAILED_NEED_EXOTIC_AMMO:
            return "SPELL_FAILED_NEED_EXOTIC_AMMO";

        case SPELL_FAILED_NEED_MORE_ITEMS:
            return "SPELL_FAILED_NEED_MORE_ITEMS";

        case SPELL_FAILED_NOPATH:
            return "SPELL_FAILED_NOPATH";

        case SPELL_FAILED_NOT_BEHIND:
            return "SPELL_FAILED_NOT_BEHIND";

        case SPELL_FAILED_NOT_FISHABLE:
            return "SPELL_FAILED_NOT_FISHABLE";

        case SPELL_FAILED_NOT_FLYING:
            return "SPELL_FAILED_NOT_FLYING";

        case SPELL_FAILED_NOT_HERE:
            return "SPELL_FAILED_NOT_HERE";

        case SPELL_FAILED_NOT_INFRONT:
            return "SPELL_FAILED_NOT_INFRONT";

        case SPELL_FAILED_NOT_IN_CONTROL:
            return "SPELL_FAILED_NOT_IN_CONTROL";

        case SPELL_FAILED_NOT_KNOWN:
            return "SPELL_FAILED_NOT_KNOWN";

        case SPELL_FAILED_NOT_MOUNTED:
            return "SPELL_FAILED_NOT_MOUNTED";

        case SPELL_FAILED_NOT_ON_TAXI:
            return "SPELL_FAILED_NOT_ON_TAXI";

        case SPELL_FAILED_NOT_ON_TRANSPORT:
            return "SPELL_FAILED_NOT_ON_TRANSPORT";

        case SPELL_FAILED_NOT_READY:
            return "SPELL_FAILED_NOT_READY";

        case SPELL_FAILED_NOT_SHAPESHIFT:
            return "SPELL_FAILED_NOT_SHAPESHIFT";

        case SPELL_FAILED_NOT_STANDING:
            return "SPELL_FAILED_NOT_STANDING";

        case SPELL_FAILED_NOT_TRADEABLE:
            return "SPELL_FAILED_NOT_TRADEABLE";

        case SPELL_FAILED_NOT_TRADING:
            return "SPELL_FAILED_NOT_TRADING";

        case SPELL_FAILED_NOT_UNSHEATHED:
            return "SPELL_FAILED_NOT_UNSHEATHED";

        case SPELL_FAILED_NOT_WHILE_GHOST:
            return "SPELL_FAILED_NOT_WHILE_GHOST";

        case SPELL_FAILED_NOT_WHILE_LOOTING:
            return "SPELL_FAILED_NOT_WHILE_LOOTING";

        case SPELL_FAILED_NO_AMMO:
            return "SPELL_FAILED_NO_AMMO";

        case SPELL_FAILED_NO_CHARGES_REMAIN:
            return "SPELL_FAILED_NO_CHARGES_REMAIN";

        case SPELL_FAILED_NO_CHAMPION:
            return "SPELL_FAILED_NO_CHAMPION";

        case SPELL_FAILED_NO_COMBO_POINTS:
            return "SPELL_FAILED_NO_COMBO_POINTS";

        case SPELL_FAILED_NO_DUELING:
            return "SPELL_FAILED_NO_DUELING";

        case SPELL_FAILED_NO_ENDURANCE:
            return "SPELL_FAILED_NO_ENDURANCE";

        case SPELL_FAILED_NO_FISH:
            return "SPELL_FAILED_NO_FISH";

        case SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED:
            return "SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED";

        case SPELL_FAILED_NO_MOUNTS_ALLOWED:
            return "SPELL_FAILED_NO_MOUNTS_ALLOWED";

        case SPELL_FAILED_NO_PET:
            return "SPELL_FAILED_NO_PET";

        case SPELL_FAILED_NO_POWER:
            return "SPELL_FAILED_NO_POWER";

        case SPELL_FAILED_NOTHING_TO_DISPEL:
            return "SPELL_FAILED_NOTHING_TO_DISPEL";

        case SPELL_FAILED_NOTHING_TO_STEAL:
            return "SPELL_FAILED_NOTHING_TO_STEAL";

        case SPELL_FAILED_ONLY_ABOVEWATER:
            return "SPELL_FAILED_ONLY_ABOVEWATER";

        case SPELL_FAILED_ONLY_DAYTIME:
            return "SPELL_FAILED_ONLY_DAYTIME";

        case SPELL_FAILED_ONLY_INDOORS:
            return "SPELL_FAILED_ONLY_INDOORS";

        case SPELL_FAILED_ONLY_MOUNTED:
            return "SPELL_FAILED_ONLY_MOUNTED";

        case SPELL_FAILED_ONLY_NIGHTTIME:
            return "SPELL_FAILED_ONLY_NIGHTTIME";

        case SPELL_FAILED_ONLY_OUTDOORS:
            return "SPELL_FAILED_ONLY_OUTDOORS";

        case SPELL_FAILED_ONLY_SHAPESHIFT:
            return "SPELL_FAILED_ONLY_SHAPESHIFT";

        case SPELL_FAILED_ONLY_STEALTHED:
            return "SPELL_FAILED_ONLY_STEALTHED";

        case SPELL_FAILED_ONLY_UNDERWATER:
            return "SPELL_FAILED_ONLY_UNDERWATER";

        case SPELL_FAILED_OUT_OF_RANGE:
            return "SPELL_FAILED_OUT_OF_RANGE";

        case SPELL_FAILED_PACIFIED:
            return "SPELL_FAILED_PACIFIED";

        case SPELL_FAILED_POSSESSED:
            return "SPELL_FAILED_POSSESSED";

        case SPELL_FAILED_REAGENTS:
            return "SPELL_FAILED_REAGENTS";

        case SPELL_FAILED_REQUIRES_AREA:
            return "SPELL_FAILED_REQUIRES_AREA";

        case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
            return "SPELL_FAILED_REQUIRES_SPELL_FOCUS";

        case SPELL_FAILED_ROOTED:
            return "SPELL_FAILED_ROOTED";

        case SPELL_FAILED_SILENCED:
            return "SPELL_FAILED_SILENCED";

        case SPELL_FAILED_SPELL_IN_PROGRESS:
            return "SPELL_FAILED_SPELL_IN_PROGRESS";

        case SPELL_FAILED_SPELL_LEARNED:
            return "SPELL_FAILED_SPELL_LEARNED";

        case SPELL_FAILED_SPELL_UNAVAILABLE:
            return "SPELL_FAILED_SPELL_UNAVAILABLE";

        case SPELL_FAILED_STUNNED:
            return "SPELL_FAILED_STUNNED";

        case SPELL_FAILED_TARGETS_DEAD:
            return "SPELL_FAILED_TARGETS_DEAD";

        case SPELL_FAILED_TARGET_AFFECTING_COMBAT:
            return "SPELL_FAILED_TARGET_AFFECTING_COMBAT";

        case SPELL_FAILED_TARGET_AURASTATE:
            return "SPELL_FAILED_TARGET_AURASTATE";

        case SPELL_FAILED_TARGET_DUELING:
            return "SPELL_FAILED_TARGET_DUELING";

        case SPELL_FAILED_TARGET_ENEMY:
            return "SPELL_FAILED_TARGET_ENEMY";

        case SPELL_FAILED_TARGET_ENRAGED:
            return "SPELL_FAILED_TARGET_ENRAGED";

        case SPELL_FAILED_TARGET_FRIENDLY:
            return "SPELL_FAILED_TARGET_FRIENDLY";

        case SPELL_FAILED_TARGET_IN_COMBAT:
            return "SPELL_FAILED_TARGET_IN_COMBAT";

        case SPELL_FAILED_TARGET_IS_PLAYER:
            return "SPELL_FAILED_TARGET_IS_PLAYER";

        case SPELL_FAILED_TARGET_IS_PLAYER_CONTROLLED:
            return "SPELL_FAILED_TARGET_IS_PLAYER_CONTROLLED";

        case SPELL_FAILED_TARGET_NOT_DEAD:
            return "SPELL_FAILED_TARGET_NOT_DEAD";

        case SPELL_FAILED_TARGET_NOT_IN_PARTY:
            return "SPELL_FAILED_TARGET_NOT_IN_PARTY";

        case SPELL_FAILED_TARGET_NOT_LOOTED:
            return "SPELL_FAILED_TARGET_NOT_LOOTED";

        case SPELL_FAILED_TARGET_NOT_PLAYER:
            return "SPELL_FAILED_TARGET_NOT_PLAYER";

        case SPELL_FAILED_TARGET_NO_POCKETS:
            return "SPELL_FAILED_TARGET_NO_POCKETS";

        case SPELL_FAILED_TARGET_NO_WEAPONS:
            return "SPELL_FAILED_TARGET_NO_WEAPONS";

        case SPELL_FAILED_TARGET_NO_RANGED_WEAPONS:
            return "SPELL_FAILED_TARGET_NO_RANGED_WEAPONS";

        case SPELL_FAILED_TARGET_UNSKINNABLE:
            return "SPELL_FAILED_TARGET_UNSKINNABLE";

        case SPELL_FAILED_THIRST_SATIATED:
            return "SPELL_FAILED_THIRST_SATIATED";

        case SPELL_FAILED_TOO_CLOSE:
            return "SPELL_FAILED_TOO_CLOSE";

        case SPELL_FAILED_TOO_MANY_OF_ITEM:
            return "SPELL_FAILED_TOO_MANY_OF_ITEM";

        case SPELL_FAILED_TOTEM_CATEGORY:
            return "SPELL_FAILED_TOTEM_CATEGORY";

        case SPELL_FAILED_TOTEMS:
            return "SPELL_FAILED_TOTEMS";

        case SPELL_FAILED_TRY_AGAIN:
            return "SPELL_FAILED_TRY_AGAIN";

        case SPELL_FAILED_UNIT_NOT_BEHIND:
            return "SPELL_FAILED_UNIT_NOT_BEHIND";

        case SPELL_FAILED_UNIT_NOT_INFRONT:
            return "SPELL_FAILED_UNIT_NOT_INFRONT";

        case SPELL_FAILED_VISION_OBSCURED:
            return "SPELL_FAILED_VISION_OBSCURED";

        case SPELL_FAILED_WRONG_PET_FOOD:
            return "SPELL_FAILED_WRONG_PET_FOOD";

        case SPELL_FAILED_NOT_WHILE_FATIGUED:
            return "SPELL_FAILED_NOT_WHILE_FATIGUED";

        case SPELL_FAILED_TARGET_NOT_IN_INSTANCE:
            return "SPELL_FAILED_TARGET_NOT_IN_INSTANCE";

        case SPELL_FAILED_NOT_WHILE_TRADING:
            return "SPELL_FAILED_NOT_WHILE_TRADING";

        case SPELL_FAILED_TARGET_NOT_IN_RAID:
            return "SPELL_FAILED_TARGET_NOT_IN_RAID";

        case SPELL_FAILED_TARGET_FREEFORALL:
            return "SPELL_FAILED_TARGET_FREEFORALL";

        case SPELL_FAILED_NO_EDIBLE_CORPSES:
            return "SPELL_FAILED_NO_EDIBLE_CORPSES";

        case SPELL_FAILED_ONLY_BATTLEGROUNDS:
            return "SPELL_FAILED_ONLY_BATTLEGROUNDS";

        case SPELL_FAILED_TARGET_NOT_GHOST:
            return "SPELL_FAILED_TARGET_NOT_GHOST";

        case SPELL_FAILED_TRANSFORM_UNUSABLE:
            return "SPELL_FAILED_TRANSFORM_UNUSABLE";

        case SPELL_FAILED_WRONG_WEATHER:
            return "SPELL_FAILED_WRONG_WEATHER";

        case SPELL_FAILED_DAMAGE_IMMUNE:
            return "SPELL_FAILED_DAMAGE_IMMUNE";

        case SPELL_FAILED_PREVENTED_BY_MECHANIC:
            return "SPELL_FAILED_PREVENTED_BY_MECHANIC";

        case SPELL_FAILED_PLAY_TIME:
            return "SPELL_FAILED_PLAY_TIME";

        case SPELL_FAILED_REPUTATION:
            return "SPELL_FAILED_REPUTATION";

        case SPELL_FAILED_MIN_SKILL:
            return "SPELL_FAILED_MIN_SKILL";

        case SPELL_FAILED_NOT_IN_RATED_BATTLEGROUND:
            return "SPELL_FAILED_NOT_IN_RATED_BATTLEGROUND";

        case SPELL_FAILED_NOT_ON_SHAPESHIFT:
            return "SPELL_FAILED_NOT_ON_SHAPESHIFT";

        case SPELL_FAILED_NOT_ON_STEALTHED:
            return "SPELL_FAILED_NOT_ON_STEALTHED";

        case SPELL_FAILED_NOT_ON_DAMAGE_IMMUNE:
            return "SPELL_FAILED_NOT_ON_DAMAGE_IMMUNE";

        case SPELL_FAILED_NOT_ON_MOUNTED:
            return "SPELL_FAILED_NOT_ON_MOUNTED";

        case SPELL_FAILED_TOO_SHALLOW:
            return "SPELL_FAILED_TOO_SHALLOW";

        case SPELL_FAILED_TARGET_NOT_IN_SANCTUARY:
            return "SPELL_FAILED_TARGET_NOT_IN_SANCTUARY";

        case SPELL_FAILED_TARGET_IS_TRIVIAL:
            return "SPELL_FAILED_TARGET_IS_TRIVIAL";

        case SPELL_FAILED_BM_OR_INVISGOD:
            return "SPELL_FAILED_BM_OR_INVISGOD";

        case SPELL_FAILED_EXPERT_RIDING_REQUIREMENT:
            return "SPELL_FAILED_EXPERT_RIDING_REQUIREMENT";

        case SPELL_FAILED_ARTISAN_RIDING_REQUIREMENT:
            return "SPELL_FAILED_ARTISAN_RIDING_REQUIREMENT";

        case SPELL_FAILED_NOT_IDLE:
            return "SPELL_FAILED_NOT_IDLE";

        case SPELL_FAILED_NOT_INACTIVE:
            return "SPELL_FAILED_NOT_INACTIVE";

        case SPELL_FAILED_PARTIAL_PLAYTIME:
            return "SPELL_FAILED_PARTIAL_PLAYTIME";

        case SPELL_FAILED_NO_PLAYTIME:
            return "SPELL_FAILED_NO_PLAYTIME";

        case SPELL_FAILED_NOT_IN_BATTLEGROUND:
            return "SPELL_FAILED_NOT_IN_BATTLEGROUND";

        case SPELL_FAILED_NOT_IN_RAID_INSTANCE:
            return "SPELL_FAILED_NOT_IN_RAID_INSTANCE";

        case SPELL_FAILED_ONLY_IN_ARENA:
            return "SPELL_FAILED_ONLY_IN_ARENA";

        case SPELL_FAILED_TARGET_LOCKED_TO_RAID_INSTANCE:
            return "SPELL_FAILED_TARGET_LOCKED_TO_RAID_INSTANCE";

        case SPELL_FAILED_ON_USE_ENCHANT:
            return "SPELL_FAILED_ON_USE_ENCHANT";

        case SPELL_FAILED_NOT_ON_GROUND:
            return "SPELL_FAILED_NOT_ON_GROUND";

        case SPELL_FAILED_CUSTOM_ERROR:
            return "SPELL_FAILED_CUSTOM_ERROR";

        case SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW:
            return "SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW";

        case SPELL_FAILED_TOO_MANY_SOCKETS:
            return "SPELL_FAILED_TOO_MANY_SOCKETS";

        case SPELL_FAILED_INVALID_GLYPH:
            return "SPELL_FAILED_INVALID_GLYPH";

        case SPELL_FAILED_UNIQUE_GLYPH:
            return "SPELL_FAILED_UNIQUE_GLYPH";

        case SPELL_FAILED_GLYPH_SOCKET_LOCKED:
            return "SPELL_FAILED_GLYPH_SOCKET_LOCKED";

        case SPELL_FAILED_NO_VALID_TARGETS:
            return "SPELL_FAILED_NO_VALID_TARGETS";

        case SPELL_FAILED_ITEM_AT_MAX_CHARGES:
            return "SPELL_FAILED_ITEM_AT_MAX_CHARGES";

        case SPELL_FAILED_NOT_IN_BARBERSHOP:
            return "SPELL_FAILED_NOT_IN_BARBERSHOP";

        case SPELL_FAILED_FISHING_TOO_LOW:
            return "SPELL_FAILED_FISHING_TOO_LOW";

        case SPELL_FAILED_ITEM_ENCHANT_TRADE_WINDOW:
            return "SPELL_FAILED_ITEM_ENCHANT_TRADE_WINDOW";

        case SPELL_FAILED_SUMMON_PENDING:
            return "SPELL_FAILED_SUMMON_PENDING";

        case SPELL_FAILED_MAX_SOCKETS:
            return "SPELL_FAILED_MAX_SOCKETS";

        case SPELL_FAILED_PET_CAN_RENAME:
            return "SPELL_FAILED_PET_CAN_RENAME";

        case SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED:
            return "SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED";

        case SPELL_FAILED_NO_ACTIONS:
            return "SPELL_FAILED_NO_ACTIONS";

        case SPELL_FAILED_CURRENCY_WEIGHT_MISMATCH:
            return "SPELL_FAILED_CURRENCY_WEIGHT_MISMATCH";

        case SPELL_FAILED_WEIGHT_NOT_ENOUGH:
            return "SPELL_FAILED_WEIGHT_NOT_ENOUGH";

        case SPELL_FAILED_WEIGHT_TOO_MUCH:
            return "SPELL_FAILED_WEIGHT_TOO_MUCH";

        case SPELL_FAILED_NO_VACANT_SEAT:
            return "SPELL_FAILED_NO_VACANT_SEAT";

        case SPELL_FAILED_NO_LIQUID:
            return "SPELL_FAILED_NO_LIQUID";

        case SPELL_FAILED_ONLY_NOT_SWIMMING:
            return "SPELL_FAILED_ONLY_NOT_SWIMMING";

        case SPELL_FAILED_BY_NOT_MOVING:
            return "SPELL_FAILED_BY_NOT_MOVING";

        case SPELL_FAILED_IN_COMBAT_RES_LIMIT_REACHED:
            return "SPELL_FAILED_IN_COMBAT_RES_LIMIT_REACHED";

        case SPELL_FAILED_NOT_IN_ARENA:
            return "SPELL_FAILED_NOT_IN_ARENA";

        case SPELL_FAILED_TARGET_NOT_GROUNDED:
            return "SPELL_FAILED_TARGET_NOT_GROUNDED";

        case SPELL_FAILED_EXCEEDED_WEEKLY_USAGE:
            return "SPELL_FAILED_EXCEEDED_WEEKLY_USAGE";

        case SPELL_FAILED_NOT_IN_LFG_DUNGEON:
            return "SPELL_FAILED_NOT_IN_LFG_DUNGEON";

        default:
            return "SPELL_FAILED_UNKNOWN";
    }
}

// OFFSET: 0x008A50D0
void ClearSpellCastState(CGUnit_C* unit, PendingSpellCast* cast) {
    unit->ClearCastingSpell(unit->m_castSpellId, unit->m_unit->m_unitChannelSpell == 0, true);
    unit->ClearEffectKit(cast->m_data.m_spellId, 4, 0, 0, 0);

    if (unit->m_mountTransitionSpellId == static_cast<uint32_t>(cast->m_data.m_spellId)) {
        if (unit->m_mountDisplayId != static_cast<int32_t>(unit->m_unit->m_unitFieldMountdisplayid)) {
            unit->OnMountDisplayChanged(unit->m_unit->m_unitFieldMountdisplayid);
        }

        unit->m_mountTransitionSpellId = 0;
    }

    unit->StopUpdatingMissileTrajectory(cast->m_data.m_spellId, cast->m_data.m_castCount);
}

// OFFSET: 0x008A5150/0x1405E7C40
bool GetEffectModifierType(int32_t effect, bool* periodic, int32_t aura, bool* rounded) {
    *rounded = false;
    *periodic = false;

    switch (effect) {
        case 2:
        case 9:
        case 10:
        case 17:
        case 31:
        case 58:
        case 67:
        case 75:
        case 121:
            *rounded = true;
            return true;

        case 6:
        case 27:
        case 35:
        case 65:
        case 119:
        case 128:
        case 129:
        case 143:
        case 174:
            switch (aura) {
                case 3:
                case 8:
                case 20:
                case 53:
                case 62:
                case 89:
                    *rounded = true;
                    *periodic = true;
                    return true;

                case 15:
                case 43:
                    *rounded = true;
                    return true;

                case 21:
                case 24:
                case 63:
                case 64:
                case 162:
                    *rounded = true;
                    return false;
            }

            return false;

        case 8:
        case 30:
        case 62:
        case 146:
            *rounded = true;
            return false;
    }

    return false;
}

// OFFSET: 0x008A5320/0x1405E7E20
CGUnit_C* Spell_C_TargetImpliedTarget(CGUnit_C* target, const SpellRec* spell, uint32_t needTargets, CGUnit_C* caster) {
    bool assistIgnoreFlags = false;
    bool attackIgnoreFlags = false;

    if (spell) {
        assistIgnoreFlags = (spell->m_attributesExF >> 3) & 1;
        attackIgnoreFlags = (spell->m_attributesExH & 0x80000000) != 0;
    }

    if (needTargets & 0x10C) {
        if (!caster->CanAttack(target, attackIgnoreFlags)) {
            return target;
        }

        CGUnit_C* implied = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target->m_unit->m_unitFieldTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!implied || !caster->CanAssist(implied, assistIgnoreFlags)) {
            return NULL;
        }

        if ((needTargets & 0x8) && caster->IsInMyPartyOrRaid(implied)) {
            return implied;
        }

        if ((needTargets & 0x4) && caster->IsInMyParty(implied)) {
            return implied;
        }

        if (needTargets & 0x100) {
            return implied;
        }

        return NULL;
    }

    if (needTargets & 0x80) {
        if (caster->CanAssist(target, assistIgnoreFlags)) {
            CGUnit_C* implied = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target->m_unit->m_unitFieldTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!implied || !caster->CanAttack(implied, attackIgnoreFlags)) {
                return NULL;
            }

            return implied;
        }
    }

    return target;
}

// OFFSET: 0x008A5440/0x1405E7F70
bool Spell_C_IsValidTarget(const CGUnit_C* caster, const SpellRec* spell, const CGUnit_C* target) {
    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (!effect) {
        return true;
    }

    int32_t implicitTarget = effect->m_implicitTargetA;

    if (implicitTarget == 6) {
        return target && caster->CanAttack(target, false);
    }

    if ((implicitTarget == 21 || implicitTarget == 118) && (!target || !caster->CanAssist(target, false))) {
        return false;
    }

    if (implicitTarget == 35) {
        return target && caster->IsInMyPartyOrRaid(target) && caster->CanAssist(target, false);
    }

    if (implicitTarget == 57 || implicitTarget == 61) {
        return target && caster->IsInMyParty(target) && caster->CanAssist(target, false);
    }

    return true;
}

// PROVEN: ba715f88 IDENTICAL/NONE
// OFFSET: 0x008A5500/0x1405E8060
bool Spell_C_IsTargeting() {
    return s_targetingSpell != NULL;
}

// PROVEN: 9e0dce4a IDENTICAL/NONE
// OFFSET: 0x008A5510/0x1405E8070
int32_t Spell_C_GetTargetingSpell() {
    if (s_targetingSpell) {
        return s_targetingSpell->m_data.m_spellId;
    }

    return 0;
}

// OFFSET: 0x008A5520/0x1405E8090
PendingSpellCast* Spell_C_GetTargetingSpellCast() {
    return s_targetingSpell;
}

// OFFSET: 0x008A5530
void SpellWorldObject::SetFacing(const float& facing) {
    this->m_facing = CMath::normalizeangle0to2pi_(facing);
}

// PROVEN: 518e1d22 IDENTICAL/NONE
// OFFSET: 0x008A5550/0x1405E80A0
bool Spell_C_CanTargetUnits() {
    return (s_needTargets & 0x1878E) != 0;
}

// OFFSET: 0x008A5570
bool Spell_C_CanTargetParty() {
    return (s_needTargets >> 3) & 1;
}

// OFFSET: 0x008A5580
bool Spell_C_CanTargetRaid() {
    return (s_needTargets >> 2) & 1;
}

// OFFSET: 0x008A5590
bool Spell_C_CanTargetFriends() {
    return (s_needTargets >> 8) & 1;
}

// OFFSET: 0x008A55A0
bool Spell_C_CanTargetEnemies() {
    return (s_needTargets >> 7) & 1;
}

// PROVEN: efc1f18e IDENTICAL/NONE
// OFFSET: 0x008A55B0/0x1405E80F0
bool Spell_C_CanTargetAlive() {
    return (s_needTargets & 0x8600) == 0;
}

// PROVEN: 3fe1043c IDENTICAL/NONE
// OFFSET: 0x008A55C0/0x1405E8100
bool Spell_C_CanTargetEnemyCorpses() {
    return (s_needTargets >> 9) & 1;
}

// PROVEN: a87bb30b IDENTICAL/NONE
// OFFSET: 0x008A55D0/0x1405E8110
bool Spell_C_CanTargetObjects() {
    return (s_needTargets & 0x4800) != 0;
}

// OFFSET: 0x008A55F0
bool Spell_C_CanTargetObject(const CGGameObject_C* object) {
    return (s_needTargets & 0x4800) && (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) && object->IsValidTargetForSpell(s_targetingSpell->m_data.m_castItem, s_targetingSpell->m_data.m_spellId);
}

// PROVEN: eac9ea42 IDENTICAL/NONE
// OFFSET: 0x008A5630/0x1405E8170
bool Spell_C_CanTargetTerrain() {
    return (s_needTargets & 0x60) != 0;
}

// PROVEN: 1e0b9fd9 IDENTICAL/NONE
// OFFSET: 0x008A5640/0x1405E8180
int32_t Spell_C_CanTargetItems() {
    return (s_needTargets & 0x4010) != 0;
}

// PROVEN: 47565330 IDENTICAL/NONE
// OFFSET: 0x008A5660/0x1405E8190
bool Spell_C_CanTargetNonCombatPet() {
    return (s_needTargets >> 16) & 1;
}

// PROVEN: 2bcfeff7 IDENTICAL/NONE
// OFFSET: 0x008A5670
SpellWorldObject* Spell_C_GetCursorWorldObject() {
    return s_cursorWorldObject;
}

// OFFSET: 0x008A5680
void Spell_C_SetCursorWorldObjectPosition(const C3Vector& position) {
    s_cursorWorldObject->m_position = position;
}

// OFFSET: 0x008A56A0
bool PendingSpellCast::HasValidTransports() const {
    if (this->m_data.m_destTarget) {
        C44Matrix matrix;

        if (!MovementGetTransportMtxSafe(this->m_data.m_destTarget, &matrix)) {
            return false;
        }
    }

    if (!this->m_data.m_sourceTarget) {
        return true;
    }

    C44Matrix matrix;
    return MovementGetTransportMtxSafe(this->m_data.m_sourceTarget, &matrix) != 0;
}

// OFFSET: 0x008A57B0
int32_t Packet_SMSG_GAMEOBJECT_RESET_STATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (gameObject) {
        gameObject->ResetDisplayState();
    }

    return 1;
}

// OFFSET: 0x008A5800/0x1405E8350
void Spell_C_PetTameFailure(uint8_t reason) {
    const char* token;
    char text[1024];

    switch (reason) {
        case 1:
            token = "PETTAME_INVALIDCREATURE";
            break;

        case 2:
            token = "PETTAME_TOOMANY";
            break;

        case 3:
            token = "PETTAME_CREATUREALREADYOWNED";
            break;

        case 4:
            token = "PETTAME_NOTTAMEABLE";
            break;

        case 5:
            token = "PETTAME_ANOTHERSUMMONACTIVE";
            break;

        case 6:
            token = "PETTAME_UNITSCANTTAME";
            break;

        case 7:
            token = "PETTAME_NOPETAVAILABLE";
            break;

        case 8:
            token = "PETTAME_INTERNALERROR";
            break;

        case 9:
            token = "PETTAME_TOOHIGHLEVEL";
            break;

        case 10:
            token = "PETTAME_DEAD";
            break;

        case 11:
            token = "PETTAME_NOTDEAD";
            break;

        case 12:
            token = "PETTAME_CANTCONTROLEXOTIC";
            break;

        case 13:
            token = "PETTAME_INVALIDSLOT";
            break;

        default:
            token = "PETTAME_UNKNOWNERROR";
            break;
    }

    FrameScript_GetText(token, text, sizeof(text));
    CGGameUI::DisplayError(ERR_TAME_FAILED, text);
}

// OFFSET: 0x008A58E0
int32_t Packet_SMSG_FEIGN_DEATH_RESISTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_FEIGN_DEATH_RESISTED);
    return 1;
}

// OFFSET: 0x008A5900
int32_t OnRemoteMissileTrajectoryCollision(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        static_cast<CGUnit_C*>(object)->OnRemoteMissileTrajectoryCollision(static_cast<MissileCollisionData*>(param));
    }

    return 1;
}

// OFFSET: 0x008A5950
int32_t Packet_Group_32(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t count;
    msg->Get(reinterpret_cast<uint32_t&>(count));

    if (msgId == SMSG_SET_PCT_SPELL_MODIFIER) {
        for (int32_t i = 0; i < count; i++) {
            int32_t modifiers;
            msg->Get(reinterpret_cast<uint32_t&>(modifiers));
            uint8_t op;
            msg->Get(op);

            for (int32_t j = 0; j < modifiers; j++) {
                uint8_t group;
                msg->Get(group);
                float value;
                msg->Get(value);
                s_spellPctModifiers[group][op] = value;
            }
        }
    } else {
        for (int32_t i = 0; i < count; i++) {
            int32_t modifiers;
            msg->Get(reinterpret_cast<uint32_t&>(modifiers));
            uint8_t op;
            msg->Get(op);

            for (int32_t j = 0; j < modifiers; j++) {
                uint8_t group;
                msg->Get(group);
                float value;
                msg->Get(value);
                s_spellFlatModifiers[group][op] = value;
            }
        }
    }

    CGSpellBook::UpdateUsable();
    return 1;
}

// PROVEN: 60f49463 IDENTICAL/NONE
// OFFSET: 0x008A5BC0/0x1405E87F0
uint32_t Spell_C_GetPowerDisplayMod(uint32_t powerType) {
    if (static_cast<int32_t>(powerType) < 0) {
        return 1;
    }

    return s_powerDisplayMod[powerType];
}

// PROVEN: cbfa403b IDENTICAL/NONE
// OFFSET: 0x008A5BE0/0x1405E8810
int32_t Spell_C_GetAutoRepeatingSpell() {
    return s_autoRepeatingSpell;
}

// OFFSET: 0x008A5BF0/0x1405E8820
void Spell_C_SetAutoRangedCombatSpell(int32_t spellId) {
    if (s_autoRangedCombatSpell == spellId) {
        return;
    }

    s_autoRangedCombatSpell = spellId;

    if (s_autoRepeatingSpell) {
        return;
    }

    if (spellId) {
        FrameScript_SignalEvent(SCRIPT_EVENT_START_AUTOREPEAT_SPELL, NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_STOP_AUTOREPEAT_SPELL, NULL);
    }
}

// PROVEN: eadd0aef IDENTICAL/NONE
// OFFSET: 0x008A5C30/0x1405E8860
int32_t Spell_C_GetAutoRangedCombatSpell() {
    return s_autoRangedCombatSpell;
}

// OFFSET: 0x008A5C40/0x1405E8870
void Spell_C_ClearLastAutoRangedCombatError() {
    s_lastAutoRangedCombatError = SPELL_FAILED_UNKNOWN;
}

// OFFSET: 0x008A5C50
void SpellSetFeedPetSpell(int32_t spellId) {
    s_feedPetSpell = spellId;
}

// OFFSET: 0x008A5C60
bool SpellVisualKitHasChainEffect(const SpellVisualKitRec* kit) {
    for (uint32_t i = 0; i < 4; i++) {
        if (!kit->m_charProc[i] || kit->m_charProc[i] == 12) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A5C90/0x1405E8890
const char* Spell_C_GetPowerTypeToken(uint32_t powerType) {
    const char* tokens[11] = { "MANA", "RAGE", "FOCUS", "ENERGY", "", "RUNES", "RUNIC_POWER", "SOUL_SHARDS", "ECLIPSE", "HOLY_POWER", "ALTERNATE" };

    if (powerType > 11) {
        return "";
    }

    return tokens[powerType];
}

// PROVEN: e3d50817 IDENTICAL/NONE
// OFFSET: 0x008A5D00
PendingSpellCast* Spell_C_GetDelayedTrajectoryCast() {
    return s_delayedTrajectoryCast;
}

// OFFSET: 0x008A5D10/0x1405E8940
void Spell_C_SetMissileTrajectoryNextSequenceIndex(uint32_t index) {
    s_missileTrajectorySequenceIndex = index;
}

// OFFSET: 0x008A5D20
uint32_t Spell_C_EncodeSequenceIntoMissileTrajectory(uint32_t* pitch, uint32_t* speed, uint32_t* source, uint32_t* dest) {
    uint32_t sequence = s_missileTrajectorySequenceIndex++;
    uint32_t key[2];
    key[0] = (((source[0] ^ (source[2] & 0xF000)) & 0x2F000) ^ (source[1] & 0x300EE630) ^ (pitch[0] & 0x6000CF0) ^ (dest[1] & 0x60E0400) ^ (speed[0] & 0x240E00) ^ (dest[0] & 0xA9070F01) ^ sequence) | 0x1010101;
    key[1] = 0;
    uint32_t hash = SStrHash(reinterpret_cast<const char*>(key), 1, 0);

    pitch[0] = (pitch[0] & ~0x3u) | ((hash >> 10) & 1) | ((hash >> 27) & 2);
    speed[0] = (speed[0] & ~0x9u) | ((hash >> 1) & 1) | ((hash >> 19) & 8);
    source[0] = (source[0] & ~0x6u) | (((hash & 0x8000) | ((hash >> 9) & 0x10000)) >> 14);
    source[1] ^= ((hash >> 1) ^ source[1]) & 2;
    source[1] ^= ((hash >> 11) ^ source[1]) & 8;
    source[2] ^= ((hash >> 6) ^ source[2]) & 8;
    source[2] ^= ((hash >> 28) ^ source[2]) & 4;
    dest[0] = (dest[0] & ~0x6u) | ((hash >> 6) & 2) | ((hash >> 3) & 4);
    dest[1] ^= ((hash >> 29) ^ dest[1]) & 1;
    dest[1] ^= ((hash >> 3) ^ dest[1]) & 8;
    dest[2] ^= ((hash >> 20) ^ dest[2]) & 1;
    dest[2] ^= ((hash >> 10) ^ dest[2]) & 4;

    pitch[0] = SetTrajectoryCheckBit(pitch[0], 3);
    speed[0] = SetTrajectoryCheckBit(speed[0], 1);
    source[0] = SetTrajectoryCheckBit(source[0], 3);
    source[1] = SetTrajectoryCheckBit(source[1], 2);
    source[2] = SetTrajectoryCheckBit(source[2], 0);
    dest[0] = SetTrajectoryCheckBit(dest[0], 0);
    dest[1] = SetTrajectoryCheckBit(dest[1], 2);
    dest[2] = SetTrajectoryCheckBit(dest[2], 1);
    return dest[2];
}

// PROVEN: 31a91915 IDENTICAL/NONE
// OFFSET: 0x008A6020/0x1405E8C90
int32_t Spell_C_IsOverride(int32_t spellId, const OverrideSpellDataRec* rec) {
    for (uint32_t i = 0; i < sizeof(rec->m_spells) / sizeof(rec->m_spells[0]); i++) {
        if (rec->m_spells[i] == spellId) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008A6050
float Spell_C_GetSpellScalingMultiplier(const CGUnit_C* unit, const SpellScalingRec* scaling, bool ignoreCoefficient) {
    float multiplier = 1.0f;

    if (!unit || !scaling) {
        return 1.0f;
    }

    int32_t level = unit->m_unit->m_unitFieldLevel;

    if (level < scaling->m_castTimeMaxLevel && scaling->m_castTimeMax) {
        multiplier = static_cast<float>(scaling->m_castTimeMin + (level - 1) * (scaling->m_castTimeMax - scaling->m_castTimeMin) / (scaling->m_castTimeMaxLevel - 1)) / static_cast<float>(scaling->m_castTimeMax);
    }

    if (!ignoreCoefficient && level < scaling->m_nerfMaxLevel) {
        return ((1.0f - scaling->m_nerfFactor) * static_cast<float>(level - 1) / static_cast<float>(scaling->m_nerfMaxLevel - 1) + scaling->m_nerfFactor) * multiplier;
    }

    return multiplier;
}

// OFFSET: 0x008A6110
bool Spell_C_SpellScalingCastTime(const CGUnit_C* unit, const SpellScalingRec* scaling, int32_t& castTime) {
    if (!unit || !scaling) {
        return false;
    }

    castTime = scaling->m_castTimeMax;

    if (castTime) {
        int32_t level = unit->m_unit->m_unitFieldLevel;

        if (level < scaling->m_castTimeMaxLevel) {
            castTime = scaling->m_castTimeMin + (level - 1) * (scaling->m_castTimeMax - scaling->m_castTimeMin) / (scaling->m_castTimeMaxLevel - 1);
        }
    }

    return true;
}

// OFFSET: 0x008A6160
int32_t Spell_C_GetEnchantmentSlot(int32_t slot, const SpellItemEnchantmentRec* enchant) {
    if (enchant && !slot) {
        for (uint32_t i = 0; i < 3; i++) {
            if (enchant->m_effect[i] == 7) {
                return 7;
            }
        }
    }

    return slot;
}

// OFFSET: 0x008A6190/0x1405E8E00
int32_t Spell_C_GetMaxStartRecoveryOffset() {
    return s_maxStartRecoveryOffset;
}

// OFFSET: 0x008A61A0/0x1405E8E10
void Spell_C_SetMaxStartRecoveryOffset(int32_t offset) {
    if (static_cast<uint32_t>(offset) > 400) {
        offset = offset < 0 ? 0 : 400;
    }

    s_maxStartRecoveryOffset = offset;
    FrameScript_SignalEvent(SCRIPT_EVENT_MAX_SPELL_START_RECOVERY_OFFSET_CHANGED, "%d", offset);
}

// OFFSET: 0x008A61E0/0x140779480
int32_t CompareSpellAuraVisibility(const void* a, const void* b) {
    const int32_t* l = static_cast<const int32_t*>(a);
    const int32_t* r = static_cast<const int32_t*>(b);
    int32_t result = l[1] - r[1];

    if (!result) {
        return l[2] - r[2];
    }

    return result;
}

// OFFSET: 0x008A65B0
bool PendingSpellCast::IsCharmedUnitCast() const {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const WOWGUID& controlled = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
    return controlled == this->m_data.m_caster;
}

// OFFSET: 0x008A6620
PendingSpellCast* GetPendingSpellCast(uint8_t castCount, int32_t spellId, const WOWGUID& caster) {
    if (!spellId) {
        return NULL;
    }

    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast->m_data.m_castCount == castCount && (cast->m_data.m_spellId == static_cast<uint32_t>(spellId) || cast->m_baseSpellId == spellId) && cast->m_data.m_caster == caster) {
            return cast;
        }
    }

    return NULL;
}

// OFFSET: 0x008A6690
ItemSubClassRec* FindAnyItemSubclassRec(int32_t classId, int32_t subclassMask) {
    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID == classId && ((1 << rec->m_subClassID) & subclassMask)) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x008A66E0
void SpellClearMacroGlobalCooldownWait(const SpellRec* spell) {
    const SpellCategoriesRec* categories;

    if (!spell->m_spellCategoriesID || !(categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID)) || !categories->m_startRecoveryCategory) {
        return;
    }

    const SpellCooldownsRec* cooldowns;

    if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID)) && cooldowns->m_startRecoveryTime) {
        CGUIMacros::m_state &= ~4;
    }
}

// OFFSET: 0x008A6740
void StartSpellCastVisuals(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t castTime, bool hidden) {
    if (!unit) {
        return;
    }

    cast->m_hidden = hidden;
    WOWGUID guid = unit->m_obj->m_guid;
    PendingSpellCast* current = GetPendingSpellCast(unit->m_castCount, unit->m_castSpellId, guid);

    if (current && current->m_hidden) {
        if (!hidden) {
            ClearSpellCastState(unit, current);

            if (unit == ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
                CGTradeSkillInfo::ActivatePendingRecast(cast->m_data.m_spellId);
            }
        }
    } else if (!hidden && unit == ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGTradeSkillInfo::ActivatePendingRecast(cast->m_data.m_spellId);
    }

    if (!unit->m_castSpellId && !cast->m_trajectoryLaunched) {
        unit->SetCastingSpell(cast);
        SpellVisualsHandleCastStart(unit, &cast->m_data, spell);
    }

    if (castTime > 0) {
        bool showCastBar = true;

        if (!(spell->m_attributesExD & 0x8000000)) {
            if (spell->m_attributes & 0x2) {
                showCastBar = false;
            }

            if (cast->m_data.m_caster != ClntObjMgrGetActivePlayer() && ((spell->m_attributes & 0x180) || (spell->m_attributesExD & 0x1))) {
                showCastBar = false;
            }
        }

        if (showCastBar) {
            if (cast->m_data.m_spellId != static_cast<uint32_t>(unit->m_castSpellId)) {
                ClearSpellCastState(unit, cast);
                unit->SetCastingSpell(cast);
                SpellVisualsHandleCastStart(unit, &cast->m_data, spell);
            }

            unit->SetUICastingSpell(spell, cast->m_data.m_objectTarget, castTime);
            WOWGUID caster = unit->m_obj->m_guid;
            Script_SendUnitSpellSignal(caster, cast->m_data.m_castCount, spell, 327);
        }
    }

    if ((cast->m_data.m_caster == ClntObjMgrGetActivePlayer() || cast->IsCharmedUnitCast()) && cast->m_data.m_castCount) {
        CGSpellBook::UpdateSelection(0);
    }
}

// OFFSET: 0x008A68F0/0x1405E95C0
void HandleSpellFailedOrGo(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t reason) {
    if (!unit) {
        return;
    }

    uint8_t castCount = unit->m_castCount;

    if (cast->m_data.m_castCount == castCount && cast->m_data.m_spellId == static_cast<uint32_t>(unit->m_castSpellId)) {
        ClearSpellCastState(unit, cast);
    }

    int32_t signal;

    switch (reason) {
        case SPELL_FAILED_UNKNOWN: {
            WOWGUID caster = unit->m_obj->m_guid;
            Script_SendUnitSpellSignal(caster, cast->m_data.m_castCount, spell, 333);
            goto done;
        }

        case SPELL_FAILED_DONT_REPORT:
        case SPELL_FAILED_SPELL_IN_PROGRESS:
        case SPELL_FAILED_CHARMED:
            signal = 330;
            break;

        case SPELL_FAILED_INTERRUPTED:
        case SPELL_FAILED_INTERRUPTED_COMBAT: {
            WOWGUID caster = unit->m_obj->m_guid;
            Script_SendUnitSpellSignal(caster, cast->m_data.m_castCount, spell, 331);
            goto done;
        }

        default:
            signal = 329;
            break;
    }

    {
        WOWGUID caster = unit->m_obj->m_guid;
        Script_SendUnitSpellSignal(caster, cast->m_data.m_castCount, spell, signal);
    }

done:
    if (cast->m_data.m_castCount == castCount && cast->m_data.m_spellId == static_cast<uint32_t>(unit->m_castingSpellId)) {
        unit->ResetUICastingSpell();
        WOWGUID caster = unit->m_obj->m_guid;
        Script_SendUnitSpellSignal(caster, cast->m_data.m_castCount, spell, 328);
    }

    if (cast == s_targetingSpell) {
        s_needTargets = 0;
        s_targetingSpell = NULL;
        CursorSetResetMode(1);
        CursorSetMode(1);
        s_cursorWorldObject->Release();
        CGInputControl* input = CGInputControl::GetActive();
        input->m_mouseModeFlags &= ~0x10;
        input->UpdateMouseMode(0);
    }

    if (reason == SPELL_FAILED_UNKNOWN) {
        return;
    }

    if (cast->m_data.m_castCount && spell->m_spellMissileID > 0) {
        SpellMissileRec* missile = g_spellMissileDB.GetRecord(spell->m_spellMissileID);

        if (missile && (missile->m_flags & 0x1)) {
            unit->ClearPrecastMissileModel(cast->m_data.m_castCount);
            CMissile::StartFizzling(cast->m_data.m_caster, cast->m_data.m_castCount, cast->m_data.m_spellId);
        }
    }

    if (reason) {
        unit->ClearPrecastMissileModel(cast->m_data.m_castCount);
    }
}

// OFFSET: 0x008A6AB0
bool Spell_C_TargetMatchesAutoAttack(const SpellRec* spell, const PendingSpellCast* cast, int32_t spellId, int32_t keepTarget) {
    if (!(spell->m_attributesExI & 0x10000) || !g_stopAutoAttackOnTargetChangeCVar || !g_stopAutoAttackOnTargetChangeCVar->m_intValue) {
        return true;
    }

    WOWGUID target = WOWGUID();

    if (spellId == GetModalNextSpell()) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player) {
            return false;
        }

        if (!keepTarget) {
            player->VerifyAttackIconPressed(&target, 0, 1, true, true);
        }
    } else {
        target = CGGameUI::m_lockedTarget;
    }

    return target == cast->m_data.m_objectTarget;
}

// OFFSET: 0x008A6B50/0x1405E7BA0
void GetCustomSpellFailedString(char* buffer, uint32_t size, int32_t customError) {
    char token[64];
    SStrPrintf(token, sizeof(token), "%s_%d", "SPELL_FAILED_CUSTOM_ERROR", customError);
    FrameScript_GetText(token, buffer, size);
    CGGameUI::DisplayError(ERR_SPELL_FAILED_S, buffer);
}

// OFFSET: 0x008A6BA0/0x1405E98C0
int32_t Spell_C_GetCategoryResetTime(int32_t cooldown, int32_t daily, int32_t gameTime) {
    if (daily) {
        return 86400000 * (cooldown / 1000);
    }

    WowTime time = g_clientGameTime;

    if (gameTime) {
        time.AddMinutes(gameTime / 60);
    }

    int32_t days = cooldown / 1000 - 1;

    if (days < 0) {
        return cooldown;
    }

    return 60000 * (60 * (8 * (3 * days + 3) - time.m_hour) - time.m_minute);
}

// OFFSET: 0x008A6C60/0x1405E9990
void Spell_C_ModifyCategoryRecovery(int32_t& recovery, int32_t category, int32_t daily) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    SpellCategoryRec* rec = g_spellCategoryDB.GetRecord(category);

    if (rec && (rec->m_flags & 0x8)) {
        recovery = Spell_C_GetCategoryResetTime(recovery, daily, player->m_player->m_playerFieldHomeRealmTimeOffset);
    }

    if (player->m_auraEffectMask[0x2A] & 0x20) {
        int32_t reduction = player->GetCategoryCooldownModifier(category);

        if (reduction <= recovery) {
            recovery -= reduction;
        } else {
            recovery = 0;
        }
    }
}

// OFFSET: 0x008A6D00/0x1405E9A60
CGUnit_C* Spell_C_GetStatTargetUnit(int32_t pet, int32_t vehicle) {
    if (vehicle) {
        return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGTooltip::m_vehicleStatUnit, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (pet) {
        return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
}

// OFFSET: 0x008A6D80/0x1405E9AE0
uint32_t Spell_C_GetSpellLevel(const SpellRec* spell, int32_t isPet, int32_t isVehicle) {
    CGUnit_C* unit = Spell_C_GetStatTargetUnit(isPet, isVehicle);

    if (!unit) {
        return 0;
    }

    return static_cast<uint32_t>(unit->GetSpellRank(spell)) / 5;
}

// OFFSET: 0x008A6DC0/0x1405E9B30
int32_t Spell_C_GetSpellLevelBonus(const SpellRec* spell, int32_t level) {
    const SpellLevelsRec* levels;

    if (spell && spell->m_spellLevelsID && (levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID))) {
        if (levels->m_maxLevel > 0 && level > levels->m_maxLevel) {
            level = levels->m_maxLevel;
        }

        if (levels->m_baseLevel > 0) {
            level -= levels->m_baseLevel;
        }
    }

    return level >= 0 ? level : 0;
}

// OFFSET: 0x008A6E20
bool SpellRec::IsMeleeRange(const SpellRec* spell) {
    SpellRangeRec* range = g_spellRangeDB.GetRecord(spell->m_rangeIndex);
    return range && (range->m_flags & 0x1);
}

// OFFSET: 0x008A6E60
bool SpellRec::IsMinimumRange(const SpellRec* spell) {
    SpellRangeRec* range = g_spellRangeDB.GetRecord(spell->m_rangeIndex);
    return range && (range->m_flags & 0x2);
}

// OFFSET: 0x008A6EA0
bool Spell_C_SpellUsesReagent(const SpellRec* spell, int32_t itemId) {
    const SpellReagentsRec* reagents;

    if (!spell->m_spellReagentsID || !(reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        return false;
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (reagents->m_reagent[i] == itemId && reagents->m_reagentCount[i] > 0) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A6F00/0x1405E9C70
int32_t Spell_C_GetSpellReagentUses(const SpellRec* spell) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t uses = 0x7FFFFFFF;
    const SpellReagentsRec* reagents;

    if (!spell->m_spellReagentsID || !(reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        return 0;
    }

    for (uint32_t i = 0; i < 8; i++) {
        int32_t reagent = reagents->m_reagent[i];

        if (reagent != -2 && reagent && reagents->m_reagentCount[i] > 0) {
            int32_t count = player->GetBag()->GetItemTypeCount(reagent, false) / reagents->m_reagentCount[i];

            if (count < uses) {
                uses = count;
            }
        }
    }

    return uses != 0x7FFFFFFF ? uses : 0;
}

// OFFSET: 0x008A6FC0
bool Spell_C_IsModal(const WOWGUID& item) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        if (s_pendingSpellCasts[i].m_data.m_castItem == item) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A7010
bool Spell_C_IsSpellPending(int32_t spellId) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        if (s_pendingSpellCasts[i].m_data.m_spellId == static_cast<uint32_t>(spellId)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A7050
bool Spell_C_HasOtherPendingCast(const WOWGUID& caster, int32_t spellId, const PendingSpellCast* exclude) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast != exclude && (!caster || cast->m_data.m_caster == caster) && cast->m_data.m_spellId == static_cast<uint32_t>(spellId)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A70B0/0x1405E9DC0
bool Spell_C_IsCurrentItem(int32_t itemId) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast->m_data.m_castItem != cast->m_data.m_caster) {
            CGObject_C* item = ClntObjMgrObjectPtr(cast->m_data.m_castItem, TYPEMASK_ITEM, __FILE__, __LINE__);

            if (item && item->m_obj->m_entryID == itemId) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x008A7120
bool Spell_C_CheckItemLevel(const PendingSpellCast* cast, const SpellRec* spell, const CGObject_C* item, const ItemRecSparse* stats, int32_t* reason) {
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster) {
        *reason = SPELL_FAILED_DONT_REPORT;
        return false;
    }

    int32_t baseLevel = 0;
    const SpellLevelsRec* levels = NULL;

    if (spell->m_spellLevelsID) {
        levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

        if (levels) {
            baseLevel = levels->m_baseLevel;
        }
    }

    int32_t maxLevel = 0;

    if (cast->m_data.m_castItem != caster->m_obj->m_guid && item && (item->m_obj->m_type & TYPEMASK_ITEM)) {
        maxLevel = levels ? levels->m_maxLevel : 0;
    }

    int32_t itemLevel = stats->m_itemLevel;

    if (itemLevel < baseLevel) {
        if (!(spell->m_attributesExB & 0x8)) {
            *reason = SPELL_FAILED_LOWLEVEL;
            return false;
        }
    } else if (maxLevel > 0 && itemLevel > maxLevel) {
        *reason = SPELL_FAILED_HIGHLEVEL;
        return false;
    }

    return true;
}

// OFFSET: 0x008A7200/0x1405E9F60
CGUnit_C* Spell_C_GetImplicitSpellTarget(const CGUnit_C* caster, const SpellRec* spell, const WOWGUID& target) {
    int32_t implicitTarget = 0;
    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect) {
        implicitTarget = effect->m_implicitTargetA;
    }

    WOWGUID guid = target ? target : CGGameUI::m_lockedTarget;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!g_autoSelfCastCVar->m_intValue || (unit && ((implicitTarget != 21 && implicitTarget != 35 && implicitTarget != 57 && implicitTarget != 61 && implicitTarget != 118) || caster->CanAssist(unit, false)))) {
        return unit;
    }

    return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
}

// OFFSET: 0x008A72B0
bool Spell_C_IsActivePlayerCasting() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    return player && player->m_castSpellId;
}

// OFFSET: 0x008A72F0
PendingSpellCast* Spell_C_GetActivePlayerPendingCast() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return NULL;
    }

    WOWGUID guid = player->m_obj->m_guid;
    return GetPendingSpellCast(player->m_castCount, player->m_castSpellId, guid);
}

// OFFSET: 0x008A7350
bool Spell_C_IsCastingInterruptedByCombat(const WOWGUID& guid) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (guid != cast->m_data.m_caster) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);
        const SpellInterruptsRec* interrupts;

        if (spell && spell->m_spellInterruptsID && (interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID)) && (interrupts->m_interruptFlags & 0x8)) {
            return true;
        }
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->m_unit->m_unitChannelSpell) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(unit->m_unit->m_unitChannelSpell);
    const SpellInterruptsRec* interrupts;

    if (!spell || !spell->m_spellInterruptsID || !(interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID)) || !(interrupts->m_channelInterruptFlags & 0x1000)) {
        return false;
    }

    return true;
}

// OFFSET: 0x008A7490/0x1405EA260
void GameObjectStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!s_targetingSpell || s_targetingSpell->m_data.m_castCount != static_cast<uint8_t>(reinterpret_cast<uintptr_t>(param))) {
        return;
    }

    WOWGUID owner = WOWGUID();
    GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(id, owner, NULL, NULL, false);

    if (!stats) {
        return;
    }

    GameObjectDisplayInfoRec* displayInfo = g_gameObjectDisplayInfoDB.GetRecord(stats->m_displayID);

    if (displayInfo) {
        s_cursorWorldObject->m_scale = stats->m_scale;
        s_cursorWorldObject->Create(displayInfo);
    }
}

// OFFSET: 0x008A7510/0x1405EA2F0
bool Spell_C_CanCastWhileCasting(const SpellRec* spell, const CGUnit_C* unit, int32_t* remaining) {
    *remaining = 0;
    int32_t castSpellId = unit->m_castSpellId;

    if (!castSpellId || (spell->m_attributesExD & 0x80000000)) {
        return true;
    }

    if (spell->m_attributesExB & 0x20) {
        SpellRec* current = g_spellDB.GetRecord(castSpellId);

        if (!current) {
            return false;
        }

        if ((current->m_attributes & 0x2) && (current->m_attributesExB & 0x20000)) {
            return true;
        }
    }

    *remaining = unit->GetCastTimeRemaining();
    return false;
}

// OFFSET: 0x008A7580
bool Spell_C_CanTargetMe() {
    if (s_needTargets & 0x850E) {
        SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);

        if (spell && !(spell->m_attributesEx & 0x80000)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A75D0
bool Spell_C_CanTargetDead() {
    if (!s_targetingSpell) {
        return false;
    }

    if (s_needTargets & 0x8600) {
        return true;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);
    return spell && (spell->m_attributesExB & 1);
}

// OFFSET: 0x008A7620
bool Spell_C_CanTargetFriendCorpses() {
    if (s_needTargets & 0x8000) {
        return true;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);
    return spell && (spell->m_attributesExI & 0x8000000);
}

// OFFSET: 0x008A7670
bool Spell_C_CanTargetCorpse(const CGCorpse_C* corpse) {
    if (!s_targetingSpell) {
        return false;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_targetingSpell->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster || (corpse->m_corpse->m_corpseFieldFlags & 1)) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);

    if (spell && (spell->m_attributesExI & 0x8000000)) {
        if ((s_needTargets & 0x8) && caster->IsActivePlayer()) {
            WOWGUID owner = corpse->m_corpse->m_corpseFieldOwner;

            if (CGGameUI::IsInParty(owner)) {
                if (caster->CanAssist(corpse)) {
                    return true;
                }

                goto check;
            }
        }

        if ((s_needTargets & 0x4) && caster->IsActivePlayer()) {
            WOWGUID owner = corpse->m_corpse->m_corpseFieldOwner;

            if (CGGameUI::IsPartyMember(owner) && caster->CanAssist(corpse)) {
                return true;
            }
        }
    }

check:
    if (s_needTargets & 0x8000) {
        return caster->CanAssist(corpse);
    }

    if ((s_needTargets & 0x200) && !caster->CanAssist(corpse)) {
        return true;
    }

    return false;
}

// OFFSET: 0x008A77D0
bool Spell_C_CanTargetPossessedFriends() {
    if (!s_targetingSpell) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);
    return spell && (spell->m_attributesExF & 0x200);
}

// OFFSET: 0x008A7810
float Spell_C_GetConeAngle(const SpellRec* spell) {
    if (!spell) {
        return 0.0f;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            switch ((&effect->m_implicitTargetA)[j]) {
                case 24:
                case 59:
                case 60:
                case 93:
                case 104:
                case 108:
                case 109:
                case 110: {
                    SpellTargetRestrictionsRec* restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID);

                    if (!restrictions) {
                        return 0.0f;
                    }

                    return restrictions->m_maxTargetRadius;
                }
            }
        }
    }

    return 0.0f;
}

// OFFSET: 0x008A7930/0x1405EA770
bool Spell_C_TargetingSpellAllowsTerrain() {
    if (!s_targetingSpell) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);
    return spell && (spell->m_attributesExF & 0x1000000);
}

// OFFSET: 0x008A7970
int32_t Packet_SMSG_SPELL_DELAYED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    int32_t delay;
    msg->Get(reinterpret_cast<uint32_t&>(delay));
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    SpellRec* spell = g_spellDB.GetRecord(unit->m_castingSpellId);

    if (spell && ((spell->m_attributesExD & 0x8000000) || (!(spell->m_attributes & 0x2) && (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() || (!(spell->m_attributes & 0x180) && !(spell->m_attributesExD & 0x1)))))) {
        unit->DelayUICastingSpell(delay);
        WOWGUID signalGuid = guid;
        Script_SendUnitSpellSignal(signalGuid, unit->m_castCount, spell, 332);
    }

    return 1;
}

// OFFSET: 0x008A7A60
int32_t Packet_MSG_CHANNEL_START(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t healAura = 0;
    uint32_t schoolImmunities = 0;
    uint32_t immunities = 0;
    SmartGUID guid;
    *msg >> guid;
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    int32_t duration;
    msg->Get(reinterpret_cast<uint32_t&>(duration));
    uint8_t hasImmunities;
    msg->Get(hasImmunities);

    if (hasImmunities) {
        msg->Get(schoolImmunities);
        msg->Get(immunities);
    }

    uint8_t hasHealPrediction;
    msg->Get(hasHealPrediction);

    if (hasHealPrediction) {
        SmartGUID beacon;
        beacon.m_guid = 0;
        SmartGUID target;
        *msg >> target;
        int32_t points;
        msg->Get(reinterpret_cast<uint32_t&>(points));
        uint8_t type;
        msg->Get(type);

        if (type == 2) {
            *msg >> beacon;
        }

        if (points > 0) {
            CGUnit_C* targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (targetUnit) {
                int32_t count = SpellGetEffectCount(spellId);

                for (int32_t i = 0; i < count; i++) {
                    const SpellEffectRec* effect = SpellGetEffect(spellId, i);

                    if (effect->m_effect == 6 && effect->m_effectAura == 23) {
                        healAura = effect->m_effectTriggerSpell;
                        break;
                    }
                }

                WOWGUID caster = guid;
                targetUnit->AddHealPrediction(spellId, healAura, caster, points, true, true);
            }
        }
    }

    if (duration <= 0) {
        return 1;
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || !((spell->m_attributesExD & 0x8000000) || (!(spell->m_attributes & 0x2) && !(spell->m_attributes & 0x180) && !(spell->m_attributesExC & 0x2000) && !(spell->m_attributesExD & 0x1)))) {
        return 1;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (hasImmunities) {
            unit->SetCastSchoolImmunities(schoolImmunities);
            unit->SetCastImmunities(immunities);
        }

        unit->SetUIChannelSpell(spell, duration);
        WOWGUID signalGuid = guid;
        Script_SendUnitSpellSignal(signalGuid, 0, spell, 334);
    }

    return 1;
}

// OFFSET: 0x008A7C70
int32_t Packet_MSG_CHANNEL_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;
    int32_t remaining;
    msg->Get(reinterpret_cast<uint32_t&>(remaining));
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    SpellRec* spell = g_spellDB.GetRecord(unit->m_channelSpellId);

    if (spell && ((spell->m_attributesExD & 0x8000000) || (!(spell->m_attributes & 0x2) && !(spell->m_attributes & 0x180) && !(spell->m_attributesExC & 0x2000) && !(spell->m_attributesExD & 0x1)))) {
        unit->OnChannelUpdate(remaining);
        int32_t signal;

        if (remaining) {
            signal = 335;
        } else {
            unit->ResetUIChannelSpell();
            signal = 336;
        }

        WOWGUID signalGuid = guid;
        Script_SendUnitSpellSignal(signalGuid, 0, spell, signal);

        if (!remaining) {
            unit->RemoveChannelEffects(spell->m_id);
        }
    }

    return 1;
}

// OFFSET: 0x008A7D70
void CreateMissilesForTargets(const TSStackArray<WOWGUID>& targets, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, SpellCast* data, const TSStackArray<uint32_t>* missReasons, const TSStackArray<uint32_t>* reflectResults, uint32_t delay) {
    if (!unit || (!visual->m_hasMissile && !ammoDisplayId)) {
        return;
    }

    int32_t hit = missReasons == NULL;
    uint32_t targetFlags = data->m_targetFlags;
    bool unitTargets = !(targetFlags & 0x60) || (visual->m_flags & 0x1);

    if (targets.m_count && (targetFlags & 0x802) && unitTargets) {
        for (uint32_t i = 0; i < targets.m_count; i++) {
            if (CanPlaySpellVisual(0, targets.m_data[i].m_guid, visual) && ClntObjMgrObjectPtr(targets.m_data[i], TYPEMASK_OBJECT, __FILE__, __LINE__)) {
                CreateMissileParams params;
                memset(&params, 0, sizeof(params));
                params.m_ammoInventoryType = ammoInventoryType;
                params.m_data = data;
                params.m_ammoDisplayId = ammoDisplayId;
                params.m_visual = visual;
                params.m_spell = spell;
                params.m_delay = delay;
                params.m_target = targets.m_data[i];
                params.m_hit = hit;
                params.m_missReason = missReasons ? missReasons->m_data[i] : 1;
                params.m_reflectResult = reflectResults ? reflectResults->m_data[i] : 0;
                params.m_targets = &targets;
                unit->CreateMissile(params);
            }
        }
    } else if ((targetFlags & 0x60) && (targets.m_count || !(visual->m_flags & 0x100))) {
        CreateMissileParams params;
        memset(&params, 0, sizeof(params));
        params.m_ammoDisplayId = ammoDisplayId;
        params.m_ammoInventoryType = ammoInventoryType;
        params.m_data = data;
        params.m_delay = delay;
        params.m_hit = hit;
        params.m_spell = spell;
        params.m_reflectResults = reflectResults;
        params.m_visual = visual;
        params.m_targets = &targets;
        params.m_missReason = 1;
        params.m_reflectResult = 0;
        params.m_missReasons = missReasons;
        unit->CreateMissile(params);
    }
}

// OFFSET: 0x008A7F60
void CreateMissilesForTargetPoints(CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, SpellCast* data, const TSStackArray<SpellCastTargetPoint>& points, uint32_t delay) {
    if (!unit || (!visual->m_hasMissile && !ammoDisplayId)) {
        return;
    }

    for (uint32_t i = 0; i < points.m_count; i++) {
        CreateMissileParams params;
        memset(&params, 0, sizeof(params));
        params.m_flags |= 1;
        params.m_data = data;
        params.m_ammoDisplayId = ammoDisplayId;
        params.m_ammoInventoryType = ammoInventoryType;
        params.m_visual = visual;
        params.m_spell = spell;
        params.m_delay = delay;
        params.m_point = &points.m_data[i];
        unit->CreateMissile(params);
    }
}

// OFFSET: 0x008A7FF0
void LaunchMissileVisual(const TSStackArray<WOWGUID>& hits, const TSStackArray<WOWGUID>& misses, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, SpellCast* data, const TSStackArray<uint32_t>& missReasons, const TSStackArray<uint32_t>& reflectResults, uint32_t delay) {
    if (!unit || (!visual->m_hasMissile && !ammoDisplayId)) {
        return;
    }

    uint32_t count = hits.m_count + misses.m_count;

    TSStackArray<WOWGUID> targets(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * count)), count, 0);

    TSStackArray<uint32_t> reasons(static_cast<uint32_t*>(alloca(sizeof(uint32_t) * count)), count, 0);

    TSStackArray<uint32_t> reflects(static_cast<uint32_t*>(alloca(sizeof(uint32_t) * count)), count, 0);

    uint32_t index = 0;

    if (!(data->m_castFlags & 0x400)) {
        for (; index < hits.m_count; index++) {
            targets.m_data[index] = hits.m_data[index];
            reasons.m_data[index] = 0;
            reflects.m_data[index] = 0;
        }

        for (uint32_t i = 0; i < misses.m_count; i++, index++) {
            targets.m_data[index] = misses.m_data[i];
            reasons.m_data[index] = missReasons.m_data[i];
            reflects.m_data[index] = reflectResults.m_data[i];
        }
    } else {
        for (uint32_t i = 0; i < misses.m_count; i++, index++) {
            targets.m_data[index] = misses.m_data[index];
            reasons.m_data[index] = missReasons.m_data[index];
            reflects.m_data[index] = reflectResults.m_data[index];
        }

        for (uint32_t i = 0; i < hits.m_count; i++, index++) {
            targets.m_data[index] = hits.m_data[i];
            reasons.m_data[index] = 0;
            reflects.m_data[index] = 0;
        }
    }

    CreateMissileParams params;
    memset(&params, 0, sizeof(params));
    params.m_data = data;
    params.m_visual = visual;
    params.m_ammoDisplayId = ammoDisplayId;
    params.m_ammoInventoryType = ammoInventoryType;
    params.m_targets = &targets;
    params.m_spell = spell;
    params.m_delay = delay;
    params.m_hit = 1;
    params.m_missReason = 1;
    params.m_reflectResult = 0;
    params.m_missReasons = &reasons;
    params.m_reflectResults = &reflects;
    unit->CreateMissile(params);
}

// OFFSET: 0x008A8210
int32_t Packet_SMSG_PET_TAME_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t reason;
    msg->Get(reason);
    Spell_C_PetTameFailure(reason);
    return 1;
}

// OFFSET: 0x008A8240
int32_t Packet_SMSG_SPELL_UPDATE_CHAIN_TARGETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID caster;
    msg->Get(caster.m_guid);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint32_t count = 0;
    msg->Get(count);
    TSStackArray<WOWGUID> targets(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * count)), count, 1);

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(targets.m_data[i].m_guid);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateChainTargets(targets, spellId);
    }

    return 1;
}

// OFFSET: 0x008A82E0
int32_t Packet_SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_vehiclePassenger) {
        player->m_vehiclePassenger->ClearFlag8();
    }

    return 1;
}

// OFFSET: 0x008A8320
int32_t Packet_SMSG_SET_PROJECTILE_POSITION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    MissileCollisionData data;
    data.m_position.x = 0.0f;
    data.m_position.y = 0.0f;
    data.m_position.z = 0.0f;
    msg->Get(data.m_casterGuid.m_guid);
    msg->Get(data.m_castID);
    msg->Get(data.m_position.x);
    msg->Get(data.m_position.y);
    msg->Get(data.m_position.z);
    CMissile::OnRemoteMissileTrajectoryCollision(data);
    ClntObjMgrEnumVisibleObjects(OnRemoteMissileTrajectoryCollision, &data);
    return 1;
}

// OFFSET: 0x008A83A0
bool OnPlaySpellVisualKit(ClientPlaySpellVisualKit* msg) {
    WOWGUID guid = *reinterpret_cast<const WOWGUID*>(msg->m_unitGUID);
    uint32_t timeOffset = msg->m_timeOffset;
    uint32_t kitType = msg->m_kitType;
    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(msg->m_kitRecID);

    if (kit && CanPlaySpellVisualKit(0, guid.m_guid, kit, NULL)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            PlaySpellVisuaKitData data(NULL, kit, kitType);
            data.m_timeOffset = timeOffset;
            unit->PlaySpellVisualKit(data);
        }
    }

    return true;
}

// OFFSET: 0x008A8440
int32_t Packet_SMSG_RESUME_CAST_BAR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t schoolImmunities = 0;
    uint32_t immunities = 0;
    SmartGUID caster;
    *msg >> caster;
    SmartGUID target;
    *msg >> target;
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint32_t remaining;
    msg->Get(remaining);
    uint32_t duration;
    msg->Get(duration);
    uint8_t hasImmunities;
    msg->Get(hasImmunities);

    if (hasImmunities) {
        msg->Get(schoolImmunities);
        msg->Get(immunities);
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 1;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    if (hasImmunities) {
        unit->SetCastSchoolImmunities(schoolImmunities);
        unit->SetCastImmunities(immunities);
    }

    uint32_t end = FrameTime::GetCurTimeMs() + remaining;
    uint32_t start = end - duration;
    int32_t signal;

    if (spell->m_attributesEx & 0x4) {
        unit->SetUIChannelDirect(spell, start, end);
        signal = 334;
    } else {
        WOWGUID targetGuid = target;
        unit->SetUICastingDirect(spell, targetGuid, start, end);
        signal = 327;
    }

    WOWGUID signalGuid = caster;
    Script_SendUnitSpellSignal(signalGuid, 0, spell, signal);
    return 1;
}

// OFFSET: 0x008A8590
void Spell_C_SetPlayerClass(int32_t classId) {
    ChrClassesRec* rec = g_chrClassesDB.GetRecord(classId);

    if (rec) {
        s_spellClassSet = rec->m_spellClassSet;
    }
}

// OFFSET: 0x008A8830/0x1405EBB50
void Spell_C_CancelChannelSpell(int32_t spellId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_CHANNELLING));
    msg.Put(static_cast<uint32_t>(spellId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008A88B0
bool SpellHasEffect(const SpellRec* spell, int32_t effect) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effect == effect) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A8900/0x1405EBC40
bool SpellHasAuraEffect(const SpellRec* spell, int32_t aura) {
    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && effect->m_effectAura == aura) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A8950
bool SpellHasNonStanceShapeshiftAuraEffect(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effectAura != 36) {
            continue;
        }

        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(effect->m_effectMiscValue);

        if (!form || !(form->m_flags & 0x1)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A89D0
bool SpellMatchesSpellClass(const SpellRec* spell, int32_t classSet, const int32_t* classMask) {
    const SpellClassOptionsRec* options = NULL;
    int32_t spellClassSet = 0;

    if (spell->m_spellClassOptionsID) {
        options = g_spellClassOptionsDB.GetRecord(spell->m_spellClassOptionsID);

        if (options) {
            spellClassSet = options->m_spellClassSet;
        }
    }

    if (spellClassSet != classSet || !options || !classMask) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (classMask[i] & options->m_spellClassMask[i]) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A8A40/0x1405EBDA0
bool SpellHasApplyAuraEffect(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && SpellIsApplyAuraEffect(effects->m_data[i]->m_effect)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A8AA0/0x1405EBE20
int32_t SpellIsTrackingAura(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        if (effect->m_effectAura == 44 || effect->m_effectAura == 151) {
            return 1;
        }

        if (effect->m_effectAura == 45) {
            return -1;
        }
    }

    return 0;
}

// OFFSET: 0x008A8B00/0x1405EBE90
bool SpellIsCallCompanion(const SpellRec* spell) {
    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (!effect) {
        return false;
    }

    if (effect->m_effect == 28) {
        SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);

        if (properties && properties->m_slot == 5) {
            return true;
        }
    }

    return effect->m_effectAura == 78;
}

// OFFSET: 0x008A8B60/0x1405EBEF0
bool SpellCancelsAura(const SpellRec* spell, const SpellRec* aura, const CGUnit_C* unit, uint32_t auraEffect, bool a5) {
    if (!spell || !(spell->m_attributesEx & 0x8000) || (aura->m_attributes & 0x20000000)) {
        return false;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    const SpellEffectRec* auraEffectRec = SpellGetEffect(aura->m_id, auraEffect);

    if (auraEffectRec && (auraEffectRec->m_effectAttributes & 0x1)) {
        return false;
    }

    int32_t dispelType = 0;
    int32_t mechanic = 0;

    if (aura->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(aura->m_spellCategoriesID);

        if (categories) {
            dispelType = categories->m_dispelType;
            mechanic = categories->m_mechanic;
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 6) {
            continue;
        }

        switch (effect->m_effectAura) {
            case 38: {
                if (auraEffectRec && effect->m_effectMiscValue == auraEffectRec->m_effectAura) {
                    return true;
                }

                break;
            }

            case 39:
            case 267: {
                if (!(aura->m_attributesExB & 0x4000000) && (effect->m_effectMiscValue & aura->m_schoolMask)) {
                    return true;
                }

                break;
            }

            case 41: {
                if (effect->m_effectMiscValue == dispelType) {
                    return true;
                }

                break;
            }

            case 77: {
                if (effect->m_effectMiscValue == mechanic || (auraEffectRec && effect->m_effectMiscValue == auraEffectRec->m_effectMechanic)) {
                    return true;
                }

                break;
            }

            case 147: {
                int32_t schoolMask = aura->m_schoolMask;
                int32_t effectMask = 1 << auraEffect;

                if (unit && unit->CheckSpellImmunity(aura, &schoolMask, effect->m_effectMiscValue, &effectMask, a5)) {
                    return true;
                }

                break;
            }
        }
    }

    return false;
}

// OFFSET: 0x008A8DC0
void Spell_C_SetAutoRepeatingSpell(int32_t spellId) {
    if (s_autoRepeatingSpell == spellId) {
        return;
    }

    s_autoRepeatingSpell = spellId;

    if (!s_autoRangedCombatSpell) {
        if (spellId) {
            FrameScript_SignalEvent(SCRIPT_EVENT_START_AUTOREPEAT_SPELL, NULL);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_STOP_AUTOREPEAT_SPELL, NULL);
        }
    }

    if (s_autoRepeatingSpell) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        player->MeleeModeLeave(true);
    }
}

// OFFSET: 0x008A8E30
bool OnCancelCombat() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !(player->m_obj->m_type & TYPEMASK_PLAYER) || !(player->m_combatModeFlags & 0x4)) {
        return false;
    }

    player->SetCombatMode(true);
    return true;
}

// OFFSET: 0x008A8E80/0x1405EC2F0
int32_t SpellFaction(const SpellRec* spell) {
    const SpellTargetRestrictionsRec* restrictions;

    if (spell->m_spellTargetRestrictionsID && (restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID))) {
        if (restrictions->m_targets & 0x100) {
            return 1;
        }

        if (restrictions->m_targets & 0x80) {
            return 2;
        }
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            switch ((&effect->m_implicitTargetA)[j]) {
                case 2:
                case 6:
                case 15:
                case 16:
                case 24:
                case 28:
                case 53:
                case 54:
                case 93:
                    return 2;
            }
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            switch ((&effect->m_implicitTargetA)[j]) {
                case 1:
                    if (effect->m_effectAura != 4) {
                        return 1;
                    }

                    break;

                case 3:
                case 4:
                case 5:
                case 20:
                case 21:
                case 27:
                case 29:
                case 30:
                case 31:
                case 33:
                case 34:
                case 35:
                case 45:
                case 56:
                case 57:
                case 58:
                case 59:
                case 61:
                case 62:
                case 118:
                case 119:
                case 121:
                    return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x008A9180/0x1405EC660
uint32_t GetRequiredAuraVisionBitmask(const SpellRec* spell) {
    uint32_t mask = 0;

    if (spell->m_spellCastingRequirementsID) {
        SpellCastingRequirementsRec* requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID);

        if (requirements && requirements->m_requiredAuraVision > 0) {
            mask = 1 << (requirements->m_requiredAuraVision - 1);
        }
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        if (effect->m_effectAura == 17) {
            mask |= 0x20;
        }

        if (effect->m_effectAura == 19 && (!effect->m_effectMiscValue || effect->m_effectMiscValue == 10)) {
            mask |= 0x40;
        }
    }

    return mask;
}

// OFFSET: 0x008A9220
bool Spell_C_SpellVisualHasChainEffect(const SpellVisualRec* visual) {
    SpellVisualKitRec* kit;

    if ((kit = g_spellVisualKitDB.GetRecord(visual->m_castKit)) && SpellVisualKitHasChainEffect(kit)) {
        return true;
    }

    if ((kit = g_spellVisualKitDB.GetRecord(visual->m_impactKit)) && SpellVisualKitHasChainEffect(kit)) {
        return true;
    }

    if ((kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit)) && SpellVisualKitHasChainEffect(kit)) {
        return true;
    }

    if ((kit = g_spellVisualKitDB.GetRecord(visual->m_channelKit)) && SpellVisualKitHasChainEffect(kit)) {
        return true;
    }

    if ((kit = g_spellVisualKitDB.GetRecord(visual->m_casterImpactKit)) && SpellVisualKitHasChainEffect(kit)) {
        return true;
    }

    return (kit = g_spellVisualKitDB.GetRecord(visual->m_targetImpactKit)) && SpellVisualKitHasChainEffect(kit);
}

// OFFSET: 0x008A9330
SpellVisualKitRec* Spell_C_GetAppropriateImpactKit(const SpellVisualRec* visual, int32_t caster) {
    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(caster ? visual->m_casterImpactKit : visual->m_targetImpactKit);

    if (kit) {
        return kit;
    }

    return g_spellVisualKitDB.GetRecord(visual->m_impactKit);
}

// OFFSET: 0x008A9390/0x1405EC910
bool Spell_C_HasNoReagentCost(CGPlayer_C* player, const SpellRec* spell) {
    if ((player->m_unit->m_unitFieldFlags & 0x20) && (spell->m_attributesExE & 0x2)) {
        return true;
    }

    const SpellClassOptionsRec* options;

    if (!spell->m_spellClassOptionsID || !(options = g_spellClassOptionsDB.GetRecord(spell->m_spellClassOptionsID)) || !options->m_spellClassSet || options->m_spellClassSet != s_spellClassSet) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (options->m_spellClassMask[i] & player->m_player->m_playerNoReagentCost1[i]) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008A9420
bool SpellIsPhysicalAttack(const SpellRec* spell) {
    const SpellCategoriesRec* categories;

    if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID)) && (categories->m_defenseType == 2 || categories->m_defenseType == 3)) {
        return true;
    }

    return (spell->m_attributes & 0x12) != 0;
}

// OFFSET: 0x008A9470
void Spell_C_PlayMissileTrajectoryCastVisuals(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, const SpellVisualRec* visual, const SpellVisualKitRec* kit, const TSStackArray<CGUnit_C*>* targets, float time) {
    if (!CanPlaySpellVisual(unit->m_obj->m_guid.m_guid, 0, visual)) {
        return;
    }

    if (CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, 0, kit, NULL)) {
        PlaySpellVisuaKitData data(spell, kit, 0);
        unit->PlaySpellVisualKit(data);
    }

    TSStackArray<WOWGUID> guids(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * targets->m_count)), targets->m_count, 0);
    guids.m_count = targets->m_count;

    for (uint32_t i = 0; i < targets->m_count; i++) {
        guids.m_data[i] = targets->m_data[i]->m_obj->m_guid;
    }

    TSStackArray<WOWGUID> misses(NULL, 0, 0);
    TSStackArray<uint32_t> missReasons(NULL, 0, 0);
    TSStackArray<uint32_t> reflectResults(NULL, 0, 0);

    if (spell->m_attributesExD & 0x40000) {
        LaunchMissileVisual(guids, misses, unit, spell, visual, 0, 0, cast ? &cast->m_data : NULL, missReasons, reflectResults, static_cast<uint32_t>(time * 1000.0f));
    } else {
        CreateMissilesForTargets(guids, unit, spell, visual, 0, 0, cast ? &cast->m_data : NULL, NULL, NULL, static_cast<uint32_t>(time * 1000.0f));
    }
}

// OFFSET: 0x008A9750
void Spell_C_PlayDestLocVisualsWithoutCaster(NotifyDestLocSpellCastData* data) {
    SpellRec* spell = g_spellDB.GetRecord(data->m_spellId);

    if (!spell) {
        return;
    }

    SpellVisualRec* visual = g_spellVisualDB.GetRecord(spell->m_spellVisualID[0]);

    if (!visual) {
        return;
    }

    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_instantAreaKit);

    if (kit) {
        uint32_t flags = spell->m_spellMissileID > 0 ? 0x402010 : 0x2010;
        SpellVisualEffectNameRec* effectName;

        if (kit->m_baseEffect && (effectName = g_spellVisualEffectNameDB.GetRecord(kit->m_baseEffect)) != NULL) {
            CEffect* effect = CEffect::Create();
            effect->AddOrphanedEffect(data->m_spellId, kit, effectName, flags, &CGObject_C::EffectOnceHandler, data->m_destPosition, 0, data->m_destTarget);
            effect->Release();
            flags &= ~0x10u;
        }

        if (kit->m_worldEffect && (effectName = g_spellVisualEffectNameDB.GetRecord(kit->m_worldEffect)) != NULL) {
            CEffect* effect = CEffect::Create();
            effect->AddOrphanedEffect(data->m_spellId, kit, effectName, flags, &CGObject_C::EffectOnceHandler, data->m_destPosition, 0, data->m_destTarget);
            effect->Release();
        }
    }

    CMissile* missiles = NULL;

    if (visual->m_hasMissile && visual->m_missileModel && spell->m_speed > 0.0f) {
        float scale = 1.0f;
        CM2Model* model;
        SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(visual->m_missileModel);

        if (!effectName || !*effectName->m_fileName || static_cast<uint8_t>(effectName->m_type) || (scale = effectName->m_scale, model = World::GetScene()->CreateModel(effectName->m_fileName, 32, 3), !model)) {
            model = World::GetScene()->CreateModel("Spells\\ErrorCube.mdx", 0, 3);
        }

        CMissile* missile = CMissile::NewMissile();
        missile->m_model = model;
        missile->m_casterGuid = data->m_caster;
        missile->m_sourceGuid = data->m_caster;
        missile->m_targetGuid = WOWGUID();
        missile->m_spellID = data->m_spellId;
        missile->m_spellVisualID = 0;
        missile->m_castID = data->m_castCount;
        missile->m_speed = spell->m_speed;
        missile->m_launchSpeed = spell->m_speed;
        missile->m_baseSpeed = spell->m_speed;
        missile->m_hitResult = 0;
        missile->m_missReason = 0;
        missile->m_flags = 1;
        missile->m_scale = scale;
        missile->m_soundKitID = visual->m_missileSound;
        missile->m_impactPos = data->m_destPosition;
        missile->m_attachmentID = -1;
        float height = visual->m_missileFollowGroundHeight * 0.001f;

        if (height <= 0.25f) {
            height = 0.25f;
        }

        missile->m_groundOffset = height;
        float rate = visual->m_missileFollowGroundDropSpeed * 0.001f;

        if (rate <= 0.01f) {
            rate = 0.01f;
        }

        missile->m_followGroundRate = rate;
        float start = visual->m_missileFollowGroundApproach * 0.001f;

        if (start < 0.0f) {
            start = 0.0f;
        } else if (start >= 1.0f) {
            start = 1.0f;
        }

        missile->m_followGroundStart = start;
        missile->m_motionFlags = visual->m_missileFollowGroundFlags;

        if (visual->m_missileFollowGroundFlags & 1) {
            missile->m_flags = 0x7801;
        }

        int32_t missileCount = 1;

        if (visual->m_missileMotion) {
            missile->m_flags |= 0xB800;
            missile->InstallMotionScriptFunction(visual->m_missileMotion);
            SpellMissileMotionRec* motion = g_spellMissileMotionDB.GetRecord(visual->m_missileMotion);

            if (motion) {
                missileCount = 1;

                if (motion->m_missileCount >= 1) {
                    missileCount = motion->m_missileCount;
                }
            }
        }

        missile->m_castTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
        missile->SetReadyToFire(&missiles);

        if (data->unk_0030 > 0.0f) {
            missile->m_pitch = data->m_elevation;
            missile->m_trajectorySpeed = data->unk_0030;
            missile->m_flags |= 0x10800;
            missile->m_arrivalTime = missile->m_castTime + data->unk_0034;
        }

        missile->m_missileCount = missileCount;

        if (missileCount > 1) {
            missile->m_missileIndex = 0;

            for (int32_t i = 1; i < missileCount; i++) {
                missile = CMissile::CopyMissile(missile);
                missile->m_missileIndex = i;
                missile->SetReadyToFire(&missiles);
            }
        }
    }

    if (!missiles) {
        SpellVisualKitRec* impactKit = g_spellVisualKitDB.GetRecord(visual->m_impactAreaKit);

        if (!impactKit) {
            return;
        }

        uint32_t delay = data->unk_0034 + CGWorldFrame::s_currentWorldFrame->GetSceneTime();
        uint32_t flags = spell->m_spellMissileID > 0 ? 0x402010 : 0x2010;
        SpellVisualEffectNameRec* effectName;

        if (impactKit->m_baseEffect && (effectName = g_spellVisualEffectNameDB.GetRecord(impactKit->m_baseEffect)) != NULL) {
            CEffect* effect = CEffect::Create();
            effect->AddOrphanedEffect(data->m_spellId, impactKit, effectName, flags, &CGObject_C::EffectOnceHandler, data->m_destPosition, delay, data->m_destTarget);
            effect->Release();
            flags &= ~0x10u;
        }

        if (impactKit->m_worldEffect && (effectName = g_spellVisualEffectNameDB.GetRecord(impactKit->m_worldEffect)) != NULL) {
            CEffect* effect = CEffect::Create();
            effect->AddOrphanedEffect(data->m_spellId, impactKit, effectName, flags, &CGObject_C::EffectOnceHandler, data->m_destPosition, delay, data->m_destTarget);
            effect->Release();
        }
    }

    while (missiles) {
        if (missiles->m_model) {
            missiles->m_model->m_fadeAlpha = 1.0f;
        }

        missiles->FireMissiles(&data->m_destLocation);
    }
}

// OFFSET: 0x008A9C80/0x1405ED470
bool Spell_C_SpellHasShapeshiftMask(const SpellRec* spell, int32_t form) {
    int32_t mask = 0;
    const SpellShapeshiftRec* shapeshift;

    if (spell->m_spellShapeshiftID && (shapeshift = g_spellShapeshiftDB.GetRecord(spell->m_spellShapeshiftID))) {
        mask = shapeshift->m_shapeshiftMask[form / 32];
    }

    return form >= 0 && (mask & (1 << (form & 0x1F)));
}

// OFFSET: 0x008A9CF0
bool Spell_C_SpellHasShapeshiftExclude(const SpellRec* spell, int32_t form) {
    int32_t mask = 0;
    const SpellShapeshiftRec* shapeshift;

    if (spell->m_spellShapeshiftID && (shapeshift = g_spellShapeshiftDB.GetRecord(spell->m_spellShapeshiftID))) {
        mask = shapeshift->m_shapeshiftExclude[form / 32];
    }

    return form >= 0 && (mask & (1 << (form & 0x1F)));
}

// OFFSET: 0x008A9D60/0x1405ED570
int32_t SpellGetSpellIDForDifficulty(int32_t spellId, int32_t difficulty) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);
    const SpellDifficultyRec* rec;

    if (!spell || !spell->m_difficulty || !(rec = g_spellDifficultyDB.GetRecord(spell->m_difficulty))) {
        return spellId;
    }

    if (rec->m_difficultySpellID[difficulty]) {
        return rec->m_difficultySpellID[difficulty];
    }

    if (difficulty == 3 && rec->m_difficultySpellID[1]) {
        return rec->m_difficultySpellID[1];
    }

    if (rec->m_difficultySpellID[0]) {
        return rec->m_difficultySpellID[0];
    }

    return spellId;
}

// OFFSET: 0x008A9DE0
bool Spell_C_CannotBeResurrected() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return true;
    }

    return (player->m_auraEffectMask[0x27] & 0x4) || !CGInstanceEncounter_C::CanResurrect(player->m_player->m_playerSelfResSpell);
}

// OFFSET: 0x008A9E30/0x1405ED600
int32_t Spell_C_GetSpellClassSet(const SpellRec* spell) {
    const SpellClassOptionsRec* options;

    if (spell && spell->m_spellClassOptionsID && (options = g_spellClassOptionsDB.GetRecord(spell->m_spellClassOptionsID))) {
        return options->m_spellClassSet;
    }

    return 0;
}

// OFFSET: 0x008A9E70
int32_t SpellScalingGetClassIndex(const SpellScalingRec* scaling) {
    if (scaling->m_class >= 0) {
        return scaling->m_class - 1;
    }

    return g_chrClassesDB.m_maxID - scaling->m_class - 1;
}

// OFFSET: 0x008A9E90
bool SpellAuraVisibilityHasTalentTab(int32_t auraVisibility, int32_t talentTab) {
    SpellAuraVisXTalentTabRec key;
    key.m_spellAuraVisibilityID = auraVisibility;
    key.m_talentTabID = talentTab;
    return bsearch(&key, g_spellAuraVisXTalentTabDB.m_numRecords <= 0 ? NULL : g_spellAuraVisXTalentTabDB.m_records, g_spellAuraVisXTalentTabDB.m_numRecords, sizeof(SpellAuraVisXTalentTabRec), CompareSpellAuraVisibility) != NULL;
}

// OFFSET: 0x008A9EE0
void Spell_C_GetDefaultMinRange(const SpellRec* spell, float* minRange) {
    SpellRangeRec* range = g_spellRangeDB.GetRecord(spell->m_rangeIndex);

    if (!range) {
        return;
    }

    int32_t friendly = SpellFaction(spell) == 1;

    if (range->m_flags & 0x1) {
        *minRange = 0.0f;
    } else if (range->m_flags & 0x7FFFFFFF) {
        *minRange = 5.0f;
        *minRange = range->m_rangeMin[friendly] + 5.0f;
    } else {
        *minRange = range->m_rangeMin[friendly];
    }
}

// OFFSET: 0x008A9F60/0x1405ED6F0
int32_t Spell_C_GetSpellReagentUses(int32_t spellId) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        return Spell_C_GetSpellReagentUses(spell);
    }

    return 0;
}

// OFFSET: 0x008A9FA0/0x1405ED730
bool Spell_C_IsActiveSpell(const SpellRec* spell, const CGUnit_C* unit) {
    if (!unit) {
        return false;
    }

    if (!spell->m_activeIconID && !SpellIsCallCompanion(spell)) {
        return false;
    }

    CGUnit_C* vehicle = unit->m_vehiclePassenger ? unit->m_vehiclePassenger->GetVehicleUnit() : NULL;
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 28) {
            continue;
        }

        if (vehicle && vehicle->m_obj->m_entryID == effect->m_effectMiscValue && vehicle->m_unit->m_unitFieldCreatedby == ClntObjMgrGetActivePlayer()) {
            return true;
        }

        SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);

        if (properties && properties->m_slot == 5) {
            return static_cast<int32_t>((unit->m_unit->m_unitFieldCritter.m_guid >> 32) & 0xFFFFF) == effect->m_effectMiscValue;
        }
    }

    bool active = false;

    for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
        const AuraData* aura = &const_cast<CGUnit_C*>(unit)->m_auras[i];

        if (aura->m_spellId != spell->m_id) {
            continue;
        }

        if (const_cast<CGUnit_C*>(unit)->IsActivePlayer() && SpellHasAuraEffect(spell, 36) && unit->m_shapeshiftSuppressed) {
            active = false;
            break;
        }

        if (aura->m_flags & 0x10) {
            active = true;
            break;
        }
    }

    if (count && effects->m_data[0] && !active && effects->m_data[0]->m_effectAura == 78) {
        for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
            const SpellEffectRec* mount = SpellGetEffect(const_cast<CGUnit_C*>(unit)->m_auras[i].m_spellId, 0);

            if (mount && mount->m_effectAura == 78 && mount->m_effectMiscValue == effects->m_data[0]->m_effectMiscValue) {
                return true;
            }
        }

        return false;
    }

    return active;
}

// OFFSET: 0x008AA210/0x1405EDA20
void Spell_C_CancelAura(const SpellRec* spell, CGUnit_C* unit, int32_t auraIndex) {
    if (!spell || !unit) {
        return;
    }

    if (!unit->IsActivePlayer()) {
        if (SpellHasApplyAuraEffect(spell)) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_PET_CANCEL_AURA));
            msg.Put(unit->m_obj->m_guid.m_guid);
            msg.Put(static_cast<uint32_t>(spell->m_id));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }

        return;
    }

    if (unit->m_unit->m_unitFieldCharmedby) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 28) {
            continue;
        }

        SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);

        if (properties && properties->m_slot == 5 && static_cast<int32_t>((unit->m_unit->m_unitFieldCritter.m_guid >> 32) & 0xFFFFF) == effect->m_effectMiscValue) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_DISMISS_CRITTER));
            msg.Put(unit->m_unit->m_unitFieldCritter.m_guid);
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    if (!SpellHasApplyAuraEffect(spell)) {
        return;
    }

    if (unit->m_mountDisplayId > 0 && !(unit->m_clientFlags & 0x10000000) && SpellHasAuraEffect(spell, 78)) {
        for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
            SpellRec* mount = g_spellDB.GetRecord(unit->m_auras[i].m_spellId);

            if (!mount) {
                continue;
            }

            const SpellEffectRec* mountEffect = SpellGetEffect(mount->m_id, 0);

            if (mountEffect && mountEffect->m_effectAura == 78 && mountEffect->m_effectMiscValue == effects->m_data[0]->m_effectMiscValue && mount->m_id != spell->m_id) {
                Spell_C_CancelAura(mount, unit, -1);
                return;
            }
        }

        unit->OnMountDisplayChanged(0);
    }

    if (unit->GetShapeshiftForm() && SpellHasAuraEffect(spell, 36)) {
        if (lua_tainted) {
            for (uint32_t i = s_shapeshiftSpells.m_count; i; ) {
                i--;

                if (s_shapeshiftSpells.m_data[i].m_spellId == spell->m_id) {
                    return;
                }
            }
        }

        for (uint32_t i = 0; i < count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect || effect->m_effectAura != 36) {
                continue;
            }

            SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(effect->m_effectMiscValue);

            if (!form || (form->m_flags & 0x2)) {
                return;
            }

            if (unit->GetShapeshiftForm() == static_cast<uint32_t>(effect->m_effectMiscValue)) {
                unit->PreemptivelyCancelShapeshift();
                break;
            }
        }
    }

    if ((spell->m_attributesEx & 0x2000) && !(spell->m_attributesEx & 0x4) && (unit->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(unit)->ToggleFarSight(NULL)) {
        return;
    }

    if (spell->m_id == 2584) {
        FrameScript_SignalEvent(SCRIPT_EVENT_AREA_SPIRIT_HEALER_OUT_OF_RANGE, NULL);
    }

    if (SpellHasAuraEffect(spell, 236)) {
        const WOWGUID* controlled = &unit->m_unit->m_unitFieldCharm;

        if (!*controlled) {
            controlled = &unit->m_unit->m_unitFieldSummon;
        }

        if (controlled->m_guid == unit->GetTransportGUID()) {
            WOWGUID transport = { unit->GetTransportGUID() };
            CGUnit_C* transportUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__));
            VehicleSeatRec* seatRec;
            CGUnit_C* mover;

            if (transportUnit && transportUnit->m_vehicle && (seatRec = transportUnit->m_vehicle->GetVehicleSeatRec(unit->m_movementData.m_transportSeat)) && (seatRec->m_flags & 0x2000000) && (mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
                mover->VehiclePassengerRequestExit();

                if (auraIndex >= 0 && unit->GetAuraCaster(auraIndex) == unit->m_obj->m_guid.m_guid) {
                    return;
                }
            }
        }
    }

    CGUnit_C* vehicleUnit = unit->m_vehiclePassenger ? unit->m_vehiclePassenger->GetVehicleUnit() : NULL;

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && effect->m_effect == 28 && vehicleUnit && vehicleUnit->m_obj->m_entryID == effect->m_effectMiscValue) {
            CGInputControl* input = CGInputControl::GetActive();
            CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (mover && input->IsVehicleSeatFlagSet(0x2000000)) {
                mover->VehiclePassengerRequestExit();
            }
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_AURA));
    msg.Put(static_cast<uint32_t>(spell->m_id));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008AA790
void PlayImpactVisual(const SpellRec* spell, const SpellVisualRec* visual, const TSStackArray<WOWGUID>& targets, CGUnit_C* caster) {
    if (!spell || !visual || !targets.m_count) {
        return;
    }

    SpellVisualKitRec* casterKit = g_spellVisualKitDB.GetRecord(visual->m_casterImpactKit);

    if (!casterKit) {
        casterKit = g_spellVisualKitDB.GetRecord(visual->m_impactKit);
    }

    SpellVisualKitRec* targetKit = g_spellVisualKitDB.GetRecord(visual->m_targetImpactKit);

    if (!targetKit) {
        targetKit = g_spellVisualKitDB.GetRecord(visual->m_impactKit);
    }

    if (!casterKit && !targetKit) {
        return;
    }

    WOWGUID casterGuid = caster ? caster->m_obj->m_guid : WOWGUID();

    if (!CanPlaySpellVisual(casterGuid.m_guid, 0, visual)) {
        return;
    }

    for (uint32_t i = 0; i < targets.m_count; i++) {
        CGObject_C* target = ClntObjMgrObjectPtr(targets.m_data[i], TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!target || !CanPlaySpellVisual(0, targets.m_data[i].m_guid, visual)) {
            continue;
        }

        SpellVisualKitRec* kit = target == caster ? casterKit : targetKit;

        if (CanPlaySpellVisualKit(casterGuid.m_guid, target->m_obj->m_guid.m_guid, kit, NULL)) {
            PlaySpellVisuaKitData data(spell, kit, 1);
            data.unk_0024 = SpellFaction(spell) == 2 && (kit->m_flags & 0x800);
            data.m_someGUID = casterGuid;
            target->PlaySpellVisualKit(data);
        }
    }
}

// OFFSET: 0x008AA960
void PlayImpactVisualOnTarget(const SpellRec* spell, const SpellVisualRec* visual, CGUnit_C* target, CGUnit_C* caster) {
    WOWGUID guid = target->m_obj->m_guid;
    TSStackArray<WOWGUID> targets(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID))), 1, 0);
    targets.m_count = 1;
    targets.m_data[0] = guid;
    PlayImpactVisual(spell, visual, targets, caster);
}

// OFFSET: 0x008AA9B0/0x1405EE4B0
bool Spell_C_GetSpellModifiers(const SpellRec* spell, int32_t modifier, float& flat, float& pct) {
    const SpellClassOptionsRec* options = NULL;
    int32_t classSet = 0;

    if (spell->m_spellClassOptionsID) {
        options = g_spellClassOptionsDB.GetRecord(spell->m_spellClassOptionsID);

        if (options) {
            classSet = Spell_C_GetSpellClassSet(spell);
        }
    }

    flat = 0.0f;

    if (!classSet || classSet != s_spellClassSet) {
        pct = 100.0f;
        return false;
    }

    if (spell->m_attributesExC & 0x20000000) {
        pct = 100.0f;
        return false;
    }

    pct = 0.0f;

    for (uint32_t i = 0; i < 96; i++) {
        if (options->m_spellClassMask[i >> 5] & (1u << (i & 0x1F))) {
            flat = s_spellFlatModifiers[i][modifier] + flat;
            pct = s_spellPctModifiers[i][modifier] + pct;
        }
    }

    if (fabs(flat) < 2.3841858e-7f && fabs(pct) < 2.3841858e-7f) {
        return false;
    }

    pct = pct + 100.0f;

    if (pct < 0.0f) {
        pct = 0.0f;
    }

    return true;
}

// OFFSET: 0x008AAC40/0x1405EE7E0
bool Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier) {
    float flat;
    float pct;

    if (!Spell_C_GetSpellModifiers(spell, modifier, flat, pct)) {
        return false;
    }

    value = static_cast<int32_t>((static_cast<float>(value) + flat) * pct * 0.0099999998f);
    return true;
}

// OFFSET: 0x008AAC90/0x1405EE830
bool Spell_C_ModifySpellValue(const SpellRec* spell, float& value, int32_t modifier) {
    float flat;
    float pct;

    if (!Spell_C_GetSpellModifiers(spell, modifier, flat, pct)) {
        return false;
    }

    value = (value + flat) * pct * 0.0099999998f;
    return true;
}

// OFFSET: 0x008AACE0
bool Spell_C_ModifySpellValue(const SpellRec* spell, CSoftFloat& value, int32_t modifier) {
    float flat;
    float pct;

    if (!Spell_C_GetSpellModifiers(spell, modifier, flat, pct)) {
        return false;
    }

    value = (value + *reinterpret_cast<const CSoftFloat*>(&flat)) * *reinterpret_cast<const CSoftFloat*>(&pct) * s_softPercent;
    return true;
}

// OFFSET: 0x008AAD60
bool Spell_C_ModifyHealingAndDamage(const SpellRec* spell, CSoftFloat& minValue, CSoftFloat& maxValue, bool healing) {
    float flat;
    float pct;

    if (!Spell_C_GetSpellModifiers(spell, healing ? 22 : 0, flat, pct)) {
        return false;
    }

    minValue = (minValue + *reinterpret_cast<const CSoftFloat*>(&flat)) * *reinterpret_cast<const CSoftFloat*>(&pct) * s_softPercent;
    maxValue = (maxValue + *reinterpret_cast<const CSoftFloat*>(&flat)) * *reinterpret_cast<const CSoftFloat*>(&pct) * s_softPercent;
    return true;
}

// OFFSET: 0x008AB1B0/0x1405EEF40
bool SpellMatchesAuraSpellClass(const SpellRec* spell, const SpellRec* aura, int32_t effectIndex) {
    if (!spell || !aura) {
        return false;
    }

    int32_t classSet = Spell_C_GetSpellClassSet(aura);
    const SpellEffectRec* effect = SpellGetEffect(aura->m_id, effectIndex);

    if (effect) {
        return SpellMatchesSpellClass(spell, classSet, &effect->m_effectSpellClassMaskA);
    }

    return SpellMatchesSpellClass(spell, classSet, NULL);
}

// OFFSET: 0x008AB210/0x1405EEFE0
bool Spell_C_CancelsAuraEffect(const CGUnit_C* unit, const SpellRec* spell, int32_t auraType, int32_t* reason) {
    bool found = false;

    for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
        int32_t spellId = const_cast<CGUnit_C*>(unit)->m_auras[i].m_spellId;
        SpellRec* aura;

        if (!spellId || !(aura = g_spellDB.GetRecord(spellId))) {
            continue;
        }

        int32_t mechanic = 0;

        if (aura->m_spellCategoriesID) {
            SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(aura->m_spellCategoriesID);

            if (categories) {
                mechanic = categories->m_mechanic;
            }
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(aura->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect || effect->m_effectAura != auraType) {
                continue;
            }

            found = true;
            bool negative = (const_cast<CGUnit_C*>(unit)->GetAura(i)->m_flags >> 7) & 1;

            if (!SpellCancelsAura(spell, aura, unit, j, negative)) {
                if (reason) {
                    int32_t value = effects->m_data[j]->m_effectMechanic;

                    if (value || (value = mechanic) != 0) {
                        *reason = value;
                    }
                }

                return false;
            }
        }
    }

    return found;
}

// PROVEN: 441e5fff IDENTICAL/NONE
// OFFSET: 0x008AB390/0x1405EF1A0
bool Spell_C_CancelsStun(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 12, reason);
}

// PROVEN: 7e1cda84 IDENTICAL/NONE
// OFFSET: 0x008AB3C0/0x1405EF1C0
bool Spell_C_CancelsSilence(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 27, reason) || Spell_C_CancelsAuraEffect(unit, spell, 60, reason);
}

// PROVEN: 954c4495 IDENTICAL/NONE
// OFFSET: 0x008AB410/0x1405EF230
bool Spell_C_CancelsPacify(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 25, reason) || Spell_C_CancelsAuraEffect(unit, spell, 60, reason);
}

// OFFSET: 0x008AB460/0x1405EF2A0
bool Spell_C_CancelsNoActions(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 321, reason);
}

// OFFSET: 0x008AB490/0x1405EF2C0
bool Spell_C_CancelsFear(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 7, reason);
}

// OFFSET: 0x008AB4C0/0x1405EF2E0
bool Spell_C_CancelsConfuse(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 5, reason);
}

// PROVEN: 5adf21d0 IDENTICAL/NONE
// OFFSET: 0x008AB4F0/0x1405EF300
bool Spell_C_CancelsCharm(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason) {
    if (reason) {
        *reason = 0;
    }

    return Spell_C_CancelsAuraEffect(unit, spell, 6, reason) || Spell_C_CancelsAuraEffect(unit, spell, 177, reason) || Spell_C_CancelsAuraEffect(unit, spell, 2, reason);
}

// OFFSET: 0x008AB550
int32_t Spell_C_GetAuraPeriod(const SpellRec* spell, uint32_t effectIndex) {
    if (!spell) {
        return 0;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, effectIndex);
    int32_t period;

    if (!effect || !(period = effect->m_effectAuraPeriod)) {
        period = 5200;
    }

    float flat;
    float pct;

    if (Spell_C_GetSpellModifiers(spell, 19, flat, pct)) {
        period = static_cast<int32_t>((static_cast<float>(period) + flat) * pct * 0.0099999998f);
    }

    if ((spell->m_attributesExE & 0x2000) && !(spell->m_attributesExC & 0x20000000)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            float haste = player->m_unit->m_unitModCastHaste;

            if (haste >= 0.001f) {
                return static_cast<int32_t>(static_cast<float>(period) * haste);
            }
        }
    }

    return period;
}

// OFFSET: 0x008AB640/0x1405EF470
int32_t Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId) {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map) {
        return spellId;
    }

    if (map->m_instanceType == 2) {
        return SpellGetSpellIDForDifficulty(spellId, CGGameUI::GetCurrentRaidDifficulty());
    }

    if (map->m_instanceType != 1) {
        return spellId;
    }

    return SpellGetSpellIDForDifficulty(spellId, CGGameUI::GetCurrentDungeonDifficulty());
}

// OFFSET: 0x008AB6B0
float Spell_C_GetPointsPerCombo(const CGUnit_C* unit, const SpellRec* spell, int32_t effectIndex) {
    float points = 0.0f;
    SpellScalingRec* scaling;

    if (unit && (scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID))) {
        int32_t classIndex = scaling->m_class;

        if (!classIndex) {
            ConsoleWriteA("Class not defined for Spell Scaling ID %d in spell %d", DEFAULT_COLOR, scaling->m_id, spell->m_id);
            return 0.0f;
        }

        if (classIndex < 0) {
            classIndex = g_chrClassesDB.m_maxID - classIndex;
        }

        float value = GameFormulaEvaluateTable(12, unit->m_unit->m_unitFieldLevel - 1, classIndex - 1);
        points = static_cast<float>(static_cast<int32_t>(Spell_C_GetSpellScalingMultiplier(unit, scaling, false) * value * scaling->m_comboPointsCoefficient[effectIndex]));
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, effectIndex);

    if (effect && fabsf(points) < 0.00000023841858f && effect->m_effectPointsPerResource != 0.0f) {
        return effect->m_effectPointsPerResource;
    }

    return points;
}

// OFFSET: 0x008AB7E0
void PendingSpellCast::AddOtherTarget(WOWGUID target) {
    uint32_t index = this->m_otherTargets.m_count;
    this->m_otherTargets.SetCount(index + 1);
    this->m_otherTargets.m_data[index] = target;
}

// OFFSET: 0x008AB860/0x1405EF700
int32_t SpellHistory::RecentlyCast(int32_t spellId, int32_t itemId) {
    uint32_t now = OsGetAsyncTimeMs();

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = this->m_history.Next(history)) {
        if (history->m_spellId == spellId && history->m_itemId == itemId && static_cast<int32_t>(now - history->m_time - 1500) < 0) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008AB8C0/0x1405EF790
int32_t SpellHistory::IsOnHold(int32_t itemId) {
    int32_t spellId = 0;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = this->m_history.Next(history)) {
        if (history->m_itemId == itemId) {
            if (history->m_onHold) {
                return 1;
            }

            spellId = history->m_spellId;
            break;
        }
    }

    int32_t category = 0;
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories) {
            category = categories->m_category;
        }
    }

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = this->m_history.Next(history)) {
        if (history->m_spellId != spellId && history->m_category == category && history->m_categoryRecoveryTime && history->m_onHold) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008AB9A0/0x1405EF880
void SpellHistory::ClearOnHold(int32_t spellId, uint32_t time, bool remove) {
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_spellId != spellId) {
            continue;
        }

        if (history->m_onHold) {
            if (!remove) {
                history->m_startRecoveryTime = 0;
                history->m_time = time;
                history->m_categoryTime = time;
                history->m_onHold = 0;
                continue;
            }
        } else if (!remove) {
            continue;
        }

        history->m_link.Unlink();
        this->m_free.LinkToTail(history);
    }
}

// OFFSET: 0x008ABA40
void SpellHistory::DelayCooldown(int32_t spellId, int32_t delay) {
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_spellId == spellId) {
            history->m_time += delay;
            history->m_categoryTime += delay;
        }
    }
}

// OFFSET: 0x008ABA80
void SpellHistory::ClearAll() {
    while (SPELLHISTORY* history = this->m_history.Head()) {
        this->m_free.LinkToTail(history);
    }
}

// OFFSET: 0x008ABB30
void SpellHistory::GarbageCollect(uint32_t time) {
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_onHold) {
            continue;
        }

        if (history->m_recoveryTime && static_cast<int32_t>(time - history->m_time - history->m_recoveryTime) < 0) {
            continue;
        }

        if (history->m_categoryRecoveryTime && static_cast<int32_t>(time - history->m_categoryTime - history->m_categoryRecoveryTime) < 0) {
            continue;
        }

        if (history->m_startRecoveryTime && static_cast<int32_t>(time - history->m_time - history->m_startRecoveryTime) < 0) {
            continue;
        }

        history->m_link.Unlink();
        this->m_free.LinkToTail(history);
    }
}

// OFFSET: 0x008ABBF0
int32_t Spell_C_WasSpellRecentlyUsed(int32_t spellId, int32_t pet) {
    if (!g_secureAbilityToggleCVar->m_intValue) {
        return 0;
    }

    return s_spellHistory[pet].RecentlyCast(spellId, 0);
}

// OFFSET: 0x008ABC20
void Spell_C_ModifyCooldown(int32_t index, int32_t spellId, int32_t delay) {
    s_spellHistory[index].DelayCooldown(spellId, delay);
    CGSpellBook::UpdateCooldowns();
    CGSpellBook::UpdateUsable();

    if (index) {
        CGPetInfo::UpdateCooldowns();
    } else {
        CGContainerInfo::UpdateCooldowns();
    }
}

// OFFSET: 0x008ABC60
void Spell_C_ClearSpellHistory(int32_t index) {
    s_spellHistory[index].ClearAll();
    CGSpellBook::UpdateCooldowns();

    if (index) {
        CGPetInfo::UpdateCooldowns();
    } else {
        CGContainerInfo::UpdateCooldowns();
    }
}

// OFFSET: 0x008ABC90/0x1405EFCB0
int32_t Spell_C_GetPowerCostPerSecond(const SpellRec* spell, const CGUnit_C* unit) {
    if (!unit) {
        return 0;
    }

    int32_t cost = 0;
    const SpellPowerRec* power;

    if (spell->m_spellPowerID && (power = g_spellPowerDB.GetRecord(spell->m_spellPowerID))) {
        cost = power->m_manaPerSecond;

        if (cost > 0) {
            float flat;
            float pct;

            if (Spell_C_GetSpellModifiers(spell, 14, flat, pct)) {
                return static_cast<int32_t>(static_cast<float>(cost) * pct * 0.0099999998f);
            }
        }
    }

    return cost;
}

// OFFSET: 0x008ABD10/0x1405EFD50
void Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, float* minPoints, float* maxPoints, int32_t spellLevel, int32_t isPet, int32_t isVehicle, int32_t noModifiers, int32_t noBonus) {
    *minPoints = 0.0f;
    *reinterpret_cast<uint32_t*>(maxPoints) = 0;

    if (!spell) {
        return;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, effectIndex);

    if (!effect) {
        return;
    }

    bool rounded;
    bool periodic;
    bool damage = GetEffectModifierType(effect->m_effect, &periodic, effect->m_effectAura, &rounded);
    bool scaled = false;
    bool exact = false;
    SpellScalingRec* scaling = NULL;
    CSoftFloat minValue;
    CSoftFloat maxValue;

    if (spell->m_spellScalingID && (scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID))) {
        CGUnit_C* unit = Spell_C_GetStatTargetUnit(isPet, isVehicle);

        if (fnotequal(scaling->m_coefficient[effectIndex], 0.0f)) {
            float multiplier = 1.0f;

            if (unit) {
                if (!scaling->m_class) {
                    ConsoleWriteA("Class not defined for Spell Scaling ID %d in spell %d", DEFAULT_COLOR, scaling->m_id, spell->m_id);
                    return;
                }

                int32_t classIndex = SpellScalingGetClassIndex(scaling);
                float table = GameFormulaEvaluateTable(12, unit->m_unit->m_unitFieldLevel - 1, classIndex);
                multiplier = Spell_C_GetSpellScalingMultiplier(unit, scaling, false) * table;
            }

            float value = scaling->m_coefficient[effectIndex] * multiplier;

            if (value > 0.0f && value < 1.0f) {
                value = 1.0f;
            }

            float variance = scaling->m_variance[effectIndex];
            int32_t points = static_cast<int32_t>(value);

            if (fequal(variance, 0.0f)) {
                maxValue = CSoftFloat::FromInt(points);
                minValue = maxValue;
                exact = true;
            } else {
                int32_t half = static_cast<int32_t>(static_cast<float>(points) * variance) / 2;
                minValue = CSoftFloat::FromInt(points - half);
                maxValue = CSoftFloat::FromInt(points + half);
            }

            scaled = true;
            goto modifiers;
        }
    }

    {
        int32_t level = spellLevel;

        if (!level) {
            level = Spell_C_GetSpellLevel(spell, isPet, isVehicle);
        }

        level = Spell_C_GetSpellLevelBonus(spell, level);
        int32_t dieSides = effect->m_effectDieSides;
        CSoftFloat perLevel = *reinterpret_cast<const CSoftFloat*>(&effect->m_effectRealPointsPerLevel);
        minValue = CSoftFloat::FromInt((dieSides != 0) + effect->m_effectBasePoints) + perLevel * CSoftFloat::FromInt(level);
        maxValue = CSoftFloat::FromInt(dieSides + effect->m_effectBasePoints) + perLevel * CSoftFloat::FromInt(level);

        if (!dieSides) {
            exact = true;
        }
    }

modifiers:
    if (!noModifiers) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        bool applyBonuses = !(spell->m_attributesExC & 0x20000000);

        if ((spell->m_attributesExH & 0x20000000) && player && !isPet && !isVehicle) {
            float mastery = effect->m_effectBonusMultiplier * player->m_player->m_playerMastery;
            minValue = minValue + *reinterpret_cast<const CSoftFloat*>(&mastery);
            maxValue = maxValue + *reinterpret_cast<const CSoftFloat*>(&mastery);
        }

        Spell_C_ModifySpellValue(spell, minValue, 8);
        Spell_C_ModifySpellValue(spell, maxValue, 8);

        if (effectIndex == 0) {
            Spell_C_ModifySpellValue(spell, minValue, 3);
            Spell_C_ModifySpellValue(spell, maxValue, 3);
        } else if (effectIndex == 1) {
            Spell_C_ModifySpellValue(spell, minValue, 12);
            Spell_C_ModifySpellValue(spell, maxValue, 12);
        } else if (effectIndex == 2) {
            Spell_C_ModifySpellValue(spell, minValue, 23);
            Spell_C_ModifySpellValue(spell, maxValue, 23);
        }

        if (damage) {
            if (isPet) {
                if (!noBonus) {
                    Spell_C_ModifyHealingAndDamage(spell, minValue, maxValue, periodic);
                }
            } else if (player) {
                float coefficient = effect->m_effectBonusMultiplier;

                if (scaled) {
                    coefficient = Spell_C_GetSpellScalingMultiplier(player, scaling, false) * coefficient;
                }

                if (coefficient > 0.0f) {
                    coefficient = coefficient * 100.0f;
                    Spell_C_ModifySpellValue(spell, coefficient, 24);
                    coefficient = coefficient * 0.0099999998f;
                }

                if (SpellIsDamageEffect(effect->m_effect, effect->m_effectAura)) {
                    if (applyBonuses && effect->m_effect != 31) {
                        CSoftFloat bonus = CSoftFloat::FromInt(player->GetModDamageDoneNeg(spell->m_schoolMask) + player->GetModDamageDonePos(spell->m_schoolMask));
                        bonus = bonus * *reinterpret_cast<const CSoftFloat*>(&coefficient);

                        if (spell->m_spellCoefficient > 0.0f) {
                            float apCoefficient = spell->m_spellCoefficient;
                            Spell_C_ModifySpellValue(spell, apCoefficient, 24);
                            CGUnitData* data = player->m_unit;
                            float attackPower = static_cast<float>(data->m_unitFieldAttackPower + data->m_unitFieldAttackPowerModPos + data->m_unitFieldAttackPowerModNeg) * (data->m_unitFieldAttackPowerMultiplier + 1.0f);
                            float rangedAttackPower = static_cast<float>(data->m_unitFieldRangedAttackPower + data->m_unitFieldRangedAttackPowerModPos + data->m_unitFieldRangedAttackPowerModNeg) * (data->m_unitFieldRangedAttackPowerMultiplier + 1.0f);

                            if (attackPower <= rangedAttackPower) {
                                attackPower = rangedAttackPower;
                            }

                            float apBonus = attackPower * apCoefficient;
                            bonus = bonus + *reinterpret_cast<const CSoftFloat*>(&apBonus);
                        }

                        minValue = minValue + bonus;
                        maxValue = maxValue + bonus;
                    }

                    if (!noBonus) {
                        Spell_C_ModifyHealingAndDamage(spell, minValue, maxValue, periodic);
                    }

                    if (applyBonuses && effect->m_effect != 31) {
                        float pct = player->GetModDamageDonePct(spell->m_schoolMask);
                        minValue = minValue * *reinterpret_cast<const CSoftFloat*>(&pct);
                        pct = player->GetModDamageDonePct(spell->m_schoolMask);
                        maxValue = maxValue * *reinterpret_cast<const CSoftFloat*>(&pct);
                    }

                    int32_t effectType = effect->m_effect;

                    if (effectType == 58 || effectType == 17 || effectType == 121) {
                        uint32_t count;
                        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

                        for (uint32_t i = effectIndex; i < count; i++) {
                            if (effects->m_data[i] && effects->m_data[i]->m_effect == 31) {
                                float low;
                                float high;
                                Spell_C_GetMinMaxPoints(spell, i, &low, &high, spellLevel, 0, isVehicle, 0, 1);
                                float lowPct = low * 0.0099999998f;
                                minValue = minValue * *reinterpret_cast<const CSoftFloat*>(&lowPct);
                                float highPct = high * 0.0099999998f;
                                maxValue = maxValue * *reinterpret_cast<const CSoftFloat*>(&highPct);
                            }
                        }

                        if (applyBonuses) {
                            int32_t attributes = spell->m_attributesExC;
                            float weapon;

                            if (attributes & 0x400) {
                                weapon = player->m_player->m_playerFieldWeaponDmgMultipliers[0];
                                minValue = minValue * *reinterpret_cast<const CSoftFloat*>(&weapon);
                                weapon = player->m_player->m_playerFieldWeaponDmgMultipliers[0];
                                maxValue = maxValue * *reinterpret_cast<const CSoftFloat*>(&weapon);
                            } else if (attributes & 0x1000000) {
                                weapon = player->m_player->m_playerFieldWeaponDmgMultipliers[1];
                                minValue = minValue * *reinterpret_cast<const CSoftFloat*>(&weapon);
                                weapon = player->m_player->m_playerFieldWeaponDmgMultipliers[1];
                                maxValue = maxValue * *reinterpret_cast<const CSoftFloat*>(&weapon);
                            } else if (spell->m_attributes & 0x2) {
                                weapon = player->m_player->m_playerFieldWeaponDmgMultipliers[2];
                                minValue = minValue * *reinterpret_cast<const CSoftFloat*>(&weapon);
                                weapon = player->m_player->m_playerFieldWeaponDmgMultipliers[2];
                                maxValue = maxValue * *reinterpret_cast<const CSoftFloat*>(&weapon);
                            }
                        }
                    }
                } else {
                    CSoftFloat bonus = CSoftFloat::FromInt(player->GetModHealingDonePos());
                    bonus = bonus * *reinterpret_cast<const CSoftFloat*>(&coefficient);
                    minValue = minValue + bonus;
                    maxValue = maxValue + bonus;
                    Spell_C_ModifyHealingAndDamage(spell, minValue, maxValue, periodic);

                    if (applyBonuses) {
                        float pct = player->m_player->m_playerFieldModHealingDonePct;
                        minValue = minValue * *reinterpret_cast<const CSoftFloat*>(&pct);
                        maxValue = maxValue * *reinterpret_cast<const CSoftFloat*>(&pct);
                    }
                }
            }
        }

        int32_t aura = effect->m_effectAura;

        if (aura == 10 || aura == 103 || aura == 183) {
            Spell_C_ModifySpellValue(spell, minValue, 2);
            Spell_C_ModifySpellValue(spell, maxValue, 2);
        }
    }

    minValue.m_bits += (minValue.m_bits & 0x7F800000) ? 0x3800000 : 0;
    minValue = CSoftFloat::Round(minValue);
    minValue.m_bits = (minValue.m_bits - 0x3800000) & ~static_cast<uint32_t>(static_cast<int32_t>((minValue.m_bits - 0x4000000) ^ minValue.m_bits) >> 31);
    maxValue.m_bits += (maxValue.m_bits & 0x7F800000) ? 0x3800000 : 0;
    maxValue = CSoftFloat::Round(maxValue);
    maxValue.m_bits = (maxValue.m_bits - 0x3800000) & ~static_cast<uint32_t>(static_cast<int32_t>((maxValue.m_bits - 0x4000000) ^ maxValue.m_bits) >> 31);

    if (rounded) {
        if (exact) {
            CSoftFloat value = CSoftFloat::Round(maxValue);
            *reinterpret_cast<CSoftFloat*>(maxPoints) = value;
            *reinterpret_cast<CSoftFloat*>(minPoints) = value;
        } else {
            *reinterpret_cast<CSoftFloat*>(minPoints) = CSoftFloat::Floor(minValue);
            *reinterpret_cast<CSoftFloat*>(maxPoints) = CSoftFloat::Ceil(maxValue);
        }
    } else {
        *reinterpret_cast<CSoftFloat*>(maxPoints) = maxValue;
        *reinterpret_cast<CSoftFloat*>(minPoints) = minValue;
    }
}

// OFFSET: 0x008AC690
bool Spell_C_IsCurrentSpell(int32_t spellId) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return false;
    }

    if (Spell_C_IsSpellPending(spellId)) {
        return true;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const SpellEffectRec* effect = SpellGetEffect(spellId, 0);

    if (effect) {
        if (effect->m_effect == 78) {
            return player && player->m_combat.m_attacking;
        }

        if (effect->m_effect == 47) {
            if (player) {
                SkillLineAbilityRec* ability = player->LookupAbility(spell->m_id);

                if (ability && ((!CGTradeSkillInfo::m_linkData && CGTradeSkillInfo::m_skillLineID == ability->m_skillLine) || ability->m_skillLine == CGResearchFrame::m_skillLine)) {
                    return true;
                }
            }

            return false;
        }
    }

    if (SpellIsCallCompanion(spell) || SpellHasAuraEffect(spell, 36)) {
        return Spell_C_IsActiveSpell(spell, player);
    }

    if (!SpellHasEffect(spell, 56)) {
        return false;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return false;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* summon = effects->m_data[i];

        if (!summon || summon->m_effect != 56) {
            continue;
        }

        if (summon->m_effectMiscValue) {
            if (pet->m_obj->m_entryID == summon->m_effectMiscValue) {
                return true;
            }

            continue;
        }

        StablePetInfo* info = CGStableInfo::GetPetInfo(effect->m_effectBasePoints);

        if (info && pet->m_unit->m_unitFieldPetnumber == info->m_petNumber) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008AC860/0x1405F0E20
bool Spell_C_TargetsAuraStateValid(const CGUnit_C* caster, const SpellRec* spell, uint32_t count, CGUnit_C* const* targets, int32_t* reason, bool skipPrimary) {
    if (!count) {
        return true;
    }

    const SpellAuraRestrictionsRec* restrictions;

    if (spell->m_spellAuraRestrictionsID && (restrictions = g_spellAuraRestrictionsDB.GetRecord(spell->m_spellAuraRestrictionsID))) {
        int32_t targetAuraSpell = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_targetAuraSpell);
        int32_t excludeTargetAuraSpell = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_excludeTargetAuraSpell);

        if (restrictions->m_targetAuraState || restrictions->m_excludeTargetAuraState || targetAuraSpell || excludeTargetAuraSpell) {
            uint32_t valid = 0;

            for (uint32_t i = count; i; ) {
                i--;
                CGUnit_C* target = targets[i];

                if (!target || !(target->m_obj->m_type & TYPEMASK_UNIT)) {
                    continue;
                }

                if (!(skipPrimary && !i)) {
                    int32_t state = restrictions->m_targetAuraState;

                    if (state && !((1 << (state - 1)) & target->m_unit->m_unitFieldAurastate) && !caster->HasIgnoreAuraState(spell)) {
                        continue;
                    }

                    state = restrictions->m_excludeTargetAuraState;

                    if (state && ((1 << (state - 1)) & target->m_unit->m_unitFieldAurastate) && !caster->HasIgnoreAuraState(spell)) {
                        continue;
                    }
                }

                if (targetAuraSpell && !target->HasAura(targetAuraSpell, WOWGUID())) {
                    continue;
                }

                if (excludeTargetAuraSpell && target->HasAura(excludeTargetAuraSpell, WOWGUID())) {
                    continue;
                }

                valid++;
            }

            if (!valid) {
                *reason = SPELL_FAILED_TARGET_AURASTATE;
                return false;
            }
        }
    }

    if (spell->m_attributesEx & 0x100) {
        uint32_t valid = 0;

        for (uint32_t i = count; i; ) {
            i--;
            CGUnit_C* target = targets[i];

            if (target && (target->m_obj->m_type & TYPEMASK_UNIT) && !(target->m_unit->m_unitFieldFlags & 0x80000)) {
                valid++;
            }
        }

        if (!valid) {
            *reason = SPELL_FAILED_TARGET_AFFECTING_COMBAT;
            return false;
        }
    }

    if (!(spell->m_attributesExI & 0x400)) {
        return true;
    }

    uint32_t valid = 0;

    for (uint32_t i = count; i; ) {
        i--;
        CGUnit_C* target = targets[i];

        if (target && (target->m_obj->m_type & TYPEMASK_UNIT) && !(target->m_movement->m_flags & 0x11100800)) {
            valid++;
        }
    }

    if (valid) {
        return true;
    }

    *reason = SPELL_FAILED_TARGET_NOT_GROUNDED;
    return false;
}

// OFFSET: 0x008ACAA0/0x1405F1110
bool Spell_C_AuraStateValid(const CGUnit_C* caster, const SpellRec* spell, const CGUnit_C* target) {
    const SpellAuraRestrictionsRec* restrictions;

    if (!spell->m_spellAuraRestrictionsID || !(restrictions = g_spellAuraRestrictionsDB.GetRecord(spell->m_spellAuraRestrictionsID))) {
        return true;
    }

    int32_t state = restrictions->m_casterAuraState;

    if (state && !((1 << (state - 1)) & caster->m_unit->m_unitFieldAurastate) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    state = restrictions->m_excludeCasterAuraState;

    if (state && ((1 << (state - 1)) & caster->m_unit->m_unitFieldAurastate) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    int32_t auraSpell = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_casterAuraSpell);

    if (auraSpell && !const_cast<CGUnit_C*>(caster)->HasAura(auraSpell, WOWGUID()) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    auraSpell = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_excludeCasterAuraSpell);

    if (auraSpell && const_cast<CGUnit_C*>(caster)->HasAura(auraSpell, WOWGUID()) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    int32_t targetAuraSpell = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_targetAuraSpell);
    int32_t excludeTargetAuraSpell = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_excludeTargetAuraSpell);
    int32_t targetState = restrictions->m_targetAuraState;

    if (!targetState && !restrictions->m_excludeTargetAuraState && !targetAuraSpell && !excludeTargetAuraSpell) {
        return true;
    }

    if (!target) {
        return !targetState;
    }

    if (targetState && !target->HasAuraState(targetState) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    state = restrictions->m_excludeTargetAuraState;

    if (state && target->HasAuraState(state) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    if (targetAuraSpell && !const_cast<CGUnit_C*>(target)->HasAura(targetAuraSpell, WOWGUID()) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    if (excludeTargetAuraSpell && const_cast<CGUnit_C*>(target)->HasAura(excludeTargetAuraSpell, WOWGUID()) && !caster->HasIgnoreAuraState(spell)) {
        return false;
    }

    return true;
}

// OFFSET: 0x008ACCA0
float Spell_C_GetEffectRadius(const SpellRec* spell, uint32_t level) {
    if (!spell) {
        return 0.0f;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    if (!count) {
        return 0.0f;
    }

    float radius = 0.0f;
    float best = 0.0f;

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        SpellRadiusRec* rec = g_spellRadiusDB.GetRecord(effect->m_effectRadiusIndex);

        if (rec) {
            float value = static_cast<float>(level) * rec->m_radiusPerLevel + rec->m_radius;

            if (best <= value) {
                radius = value;
                best = value;
            }
        }
    }

    float flat;
    float pct;

    if (Spell_C_GetSpellModifiers(spell, 6, flat, pct)) {
        return (flat + radius) * pct * 0.0099999998f;
    }

    return best;
}

// OFFSET: 0x008ACD80
void PendingSpellCast::OnSpellStartPacket(CDataStore* msg) {
    WOWGUID caster = this->m_data.m_caster;
    uint32_t castFlags = this->m_data.m_castFlags;
    uint32_t castTime;
    msg->Get(castTime);
    SpellGetCastTargets(&this->m_data, msg);
    uint32_t power = 0;
    uint32_t hasPower = castFlags & 0x800;

    if (hasPower) {
        msg->Get(power);
    }

    uint8_t runeCooldowns[6] = { 0, 0, 0, 0, 0, 0 };
    uint8_t runesBefore = 0;
    uint8_t runesAfter = 0;
    uint32_t hasRunes = castFlags & 0x200000;

    if (hasRunes) {
        msg->Get(runesBefore);
        msg->Get(runesAfter);

        for (uint32_t i = 0; i < 6; i++) {
            msg->Get(runeCooldowns[i]);
        }
    }

    uint32_t ammoDisplayId = 0;
    uint32_t ammoInventoryType = 0;

    if (castFlags & 0x20) {
        msg->Get(ammoDisplayId);
        msg->Get(ammoInventoryType);
    }

    uint32_t schoolImmunities = 0;
    uint32_t immunities = 0;
    uint32_t hasImmunities = castFlags & 0x4000000;

    if (hasImmunities) {
        msg->Get(schoolImmunities);
        msg->Get(immunities);
    }

    int32_t predictedHeal = 0;
    uint8_t predictionType = 0;
    WOWGUID predictionTarget = WOWGUID();
    uint32_t hasPrediction = castFlags & 0x40000000;

    if (hasPrediction) {
        msg->Get(predictedHeal);
        msg->Get(predictionType);

        if (predictionType == 2) {
            SmartGUID guid;
            *msg >> guid;
            predictionTarget = guid;
        }
    }

    SpellRec* spell = g_spellDB.GetRecord(this->m_data.m_spellId);

    if (!spell) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (hasPower && !(unit->m_unit->m_unitFieldFlags2 & 0x100)) {
        if (spell->m_powerType == -2) {
            unit->SetPredictedHealth(power);
        } else if (spell->m_powerType == 5) {
            if (caster == ClntObjMgrGetActivePlayer() && hasRunes) {
                CGRuneInfo::SetPredictedRunes(runesBefore, runesAfter, runeCooldowns, true);
            }
        } else {
            unit->SetPredictedPower(spell->m_powerType, power, false);
        }
    }

    if (hasImmunities) {
        unit->SetCastSchoolImmunities(schoolImmunities);
        unit->SetCastImmunities(immunities);
    }

    uint32_t hidden = castFlags & 0x1;

    if (!hidden) {
        StartSpellCastVisuals(unit, this, spell, castTime, false);
    }

    if (caster == CGUnit_C::s_activeMover && castTime) {
        SpellMissileRec* missile = g_spellMissileDB.GetRecord(spell->m_spellMissileID);

        if (missile && (missile->m_flags & 0x1)) {
            unit->StartUpdatingMissileTrajectory(this->m_data.m_spellId, FrameTime::GetCurTimeMs(), castTime);
        }
    }

    if (!hidden) {
        WOWGUID guid = unit->m_obj->m_guid;
        UnitCombatLogCastStart(guid, this->m_data.m_objectTarget, spell);
    }

    if (SpellHasEffect(spell, 18) && this->m_data.m_objectTarget) {
        IncomingResurrectionCastStart(this->m_data.m_objectTarget, caster, spell->m_id);
    }

    if (hasPrediction && (this->m_data.m_targetFlags & 0x2) && this->m_data.m_objectTarget) {
        CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_data.m_objectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (target) {
            target->AddHealPrediction(spell->m_id, 0, caster, predictedHeal, false, false);

            switch (predictionType) {
                case 1: {
                    unit->AddHealPrediction(spell->m_id, 0, caster, predictedHeal, false, false);
                    this->AddOtherTarget(caster);
                    break;
                }

                case 2: {
                    CGUnit_C* other;

                    if (predictionTarget && (other = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(predictionTarget, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
                        other->AddHealPrediction(spell->m_id, 0, caster, predictedHeal, false, false);
                        this->AddOtherTarget(predictionTarget);
                    }

                    break;
                }

                case 3: {
                    if (CGRaidInfo::m_numMembers) {
                        CGRaidMember* member = CGRaidInfo::FindMemberByGuid(target->m_obj->m_guid);

                        if (!member) {
                            break;
                        }

                        uint32_t subgroup = member->m_subgroup;

                        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
                            CGRaidMember* other = CGRaidInfo::GetMember(i);
                            CGUnit_C* otherUnit;

                            if (target->m_obj->m_guid != other->m_guid && subgroup == other->m_subgroup && (otherUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(other->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
                                otherUnit->AddHealPrediction(spell->m_id, 0, caster, predictedHeal, false, false);
                                this->AddOtherTarget(other->m_guid);
                            }
                        }
                    } else if (CGPartyInfo::InParty()) {
                        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
                            WOWGUID guid = CGPartyInfo::GetMember(i);
                            CGUnit_C* otherUnit;

                            if (guid != target->m_obj->m_guid && (otherUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
                                otherUnit->AddHealPrediction(spell->m_id, 0, caster, predictedHeal, false, false);
                                this->AddOtherTarget(guid);
                            }

                            if (target != ClntObjMgrGetActivePlayerObj()) {
                                CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

                                if (player) {
                                    player->AddHealPrediction(spell->m_id, 0, caster, predictedHeal, false, false);
                                    this->AddOtherTarget(player->m_obj->m_guid);
                                }
                            }
                        }
                    }

                    break;
                }
            }
        }
    }

    if (unit->IsActivePlayer()) {
        WOWGUID target;

        if ((spell->m_attributes & 0x400000) && (this->m_data.m_targetFlags & 0x2) && this->m_data.m_objectTarget) {
            target = this->m_data.m_objectTarget;
        } else {
            target = CGGameUI::m_lockedTarget;
        }

        if (spell->m_attributes & 0x400000) {
            unit->SetTrackingTarget(target, 0);
        }
    } else if ((this->m_data.m_targetFlags & 0x2) && this->m_data.m_objectTarget && this->m_data.m_objectTarget == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player && !unit->CanAssist(player, false)) {
            WOWGUID guid = unit->m_obj->m_guid;
            TargetAttacker(guid);
        }
    }
}

// OFFSET: 0x008AD3B0
int32_t Packet_Group_31(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t spellId;
    WOWGUID guid;
    msg->Get(spellId);
    msg->Get(guid.m_guid);
    int32_t pet = guid == CGPetInfo::GetPet(0);

    switch (msgId) {
        case SMSG_COOLDOWN_EVENT:
        case SMSG_CLEAR_COOLDOWN: {
            s_spellHistory[pet].ClearOnHold(spellId, time, msgId == SMSG_CLEAR_COOLDOWN);
            CGSpellBook::UpdateCooldowns();
            CGSpellBook::UpdateUsable();

            if (pet) {
                CGPetInfo::UpdateCooldowns();
            } else {
                CGContainerInfo::UpdateCooldowns();
            }

            break;
        }

        case SMSG_COOLDOWN_CHEAT: {
            Spell_C_ClearSpellHistory(pet);
            break;
        }

        case SMSG_MODIFY_COOLDOWN: {
            int32_t delay;
            msg->Get(reinterpret_cast<uint32_t&>(delay));

            if (spellId) {
                Spell_C_ModifyCooldown(pet, spellId, delay);
            }

            break;
        }
    }

    return 1;
}

// OFFSET: 0x008AD4B0
int32_t Packet_SMSG_COOLDOWN_CHEAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (guid == ClntObjMgrGetActivePlayer()) {
        s_spellHistory[0].ClearAll();
        CGSpellBook::UpdateCooldowns();
        CGContainerInfo::UpdateCooldowns();
        return 1;
    }

    if (guid == CGPetInfo::GetPet(0)) {
        s_spellHistory[1].ClearAll();
        CGSpellBook::UpdateCooldowns();
        CGPetInfo::UpdateCooldowns();
    }

    return 1;
}

// OFFSET: 0x008AD530
bool OnClearCooldowns(ClientClearCooldowns* msg) {
    int32_t pet = *reinterpret_cast<const WOWGUID*>(msg->m_guid) == CGPetInfo::GetPet(0);

    for (uint32_t i = 0; i < msg->m_spellID.Count(); i++) {
        s_spellHistory[pet].ClearOnHold(msg->m_spellID[i], 0, true);
        CGSpellBook::UpdateCooldowns();
        CGSpellBook::UpdateUsable();

        if (pet) {
            CGPetInfo::UpdateCooldowns();
        } else {
            CGContainerInfo::UpdateCooldowns();
        }
    }

    return true;
}

// OFFSET: 0x008AD5B0/0x1405F1D80
int32_t Spell_C_GetSpellCategoryWeeklyUses(const SpellRec* spell, uint8_t& used, uint8_t& maximum) {
    used = 0;
    maximum = 0;
    const SpellCategoriesRec* categories;

    if (!spell || !spell->m_spellCategoriesID || !(categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID)) || !categories->m_category) {
        return 0;
    }

    SpellCategoryRec* category = g_spellCategoryDB.GetRecord(categories->m_category);

    if (!category || !static_cast<uint8_t>(category->m_usesPerWeek)) {
        return 0;
    }

    maximum = static_cast<uint8_t>(category->m_usesPerWeek);

    for (WEEKLYSPELLUSE* entry = s_weeklySpellUses.Head(); entry; entry = s_weeklySpellUses.Next(entry)) {
        if (entry->m_usage.m_category == category->m_id) {
            used = entry->m_usage.m_uses;
            return category->m_id;
        }
    }

    return category->m_id;
}

// OFFSET: 0x008AD670
bool Spell_C_HasSpellModifiers(const SpellRec* spell, int32_t modifier) {
    float flat;
    float pct;
    return Spell_C_GetSpellModifiers(spell, modifier, flat, pct);
}

// OFFSET: 0x008AD6A0
void Spell_C_ClearPetCooldowns() {
    s_spellHistory[1].ClearAll();
}

// OFFSET: 0x008AD6B0
int32_t Spell_C_GetDuration(const SpellRec* spell, int32_t isPet, int32_t isUnit, int32_t noModifiers, int32_t applyHaste) {
    if (!spell) {
        return 0;
    }

    SpellDurationRec* rec = g_spellDurationDB.GetRecord(spell->m_durationIndex);

    if (!rec) {
        return 0;
    }

    int32_t level = Spell_C_GetSpellLevel(spell, isPet, isUnit);
    int32_t duration = Spell_C_GetSpellLevelBonus(spell, level) * rec->m_durationPerLevel + rec->m_duration;

    if (duration >= rec->m_maxDuration) {
        duration = rec->m_maxDuration;
    }

    if (!noModifiers) {
        Spell_C_ModifySpellValue(spell, duration, 1);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !applyHaste || (!(spell->m_attributesExH & 0x20000) && !(spell->m_attributesExE & 0x2000))) {
        return duration;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    uint32_t index;

    for (index = 0; index < count; index++) {
        if (effects->m_data[index] && effects->m_data[index]->m_effectAuraPeriod) {
            break;
        }
    }

    if (index >= count) {
        return duration;
    }

    if (((spell->m_attributesExE & 0x2000) || player->HasAuraMatchingSpellClass(316, spell)) && !(spell->m_attributesExC & 0x20000000)) {
        float haste = player->m_unit->m_unitModCastHaste;

        if (haste >= 0.001f) {
            if (spell->m_attributesExH & 0x20000) {
                return static_cast<int32_t>(static_cast<float>(duration) * haste);
            }

            int32_t period = Spell_C_GetAuraPeriod(spell, index);

            if (period > 0) {
                duration = period * static_cast<int32_t>(static_cast<float>(duration) / static_cast<float>(period));
            }
        }
    }

    return duration;
}

// OFFSET: 0x008AD870
int32_t Spell_C_IsSpellInTransit(WOWGUID caster, uint32_t spellId, uint32_t castTime) {
    for (SpellInTransit* spell = s_spellsInTransit.Head(); spell; spell = s_spellsInTransit.Next(spell)) {
        if (spell->m_caster == caster && spell->m_spellId == static_cast<int32_t>(spellId) && spell->m_serverTime == castTime) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008AD930
PendingSpellCast* CreatePendingSpellCast(bool assignCastCount) {
    PendingSpellCast* cast = &s_pendingSpellCasts[s_numPendingSpellCasts++];
    cast->m_hidden = 0;
    cast->m_trajectoryLaunched = 0;
    cast->m_petGuid = WOWGUID();
    cast->m_baseSpellId = 0;
    cast->m_cooldownDelay = 0;
    cast->m_otherTargets.m_count = 0;
    memset(&cast->m_data, 0, sizeof(SpellCast));

    if (assignCastCount) {
        uint8_t castCount = s_nextCastCount;

        if (!castCount) {
            castCount = 1;
        }

        cast->m_data.m_castCount = castCount;
        s_nextCastCount = castCount + 1;
    }

    return cast;
}

// OFFSET: 0x008AD9B0
PendingSpellCast* FindPendingSpellCast(const WOWGUID& castItem, PendingSpellCast* fallback, uint8_t castCount, int32_t spellId, const WOWGUID& caster) {
    PendingSpellCast* cast = GetPendingSpellCast(castCount, spellId, caster);

    if (cast) {
        return cast;
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (caster && (caster == ClntObjMgrGetActivePlayer() || (spell && (SpellHasEffect(spell, 18) || (spell->m_attributesExH & 0x1000000)))) && s_numPendingSpellCasts < 100) {
        cast = CreatePendingSpellCast(false);
    } else {
        cast = fallback;
    }

    cast->m_data.m_castCount = castCount;
    cast->m_data.m_spellId = spellId;
    cast->m_data.m_castItem = castItem;
    cast->m_data.m_caster = caster;
    return cast;
}

// OFFSET: 0x008ADA60
void SpellHistory::RemoveStartCooldown(int32_t spellId, uint8_t castCount) {
    bool removed = false;
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_spellId == spellId && (!castCount || !history->m_castCount || history->m_castCount == castCount)) {
            history->m_startRecoveryCategory = 0;
            history->m_startRecoveryTime = 0;
            removed = true;
        }
    }

    if (removed) {
        this->GarbageCollect(OsGetAsyncTimeMs());
        CGSpellBook::UpdateCooldowns();
    }
}

// OFFSET: 0x008ADAE0/0x1405F2120
void Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, int32_t* minPoints, int32_t* maxPoints, int32_t spellLevel, int32_t isPet, int32_t isVehicle, int32_t noModifiers) {
    float low;
    float high;
    Spell_C_GetMinMaxPoints(spell, effectIndex, &low, &high, spellLevel, isPet, isVehicle, noModifiers, 0);
    *minPoints = static_cast<int32_t>(low);
    *maxPoints = static_cast<int32_t>(high);
}

// OFFSET: 0x008ADB30
PendingSpellCast::PendingSpellCast() {
    this->m_hidden = 0;
    this->m_trajectoryLaunched = 0;
    this->m_petGuid = WOWGUID();
    this->m_baseSpellId = 0;
    this->m_cooldownDelay = 0;
    this->m_otherTargets.m_count = 0;
    memset(&this->m_data, 0, sizeof(SpellCast));
}

// OFFSET: 0x008ADBA0
float Spell_C_GetSpellRadius() {
    PendingSpellCast* cast = s_targetingSpell;

    if (!cast) {
        return 0.0f;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster) {
        return 0.0f;
    }

    return Spell_C_GetEffectRadius(g_spellDB.GetRecord(cast->m_data.m_spellId), caster->m_unit->m_unitFieldLevel);
}

// OFFSET: 0x008ADC10
void Spell_C_PetSpellFailedUpdateCooldowns(int32_t spellId) {
    s_spellHistory[1].RemoveStartCooldown(spellId, 0);
    CGPetInfo::UpdateCooldowns();
}

// OFFSET: 0x008ADC30
void Spell_C_AddToInTransitList(WOWGUID caster, const SpellRec* spell, uint32_t serverTime, uint32_t arrivalTime) {
    if (!spell || spell->m_speed <= 0.0f) {
        return;
    }

    void* memory = SMemAlloc(sizeof(SpellInTransit), __FILE__, __LINE__, 0);
    SpellInTransit* node = memory ? new (memory) SpellInTransit() : NULL;
    node->m_caster = caster;
    node->m_spellId = spell->m_id;
    node->m_serverTime = serverTime;
    node->m_arrivalTime = arrivalTime;
    SpellInTransit* before = NULL;

    for (SpellInTransit* entry = s_spellsInTransit.Head(); entry; entry = s_spellsInTransit.Next(entry)) {
        if (static_cast<int32_t>(entry->m_arrivalTime - arrivalTime) >= 0) {
            before = entry;
            break;
        }
    }

    s_spellsInTransit.LinkNode(node, 2, before);
}

// OFFSET: 0x008ADD10
void Spell_C_ClearInTransitList() {
    SpellInTransit* spell = s_spellsInTransit.Head();

    while (spell) {
        SpellInTransit* next = s_spellsInTransit.Next(spell);
        s_spellsInTransit.UnlinkNode(spell);
        delete spell;
        spell = next;
    }
}

// OFFSET: 0x008AE1A0
bool OnWeeklySpellUsageUpdate(ClientWeeklySpellUsageUpdate* msg) {
    for (WEEKLYSPELLUSE* entry = s_weeklySpellUses.Head(); entry; entry = s_weeklySpellUses.Next(entry)) {
        if (entry->m_usage.m_category == msg->m_usage.m_category) {
            entry->m_usage.m_uses = msg->m_usage.m_uses;
            return true;
        }
    }

    WEEKLYSPELLUSE* entry = s_weeklySpellUses.NewNode(2, 0, 8);
    entry->m_usage = msg->m_usage;
    return true;
}

// OFFSET: 0x008AE280
void Spell_C_ClearCooldowns() {
    s_itemCooldowns.InternalClear(0);

    for (uint32_t i = 0; i < sizeof(s_spellHistory) / sizeof(s_spellHistory[0]); i++) {
        s_spellHistory[i].ClearAll();
    }
}

// OFFSET: 0x008AE2B0
void Spell_C_UpdateInTransitList() {
    uint32_t now = OsGetAsyncTimeMs();
    SpellInTransit* spell = s_spellsInTransit.Head();

    while (spell) {
        SpellInTransit* next = s_spellsInTransit.Next(spell);

        if (static_cast<int32_t>(now - spell->m_arrivalTime) < 0) {
            break;
        }

        s_spellsInTransit.UnlinkNode(spell);
        delete spell;
        spell = next;
    }
}

// OFFSET: 0x008AE360
void Spell_C_CleanUpProcessedDestLocSpellCasts(uint32_t time) {
    for (DESTLOCSPELLCASTINDEXHASHNODE* node = s_destLocSpellCasts.m_fulllist.Head(); node; ) {
        DESTLOCSPELLCASTINDEXHASHNODE* next = s_destLocSpellCasts.m_fulllist.Next(node);

        if (static_cast<int32_t>(time - node->m_time) >= 0) {
            s_destLocSpellCasts.Delete(node);
        }

        node = next;
    }
}

// OFFSET: 0x008AE540
PendingSpellCast& PendingSpellCast::operator=(const PendingSpellCast& other) {
    this->m_data = other.m_data;
    this->m_hidden = other.m_hidden;
    this->m_trajectoryLaunched = other.m_trajectoryLaunched;
    this->m_petGuid = other.m_petGuid;
    this->m_baseSpellId = other.m_baseSpellId;
    this->m_cooldownDelay = other.m_cooldownDelay;

    if (&this->m_otherTargets != &other.m_otherTargets) {
        this->m_otherTargets.Set(other.m_otherTargets.m_count, other.m_otherTargets.m_data);
    }

    this->m_otherTargets.m_chunk = other.m_otherTargets.m_chunk;
    return *this;
}

// OFFSET: 0x008AE710
void SpellHistory::AddHistory(int32_t spellId, int32_t itemId, uint32_t time, uint32_t recoveryTime, int32_t category, uint32_t categoryTime, uint32_t categoryRecoveryTime, bool onHold, int32_t startRecoveryCategory, uint32_t startRecoveryTime, uint8_t castCount) {
    if (!recoveryTime && !categoryRecoveryTime && !onHold && !startRecoveryTime) {
        return;
    }

    SPELLHISTORY* history = this->m_free.Head();

    if (history) {
        history->m_link.Unlink();
        this->m_history.LinkToTail(history);
    } else {
        history = this->m_history.NewNode(2, 0, 8);
    }

    history->m_spellId = spellId;
    history->m_itemId = itemId;
    history->m_castCount = castCount;
    history->m_time = time;
    history->m_recoveryTime = recoveryTime;
    history->m_category = category;
    history->m_categoryTime = categoryTime;
    history->m_categoryRecoveryTime = categoryRecoveryTime;
    history->m_onHold = onHold;
    history->m_startRecoveryCategory = startRecoveryCategory;
    history->m_startRecoveryTime = startRecoveryTime;
}

// OFFSET: 0x008AE820
int32_t Packet_SMSG_ITEM_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));

    if (g_spellDB.GetRecord(spellId)) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            int32_t itemId = item->m_obj->m_entryID;
            SpellHistory& history = s_spellHistory[0];
            SPELLHISTORY* entry = history.m_free.Head();

            if (entry) {
                entry->m_link.Unlink();
                history.m_history.LinkToTail(entry);
            } else {
                entry = history.m_history.NewNode(2, 0, 8);
            }

            entry->m_spellId = spellId;
            entry->m_itemId = itemId;
            entry->m_castCount = 0;
            entry->m_time = time;
            entry->m_recoveryTime = 30000;
            entry->m_category = 0;
            entry->m_categoryTime = 0;
            entry->m_categoryRecoveryTime = 0;
            entry->m_onHold = false;
            entry->m_startRecoveryCategory = 0;
            entry->m_startRecoveryTime = 0;
        }
    }

    CGSpellBook::UpdateCooldowns();
    CGContainerInfo::UpdateCooldowns();
    return 1;
}

// OFFSET: 0x008AE980
int32_t Packet_SMSG_RESET_RANGED_COMBAT_TIMER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t cooldown;
    msg->Get(cooldown);

    for (uint32_t i = 0; i < s_spellBookItems.m_count; i++) {
        SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetSpell(i, 0));

        if (spell && (spell->m_attributes & 0x2) && !(spell->m_attributesExB & 0x20000) && !(spell->m_attributesExD & 0x10000000)) {
            s_spellHistory[0].AddHistory(spell->m_id, 0, OsGetAsyncTimeMs(), cooldown, 0, 0, 0, false, 0, 0, 0);
            CGSpellBook::UpdateCooldowns();
        }
    }

    return 1;
}

// OFFSET: 0x008AEB00
bool OnWeeklySpellUsage(ClientWeeklySpellUsage* msg) {
    s_weeklySpellUses.Clear();

    for (uint32_t i = 0; i < msg->m_usage.Count(); i++) {
        WEEKLYSPELLUSE* entry = s_weeklySpellUses.NewNode(2, 0, 8);
        entry->m_usage = msg->m_usage[i];
    }

    return true;
}

// OFFSET: 0x008AEBD0/0x1405F3CA0
void Spell_C_CancelTriggeredAuras(WOWGUID caster, const SpellRec* spell) {
    int32_t spellId = spell->m_id;
    PeriodicClientTrigger* next;

    for (PeriodicClientTrigger* trigger = s_periodicClientTriggers.Head(); trigger; trigger = next) {
        next = s_periodicClientTriggers.Next(trigger);

        if (trigger->m_caster == caster && (trigger->m_spellId == spellId || !spellId) && !trigger->m_period && spell->m_attributes >= 0) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_CANCEL_AURA));
            msg.Put(static_cast<uint32_t>(trigger->m_auraSpellId));
            msg.Finalize();
            ClientServices::Send2(&msg);
            s_periodicClientTriggers.DeleteNode(trigger);
        }
    }
}

// OFFSET: 0x008AEF50/0x1405F4200
SpellHistory::~SpellHistory() {
    this->m_history.Clear();
    this->m_free.Clear();
}

// OFFSET: 0x008AEFF0
void ReleasePendingSpellCast(PendingSpellCast* cast) {
    uint32_t index = cast - s_pendingSpellCasts;

    if (index >= s_numPendingSpellCasts) {
        return;
    }

    if (cast == s_castingSpellCast) {
        s_castingSpellCast = NULL;
    } else if (cast == s_targetingSpell) {
        s_targetingSpell = NULL;
        CursorSetResetMode(1);
        CursorSetMode(1);
        s_cursorWorldObject->Release();
        CGInputControl* input = CGInputControl::GetActive();
        input->m_mouseModeFlags &= ~0x10;
        input->UpdateMouseMode(0);
    } else {
        PendingSpellCast* last = &s_pendingSpellCasts[s_numPendingSpellCasts - 1];

        if (s_targetingSpell == last) {
            s_targetingSpell = &s_pendingSpellCasts[index];
        } else if (s_castingSpellCast == last) {
            s_castingSpellCast = &s_pendingSpellCasts[index];
        }
    }

    if (index != s_numPendingSpellCasts - 1) {
        s_pendingSpellCasts[index] = s_pendingSpellCasts[s_numPendingSpellCasts - 1];
    }

    s_numPendingSpellCasts--;
}

// OFFSET: 0x008AF100/0x1405F42D0
bool SpellHistory::GetCooldown(int32_t spellId, int32_t itemId, uint32_t* duration, uint32_t* start, uint32_t* enable, int32_t* remaining) {
    if (remaining) {
        *remaining = 0;
    }

    int32_t category = 0;
    int32_t startRecoveryCategory = 0;

    if (enable) {
        *enable = 1;
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return false;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect && (effect->m_effect == 78 || effect->m_effect == 47)) {
        return false;
    }

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories) {
            category = categories->m_category;
            startRecoveryCategory = categories->m_startRecoveryCategory;
        }
    }

    if (itemId) {
        ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

        if (item) {
            for (uint32_t i = 0; i < 5; i++) {
                if (item->m_spellID[i] == spellId && item->m_spellCategory[i] > 0) {
                    category = item->m_spellCategory[i];
                }
            }
        }
    }

    uint32_t now = OsGetAsyncTimeMs();
    uint32_t end = now;
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_spellId == spellId && history->m_itemId == itemId) {
            uint32_t recovery = history->m_recoveryTime;

            if (recovery || history->m_onHold) {
                uint32_t finish = recovery + (history->m_onHold ? now + 1 : history->m_time);

                if (static_cast<int32_t>(finish - end) >= 0) {
                    if (duration) {
                        *duration = history->m_onHold ? 1 : recovery;
                    }

                    if (start) {
                        *start = history->m_onHold ? now : history->m_time;
                    }

                    if (enable && *enable) {
                        *enable = !history->m_onHold;
                    }

                    end = finish;
                }
            }
        }

        SpellCategoryRec* categoryRec = g_spellCategoryDB.GetRecord(history->m_category);

        if ((history->m_category && history->m_category == category) || (categoryRec && (categoryRec->m_flags & 0x2))) {
            uint32_t recovery = history->m_categoryRecoveryTime;

            if (recovery) {
                uint32_t finish = recovery + (history->m_onHold ? now : history->m_categoryTime);

                if (static_cast<int32_t>(finish - end) >= 0) {
                    if (duration) {
                        *duration = recovery;
                    }

                    if (start) {
                        *start = history->m_onHold ? now : history->m_categoryTime;
                    }

                    if (enable && *enable) {
                        *enable = !history->m_onHold;
                    }

                    end = finish;
                }
            }
        }

        if (history->m_startRecoveryCategory == startRecoveryCategory) {
            uint32_t recovery = history->m_startRecoveryTime;

            if (recovery) {
                uint32_t time = history->m_time;
                uint32_t finish = time + recovery;

                if (time != now && static_cast<int32_t>(time - now) >= 0) {
                    continue;
                }

                if (static_cast<int32_t>(finish - end) >= 0) {
                    if (duration) {
                        *duration = recovery;
                    }

                    if (start) {
                        *start = history->m_time;
                    }

                    end = finish;

                    if (remaining) {
                        *remaining = finish - now;
                    }
                }
            }
        }

        int32_t runeCost = spell->m_runeCostID;

        if (!runeCost) {
            continue;
        }

        if (history->m_spellId != spellId) {
            SpellRec* other = g_spellDB.GetRecord(history->m_spellId);

            if (!other || !other->m_runeCostID) {
                continue;
            }

            SpellRuneCostRec* cost = g_spellRuneCostDB.GetRecord(runeCost);
            SpellRuneCostRec* otherCost = g_spellRuneCostDB.GetRecord(other->m_runeCostID);

            if (!cost || !otherCost || ((!cost->m_blood || !otherCost->m_blood) && (!cost->m_unholy || !otherCost->m_unholy) && (!cost->m_frost || !otherCost->m_frost))) {
                continue;
            }
        }

        uint32_t runeEnd;

        if (!CGRuneInfo::ComputeRuneSpellCooldown(spell, &runeEnd) || static_cast<int32_t>(runeEnd - end) < 0) {
            continue;
        }

        end = runeEnd;

        if (start) {
            *start = history->m_time;
        }

        if (duration && start) {
            if (CGRuneInfo::HasRunesForSpell(spellId, NULL)) {
                *duration = history->m_startRecoveryTime;
            } else {
                *start = end - 10000;
                *duration = 10000;
            }
        }
    }

    return end != now;
}

// OFFSET: 0x008AF540/0x1405F4750
int32_t SpellHistory::IsOnHold(int32_t spellId, int32_t itemId) {
    int32_t category = 0;
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories) {
            category = categories->m_category;
        }
    }

    if (itemId) {
        ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

        if (item) {
            for (uint32_t i = 0; i < 5; i++) {
                if (item->m_spellID[i] == spellId && item->m_spellCategory[i] > 0) {
                    category = item->m_spellCategory[i];
                }
            }
        }
    }

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = this->m_history.Next(history)) {
        if (history->m_spellId == spellId && history->m_itemId == itemId && (history->m_recoveryTime || history->m_onHold) && history->m_onHold) {
            return 1;
        }

        if (history->m_category == category && history->m_categoryRecoveryTime && history->m_onHold) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008AF6C0/0x1405F48B0
void SpellMissingItemCallback(int32_t itemId, const WOWGUID& guid, void* param, bool success) {
    int32_t reason = static_cast<int32_t>(reinterpret_cast<intptr_t>(param));
    UIErrors error = ERR_SPELL_FAILED_S;
    char format[1024];
    FrameScript_GetText(GetSpellFailedResult(reason), format, sizeof(format));

    if (reason == SPELL_FAILED_REAGENTS) {
        error = ERR_SPELL_FAILED_REAGENTS;
    } else if (reason == SPELL_FAILED_TOTEMS) {
        error = ERR_SPELL_FAILED_TOTEMS;
    }

    char text[1024];
    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (item) {
        SStrPrintf(text, sizeof(text), format, item->GetName(0));
    } else {
        SStrPrintf(text, sizeof(text), format, "UNKNOWN");
    }

    CGGameUI::DisplayError(error, text);
}

// OFFSET: 0x008AF790/0x1405F49C0
void SpellMissingItemsCallback(int32_t itemId, const WOWGUID& guid, void* param, bool success) {
    char format[1024];
    FrameScript_GetText("SPELL_FAILED_NEED_MORE_ITEMS", format, sizeof(format));
    char text[1024];
    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (item) {
        SStrPrintf(text, sizeof(text), format, static_cast<int32_t>(reinterpret_cast<intptr_t>(param)), item->GetName(0));
    } else {
        SStrPrintf(text, sizeof(text), format, 0, "UNKNOWN");
    }

    CGGameUI::DisplayError(ERR_SPELL_FAILED_S, text);
}

// OFFSET: 0x008AF840/0x1405F3450
void FinishPendingSpellCast(PendingSpellCast* cast, CGUnit_C* unit, const SpellRec* spell, int32_t reason) {
    bool playerCast = false;

    if (cast->m_data.m_caster == ClntObjMgrGetActivePlayer() || cast->IsCharmedUnitCast()) {
        playerCast = true;
        const SpellClassOptionsRec* options;

        if (spell->m_spellClassOptionsID && (options = g_spellClassOptionsDB.GetRecord(spell->m_spellClassOptionsID)) && options->m_modalNextSpell && Spell_C_TargetMatchesAutoAttack(spell, cast, options->m_modalNextSpell, reason)) {
            int32_t nextSpell = options->m_modalNextSpell;

            if (nextSpell == GetModalNextSpell()) {
                CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

                if (player && !player->IsInMelee() && !player->IsCharmed() && !reason) {
                    player->OnAttackIconPressed(WOWGUID(), 0, 0, 0);
                }
            } else {
                Spell_C_CastSpell(unit, nextSpell, NULL, cast->m_data.m_objectTarget, NULL, false, false, 0, 0);
            }
        }
    }

    if (SpellHasEffect(spell, 18)) {
        IncomingResurrectionCastEnd(cast->m_data.m_objectTarget, cast->m_data.m_caster, spell->m_id);
    }

    if (spell->m_attributesExH & 0x1000000) {
        CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_objectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (target) {
            target->RemoveHealPrediction(spell->m_id, cast->m_data.m_caster, false, false);

            for (uint32_t i = 0; i < cast->m_otherTargets.m_count; i++) {
                CGUnit_C* other = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_otherTargets.m_data[i], TYPEMASK_UNIT, __FILE__, __LINE__));

                if (other) {
                    other->RemoveHealPrediction(spell->m_id, cast->m_data.m_caster, false, false);
                }
            }
        }
    }

    ReleasePendingSpellCast(cast);

    if (playerCast && cast->m_data.m_castCount) {
        CGSpellBook::UpdateSelection(reason == SPELL_FAILED_DONT_REPORT);
    }
}

// OFFSET: 0x008AFA20/0x1405F4AD0
void Spell_C_FailPendingCast(PendingSpellCast* cast, const SpellRec* spell, int32_t reason) {
    if (!cast) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        Spell_C_CancelTriggeredAuras(cast->m_data.m_castItem, spell);
        HandleSpellFailedOrGo(unit, cast, spell, reason);
        FinishPendingSpellCast(cast, unit, spell, reason);
    }
}

// OFFSET: 0x008AFA80/0x1405F4B60
UIErrors Spell_C_GetSpellFailedError(const SpellRec* spell, int32_t reason, int32_t misc) {
    int32_t category = 0;

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories) {
            category = categories->m_category;
        }
    }

    UIErrors error = ERR_SPELL_FAILED_S;

    switch (reason) {
        case SPELL_FAILED_ALREADY_AT_FULL_HEALTH:
            return ERR_SPELL_FAILED_ALREADY_AT_FULL_HEALTH;

        case SPELL_FAILED_ALREADY_AT_FULL_MANA:
            return ERR_SPELL_FAILED_ALREADY_AT_FULL_MANA;

        case SPELL_FAILED_ALREADY_AT_FULL_POWER:
            return ERR_SPELL_FAILED_ALREADY_AT_FULL_POWER_S;

        case SPELL_FAILED_BAD_IMPLICIT_TARGETS:
            return ERR_GENERIC_NO_TARGET;

        case SPELL_FAILED_BAD_TARGETS: {
            SpellTargetRestrictionsRec* restrictions;

            if (!spell->m_spellTargetRestrictionsID || !(restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID))) {
                return error;
            }

            if (restrictions->m_targets & 0x10) {
                return ERR_INVALID_ITEM_TARGET;
            }

            if (!(restrictions->m_targets & 0x20000)) {
                return error;
            }

            return ERR_INVALID_GLYPH_SLOT;
        }

        case SPELL_FAILED_EQUIPPED_ITEM:
            return ERR_SPELL_FAILED_EQUIPPED_ITEM;

        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
            return ERR_SPELL_FAILED_EQUIPPED_ITEM_CLASS_S;

        case SPELL_FAILED_ITEM_NOT_READY:
            switch (category) {
                case 4:
                case 9:
                    return ERR_POTION_COOLDOWN;

                case 10:
                case 11:
                    return ERR_FOOD_COOLDOWN;

                default:
                    return ERR_ITEM_COOLDOWN;
            }

        case SPELL_FAILED_NEED_AMMO:
            return ERR_NOAMMO_S;

        case SPELL_FAILED_NOT_READY: {
            switch (category) {
                case 4:
                case 9:
                    error = ERR_POTION_COOLDOWN;
                    break;

                case 10:
                case 11:
                    error = ERR_FOOD_COOLDOWN;
                    break;

                default:
                    error = static_cast<UIErrors>(((spell->m_attributes & 0x10) | 0x340) >> 4);
                    break;
            }

            if (misc || !s_spellHistory[0].IsOnHold(spell->m_id, 0)) {
                return error;
            }

            s_spellHistory[0].ClearOnHold(spell->m_id, 0, true);
            CGSpellBook::UpdateCooldowns();
            CGSpellBook::UpdateUsable();
            CGContainerInfo::UpdateCooldowns();
            return error;
        }

        case SPELL_FAILED_NOT_UNSHEATHED:
            return ERR_SPELL_FAILED_NOTUNSHEATHED;

        case SPELL_FAILED_ONLY_SHAPESHIFT:
            return ERR_SPELL_FAILED_SHAPESHIFT_FORM_S;

        case SPELL_FAILED_OUT_OF_RANGE:
            return ERR_SPELL_OUT_OF_RANGE;

        case SPELL_FAILED_REAGENTS:
            return ERR_SPELL_FAILED_REAGENTS;

        case SPELL_FAILED_TOO_MANY_OF_ITEM:
            if (misc > 0) {
                error = ERR_ITEM_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS;
            }

            return error;

        case SPELL_FAILED_TOTEMS:
            return ERR_SPELL_FAILED_TOTEMS;

        case SPELL_FAILED_PLAY_TIME:
            return ERR_PLAY_TIME_EXCEEDED;

        case SPELL_FAILED_ON_USE_ENCHANT:
            return ERR_BAD_ON_USE_ENCHANT;

        case SPELL_FAILED_TOO_MANY_SOCKETS:
            return ERR_TOO_MANY_SOCKETS;

        case SPELL_FAILED_NO_VALID_TARGETS:
            return ERR_GENERIC_NO_VALID_TARGETS;

        case SPELL_FAILED_MAX_SOCKETS:
            return ERR_MAX_SOCKETS;

        default:
            return error;
    }
}

// OFFSET: 0x008AFDE0
void Spell_C_PetSpellFailed(PendingSpellCast* cast, const SpellRec* spell, int32_t reason, int32_t misc1, int32_t misc2) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    bool vehicleCast = player && cast && cast->m_data.m_caster && player->GetTransportGUID() == cast->m_data.m_caster.m_guid;
    char format[1024];
    char text[1024];

    switch (reason) {
        case SPELL_FAILED_AFFECTING_COMBAT:
            CGGameUI::DisplayError(ERR_PET_SPELL_AFFECTING_COMBAT);
            break;

        case SPELL_FAILED_CASTER_DEAD:
            CGGameUI::DisplayError(ERR_PET_SPELL_DEAD);
            break;

        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND: {
            FrameScript_GetText(GetSpellFailedResult(reason), format, sizeof(format));
            bool found = false;

            for (uint32_t i = 0; i < static_cast<uint32_t>(g_itemSubClassMaskDB.m_numRecords); i++) {
                ItemSubClassMaskRec* rec = g_itemSubClassMaskDB.GetRecordByIndex(i);

                if (rec->m_classID == misc1 && rec->m_mask == misc2) {
                    SStrPrintf(text, sizeof(text), format, static_cast<const char*>(rec->m_name));
                    CGGameUI::DisplayError(ERR_SPELL_FAILED_S, text);
                    found = true;
                    break;
                }
            }

            if (!found) {
                ItemSubClassRec* rec = FindAnyItemSubclassRec(misc1, misc2);

                if (rec) {
                    SStrPrintf(text, sizeof(text), format, static_cast<const char*>(rec->m_displayName));
                    CGGameUI::DisplayError(ERR_SPELL_FAILED_S, text);
                }
            }

            break;
        }

        case SPELL_FAILED_NOPATH:
            CGGameUI::DisplayError(ERR_PET_SPELL_NOPATH);
            break;

        case SPELL_FAILED_NOT_BEHIND:
            CGGameUI::DisplayError(ERR_PET_SPELL_NOT_BEHIND);
            break;

        case SPELL_FAILED_NOT_READY:
            CGGameUI::DisplayError(static_cast<UIErrors>(((spell->m_attributes & 0x10) | 0x340) >> 4));
            break;

        case SPELL_FAILED_NO_POWER: {
            int32_t powerBar = 0;
            const SpellPowerRec* power;
            PowerDisplayRec* display;

            if (spell->m_spellPowerID && (power = g_spellPowerDB.GetRecord(spell->m_spellPowerID)) && !(powerBar = power->m_unitPowerBarID) && (display = g_powerDisplayDB.GetRecord(power->m_powerDisplayID))) {
                char name[128];
                FrameScript_GetText(display->m_globalStringBaseTag, name, 127);
                CGGameUI::DisplayError(ERR_OUT_OF_POWER_DISPLAY, name);
            } else if (spell->m_powerType == -2) {
                CGGameUI::DisplayError(ERR_OUT_OF_HEALTH);
            } else if (misc1 == 8) {
                if (static_cast<uint32_t>(misc2) <= 1) {
                    CGGameUI::DisplayError(s_eclipseErrors[misc2]);
                }
            } else if (misc1 == 10) {
                CGPlayer_C* active = ClntObjMgrGetActivePlayerObj();

                if (active) {
                    const WOWGUID* controlled = &active->m_unit->m_unitFieldCharm;

                    if (!*controlled) {
                        controlled = &active->m_unit->m_unitFieldSummon;
                    }

                    if (*controlled) {
                        const WOWGUID* pet = &active->m_unit->m_unitFieldCharm;

                        if (!*pet) {
                            pet = &active->m_unit->m_unitFieldSummon;
                        }

                        if (ClntObjMgrObjectPtr(*pet, TYPEMASK_UNIT, __FILE__, __LINE__) && powerBar) {
                            UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(powerBar);

                            if (bar) {
                                CGGameUI::AddErrorMessage(bar->m_outOfError, 1);
                            }
                        }
                    }
                }
            } else {
                CGGameUI::DisplayError(s_powerErrors[spell->m_powerType]);
            }

            break;
        }

        case SPELL_FAILED_ONLY_SHAPESHIFT: {
            FrameScript_GetText(GetSpellFailedResult(reason), text, sizeof(text));
            uint32_t count = g_spellShapeshiftFormDB.m_numRecords;

            if (count > 64) {
                count = 64;
            }

            for (uint32_t i = 0; i < count; i++) {
                if (!Spell_C_SpellHasShapeshiftMask(spell, i)) {
                    continue;
                }

                SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecordByIndex(i);

                if (form && form->m_name && *form->m_name) {
                    SStrPrintf(format, sizeof(format), text, static_cast<const char*>(form->m_name));
                    CGGameUI::DisplayError(ERR_SPELL_FAILED_SHAPESHIFT_FORM_S, format);
                    break;
                }
            }

            break;
        }

        case SPELL_FAILED_OUT_OF_RANGE:
            if (vehicleCast) {
                CGGameUI::DisplayError(ERR_SPELL_OUT_OF_RANGE);
            } else {
                CGGameUI::DisplayError(ERR_PET_SPELL_OUT_OF_RANGE);
            }

            break;

        case SPELL_FAILED_REQUIRES_AREA: {
            FrameScript_GetText(GetSpellFailedResult(reason), text, sizeof(text));
            AreaTableRec* area = g_areaTableDB.GetRecord(misc1);

            if (area) {
                SStrPrintf(format, sizeof(format), text, static_cast<const char*>(area->m_areaName));
                CGGameUI::DisplayError(ERR_SPELL_FAILED_S, format);
            }

            break;
        }

        case SPELL_FAILED_REQUIRES_SPELL_FOCUS: {
            FrameScript_GetText(GetSpellFailedResult(reason), text, sizeof(text));
            SpellFocusObjectRec* focus = g_spellFocusObjectDB.GetRecord(misc1);

            if (focus) {
                SStrPrintf(format, sizeof(format), text, static_cast<const char*>(focus->m_name));
                CGGameUI::DisplayError(ERR_SPELL_FAILED_S, format);
            }

            break;
        }

        case SPELL_FAILED_PREVENTED_BY_MECHANIC: {
            FrameScript_GetText(GetSpellFailedResult(reason), text, sizeof(text));
            SpellMechanicRec* mechanic = g_spellMechanicDB.GetRecord(misc1);

            if (mechanic) {
                SStrPrintf(format, sizeof(format), text, static_cast<const char*>(mechanic->m_stateName));
                CGGameUI::DisplayError(ERR_SPELL_FAILED_S, format);
            }

            break;
        }

        case SPELL_FAILED_ROOTED:
            CGGameUI::DisplayError(ERR_PET_SPELL_ROOTED);
            break;

        case SPELL_FAILED_TARGETS_DEAD:
            CGGameUI::DisplayError(ERR_PET_SPELL_TARGETS_DEAD);
            break;

        case SPELL_FAILED_CUSTOM_ERROR:
            GetCustomSpellFailedString(text, sizeof(text), misc1);
            break;

        default:
            CGGameUI::DisplayError(ERR_SPELL_FAILED_S, FrameScript_GetText(GetSpellFailedResult(reason), -1, GENDER_NOT_APPLICABLE));
            break;
    }

    Spell_C_FailPendingCast(cast, spell, reason);
}

// OFFSET: 0x008B03E0
void Spell_C_ModifyCategoryRecovery(int32_t& recovery, const SpellRec* spell, int32_t pet, int32_t daily) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    int32_t category = 0;
    bool reset = false;
    CGUnit_C* unit = player;

    if (!pet) {
        SpellCategoriesRec* categories;

        if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
            category = categories->m_category;
            SpellCategoryRec* rec = g_spellCategoryDB.GetRecord(category);

            if (rec) {
                if (rec->m_flags & 0x8) {
                    reset = true;
                }

                if (rec->m_flags & 0x1) {
                    CGItem_C* item = player->GetBag()->GetItemPointer(15);

                    if (item) {
                        ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

                        if (stats) {
                            recovery = recovery * stats->m_delay / 1000;
                        }
                    }
                }
            }
        }

        if ((spell->m_attributes & 0x2) && !(spell->m_attributesExB & 0x20000) && !(spell->m_attributesExD & 0x10000000) && !(spell->m_attributesExI & 0x100)) {
            recovery += player->m_unit->m_unitFieldRangedattacktime;
        }

        if (reset) {
            recovery = Spell_C_GetCategoryResetTime(recovery, daily, player->m_player->m_playerFieldHomeRealmTimeOffset);
        }
    }

    if (spell->m_attributesExF >= 0) {
        if (pet) {
            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (unit && category && unit->HasAuraEffect(341)) {
            int32_t reduction = unit->GetCategoryCooldownModifier(category);

            if (reduction <= recovery) {
                recovery -= reduction;
            } else {
                recovery = 0;
            }
        }

        float flat;
        float pct;

        if (Spell_C_GetSpellModifiers(spell, 11, flat, pct)) {
            recovery = static_cast<int32_t>((static_cast<float>(recovery) + flat) * pct * 0.0099999998f);
        }
    }
}

// OFFSET: 0x008B05D0
void Spell_C_SetCooldownLeft(int32_t spellId, uint32_t itemId, uint32_t category, int32_t cooldownLeft, int32_t categoryCooldownLeft, bool onHold, int32_t pet) {
    uint32_t now = OsGetAsyncTimeMs();
    uint32_t index = 0;
    uint32_t time = 0;
    int32_t recovery = 0;
    uint32_t categoryTime = 0;
    int32_t categoryRecovery = 0;
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    ItemRecSparse* stats = NULL;

    if (itemId) {
        stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

        if (stats) {
            while (stats->m_spellID[index] != spellId) {
                if (++index >= 5) {
                    stats = NULL;
                    break;
                }
            }
        }
    }

    const SpellCooldownsRec* cooldowns = NULL;

    if (spell->m_spellCooldownsID) {
        cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);
    }

    if (cooldownLeft > 0) {
        int32_t value;

        if (!stats || (value = stats->m_spellCooldown[index]) < 0) {
            value = cooldowns ? cooldowns->m_recoveryTime : 0;
        }

        if (value <= cooldownLeft) {
            value = cooldownLeft;
        }

        float flat;
        float pct;

        if (Spell_C_GetSpellModifiers(spell, 11, flat, pct)) {
            value = static_cast<int32_t>((static_cast<float>(value) + flat) * pct * 0.0099999998f);
        }

        recovery = value;
        time = now - (value - cooldownLeft);
    }

    if (categoryCooldownLeft > 0) {
        int32_t value;

        if (stats && stats->m_spellCategoryCooldown[index] >= 0) {
            value = stats->m_spellCategoryCooldown[index];
            Spell_C_ModifyCategoryRecovery(value, stats->m_spellCategory[index], 0);
        } else {
            value = cooldowns ? cooldowns->m_categoryRecoveryTime : 0;
            Spell_C_ModifyCategoryRecovery(value, spell, pet, 0);
        }

        if (value <= categoryCooldownLeft) {
            value = categoryCooldownLeft;
        }

        categoryTime = now - (value - categoryCooldownLeft);
        categoryRecovery = value;
    }

    if (!recovery && !categoryRecovery && !onHold) {
        return;
    }

    SpellHistory& history = s_spellHistory[pet];
    SPELLHISTORY* entry = history.m_free.Head();

    if (entry) {
        entry->m_link.Unlink();
        history.m_history.LinkToTail(entry);
    } else {
        entry = history.m_history.NewNode(2, 0, 8);
    }

    entry->m_spellId = spellId;
    entry->m_itemId = itemId;
    entry->m_recoveryTime = recovery;
    entry->m_time = time;
    entry->m_category = category;
    entry->m_categoryRecoveryTime = categoryRecovery;
    entry->m_castCount = 0;
    entry->m_categoryTime = categoryTime;
    entry->m_onHold = onHold;
    entry->m_startRecoveryCategory = 0;
    entry->m_startRecoveryTime = 0;
}

// OFFSET: 0x008B0880
void Spell_C_PendingItemCooldownCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    HASHKEY_NONE key;
    ITEMCOOLDOWNHASHNODE* node = s_itemCooldowns.Ptr(id, key);

    if (!node) {
        return;
    }

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(id, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        s_itemCooldowns.Delete(node);
        return;
    }

    int32_t spellId = node->m_spellId;
    uint32_t index;

    for (index = 0; index < 5; index++) {
        if (stats->m_spellID[index] == spellId) {
            break;
        }
    }

    if (index < 5) {
        SpellRec* spell = g_spellDB.GetRecord(spellId);
        int32_t defaultRecovery = 0;
        int32_t defaultCategoryRecovery = 0;
        const SpellCooldownsRec* cooldowns;

        if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID))) {
            defaultRecovery = cooldowns->m_recoveryTime;
            defaultCategoryRecovery = cooldowns->m_categoryRecoveryTime;
        }

        int32_t recovery = stats->m_spellCooldown[index] >= 0 ? stats->m_spellCooldown[index] : defaultRecovery;
        int32_t category = 0;
        int32_t categoryRecovery = 0;

        if (param) {
            int32_t spellCategory = 0;
            const SpellCategoriesRec* categories;

            if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
                spellCategory = categories->m_category;
            }

            category = spellCategory;

            if (stats->m_spellCategory[index] > 0) {
                category = stats->m_spellCategory[index];
            }

            categoryRecovery = stats->m_spellCategoryCooldown[index] >= 0 ? stats->m_spellCategoryCooldown[index] : defaultCategoryRecovery;
            Spell_C_ModifyCategoryRecovery(categoryRecovery, category, 0);
        }

        if (recovery || categoryRecovery) {
            bool onHold = node->m_onHold;
            uint32_t time = node->m_time;

            if (recovery || categoryRecovery || onHold) {
                SpellHistory& history = s_spellHistory[0];
                SPELLHISTORY* entry = history.m_free.Head();

                if (entry) {
                    entry->m_link.Unlink();
                    history.m_history.LinkToTail(entry);
                } else {
                    entry = history.m_history.NewNode(2, 0, 8);
                }

                entry->m_spellId = spellId;
                entry->m_itemId = id;
                entry->m_category = category;
                entry->m_castCount = 0;
                entry->m_time = time;
                entry->m_recoveryTime = recovery;
                entry->m_categoryTime = time;
                entry->m_categoryRecoveryTime = categoryRecovery;
                entry->m_onHold = onHold;
                entry->m_startRecoveryCategory = 0;
                entry->m_startRecoveryTime = 0;
            }
        }
    }

    CGSpellBook::UpdateCooldowns();
    CGContainerInfo::UpdateCooldowns();
    s_itemCooldowns.Delete(node);
}

// OFFSET: 0x008B0B10
bool Spell_C_HasCooldownOnEvent(const SpellRec* spell, WOWGUID guid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (!item || !(item->m_obj->m_type & TYPEMASK_ITEM)) {
        return (spell->m_attributes & 0x2000000) != 0;
    }

    CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_UNIT, __FILE__, __LINE__));
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    int32_t slot = item->GetOnUseSlot();
    int32_t category;

    if (stats && owner && slot >= 0) {
        category = stats->m_spellCategory[slot];
    } else {
        const SpellCategoriesRec* categories;

        if (!spell->m_spellCategoriesID || !(categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
            return (spell->m_attributes & 0x2000000) != 0;
        }

        category = categories->m_category;
    }

    SpellCategoryRec* rec = g_spellCategoryDB.GetRecord(category);

    if (rec && (rec->m_flags & 0x4)) {
        return true;
    }

    return (spell->m_attributes & 0x2000000) != 0;
}

// OFFSET: 0x008B0C20
void Spell_C_StartSpellCooldown(const SpellRec* spell, CGUnit_C* unit, uint8_t castCount, int32_t delay) {
    int32_t startRecoveryCategory = 0;

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories) {
            startRecoveryCategory = categories->m_startRecoveryCategory;
        }
    }

    int32_t startRecoveryTime = 0;

    if (spell->m_spellCooldownsID) {
        SpellCooldownsRec* cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);

        if (cooldowns) {
            startRecoveryTime = cooldowns->m_startRecoveryTime;
        }
    }

    if (!startRecoveryCategory && !startRecoveryTime) {
        return;
    }

    float flat;
    float pct;

    if (Spell_C_GetSpellModifiers(spell, 21, flat, pct)) {
        startRecoveryTime = static_cast<int32_t>((static_cast<float>(startRecoveryTime) + flat) * pct * 0.0099999998f);
    }

    if (startRecoveryCategory == 133 && !SpellIsPhysicalAttack(spell) && startRecoveryTime > 1000) {
        startRecoveryTime = static_cast<int32_t>(static_cast<float>(startRecoveryTime) * unit->m_unit->m_unitModCastHaste);

        if (static_cast<uint32_t>(startRecoveryTime - 1000) > 500) {
            startRecoveryTime = startRecoveryTime < 1000 ? 1000 : 1500;
        }
    }

    if (startRecoveryTime <= 0) {
        return;
    }

    int32_t pet = unit->m_obj->m_guid == CGPetInfo::GetPet(0);
    bool onHold = Spell_C_HasCooldownOnEvent(spell, unit->m_obj->m_guid);
    uint32_t now = OsGetAsyncTimeMs();
    s_spellHistory[pet].AddHistory(spell->m_id, 0, delay + now, 0, 0, delay + now, 0, onHold, startRecoveryCategory, startRecoveryTime, castCount);
    CGSpellBook::UpdateCooldowns();

    if (spell->m_attributes & 0x2000000) {
        CGSpellBook::UpdateUsable();
    }

    if (pet) {
        CGPetInfo::UpdateCooldowns();
    }
}

// OFFSET: 0x008B0DD0/0x1405F66A0
bool Spell_C_GetSpellCooldown(int32_t spellId, int32_t petSpell, uint32_t* duration, uint32_t* start, uint32_t* enable, int32_t* remaining) {
    return s_spellHistory[petSpell].GetCooldown(spellId, 0, duration, start, enable, remaining);
}

// OFFSET: 0x008B0E00/0x1405F66F0
bool Spell_C_GetItemCooldown(int32_t itemId, uint32_t* duration, uint32_t* start, uint32_t* enable) {
    WOWGUID player = ClntObjMgrGetActivePlayer();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, player, ItemCheckCooldownCallback, NULL, true);

    if (!stats) {
        return false;
    }

    uint32_t index;

    for (index = 0; index < 5; index++) {
        if (stats->m_spellID[index] > 0 && !stats->m_spellTrigger[index]) {
            break;
        }
    }

    if (index >= 5) {
        return false;
    }

    return s_spellHistory[0].GetCooldown(stats->m_spellID[index], itemId, duration, start, enable, NULL);
}

// OFFSET: 0x008B0E90/0x1405F67B0
bool Spell_C_GetItemCooldown(CGItem_C* item, uint32_t* duration, uint32_t* start, uint32_t* enable) {
    if (!item) {
        return false;
    }

    int32_t spellId = item->GetEnchantmentSpell(0);
    return s_spellHistory[0].GetCooldown(spellId, item->m_obj->m_entryID, duration, start, enable, NULL);
}

// OFFSET: 0x008B0ED0/0x1405F6830
int32_t Spell_C_WasItemRecentlyUsed(int32_t itemId) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    uint32_t index;

    for (index = 0; index < 5; index++) {
        if (stats->m_spellID[index] > 0 && !stats->m_spellTrigger[index]) {
            break;
        }
    }

    if (index >= 5) {
        return 0;
    }

    return s_spellHistory[0].RecentlyCast(stats->m_spellID[index], itemId);
}

// OFFSET: 0x008B0F50
int32_t Spell_C_IsActionOnHold(const int32_t* spellId, int32_t index) {
    return s_spellHistory[index].IsOnHold(*spellId, 0);
}

// OFFSET: 0x008B0F70/0x1405F6910
int32_t Spell_C_GetPowerCost(const SpellRec* spell, const CGUnit_C* unit) {
    if (!unit) {
        return -1;
    }

    const SpellLevelsRec* levels = NULL;

    if (spell->m_spellLevelsID) {
        levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);
    }

    int32_t cost = 0;
    bool negative = false;
    const SpellPowerRec* power;

    if (spell->m_spellPowerID && (power = g_spellPowerDB.GetRecord(spell->m_spellPowerID))) {
        float pct = power->m_manaCostPercentageFloat;
        cost = power->m_manaCost;

        if (!fnotequal(pct, 0.0f)) {
            pct = static_cast<float>(power->m_manaCostPercentage);
        }

        if (cost < 0 || pct < 0.0f) {
            cost = -cost;
            negative = true;
        } else {
            negative = false;
        }

        if (power->m_manaCostPercentage) {
            if (negative) {
                pct = -pct;
            }

            cost += static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(unit->GetBasePowerForType(spell->m_powerType))) * pct * 0.0099999998f);
        }

        SpellScalingRec* scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID);

        if (scaling) {
            cost = static_cast<int32_t>(Spell_C_GetSpellScalingMultiplier(unit, scaling, true) * static_cast<float>(cost));
        } else {
            int32_t perLevel = power->m_manaCostPerLevel;

            if (negative) {
                perLevel = -perLevel;
            }

            cost = perLevel * Spell_C_GetSpellLevelBonus(spell, unit->GetSpellLevel(spell)) + cost;
        }
    }

    if ((spell->m_attributesExD & 0x400) && const_cast<CGUnit_C*>(unit)->IsActivePlayer()) {
        uint32_t speed = 0;
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(unit->GetShapeshiftForm());

        if (form && form->m_combatRoundTime) {
            speed = form->m_combatRoundTime;
        } else {
            uint32_t slot = ((spell->m_attributesExC & 0x1000000) != 0) + 15;
            const CGItem_C* item = const_cast<CGUnit_C*>(unit)->GetBag()->GetItemPointer(slot);

            if (item && item->IsUsable() && !unit->IsDisarmed((spell->m_attributesExC >> 24) & 0x1)) {
                ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

                if (stats) {
                    speed = stats->m_delay;
                }
            } else {
                speed = 2000;
            }
        }

        cost += speed / 100;
    }

    int32_t powerType = spell->m_powerType;

    if (unit->m_unit->m_unitFieldBytes0[3] == powerType) {
        int32_t school = spell->m_schoolMask;
        cost += unit->GetSpellCostModifier(school);
        cost = static_cast<int32_t>(unit->GetSpellCostMultiplier(school) * static_cast<float>(cost));
    }

    float flat;
    float pct;

    if (Spell_C_GetSpellModifiers(spell, 14, flat, pct)) {
        cost = static_cast<int32_t>((static_cast<float>(cost) + flat) * pct * 0.0099999998f);
    }

    if (levels && !(unit->m_obj->m_type & TYPEMASK_PLAYER) && (spell->m_attributes & 0x80000) && !powerType && levels->m_spellLevel) {
        float spellLevelValue = GameFormulaEvaluateTable(6, levels->m_spellLevel - 1, 0);
        float unitLevelValue = GameFormulaEvaluateTable(6, unit->m_unit->m_unitFieldLevel - 1, 0);

        if (spellLevelValue > 0.0f && unitLevelValue > 0.0f) {
            cost = static_cast<int32_t>(unitLevelValue / spellLevelValue * static_cast<float>(cost));
        }
    }

    if (negative) {
        cost = -cost;
    }

    if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && SpellHasEffect(spell, 97)) {
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t i = 0; i < count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect || effect->m_effect != 97) {
                continue;
            }

            uint32_t first = effect->m_effectMiscValue;
            int32_t slots = effect->m_effectMiscValueB;

            if (first >= 12 || slots + first > 12) {
                continue;
            }

            for (int32_t j = 0; j < slots; j++) {
                int32_t petSpell;
                int32_t spellId = CGActionBar::GetSpell(first + j + 132, petSpell);

                if (!spellId || (spellId & 0xF0000000)) {
                    continue;
                }

                SpellRec* totem = g_spellDB.GetRecord(spellId & 0xF1FFFFFF);

                if (totem && (totem->m_attributesExG & 0x20)) {
                    cost += Spell_C_GetPowerCost(totem, unit);
                }
            }
        }
    }

    if (negative) {
        return cost >= 0 ? 0 : cost;
    }

    return cost <= 0 ? 0 : cost;
}

// OFFSET: 0x008B1470/0x1405F7050
int32_t Spell_C_GetCastTime(const SpellRec* spell, int32_t isPet, int32_t isVehicle, int32_t allowNegative) {
    if (!spell) {
        return 0;
    }

    int32_t castTime = 0;
    SpellScalingRec* scaling;

    if (!spell->m_spellScalingID || !(scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID)) || !Spell_C_SpellScalingCastTime(Spell_C_GetStatTargetUnit(isPet, isVehicle), scaling, castTime)) {
        SpellCastTimesRec* rec = g_spellCastTimesDB.GetRecord(spell->m_castingTimeIndex);

        if (!rec) {
            return 0;
        }

        uint32_t level = Spell_C_GetSpellLevel(spell, isPet, isVehicle);
        castTime = rec->m_base + rec->m_perLevel * Spell_C_GetSpellLevelBonus(spell, level);

        if (castTime < rec->m_minimum) {
            castTime = rec->m_minimum;
        }
    }

    float flat;
    float pct;

    if (Spell_C_GetSpellModifiers(spell, 10, flat, pct)) {
        castTime = static_cast<int32_t>((static_cast<float>(castTime) + flat) * pct * 0.0099999998f);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (spell->m_attributes & 0x10) {
        if ((spell->m_attributesExI & 0x2000) && !isPet) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                CGBag_C* bag = &player->m_bag;

                if (bag) {
                    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(bag->GetItemGuid(15)), TYPEMASK_ITEM, __FILE__, __LINE__));

                    if (item) {
                        ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, player->m_obj->m_guid, NULL, NULL, false);
                        uint32_t speed = player->m_unit->m_unitFieldBaseattacktime[0];

                        if (stats && stats->m_delay && speed) {
                            castTime = static_cast<int32_t>(static_cast<double>(speed) / static_cast<double>(stats->m_delay) * static_cast<double>(castTime));
                        }
                    }
                }
            }
        }
    } else if (!(spell->m_attributes & 0x20) && !(spell->m_attributesExC & 0x20000000)) {
        if (isPet) {
            if (!unit) {
                goto ranged;
            }

            const WOWGUID* guid = &unit->m_unit->m_unitFieldCharm;

            if (!*guid) {
                guid = &unit->m_unit->m_unitFieldSummon;
            }

            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*guid, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (unit && castTime > 0) {
            float speed = unit->m_unit->m_unitModCastSpeed;

            if (fnotequal(speed, 1.0f)) {
                castTime = static_cast<int32_t>(static_cast<float>(castTime) * speed);
            }
        }
    }

ranged:
    if ((spell->m_attributes & 0x2) && !isPet) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGBag_C* bag = &player->m_bag;

            if (bag) {
                CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(bag->GetItemGuid(17)), TYPEMASK_ITEM, __FILE__, __LINE__));

                if (item) {
                    ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, player->m_obj->m_guid, NULL, NULL, false);

                    if (stats && stats->m_delay) {
                        uint32_t speed = player->m_unit->m_unitFieldRangedattacktime;

                        if (speed) {
                            castTime = static_cast<int32_t>(static_cast<double>(speed) / static_cast<double>(stats->m_delay) * static_cast<double>(castTime));
                        }
                    }
                }
            }
        }
    }

    if (!allowNegative) {
        return castTime > 0 ? castTime : 0;
    }

    return castTime;
}

// OFFSET: 0x008B1800/0x1405F7460
void Spell_C_GetMinMaxRange(CGUnit_C* caster, const SpellRec* spell, float* minRange, float* maxRange, int32_t friendly, CGObject_C* target) {
    CGUnit_C* targetUnit = target && (target->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(target) : NULL;

    if (spell->m_attributes & 0x404) {
        *maxRange = 100.0f;
        return;
    }

    float bonus = 0.0f;
    SpellRangeRec* rec = g_spellRangeDB.GetRecord(spell->m_rangeIndex);

    if (rec) {
        if (rec->m_flags & 1) {
            *minRange = 0.0f;
            float reach;

            if (!targetUnit && !(targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster->m_combat.m_attacking, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
                reach = caster->m_unit->m_unitFieldCombatreach;
            } else {
                reach = targetUnit->m_unit->m_unitFieldCombatreach;
            }

            bonus = caster->m_unit->m_unitFieldCombatreach + reach + 1.3333334f;

            if (bonus <= 5.0f) {
                bonus = 5.0f;
            }
        } else {
            if (rec->m_flags & 2) {
                float reach = targetUnit ? targetUnit->m_unit->m_unitFieldCombatreach : caster->m_unit->m_unitFieldCombatreach;
                float value = caster->m_unit->m_unitFieldCombatreach + reach + 1.3333334f;
                *minRange = value;

                if (value <= 5.0f) {
                    value = 5.0f;
                }

                *minRange = value;
                *minRange = rec->m_rangeMin[friendly] + value;
            } else {
                *minRange = rec->m_rangeMin[friendly];
            }

            *maxRange = rec->m_rangeMax[friendly];

            if (target && (target->m_obj->m_type & 0x88)) {
                float reach = caster->m_unit->m_unitFieldCombatreach;

                if (targetUnit) {
                    reach = targetUnit->m_unit->m_unitFieldCombatreach;
                }

                bonus = reach + caster->m_unit->m_unitFieldCombatreach;

                if (*minRange != 0.0f && !(rec->m_flags & 2)) {
                    *minRange = *minRange + bonus;
                }
            }
        }

        if (targetUnit && (caster->m_movement->m_flags & 0x80D) && (targetUnit->m_movement->m_flags & 0x80D) && !caster->IsRunning() && !targetUnit->IsRunning() && ((rec->m_flags & 1) || (targetUnit->m_obj->m_type & TYPEMASK_PLAYER))) {
            bonus = bonus + 2.6666667f;
        }
    }

    if ((spell->m_attributes & 2) && (caster->m_obj->m_type & TYPEMASK_PLAYER)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(caster);
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(player->m_bag.GetItemGuid(17)), TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, caster->m_obj->m_guid, NULL, NULL, false);

            if (stats) {
                *maxRange = stats->m_rangedModRange * 0.0099999998f * *maxRange;
            }
        }
    }

    float flat;
    float pct;

    if (Spell_C_GetSpellModifiers(spell, 5, flat, pct)) {
        *maxRange = (*maxRange + flat) * pct * 0.0099999998f;
    }

    *maxRange = *maxRange + bonus;
}

// OFFSET: 0x008B1B50
bool Spell_C_CheckEnchantItemLevel(const PendingSpellCast* cast, const SpellRec* spell, WOWGUID itemGuid, int32_t* reason) {
    if (!itemGuid) {
        return true;
    }

    CGObject_C* item = ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__);

    if (!item) {
        *reason = SPELL_FAILED_NO_VALID_TARGETS;
        return false;
    }

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects;

    if (!stats || (effects = SpellGetEffects(spell->m_id, count), !count)) {
        return true;
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && effect->m_effect >= 53 && (effect->m_effect <= 54 || effect->m_effect == 156) && !Spell_C_CheckItemLevel(cast, spell, item, stats, reason)) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x008B1C40/0x1405F7930
bool Spell_C_HaveSpellPower(CGUnit_C* unit, const SpellRec* spell, int32_t& powerType, int32_t& requirement) {
    powerType = spell->m_powerType;
    requirement = -1;

    if (!unit) {
        return false;
    }

    if (spell->m_powerType == -2) {
        int32_t cost = Spell_C_GetPowerCost(spell, unit);
        return cost < unit->GetPredictedHealth();
    }

    if (spell->m_powerType == 5) {
        return unit->IsActivePlayer() && CGRuneInfo::HasRunesForSpell(spell->m_id, NULL);
    }

    bool wrongBar = false;
    int32_t cost = Spell_C_GetPowerCost(spell, unit);
    bool inverted = spell->m_powerType == 8;

    if (spell->m_powerType == 10 && cost && spell->m_spellPowerID) {
        SpellPowerRec* power = g_spellPowerDB.GetRecord(spell->m_spellPowerID);

        if (power && power->m_unitPowerBarID) {
            if (power->m_unitPowerBarID != unit->m_powerBarID) {
                wrongBar = true;
            }

            UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(power->m_unitPowerBarID);

            if (bar && bar->m_centerPower > bar->m_minPower) {
                inverted = true;
            }
        }
    }

    if (cost < 0) {
        requirement = 1;

        if (!wrongBar && (!inverted || cost >= unit->GetPredictedPower(spell->m_powerType))) {
            return true;
        }
    } else {
        requirement = 0;

        if (!wrongBar && cost <= unit->GetPredictedPower(spell->m_powerType)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008B1D90/0x1405F7B10
void Spell_C_CancelSpell(PendingSpellCast* cast, bool stopRecast, bool notifyServer, int32_t reason) {
    SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);

    if (!spell) {
        return;
    }

    CGGlyphInfo::ClearSelectedGlyphBySpell(spell->m_id);

    if (CGTradeInfo::m_enchantingItem && CGTradeInfo::m_enchantingItem == cast->m_data.m_castItem) {
        CGTradeInfo::SetEnchantingItem(NULL);
    }

    if (cast != s_targetingSpell && notifyServer) {
        CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

        if ((caster && caster->GetSpellCastingTime(spell) > 0) || (spell->m_attributes & 0x404)) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_CANCEL_CAST));
            msg.Put(cast->m_data.m_castCount);
            msg.Put(static_cast<uint32_t>(cast->m_data.m_spellId));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    Spell_C_FailPendingCast(cast, spell, reason);

    if (stopRecast) {
        CGTradeSkillInfo::StopRecast();
    }
}

// OFFSET: 0x008B1EE0
void Spell_C_CancelSpellByCaster(const WOWGUID& caster) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast->m_data.m_castItem == caster) {
            Spell_C_CancelSpell(cast, true, true, SPELL_FAILED_DONT_REPORT);
        }
    }
}

// OFFSET: 0x008B1F30/0x1405F7D10
void Spell_C_CancelSpellByTarget(const WOWGUID& target, int32_t reason) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast->m_data.m_objectTarget == target) {
            Spell_C_CancelSpell(cast, true, true, reason);
        }
    }
}

// OFFSET: 0x008B1F80/0x1405F7DB0
void Spell_C_CancelSpellBySpellID(int32_t spellId) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast->m_data.m_spellId == static_cast<uint32_t>(spellId)) {
            Spell_C_CancelSpell(cast, true, true, SPELL_FAILED_DONT_REPORT);
        }
    }
}

// OFFSET: 0x008B1FC0/0x1405F7E30
void Spell_C_CancelMeleeSpells() {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];
        SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);

        if (spell && (spell->m_attributes & 0x404)) {
            Spell_C_CancelSpell(cast, false, true, SPELL_FAILED_DONT_REPORT);
        }
    }
}

// OFFSET: 0x008B2030
void Spell_C_CancelRangedSpells() {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];
        SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);

        if (spell && (spell->m_attributes & 0x2)) {
            Spell_C_CancelSpell(cast, false, true, SPELL_FAILED_DONT_REPORT);
        }
    }
}

// OFFSET: 0x008B20A0
void Spell_C_CancelOtherRangedSpells(int32_t spellId) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        int32_t castSpell = s_pendingSpellCasts[i].m_data.m_spellId;

        if (!i && castSpell == spellId) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(castSpell);

        if (spell && (spell->m_attributes & 0x2)) {
            Spell_C_CancelSpell(&s_pendingSpellCasts[i], false, true, SPELL_FAILED_DONT_REPORT);
        }
    }
}

// OFFSET: 0x008B2120
void Spell_C_CancelCastingSpellByMovement(CGUnit_C* unit) {
    for (uint32_t i = 0; i < s_numPendingSpellCasts; i++) {
        PendingSpellCast* cast = &s_pendingSpellCasts[i];

        if (cast == s_targetingSpell || cast->m_data.m_caster != unit->m_obj->m_guid) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);
        const SpellInterruptsRec* interrupts;

        if (spell && spell->m_spellInterruptsID && (interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID)) && (interrupts->m_interruptFlags & 0x1) && !unit->HasAuraMatchingSpellClass(330, spell)) {
            Spell_C_CancelSpell(cast, false, true, SPELL_FAILED_INTERRUPTED);
        }
    }
}

// OFFSET: 0x008B2200
int32_t Packet_SMSG_SPELL_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID caster;
    *msg >> caster;
    uint8_t castCount;
    msg->Get(castCount);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint8_t reason;
    msg->Get(reason);
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        PendingSpellCast local;
        PendingSpellCast* cast = FindPendingSpellCast(caster, &local, castCount, spellId, caster);
        Spell_C_FailPendingCast(cast, spell, reason);
    }

    return 1;
}

// OFFSET: 0x008B22C0
int32_t Packet_SMSG_PET_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID pet;

    if (player) {
        pet = player->m_unit->m_unitFieldCharm.m_guid ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
    } else {
        pet = WOWGUID::Make(0);
    }

    uint8_t castCount;
    msg->Get(castCount);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint8_t reason;
    msg->Get(reason);
    int32_t misc1 = -1;
    int32_t misc2 = -1;

    if (msg->m_read < msg->m_size) {
        msg->Get(reinterpret_cast<uint32_t&>(misc1));
    }

    if (msg->m_read < msg->m_size) {
        msg->Get(reinterpret_cast<uint32_t&>(misc2));
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        PendingSpellCast local;
        WOWGUID caster = pet;
        WOWGUID castItem = pet;
        PendingSpellCast* cast = FindPendingSpellCast(castItem, &local, castCount, spellId, caster);
        s_spellHistory[1].RemoveStartCooldown(spellId, 0);
        CGPetInfo::UpdateCooldowns();
        Spell_C_PetSpellFailed(cast, spell, reason, misc1, misc2);
    }

    return 1;
}

// OFFSET: 0x008B2410
int32_t Packet_SMSG_SPELL_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    bool startRecovery = false;
    bool ignoreEvent = false;
    WOWGUID guid;
    msg->Get(guid.m_guid);
    uint8_t flags;
    msg->Get(flags);

    if (flags & 0x1) {
        startRecovery = true;
    }

    if (flags & 0x2) {
        ignoreEvent = true;
    }

    int32_t pet;

    if (guid == ClntObjMgrGetActivePlayer()) {
        pet = 0;
    } else {
        if (guid != CGPetInfo::GetPet(0)) {
            return 1;
        }

        pet = 1;
    }

    while (!msg->IsRead()) {
        int32_t spellId;
        msg->Get(reinterpret_cast<uint32_t&>(spellId));
        int32_t cooldown;
        msg->Get(reinterpret_cast<uint32_t&>(cooldown));
        SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (!spell) {
            continue;
        }

        bool onHold = ignoreEvent ? false : Spell_C_HasCooldownOnEvent(spell, guid);
        int32_t categoryRecoveryTime = 0;
        int32_t recoveryTime = 0;
        int32_t startRecoveryTime = 0;
        const SpellCooldownsRec* cooldowns;

        if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID))) {
            categoryRecoveryTime = cooldowns->m_categoryRecoveryTime;
            recoveryTime = cooldowns->m_recoveryTime;
            startRecoveryTime = cooldowns->m_startRecoveryTime;
        }

        int32_t recovery;

        if (cooldown) {
            recovery = cooldown;
        } else {
            recovery = recoveryTime;
            float flat;
            float pct;

            if (Spell_C_GetSpellModifiers(spell, 11, flat, pct)) {
                recovery = static_cast<int32_t>((static_cast<float>(recoveryTime) + flat) * pct * 0.0099999998f);
            }
        }

        int32_t startRecoveryCategory = 0;
        int32_t category = 0;
        const SpellCategoriesRec* categories;

        if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
            startRecoveryCategory = categories->m_startRecoveryCategory;
            category = categories->m_category;
        }

        int32_t categoryRecovery;

        if (cooldown) {
            categoryRecovery = 0;
        } else {
            categoryRecovery = categoryRecoveryTime;
            Spell_C_ModifyCategoryRecovery(categoryRecovery, spell, pet, 0);
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->SpellHasteCooldowns(spell, recovery, categoryRecovery);
        }

        int32_t startRecoveryValue = 0;

        if (!startRecovery || onHold) {
            startRecoveryCategory = 0;
        } else {
            startRecoveryValue = startRecoveryTime;
            float flat;
            float pct;

            if (Spell_C_GetSpellModifiers(spell, 21, flat, pct)) {
                startRecoveryValue = static_cast<int32_t>((static_cast<float>(startRecoveryValue) + flat) * pct * 0.0099999998f);
            }
        }

        s_spellHistory[pet].AddHistory(spellId, 0, time, recovery, category, time, categoryRecovery, onHold, startRecoveryCategory, startRecoveryValue, 0);
    }

    CGSpellBook::UpdateCooldowns();

    if (pet) {
        CGPetInfo::UpdateCooldowns();
    } else {
        CGContainerInfo::UpdateCooldowns();
    }

    return 1;
}

// OFFSET: 0x008B26D0/0x1405F8AD0
void Spell_C_CancelAutoRepeat(bool notifyServer) {
    if (!s_autoRepeatingSpell) {
        return;
    }

    if (notifyServer) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CANCEL_AUTO_REPEAT_SPELL));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->SetCastingAnim(0);
        player->CancelRangedMode();
    }

    Spell_C_CancelSpellBySpellID(s_autoRepeatingSpell);

    if (!s_autoRepeatingSpell) {
        return;
    }

    s_autoRepeatingSpell = 0;

    if (s_autoRangedCombatSpell) {
        return;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_STOP_AUTOREPEAT_SPELL, NULL);

    if (s_autoRepeatingSpell) {
        static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->MeleeModeLeave(true);
    }
}

// OFFSET: 0x008B3090
bool SpellHistory::RefreshCategoryRecovery(int32_t category) {
    bool changed = false;
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_onHold || history->m_category != category || !history->m_categoryRecoveryTime) {
            continue;
        }

        uint32_t now = OsGetAsyncTimeMs();
        uint32_t end = history->m_categoryTime + history->m_categoryRecoveryTime;

        if (static_cast<int32_t>(now - end) >= 0) {
            continue;
        }

        int32_t recovery;
        ItemRecSparse* stats;
        uint32_t index;

        if (history->m_itemId && (stats = g_itemSparseDB.GetRecord(history->m_itemId, WOWGUID(), NULL, NULL, false))) {
            for (index = 0; index < 5; index++) {
                if (stats->m_spellCategory[index] == category) {
                    break;
                }
            }

            if (index < 5 && stats->m_spellCategoryCooldown[index] >= 0) {
                recovery = stats->m_spellCategoryCooldown[index];
                Spell_C_ModifyCategoryRecovery(recovery, stats->m_spellCategory[index], 0);
                goto apply;
            }
        }

        {
            SpellRec* spell = g_spellDB.GetRecord(history->m_spellId);
            SpellCooldownsRec* cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);
            recovery = cooldowns ? cooldowns->m_categoryRecoveryTime : 0;
            Spell_C_ModifyCategoryRecovery(recovery, spell, 0, 0);
        }

    apply:
        if (recovery != static_cast<int32_t>(history->m_categoryRecoveryTime) && recovery > static_cast<int32_t>(end - now)) {
            history->m_categoryTime = end - recovery;
            history->m_categoryRecoveryTime = recovery;
            changed = true;
        }
    }

    return changed;
}

// OFFSET: 0x008B3200/0x1405F9AD0
void Spell_C_SpellFailed(PendingSpellCast* cast, const SpellRec* spell, int32_t reason, int32_t misc1, int32_t misc2, bool resetCooldown) {
    int32_t spellId = cast->m_data.m_spellId;
    bool suppress = false;

    if (cast == s_delayedTrajectoryCast) {
        s_delayedTrajectoryCast = NULL;
    }

    CGTradeSkillInfo::StopRecast(spellId);

    if (s_autoRepeatingSpell && (reason == SPELL_FAILED_NOT_READY || reason == SPELL_FAILED_SPELL_IN_PROGRESS)) {
        SpellRec* autoRepeat = g_spellDB.GetRecord(s_autoRepeatingSpell);

        if (autoRepeat && (autoRepeat->m_attributesExC & 0x400000)) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_CANCEL_CAST));
            msg.Put(static_cast<uint8_t>(0));
            msg.Put(static_cast<uint32_t>(s_autoRepeatingSpell));
            msg.Finalize();
            ClientServices::Send2(&msg);
            Spell_C_CancelAutoRepeat(true);
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && reason != SPELL_FAILED_DONT_REPORT) {
        SI3::PlaySpellFizzleSound(spellId, player, true);
    }

    UIErrors error = Spell_C_GetSpellFailedError(spell, reason, misc1);

    if (player) {
        bool autoRepeat;

        if (cast == s_targetingSpell) {
            autoRepeat = spellId == s_autoRepeatingSpell;
        } else if (spellId == s_autoRepeatingSpell) {
            autoRepeat = true;
        } else {
            autoRepeat = false;

            switch (reason) {
                case SPELL_FAILED_LINE_OF_SIGHT:
                case SPELL_FAILED_NEED_AMMO:
                case SPELL_FAILED_NEED_AMMO_POUCH:
                case SPELL_FAILED_NEED_EXOTIC_AMMO:
                case SPELL_FAILED_OUT_OF_RANGE:
                case SPELL_FAILED_TOO_CLOSE:
                case SPELL_FAILED_UNIT_NOT_INFRONT:
                    player->CancelRangedMode();
                    break;
            }
        }

        if (autoRepeat) {
            bool cancel = true;

            if (spellId == s_autoRangedCombatSpell && resetCooldown) {
                switch (reason) {
                    case SPELL_FAILED_BAD_IMPLICIT_TARGETS:
                    case SPELL_FAILED_BAD_TARGETS:
                    case SPELL_FAILED_CASTER_DEAD:
                    case SPELL_FAILED_EQUIPPED_ITEM:
                    case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
                    case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
                    case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
                    case SPELL_FAILED_NEED_AMMO:
                    case SPELL_FAILED_OUT_OF_RANGE:
                    case SPELL_FAILED_TOO_CLOSE:
                    case SPELL_FAILED_UNIT_NOT_INFRONT:
                        break;

                    default:
                        cancel = false;
                        break;
                }
            }

            if (cancel && reason != SPELL_FAILED_DONT_REPORT) {
                Spell_C_CancelAutoRepeat(true);

                switch (reason) {
                    case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
                    case SPELL_FAILED_NEED_AMMO:
                    case SPELL_FAILED_NEED_AMMO_POUCH:
                    case SPELL_FAILED_NEED_EXOTIC_AMMO:
                    case SPELL_FAILED_OUT_OF_RANGE:
                    case SPELL_FAILED_TOO_CLOSE:
                    case SPELL_FAILED_UNIT_NOT_INFRONT:
                        player->CancelRangedMode();
                        break;
                }
            }
        }

        if (spellId == s_autoRangedCombatSpell) {
            if (reason == s_lastAutoRangedCombatError) {
                suppress = true;
            }

            s_lastAutoRangedCombatError = reason;
        }

        uint32_t now = OsGetAsyncTimeMs();

        if (static_cast<uint32_t>(spellId) == s_lastFailedSpellId && reason == s_lastFailedReason) {
            if (static_cast<int32_t>(now - s_lastFailedTime - 3000) < 0) {
                suppress = true;
            }
        } else {
            s_lastFailedSpellId = spellId;
            s_lastFailedReason = reason;
        }

        s_lastFailedTime = now;
    }

    if (reason == SPELL_FAILED_DONT_REPORT) {
        Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
        return;
    }

    char format[1024];
    format[0] = 0;
    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect && (effect->m_effect == 57 || (effect->m_effect == 36 && effect->m_implicitTargetA == 5))) {
        char token[64];
        SStrPrintf(token, sizeof(token), "%s_PET", GetSpellFailedResult(reason));

        if (player) {
            SStrCopy(format, player->GetFrameScriptText(token, -1), sizeof(format));
        } else {
            FrameScript_GetText(token, format, sizeof(format));
        }
    }

    if (!format[0]) {
        if (reason == SPELL_FAILED_CUSTOM_ERROR) {
            GetCustomSpellFailedString(format, sizeof(format), misc1);
            suppress = true;
        }

        if (!format[0]) {
            if (player) {
                SStrCopy(format, player->GetFrameScriptText(GetSpellFailedResult(reason), -1), sizeof(format));
            } else {
                FrameScript_GetText(GetSpellFailedResult(reason), format, sizeof(format));
            }
        }
    }

    if (resetCooldown) {
        s_spellHistory[0].RemoveStartCooldown(spellId, cast->m_data.m_castCount);
    }

    SpellClearMacroGlobalCooldownWait(spell);
    char text[1024];
    const char* message = format;
    bool fail = true;

    switch (reason) {
        case SPELL_FAILED_ALREADY_AT_FULL_POWER: {
            if (!player) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            uint32_t powerType = player->m_unit->m_unitFieldBytes0[3];

            if (powerType > 10) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            const char* power = FrameScript_GetText(Spell_C_GetPowerTypeToken(powerType), -1, GENDER_NOT_APPLICABLE);

            if (!power || !*power) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            SStrCopy(text, power, sizeof(text));
            message = text;
            break;
        }

        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND: {
            bool found = false;

            for (uint32_t i = 0; i < static_cast<uint32_t>(g_itemSubClassMaskDB.m_numRecords); i++) {
                ItemSubClassMaskRec* rec = g_itemSubClassMaskDB.GetRecordByIndex(i);

                if (rec->m_classID == misc1 && rec->m_mask == misc2) {
                    SStrPrintf(text, sizeof(text), format, static_cast<const char*>(rec->m_name));
                    message = text;
                    found = true;
                    break;
                }
            }

            if (!found) {
                ItemSubClassRec* rec = FindAnyItemSubclassRec(misc1, misc2);

                if (rec) {
                    SStrPrintf(text, sizeof(text), format, static_cast<const char*>(rec->m_displayName));
                    message = text;
                }
            }

            break;
        }

        case SPELL_FAILED_NEED_EXOTIC_AMMO: {
            ItemSubClassRec* rec = FindAnyItemSubclassRec(6, 1 << misc1);

            if (rec) {
                SStrPrintf(text, sizeof(text), format, static_cast<const char*>(rec->m_displayName));
                message = text;
            }

            break;
        }

        case SPELL_FAILED_NEED_MORE_ITEMS: {
            WOWGUID guid = { static_cast<uint64_t>(static_cast<int64_t>(spellId)) | 0x1FE0000000000000ull };
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(misc1, guid, SpellMissingItemsCallback, reinterpret_cast<void*>(static_cast<intptr_t>(misc2)), false);

            if (!stats) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            SStrPrintf(text, sizeof(text), format, misc2, stats->GetName(FrameScript_GetPluralIndex(misc2)));
            message = text;
            break;
        }

        case SPELL_FAILED_NO_POWER: {
            int32_t powerBar = 0;
            const SpellPowerRec* power;
            PowerDisplayRec* display;

            if (spell->m_spellPowerID && (power = g_spellPowerDB.GetRecord(spell->m_spellPowerID))) {
                powerBar = power->m_unitPowerBarID;

                if (!powerBar && (display = g_powerDisplayDB.GetRecord(power->m_powerDisplayID))) {
                    char name[128];
                    FrameScript_GetText(display->m_globalStringBaseTag, name, 127);
                    CGGameUI::DisplayError(ERR_OUT_OF_POWER_DISPLAY, name);
                    text[0] = 0;
                    message = text;
                    break;
                }
            }

            if (spell->m_powerType == -2) {
                error = ERR_OUT_OF_HEALTH;
                text[0] = 0;
                message = text;
                break;
            }

            if (misc1 == 8) {
                if (static_cast<uint32_t>(misc2) <= 1) {
                    error = s_eclipseErrors[misc2];
                }

                text[0] = 0;
                message = text;
                break;
            }

            if (misc1 != 10) {
                error = s_powerErrors[spell->m_powerType];
                text[0] = 0;
                message = text;
                break;
            }

            UnitPowerBarRec* bar;

            if (player && powerBar && (bar = g_unitPowerBarDB.GetRecord(powerBar))) {
                CGGameUI::AddErrorMessage(bar->m_outOfError, 1);
            }

            break;
        }

        case SPELL_FAILED_ONLY_SHAPESHIFT: {
            uint32_t count = g_spellShapeshiftFormDB.m_numRecords;

            if (count > 64) {
                count = 64;
            }

            if (!count) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            char forms[512];
            bool first = true;

            for (uint32_t i = 0; i < count; i++) {
                if (!Spell_C_SpellHasShapeshiftMask(spell, i)) {
                    continue;
                }

                SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecordByIndex(i);

                if (!form || !form->m_name || !*form->m_name) {
                    continue;
                }

                if (first) {
                    SStrCopy(forms, form->m_name, sizeof(forms));
                    first = false;
                } else {
                    SStrPack(forms, ", ", sizeof(forms));
                    SStrPack(forms, form->m_name, sizeof(forms));
                }
            }

            if (first) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            SStrPrintf(text, sizeof(text), format, forms);
            message = text;
            break;
        }

        case SPELL_FAILED_REAGENTS:
        case SPELL_FAILED_TOTEMS: {
            if (reason == SPELL_FAILED_REAGENTS && misc1 == -2) {
                error = static_cast<UIErrors>(0);
                text[0] = 0;
                message = text;
                break;
            }

            WOWGUID guid = { static_cast<uint64_t>(static_cast<int64_t>(spellId)) | 0x1FE0000000000000ull };
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(misc1, guid, SpellMissingItemCallback, reinterpret_cast<void*>(static_cast<intptr_t>(reason)), false);

            if (!stats) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            SStrPrintf(text, sizeof(text), format, stats->GetName(0));
            message = text;
            break;
        }

        case SPELL_FAILED_REQUIRES_AREA: {
            AreaTableRec* area = g_areaTableDB.GetRecord(misc1);

            if (area) {
                SStrPrintf(text, sizeof(text), format, static_cast<const char*>(area->m_areaName));
                message = text;
            }

            break;
        }

        case SPELL_FAILED_REQUIRES_SPELL_FOCUS: {
            SpellFocusObjectRec* focus = g_spellFocusObjectDB.GetRecord(misc1);

            if (focus) {
                SStrPrintf(text, sizeof(text), format, static_cast<const char*>(focus->m_name));
                message = text;
            }

            break;
        }

        case SPELL_FAILED_SPELL_IN_PROGRESS: {
            if (resetCooldown && !cast->m_data.m_castCount) {
                fail = false;
            }

            break;
        }

        case SPELL_FAILED_TOO_MANY_OF_ITEM: {
            ItemLimitCategoryRec* limit;

            if (!misc1 || !(limit = g_itemLimitCategoryDB.GetRecord(misc1))) {
                break;
            }

            CGGameUI::DisplayError(error, limit->m_quantity, static_cast<const char*>(limit->m_name));
            WOWGUID guid = ClntObjMgrGetActivePlayer();
            UnitCombatLogSpellFail(guid, spell, CGGameUI::LastErrorMessage());
            Spell_C_FailPendingCast(cast, spell, reason);
            return;
        }

        case SPELL_FAILED_TOTEM_CATEGORY: {
            TotemCategoryRec* totem = g_totemCategoryDB.GetRecord(misc1);

            if (!totem) {
                Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_DONT_REPORT);
                return;
            }

            SStrPrintf(text, sizeof(text), format, static_cast<const char*>(totem->m_name));
            message = text;
            break;
        }

        case SPELL_FAILED_PREVENTED_BY_MECHANIC: {
            SpellMechanicRec* mechanic = g_spellMechanicDB.GetRecord(misc1);

            if (mechanic) {
                SStrPrintf(text, sizeof(text), format, static_cast<const char*>(mechanic->m_stateName));
                message = text;
            }

            break;
        }

        case SPELL_FAILED_MIN_SKILL: {
            SkillLineRec* skill = g_skillLineDB.GetRecord(misc1);
            SStrPrintf(text, sizeof(text), format, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", misc2);
            message = text;
            break;
        }

        case SPELL_FAILED_UNIQUE_GLYPH: {
            CGObject_C* object = ClntObjMgrObjectPtr(cast->m_data.m_castItem, TYPEMASK_OBJECT, __FILE__, __LINE__);
            const char* name = spell->m_name;

            if (object && (object->m_obj->m_type & TYPEMASK_ITEM)) {
                ItemRecSparse* stats = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

                if (stats && stats->GetName(0)) {
                    name = stats->GetName(0);
                }
            }

            SStrPrintf(text, sizeof(text), format, name);
            break;
        }

        case SPELL_FAILED_FISHING_TOO_LOW:
            SStrPrintf(text, sizeof(text), format, misc1);
            message = text;
            break;

        case SPELL_FAILED_EXCEEDED_WEEKLY_USAGE: {
            SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);
            SpellCategoryRec* category = g_spellCategoryDB.GetRecord(categories->m_category);
            SStrPrintf(text, sizeof(text), format, static_cast<const char*>(category->m_name), misc1);
            message = text;
            break;
        }
    }

    if (!suppress) {
        CGGameUI::DisplayError(error, message);
        WOWGUID guid = ClntObjMgrGetActivePlayer();

        if (*message) {
            UnitCombatLogSpellFail(guid, spell, message);
        } else {
            UnitCombatLogSpellFail(guid, spell, CGGameUI::LastErrorMessage());
        }
    }

    if (fail) {
        Spell_C_FailPendingCast(cast, spell, reason);
    }
}

// OFFSET: 0x008B3F90/0x1405FAD90
void ItemCheckCooldownCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success && Spell_C_GetItemCooldown(id, NULL, NULL, NULL)) {
        CGSpellBook::UpdateCooldowns();
        CGContainerInfo::UpdateCooldowns();
    }
}

// OFFSET: 0x008B3FC0/0x1405FADC0
int32_t Spell_C_NeedsCooldownEvent(int32_t itemId) {
    WOWGUID player = ClntObjMgrGetActivePlayer();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, player, ItemCheckCooldownCallback, NULL, true);

    if (!stats) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (stats->m_spellID[i] > 0 && !stats->m_spellTrigger[i]) {
            return s_spellHistory[0].IsOnHold(stats->m_spellID[i], itemId);
        }
    }

    return s_spellHistory[0].IsOnHold(itemId);
}

// OFFSET: 0x008B4040
void Spell_C_RefreshCategoryCooldowns(int32_t category) {
    if (s_spellHistory[0].RefreshCategoryRecovery(category)) {
        CGSpellBook::UpdateCooldowns();
        CGContainerInfo::UpdateCooldowns();
    }
}

// OFFSET: 0x008B4070/0x1405FAEA0
void Spell_C_GetMinMaxRange(CGUnit_C* caster, const SpellRec* spell, float* minRange, float* maxRange, CGObject_C* target) {
    *minRange = 0.0f;
    *maxRange = 0.0f;

    if (!caster) {
        return;
    }

    int32_t friendly;

    if (!target || !(target->m_obj->m_type & TYPEMASK_UNIT)) {
        friendly = SpellFaction(spell) == 1;
    } else {
        friendly = caster->CanAssist(static_cast<CGUnit_C*>(target), false) != 0;
    }

    Spell_C_GetMinMaxRange(caster, spell, minRange, maxRange, friendly, target);
}

// OFFSET: 0x008B40F0
bool Spell_C_HaveSpellTokens(CGUnit_C* unit, const SpellRec* spell, bool report, PendingSpellCast* cast) {
    if (!unit->IsActivePlayer()) {
        return true;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
    const SpellTotemsRec* totems;

    if (spell->m_spellTotemsID && (totems = g_spellTotemsDB.GetRecord(spell->m_spellTotemsID))) {
        for (uint32_t i = 0; i < 2; i++) {
            if (totems->m_totem[i] && !player->m_bag.FindItemOfType(totems->m_totem[i], 0)) {
                if (!report) {
                    return false;
                }

                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_TOTEMS, totems->m_totem[i], -1, false);
                return false;
            }
        }

        if (!(spell->m_attributesExI & 0x20)) {
            for (uint32_t i = 0; i < 2; i++) {
                if (totems->m_requiredTotemCategoryID[i] && !player->m_bag.FindTotemCategory(totems->m_requiredTotemCategoryID[i], 0)) {
                    if (report) {
                        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_TOTEM_CATEGORY, totems->m_requiredTotemCategoryID[i], -1, false);
                    }

                    return false;
                }
            }
        }
    }

    const SpellReagentsRec* reagents;

    if (!spell->m_spellReagentsID || Spell_C_HasNoReagentCost(player, spell) || !(reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        return true;
    }

    for (uint32_t i = 0; i < 8; i++) {
        int32_t reagent = reagents->m_reagent[i];

        if (!reagent) {
            continue;
        }

        uint32_t have = player->m_bag.GetItemTypeCount(reagent, false);
        uint32_t need = reagents->m_reagentCount[i];
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(reagent, WOWGUID(), NULL, NULL, false);

        if (stats) {
            for (uint32_t j = 0; j < 5; j++) {
                if (stats->m_spellID[j] == spell->m_id && stats->m_spellCharges[j]) {
                    need++;
                }
            }
        }

        if (have < need) {
            if (report) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_REAGENTS, reagent, -1, false);
            }

            return false;
        }
    }

    return true;
}

// OFFSET: 0x008B4350
bool Spell_C_HaveEquippedSpellItems(CGUnit_C* unit, const SpellRec* spell, bool report, PendingSpellCast* cast) {
    if (!unit->IsActivePlayer()) {
        return true;
    }

    const SpellEquippedItemsRec* items = NULL;
    int32_t itemClass = -1;
    int32_t itemSubclass = 0;

    if (spell->m_spellEquippedItemsID) {
        items = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID);

        if (items) {
            itemClass = items->m_equippedItemClass;
            itemSubclass = items->m_equippedItemSubclass;
        }
    }

    uint8_t targets = 0;

    if (spell->m_spellTargetRestrictionsID) {
        SpellTargetRestrictionsRec* restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID);

        if (restrictions) {
            targets = restrictions->m_targets;
        }
    }

    if ((targets & 0x10) || itemClass < 0 || !itemSubclass) {
        return true;
    }

    uint32_t slots;

    if (spell->m_attributesExC & 0x400) {
        slots = 0x8000;
    } else if (spell->m_attributesExC & 0x1000000) {
        slots = 0x10000;
    } else {
        slots = (spell->m_attributes & 0x2) ? 0x20000 : 0xFFFFFFFF;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);

    if (!player->GetEquippedItemForSpell(items, slots)) {
        if (!report) {
            return false;
        }

        int32_t reason;

        if (spell->m_attributesExC & 0x400) {
            reason = SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND;
        } else {
            reason = (spell->m_attributesExC & 0x1000000) ? SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND : SPELL_FAILED_EQUIPPED_ITEM_CLASS;
        }

        Spell_C_SpellFailed(cast, spell, reason, items->m_equippedItemClass, items->m_equippedItemSubclass, false);
        return false;
    }

    if ((spell->m_attributesExC & 0x1000400) == 0x1000400 && !player->GetEquippedItemForSpell(items, 0x10000)) {
        if (report) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND, items->m_equippedItemClass, items->m_equippedItemSubclass, false);
        }

        return false;
    }

    return true;
}

// OFFSET: 0x008B44E0/0x1405FB3B0
void Spell_C_StopTargeting() {
    if (s_targetingSpell) {
        Spell_C_CancelSpell(s_targetingSpell, true, true, SPELL_FAILED_DONT_REPORT);
    }

    if (CGPetInfo::m_petOrderMoveTo) {
        CGPetInfo::ClearPetOrderMoveTo();
    }
}

// OFFSET: 0x008B4510
void Spell_C_StopCasting() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_castSpellId) {
        Spell_C_CancelSpell(Spell_C_GetActivePlayerPendingCast(), true, true, SPELL_FAILED_DONT_REPORT);
    }
}

// PROVEN: 28edb524 IDENTICAL/NONE
// OFFSET: 0x008B4550
void Spell_C_CancelPlayerSpells() {
    WOWGUID player = ClntObjMgrGetActivePlayer();
    Spell_C_CancelSpellByCaster(player);
}

// OFFSET: 0x008B4580
int32_t Packet_SMSG_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID player = ClntObjMgrGetActivePlayer();
    uint8_t castCount;
    msg->Get(castCount);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint8_t reason;
    msg->Get(reason);
    int32_t misc1 = -1;
    int32_t misc2 = -1;

    if (msg->m_read < msg->m_size) {
        msg->Get(reinterpret_cast<uint32_t&>(misc1));
    }

    if (msg->m_read < msg->m_size) {
        msg->Get(reinterpret_cast<uint32_t&>(misc2));
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        PendingSpellCast local;
        PendingSpellCast* cast = FindPendingSpellCast(player, &local, castCount, spellId, player);
        Spell_C_SpellFailed(cast, spell, reason, misc1, misc2, true);

        if (reason != SPELL_FAILED_NOT_READY && (spell->m_attributes & 0x2000000)) {
            s_spellHistory[0].ClearOnHold(spellId, 0, true);
            CGSpellBook::UpdateCooldowns();
            CGSpellBook::UpdateUsable();
            CGContainerInfo::UpdateCooldowns();
        }
    }

    return 1;
}

// OFFSET: 0x008B46A0
int32_t Packet_SMSG_SPELL_FAILED_OTHER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID caster;
    *msg >> caster;
    uint8_t castCount;
    msg->Get(castCount);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint8_t reason;
    msg->Get(reason);
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 1;
    }

    PendingSpellCast local;
    WOWGUID casterGuid = caster;
    PendingSpellCast* cast = FindPendingSpellCast(casterGuid, &local, castCount, spellId, casterGuid);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        SI3::PlaySpellFizzleSound(spellId, unit, unit->IsActivePlayer());

        if (unit->IsActivePlayer() && (reason == SPELL_FAILED_NEED_AMMO || reason == SPELL_FAILED_NEED_AMMO_POUCH || reason == SPELL_FAILED_NEED_EXOTIC_AMMO)) {
            Spell_C_CancelAutoRepeat(true);
        }

        unit->SpellInterrupted(cast->m_data.m_castCount, cast->m_data.m_spellId);
    }

    Spell_C_FailPendingCast(cast, spell, reason);
    return 1;
}

// OFFSET: 0x008B4970
void Spell_C_Destroy() {
    Spell_C_StopTargeting();
    ClientServices::ClearMessageHandler(SMSG_SPELL_START);
    ClientServices::ClearMessageHandler(SMSG_SPELL_GO);
    ClientServices::ClearMessageHandler(SMSG_CAST_FAILED);
    ClientServices::ClearMessageHandler(SMSG_SPELL_FAILED_OTHER);
    ClientServices::ClearMessageHandler(SMSG_SPELL_FAILURE);
    ClientServices::ClearMessageHandler(SMSG_PET_CAST_FAILED);
    ClientServices::ClearMessageHandler(SMSG_SPELL_COOLDOWN);
    ClientServices::ClearMessageHandler(SMSG_ITEM_COOLDOWN);
    ClientServices::ClearMessageHandler(SMSG_COOLDOWN_EVENT);
    ClientServices::ClearMessageHandler(SMSG_CLEAR_COOLDOWN);
    ClientServices::ClearMessageHandler(SMSG_MODIFY_COOLDOWN);
    ClientServices::ClearMessageHandler(SMSG_COOLDOWN_CHEAT);
    ClientServices::ClearMessageHandler(SMSG_PET_TAME_FAILURE);
    ClientServices::ClearMessageHandler(SMSG_SPELL_DELAYED);
    ClientServices::ClearMessageHandler(MSG_CHANNEL_START);
    ClientServices::ClearMessageHandler(MSG_CHANNEL_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_SET_FLAT_SPELL_MODIFIER);
    ClientServices::ClearMessageHandler(SMSG_SET_PCT_SPELL_MODIFIER);
    ClientServices::ClearMessageHandler(SMSG_GAME_OBJECT_RESET_STATE);
    ClientServices::ClearMessageHandler(SMSG_FEIGN_DEATH_RESISTED);
    ClientServices::ClearMessageHandler(SMSG_SPELL_UPDATE_CHAIN_TARGETS);
    ClientServices::ClearMessageHandler(SMSG_NOTIFY_DEST_LOC_SPELL_CAST);
    ClientServices::ClearMessageHandler(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA);
    ClientServices::ClearMessageHandler(SMSG_SET_PROJECTILE_POSITION);
    ClientServices::ClearMessageHandler(SMSG_RESET_RANGED_COMBAT_TIMER);
    ClientServices::ClearMessageHandler(SMSG_RESUME_CAST_BAR);
    ClientPlaySpellVisualKit::s_handler = NULL;
    ClientWeeklySpellUsage::s_handler = NULL;
    ClientWeeklySpellUsageUpdate::s_handler = NULL;
    ClientClearCooldowns::s_handler = NULL;
    s_itemCooldowns.InternalClear(0);

    for (uint32_t i = 0; i < sizeof(s_spellHistory) / sizeof(s_spellHistory[0]); i++) {
        s_spellHistory[i].ClearAll();
    }

    s_cursorWorldObject->Release();
    delete s_cursorWorldObject;
    s_cursorWorldObject = NULL;
    s_periodicClientTriggers.Clear();
    s_destLocSpellCasts.InternalClear(0);
    s_enableErrorSpeechCVar = NULL;
    s_weeklySpellUses.Clear();
}

// OFFSET: 0x008B4C00/0x1405FBFA0
int32_t Spell_C_PrepareMissileTrajectoryCast(CGUnit_C* caster, PendingSpellCast* cast, const SpellRec* spell, const TSStackArray<CGUnit_C*>* targets, float* pitch, float* speed) {
    if (caster->IsUpdatingMissileTrajectory()) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_SPELL_IN_PROGRESS, -1, -1, false);
        return 0;
    }

    int32_t instant = Spell_C_GetCastTime(spell, caster->m_unit->m_unitFieldPetnumber != 0, 0, 1) <= 0;
    SpellVisualRec visualRec;
    const SpellVisualRec* visual = caster->GetAppropriateSpellVisual(spell, visualRec, 0, 0);
    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual ? visual->m_castKit : 0);
    int32_t playNow = 0;

    if (caster->m_castSpellId || cast->m_trajectoryLaunched || !instant || !visual || !visual->m_hasMissile) {
        instant = 0;
    } else {
        instant = 1;
        playNow = kit && (kit->m_animID > 0 || kit->m_animKitID > 0) && !(visual->m_flags & 0x4);
        cast->m_data.m_castFlags |= 0x20000;
        cast->m_trajectoryLaunched = 1;
    }

    if (playNow) {
        if (s_delayedTrajectoryCast || (kit && kit->m_animID >= 0 && caster->m_specialIdleAnim == kit->m_animID)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_SPELL_IN_PROGRESS, -1, -1, false);
            return 0;
        }

        cast->m_data.m_targetFlags |= 0x60;
        cast->m_data.m_sourcePos = C3Vector(0.0f, 0.0f, 0.0f);
        cast->m_data.m_destPos = C3Vector(0.0f, 0.0f, 0.0f);
        cast->m_data.m_elevation = 0.0f;
        cast->m_data.m_speed = 1.0f;

        if (visual->m_flags & 0x20) {
            caster->AttachPrecastMissileModel(visual, cast->m_data.m_castCount);
        }

        Spell_C_PlayMissileTrajectoryCastVisuals(caster, cast, spell, visual, kit, targets, 1.0f);
        s_delayedTrajectoryCast = cast;
        return 0;
    }

    C3Vector impact(0.0f, 0.0f, 0.0f);
    C3Vector launch(0.0f, 0.0f, 0.0f);
    float time;

    if (!caster->ComputeMissileTrajectory(spell, pitch, speed, &launch, &impact, &time, NULL, NULL, 0)) {
        ConsoleWrite("Could not compute missile trajectory", DEFAULT_COLOR);
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ERROR, -1, -1, false);
        return 0;
    }

    float bytes;
    uint32_t latency;
    ClientServices::Connection()->GetNetStats(CONNECTION_INSTANCE, bytes, bytes, latency);
    float limit = time * 1.3f;
    float arrival = latency * 0.001f * 0.5f + time + 0.2f;

    if (arrival > limit) {
        arrival = limit;
    }

    cast->m_data.m_targetFlags |= 0x60;
    cast->m_data.m_sourcePos = launch;
    cast->m_data.m_destPos = impact;
    cast->m_data.m_elevation = *pitch;
    cast->m_data.m_speed = *speed;

    if (instant) {
        Spell_C_PlayMissileTrajectoryCastVisuals(caster, cast, spell, visual, kit, targets, arrival);
        caster->ReleaseMissiles();
        return 1;
    }

    if (cast->m_trajectoryLaunched) {
        caster->CorrectTrajectoryOfPendingMissiles(cast->m_data.m_castCount, impact, *pitch, *speed, arrival);
    }

    return 1;
}

// OFFSET: 0x008B5100/0x1405FC450
void Spell_C_GetMinMaxRange(CGUnit_C* caster, int32_t spellId, float* minRange, float* maxRange, CGObject_C* target) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        Spell_C_GetMinMaxRange(caster, spell, minRange, maxRange, target);
        return;
    }

    *minRange = 0.0f;
    *maxRange = 0.0f;
}

// OFFSET: 0x008B5160/0x1405FC4A0
void Spell_C_SendPendingCast(PendingSpellCast* cast, bool hideVisuals) {
    WOWGUID castItem = cast->m_data.m_castItem == cast->m_data.m_caster ? WOWGUID::Make(0) : cast->m_data.m_castItem;
    s_cursorWorldObject->Release();
    CursorSetResetMode(1);
    CursorSetMode(1);
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster || !cast->m_data.m_castCount) {
        ReleasePendingSpellCast(cast);
        return;
    }

    if (!castItem) {
        CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_objectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));
        CGSpellBook::GetAppropriateSpellRank(reinterpret_cast<int32_t&>(cast->m_data.m_spellId), caster, target);
    }

    SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);
    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
    bool casterIncluded = false;
    TSStackArray<CGUnit_C*> targets(static_cast<CGUnit_C**>(alloca(sizeof(CGUnit_C*) * 2)), 2, 0);

    for (uint32_t i = 0; i < effectCount; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && (effect->m_implicitTargetA == 1 || effect->m_implicitTargetB == 1)) {
            targets.m_count = 1;
            targets.m_data[0] = caster;
            casterIncluded = true;
            break;
        }
    }

    if (cast->m_data.m_objectTarget) {
        targets.m_data[targets.m_count++] = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_objectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    int32_t reason;

    if (!Spell_C_TargetsAuraStateValid(caster, spell, targets.m_count, targets.m_data, &reason, casterIncluded) || (cast->m_data.m_itemTarget && !(cast->m_data.m_targetFlags & 0x1000) && !Spell_C_CheckEnchantItemLevel(cast, spell, cast->m_data.m_itemTarget, &reason))) {
        Spell_C_SpellFailed(cast, spell, reason, -1, -1, false);
        return;
    }

    if (!(spell->m_attributes & 0x8000000) && caster->GetClientStandState()) {
        if (!caster->CanAutoStand()) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_STANDING, -1, -1, false);
            return;
        }

        caster->ChangeStandState(0);
    }

    if (!(spell->m_attributes & 0x1000000) && static_cast<int32_t>(caster->m_unit->m_unitFieldMountdisplayid) > 0 && !caster->MountedCombatAllowed() && caster->m_mountDisplayId > 0 && !(caster->m_clientFlags & 0x10000000)) {
        if (!caster->CanAutoDismount()) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_MOUNTED, -1, -1, false);
            return;
        }

        caster->Dismount();
    }

    if (caster->HasVehicleTransport() && !(spell->m_attributesExF & 0x1000)) {
        caster->CheckVehicleExitOnCast();
    }

    if (!caster->CheckShapeshiftRules(spell, &reason)) {
        if (reason == SPELL_FAILED_ONLY_SHAPESHIFT || !caster->CanAutoUnshift()) {
            Spell_C_SpellFailed(cast, spell, reason, -1, -1, false);
            return;
        }

        caster->CancelShapeshift();
    }

    if (SpellHasAuraEffect(spell, 78) && !caster->CanCurrentFormMount() && caster->CanAutoUnshift()) {
        caster->CancelShapeshiftForMount();
    }

    if ((caster->m_movement->m_flags & 0x1000000) && ((static_cast<int32_t>(caster->m_unit->m_unitFieldMountdisplayid) > 0 && (caster->m_mountDisplayId <= 0 || (caster->m_clientFlags & 0x10000000))) || (caster->m_shapeshiftSuppressed && caster->HasFlightFormAura())) && !caster->HasAuraMatchingSpellClass(330, spell)) {
        const SpellInterruptsRec* interrupts;

        if (spell->m_spellInterruptsID && (interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID))) {
            if (spell->m_attributesEx & 0x44) {
                if (interrupts->m_channelInterruptFlags & 0x18) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_MOVING, -1, -1, false);
                    return;
                }

                if (interrupts->m_channelInterruptFlags2 & 0x1) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_FALLING, -1, -1, false);
                    return;
                }
            }

            if ((interrupts->m_interruptFlags & 0x1) && !(spell->m_attributesExB & 0x20)) {
                if (Spell_C_GetCastTime(spell, caster->m_unit->m_unitFieldPetnumber != 0, 0, 0) || (interrupts->m_auraInterruptFlags & 0x18)) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_MOVING, -1, -1, false);
                    return;
                }

                if (interrupts->m_auraInterruptFlags2 & 0x1) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_FALLING, -1, -1, false);
                    return;
                }
            }
        }
    }

    int32_t spellId = cast->m_baseSpellId ? cast->m_baseSpellId : cast->m_data.m_spellId;
    CDataStore msg;
    uint8_t castFlags = 0;

    if (castItem) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(castItem, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item) {
            ConsoleWrite("Casting item not found", DEFAULT_COLOR);
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ERROR, -1, -1, false);
            return;
        }

        CGObject_C* container = ClntObjMgrObjectPtr(item->m_item->m_itemFieldContained, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!container) {
            ConsoleWrite("Casting item's container not found", DEFAULT_COLOR);
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ERROR, -1, -1, false);
            return;
        }

        int32_t slot = container->GetBag()->FindItemSlot(item->m_obj->m_guid);

        if (slot < 0) {
            ConsoleWrite("Casting item not found in container", DEFAULT_COLOR);
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ERROR, -1, -1, false);
            return;
        }

        CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!owner) {
            ConsoleWrite("Active player not found", DEFAULT_COLOR);
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_ERROR, -1, -1, false);
            return;
        }

        uint8_t bag = owner->FindSlotIndex(item->m_item->m_itemFieldContained);

        if (item->GetInventoryType() && (bag != 0xFF || slot > 18)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_EQUIPPED_ITEM, -1, -1, false);
            return;
        }

        msg.Put(static_cast<uint32_t>(CMSG_USE_ITEM));
        msg.Put(bag);
        msg.Put(static_cast<uint8_t>(slot));
        msg.Put(cast->m_data.m_castCount);
        msg.Put(static_cast<uint32_t>(spellId));
        msg.Put(castItem.m_guid);
        msg.Put(cast->m_data.m_glyphIndex);
    } else if (caster->IsActivePlayer() && !(spell->m_attributesExF & 0x40000)) {
        if (!s_castSpellThrottle.IncrementAndCheck()) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_READY, -1, -1, false);
            return;
        }

        msg.Put(static_cast<uint32_t>(CMSG_CAST_SPELL));
        msg.Put(cast->m_data.m_castCount);
        msg.Put(static_cast<uint32_t>(spellId));
        msg.Put(cast->m_data.m_glyphIndex);
    } else {
        msg.Put(static_cast<uint32_t>(CMSG_PET_CAST_SPELL));
        msg.Put(cast->m_petGuid.m_guid);
        msg.Put(cast->m_data.m_castCount);
        msg.Put(static_cast<uint32_t>(spellId));
    }

    SpellMissileRec* missile = g_spellMissileDB.GetRecord(spell->m_spellMissileID);
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (missile && (missile->m_flags & 0x1)) {
        if (mover != caster) {
            ConsoleWrite("Missile trajectory does not have active mover caster", DEFAULT_COLOR);
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_DONT_REPORT, -1, -1, false);
            return;
        }

        castFlags = 2;
        CGUnit_C::SetMissileTrajectoryTargetingSpell(spell->m_id);
    } else if (spell->m_researchProjectID) {
        int32_t failed;

        if (!CGResearchFrame::CanCastSpell(spell->m_id, failed)) {
            Spell_C_SpellFailed(cast, spell, failed, -1, -1, false);
            return;
        }

        castFlags = 8;
    }

    msg.Put(castFlags);

    if (castFlags & 0x2) {
        float pitch;
        float speed;

        if (!Spell_C_PrepareMissileTrajectoryCast(caster, cast, spell, &targets, &pitch, &speed)) {
            return;
        }

        Spell_C_EncodeSequenceIntoMissileTrajectory(reinterpret_cast<uint32_t*>(&pitch), reinterpret_cast<uint32_t*>(&speed), reinterpret_cast<uint32_t*>(&cast->m_data.m_sourcePos), reinterpret_cast<uint32_t*>(&cast->m_data.m_destPos));
        SpellPutCastTargets(&cast->m_data, &msg);
        msg.Put(pitch);
        msg.Put(speed);
        WriteMovementPacket(mover, &msg);
        caster->PreStartUpdatingMissileTrajectory(cast->m_data.m_spellId, cast->m_data.m_castCount, FrameTime::GetCurTimeMs());
    } else {
        SpellPutCastTargets(&cast->m_data, &msg);

        if (castFlags & 0x8) {
            CGResearchFrame::PutWeightForSpell(&msg);
        }
    }

    if (cast == s_delayedTrajectoryCast) {
        s_delayedTrajectoryCast = NULL;
    }

    if (mover && mover == caster && (spell->m_attributesExG & 0x80)) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player && player->m_vehiclePassenger) {
            player->m_vehiclePassenger->StopMovement();
        }

        mover->OnMoveStopLocal(FrameTime::GetCurTimeMs());
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    s_targetingSpell = NULL;
    CGInputControl* input = CGInputControl::GetActive();
    input->m_mouseModeFlags &= ~0x10u;
    input->UpdateMouseMode(0);

    if (caster->IsActivePlayer()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_SPELLCAST_SENT, "%s%s%s%s%d", "player", static_cast<const char*>(spell->m_name), static_cast<const char*>(spell->m_nameSubtext), cast->m_data.m_objectTarget ? GetCombatLogObjectName(cast->m_data.m_objectTarget) : "", cast->m_data.m_castCount);
    }

    if (!hideVisuals) {
        StartSpellCastVisuals(caster, cast, spell, 0, true);
    }

    Spell_C_StartSpellCooldown(spell, caster, cast->m_data.m_castCount, cast->m_cooldownDelay);
}

// OFFSET: 0x008B5B10/0x1405FCF60
bool Spell_C_IsUsableAction(CGUnit_C* unit, const SpellRec* spell, const WOWGUID& target, const ItemRecSparse* item, bool ignoreControl, bool ignorePetControl, bool ignoreCost) {
    if (!unit) {
        return false;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect && effect->m_effect == 47) {
        return true;
    }

    if (unit->IsActivePlayer()) {
        bool attack = (effect && effect->m_effect == 78) || (spell->m_attributes & 0x2);
        uint32_t flags = static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags;

        if (flags & 0x1000000) {
            if (attack || Spell_C_IsRestrictedAbility(spell) || (effect && SpellIsWeaponDamageEffect(effect->m_effect))) {
                return false;
            }
        } else if (!attack && (flags & 0x800000)) {
            return false;
        }
    }

    if (unit->IsDeadOrGhost() && !(spell->m_attributes & 0x800000)) {
        return false;
    }

    int32_t categoryRecoveryTime = 0;
    int32_t recoveryTime = 0;
    const SpellCooldownsRec* cooldowns;

    if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID))) {
        categoryRecoveryTime = cooldowns->m_categoryRecoveryTime;
        recoveryTime = cooldowns->m_recoveryTime;
    }

    int32_t category = 0;
    const SpellCategoriesRec* categories;

    if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
        category = categories->m_category;
    }

    Spell_C_ModifyCategoryRecovery(categoryRecoveryTime, category, 1);
    unit->SpellHasteCooldowns(spell, recoveryTime, categoryRecoveryTime);
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map) {
        if (map->m_instanceType == 4 && !(spell->m_attributesExD & 0x20000)) {
            if (recoveryTime > 900000 || categoryRecoveryTime > 900000 || (spell->m_attributesExD & 0x10000) || (spell->m_attributesExI & 0x200)) {
                return false;
            }
        } else if (map->m_instanceType == 3) {
            if (spell->m_attributesExH & 0x10000000) {
                return false;
            }

            if (!CGUnit_C::CheckSpellPvPUsability(spell, NULL) && !(spell->m_attributesExI & 0x4000) && (recoveryTime > 900000 || categoryRecoveryTime > 900000 || (spell->m_attributesExD & 0x10000))) {
                return false;
            }
        } else {
            AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);
            World_PVP_AreaRec* pvpArea;

            if (area && area->m_worldPvpID && (pvpArea = g_world_PVP_AreaDB.GetRecord(area->m_worldPvpID)) && static_cast<int32_t>(GetWorldState(pvpArea->m_gameTimeWorldState)) > 0 && (spell->m_attributesExH & 0x10000000)) {
                return false;
            }
        }

        if (map->m_instanceType != 4 && (spell->m_attributesExF & 0x2)) {
            return false;
        }
    }

    if (!CGGameUI::m_hasControl && !ignoreControl) {
        int32_t allowed = 0;

        if ((unit->m_unit->m_unitFieldFlags & 0x800000) && ((spell->m_attributesExE & 0x20000) || Spell_C_CancelsAuraEffect(unit, spell, 7, NULL))) {
            allowed = 1;
        }

        if ((unit->m_unit->m_unitFieldFlags & 0x400000) && ((spell->m_attributesExE & 0x40000) || Spell_C_CancelsAuraEffect(unit, spell, 5, NULL))) {
            allowed = 1;
        }

        if (unit->IsCharmed() && ((spell->m_attributesExH & 0x20) || Spell_C_CancelsCharm(unit, spell, NULL))) {
            allowed = 1;
        }

        if ((!(unit->m_unit->m_unitFieldFlags & 0x100000) || !(spell->m_attributes & 0x1000000)) && !allowed) {
            return false;
        }
    }

    if (!ignoreCost && (!item || !(item->m_flags[0] & 0x10000000)) && !Spell_C_HaveSpellTokens(unit, spell, false, NULL)) {
        return false;
    }

    if (!Spell_C_HaveEquippedSpellItems(unit, spell, false, NULL)) {
        return false;
    }

    if (spell->m_attributesEx & 0x500000) {
        if (unit->IsActivePlayer() && !CGGameUI::GetComboPoints(target, (spell->m_attributesExH & 0x80000) != 0) && !unit->HasIgnoreAuraState(spell)) {
            return false;
        }

        if (unit->m_obj->m_guid == CGPetInfo::GetPet(0) && !CGPetInfo::GetComboPoints(target, (spell->m_attributesExH & 0x80000) != 0)) {
            return false;
        }
    }

    int32_t reason;

    if (!unit->CheckShapeshiftRules(spell, &reason) && (reason == SPELL_FAILED_ONLY_SHAPESHIFT || !unit->CanAutoUnshift())) {
        return false;
    }

    if (!unit->CheckStealthRules(spell, &reason) || !unit->CheckMountRules(spell, reinterpret_cast<SpellCastResult*>(&reason)) || ((spell->m_attributes & 0x10000000) && (unit->m_unit->m_unitFieldFlags & 0x80000) && !unit->HasIgnoreAuraState(spell))) {
        return false;
    }

    CGUnit_C* targetUnit = Spell_C_GetImplicitSpellTarget(unit, spell, target);

    if (!Spell_C_AuraStateValid(unit, spell, targetUnit)) {
        return false;
    }

    if (targetUnit && spell->m_spellAuraRestrictionsID) {
        SpellAuraRestrictionsRec* restrictions = g_spellAuraRestrictionsDB.GetRecord(spell->m_spellAuraRestrictionsID);

        if (restrictions) {
            int32_t targetAura = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_targetAuraSpell);
            int32_t excludeTargetAura = Spell_C_SpellGetSpellIDForDifficulty(restrictions->m_excludeTargetAuraSpell);

            if ((restrictions->m_targetAuraState || restrictions->m_excludeTargetAuraState || targetAura || excludeTargetAura) && !Spell_C_IsValidTarget(unit, spell, targetUnit)) {
                return false;
            }
        }
    }

    if ((spell->m_attributes & 0x2000000) && Spell_C_IsActionOnHold(&spell->m_id, 0)) {
        return false;
    }

    if (unit->m_obj->m_guid == CGPetInfo::GetPet(0) && !CGPetInfo::PetActionsUsable(false, ignorePetControl)) {
        return false;
    }

    const SpellInterruptsRec* interrupts;

    if (spell->m_spellInterruptsID && (interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID))) {
        if (((interrupts->m_auraInterruptFlags & 0x100) || ((spell->m_attributesEx & 0x44) && (interrupts->m_channelInterruptFlags & 0x100))) && !(unit->m_clientFlags & 0x200000)) {
            return false;
        }

        if (((interrupts->m_auraInterruptFlags & 0x80) || ((spell->m_attributesEx & 0x44) && (interrupts->m_channelInterruptFlags & 0x80))) && (unit->m_clientFlags & 0x200000)) {
            return false;
        }

        if (((interrupts->m_auraInterruptFlags2 & 0x2) || ((spell->m_attributesEx & 0x44) && (interrupts->m_channelInterruptFlags2 & 0x2))) && (unit->m_movement->m_flags & 0x100000)) {
            return false;
        }
    }

    if (spell->m_attributes & 0xC000) {
        if (unit->IsOutdoors()) {
            if (spell->m_attributes & 0x4000) {
                return false;
            }
        } else if (spell->m_attributes & 0x8000) {
            return false;
        }
    }

    if ((spell->m_attributesExD & 0x4000000) && !unit->FlyingMountAllowed(true)) {
        return false;
    }

    if ((spell->m_attributesExC & 0x800) && map && map->m_instanceType != 3) {
        return false;
    }

    if ((spell->m_attributesExF & 0x800) && map && map->m_instanceType == 2) {
        return false;
    }

    if ((spell->m_attributesExD & 0x40000000) && map && !map->m_instanceType) {
        return false;
    }

    return true;
}

// OFFSET: 0x008B61A0
bool RangeCheck(CGUnit_C* caster, int32_t spellId, CGObject_C* target, uint8_t* tooFar) {
    if (tooFar) {
        *tooFar = 0;
    }

    float minRange;
    float maxRange;
    Spell_C_GetMinMaxRange(caster, spellId, &minRange, &maxRange, target);
    C3Vector targetPos = target->GetPosition();
    C3Vector casterPos = caster->GetPosition();
    float dx = casterPos.x - targetPos.x;
    float dy = casterPos.y - targetPos.y;
    float dz = casterPos.z - targetPos.z;
    float distance = dx * dx + dz * dz + dy * dy;

    if ((caster->m_obj->m_type & TYPEMASK_UNIT) && !(caster->m_obj->m_type & TYPEMASK_PLAYER) && (target->m_obj->m_type & TYPEMASK_PLAYER) && (caster->m_unit->m_unitFieldFlags2 & 0x8000000)) {
        minRange = 0.0f;
    }

    if (minRange * minRange > distance) {
        return false;
    }

    if (distance > maxRange * maxRange) {
        if (tooFar) {
            *tooFar = 1;
        }

        return false;
    }

    return true;
}

// OFFSET: 0x008B62D0/0x1405FD880
bool Spell_C_RangeCheckSelected(CGUnit_C* caster, const SpellRec* spell, WOWGUID target, uint8_t* inRange, bool report, PendingSpellCast* cast) {
    if (!target) {
        target = CGGameUI::m_lockedTarget;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return false;
    }

    uint32_t needTargets = 0;
    int32_t targetCreatureType = 0;
    const SpellTargetRestrictionsRec* restrictions;

    if (spell->m_spellTargetRestrictionsID && (restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID))) {
        needTargets = static_cast<uint16_t>(restrictions->m_targets);
        targetCreatureType = restrictions->m_targetCreatureType;
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
    bool pet = false;

    if (effectCount) {
        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect) {
                continue;
            }

            switch (effect->m_implicitTargetA) {
                case 1:
                    if (needTargets & 0x400) {
                        needTargets &= 0xFBFF;
                    }

                    break;

                case 5:
                    pet = true;

                    if (needTargets & 0x8000) {
                        needTargets &= 0x7FFF;
                    }

                    break;

                case 6:
                case 53:
                    needTargets |= 0x80;
                    break;

                case 21:
                case 45:
                case 118:
                    needTargets |= 0x100;
                    break;

                case 23:
                    needTargets |= 0x800;
                    break;

                case 25:
                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                case 69:
                case 70:
                case 71:
                case 74:
                case 75:
                    needTargets |= 0x2;
                    break;

                case 26:
                    needTargets |= 0x4000;
                    break;

                case 35:
                    needTargets |= 0x8;
                    break;

                case 57:
                case 61:
                    needTargets |= 0x4;
                    break;
            }
        }

        if (pet && !(needTargets & 0x2)) {
            object = ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (!object) {
                return false;
            }

            needTargets |= 0x2;
        }
    }

    uint32_t type = object->m_obj->m_type;
    uint8_t flag = 0;

    if (type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);

        if (spell->m_attributesExE & 0x800) {
            unit = Spell_C_TargetImpliedTarget(unit, spell, needTargets, caster);

            if (!unit) {
                return false;
            }

            object = unit;
        }

        if ((unit->m_unit->m_unitFieldFlags & 0x10000) && !(spell->m_attributesExF & 0x1000000)) {
            return false;
        }

        if (targetCreatureType && (!unit->GetCreatureType() || !((1 << (unit->GetCreatureType() - 1)) & targetCreatureType))) {
            return false;
        }

        CGCorpse_C* corpse = NULL;

        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x10) && (spell->m_attributesExI & 0x8000000)) {
            corpse = ClntObjMgrFindCorpseByOwner(unit->m_obj->m_guid);
        }

        if (corpse) {
            object = corpse;
        } else {
            if (unit->m_unit->m_unitFieldHealth > 0) {
                if (needTargets & 0x400) {
                    return false;
                }
            } else if (!(unit->m_unit->m_unitDynamicFlags & 0x20) && !(needTargets & 0x8600) && !(spell->m_attributesExB & 0x1)) {
                return false;
            }

            bool valid = ((needTargets & 0x8) && caster->IsInMyPartyOrRaid(unit) && caster->CanAssist(unit, false)) || ((needTargets & 0x4) && caster->IsInMyParty(unit) && caster->CanAssist(unit, false)) || ((needTargets & 0x100) && caster->CanAssist(unit, (spell->m_attributesExF & 0x8) != 0)) || ((needTargets & 0x80) && caster->CanAttack(unit, spell->m_attributesExH < 0)) || ((needTargets & 0x2) && !(unit->m_unit->m_unitFieldFlags & 0x100));

            if (!valid) {
                if ((needTargets & 0x8000) && unit->m_unit->m_unitFieldHealth <= 0 && ((unit->m_obj->m_type & TYPEMASK_PLAYER) || unit->m_unit->m_unitFieldPetnumber)) {
                    if (!caster->CanAssist(unit, false)) {
                        return false;
                    }
                } else if (!((needTargets & 0x200) && unit->m_unit->m_unitFieldHealth <= 0 && (unit->m_obj->m_type & TYPEMASK_PLAYER) && !caster->CanAssist(unit, false))) {
                    return false;
                }
            }
        }
    } else if (type & TYPEMASK_GAMEOBJECT) {
        if (!(needTargets & 0x4800)) {
            return false;
        }
    } else if (type & TYPEMASK_CORPSE) {
        const CGCorpse_C* corpse = static_cast<const CGCorpse_C*>(object);

        if (((needTargets & 0x8000) || (spell->m_attributesExI & 0x8000000)) && !(corpse->m_corpse->m_corpseFieldFlags & 0x1) && caster->CanAssist(corpse)) {
            flag = 1;
        }

        if (!((needTargets & 0x200) && !(corpse->m_corpse->m_corpseFieldFlags & 0x1) && !caster->CanAssist(corpse)) && !flag) {
            return false;
        }
    } else {
        return false;
    }

    bool result = RangeCheck(caster, spell->m_id, object, &flag);
    *inRange = result;

    if (!result && report) {
        if (flag) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_OUT_OF_RANGE, -1, -1, false);
        } else {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_TOO_CLOSE, -1, -1, false);
        }
    }

    return true;
}

// OFFSET: 0x008B67F0/0x1405FDEA0
bool Spell_C_HandleSpriteClick(CGObject_C* object, PendingSpellCast* cast) {
    if (!cast) {
        cast = s_targetingSpell;
    }

    if (!object || !cast) {
        return false;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);

    if (!spell) {
        return false;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
    uint32_t needTargets = s_needTargets;

    if (!effect || effect->m_effect != 83) {
        uint32_t action = (effect && effect->m_effect == 47) || (spell->m_attributes & 0x20) ? 6 : 2;

        if (!CGGameUI::CanPerformAction(action)) {
            return false;
        }
    }

    bool result = false;
    uint32_t type = object->m_obj->m_type;

    if (type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);

        if (spell->m_attributesExE & 0x800) {
            unit = Spell_C_TargetImpliedTarget(unit, spell, s_needTargets, caster);

            if (!unit) {
                return false;
            }

            object = unit;

            if (s_needTargets & 0x10C) {
                s_needTargets &= ~0x80u;
            } else if (s_needTargets & 0x80) {
                s_needTargets &= ~0x100u;
            }
        }

        if (unit->m_obj->m_guid == cast->m_data.m_caster && (spell->m_attributesEx & 0x80000)) {
            return false;
        }

        if ((unit->m_unit->m_unitFieldFlags & 0x10000) && !(spell->m_attributesExF & 0x1000000)) {
            return false;
        }

        if (unit->m_unit->m_unitFieldFlags2 & 0x4000000) {
            return false;
        }

        const SpellTargetRestrictionsRec* restrictions;

        if (spell->m_spellTargetRestrictionsID && (restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID)) && restrictions->m_targetCreatureType && (!unit->GetCreatureType() || !((1 << (unit->GetCreatureType() - 1)) & restrictions->m_targetCreatureType))) {
            return false;
        }

        CGCorpse_C* corpse = NULL;

        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x10) && (spell->m_attributesExI & 0x8000000)) {
            corpse = ClntObjMgrFindCorpseByOwner(unit->m_obj->m_guid);
        }

        if (corpse) {
            object = corpse;
            result = true;
        } else {
            int32_t health = unit->m_unit->m_unitFieldHealth;

            if (health <= 0 && !(unit->m_unit->m_unitDynamicFlags & 0x20) && !(s_needTargets & 0x8600) && !(spell->m_attributesExB & 0x1)) {
                return false;
            }

            if (health > 0 && (s_needTargets & 0x400)) {
                return false;
            }

            if ((s_needTargets & 0x8) && caster->IsInMyPartyOrRaid(unit) && caster->CanAssist(unit, false)) {
                cast->m_data.m_targetFlags |= 0x2;
                s_needTargets &= ~0x8u;
                cast->m_data.m_objectTarget = object->m_obj->m_guid;

                if (unit->m_unit->m_unitFieldHealth <= 0) {
                    s_needTargets &= ~0x400u;
                }

                result = true;
            } else if ((s_needTargets & 0x4) && caster->IsInMyParty(unit) && caster->CanAssist(unit, false)) {
                cast->m_data.m_targetFlags |= 0x2;
                s_needTargets &= ~0x4u;
                cast->m_data.m_objectTarget = object->m_obj->m_guid;

                if (unit->m_unit->m_unitFieldHealth <= 0) {
                    s_needTargets &= ~0x400u;
                }

                result = true;
            } else if ((s_needTargets & 0x100) && caster->CanAssist(unit, (spell->m_attributesExF & 0x8) != 0)) {
                cast->m_data.m_targetFlags |= 0x2;
                s_needTargets &= ~0x100u;
                cast->m_data.m_objectTarget = object->m_obj->m_guid;

                if (unit->m_unit->m_unitFieldHealth <= 0) {
                    s_needTargets &= ~0x400u;
                }

                result = true;
            } else if ((s_needTargets & 0x80) && caster->CanAttack(unit, spell->m_attributesExH < 0)) {
                cast->m_data.m_targetFlags |= 0x2;
                s_needTargets &= ~0x80u;
                cast->m_data.m_objectTarget = object->m_obj->m_guid;

                if (unit->m_unit->m_unitFieldHealth <= 0) {
                    s_needTargets &= ~0x400u;
                }

                result = true;
            } else if ((s_needTargets & 0x2) && (!(unit->m_unit->m_unitFieldFlags & 0x100) || (spell->m_attributesExC & 0x10000000) || (spell->m_attributesExE & 0x200000))) {
                cast->m_data.m_objectTarget = object->m_obj->m_guid;
                cast->m_data.m_targetFlags |= 0x2;
                uint32_t remaining = s_needTargets & ~0x2u;
                s_needTargets = remaining;

                if (unit->m_unit->m_unitFieldHealth <= 0) {
                    s_needTargets = remaining & 0xFFFF79FF;
                }

                result = true;
            } else if ((s_needTargets & 0x8000) && unit->m_unit->m_unitFieldHealth <= 0 && ((unit->m_obj->m_type & TYPEMASK_PLAYER) || unit->m_unit->m_unitFieldPetnumber)) {
                if (!caster->CanAssist(unit, false)) {
                    return result;
                }

                cast->m_data.m_objectTarget = object->m_obj->m_guid;
                cast->m_data.m_targetFlags |= 0x2;
                s_needTargets &= ~0x8000u;
                result = true;
            } else if ((s_needTargets & 0x200) && unit->m_unit->m_unitFieldHealth <= 0 && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
                if (caster->CanAssist(unit, false)) {
                    return result;
                }

                cast->m_data.m_objectTarget = object->m_obj->m_guid;
                cast->m_data.m_targetFlags |= 0x2;
                s_needTargets &= ~0x200u;
                result = true;
            } else if (s_needTargets & 0x10000) {
                if (unit->GetCreatureType() != 12) {
                    return result;
                }

                cast->m_data.m_objectTarget = object->m_obj->m_guid;
                cast->m_data.m_targetFlags |= 0x10000;
                s_needTargets &= ~0x10000u;
                result = true;
            } else if ((s_needTargets & 0x100) && (spell->m_attributesExF & 0x200) && unit->IsCharmed() && !caster->CanAttack(unit, false)) {
                cast->m_data.m_objectTarget = object->m_obj->m_guid;
                cast->m_data.m_targetFlags |= 0x100;
                s_needTargets &= ~0x100u;
                result = true;
            } else {
                return result;
            }
        }
    } else if (type & TYPEMASK_ITEM) {
        if (!(s_needTargets & 0x4010)) {
            return result;
        }

        cast->m_data.m_itemTarget = object->m_obj->m_guid;
        cast->m_data.m_targetFlags |= 0x10;
        s_needTargets &= 0xFFFFBFEF;
        result = true;
    } else if (type & TYPEMASK_GAMEOBJECT) {
        if (!(s_needTargets & 0x4800)) {
            return result;
        }

        cast->m_data.m_objectTarget = object->m_obj->m_guid;
        cast->m_data.m_targetFlags |= 0x800;
        s_needTargets &= 0xFFFFB7FF;
        result = true;
    } else if (type & TYPEMASK_CORPSE) {
        CGCorpse_C* corpse = static_cast<CGCorpse_C*>(object);

        if (((s_needTargets & 0x8000) || (spell->m_attributesExI & 0x8000000)) && !(corpse->m_corpse->m_corpseFieldFlags & 0x1) && caster->CanAssist(corpse)) {
            bool assigned = true;

            if ((s_needTargets & 0x8) && caster->IsActivePlayer() && CGGameUI::IsInParty(corpse->m_corpse->m_corpseFieldOwner)) {
                s_needTargets &= ~0x8u;
            } else if ((s_needTargets & 0x4) && caster->IsActivePlayer() && CGGameUI::IsPartyMember(corpse->m_corpse->m_corpseFieldOwner)) {
                s_needTargets &= ~0x4u;
            } else if (s_needTargets & 0x8000) {
                s_needTargets &= ~0x8000u;
            } else {
                assigned = false;
            }

            if (assigned) {
                cast->m_data.m_objectTarget = object->m_obj->m_guid;
                cast->m_data.m_targetFlags |= 0x8000;
                result = true;
            }
        }

        if ((s_needTargets & 0x200) && !(corpse->m_corpse->m_corpseFieldFlags & 0x1) && !caster->CanAssist(corpse)) {
            cast->m_data.m_objectTarget = object->m_obj->m_guid;
            cast->m_data.m_targetFlags |= 0x200;
            s_needTargets &= ~0x200u;
            result = true;
        } else if (!result) {
            return result;
        }
    } else {
        return result;
    }

    if ((cast->m_data.m_targetFlags & 0x18202) && cast->m_data.m_objectTarget == object->m_obj->m_guid) {
        uint8_t tooFar;

        if (!RangeCheck(caster, cast->m_data.m_spellId, object, &tooFar)) {
            Spell_C_SpellFailed(cast, spell, tooFar ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_TOO_CLOSE, -1, -1, false);
            cast->m_data.m_objectTarget = WOWGUID::Make(0);
            s_needTargets = needTargets;
            return true;
        }
    }

    if (result && !s_needTargets) {
        Spell_C_SendPendingCast(cast, false);
    }

    return result;
}

// OFFSET: 0x008B6FD0
bool Spell_C_HandleTerrainClick(const CTerrainClickEvent& evt) {
    PendingSpellCast* spell = s_targetingSpell;

    if (!spell) {
        return false;
    }

    uint32_t needTargets = s_needTargets;

    if (needTargets & 0x20) {
        spell->m_data.m_sourceTarget = evt.m_guid;
        spell->m_data.m_sourcePos = evt.m_point;
        spell->m_data.m_targetFlags |= 0x20;
        needTargets &= ~0x20u;
    } else if (needTargets & 0x40) {
        spell->m_data.m_destTarget = evt.m_guid;
        spell->m_data.m_destPos = evt.m_point;
        spell->m_data.m_targetFlags |= 0x40;
        needTargets &= ~0x40u;
    } else {
        return false;
    }

    s_needTargets = needTargets;

    if (!needTargets) {
        Spell_C_SendPendingCast(spell, false);
    }

    return true;
}

// OFFSET: 0x008B7070/0x1405FE800
bool Spell_C_HandleGlyphClick(int32_t socket) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    PendingSpellCast* cast = s_targetingSpell;

    if (!cast) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);

    if (!spell) {
        return false;
    }

    CGPlayerLocalData* data = static_cast<CGPlayerLocalData*>(player->m_player);

    if (!((1 << socket) & data->m_playerGlyphsEnabled)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_GLYPH_SOCKET_LOCKED, -1, -1, false);
        return false;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    bool found = false;

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 74) {
            continue;
        }

        if (!effect->m_effectMiscValue) {
            found = true;
            break;
        }

        GlyphPropertiesRec* properties = g_glyphPropertiesDB.GetRecord(effect->m_effectMiscValue);
        GlyphSlotRec* slot = g_glyphSlotDB.GetRecord(data->m_playerFieldGlyphSlots1[socket]);

        if (!properties || !slot) {
            continue;
        }

        if (slot->m_type != properties->m_glyphSlotFlags) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_INVALID_GLYPH, -1, -1, false);
            return false;
        }

        for (uint32_t j = 0; j < sizeof(data->m_playerFieldGlyphs1) / sizeof(data->m_playerFieldGlyphs1[0]); j++) {
            if (data->m_playerFieldGlyphs1[j] == static_cast<uint32_t>(effect->m_effectMiscValue)) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_UNIQUE_GLYPH, -1, -1, false);
                return false;
            }
        }

        found = true;
        break;
    }

    if (!found) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_INVALID_GLYPH, -1, -1, false);
        return false;
    }

    cast->m_data.m_glyphIndex = socket;
    bool result = false;

    if (s_needTargets & 0x20000) {
        cast->m_data.m_targetFlags |= 0x20000;
        s_needTargets &= ~0x20000u;
        result = true;

        if (!s_needTargets) {
            Spell_C_SendPendingCast(cast, false);
        }
    }

    return result;
}

// OFFSET: 0x008B7280/0x1405FEA90
bool Spell_C_HandleSpriteRay(const WOWGUID& guid, bool checkRange) {
    PendingSpellCast* targeting = s_targetingSpell;

    if (!targeting) {
        return false;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return false;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targeting->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(targeting->m_data.m_spellId);

    if (!spell) {
        return false;
    }

    uint32_t type = object->m_obj->m_type;

    if (type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);

        if (spell->m_attributesExE & 0x800) {
            unit = Spell_C_TargetImpliedTarget(unit, spell, s_needTargets, caster);

            if (!unit) {
                return false;
            }

            object = unit;
        }

        if (unit->m_obj->m_guid == targeting->m_data.m_caster && (spell->m_attributesEx & 0x80000)) {
            return false;
        }

        CGUnitData* data = unit->m_unit;

        if ((data->m_unitFieldFlags & 0x10000) && !(spell->m_attributesExF & 0x1000000)) {
            return false;
        }

        if (data->m_unitFieldFlags2 & 0x4000000) {
            return false;
        }

        const SpellTargetRestrictionsRec* restrictions;

        if (spell->m_spellTargetRestrictionsID && (restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID)) && restrictions->m_targetCreatureType && (!unit->GetCreatureType() || !((1 << (unit->GetCreatureType() - 1)) & restrictions->m_targetCreatureType))) {
            return false;
        }

        int32_t health = data->m_unitFieldHealth;
        uint32_t needTargets = s_needTargets;

        if (health > 0) {
            if (needTargets & 0x400) {
                return false;
            }
        } else if (!(data->m_unitDynamicFlags & 0x20) && !(needTargets & 0x8600) && !(spell->m_attributesExB & 0x1)) {
            return false;
        }

        bool valid = false;

        if ((s_needTargets & 0x8) && caster->IsInMyPartyOrRaid(unit) && caster->CanAssist(unit, false)) {
            valid = true;
        } else if ((s_needTargets & 0x4) && caster->IsInMyParty(unit) && caster->CanAssist(unit, false)) {
            valid = true;
        } else if ((s_needTargets & 0x100) && caster->CanAssist(unit, (spell->m_attributesExF & 0x8) != 0)) {
            valid = true;
        } else if ((s_needTargets & 0x80) && caster->CanAttack(unit, spell->m_attributesExH < 0)) {
            valid = true;
        } else {
            needTargets = s_needTargets;

            if ((needTargets & 0x2) && (!(data->m_unitFieldFlags & 0x100) || (spell->m_attributesExC & 0x10000000) || (spell->m_attributesExE & 0x200000))) {
                valid = true;
            } else if ((needTargets & 0x8000) && data->m_unitFieldHealth <= 0 && ((unit->m_obj->m_type & TYPEMASK_PLAYER) || data->m_unitFieldPetnumber)) {
                valid = caster->CanAssist(unit, false);
            } else if ((needTargets & 0x200) && data->m_unitFieldHealth <= 0 && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
                if (caster->CanAssist(unit, false)) {
                    return false;
                }

                valid = true;
            } else if ((needTargets & 0x10000) && unit->GetCreatureType() == 12) {
                valid = true;
            } else if ((s_needTargets & 0x100) && (spell->m_attributesExF & 0x200) && unit->IsCharmed()) {
                valid = caster->CanAttack(unit, false);
            }
        }

        if (!valid) {
            return false;
        }
    } else if (type & TYPEMASK_ITEM) {
        if (!(s_needTargets & 0x4010)) {
            return false;
        }
    } else if (type & TYPEMASK_GAMEOBJECT) {
        if (!(s_needTargets & 0x4800) || !static_cast<CGGameObject_C*>(object)->IsValidTargetForSpell(targeting->m_data.m_castItem, targeting->m_data.m_spellId)) {
            return false;
        }
    } else if (type & TYPEMASK_CORPSE) {
        const CGCorpse_C* corpse = static_cast<CGCorpse_C*>(object);
        bool friendly = false;

        if (((s_needTargets & 0x8000) || (spell->m_attributesExI & 0x8000000)) && !(corpse->m_corpse->m_corpseFieldFlags & 1) && caster->CanAssist(corpse)) {
            friendly = true;
        }

        if (!((s_needTargets & 0x200) && !(corpse->m_corpse->m_corpseFieldFlags & 1) && !caster->CanAssist(corpse)) && !friendly) {
            return false;
        }
    } else {
        return false;
    }

    if (!checkRange) {
        return true;
    }

    float distance = caster->GetDistanceToPos(object->GetPosition());
    float minRange;
    float maxRange;
    Spell_C_GetMinMaxRange(caster, targeting->m_data.m_spellId, &minRange, &maxRange, object);
    return distance >= minRange * minRange && maxRange * maxRange >= distance;
}

// OFFSET: 0x008B7740
int32_t Spell_C_HandleTerrainRay(const CTerrainClickEvent& evt, bool checkRange) {
    if (!(s_needTargets & 0x60)) {
        return 1;
    }

    SpellCast* data = s_targetingSpell ? &s_targetingSpell->m_data : NULL;
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(data->m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!caster) {
        return 1;
    }

    if (!checkRange) {
        return 0;
    }

    C3Vector position = evt.m_point;

    if (evt.m_guid) {
        CGObject_C* object = ClntObjMgrObjectPtr(evt.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            position *= object->GetMatrix();
        }
    }

    float distance = caster->GetDistanceToPos(position);
    float minRange;
    float maxRange;
    Spell_C_GetMinMaxRange(caster, data->m_spellId, &minRange, &maxRange, NULL);

    if (distance < minRange * minRange || maxRange * maxRange < distance) {
        return maxRange * maxRange * 16.0f >= distance ? 1 : 2;
    }

    if (data->m_spellId != 6544) {
        return 0;
    }

    float height = evt.m_point.z;
    return height - caster->GetPosition().z > 4.0f ? 1 : 0;
}

// OFFSET: 0x008B78B0/0x1405FF370
int32_t Spell_C_TargetTradeItem() {
    if (!(s_needTargets & 0x4010) || !CGTradeInfo::m_targetTrade.m_items[6].m_itemID) {
        return 0;
    }

    PendingSpellCast* cast = s_targetingSpell;
    cast->m_data.m_targetFlags |= 0x1000;
    s_needTargets &= 0xFFFFBFEF;
    cast->m_data.m_itemTarget.m_guid = 6;

    if (!s_needTargets) {
        Spell_C_SendPendingCast(cast, true);
        Spell_C_CancelSpell(cast, false, false, SPELL_FAILED_DONT_REPORT);
    }

    return 1;
}

// OFFSET: 0x008B7B50/0x1405FF650
bool Spell_C_BeginCast(CGUnit_C* caster, const SpellRec* spell, WOWGUID target, PendingSpellCast* cast, bool confirmed) {
    s_needTargets = 0;
    const SpellTargetRestrictionsRec* restrictions;

    if (spell->m_spellTargetRestrictionsID && (restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID))) {
        s_needTargets = restrictions->m_targets;
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
    int32_t implicitTarget = 0;

    if (effectCount && effects->m_data[0]) {
        implicitTarget = effects->m_data[0]->m_implicitTargetA;
    }

    bool noTarget = false;

    switch (implicitTarget) {
        case 1:
            if (s_needTargets & 0x400) {
                s_needTargets &= ~0x400u;
            }

            break;

        case 5:
            if (s_needTargets & 0x8000) {
                s_needTargets &= ~0x8000u;
            }

            break;

        case 6:
        case 53:
            s_needTargets |= 0x80;
            break;

        case 16:
            noTarget = true;
            break;

        case 21:
        case 45:
        case 118:
            s_needTargets |= 0x100;
            break;

        case 23:
            s_needTargets |= 0x800;
            break;

        case 25:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
        case 74:
        case 75:
            s_needTargets |= 0x2;
            break;

        case 26:
            s_needTargets |= 0x4000;
            break;

        case 35:
            s_needTargets |= 0x8;
            break;

        case 57:
        case 61:
            s_needTargets |= 0x4;
            break;

        case 89: {
            SpellMissileRec* missile = g_spellMissileDB.GetRecord(spell->m_spellMissileID);

            if (missile && (missile->m_flags & 0x1)) {
                s_needTargets &= ~0x40u;
            }

            break;
        }

        case 90:
            s_needTargets |= 0x10000;
            break;

        case 121:
            s_needTargets |= 0x8000;
            break;
    }

    if ((s_needTargets & 0x2000) && s_targetString[0]) {
        SStrPrintf(cast->m_data.m_targetString, sizeof(cast->m_data.m_targetString), "%s", s_targetString);
        s_needTargets &= ~0x2000u;
        s_targetString[0] = 0;
        cast->m_data.m_targetFlags |= 0x2000;
    }

    if (!s_needTargets) {
        Spell_C_SendPendingCast(cast, false);
        return true;
    }

    if (spell->m_attributesExE & 0x800) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));
        CGUnit_C* implied = unit ? Spell_C_TargetImpliedTarget(unit, spell, s_needTargets, caster) : NULL;

        if (implied) {
            if (implied != unit) {
                target = implied->m_obj->m_guid;
            }
        } else if (s_needTargets & 0x10C) {
            s_needTargets |= 0x80;
        } else if (s_needTargets & 0x80) {
            s_needTargets |= 0x100;
        }
    }

    if (noTarget || (static_cast<uint32_t>(target.m_guid) & static_cast<uint32_t>(target.m_guid >> 32)) == 0xFFFFFFFF || target.m_guid == 0xFFFFFFFFFFFFFFFEull) {
        return false;
    }

    if ((caster->m_obj->m_type & TYPEMASK_PLAYER) && (spell->m_attributes & 0x200)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(caster);
        bool found = false;

        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect || effect->m_effect != 54) {
                continue;
            }

            CGItem_C* items[2];
            items[0] = player->m_bag.GetItemPointer(15);
            items[1] = player->m_bag.GetItemPointer(16);
            const SpellEquippedItemsRec* equipped;

            if (spell->m_spellEquippedItemsID && (equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID)) && equipped->m_equippedItemSubclass) {
                for (uint32_t j = 0; j < 2; j++) {
                    CGItem_C* item = items[j];

                    if (item && (equipped->m_equippedItemClass != item->GetClassID() || !((1 << item->GetSubtypeID()) & equipped->m_equippedItemSubclass))) {
                        items[j] = NULL;
                    }
                }
            }

            CGItem_C* mainHand = items[0];
            CGItem_C* offHand = items[1];
            uint32_t mainEnchant = mainHand ? mainHand->GetEnchantmentId(1) : 0;
            uint32_t offEnchant = offHand ? offHand->GetEnchantmentId(1) : 0;
            CGItem_C* chosen = offHand;

            if (mainHand) {
                if (offHand) {
                    if (!mainEnchant) {
                        chosen = mainHand;
                    } else if (offEnchant) {
                        uint32_t misc = effect->m_effectMiscValue;

                        if (mainEnchant == misc) {
                            if (offEnchant == misc) {
                                if (mainHand->GetEnchantmentDuration(1) <= offHand->GetEnchantmentDuration(1)) {
                                    chosen = mainHand;
                                }
                            } else {
                                chosen = mainHand;
                            }
                        } else if (offEnchant != misc) {
                            continue;
                        }
                    }
                } else {
                    chosen = mainHand;
                }
            } else if (!offHand) {
                continue;
            }

            if (chosen->IsBound() && !chosen->IsPermanentlyBoundForTrade() && !confirmed) {
                CGGameUI::SetLastItemSpellTarget(chosen->m_obj->m_guid);
                CGGameUI::SetLastEnchantSpellID(cast->m_data.m_spellId);

                if ((cast->m_data.m_castItem.m_guid & 0xF000000000000000ull) == 0x4000000000000000ull) {
                    CGGameUI::SetLastEnchantItem(cast->m_data.m_castItem);
                } else {
                    CGGameUI::SetLastEnchantItem(WOWGUID::Make(0));
                }

                ReleasePendingSpellCast(cast);
                FrameScript_SignalEvent(SCRIPT_EVENT_END_BOUND_TRADEABLE, "%s", "spellenchant");
                return true;
            }

            target = chosen->m_obj->m_guid;

            if (target) {
                found = true;
                break;
            }
        }

        if (!found) {
            target = WOWGUID::Make(player->m_bag.GetItemGuid(15));
        }
    } else if (!target) {
        target = CGGameUI::m_lockedTarget;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        if (!(target.m_guid & 0xF000000000000000ull) && (target.m_guid & 0xF07FFFFFFFFFFFFFull) && ((s_needTargets & 0x8000) || (spell->m_attributesExI & 0x8000000))) {
            CGCorpse_C* corpse = ClntObjMgrFindCorpseByOwner(target);

            if (corpse) {
                object = corpse;
            }
        }

        if (!object) {
            float minRange;
            float maxRange;
            Spell_C_GetMinMaxRange(caster, spell->m_id, &minRange, &maxRange, NULL);

            if (maxRange > 100.0f) {
                cast->m_data.m_objectTarget = target;
                Spell_C_SendPendingCast(cast, false);
                return true;
            }
        }
    }

    if (Spell_C_HandleSpriteClick(object, cast)) {
        return true;
    }

    if (!g_autoSelfCastCVar->m_intValue) {
        return false;
    }

    if (target && (!object || !(object->m_obj->m_type & TYPEMASK_UNIT) || caster->CanAssist(static_cast<CGUnit_C*>(object), false))) {
        return false;
    }

    if (Spell_C_HandleSpriteClick(ClntObjMgrGetActivePlayerObj(), cast)) {
        return true;
    }

    if (caster->IsActivePlayer()) {
        return false;
    }

    return Spell_C_HandleSpriteClick(caster, cast);
}

// OFFSET: 0x008B81E0/0x1405FFE90
bool Spell_C_CastSpell(CGUnit_C* caster, int32_t spellId, const CGItem_C* item, WOWGUID target, PendingSpellCast* cast, bool confirmed, bool a7, int32_t baseSpell, int32_t ignoreCost) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || (spell->m_attributes & 0x40)) {
        return false;
    }

    CGUnit_C* unit = caster;
    WOWGUID petGuid = caster ? caster->m_obj->m_guid : WOWGUID::Make(0);

    if (!caster) {
        return false;
    }

    if (spell->m_attributesExF & 0x40000) {
        CGUnitData* data = caster->m_unit;
        const WOWGUID* owner = &data->m_unitFieldCharmedby;

        if (!*owner) {
            owner = &data->m_unitFieldCreatedby;
        }

        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*owner, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (!unit || unit->IsVehicleTransitionPreventingAction() || unit->IsAlteredFormTransitionPreventingAction()) {
        return false;
    }

    if (!cast) {
        if (s_numPendingSpellCasts >= 100) {
            return false;
        }

        cast = CreatePendingSpellCast(true);
        cast->m_data.m_spellId = spellId;
        cast->m_data.m_castItem = item ? item->m_obj->m_guid : unit->m_obj->m_guid;
        cast->m_data.m_caster = unit->m_obj->m_guid;
        cast->m_baseSpellId = baseSpell;
    }

    cast->m_petGuid = petGuid;
    s_castingSpellCast = cast;
    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
    int32_t effect = 0;
    bool melee;

    if ((effectCount && effects->m_data[0] && (effect = effects->m_data[0]->m_effect) == 78) || (spell->m_attributes & 0x2) || (spell->m_attributesExB & 0x20)) {
        melee = true;
    } else {
        melee = false;

        if (!item && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            int16_t overrideId = *reinterpret_cast<const int16_t*>(static_cast<CGPlayer_C*>(unit)->m_player->m_playerFieldBytes2);

            if (overrideId) {
                OverrideSpellDataRec* rec = g_overrideSpellDataDB.GetRecord(overrideId);

                if (rec && !(rec->m_flags & 0xC) && !Spell_C_IsOverride(spellId, rec)) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW, -1, -1, false);
                    return false;
                }
            }
        }
    }

    if (!IsTutorialFlagged(5)) {
        s_meleeTutorialActive = 1;
        s_meleeTutorialCount = 0;
    }

    if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && s_meleeTutorialActive && IsTutorialFlagged(5)) {
        if (melee) {
            s_meleeTutorialCount++;
        } else {
            s_meleeTutorialActive = 0;
        }

        if (s_meleeTutorialCount > 10) {
            CGTutorial::TriggerTutorial(59);
        }
    }

    if (unit->HasVehicleTransport()) {
        WOWGUID transport = { unit->GetTransportGUID() };
        CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (vehicle) {
            VehicleSeatRec* seat = vehicle->GetVehicleSeatRec(unit->m_movementData.m_transportSeat);

            if (seat) {
                uint32_t required = 0;

                if (SpellHasNonStanceShapeshiftAuraEffect(spell)) {
                    required = 0x2000;
                }

                if (SpellHasAuraEffect(spell, 78)) {
                    required |= 0x1000;
                }

                if (!required) {
                    required = 0x4000;
                }

                if (!(spell->m_attributesExF & 0x1000) && !(spell->m_attributes & 0x1000000) && (seat->m_flags & required) != required) {
                    Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW, -1, -1, false);
                    return false;
                }

                if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (seat->m_flags & 0x40000800)) {
                    for (uint32_t i = 0; i < effectCount; i++) {
                        const SpellEffectRec* summon = effects->m_data[i];

                        if (!summon) {
                            continue;
                        }

                        for (int32_t j = 0; j < 4; j++) {
                            if (summon->m_effect != j) {
                                continue;
                            }

                            SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(summon->m_effectMiscValueB);

                            if (properties && properties->m_control) {
                                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW, -1, -1, false);
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    if (effect != 83 && effect != 152 && effect != 162 && !CGSpellBook::IsTrackingSpell(spellId) && !SpellIsCallCompanion(spell)) {
        uint32_t action = effect == 47 || (spell->m_attributes & 0x20) ? 6 : 2;

        if (!CGGameUI::CanPerformAction(action)) {
            ReleasePendingSpellCast(cast);
            return false;
        }
    }

    const SpellCooldownsRec* cooldowns;

    if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID)) && cooldowns->m_startRecoveryTime && !CGUIMacros::IsActionAllowed(2)) {
        ReleasePendingSpellCast(cast);
        return false;
    }

    if (effect == 47) {
        uint64_t guid = unit->m_obj->m_guid.m_guid;
        SkillLineRec* skillLine = CGTradeSkillInfo::GetSkillLineRec(spell->m_id, guid);

        if (!skillLine) {
            return false;
        }

        if (skillLine->m_id == 794) {
            CGResearchFrame::m_skillLine = CGResearchFrame::m_skillLine != 794 ? 794 : 0;
            FrameScript_SignalEvent(SCRIPT_EVENT_ARCHAEOLOGY_TOGGLE, NULL);
        } else {
            uint64_t tradeGuid = unit->m_obj->m_guid.m_guid;
            CGTradeSkillInfo::SetTradeSkill(spell->m_id, tradeGuid, NULL, 0, 0);
        }

        Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_UNKNOWN);
        return false;
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        uint32_t flags = static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags;

        if (flags & 0x1000000) {
            if (melee || Spell_C_IsRestrictedAbility(spell) || SpellIsWeaponDamageEffect(effect)) {
                Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW, -1, -1, false);
                return false;
            }
        } else if (!melee && (flags & 0x800000)) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW, -1, -1, false);
            return false;
        }
    }

    if (unit->IsDeadOrGhost() && !(spell->m_attributes & 0x800000)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_CASTER_DEAD, -1, -1, false);
        return false;
    }

    if (effect == 78) {
        if (unit->OnAttackIconPressed(target, 0, 0, 0)) {
            Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_UNKNOWN);
        } else {
            SpellClearMacroGlobalCooldownWait(spell);
            ReleasePendingSpellCast(cast);
        }

        return false;
    }

    if ((effect == 99 || effect == 127 || effect == 158) && unit->GetComboPointTarget()) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_NOT_WHILE_LOOTING, -1, -1, false);
        return false;
    }

    if (((effect == 53 || effect == 54 || effect == 92 || effect == 156) && unit->GetComboPointTarget()) || unit->m_mountTransitionSpellId) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_SPELL_IN_PROGRESS, -1, -1, false);
        return false;
    }

    bool onCooldown = Spell_C_GetSpellCooldown(spellId, !unit->IsActivePlayer(), NULL, NULL, NULL, &cast->m_cooldownDelay);
    WOWGUID casterGuid = unit->m_obj->m_guid;

    if (Spell_C_HasOtherPendingCast(casterGuid, spellId, cast) && ((onCooldown && cast->m_cooldownDelay >= s_maxStartRecoveryOffset) || (spell->m_attributes & 0x404))) {
        if (s_enableErrorSpeechCVar && s_enableErrorSpeechCVar->m_intValue) {
            SE3SoundKitProperties properties;
            properties.m_uniqueness = 2;
            SE3::PlaySoundKit("igPlayerInviteDecline", NULL, &properties, NULL, __FILE__, __LINE__, "Spell_C_CastSpell()");
        }

        ReleasePendingSpellCast(cast);
        return false;
    }

    Spell_C_StopTargeting();
    cast = s_castingSpellCast;

    if (item) {
        const_cast<CGItem_C*>(item)->HasUseEffect();
    }

    int32_t petIndex = CGStableInfo::FindPetIndexBySpellID(spell->m_id, 0);

    if (petIndex != -1) {
        CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
        const StablePetInfo* stablePet = CGStableInfo::GetPetInfo(petIndex);

        if (pet && stablePet && pet->m_unit->m_unitFieldPetnumber == stablePet->m_petNumber) {
            CGPetInfo::PetFollow();
            Spell_C_FailPendingCast(cast, spell, SPELL_FAILED_UNKNOWN);
            return true;
        }
    }

    if ((spell->m_attributesExG & 0x8) && !(unit->m_unit->m_unitFieldFlags2 & 0x40000)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_SPELL_UNAVAILABLE, -1, -1, false);
        return false;
    }

    if (!ignoreCost && (!item || !(const_cast<CGItem_C*>(item)->GetItemStaticFlags(0) & 0x10000000)) && !Spell_C_HaveSpellTokens(unit, spell, true, cast)) {
        ReleasePendingSpellCast(cast);
        return false;
    }

    if (!Spell_C_HaveEquippedSpellItems(unit, spell, true, cast)) {
        ReleasePendingSpellCast(cast);
        return false;
    }

    if (spell->m_attributes & 0x2) {
        bool usable;

        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            CGItem_C* weapon = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(static_cast<CGPlayer_C*>(unit)->m_bag.GetItemGuid(17)), TYPEMASK_ITEM, __FILE__, __LINE__));
            usable = weapon && weapon->IsUsable();
        } else {
            usable = unit->GetVirtualItem(2, false) != NULL;
        }

        if (!usable) {
            int32_t itemClass = -1;
            int32_t itemSubclass = 0;
            const SpellEquippedItemsRec* equipped;

            if (spell->m_spellEquippedItemsID && (equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID))) {
                itemClass = equipped->m_equippedItemClass;
                itemSubclass = equipped->m_equippedItemSubclass;
            }

            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_EQUIPPED_ITEM, itemClass, itemSubclass, false);
            return false;
        }
    }

    if (spell->m_attributesEx & 0x500000) {
        bool anyTarget = (spell->m_attributesExH & 0x80000) != 0;

        if (((unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() && !CGGameUI::GetComboPoints(target, anyTarget)) || (unit->m_obj->m_guid == CGPetInfo::GetPet(0) && !CGPetInfo::GetComboPoints(target, anyTarget))) && !unit->HasIgnoreAuraState(spell)) {
            Spell_C_SpellFailed(cast, spell, unit->m_unit->m_unitFieldBytes0[1] == 1 ? SPELL_FAILED_CASTER_AURASTATE : SPELL_FAILED_NO_COMBO_POINTS, -1, -1, false);
            return false;
        }
    }

    if (!CGInstanceEncounter_C::CanResurrect(spell->m_id)) {
        Spell_C_SpellFailed(cast, spell, SPELL_FAILED_IN_COMBAT_RES_LIMIT_REACHED, -1, -1, false);
        return false;
    }

    bool autoAttack = false;

    if (Spell_C_IsRestrictedAbility(spell) && unit->IsActivePlayer()) {
        bool face = !((spell->m_attributesExF >> 28) & 1);
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player->IsInMelee() && (!target || !unit->m_unit->m_unitFieldTarget || target == unit->m_unit->m_unitFieldTarget)) {
            if (!unit->VerifyAttackIconPressed(&target, 0, 0, face, true)) {
                SpellClearMacroGlobalCooldownWait(spell);
                ReleasePendingSpellCast(cast);
                return false;
            }
        }

        if (!(spell->m_attributesExB & 0x100000)) {
            autoAttack = true;
        }
    }

    uint8_t inRange;

    if ((Spell_C_RangeCheckSelected(unit, spell, target, &inRange, true, cast) && !inRange) || !unit->CheckAndReportSpellInhibitFlags(cast, spell, item, ignoreCost)) {
        ReleasePendingSpellCast(cast);
        return false;
    }

    int32_t delay = 0;

    if (!Spell_C_CanCastWhileCasting(spell, unit, &delay)) {
        cast->m_cooldownDelay = delay;
    }

    if (item) {
        SI3::PlayItemSound(2, const_cast<CGItem_C*>(item));
    } else if (spell->m_attributes & 0x10) {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("GAMEABILITYACTIVATE", NULL, &properties, NULL, __FILE__, __LINE__, "Spell_C_CastSpell()");
    } else {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("GAMESPELLACTIVATE", NULL, &properties, NULL, __FILE__, __LINE__, "Spell_C_CastSpell()");
    }

    if (!Spell_C_BeginCast(unit, spell, target, cast, confirmed)) {
        if (target.m_guid == 0xFFFFFFFFFFFFFFFEull) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_BAD_IMPLICIT_TARGETS, -1, -1, false);
            return false;
        }

        if ((spell->m_attributes & 0x200) && (!(unit->m_obj->m_type & TYPEMASK_PLAYER) || !static_cast<CGPlayer_C*>(unit)->m_bag.GetItemGuid(15))) {
            Spell_C_SpellFailed(cast, spell, SPELL_FAILED_MAINHAND_EMPTY, -1, -1, false);
            return false;
        }

        if ((s_needTargets & 0x80) && (static_cast<uint32_t>(target.m_guid) & static_cast<uint32_t>(target.m_guid >> 32)) != 0xFFFFFFFF && (!(spell->m_attributesExE & 0x800) || !(s_needTargets & 0x10C))) {
            Spell_C_SpellFailed(cast, spell, CGGameUI::m_lockedTarget ? SPELL_FAILED_BAD_TARGETS : SPELL_FAILED_BAD_IMPLICIT_TARGETS, -1, -1, false);
            return false;
        }

        s_targetingSpell = cast;
        CursorSetResetMode(2);
        CursorSetMode(28);
        CGSpellBook::UpdateSelection(0);
        CGInputControl* input = CGInputControl::GetActive();
        input->m_mouseModeFlags |= 0x10;
        input->UpdateMouseMode(0);
        float facing = 0.0f;
        s_cursorWorldObject->SetFacing(facing);
        s_cursorWorldObject->m_scale = 1.0f;

        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* objectEffect = effects->m_data[i];

            if (!objectEffect) {
                continue;
            }

            int32_t type = objectEffect->m_effect;

            if (type == 50 || type == 104 || type == 76 || type == 81 || type == 171) {
                WOWGUID guid = { static_cast<uint64_t>(static_cast<int64_t>(spell->m_id)) | 0x1FE0000000000000ull };
                GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(objectEffect->m_effectMiscValue, guid, GameObjectStatsCallback, reinterpret_cast<void*>(static_cast<uintptr_t>(s_targetingSpell->m_data.m_castCount)), false);

                if (stats) {
                    GameObjectDisplayInfoRec* display = g_gameObjectDisplayInfoDB.GetRecord(stats->m_displayID);

                    if (display) {
                        s_cursorWorldObject->m_scale = stats->m_scale;
                        s_cursorWorldObject->Create(display);
                    }
                }

                break;
            }
        }

        CSimpleTop::s_instance->m_checkFocus = 1;
    }

    if (!s_targetingSpell) {
        if (autoAttack && unit->IsActivePlayer() && !ClntObjMgrGetActivePlayerObj()->IsInMelee() && target == CGGameUI::m_lockedTarget) {
            unit->OnAttackIconPressed(target, 0, 0, 0);
        }

        if (!s_targetingSpell && spellId == GetModalNextSpell() && unit->IsActivePlayer() && !ClntObjMgrGetActivePlayerObj()->IsInMelee()) {
            unit->OnAttackIconPressed(target, 0, 0, 0);
        }
    }

    if (spell->m_attributesExB & 0x20) {
        if (unit->IsActivePlayer()) {
            Spell_C_SetAutoRepeatingSpell(spellId);
            return true;
        }
    } else if (s_autoRepeatingSpell) {
        SpellRec* autoRepeat = g_spellDB.GetRecord(s_autoRepeatingSpell);
        const SpellEquippedItemsRec* equipped;

        if (autoRepeat && autoRepeat->m_spellEquippedItemsID && (equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID)) && equipped->m_equippedItemClass == 2 && (equipped->m_equippedItemSubclass & 0x80000)) {
            Spell_C_CancelAutoRepeat(true);
        }
    }

    return true;
}

// OFFSET: 0x008B9060/0x140601060
bool Spell_C_CastSpell(int32_t spellId, const CGItem_C* item, WOWGUID target, bool confirmed, bool a5, int32_t baseSpell, int32_t ignoreCost) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    return Spell_C_CastSpell(player, spellId, item, target, NULL, confirmed, a5, baseSpell, ignoreCost);
}

// OFFSET: 0x008B90B0/0x140601100
void Spell_C_ClickSpell(int32_t spellId, const CGItem_C* item, WOWGUID target, int32_t baseSpell, int32_t ignoreCost) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    if (Spell_C_IsActiveSpell(spell, player)) {
        if (!g_secureAbilityToggleCVar->m_intValue || !s_spellHistory[0].RecentlyCast(spellId, 0)) {
            Spell_C_CancelAura(spell, player, -1);
        }
    } else if (spellId == GetModalNextSpell()) {
        WOWGUID none = WOWGUID::Make(0);
        player->OnAttackIconPressed(none, 0, 0, 0);
    } else if (spellId == s_autoRepeatingSpell) {
        Spell_C_CancelAutoRepeat(true);
    } else if (spellId != Spell_C_GetTargetingSpell()) {
        Spell_C_CastSpell(spellId, item, target, false, false, baseSpell, ignoreCost);
    }
}

// OFFSET: 0x008B91B0/0x140601250
bool Spell_C_HandleItemDrop(CGItem_C* item, CGUnit_C* unit) {
    if (!item || !unit || !unit->m_unit->m_unitFieldPetnumber || !s_feedPetSpell) {
        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || unit->m_unit->m_unitFieldCreatedby != player->m_obj->m_guid) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_feedPetSpell);

    if (!spell || !Spell_C_CastSpell(spell->m_id, NULL, WOWGUID::Make(0), false, false, 0, 0)) {
        return false;
    }

    return Spell_C_HandleSpriteClick(item, NULL);
}

// OFFSET: 0x008B9250/0x140601340
int32_t Spell_C_Idle(const void* data, void* param) {
    uint32_t now = FrameTime::GetCurTimeMs();

    for (PeriodicClientTrigger* trigger = s_periodicClientTriggers.Head(); trigger; ) {
        PeriodicClientTrigger* next = s_periodicClientTriggers.Next(trigger);
        int32_t targeting = Spell_C_GetTargetingSpell();

        if ((trigger->m_period || !targeting || targeting != trigger->m_spellId) && static_cast<int32_t>(now - trigger->m_nextTime) >= 0) {
            CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(trigger->m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (caster) {
                if (!Spell_C_CastSpell(caster, trigger->m_spellId, NULL, WOWGUID::Make(0), NULL, false, false, 0, 0)) {
                    trigger = next;
                    continue;
                }

                if (trigger->m_period && s_targetingSpell) {
                    Spell_C_StopTargeting();
                }
            }

            trigger->m_nextTime = now + trigger->m_period;
        }

        trigger = next;
    }

    return 1;
}

// OFFSET: 0x008B9330
int32_t Spell_C_SendDelayedMissileTrajectoryCast(CGUnit_C* unit) {
    PendingSpellCast* cast = s_delayedTrajectoryCast;

    if (!cast) {
        return 0;
    }

    if (unit->m_obj->m_guid != CGUnit_C::s_activeMover) {
        s_delayedTrajectoryCast = NULL;
        return 0;
    }

    CGItem_C* item;

    if (cast->m_data.m_castItem) {
        CGObject_C* object = ClntObjMgrObjectPtr(cast->m_data.m_castItem, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && !(object->m_obj->m_type & TYPEMASK_ITEM)) {
            object = NULL;
        }

        item = static_cast<CGItem_C*>(object);
        cast = s_delayedTrajectoryCast;
    } else {
        item = NULL;
    }

    int32_t result = Spell_C_CastSpell(unit, cast->m_data.m_spellId, item, cast->m_data.unk_0038, cast, false, false, 0, 0);
    s_delayedTrajectoryCast = NULL;
    return result;
}

// OFFSET: 0x008B93E0
void Spell_C_AddPendingItemCooldown(int32_t itemId, int32_t spellId, uint32_t time, bool onHold) {
    HASHKEY_NONE key;
    ITEMCOOLDOWNHASHNODE* node = s_itemCooldowns.Ptr(itemId, key);

    if (!node) {
        node = s_itemCooldowns.New(itemId, key, 0, 0);
    }

    node->m_spellId = spellId;
    node->m_time = time;
    node->m_onHold = onHold;
}

// OFFSET: 0x008B9480/0x140601720
void Spell_C_AddPeriodicClientTrigger(WOWGUID caster, int32_t auraSpellId, int32_t spellId, uint32_t period) {
    if (!s_periodicClientTriggers.Head()) {
        EventRegister(EVENT_ID_IDLE, Spell_C_Idle);
    }

    PeriodicClientTrigger* trigger = s_periodicClientTriggers.NewNode(2, 0, 8);
    trigger->m_auraSpellId = auraSpellId;
    trigger->m_spellId = spellId;
    trigger->m_period = period;
    trigger->m_nextTime = FrameTime::GetCurTimeMs() + period;
    trigger->m_caster = caster;
}

// OFFSET: 0x008B9550
void Spell_C_RemovePeriodicClientTrigger(WOWGUID caster, int32_t auraSpellId) {
    for (PeriodicClientTrigger* trigger = s_periodicClientTriggers.Head(); trigger; ) {
        PeriodicClientTrigger* next = s_periodicClientTriggers.Next(trigger);

        if (trigger->m_caster == caster && (trigger->m_auraSpellId == auraSpellId || !auraSpellId)) {
            if (!trigger->m_period && Spell_C_GetTargetingSpell() == trigger->m_spellId) {
                Spell_C_StopTargeting();
                break;
            }

            s_periodicClientTriggers.DeleteNode(trigger);
        }

        trigger = next;
    }

    if (!s_periodicClientTriggers.Head()) {
        EventUnregister(EVENT_ID_IDLE, Spell_C_Idle);
    }
}

// OFFSET: 0x008B9600
void Spell_C_AddProcessedDestLocSpellCast(WOWGUID caster, uint8_t count, uint32_t time) {
    CHashKeyGUID key;
    key.m_guid = caster;
    DESTLOCSPELLCASTINDEXHASHNODE* node = s_destLocSpellCasts.Ptr(static_cast<uint32_t>(caster.m_guid), key);

    if (!node) {
        node = s_destLocSpellCasts.New(static_cast<uint32_t>(caster.m_guid), key, 0, 0);
    }

    node->m_castCount = count;
    node->m_time = time + 10000;
}

// OFFSET: 0x008B96B0
void PendingSpellCast::OnSpellGo(CDataStore* msg) {
    WOWGUID castItem = this->m_data.m_castItem;
    int32_t spellId = this->m_data.m_spellId;
    WOWGUID caster = this->m_data.m_caster;
    uint32_t castFlags = this->m_data.m_castFlags;
    uint32_t castFlagsEx = this->m_data.m_castFlagsEx;
    uint32_t serverTime = 0;
    msg->Get(serverTime);

    uint8_t hitCount = 0;
    msg->Get(hitCount);
    TSStackArray<WOWGUID> hits(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * hitCount)), hitCount, 1);

    for (uint32_t i = 0; i < hitCount; i++) {
        msg->Get(hits.m_data[i].m_guid);
    }

    uint8_t missCount = 0;
    msg->Get(missCount);
    TSStackArray<WOWGUID> misses(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * missCount)), missCount, 1);
    TSStackArray<uint32_t> missReasons(static_cast<uint32_t*>(alloca(sizeof(uint32_t) * missCount)), missCount, 1);
    TSStackArray<uint32_t> reflectResults(static_cast<uint32_t*>(alloca(sizeof(uint32_t) * missCount)), missCount, 1);

    for (uint32_t i = 0; i < missCount; i++) {
        msg->Get(misses.m_data[i].m_guid);
        uint8_t value = 0;
        msg->Get(value);
        missReasons.m_data[i] = value;

        if (missReasons.m_data[i] == 11) {
            value = 0;
            msg->Get(value);
            reflectResults.m_data[i] = value;
        }
    }

    SpellGetCastTargets(&this->m_data, msg);
    uint32_t power = 0;
    uint32_t hasPower = castFlags & 0x800;

    if (hasPower) {
        msg->Get(power);
    }

    uint8_t runesBefore = 0;
    uint8_t runesAfter = 0;
    uint8_t runeCooldowns[6] = { 0, 0, 0, 0, 0, 0 };
    uint32_t hasRunes = castFlags & 0x200000;

    if (hasRunes) {
        msg->Get(runesBefore);
        msg->Get(runesAfter);

        for (uint32_t i = 0; i < 6; i++) {
            msg->Get(runeCooldowns[i]);
        }
    }

    uint32_t delay = 0;

    if (castFlags & 0x20000) {
        msg->Get(this->m_data.m_elevation);
        msg->Get(delay);
    }

    uint32_t ammoDisplayId = 0;
    uint32_t ammoInventoryType = 0;

    if (castFlags & 0x20) {
        msg->Get(ammoDisplayId);
        msg->Get(ammoInventoryType);
    }

    uint32_t visualA = 0;
    uint32_t visualB = 0;

    if (castFlags & 0x80000) {
        msg->Get(visualA);
        msg->Get(visualB);
    }

    uint32_t now = OsGetAsyncTimeMs();

    if (this->m_data.m_targetFlags & 0x40) {
        uint8_t count = 0;
        msg->Get(count);

        if (count) {
            Spell_C_AddProcessedDestLocSpellCast(caster, count, now);
        }
    }

    uint32_t pointCount = 0;

    if (this->m_data.m_targetFlags & 0x80000) {
        msg->Get(pointCount);
    }

    TSStackArray<SpellCastTargetPoint> points(static_cast<SpellCastTargetPoint*>(alloca(sizeof(SpellCastTargetPoint) * pointCount)), pointCount, 1);

    if (this->m_data.m_targetFlags & 0x80000) {
        uint32_t valid = 0;

        for (uint32_t i = 0; i < pointCount; i++) {
            SpellCastTargetPoint& point = points.m_data[valid];
            msg->GetArray(reinterpret_cast<uint32_t*>(&point.m_position), 3);
            msg->Get(point.m_transport.m_guid);

            if (point.m_transport.m_guid) {
                C44Matrix matrix;

                if (!MovementGetTransportMtxX(point.m_transport, &matrix, WOWGUID::Make(0), __FILE__, __LINE__)) {
                    continue;
                }

                point.m_worldPosition = point.m_position * matrix;
            } else {
                point.m_worldPosition = point.m_position;
            }

            valid++;
        }

        points.SetCount(valid);
    }

    SpellRec* spell = g_spellDB.GetRecord(this->m_data.m_spellId);

    if (!spell) {
        ReleasePendingSpellCast(this);
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!this->HasValidTransports()) {
        if (!(this->m_data.m_castFlags & 0x1) && unit) {
            FinishPendingSpellCast(this, unit, spell, 0);
        } else {
            ReleasePendingSpellCast(this);
        }

        return;
    }

    int32_t categoryRecoveryTime = 0;
    int32_t recoveryTime = 0;
    int32_t category = 0;
    int32_t startRecoveryCategory = 0;
    int32_t startRecoveryTime = 0;
    const SpellCategoriesRec* categories;

    if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
        category = categories->m_category;
        startRecoveryCategory = categories->m_startRecoveryCategory;
    }

    const SpellCooldownsRec* cooldowns;

    if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID))) {
        categoryRecoveryTime = cooldowns->m_categoryRecoveryTime;
        recoveryTime = cooldowns->m_recoveryTime;
        startRecoveryTime = cooldowns->m_startRecoveryTime;
    }

    if (unit && hasPower) {
        if (spell->m_powerType == -2) {
            if (!(unit->m_unit->m_unitFieldFlags2 & 0x100)) {
                unit->SetPredictedHealth(power);
            }
        } else if (spell->m_powerType == 5) {
            if (caster == ClntObjMgrGetActivePlayer() && hasRunes) {
                CGRuneInfo::SetPredictedRunes(runesBefore, runesAfter, runeCooldowns, true);
            }
        } else {
            if (!(unit->m_unit->m_unitFieldFlags2 & 0x100)) {
                unit->SetPredictedPower(spell->m_powerType, power, true);
            }

            if (!(unit->m_obj->m_type & TYPEMASK_PLAYER) && !spell->m_powerType) {
                unit->SetManaRegenInterrupt();
            }
        }
    }

    uint32_t hidden = castFlags & 0x1;

    if (!hidden) {
        s_lastFailedSpellId = 0;
        HandleSpellFailedOrGo(unit, this, spell, SPELL_FAILED_UNKNOWN);
    }

    uint32_t arrival = now;
    C3Vector destLocation = this->m_data.GetDestLocation();

    if (unit && spell->m_speed != 0.0f) {
        bool speedIsTime = (spell->m_attributesExI & 0x10) != 0;

        if (this->m_data.m_targetFlags & 0x40) {
            int64_t travel;

            if (speedIsTime) {
                travel = static_cast<int64_t>(spell->m_speed * -1000.0f);
            } else {
                C3Vector position = unit->GetPosition();
                float dx = position.x - destLocation.x;
                float dy = position.y - destLocation.y;
                float dz = position.z - destLocation.z;
                travel = static_cast<int64_t>(sqrtf(dx * dx + dy * dy + dz * dz) / spell->m_speed * -1000.0f);
            }

            arrival = now - static_cast<uint32_t>(travel);
            Spell_C_AddToInTransitList(caster, spell, serverTime, arrival);
        } else if (hitCount) {
            CGObject_C* target = ClntObjMgrObjectPtr(hits.m_data[0], TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (target) {
                int64_t travel;

                if (speedIsTime) {
                    travel = static_cast<int64_t>(spell->m_speed * -1000.0f);
                } else {
                    C3Vector targetPosition = target->GetPosition();
                    C3Vector position = unit->GetPosition();
                    float dx = position.x - targetPosition.x;
                    float dy = position.y - targetPosition.y;
                    float dz = position.z - targetPosition.z;
                    travel = static_cast<int64_t>(sqrtf(dx * dx + dy * dy + dz * dz) / spell->m_speed * -1000.0f);
                }

                arrival = now - static_cast<uint32_t>(travel);
                Spell_C_AddToInTransitList(caster, spell, serverTime, arrival);
            }
        }

        if (this->m_data.m_targetFlags & 0x80000) {
            for (uint32_t i = 0; i < points.m_count; i++) {
                const SpellCastTargetPoint& point = points.m_data[i];
                int64_t travel;

                if (speedIsTime) {
                    travel = static_cast<int64_t>(spell->m_speed * -1000.0f);
                } else {
                    C3Vector position = unit->GetPosition();
                    float dx = position.x - point.m_worldPosition.x;
                    float dy = position.y - point.m_worldPosition.y;
                    float dz = position.z - point.m_worldPosition.z;
                    travel = static_cast<int64_t>(sqrtf(dx * dx + dy * dy + dz * dz) / spell->m_speed * -1000.0f);
                }

                arrival = now - static_cast<uint32_t>(travel);
                Spell_C_AddToInTransitList(caster, spell, serverTime, arrival);
            }
        }
    }

    CGObject_C* castObject = ClntObjMgrObjectPtr(castItem, TYPEMASK_OBJECT, __FILE__, __LINE__);
    WOWGUID source = castObject && (castObject->m_obj->m_type & (TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT)) ? castItem : caster;

    if (hidden && spell->m_attributesExG >= 0) {
        if (spell->m_attributesExF & 0x40) {
            UnitCombatLogProcGo(castItem, spell);
        }
    } else {
        UnitCombatLogCastGo(caster, this->m_data.m_objectTarget, spell);
    }

    if (spell->m_speed == 0.0f && !(spell->m_attributesExB & 0x80)) {
        for (uint32_t i = 0; i < missCount; i++) {
            UnitCombatLogSpellMissed(source, misses.m_data[i], spell, missReasons.m_data[i], false, false, 0);

            if (!(spell->m_attributes & 0x100)) {
                CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(misses.m_data[i], TYPEMASK_UNIT, __FILE__, __LINE__));

                if (target) {
                    target->AddWorldText(source, missReasons.m_data[i], spell, false, 0);
                }

                CGGameUI::ShowSpellMissFeedback(misses.m_data[i], missReasons.m_data[i]);
            }

            if (missReasons.m_data[i] == 11 && reflectResults.m_data[i]) {
                UnitCombatLogSpellMissed(source, caster, spell, reflectResults.m_data[i], false, false, 0);

                if (!(spell->m_attributes & 0x100)) {
                    CGUnit_C* reflector = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (reflector) {
                        reflector->AddWorldText(source, reflectResults.m_data[i], spell, false, 0);
                    }

                    CGGameUI::ShowSpellMissFeedback(caster, reflectResults.m_data[i]);
                }
            }

            CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(misses.m_data[i], TYPEMASK_UNIT, __FILE__, __LINE__));

            if (target) {
                target->PlayCombatSpellMissEffects(missReasons.m_data[i], source);
            }
        }
    }

    bool onHold = Spell_C_HasCooldownOnEvent(spell, castItem);
    TSStackArray<WOWGUID> targets(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * (missCount + hitCount))), missCount + hitCount, 1);

    if (castFlags & 0x400) {
        for (uint32_t i = 0; i < missCount; i++) {
            targets.m_data[i] = misses.m_data[i];
        }

        for (uint32_t i = 0; i < hitCount; i++) {
            targets.m_data[missCount + i] = hits.m_data[i];
        }
    } else {
        for (uint32_t i = 0; i < hitCount; i++) {
            targets.m_data[i] = hits.m_data[i];
        }

        for (uint32_t i = 0; i < missCount; i++) {
            targets.m_data[hitCount + i] = misses.m_data[i];
        }
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual;

    if (unit) {
        visual = unit->GetAppropriateSpellVisual(spell, visualRec, visualA, visualB);

        if (visual && Spell_C_SpellVisualHasChainEffect(visual)) {
            unit->SaveChainSpellTargets(targets);
            bool chainLocation = (visual->m_flags & 0x1) && targets.m_count;

            if ((this->m_data.m_targetFlags & 0x40) && !chainLocation) {
                unit->SaveChainSpellLocation(&destLocation);
            } else {
                unit->SaveChainSpellLocation(NULL);
            }
        }
    } else {
        visual = GetSpellVisual(spell, 0);
    }

    if (visual) {
        bool canPlay = unit && CanPlaySpellVisual(unit->m_obj->m_guid.m_guid, this->m_data.m_objectTarget.m_guid, visual);
        SpellVisualKitRec* castKit = g_spellVisualKitDB.GetRecord(visual->m_castKit);

        if (canPlay && unit && !this->m_trajectoryLaunched && CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, this->m_data.m_objectTarget.m_guid, castKit, NULL)) {
            PlaySpellVisuaKitData kitData(spell, castKit, 0);
            kitData.m_someGUID = unit->m_obj->m_guid;
            kitData.m_displayId = ammoDisplayId;
            kitData.m_inventoryType = ammoInventoryType;
            unit->PlaySpellVisualKit(kitData);
        }

        if (this->m_data.m_targetFlags & 0x40) {
            SpellVisualKitRec* instantKit = g_spellVisualKitDB.GetRecord(visual->m_instantAreaKit);

            if (canPlay && unit && CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, 0, instantKit, NULL)) {
                PlaySpellVisuaKitData kitData(spell, instantKit, 3);
                kitData.m_position = &this->m_data.m_destPos;
                kitData.m_transportGUID = this->m_data.m_destTarget;
                kitData.m_displayId = ammoDisplayId;
                kitData.m_inventoryType = ammoInventoryType;
                unit->PlaySpellVisualKit(kitData);
            }

            int32_t impactKitId = visual->m_impactAreaKit;
            SpellVisualKitRec* impactKit = g_spellVisualKitDB.GetRecord(impactKitId);

            if (impactKit && (!visual->m_hasMissile || spell->m_speed <= 0.0f) && !Spell_C_SpellVisualHasChainEffect(visual)) {
                if (unit) {
                    if (canPlay && CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, 0, impactKit, NULL)) {
                        if (spell->m_speed <= 0.0f) {
                            PlaySpellVisuaKitData kitData(spell, impactKit, 3);
                            kitData.m_position = &this->m_data.m_destPos;
                            kitData.m_transportGUID = this->m_data.m_destTarget;
                            kitData.m_displayId = ammoDisplayId;
                            kitData.m_inventoryType = ammoInventoryType;
                            unit->PlaySpellVisualKit(kitData);
                        } else {
                            unit->DelaySpellVisualKit(spellId, impactKitId, 3, &this->m_data.m_destPos, this->m_data.m_destTarget, 0, 0, -1, 1, arrival, ammoDisplayId, ammoInventoryType);
                        }
                    }
                } else if (ClntObjMgrObjectPtr(castItem, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__)) {
                    NotifyDestLocSpellCastData data;
                    data.m_caster = castItem;
                    data.m_destTarget = this->m_data.m_destTarget;
                    data.m_spellId = spellId;
                    data.m_destLocation = destLocation;
                    data.m_destPosition = this->m_data.m_destPos;
                    data.m_elevation = this->m_data.m_elevation;
                    data.unk_0030 = this->m_data.m_speed;
                    data.unk_0034 = 0;
                    data.unk_0038 = 0;
                    data.m_castCount = this->m_data.m_castCount;
                    Spell_C_PlayDestLocVisualsWithoutCaster(&data);
                }
            }
        }

        if (this->m_data.m_targetFlags & 0x80000) {
            for (uint32_t i = 0; i < points.m_count; i++) {
                SpellCastTargetPoint& point = points.m_data[i];
                SpellVisualKitRec* instantKit = g_spellVisualKitDB.GetRecord(visual->m_instantAreaKit);

                if (canPlay && unit && CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, 0, instantKit, NULL)) {
                    PlaySpellVisuaKitData kitData(spell, instantKit, 3);
                    kitData.m_position = &point.m_position;
                    kitData.m_transportGUID = point.m_transport;
                    kitData.m_displayId = ammoDisplayId;
                    kitData.m_inventoryType = ammoInventoryType;
                    unit->PlaySpellVisualKit(kitData);
                }

                int32_t impactKitId = visual->m_impactAreaKit;
                SpellVisualKitRec* impactKit = g_spellVisualKitDB.GetRecord(impactKitId);

                if (!impactKit || (visual->m_hasMissile && spell->m_speed > 0.0f) || Spell_C_SpellVisualHasChainEffect(visual)) {
                    continue;
                }

                if (unit) {
                    if (canPlay && CanPlaySpellVisualKit(unit->m_obj->m_guid.m_guid, 0, impactKit, NULL)) {
                        if (spell->m_speed <= 0.0f) {
                            PlaySpellVisuaKitData kitData(spell, impactKit, 3);
                            kitData.m_position = &point.m_position;
                            kitData.m_transportGUID = point.m_transport;
                            kitData.m_displayId = ammoDisplayId;
                            kitData.m_inventoryType = ammoInventoryType;
                            unit->PlaySpellVisualKit(kitData);
                        } else {
                            unit->DelaySpellVisualKit(spellId, impactKitId, 3, &point.m_position, point.m_transport, 0, 0, -1, 1, arrival, ammoDisplayId, ammoInventoryType);
                        }
                    }
                } else if (ClntObjMgrObjectPtr(castItem, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__)) {
                    NotifyDestLocSpellCastData data;
                    data.m_caster = castItem;
                    data.m_destTarget = WOWGUID::Make(0);
                    data.m_spellId = spellId;
                    data.m_destLocation = point.m_worldPosition;
                    data.m_destPosition = point.m_worldPosition;
                    data.m_elevation = this->m_data.m_elevation;
                    data.unk_0030 = this->m_data.m_speed;
                    data.unk_0034 = 0;
                    data.unk_0038 = 0;
                    data.m_castCount = this->m_data.m_castCount;
                    Spell_C_PlayDestLocVisualsWithoutCaster(&data);
                }
            }
        }

        if (spell->m_speed > 0.0f) {
            if (unit && CanPlaySpellVisual(unit->m_obj->m_guid.m_guid, this->m_data.m_objectTarget.m_guid, visual)) {
                if (this->m_trajectoryLaunched) {
                    if (this->m_data.m_castFlags & 0x20000) {
                        CMissile::AdjustAllMissileTrajectoryDurations(this->m_data.m_castCount, delay);
                    }
                } else {
                    if (spell->m_attributesExD & 0x40000) {
                        LaunchMissileVisual(hits, misses, unit, spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, missReasons, reflectResults, delay);

                        if ((visual->m_hasMissile || ammoDisplayId) && points.m_count) {
                            CreateMissileParams params;
                            memset(&params, 0, sizeof(params));
                            params.m_flags |= 1;
                            params.m_data = &this->m_data;
                            params.m_ammoDisplayId = ammoDisplayId;
                            params.m_ammoInventoryType = ammoInventoryType;
                            params.m_visual = visual;
                            params.m_spell = spell;
                            params.m_delay = delay;
                            params.m_points = &points;
                            unit->CreateMissile(params);
                        }
                    } else {
                        CreateMissilesForTargets(hits, unit, spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, NULL, NULL, delay);

                        if (misses.m_count) {
                            CreateMissilesForTargets(misses, unit, spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, &missReasons, &reflectResults, delay);
                        }

                        CreateMissilesForTargetPoints(unit, spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, points, delay);
                    }

                    if (visual->m_hasMissile && (!castKit || (castKit->m_animID <= 0 && castKit->m_animKitID <= 0))) {
                        unit->ReleaseMissiles();
                    }
                }
            }
        } else {
            PlayImpactVisual(spell, visual, hits, unit);

            if (unit) {
                for (uint32_t i = 0; i < missCount; i++) {
                    if (missReasons.m_data[i] == 11 && !reflectResults.m_data[i] && !(spell->m_attributesExG & 0x20000000)) {
                        PlayImpactVisualOnTarget(spell, visual, unit, unit);
                    }
                }
            }
        }
    }

    if (caster != ClntObjMgrGetActivePlayer() && unit && unit->GetControllingPlayer() == static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        unit->SpellHasteCooldowns(spell, recoveryTime, categoryRecoveryTime);
    } else if (caster == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);

        if (castItem == ClntObjMgrGetActivePlayer() && !(castFlags & 0x41)) {
            CGTradeSkillInfo::DoRecast(spellId);
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
        uint32_t index;

        for (index = 0; index < effectCount; index++) {
            if (effects->m_data[index] && effects->m_data[index]->m_effect == 33) {
                break;
            }
        }

        if (index < effectCount && hitCount == 1) {
            CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(hits.m_data[0], TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));
            int8_t type = gameObject ? gameObject->m_gameObject->m_typeID : -1;

            if (gameObject && type >= 0 && (type <= 1 || type == 3)) {
                if (type == 3) {
                    player->OnLootGameObject(hits.m_data[0]);
                }

                int32_t action = -1;
                LockRec* lock = gameObject->GetLockRec();

                if (lock) {
                    for (uint32_t i = 0; i < effectCount; i++) {
                        const SpellEffectRec* effect = effects->m_data[i];

                        if (!effect || effect->m_effect != 33) {
                            continue;
                        }

                        for (uint32_t j = 0; j < 8 && action < 0; j++) {
                            switch (lock->m_type[j]) {
                                case 1:
                                    if ((castObject->m_obj->m_type & TYPEMASK_ITEM) && castObject->m_obj->m_entryID == lock->m_index[j]) {
                                        action = lock->m_action[j];
                                    }

                                    break;

                                case 2:
                                    if (effect->m_effectMiscValue == lock->m_index[j]) {
                                        action = lock->m_action[j];
                                    }

                                    break;

                                case 3:
                                    if (spell->m_id == lock->m_index[j]) {
                                        action = lock->m_action[j];
                                    }

                                    break;
                            }
                        }
                    }

                    if (action >= 0) {
                        if (action <= 1) {
                            gameObject->SetDisplayState(0);
                        } else if (action == 2) {
                            gameObject->SetDisplayState(1);
                        }
                    }
                }
            }
        }

        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect) {
                continue;
            }

            int32_t type = effect->m_effect;

            if (type == 71 || type == 95 || type == 116 || ((type == 99 || type == 127 || type == 158 || type == 107) && CGGameUI::IsAutoLooting())) {
                if (!player->m_autoLoot) {
                    player->SetAutoLoot(g_autoLootDefaultCVar->m_intValue);
                }

                break;
            }
        }

        if ((spell->m_attributesExB & 0x100000) && !(player->m_combatModeFlags & 0x2) && !player->IsInMelee() && hitCount && hits.m_data[0] && hits.m_data[0] == CGGameUI::m_lockedTarget) {
            player->OnAttackIconPressed(hits.m_data[0], 0, 1, spellId);
        }

        int32_t itemId = 0;
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(castItem, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            itemId = item->m_obj->m_entryID;
        }

        if (static_cast<int32_t>(castFlags) >= 0 && !(castFlagsEx & 0x1) && (castItem == caster || itemId)) {
            if (castItem == caster) {
                int32_t categoryRecovery = categoryRecoveryTime;
                Spell_C_ModifyCategoryRecovery(categoryRecovery, spell, 0, 0);
                int32_t recovery = recoveryTime;
                float flat;
                float pct;

                if (Spell_C_GetSpellModifiers(spell, 11, flat, pct)) {
                    recovery = static_cast<int32_t>((static_cast<float>(recoveryTime) + flat) * pct * 0.0099999998f);
                }

                if (unit) {
                    unit->SpellHasteCooldowns(spell, recovery, categoryRecovery);
                }

                if (categoryRecovery < 0) {
                    categoryRecovery = 0;
                }

                uint32_t time = OsGetAsyncTimeMs();

                for (SPELLHISTORY* history = s_spellHistory[0].m_history.Head(); history; history = s_spellHistory[0].m_history.Next(history)) {
                    if (history->m_startRecoveryTime && static_cast<int32_t>(history->m_time - time) >= 0) {
                        history->m_time = time;
                    }
                }

                s_spellHistory[0].AddHistory(spellId, 0, now, recovery, category, now, categoryRecovery, onHold, 0, 0, 0);
            } else {
                if (!g_itemSparseDB.GetRecord(itemId, castItem, Spell_C_PendingItemCooldownCallback, reinterpret_cast<void*>(1), false)) {
                    Spell_C_AddPendingItemCooldown(itemId, spellId, now, onHold);
                    return;
                }

                if (item->GetEnchantmentSpell(0) == spellId) {
                    uint32_t itemCooldown;
                    uint32_t itemCategoryCooldown;
                    uint32_t itemCategoryValue;
                    item->GetSpellCooldownInfo(itemCooldown, itemCategoryCooldown, itemCategoryValue);
                    int32_t cooldown = itemCooldown;
                    int32_t categoryCooldown = itemCategoryCooldown;
                    int32_t itemCategory = itemCategoryValue;

                    if (cooldown < 0) {
                        cooldown = recoveryTime;
                    }

                    if (categoryCooldown < 0) {
                        categoryCooldown = categoryRecoveryTime;
                    }

                    Spell_C_ModifyCategoryRecovery(categoryCooldown, itemCategory, 0);

                    if (unit) {
                        int32_t before = cooldown;
                        player->ModifyCategoryCooldown(cooldown, itemCategory);

                        if (cooldown < before) {
                            categoryCooldown = cooldown;
                        }
                    }

                    uint32_t time = OsGetAsyncTimeMs();

                    for (SPELLHISTORY* history = s_spellHistory[0].m_history.Head(); history; history = s_spellHistory[0].m_history.Next(history)) {
                        if (history->m_startRecoveryTime && static_cast<int32_t>(history->m_time - time) >= 0) {
                            history->m_time = time;
                        }
                    }

                    s_spellHistory[0].AddHistory(spellId, itemId, now, cooldown, itemCategory, now, categoryCooldown, onHold, 0, 0, 0);
                }
            }

            CGSpellBook::UpdateCooldowns();

            if (onHold) {
                CGSpellBook::UpdateUsable();
            }

            CGContainerInfo::UpdateCooldowns();
        }
    }

    if (unit) {
        const WOWGUID& controller = unit->m_unit->m_unitFieldCharmedby.m_guid ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldSummonedby;

        if (controller == ClntObjMgrGetActivePlayer()) {
            int32_t recovery = recoveryTime;
            int32_t categoryRecovery = categoryRecoveryTime;
            float flat;
            float pct;

            if (Spell_C_GetSpellModifiers(spell, 11, flat, pct)) {
                recovery = static_cast<int32_t>((static_cast<float>(recoveryTime) + flat) * pct * 0.0099999998f);
            }

            if (spell->m_attributesExF >= 0 && Spell_C_GetSpellModifiers(spell, 11, flat, pct)) {
                categoryRecovery = static_cast<int32_t>((static_cast<float>(categoryRecoveryTime) + flat) * pct * 0.0099999998f);
            }

            Spell_C_ModifyCategoryRecovery(categoryRecovery, category, 0);
            int32_t startRecovery = 0;

            if (!(this->m_data.m_castFlags & 0x40000)) {
                startRecovery = startRecoveryTime;

                if (startRecoveryTime || startRecoveryCategory) {
                    if (startRecoveryCategory == 133 && startRecoveryTime == 1500) {
                        const SpellCategoriesRec* defense;

                        if ((!spell->m_spellCategoriesID || !(defense = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID)) || (defense->m_defenseType != 2 && defense->m_defenseType != 3)) && !(spell->m_attributes & 0x12)) {
                            startRecovery = static_cast<int32_t>(unit->m_unit->m_unitModCastHaste * 1500.0f);
                        }
                    }

                    if (Spell_C_GetSpellModifiers(spell, 21, flat, pct)) {
                        startRecovery = static_cast<int32_t>((static_cast<float>(startRecovery) + flat) * pct * 0.0099999998f);
                    }

                    if (static_cast<uint32_t>(startRecovery - 1000) > 500) {
                        startRecovery = startRecovery - 1000 < 0 ? 1000 : 1500;
                    }
                }
            }

            uint32_t time = OsGetAsyncTimeMs();

            for (SPELLHISTORY* history = s_spellHistory[1].m_history.Head(); history; history = s_spellHistory[1].m_history.Next(history)) {
                if (history->m_startRecoveryTime && static_cast<int32_t>(history->m_time - time) >= 0) {
                    history->m_time = time;
                }
            }

            s_spellHistory[1].AddHistory(spellId, 0, now, recovery, category, now, categoryRecovery, onHold, startRecoveryCategory, startRecovery, 0);
            CGSpellBook::UpdateCooldowns();

            if (onHold) {
                CGSpellBook::UpdateUsable();
            }

            CGPetInfo::UpdateCooldowns();
        }
    }

    if (static_cast<int32_t>(now - s_nextHistoryCleanup) >= 0) {
        s_nextHistoryCleanup = now + 120000;

        for (uint32_t i = 0; i < sizeof(s_spellHistory) / sizeof(s_spellHistory[0]); i++) {
            s_spellHistory[i].GarbageCollect(now);
        }
    }

    if (!(this->m_data.m_castFlags & 0x1) && unit) {
        FinishPendingSpellCast(this, unit, spell, 0);
    } else {
        ReleasePendingSpellCast(this);
    }
}

// OFFSET: 0x008BBAC0
int32_t Packet_Group_30(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID castItem;
    *msg >> castItem;
    SmartGUID caster;
    *msg >> caster;
    uint8_t castCount;
    msg->Get(castCount);
    int32_t spellId;
    msg->Get(reinterpret_cast<uint32_t&>(spellId));
    uint32_t castFlags;
    msg->Get(castFlags);
    uint32_t castFlagsEx;
    msg->Get(castFlagsEx);
    PendingSpellCast local;
    WOWGUID castItemGuid = castItem;
    WOWGUID casterGuid = caster;
    PendingSpellCast* cast = FindPendingSpellCast(castItemGuid, &local, castCount, spellId, casterGuid);
    cast->m_data.m_castFlags = castFlags;
    cast->m_data.m_castFlagsEx = castFlagsEx;

    if (msgId == SMSG_SPELL_START) {
        cast->OnSpellStartPacket(msg);
    } else if (msgId == SMSG_SPELL_GO) {
        cast->OnSpellGo(msg);
    }

    return 1;
}

// OFFSET: 0x008BBBB0
int32_t Packet_SMSG_NOTIFY_DEST_LOC_SPELL_CAST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    NotifyDestLocSpellCastData data;
    data.m_destLocation = C3Vector();
    data.m_destPosition = C3Vector();
    msg->GetData(&data, sizeof(data));

    if (data.m_caster) {
        CHashKeyGUID key;
        key.m_guid = data.m_caster;
        DESTLOCSPELLCASTINDEXHASHNODE* node = s_destLocSpellCasts.Ptr(static_cast<uint32_t>(data.m_caster.m_guid), key);

        if (node && static_cast<int8_t>(data.unk_0038 - node->m_castCount) <= 0) {
            return 1;
        }

        Spell_C_AddProcessedDestLocSpellCast(data.m_caster, data.unk_0038, OsGetAsyncTimeMs());
    }

    Spell_C_PlayDestLocVisualsWithoutCaster(&data);
    return 1;
}

// OFFSET: 0x008BBC50/0x140604900
void Spell_C_Initialize() {
    ClientServices::SetMessageHandler(SMSG_SPELL_START, Packet_Group_30, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_GO, Packet_Group_30, NULL);
    ClientServices::SetMessageHandler(SMSG_CAST_FAILED, Packet_SMSG_CAST_FAILED, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_FAILED_OTHER, Packet_SMSG_SPELL_FAILED_OTHER, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_FAILURE, Packet_SMSG_SPELL_FAILURE, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_CAST_FAILED, Packet_SMSG_PET_CAST_FAILED, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_COOLDOWN, Packet_SMSG_SPELL_COOLDOWN, NULL);
    ClientServices::SetMessageHandler(SMSG_ITEM_COOLDOWN, Packet_SMSG_ITEM_COOLDOWN, NULL);
    ClientServices::SetMessageHandler(SMSG_COOLDOWN_EVENT, Packet_Group_31, NULL);
    ClientServices::SetMessageHandler(SMSG_CLEAR_COOLDOWN, Packet_Group_31, NULL);
    ClientServices::SetMessageHandler(SMSG_MODIFY_COOLDOWN, Packet_Group_31, NULL);
    ClientServices::SetMessageHandler(SMSG_COOLDOWN_CHEAT, Packet_SMSG_COOLDOWN_CHEAT, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_TAME_FAILURE, Packet_SMSG_PET_TAME_FAILURE, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_DELAYED, Packet_SMSG_SPELL_DELAYED, NULL);
    ClientServices::SetMessageHandler(MSG_CHANNEL_START, Packet_MSG_CHANNEL_START, NULL);
    ClientServices::SetMessageHandler(MSG_CHANNEL_UPDATE, Packet_MSG_CHANNEL_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_FLAT_SPELL_MODIFIER, Packet_Group_32, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_PCT_SPELL_MODIFIER, Packet_Group_32, NULL);
    ClientServices::SetMessageHandler(SMSG_GAME_OBJECT_RESET_STATE, Packet_SMSG_GAMEOBJECT_RESET_STATE, NULL);
    ClientServices::SetMessageHandler(SMSG_FEIGN_DEATH_RESISTED, Packet_SMSG_FEIGN_DEATH_RESISTED, NULL);
    ClientServices::SetMessageHandler(SMSG_SPELL_UPDATE_CHAIN_TARGETS, Packet_SMSG_SPELL_UPDATE_CHAIN_TARGETS, NULL);
    ClientServices::SetMessageHandler(SMSG_NOTIFY_DEST_LOC_SPELL_CAST, Packet_SMSG_NOTIFY_DEST_LOC_SPELL_CAST, NULL);
    ClientServices::SetMessageHandler(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, Packet_SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_PROJECTILE_POSITION, Packet_SMSG_SET_PROJECTILE_POSITION, NULL);
    ClientServices::SetMessageHandler(SMSG_RESET_RANGED_COMBAT_TIMER, Packet_SMSG_RESET_RANGED_COMBAT_TIMER, NULL);
    ClientServices::SetMessageHandler(SMSG_RESUME_CAST_BAR, Packet_SMSG_RESUME_CAST_BAR, NULL);
    ClientPlaySpellVisualKit::s_handler = OnPlaySpellVisualKit;
    ClientWeeklySpellUsage::s_handler = OnWeeklySpellUsage;
    ClientWeeklySpellUsageUpdate::s_handler = OnWeeklySpellUsageUpdate;
    ClientClearCooldowns::s_handler = OnClearCooldowns;

    if (!s_cursorWorldObject) {
        void* mem = SMemAlloc(sizeof(SpellWorldObject), __FILE__, __LINE__, 0);
        s_cursorWorldObject = mem ? new (mem) SpellWorldObject() : NULL;
    }

    s_needTargets = 0;
    s_targetingSpell = NULL;
    s_castingSpellCast = NULL;
    memset(s_spellFlatModifiers, 0, sizeof(s_spellFlatModifiers));
    memset(s_spellPctModifiers, 0, sizeof(s_spellPctModifiers));
    s_autoRepeatingSpell = 0;
    s_autoRangedCombatSpell = 0;
    s_lastAutoRangedCombatError = 203;
    s_lastFailedSpellId = 0;
    s_feedPetSpell = 0;
    s_spellClassSet = 0;
    s_delayedTrajectoryCast = NULL;
    s_enableErrorSpeechCVar = CVar::Lookup("Sound_EnableErrorSpeech");
    s_numPendingSpellCasts = 0;
}

// OFFSET: #0x007FE4B0
const char* GetPowerString(uint32_t powerType) {
    const char* tokens[7] = { "MANA", "RAGE", "FOCUS", "ENERGY", "HAPPINESS", "RUNES", "RUNIC_POWER" };

    if (powerType < sizeof(tokens) / sizeof(tokens[0])) {
        return tokens[powerType];
    }

    return "";
}
