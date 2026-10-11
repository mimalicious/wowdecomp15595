#include "Tooltip.h"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include <typeinfo>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CreatureTypeRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemSetRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/PowerDisplayRec.h"
#include "WoW/Source/DB/Rec/ScalingStatDistributionRec.h"
#include "WoW/Source/DB/Rec/ScalingStatValuesRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentConditionRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/DB/Rec/TalentRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/QuestLogLeaderBoardParams.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include <cstdio>
#include <cstdlib>
#include "Engine/Source/Frame/CBackdropGenerator.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/GlyphSlotRec.h"
#include "WoW/Source/DB/Rec/ItemClassRec.h"
#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"
#include "WoW/Source/DB/Rec/ItemPurchaseGroupRec.h"
#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/LockTypeRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"
#include "WoW/Source/Object/CGPetition.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ArenaTeamInfo.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Magic/Spell.h"
#include "WoW/Source/Ui/ScalingStats.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/Flyout.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"
#include "WoW/Source/DB/Rec/ItemNameDescriptionRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemReforgeRec.h"
#include "WoW/Source/DB/Rec/PlayerConditionRec.h"
#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellClassOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"
#include "WoW/Source/DB/Rec/SpellDurationRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellLevelsRec.h"
#include "WoW/Source/DB/Rec/SpellPowerRec.h"
#include "WoW/Source/DB/Rec/SpellRangeRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"

// STATIC: 0x00C9A2A8
int32_t CGTooltip::s_metatable = LUA_NOREF;

// STATIC: 0x00D3A7F4
int32_t CGTooltip::s_objectType;

// STATIC: 0x00D3AC44
CDataAllocator CGTooltip::s_tooltipHeap(sizeof(CGTooltip), 5);

// STATIC: 0x00F4A330
WOWGUID CGTooltip::m_vehicleStatUnit;

// STATIC: 0x00C99F1C
static const char* s_tooltipTextColorString = "ffffffff";

// STATIC: 0x00C99F20
static const char* s_tooltipRedColorString = "ffff2020";

// STATIC: 0x00C99F24
static const char* s_tooltipGrayColorString = "ff808080";

// STATIC: 0x00C99F30
static const char* s_achievementLinkColor = "ffffff00";

// STATIC: 0x00C99F40
static const char* s_questDifficultyColorStrings[5] = { "ff808080", "ff40c040", "ffffff00", "ffff8040", "ffff2020" };

// STATIC: 0x00C99F54
static const char* s_positiveDeltaColorString = "ff00ff00";

// STATIC: 0x00C99F58
static const char* s_negativeDeltaColorString = "ffff2020";

// STATIC: 0x00C99F98
static const char* s_glyphLinkColor = "ff66bbff";

// STATIC: 0x00C99FA0
static const char* s_spellLinkColor = "ff71d5ff";

// STATIC: 0x00C99FA8
static const char* s_talentLinkColor = "ff4e96f7";

// STATIC: 0x00C99FAC
static const char* s_instanceLockLinkColor = "ffff8000";

// STATIC: 0x00C99FB0
static const char* s_currencyLinkColor = "ff00aa00";

// STATIC: 0x00C99FDC
static const char* s_gemColorTokens[5] = { NULL, "META_GEM", "RED_GEM", "YELLOW_GEM", "BLUE_GEM" };

// STATIC: 0x00C99FF0
static const char* s_colorblindSuffixes[5] = { "", "[+] ", "[++] ", "[+++] ", "[-] " };

// STATIC: 0x00C9A004
const char* CGTooltip::s_objectTypeName = "GameTooltip";

// STATIC: 0x00C9A2B8
CImVector s_tooltipTitleColor = CImVector(0xFFFFD200);

// STATIC: 0x00C9A2BC
CImVector s_tooltipTextColor = CImVector(0xFFFFFFFF);

// STATIC: 0x00C9A2C0
static CImVector s_tooltipRedColor = CImVector(0xFFFF2020);

// STATIC: 0x00C9A2C4
static CImVector s_tooltipGrayColor = CImVector(0xFF808080);

// STATIC: 0x00C9A2C8
static CImVector s_tooltipDisenchantColor = CImVector(0xFF88AAFF);

// STATIC: 0x00C9A2CC
static CImVector s_tooltipGreenColor = CImVector(0xFF00FF00);

// STATIC: 0x00C9A2D4
static CImVector s_tooltipBrightRedColor = CImVector(0xFFFF0000);

// STATIC: 0x00C9A2D8
static CImVector s_tooltipObjectBackdropColor = CImVector(0xFF002E5A);

// STATIC: 0x00C9A2DC
static CImVector s_tooltipItemSetColor = CImVector(0xFFFFFF97);

// STATIC: 0x00C9A2E0
static CImVector s_skillColors[5] = { CImVector(0xFF808080), CImVector(0xFF40C040), CImVector(0xFFFFFF00), CImVector(0xFFFF8040), CImVector(0xFFFF2020) };

// STATIC: 0x00C9A338
static CImVector s_tooltipGlyphColor = CImVector(0xFF66BBFF);

// STATIC: 0x00C9A358
static CImVector s_tooltipTransmogrifyColor = CImVector(0xFFFF80FF);

// STATIC: 0x00C9A35C
static CImVector s_tooltipRefundColor = CImVector(0xFF00CCFF);

// STATIC: 0x00C9A360
static CImVector s_tooltipBindTradeColor = CImVector(0xFF00CCFF);

// STATIC: 0x00C9A364
static CImVector s_frameStackMouseColors[2] = { CImVector(0xFFFF9966), CImVector(0xFFDDDD88) };

// STATIC: 0x00C9A36C
static CImVector s_frameStackColors[2] = { CImVector(0xFF6699FF), CImVector(0xFF88DDDD) };

// STATIC: 0x00C9A374
static CImVector s_frameStackHiddenColors[2] = { CImVector(0xFF666666), CImVector(0xFF999999) };

// STATIC: 0x00C9A3A0
static const int32_t s_combatRatingStats[26] = { -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 35, 35, 27, 28, 29, 30, -1, -1, -1, 37, 44, 49 };

// STATIC: 0x00C9A488
static const char* s_classificationTokens[6] = { NULL, "ELITE", "ELITE", NULL, NULL, NULL };

// STATIC: 0x00C9A4A0
static const char* s_useAllPowerTokens[11] = { "SPELL_USE_ALL_MANA", "SPELL_USE_ALL_RAGE", "SPELL_USE_ALL_FOCUS", "SPELL_USE_ALL_ENERGY", "", "SPELL_USE_ALL_RUNE", "SPELL_USE_ALL_RUNIC_POWER", "SPELL_USE_ALL_SOUL_SHARDS", "SPELL_USE_ALL_HOLY_POWER", "", NULL };

// STATIC: 0x00C9A4CC
static const char* s_useAllPositiveEnergyToken = "SPELL_USE_ALL_BALANCE_POSITIVE";

// STATIC: 0x00C9A4D0
static const char* s_useAllNegativeEnergyToken = "SPELL_USE_ALL_BALANCE_NEGATIVE";

// STATIC: 0x00C9A4D4
static const char* s_powerCostTokens[11] = { "MANA_COST", "RAGE_COST", "FOCUS_COST", "ENERGY_COST", "", "RUNE_COST", "RUNIC_POWER_COST", "SOUL_SHARDS_COST", "", "HOLY_POWER_COST", "" };

// STATIC: 0x00C9A500
static const char* s_balancePositiveEnergyCostToken = "BALANCE_POSITIVE_ENERGY_COST";

// STATIC: 0x00C9A504
static const char* s_balanceNegativeEnergyCostToken = "BALANCE_NEGATIVE_ENERGY_COST";

// STATIC: 0x00C9A508
static int32_t s_comparisonIndex = 1;

// STATIC: 0x00C9CAD8
const char* s_itemModTokens[57] = { "ITEM_MOD_MANA", "ITEM_MOD_HEALTH", "", "ITEM_MOD_AGILITY", "ITEM_MOD_STRENGTH", "ITEM_MOD_INTELLECT", "ITEM_MOD_SPIRIT", "ITEM_MOD_STAMINA", "", "", "", "", "ITEM_MOD_DEFENSE_SKILL_RATING", "ITEM_MOD_DODGE_RATING", "ITEM_MOD_PARRY_RATING", "ITEM_MOD_BLOCK_RATING", "ITEM_MOD_HIT_MELEE_RATING", "ITEM_MOD_HIT_RANGED_RATING", "ITEM_MOD_HIT_SPELL_RATING", "ITEM_MOD_CRIT_MELEE_RATING", "ITEM_MOD_CRIT_RANGED_RATING", "ITEM_MOD_CRIT_SPELL_RATING", "ITEM_MOD_HIT_TAKEN_MELEE_RATING", "ITEM_MOD_HIT_TAKEN_RANGED_RATING", "ITEM_MOD_HIT_TAKEN_SPELL_RATING", "ITEM_MOD_CRIT_TAKEN_MELEE_RATING", "ITEM_MOD_CRIT_TAKEN_RANGED_RATING", "ITEM_MOD_CRIT_TAKEN_SPELL_RATING", "ITEM_MOD_HASTE_MELEE_RATING", "ITEM_MOD_HASTE_RANGED_RATING", "ITEM_MOD_HASTE_SPELL_RATING", "ITEM_MOD_HIT_RATING", "ITEM_MOD_CRIT_RATING", "ITEM_MOD_HIT_TAKEN_RATING", "ITEM_MOD_CRIT_TAKEN_RATING", "ITEM_MOD_RESILIENCE_RATING", "ITEM_MOD_HASTE_RATING", "ITEM_MOD_EXPERTISE_RATING", "ITEM_MOD_ATTACK_POWER", "ITEM_MOD_RANGED_ATTACK_POWER", "", "ITEM_MOD_SPELL_HEALING_DONE", "ITEM_MOD_SPELL_DAMAGE_DONE", "ITEM_MOD_MANA_REGENERATION", "ITEM_MOD_ARMOR_PENETRATION_RATING", "ITEM_MOD_SPELL_POWER", "ITEM_MOD_HEALTH_REGEN", "ITEM_MOD_SPELL_PENETRATION", "ITEM_MOD_BLOCK_VALUE", "ITEM_MOD_MASTERY_RATING", "ITEM_MOD_EXTRA_ARMOR", "ITEM_MOD_FIRE_RESISTANCE", "ITEM_MOD_FROST_RESISTANCE", "ITEM_MOD_HOLY_RESISTANCE", "ITEM_MOD_SHADOW_RESISTANCE", "ITEM_MOD_NATURE_RESISTANCE", "ITEM_MOD_ARCANE_RESISTANCE" };

// STATIC: 0x00C9CBBC
static const char* s_socketNames[6] = { "Meta", "Red", "Yellow", "Blue", "Hydraulic", "Cogwheel" };

// STATIC: 0x00C9F730
static const char* s_socketIdentifiers[8] = { s_socketNames[0], s_socketNames[1], s_socketNames[2], s_socketNames[3], s_socketNames[4], s_socketNames[5], "Prismatic", "Prismatic" };

// STATIC: 0x00D0B1E0
const char* s_inventoryTypeTokens[29] = { "", "INVTYPE_HEAD", "INVTYPE_NECK", "INVTYPE_SHOULDER", "INVTYPE_BODY", "INVTYPE_CHEST", "INVTYPE_WAIST", "INVTYPE_LEGS", "INVTYPE_FEET", "INVTYPE_WRIST", "INVTYPE_HAND", "INVTYPE_FINGER", "INVTYPE_TRINKET", "INVTYPE_WEAPON", "INVTYPE_SHIELD", "INVTYPE_RANGED", "INVTYPE_CLOAK", "INVTYPE_2HWEAPON", "INVTYPE_BAG", "INVTYPE_TABARD", "INVTYPE_ROBE", "INVTYPE_WEAPONMAINHAND", "INVTYPE_WEAPONOFFHAND", "INVTYPE_HOLDABLE", "INVTYPE_AMMO", "INVTYPE_THROWN", "INVTYPE_RANGEDRIGHT", "INVTYPE_QUIVER", "INVTYPE_RELIC" };

// STATIC: 0x00B95368
static const int32_t s_attackPowerGroup[2] = { 8, 9 };

// STATIC: 0x00B95370
static const int32_t s_manaRegenGroup[1] = { 54 };

// STATIC: 0x00B95374
static const int32_t s_hasteRatingGroup[3] = { 39, 40, 41 };

// STATIC: 0x00B95380
static const int32_t s_critRatingGroup[3] = { 30, 31, 32 };

// STATIC: 0x00B9538C
static const int32_t s_hitRatingGroup[3] = { 27, 28, 29 };

// STATIC: 0x00B95398
static const int32_t s_spellPowerGroup[2] = { 53, 52 };

// STATIC: 0x00B951AC
static const int32_t s_gemColorIndices[5] = { -1, 0, 1, 2, 3 };

// STATIC: 0x00B951C4
static const int32_t s_extendedCostCurrencyFlags[5] = { 0x2, 0x4, 0x8, 0x10, 0x20 };

// STATIC: 0x00B96FB0
static const int32_t s_secondaryStatOrder[45] = { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56 };

// STATIC: 0x00B9740C
static const int32_t s_primaryStatOrder[11] = { 4, 3, 7, 5, 6, 1, 0, 8, 9, 2, 10 };

// STATIC: 0x00B9E6AC
static const int32_t s_socketColors[8] = { 1, 2, 4, 8, 16, 32, 14, 0 };

// STATIC: 0x00D3A7F0
static const ItemSetRec* s_sortItemSet;

// STATIC: 0x00D3A7F8
static char s_itemLink[1024];

// STATIC: 0x00D3ABF8
static CImVector s_threatStatusColors[5] = { CImVector(0xFFFFFFFF), CImVector(0xFFB0B0B0), CImVector(0xFFFFFF77), CImVector(0xFFFF9900), CImVector(0xFFFF0000) };

// STATIC: 0x00D3AC0C
static CImVector s_itemQualityColors[8] = { CImVector(0xFF9D9D9D), CImVector(0xFFFFFFFF), CImVector(0xFF1EFF00), CImVector(0xFF0070DD), CImVector(0xFFA335EE), CImVector(0xFFFF8000), CImVector(0xFFE6CC80), CImVector(0xFFE6CC80) };

// STATIC: 0x00D3AC58
static TSGrowableArray<FrameStackInfo> s_frameStack;

// STATIC: 0x00D3AC70
static const char* s_itemQualityColorStrings[8] = { "ff9d9d9d", "ffffffff", "ff1eff00", "ff0070dd", "ffa335ee", "ffff8000", "ffe6cc80", "ffe6cc80" };

// OFFSET: 0x00464260/0x140077C00
void TooltipExtendedItemInfo::Clear() {
    this->m_itemId = 0;
    memset(this->m_enchants, 0, sizeof(this->m_enchants));
    memset(this->m_enchantDurations, 0, sizeof(this->m_enchantDurations));
    memset(this->m_enchantCharges, 0, sizeof(this->m_enchantCharges));
    this->m_cooldown = 0;
    this->m_proposedEnchant = 0;
    this->m_creator = WOWGUID::Make(0);
    this->m_giftCreator = WOWGUID::Make(0);
    this->m_propertySeed = 0;
    this->m_randomPropertiesId = 0;
    this->m_wrapped = 0;
    this->m_lockId = 0;
    this->m_maxDurability = 0;
    this->m_durability = 0;
    this->m_level = 0;
    this->m_soulbound = 0;
    this->m_spellCharges = -1;
    this->m_stackCount = 1;
    this->m_maxStackCount = -1;
}

// OFFSET: 0x00464300/0x140077CC0
int32_t TooltipExtendedItemInfo::GetItemIdFromLink(const char* link) {
    this->Clear();
    const char* cursor = strstr(link, "item:");

    if (!cursor) {
        return 0;
    }

    cursor += 5;
    this->m_itemId = SStrToInt(cursor);

    if (static_cast<int32_t>(this->m_itemId) <= 0) {
        return 0;
    }

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (*cursor) {
        this->m_enchants[0] = SStrToInt(++cursor);
    }

    for (uint32_t i = 2; i < 5; i++) {
        while (*cursor && *cursor != ':') {
            cursor++;
        }

        if (*cursor) {
            this->m_enchants[i] = SStrToInt(++cursor);
        }
    }

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (!*cursor) {
        return this->m_itemId;
    }

    this->m_enchants[5] = SStrToInt(++cursor);

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (!*cursor) {
        return this->m_itemId;
    }

    this->m_randomPropertiesId = SStrToInt(++cursor);

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (!*cursor) {
        return this->m_itemId;
    }

    this->m_propertySeed = SStrToInt(++cursor);

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (!*cursor) {
        return this->m_itemId;
    }

    this->m_level = SStrToInt(++cursor);

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (!*cursor) {
        return this->m_itemId;
    }

    this->m_enchants[8] = SStrToInt(++cursor);
    return this->m_itemId;
}

// OFFSET: 0x004648F0/0x1400780E0
const char* GetSocketIdentifier(int32_t color) {
    for (uint32_t i = 0; i < 8; i++) {
        if (s_socketColors[i] == color) {
            return s_socketIdentifiers[i];
        }
    }

    return "Unknown";
}

// OFFSET: 0x00464920/0x140078120
FrameScript_Object::ScriptIx* CGTooltip::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!strcmp(name, "ontooltipsetdefaultanchor")) {
        return &this->m_onTooltipSetDefaultAnchor;
    }

    if (!strcmp(name, "ontooltipcleared")) {
        return &this->m_onTooltipCleared;
    }

    if (!strcmp(name, "ontooltipaddmoney")) {
        data.m_wrapper = "return function(self,cost,maxcost) %s end";
        return &this->m_onTooltipAddMoney;
    }

    if (!strcmp(name, "ontooltipsetunit")) {
        return &this->m_onTooltipSetUnit;
    }

    if (!strcmp(name, "ontooltipsetitem")) {
        return &this->m_onTooltipSetItem;
    }

    if (!strcmp(name, "ontooltipsetspell")) {
        return &this->m_onTooltipSetSpell;
    }

    if (!strcmp(name, "ontooltipsetquest")) {
        return &this->m_onTooltipSetQuest;
    }

    if (!strcmp(name, "ontooltipsetachievement")) {
        return &this->m_onTooltipSetAchievement;
    }

    if (!strcmp(name, "ontooltipsetequipmentset")) {
        return &this->m_onTooltipSetEquipmentSet;
    }

    if (!strcmp(name, "ontooltipsetframestack")) {
        return &this->m_onTooltipSetFrameStack;
    }

    return NULL;
}

// OFFSET: 0x00464BC0
void CGTooltip::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetDefaultAnchor, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipCleared, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipAddMoney, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetUnit, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetItem, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetSpell, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetQuest, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetAchievement, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetEquipmentSet, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onTooltipSetFrameStack, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// OFFSET: 0x00464CB0/0x1400783F0
const CImVector* CGTooltip::GetThreatStateColor(uint32_t status) {
    if (status >= 5) {
        status = 1;
    }

    return &s_threatStatusColors[status];
}

// PROVEN: 476d7a82 IDENTICAL/NONE
// OFFSET: 0x00464CD0/0x140078410
const CImVector* CGTooltip::GetItemQualityColor(uint32_t quality) {
    if (quality >= sizeof(s_itemQualityColors) / sizeof(s_itemQualityColors[0])) {
        quality = 1;
    }

    return reinterpret_cast<const CImVector*>(&s_itemQualityColors[quality]);
}

// PROVEN: c6683b2a IDENTICAL/NONE
// OFFSET: 0x00464CF0/0x140078430
const char* CGTooltip::GetItemQualityColorString(uint32_t quality) {
    if (quality >= sizeof(s_itemQualityColorStrings) / sizeof(s_itemQualityColorStrings[0])) {
        quality = 1;
    }

    return s_itemQualityColorStrings[quality];
}

// OFFSET: 0x00464D10/0x140078450
const char* CGTooltip::GetTalentHyperlinkString(const TalentRec* talent, const char* name, int32_t rank) {
    if (!talent || !name) {
        return "";
    }

    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Htalent:%d:%d|h[%s]|h%s", s_talentLinkColor, talent->m_id, rank, name, *s_talentLinkColor ? "|r" : "");
    return s_itemLink;
}

// OFFSET: 0x00464D70
int32_t FormatString(const char* token, int32_t count, FRAMESCRIPT_GENDER gender, char* buffer, uint32_t size) {
    const char* text = FrameScript_GetText(token, count, gender);
    SStrCopy(buffer, text, size);
    return *text != '\0';
}

// PROVEN: e6f9b035 IDENTICAL/NONE
// OFFSET: 0x00464ED0/0x1400784F0
static int32_t QSortFrameStack(const void* a, const void* b) {
    const FrameStackInfo* infoA = static_cast<const FrameStackInfo*>(a);
    const FrameStackInfo* infoB = static_cast<const FrameStackInfo*>(b);

    if (infoA->m_strata < infoB->m_strata) {
        return 1;
    }

    if (infoA->m_strata > infoB->m_strata) {
        return -1;
    }

    if (infoA->m_level < infoB->m_level) {
        return 1;
    }

    if (infoA->m_level > infoB->m_level) {
        return -1;
    }

    return SStrCmpI(infoA->m_name, infoB->m_name, STORM_MAX_STR);
}

// OFFSET: 0x00464F20/0x140078530
static int32_t SortItemSetBonuses(const void* a, const void* b) {
    uint8_t left = *static_cast<const uint8_t*>(a);
    uint8_t right = *static_cast<const uint8_t*>(b);
    int32_t leftThreshold = s_sortItemSet->m_setThreshold[left];
    int32_t rightThreshold = s_sortItemSet->m_setThreshold[right];

    if (leftThreshold > rightThreshold) {
        return 1;
    }

    if (leftThreshold < rightThreshold) {
        return -1;
    }

    return left < right ? -1 : 1;
}

// OFFSET: 0x00464F70
int32_t CGTooltip::GetScalingStatItemBudget(const ItemRecSparse* item, const ScalingStatValuesRec* values) {
    if (!item || !values || !item->m_scalingStatDistribution) {
        return 0;
    }

    switch (item->m_inventoryType) {
        case 1:
        case 5:
        case 7:
        case 17:
        case 20:
            return values->m_statMultiplier[0];

        case 2:
        case 9:
        case 11:
        case 14:
        case 16:
        case 23:
            return values->m_statMultiplier[2];

        case 3:
        case 6:
        case 8:
        case 10:
        case 12:
            return values->m_statMultiplier[1];

        case 13:
        case 21:
        case 22:
            return values->m_statMultiplier[4];

        case 15:
        case 25:
        case 26:
        case 28:
            return values->m_statMultiplier[3];

        default:
            return 0;
    }
}

// OFFSET: 0x00465000
int32_t CGTooltip::GetScalingStatArmor(const ItemRecSparse* item, const ScalingStatValuesRec* values) {
    if (!item) {
        return 0;
    }

    const ItemRec* rec = item->GetNonSparse();

    if (!rec || !values || rec->m_classID != 4 || rec->m_subclassID < 1 || rec->m_subclassID > 4) {
        return 0;
    }

    uint32_t subclass = rec->m_subclassID - 1;

    if (subclass > 3) {
        return 0;
    }

    switch (rec->m_inventoryType) {
        case 1:
            return values->m_armorHead[subclass];

        case 3:
            return values->m_armorShoulder[subclass];

        case 5:
        case 20:
            return values->m_armorChest[subclass];

        case 6:
            return values->m_armorWaist[subclass];

        case 7:
            return values->m_armorLegs[subclass];

        case 8:
            return values->m_armorFeet[subclass];

        case 9:
            return values->m_armorWrist[subclass];

        case 10:
            return values->m_armorHands[subclass];

        case 16:
            return values->m_armorCloak;

        default:
            return 0;
    }
}

// OFFSET: 0x00465220
bool CGTooltip::DurationIsInt(float time, float fraction) {
    float value;

    if (time >= 86400000.0f) {
        value = time * 0.000000011574074f;
    } else if (time >= 3600000.0f) {
        value = time * 0.00000027777779f;
    } else if (time >= 60000.0f) {
        value = time * 0.000016666667f;
    } else {
        value = time * 0.001f;
    }

    return fraction > value - floor(value);
}

// OFFSET: 0x004652A0
void CGTooltip::GetDurationString(char* buffer, uint32_t size, uint64_t time, const char* token, const char* prefix, int32_t roundUp, bool seconds) {
    if (!buffer || !token) {
        return;
    }

    uint32_t minute = seconds ? 60 : 60000;
    int32_t value = static_cast<int32_t>(time);
    char key[256];
    *buffer = '\0';

    if (time >= 1440 * minute) {
        uint64_t day = 1440 * minute;
        value = static_cast<int32_t>(roundUp ? (time - 1) / day + 1 : time / day);
        SStrPrintf(key, sizeof(key), "%s_DAYS", token);
    } else if (time >= 60 * minute) {
        uint64_t hour = 60 * minute;
        uint64_t hours = roundUp ? (time - 1) / hour + 1 : time / hour;
        value = static_cast<int32_t>(hours);

        if (hours == 24) {
            value = 1;
            SStrPrintf(key, sizeof(key), "%s_DAYS", token);
        } else {
            SStrPrintf(key, sizeof(key), "%s_HOURS", token);
        }
    } else if (static_cast<uint32_t>(time) >= minute) {
        uint64_t minutes = roundUp ? (time - 1) / minute + 1 : time / minute;
        value = static_cast<int32_t>(minutes);

        if (minutes == 60) {
            value = 1;
            SStrPrintf(key, sizeof(key), "%s_HOURS", token);
        } else {
            SStrPrintf(key, sizeof(key), "%s_MIN", token);
        }
    } else {
        if (!seconds) {
            value = static_cast<int32_t>(time / 1000);
        }

        SStrPrintf(key, sizeof(key), "%s_SEC", token);
    }

    if (prefix) {
        SStrPrintf(buffer, size, FrameScript_GetText(key, -1, GENDER_NOT_APPLICABLE), prefix, value);
    } else {
        SStrPrintf(buffer, size, FrameScript_GetText(key, -1, GENDER_NOT_APPLICABLE), value);
    }
}

// OFFSET: 0x004654B0
void CGTooltip::GetFloatDurationString(char* buffer, uint32_t size, float time, const char* token, const char* prefix) {
    if (!buffer || !token) {
        return;
    }

    // STATIC: 0x00D3ACA8
    static const float s_hour = 3600000.0f;
    // STATIC: 0x00D3ACA4
    static const float s_day = s_hour * 24.0f;

    *buffer = '\0';
    float value;
    const char* format;

    if (time >= s_day) {
        value = time / s_day;
        format = "%s_DAYS";
    } else if (time >= s_hour) {
        format = "%s_HOURS";
        value = time / s_hour;
    } else if (time >= 60000.0f) {
        value = time * 0.000016666667f;
        format = "%s_MIN";
    } else {
        value = time * 0.001f;
        format = "%s_SEC";
    }

    char key[256];
    SStrPrintf(key, sizeof(key), format, token);

    if (value <= 0.0f) {
        value = 0.0f;
    }

    if (prefix) {
        SStrPrintf(buffer, size, FrameScript_GetText(key, -1, GENDER_NOT_APPLICABLE), prefix, static_cast<double>(value));
    } else {
        SStrPrintf(buffer, size, FrameScript_GetText(key, -1, GENDER_NOT_APPLICABLE), static_cast<double>(value));
    }
}

// OFFSET: 0x00465620
void CGTooltip::ResetPosition(int32_t force) {
    if (!this->m_owner || this->m_anchorType == ANCHOR_PRESERVE) {
        return;
    }

    if (force || this->m_anchorType != ANCHOR_NONE) {
        this->MarkFramePointsUnused();
    }

    switch (this->m_anchorType) {
        case ANCHOR_LEFT:
            this->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_owner, FRAMEPOINT_TOPLEFT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_RIGHT:
            this->SetPoint(FRAMEPOINT_BOTTOMLEFT, this->m_owner, FRAMEPOINT_TOPRIGHT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_BOTTOMLEFT:
            this->SetPoint(FRAMEPOINT_TOPRIGHT, this->m_owner, FRAMEPOINT_BOTTOMLEFT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_BOTTOM:
            this->SetPoint(FRAMEPOINT_TOP, this->m_owner, FRAMEPOINT_BOTTOM, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_BOTTOMRIGHT:
            this->SetPoint(FRAMEPOINT_TOPLEFT, this->m_owner, FRAMEPOINT_BOTTOMRIGHT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_TOPLEFT:
            this->SetPoint(FRAMEPOINT_BOTTOMLEFT, this->m_owner, FRAMEPOINT_TOPLEFT, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_TOP:
            this->SetPoint(FRAMEPOINT_BOTTOM, this->m_owner, FRAMEPOINT_TOP, this->m_offsetX, this->m_offsetY, 1);
            break;

        case ANCHOR_TOPRIGHT:
            this->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_owner, FRAMEPOINT_TOPRIGHT, this->m_offsetX, this->m_offsetY, 1);
            break;

        default:
            break;
    }
}

// PROVEN: 4a3f42d4 IDENTICAL/NONE
// OFFSET: 0x004658B0/0x140078E80
void CGTooltip::FadeOut() {
    if (this->m_anchorType == ANCHOR_CURSOR || this->m_anchorType == ANCHOR_CURSOR_RIGHT) {
        this->HideThis();
        this->m_fading = 0;
    } else {
        this->m_fading = 1;
        this->m_fadeTime = 2.0f;
    }
}

// OFFSET: 0x00465900
void CGTooltip::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (this->m_anchorType == ANCHOR_CURSOR) {
        float x = 0.0f;
        float y = 0.0f;
        NDCToDDC(this->m_simpleTop->m_mousePosition.m_x, this->m_simpleTop->m_mousePosition.m_y, &x, &y);
        float scale = 1.0f / this->m_layoutScale;
        y *= scale;
        x *= scale;
        this->SetPoint(FRAMEPOINT_BOTTOM, CSimpleTop::s_instance, FRAMEPOINT_BOTTOMLEFT, x, y, 1);
    } else if (this->m_anchorType == ANCHOR_CURSOR_RIGHT) {
        float x = 0.0f;
        float y = 0.0f;
        NDCToDDC(this->m_simpleTop->m_mousePosition.m_x, this->m_simpleTop->m_mousePosition.m_y, &x, &y);
        float scale = 1.0f / this->m_layoutScale;
        y *= scale;
        x *= scale;
        this->SetPoint(FRAMEPOINT_BOTTOMLEFT, CSimpleTop::s_instance, FRAMEPOINT_BOTTOMLEFT, this->m_offsetX + x, this->m_offsetY + y, 1);
    }

    if (this->m_fading) {
        float alpha = this->m_fadeTime - elapsedSec;
        this->m_fadeTime = alpha;

        if (alpha > 0.0f) {
            if (alpha >= 1.0f) {
                alpha = 1.0f;
            }

            this->SetFrameAlpha(static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f)));
        } else {
            this->SetFrameAlpha(255);
            this->Hide();
        }
    }
}

// PROVEN: a78fb364 IDENTICAL/NONE
// OFFSET: 0x00465B30/0x140079140
void __stdcall CGTooltip::FormatStringForXML(const char* text, char* buffer) {
    int32_t length = strlen(text);
    const char* processed = LanguageProcess(text);
    int32_t out = 0;

    for (int32_t i = 0; i < length; i++) {
        char c = processed[i];

        if (c == '<') {
            buffer[out++] = '&';
            buffer[out++] = 'l';
            buffer[out++] = 't';
            buffer[out++] = ';';
            i++;
        } else if (c == '>') {
            buffer[out++] = '&';
            buffer[out++] = 'g';
            buffer[out++] = 't';
            buffer[out++] = ';';
            i++;
        } else if (c == '&') {
            buffer[out++] = '&';
            buffer[out++] = 'a';
            buffer[out++] = 'm';
            buffer[out++] = 'p';
            buffer[out++] = ';';
            i++;
        }

        buffer[out++] = processed[i];
    }

    buffer[out] = '\0';
}

// OFFSET: 0x00465BD0/0x140079240
void CGItemStatsSummary::Clear() {
    memset(this->m_stats, 0, sizeof(this->m_stats));
    this->m_dps = 0.0f;
    this->m_flags = 0;
}

// OFFSET: 0x00465C00
void CGItemStatsSummary::AddStatMod(int32_t stat, int32_t amount) {
    if (stat == 50) {
        this->m_stats[0] = amount;
        return;
    }

    this->m_stats[stat + 11] += amount;

    if (stat == 38) {
        this->m_stats[8] += amount;
        this->m_stats[9] += amount;
    } else if (stat == 39) {
        this->m_stats[9] += amount;
    }
}

// OFFSET: 0x00465C40
void CGItemStatsSummary::GetSocketFlags(const ItemRecSparse* item) {
    this->m_flags |= 0x10;

    for (uint32_t i = 0; i < 3; i++) {
        int32_t color = item->m_socketColor[i];

        if (!color) {
            continue;
        }

        for (uint32_t j = 0; j < 8; j++) {
            if (color == s_socketColors[j]) {
                this->m_stats[j + 81]++;
                break;
            }
        }
    }
}

// PROVEN: 58d0cfe9 IDENTICAL/NONE
// OFFSET: 0x00465CA0
void CGItemStatsSummary::MakeStatNamesPretty(int32_t stat, const int32_t* group, uint32_t count, int32_t merge) {
    int32_t value = this->m_stats[stat];
    int32_t previous = 0;

    for (uint32_t i = 0; i < count; i++) {
        this->m_stats[group[i]] += value;
        merge = merge && (!i || this->m_stats[group[i]] == previous);
        previous = this->m_stats[group[i]];
    }

    if (merge) {
        this->m_stats[stat] = this->m_stats[group[0]];

        for (uint32_t i = 0; i < count; i++) {
            this->m_stats[group[i]] = 0;
        }
    } else {
        this->m_stats[stat] = 0;
    }
}

// OFFSET: 0x00465D50
void CGItemStatsSummary::CleanStatNames() {
    if (this->m_stats[8] == this->m_stats[10]) {
        this->m_stats[10] = 0;
    }

    this->MakeStatNamesPretty(56, s_spellPowerGroup, 2, 1);
    this->MakeStatNamesPretty(42, s_hitRatingGroup, 3, 1);
    this->MakeStatNamesPretty(43, s_critRatingGroup, 3, 1);
    this->MakeStatNamesPretty(47, s_hasteRatingGroup, 3, 1);
    this->MakeStatNamesPretty(69, s_manaRegenGroup, 1, 1);
    this->MakeStatNamesPretty(7, s_attackPowerGroup, 2, 1);
}

// OFFSET: 0x00466060
void CGTooltip::AddMoney(int32_t money, int32_t maxMoney) {
    if (!this->HasScript(this->m_onTooltipAddMoney)) {
        return;
    }

    lua_State* L = FrameScript_GetContext();
    lua_pushnumber(L, money);

    if (maxMoney <= 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, maxMoney);
    }

    this->RunScript(this->m_onTooltipAddMoney, 2, NULL);
}

// OFFSET: 0x004660F0/0x1400797F0
void CGTooltip::RunOnTooltipSetUnit() {
    if (this->HasScript(this->m_onTooltipSetUnit)) {
        this->RunScript(this->m_onTooltipSetUnit, 0, NULL);
    }
}

// PROVEN: 3696c42e IDENTICAL/NONE
// OFFSET: 0x00466200/0x1400798A0
static const char* GetColorblindSuffix(int32_t index) {
    CVar* colorblindMode = CVar::Lookup("colorblindMode");

    if (colorblindMode && colorblindMode->m_intValue) {
        return s_colorblindSuffixes[index];
    }

    return "";
}

// OFFSET: 0x00466230/0x1400798E0
static const CImVector* GetSkillColor(int32_t required, int32_t current, const char** suffix) {
    const CImVector* color = &s_skillColors[4];
    int32_t index = 4;

    if (current >= required + 100) {
        index = 0;
        color = &s_skillColors[0];
    } else if (current >= required + 50) {
        index = 1;
        color = &s_skillColors[1];
    } else if (current >= required + 25) {
        index = 2;
        color = &s_skillColors[2];
    } else if (current >= required) {
        index = 3;
        color = &s_skillColors[3];
    }

    CVar* colorblindMode = CVar::Lookup("colorblindMode");

    if (colorblindMode && colorblindMode->m_intValue) {
        *suffix = s_colorblindSuffixes[index];
    } else {
        *suffix = "";
    }

    return color;
}

// OFFSET: 0x004662C0/0x1400799B0
static int32_t HealthUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(param);
        statusBar->SetMinMaxValues(0.0f, static_cast<float>(static_cast<int32_t>(unit->m_unit->m_unitFieldMaxhealth)));
        statusBar->SetValue(static_cast<float>(static_cast<int32_t>(unit->m_unit->m_unitFieldHealth)));
    }

    return 1;
}

// OFFSET: 0x00466330/0x140079A30
static int32_t ObjHealthUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (object) {
        CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(param);
        statusBar->SetMinMaxValues(0.0f, 1.0f);
        statusBar->SetValue(static_cast<float>(object->m_gameObject->m_animProgress) * 0.0039215689f);
    }

    return 1;
}

// OFFSET: 0x00466410/0x140079B50
const char* CGTooltip::GetSpellHyperlinkString(int32_t spellId) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return "";
    }

    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hspell:%d|h[%s]|h%s", s_spellLinkColor, spellId, static_cast<const char*>(spell->m_name), *s_spellLinkColor ? "|r" : "");
    return s_itemLink;
}

// OFFSET: 0x00466480/0x140079BF0
const char* CGTooltip::GetAchievementHyperlink(int32_t achievementId, WOWGUID guid, bool completed, WowTime time, uint32_t criteria1, uint32_t criteria2, uint32_t criteria3, uint32_t criteria4) {
    AchievementRec* achievement = g_achievementDB.GetRecord(achievementId);

    if (!achievement) {
        return "";
    }

    char name[800];
    QuestParserParseText(achievement->m_title, name, sizeof(name), guid, NULL, 0);
    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hachievement:%d:%016llX:%d:%d:%d:%d:%u:%u:%u:%u|h[%s]|h%s", s_achievementLinkColor, achievementId, guid.m_guid, static_cast<uint8_t>(completed), time.m_month + 1, time.m_monthDay + 1, time.m_year, criteria1, criteria2, criteria3, criteria4, name, "|r");
    return s_itemLink;
}

// OFFSET: 0x00466540/0x140079D50
const char* CGTooltip::GetGlyphHyperlinkString(int32_t glyphId) {
    GlyphPropertiesRec* glyph = g_glyphPropertiesDB.GetRecord(glyphId);

    if (!glyph) {
        return "";
    }

    SpellRec* spell = g_spellDB.GetRecord(glyph->m_spellID);

    if (!spell) {
        return "";
    }

    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hglyph:%d|h[%s]|h%s", s_glyphLinkColor, glyphId, static_cast<const char*>(spell->m_name), *s_glyphLinkColor ? "|r" : "");
    return s_itemLink;
}

// OFFSET: 0x004665D0/0x140079E30
const char* CGTooltip::GetInstanceLockHyperlink(int32_t mapId, int32_t difficulty, uint32_t encounterMask) {
    MapRec* map = g_mapDB.GetRecord(mapId);

    if (!map) {
        return "";
    }

    const char* color = s_instanceLockLinkColor;
    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hinstancelock:%016llX:%d:%d:%d|h[%s]|h%s", color, ClntObjMgrGetActivePlayer().m_guid, mapId, difficulty, encounterMask, static_cast<const char*>(map->m_mapName), *color ? "|r" : "");
    return s_itemLink;
}

// OFFSET: 0x00466650/0x140079F30
int32_t GetEnchantmentConditionText(CGPlayer_C* player, const SpellItemEnchantmentRec* enchant, char* buffer, uint32_t size, int32_t newline) {
    *buffer = '\0';

    if (!enchant || !enchant->m_srcItemID || !enchant->m_conditionID || !player) {
        return 1;
    }

    SpellItemEnchantmentConditionRec* condition = g_spellItemEnchantmentConditionDB.GetRecord(enchant->m_conditionID);

    if (!condition) {
        return 1;
    }

    int32_t allMet = 1;

    for (uint32_t i = 0; i < 5; i++) {
        char line[1024];
        char rightName[1024];
        char text[1024];
        char leftName[1024];
        int32_t met = 1;
        const char* color;

        if (newline || *buffer) {
            SStrPack(buffer, "\n", size);
        }

        uint8_t leftType = condition->m_ltOperandType[i];
        uint8_t rightType = condition->m_rtOperandType[i];

        if (rightType) {
            uint32_t leftIndex = s_gemColorIndices[leftType];
            int32_t left = leftIndex >= 6 ? 0 : player->m_gemCounts[leftIndex];
            uint32_t rightIndex = s_gemColorIndices[rightType];
            int32_t right = rightIndex >= 6 ? 0 : player->m_gemCounts[rightIndex];

            if (leftType >= 5) {
                FormatString("UNKNOWN", -1, GENDER_NOT_APPLICABLE, leftName, sizeof(leftName));
            } else {
                FormatString(s_gemColorTokens[leftType], -1, GENDER_NOT_APPLICABLE, leftName, sizeof(leftName));
            }

            if (rightType >= 5) {
                FormatString("UNKNOWN", -1, GENDER_NOT_APPLICABLE, rightName, sizeof(rightName));
            } else {
                FormatString(s_gemColorTokens[rightType], -1, GENDER_NOT_APPLICABLE, rightName, sizeof(rightName));
            }

            switch (condition->m_operator[i]) {
                case 0:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_EQUAL_COMPARE", -1, GENDER_NOT_APPLICABLE), leftName, rightName);
                    met = left == right;
                    break;

                case 1:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_NOT_EQUAL_COMPARE", -1, GENDER_NOT_APPLICABLE), leftName, rightName);
                    met = left != right;
                    break;

                case 2:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_MORE_COMPARE", -1, GENDER_NOT_APPLICABLE), rightName, leftName);
                    met = right > left;
                    break;

                case 3:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_MORE_COMPARE", -1, GENDER_NOT_APPLICABLE), leftName, rightName);
                    met = left > right;
                    break;

                case 4:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_MORE_EQUAL_COMPARE", -1, GENDER_NOT_APPLICABLE), rightName, leftName);
                    met = right >= left;
                    break;

                case 5:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_MORE_EQUAL_COMPARE", -1, GENDER_NOT_APPLICABLE), leftName, rightName);
                    met = left >= right;
                    break;

                default:
                    break;
            }

            color = met ? s_tooltipTextColorString : s_tooltipGrayColorString;
        } else {
            int32_t value = condition->m_rtOperand[i];
            uint32_t leftIndex = s_gemColorIndices[leftType];
            int32_t left = leftIndex >= 6 ? 0 : player->m_gemCounts[leftIndex];

            if (leftType >= 5) {
                FormatString("UNKNOWN", -1, GENDER_NOT_APPLICABLE, leftName, sizeof(leftName));
            } else {
                FormatString(s_gemColorTokens[leftType], -1, GENDER_NOT_APPLICABLE, leftName, sizeof(leftName));
            }

            switch (condition->m_operator[i]) {
                case 0:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_EQUAL_VALUE", -1, GENDER_NOT_APPLICABLE), value, leftName);
                    met = left == value;
                    break;

                case 1:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_NOT_EQUAL_VALUE", -1, GENDER_NOT_APPLICABLE), value, leftName);
                    met = left != value;
                    break;

                case 2:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_LESS_VALUE", -1, GENDER_NOT_APPLICABLE), value, leftName);
                    met = left < value;
                    break;

                case 3:
                    value++;
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_MORE_VALUE", -1, GENDER_NOT_APPLICABLE), value, leftName);
                    met = left >= value;
                    break;

                case 4:
                    value++;
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_LESS_VALUE", -1, GENDER_NOT_APPLICABLE), value, leftName);
                    met = left < value;
                    break;

                case 5:
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ENCHANT_CONDITION_MORE_VALUE", -1, GENDER_NOT_APPLICABLE), value, leftName);
                    met = left >= value;
                    break;

                default:
                    break;
            }

            color = met ? s_tooltipTextColorString : s_tooltipGrayColorString;
        }

        SStrPrintf(line, sizeof(line), "  |c%s%s%s%s", color, FrameScript_GetText("ENCHANT_CONDITION_REQUIRES", -1, GENDER_NOT_APPLICABLE), text, "|r");
        SStrPack(buffer, line, size);

        if (!met || !allMet) {
            allMet = 0;
        } else {
            allMet = 1;
        }

        if (!condition->m_logic[i]) {
            break;
        }
    }

    return allMet;
}

// OFFSET: 0x00466C70/0x14007A7F0
void CGTooltip::GetItemDurationString(char* buffer, uint32_t size, uint64_t time, const char* token) {
    if (!buffer || !token) {
        return;
    }

    *buffer = '\0';
    int32_t units = 0;
    char part[256] = { 0 };
    char text[1024] = { 0 };

    if (time >= 86400000) {
        SStrPrintf(part, sizeof(part), FrameScript_GetText("DAYS_ABBR", -1, GENDER_NOT_APPLICABLE), static_cast<uint32_t>(time) / 86400000);
        SStrPack(text, part, sizeof(text));
        time %= 86400000;
        units = 1;
    }

    if (time >= 3600000) {
        if (text[0]) {
            SStrPack(text, FrameScript_GetText("TIME_UNIT_DELIMITER", -1, GENDER_NOT_APPLICABLE), sizeof(text));
        }

        SStrPrintf(part, sizeof(part), FrameScript_GetText("HOURS_ABBR", -1, GENDER_NOT_APPLICABLE), static_cast<uint32_t>(time) / 3600000);
        SStrPack(text, part, sizeof(text));
        time %= 3600000;
        units++;
    }

    if (units < 2) {
        if (time >= 60000) {
            if (text[0]) {
                SStrPack(text, FrameScript_GetText("TIME_UNIT_DELIMITER", -1, GENDER_NOT_APPLICABLE), sizeof(text));
            }

            SStrPrintf(part, sizeof(part), FrameScript_GetText("MINUTES_ABBR", -1, GENDER_NOT_APPLICABLE), static_cast<uint32_t>(time) / 60000);
            SStrPack(text, part, sizeof(text));
            time %= 60000;
            units++;
        }

        if (units < 2 && time) {
            if (text[0]) {
                SStrPack(text, FrameScript_GetText("TIME_UNIT_DELIMITER", -1, GENDER_NOT_APPLICABLE), sizeof(text));
            }

            SStrPrintf(part, sizeof(part), FrameScript_GetText("SECONDS_ABBR", -1, GENDER_NOT_APPLICABLE), static_cast<uint32_t>(time / 1000));
            SStrPack(text, part, sizeof(text));
        }
    }

    SStrPrintf(buffer, size, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), text);
}

// OFFSET: 0x004670F0/0x14007AD20
void CGTooltip::GetSummonedByString(const CGUnit_C* unit, char* buffer, uint32_t size) {
    if (!unit || !buffer) {
        return;
    }

    WOWGUID owner = *unit->GetCharmerOrCreatorGuid();

    if (!owner) {
        return;
    }

    CGUnit_C* ownerUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(owner, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (ownerUnit && *ownerUnit->GetCharmerOrCreatorGuid()) {
        owner = *ownerUnit->GetCharmerOrCreatorGuid();
    }

    char token[1024];
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(unit->m_unit->m_unitCreatedBySpell, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 28) {
            continue;
        }

        SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);

        if (properties && properties->m_title != -1) {
            if (properties->m_title) {
                SStrPrintf(token, sizeof(token), "UNITNAME_SUMMON_TITLE%d", properties->m_title);
                SStrPrintf(buffer, size, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), GetCombatLogObjectName(owner));
            }

            return;
        }

        break;
    }

    if (const_cast<CGUnit_C*>(unit)->GetCreatureType() == 1) {
        SStrPrintf(token, sizeof(token), "UNITNAME_SUMMON_TITLE1");
    } else {
        SStrPrintf(token, sizeof(token), "UNITNAME_SUMMON_TITLE3");
    }

    SStrPrintf(buffer, size, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), GetCombatLogObjectName(owner));
}

// OFFSET: 0x004672C0/0x14007AF00
void CGTooltip::ClearTooltip() {
    if (this->m_healthGuid) {
        if (this->m_unitGuid) {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), HealthUpdateHandler, this->m_statusBar);
        } else {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_GAMEOBJECT, offsetof(CGGameObjectData, m_animProgress), ObjHealthUpdateHandler, this->m_statusBar);
        }

        this->m_statusBar->Hide();
    }

    this->m_unitGuid = WOWGUID::Make(0);
    this->m_objectGuid = WOWGUID::Make(0);
    this->m_unitAuraGuid = WOWGUID::Make(0);
    this->m_corpseGuid = WOWGUID::Make(0);
    this->m_healthGuid = WOWGUID::Make(0);
    this->m_itemId = 0;
    this->m_currencyId = 0;
    this->m_spellId = 0;
    this->m_questId = 0;
    this->m_achievementId = 0;
    this->m_nextRankSpellId = 0;

    for (uint32_t i = 0; i < this->m_numTextures; i++) {
        uint32_t line = this->m_textureLines[i];
        CSimpleFontString** left = &this->m_leftStrings.m_data[line];
        this->m_leftStrings.m_data[line]->SetPoint(FRAMEPOINT_TOPLEFT, left[-1], FRAMEPOINT_BOTTOMLEFT, 0.0f, NDCToDDCWidth(-2.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 1);

        if (line < this->m_maxLines - 1 && this->m_leftStrings.m_data[line + 1]) {
            CSimpleFontString** strings = &this->m_leftStrings.m_data[line];
            strings[1]->SetPoint(FRAMEPOINT_TOPLEFT, strings[0], FRAMEPOINT_BOTTOMLEFT, 0.0f, 0.0f, 1);
        }

        this->m_textures[i]->Hide();
    }

    bool hadLines = this->m_numLines > 0;
    this->m_numTextures = 0;

    if (this->m_numLines) {
        for (uint32_t i = 0; i < this->m_numLines; i++) {
            this->m_leftStrings.m_data[i]->SetWidth(0.0f);
            this->m_leftStrings.m_data[i]->SetText("", 1);
            this->m_leftStrings.m_data[i]->Hide();
            this->m_rightStrings.m_data[i]->SetText("", 1);
            this->m_rightStrings.m_data[i]->Hide();
            this->m_lineWrap.m_data[i] = 0;
        }

        this->m_numLines = 0;
    }

    if (!this->m_minimumWidthForce) {
        this->m_minimumWidth = 0.0f;
    }

    if (hadLines && this->HasScript(this->m_onTooltipCleared)) {
        this->RunScript(this->m_onTooltipCleared, 0, NULL);
    }
}

// OFFSET: 0x00467580/0x14007B2B0
void CGTooltip::AddTexture(const char* texture, const CAaRect& coords, const CImVector& color) {
    uint32_t index = this->m_numTextures;

    if (index >= 10 || !this->m_textures[index]) {
        return;
    }

    uint32_t lines = this->m_numLines;

    if (lines < 2) {
        return;
    }

    uint32_t line = lines - 1;
    float offset;

    if (index && this->m_textureLines[index - 1] == line - 1) {
        offset = 0.0f;
    } else {
        offset = this->m_textures[index]->GetWidth() + 0.0049999999f;
    }

    CSimpleFontString** strings = &this->m_leftStrings.m_data[line];
    strings[0]->SetPoint(FRAMEPOINT_TOPLEFT, strings[-1], FRAMEPOINT_BOTTOMLEFT, offset, NDCToDDCWidth(-2.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 1);
    this->m_textures[this->m_numTextures]->MarkFramePointsUnused();
    this->m_textures[this->m_numTextures]->SetPoint(FRAMEPOINT_TOPRIGHT, this->m_leftStrings.m_data[line], FRAMEPOINT_TOPLEFT, -0.0049999999f, 0.0f, 1);

    if (line < this->m_maxLines - 1) {
        CSimpleFontString** next = &this->m_leftStrings.m_data[line];

        if (next[1]) {
            CSimpleTexture* current = this->m_textures[this->m_numTextures];
            float y = NDCToDDCWidth(-2.0f / (CoordinateGetAspectCompensation() * 1024.0f));
            next[1]->SetPoint(FRAMEPOINT_TOPLEFT, next[0], FRAMEPOINT_BOTTOMLEFT, -(current->GetWidth() + 0.0049999999f), y, 1);
        }
    }

    this->m_textures[this->m_numTextures]->SetTexture(texture, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    this->m_textures[this->m_numTextures]->SetTexCoord(reinterpret_cast<const CRect&>(coords));
    this->m_textures[this->m_numTextures]->SetVertexColor(color);
    CSimpleTexture* shown = this->m_textures[this->m_numTextures];
    shown->Show();
    this->m_textureLines[this->m_numTextures++] = line;
}

// OFFSET: 0x004677C0/0x14007B5D0
void CGTooltip::CalculateSize() {
    if (!this->m_owner) {
        return;
    }

    float width = NDCToDDCWidth(this->m_minimumWidth / (CoordinateGetAspectCompensation() * 1024.0f));

    if (!this->m_minimumWidthForce) {
        for (uint32_t i = 0; i < this->m_numLines; i++) {
            if (this->m_lineWrap.m_data[i]) {
                continue;
            }

            float lineWidth = 0.0f;

            if (this->m_leftStrings.m_data[i]->m_shown && this->m_rightStrings.m_data[i]->m_shown) {
                lineWidth = NDCToDDCWidth(38.400002f / (CoordinateGetAspectCompensation() * 1024.0f));
            }

            if (this->m_leftStrings.m_data[i]->m_shown) {
                lineWidth = this->m_leftStrings.m_data[i]->GetWidth() + lineWidth;
            }

            if (this->m_rightStrings.m_data[i]->m_shown) {
                lineWidth = this->m_rightStrings.m_data[i]->GetWidth() + lineWidth;
            }

            for (uint32_t j = 0; j < this->m_numTextures; j++) {
                if (this->m_textureLines[j] == i) {
                    lineWidth = this->m_textures[j]->GetWidth() + lineWidth + 0.0049999999f;
                    break;
                }
            }

            if (lineWidth > width) {
                width = lineWidth;
            }
        }
    }

    for (uint32_t i = 0; i < this->m_numLines; i++) {
        if (!this->m_minimumWidthForce && !this->m_lineWrap.m_data[i]) {
            continue;
        }

        float stringWidth = this->m_leftStrings.m_data[i]->GetStringWidth();

        if (NDCToDDCWidth(230.39999f / (CoordinateGetAspectCompensation() * 1024.0f)) <= stringWidth) {
            stringWidth = NDCToDDCWidth(230.39999f / (CoordinateGetAspectCompensation() * 1024.0f));
        }

        if (width <= stringWidth) {
            width = stringWidth;
        }

        this->m_leftStrings.m_data[i]->SetWidth(width);
    }

    for (uint32_t i = 0; i < this->m_numLines; i++) {
        float offset = width;

        if (!this->m_rightStrings.m_data[i]->m_shown) {
            continue;
        }

        for (uint32_t j = 0; j < this->m_numTextures; j++) {
            if (this->m_textureLines[j] == i) {
                offset = width - this->m_textures[j]->GetWidth() - 0.0049999999f;
                break;
            }
        }

        this->m_rightStrings.m_data[i]->SetPoint(FRAMEPOINT_RIGHT, this->m_leftStrings.m_data[i], FRAMEPOINT_LEFT, offset, 0.0f, 1);
    }

    float height = 0.0f;

    for (uint32_t i = 0; i < this->m_numLines; i++) {
        if (!this->m_leftStrings.m_data[i]->m_shown) {
            continue;
        }

        if (height != 0.0f) {
            height = NDCToDDCWidth(2.0f / (CoordinateGetAspectCompensation() * 1024.0f)) + height;
        }

        height = this->m_leftStrings.m_data[i]->GetHeight() + height;
    }

    float borderX = NDCToDDCWidth(10.24f / (CoordinateGetAspectCompensation() * 1024.0f));
    float totalWidth = borderX + borderX + this->m_padding + width;
    float borderY = NDCToDDCWidth(10.24f / (CoordinateGetAspectCompensation() * 1024.0f));
    float totalHeight = borderY + borderY + height;
    this->SetWidth(totalWidth);
    this->SetHeight(totalHeight);

    if (this->m_anchorType != ANCHOR_CURSOR && this->m_anchorType != ANCHOR_CURSOR_RIGHT) {
        this->ResetPosition(0);
    }

    this->Resize(0);
}

// OFFSET: 0x00467BD0/0x14007BA90
void CGTooltip::ShowThis(bool loadPending) {
    if (!this->m_owner || !this->m_numLines) {
        this->Hide();
    }

    this->SetFrameAlpha(255);
    this->m_fading = 0;
    this->CalculateSize();
    CSimpleFrame::ShowThis(false);
}

// OFFSET: 0x004688B0
void CGItemStatsSummary::ParseSpellRec(const SpellRec* spell) {
    if (!(spell->m_attributes & 0x40)) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 6) {
            continue;
        }

        int32_t value = effect->m_effectBasePoints;
        int32_t misc = effect->m_effectMiscValue;

        switch (effect->m_effectAura) {
            case 13:
                this->m_stats[53] += value;
                break;

            case 22:
                for (uint32_t r = 0; r < 7; r++) {
                    if (misc & (1 << r)) {
                        this->m_stats[r] += value;
                    }
                }

                break;

            case 85:
                this->m_stats[misc + 69] += value;
                break;

            case 99:
                this->m_stats[8] += value;
                break;

            case 123:
                this->m_stats[58] -= value;
                break;

            case 124:
                this->m_stats[9] += value;
                break;

            case 135:
                this->m_stats[52] += value;
                break;

            case 161:
                this->m_stats[80] += value;
                break;

            case 189:
                for (int32_t r = 0; r < 26; r++) {
                    if ((misc & (1 << r)) && s_combatRatingStats[r] != -1) {
                        this->m_stats[s_combatRatingStats[r] + 11] += value;
                    }
                }

                break;

            default:
                break;
        }
    }
}

// OFFSET: 0x00468B30
void CGItemStatsSummary::ScanEnchantSpells(const ItemRecSparse* item) {
    this->m_flags |= 0x1;

    for (uint32_t i = 0; i < 5; i++) {
        if (item->m_spellID[i] <= 0) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(item->m_spellID[i]);

        if (spell) {
            this->ParseSpellRec(spell);
        }
    }
}

// OFFSET: 0x00468BA0/0x14007CCF0
CGItemStatsSummary CGItemStatsSummary::operator-(const CGItemStatsSummary& other) const {
    CGItemStatsSummary result;
    result.Clear();

    for (uint32_t i = 0; i < sizeof(this->m_stats) / sizeof(this->m_stats[0]); i++) {
        result.m_stats[i] = this->m_stats[i] - other.m_stats[i];
    }

    result.m_dps = this->m_dps - other.m_dps;
    result.CleanStatNames();
    return result;
}

// OFFSET: 0x00468C20/0x14007CD80
char* CGItemStatsSummary::GetStatIntGlobalString(uint32_t stat, char* buffer, uint32_t size) {
    if (stat - 11 <= 56) {
        SStrPrintf(buffer, size, "%s%s", s_itemModTokens[stat - 11], "_SHORT");
        return buffer;
    }

    if (stat < 7) {
        SStrPrintf(buffer, size, "%s%d%s", "RESISTANCE", stat, "_NAME");
        return buffer;
    }

    if (stat - 81 <= 7) {
        char color[64];
        SStrCopy(color, s_socketIdentifiers[stat - 81], sizeof(color));
        _strupr(color);
        SStrPrintf(buffer, size, "%s%s", "EMPTY_SOCKET_", color);
        return buffer;
    }

    if (stat - 69 <= 10) {
        SStrPrintf(buffer, size, "%s%d%s", "ITEM_MOD_POWER_REGEN", stat - 69, "_SHORT");
        return buffer;
    }

    switch (stat) {
        case 7:
            SStrCopy(buffer, "ITEM_MOD_ATTACK_POWER_SHORT", size);
            break;

        case 8:
            SStrCopy(buffer, "ITEM_MOD_MELEE_ATTACK_POWER_SHORT", size);
            break;

        case 9:
            SStrCopy(buffer, "ITEM_MOD_RANGED_ATTACK_POWER_SHORT", size);
            break;

        case 10:
            SStrCopy(buffer, "ITEM_MOD_FERAL_ATTACK_POWER_SHORT", size);
            break;

        case 68:
            SStrCopy(buffer, "ITEM_MOD_BLOCK_VALUE_SHORT", size);
            break;

        case 80:
            SStrCopy(buffer, "ITEM_MOD_HEALTH_REGEN_SHORT", size);
            break;

        default:
            break;
    }

    return buffer;
}

// OFFSET: 0x004650D0
bool CGTooltip::GetScalingStatWeaponDPS(const ItemRecSparse* item, const ScalingStatValuesRec* values, float* minDamage, float* maxDamage) {
    if (!item) {
        return false;
    }

    const ItemRec* rec = item->GetNonSparse();

    if (!rec || !minDamage || !maxDamage || !values || !item->m_scalingStatDistribution || rec->m_classID != 2) {
        return false;
    }

    float variance = 0.30000001f;
    float dps;

    if (item->m_flags[1] & 0x200) {
        switch (rec->m_subclassID) {
            case 1:
            case 5:
            case 6:
            case 8:
            case 10:
                dps = static_cast<float>(values->m_dpsMod[3]);
                variance = 0.2f;
                break;

            case 19:
                dps = static_cast<float>(values->m_dpsMod[5]);
                break;

            default:
                dps = static_cast<float>(values->m_dpsMod[2]);
                break;
        }
    } else {
        switch (rec->m_subclassID) {
            case 0:
            case 4:
            case 7:
            case 15:
            case 16:
                dps = static_cast<float>(values->m_dpsMod[0]);
                break;

            case 1:
            case 5:
            case 6:
            case 8:
            case 10:
            case 20:
                dps = static_cast<float>(values->m_dpsMod[1]);
                variance = 0.2f;
                break;

            case 2:
            case 3:
            case 18:
                dps = static_cast<float>(values->m_dpsMod[4]);
                break;

            case 19:
                dps = static_cast<float>(values->m_dpsMod[5]);
                break;

            default:
                return false;
        }
    }

    *minDamage = dps - dps * variance;
    *maxDamage = (variance + 1.0f) * dps;
    return true;
}

// OFFSET: 0x00469040
void TooltipObjectCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_objectGuid) {
        WOWGUID objectGuid = tooltip->m_objectGuid;
        tooltip->SetObject(objectGuid);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x00469080/0x14007D220
void TooltipCorpseCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_corpseGuid) {
        WOWGUID corpseGuid = tooltip->m_corpseGuid;
        tooltip->SetCorpse(corpseGuid);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x004690C0/0x14007D260
void TooltipSpellCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_pendingQueries) {
        tooltip->m_pendingQueries--;
    }

    if (tooltip->m_pendingQueries <= 0 && tooltip->m_spellId) {
        tooltip->SetSpell(tooltip->m_spellId, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x00469130/0x14007D310
void TooltipUnitNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_unitGuid) {
        WOWGUID unitGuid = tooltip->m_unitGuid;
        WOWGUID healthGuid = WOWGUID::Make(0);
        tooltip->SetUnit(unitGuid, 1, healthGuid, 0);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x00469180/0x14007D360
void TooltipQuestCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_questId) {
        tooltip->SetQuest(tooltip->m_questId);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x004691B0/0x14007D390
void TooltipAchievementCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_achievementId) {
        tooltip->SetAchievement(tooltip->m_achievementId, tooltip->m_achievementGuid, tooltip->m_achievementCompleted, tooltip->m_achievementTime, tooltip->m_achievementCriteria[0], tooltip->m_achievementCriteria[1], tooltip->m_achievementCriteria[2], tooltip->m_achievementCriteria[3]);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x00469240
CGTooltip::CGTooltip(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_itemLinkInfo.Clear();
    this->m_owner = NULL;
    this->m_numLines = 0;
    this->m_maxLines = 0;
    this->m_numTextures = 0;
    this->m_unitGuid = WOWGUID::Make(0);
    this->m_objectGuid = WOWGUID::Make(0);
    this->m_unitAuraGuid = WOWGUID::Make(0);
    this->m_itemGuid = WOWGUID::Make(0);
    this->m_corpseGuid = WOWGUID::Make(0);
    this->m_healthGuid = WOWGUID::Make(0);
    this->m_itemId = 0;
    this->m_currencyId = 0;
    this->m_spellId = 0;
    this->m_questId = 0;
    this->m_achievementId = 0;
    this->m_nextRankSpellId = 0;
    this->m_itemNameOnly = 0;
    this->m_itemSocketPreview = 0;
    this->m_itemCompare = 0;
    this->m_itemOwner = WOWGUID::Make(0);
    this->m_pendingQueries = 0;
    this->m_fading = 0;
    this->m_itemMinimumLevel = 0;
    this->m_itemDisplayedLevel = 0;
    this->m_itemSkipSellPrice = 0;
    this->m_statusBar = NULL;
    this->m_padding = 0.0f;
    this->m_minimumWidth = 0.0f;
    this->m_offsetX = 0.0f;
    this->m_offsetY = 0.0f;
    this->m_minimumWidthForce = 0;
}

// OFFSET: 0x004693E0
const char* CGTooltip::GetObjectTypeName() {
    return CGTooltip::s_objectTypeName;
}

// PROVEN: f4d4043b IDENTICAL/NONE
// OFFSET: 0x004693F0/0x14007D620
int32_t CGTooltip::GetScriptMetaTable() {
    return CGTooltip::s_metatable;
}

// OFFSET: 0x00469570/0x14007D770
bool CGTooltip::IsA(int32_t type) {
    return type == CGTooltip::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x004695B0
bool CGTooltip::IsA(const char* typeName) {
    return !_stricmp(typeName, CGTooltip::s_objectTypeName) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x004695F0/0x14007D810
CGTooltip::~CGTooltip() {
    if (this->m_healthGuid && ClntObjMgrGetCurrent()) {
        if (this->m_unitGuid) {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), HealthUpdateHandler, this->m_statusBar);
        } else {
            ClntObjMgrUnsetObjMirrorHandler(this->m_healthGuid, ID_GAMEOBJECT, offsetof(CGGameObjectData, m_animProgress), ObjHealthUpdateHandler, this->m_statusBar);
        }
    }
}

// OFFSET: 0x004696D0/0x14007D920
const char* CGTooltip::GetItemHyperlinkString(int32_t itemId, uint32_t quality, int32_t enchant, const int32_t* gems, int32_t randomPropertiesId, int32_t seed, int32_t reforge) {
    int32_t level = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        level = player->m_unit->m_unitFieldLevel;
    }

    if (quality >= 8) {
        quality = 1;
    }

    const char* color = s_itemQualityColorStrings[quality];
    char name[256];
    CGItem_C::BuildItemName(name, sizeof(name), itemId, randomPropertiesId);
    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hitem:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d|h[%s]|h%s", color, itemId, enchant, gems[0], gems[1], gems[2], 0, randomPropertiesId, seed, level, reforge, name, *color ? "|r" : "");
    return s_itemLink;
}

// OFFSET: 0x004697B0/0x14007DA70
const char* CGTooltip::GetItemHyperlinkString(int32_t itemId, uint32_t quality, int32_t enchant, int32_t randomPropertiesId, int32_t seed, int32_t reforge) {
    int32_t gems[3] = { 0, 0, 0 };
    return CGTooltip::GetItemHyperlinkString(itemId, quality, enchant, gems, randomPropertiesId, seed, reforge);
}

// OFFSET: 0x004697F0/0x14007DAB0
const char* CGTooltip::GetQuestDifficultyColorString(int32_t level) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return s_questDifficultyColorStrings[2];
    }

    int32_t difference = level - static_cast<int32_t>(player->m_unit->m_unitFieldLevel);

    if (difference >= 5) {
        return s_questDifficultyColorStrings[0];
    }

    if (difference >= 3) {
        return s_questDifficultyColorStrings[1];
    }

    if (difference >= -2) {
        return s_questDifficultyColorStrings[2];
    }

    if (-difference <= CGQuestLog::GetQuestGreenRange()) {
        return s_questDifficultyColorStrings[3];
    }

    return s_questDifficultyColorStrings[4];
}

// OFFSET: 0x00469870/0x14007DB50
const char* CGTooltip::GetQuestHyperlinkString(int32_t questId, int32_t level) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);

    if (!quest) {
        return "";
    }

    const char* color = CGTooltip::GetQuestDifficultyColorString(level);
    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hquest:%d:%d|h[%s]|h%s", color, questId, quest->m_level, static_cast<const char*>(quest->m_name), *color ? "|r" : "");
    return s_itemLink;
}

// OFFSET: 0x00469900
int32_t CGTooltip::GetDislayedScalingItemLevel(const ItemRecSparse* item, const TooltipExtendedItemInfo* info, WOWGUID target, int32_t minimumLevel) {
    int32_t level = 0;
    ScalingStatDistributionRec* distribution = g_scalingStatDistributionDB.GetRecord(item->m_scalingStatDistribution);

    if (minimumLevel) {
        level = 1;
    } else if ((!info || (level = info->m_level) == 0) && target != WOWGUID::Make(0)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            level = unit->m_unit->m_unitFieldLevel;
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !level) {
        level = player->m_unit->m_unitFieldLevel;
    }

    if (distribution) {
        int32_t minLevel = item->m_requiredLevel <= distribution->m_minlevel ? distribution->m_minlevel : item->m_requiredLevel;
        int32_t effective = minLevel <= level ? level : minLevel;

        if (effective >= distribution->m_maxlevel) {
            level = distribution->m_maxlevel;
        } else if (minLevel > level) {
            level = minLevel;
        }
    }

    return level > 1 ? level : 1;
}

// OFFSET: 0x00469A10/0x14007DD60
void CGTooltip::SetOwner(CSimpleFrame* owner, TOOLTIP_ANCHORPOINT anchorType, float offsetX, float offsetY) {
    this->ClearTooltip();
    this->SetFrameAlpha(255);
    this->m_fading = 0;

    if (owner == this->m_owner && anchorType == this->m_anchorType && fabs(offsetX - this->m_offsetX) < 0.00000023841858f && fabs(offsetY - this->m_offsetY) < 0.00000023841858f) {
        return;
    }

    this->m_offsetX = offsetX;
    this->m_owner = owner;
    this->m_anchorType = ANCHOR_NONE;
    this->m_offsetY = offsetY;

    if (owner) {
        this->m_anchorType = anchorType;
    }

    this->ResetPosition(1);
}

// OFFSET: 0x00469AC0/0x14007DE40
void CGTooltip::AppendText(const char* text) {
    if (!this->m_numLines) {
        return;
    }

    const char* current = this->m_leftStrings.m_data[0]->m_text;

    if (!current || !*current) {
        current = NULL;
    }

    char buffer[256];
    SStrPrintf(buffer, sizeof(buffer), "%s%s", current, text);
    this->m_leftStrings.m_data[0]->SetText(buffer, 1);
    this->CalculateSize();
}

// PROVEN: 6868dd86 IDENTICAL/NONE
// OFFSET: 0x00469B30/0x14007DEC0
void CGTooltip::HideThis() {
    this->SetOwner(NULL, ANCHOR_LEFT, 0.0f, 0.0f);
    CSimpleFrame::HideThis();
}

// OFFSET: 0x00469FC0
void CGItemStatsSummary::AddEnchantEffect(int32_t type, int32_t arg, int32_t amount) {
    if (type == 3) {
        SpellRec* spell = g_spellDB.GetRecord(arg);

        if (spell) {
            this->ParseSpellRec(spell);
        }
    } else if (type == 4) {
        this->m_stats[arg] += amount;
    } else if (type == 5) {
        if (arg == 50) {
            this->m_stats[0] = amount;
        } else {
            this->m_stats[arg + 11] += amount;

            if (arg == 38) {
                this->m_stats[8] += amount;
                this->m_stats[9] += amount;
            } else if (arg == 39) {
                this->m_stats[9] += amount;
            }
        }
    }
}

// OFFSET: 0x0046A050
void CGItemStatsSummary::ParseWeaponDPS(const ItemRecSparse* item) {
    if (!item) {
        return;
    }

    const ItemRec* rec = item->GetNonSparse();

    if (!rec) {
        return;
    }

    this->m_flags |= 0x4;

    if (!item->m_delay) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    float damage;

    if (player) {
        float minimum = GetEffectiveWeaponDamage(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, rec->m_classID, rec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], true);
        damage = GetEffectiveWeaponDamage(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, rec->m_classID, rec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], false) + minimum;
    } else {
        float minimum = GetEffectiveWeaponDamage(item->m_itemLevel, item->m_itemLevel, rec->m_classID, rec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], true);
        damage = GetEffectiveWeaponDamage(item->m_itemLevel, item->m_itemLevel, rec->m_classID, rec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], false) + minimum;
    }

    float average = damage * 0.5f;
    this->m_dps = average * 1000.0f / static_cast<float>(item->m_delay) + this->m_dps;
}

// OFFSET: 0x0046A200
void CGItemStatsSummary::ParseResistances(const ItemRecSparse* item) {
    if (!item) {
        return;
    }

    const ItemRec* rec = item->GetNonSparse();

    if (!rec) {
        return;
    }

    this->m_flags |= 0x8;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    this->m_stats[0] += GetEffectiveArmor(item->m_itemLevel, item->m_itemLevel, rec->m_classID, rec->m_subclassID, item->m_quality, item->m_inventoryType);

    for (uint32_t i = 0; i < 10; i++) {
        int32_t stat = item->m_statModifierBonusStat[i];

        if (stat >= 51 && stat <= 56) {
            int32_t amount = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
            this->m_stats[GetDamageClassFromBonusStat(stat)] += amount;
        }
    }
}

// OFFSET: 0x0046A2E0
void CGItemStatsSummary::ParseBasicStats(const ItemRecSparse* item) {
    this->m_flags |= 0x20;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    for (uint32_t i = 0; player && i < 10; i++) {
        int32_t amount = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
        int32_t stat = item->m_statModifierBonusStat[i];

        if (stat == 50) {
            this->m_stats[0] = amount;
            continue;
        }

        this->m_stats[stat + 11] += amount;

        if (stat == 38) {
            this->m_stats[8] += amount;
            this->m_stats[9] += amount;
        } else if (stat == 39) {
            this->m_stats[9] += amount;
        }
    }
}

// OFFSET: 0x0046A390
void CGItemStatsSummary::FillOutScalingItem(const ItemRecSparse* item, int32_t level) {
    this->m_flags |= 0x80;
    ScalingStatValuesRec* values = NULL;

    if (!level) {
        level = CGTooltip::GetDislayedScalingItemLevel(item, NULL, WOWGUID::Make(0), 0);
    }

    ScalingStatDistributionRec* distribution = g_scalingStatDistributionDB.GetRecord(item->m_scalingStatDistribution);

    if (level - 1 >= 0 && level - 1 < g_scalingStatValuesDB.m_numRecords) {
        values = g_scalingStatValuesDB.GetRecordByIndex(level - 1);
    }

    if (!distribution || !values) {
        return;
    }

    this->ScanEnchantSpells(item);

    if (item->m_scalingStatDistribution) {
        int32_t budget = CGTooltip::GetScalingStatItemBudget(item, values);

        for (uint32_t i = 0; i < 10; i++) {
            if (distribution->m_statID[i] > 0) {
                this->AddStatMod(distribution->m_statID[i], budget * distribution->m_bonus[i] / 10000);
            }
        }
    } else {
        this->ParseBasicStats(item);
    }

    int32_t armor = CGTooltip::GetScalingStatArmor(item, values);

    if (armor <= 0) {
        this->ParseResistances(item);
    } else {
        this->m_stats[0] += armor;
    }

    float minDamage = 0.0f;
    float maxDamage = 0.0f;

    if (CGTooltip::GetScalingStatWeaponDPS(item, values, &minDamage, &maxDamage)) {
        this->m_dps = (maxDamage + minDamage) * 0.5f;
    } else {
        this->ParseWeaponDPS(item);
    }

    if (item->m_scalingStatDistribution && (item->m_flags[1] & 0x200)) {
        this->m_stats[56] += values->m_spellPower;
    }
}

// OFFSET: 0x0046A840
void CGItemStatsSummary::AddEnchantment(const SpellItemEnchantmentRec* enchant) {
    for (uint32_t i = 0; i < 3; i++) {
        this->AddEnchantEffect(enchant->m_effect[i], enchant->m_effectArg[i], (enchant->m_effectPointsMin[i] + enchant->m_effectPointsMax[i]) / 2);
    }
}

// OFFSET: 0x0046A880
void CGItemStatsSummary::AddEnchantment(const SpellItemEnchantmentRec* enchant, int32_t amount) {
    for (uint32_t i = 0; i < 3; i++) {
        this->AddEnchantEffect(enchant->m_effect[i], enchant->m_effectArg[i], amount);
    }
}

// OFFSET: 0x0046A8C0/0x14007EF80
void CGTooltip::PostLoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::PostLoadXML(node, status);

    for (int32_t pass = 0; pass < 2; pass++) {
        if (pass == 1) {
            this->m_leftStrings.SetCount(this->m_maxLines);
            this->m_rightStrings.SetCount(this->m_maxLines);
            this->m_lineWrap.SetCount(this->m_maxLines);
        }

        for (this->m_maxLines = 0; ; this->m_maxLines++) {
            char leftName[256];
            char rightName[256];

            const char* name = this->GetName();
            SStrPrintf(leftName, sizeof(leftName), "%sTextLeft%d", name ? name : "<unnamed>", this->m_maxLines + 1);

            name = this->GetName();
            SStrPrintf(rightName, sizeof(rightName), "%sTextRight%d", name ? name : "<unnamed>", this->m_maxLines + 1);

            CSimpleFontString* left = static_cast<CSimpleFontString*>(CScriptObject::GetScriptObjectByName(leftName, CSimpleFontString::GetObjectType()));
            CSimpleFontString* right = static_cast<CSimpleFontString*>(CScriptObject::GetScriptObjectByName(rightName, CSimpleFontString::GetObjectType()));

            if (!left || !right) {
                break;
            }

            if (pass == 1) {
                this->m_leftStrings[this->m_maxLines] = left;
                this->m_rightStrings[this->m_maxLines] = right;
                this->m_lineWrap[this->m_maxLines] = 0;
            }
        }
    }

    char buffer[256];

    for (uint32_t i = 0; i < sizeof(this->m_textures) / sizeof(this->m_textures[0]); i++) {
        const char* name = this->GetName();
        SStrPrintf(buffer, sizeof(buffer), "%sTexture%d", name ? name : "<unnamed>", i + 1);
        this->m_textures[i] = static_cast<CSimpleTexture*>(CScriptObject::GetScriptObjectByName(buffer, CSimpleTexture::GetObjectType()));
    }

    const char* name = this->GetName();
    SStrPrintf(buffer, sizeof(buffer), "%sStatusBar", name ? name : "<unnamed>");
    this->m_statusBar = static_cast<CSimpleStatusBar*>(CScriptObject::GetScriptObjectByName(buffer, CSimpleStatusBar::GetObjectType()));
}

// OFFSET: 0x0046ABD0
void CGTooltip::AddFontStrings(CSimpleFontString* left, CSimpleFontString* right) {
    this->m_maxLines++;
    this->m_leftStrings.SetCount(this->m_maxLines);
    this->m_rightStrings.SetCount(this->m_maxLines);
    this->m_lineWrap.SetCount(this->m_maxLines);
    this->m_leftStrings.m_data[this->m_maxLines - 1] = left;
    this->m_rightStrings.m_data[this->m_maxLines - 1] = right;
    this->m_lineWrap.m_data[this->m_maxLines - 1] = 0;
}

// OFFSET: 0x0046ACA0/0x14007F4C0
void CGTooltip::AddLine(const char* left, const char* right, const CImVector& leftColor, const CImVector& rightColor, int32_t wrap) {
    if ((!left || !*left) && (!right || !*right)) {
        return;
    }

    if (!this->m_maxLines) {
        return;
    }

    if (this->m_numLines == this->m_maxLines - 1) {
        char name[256];

        const char* tooltipName = this->GetName();
        SStrPrintf(name, sizeof(name), "%sTextLeft%d", tooltipName ? tooltipName : "<unnamed>", this->m_maxLines + 1);
        void* data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        CSimpleFontString* leftString = data ? new (data) CSimpleFontString(this, 2, true) : NULL;
        leftString->SetName(name);
        leftString->SetFontObject(this->m_leftStrings.m_data[this->m_maxLines - 1]->m_fontObject);
        CSimpleFontString* previous = this->m_leftStrings.m_data[this->m_maxLines - 1];
        leftString->SetPoint(FRAMEPOINT_TOPLEFT, previous, FRAMEPOINT_BOTTOMLEFT, 0.0f, NDCToDDCWidth(-2.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 0);
        leftString->Hide();

        tooltipName = this->GetName();
        SStrPrintf(name, sizeof(name), "%sTextRight%d", tooltipName ? tooltipName : "<unnamed>", this->m_maxLines + 1);
        data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        CSimpleFontString* rightString = data ? new (data) CSimpleFontString(this, 2, true) : NULL;
        rightString->SetName(name);
        rightString->SetFontObject(this->m_rightStrings.m_data[this->m_maxLines - 1]->m_fontObject);
        rightString->SetPoint(FRAMEPOINT_RIGHT, leftString, FRAMEPOINT_LEFT, NDCToDDCWidth(40.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 0.0f, 0);
        rightString->Hide();

        this->AddFontStrings(leftString, rightString);
    }

    if (left && *left) {
        CSimpleFontString* string = this->m_leftStrings.m_data[this->m_numLines];
        string->SetVertexColor(leftColor);
        string->SetText(left, 1);
        this->m_leftStrings.m_data[this->m_numLines]->Show();
    }

    int32_t lineWrap;

    if (right && *right) {
        CSimpleFontString* string = this->m_rightStrings.m_data[this->m_numLines];
        string->SetVertexColor(rightColor);
        string->SetText(right, 1);
        this->m_rightStrings.m_data[this->m_numLines]->Show();
        lineWrap = 0;
    } else {
        lineWrap = wrap;
    }

    this->m_lineWrap.m_data[this->m_numLines++] = lineWrap;
}

// PROVEN: 94bb7ffa IDENTICAL/NONE
// OFFSET: 0x0046AFC0/0x14007F8E0
void CGTooltip::AddLine(const char* left, const char* right, int32_t wrap) {
    const CImVector& titleColor = *reinterpret_cast<const CImVector*>(&s_tooltipTitleColor);
    this->AddLine(left, right, titleColor, titleColor, wrap);
}

// OFFSET: 0x0046B4C0
void CGItemStatsSummary::FillOutRandomProperties(int32_t randomPropertiesId, int32_t seed) {
    this->m_flags |= 0x2;

    if (randomPropertiesId <= 0) {
        ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

        if (!suffix) {
            return;
        }

        for (uint32_t i = 0; i < 5; i++) {
            int32_t amount = CGItem_C::GetRandomPropertyOverrideAmountEx(i + 10, randomPropertiesId, static_cast<uint16_t>(seed));
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(suffix->m_enchantment[i]);

            if (enchant) {
                this->AddEnchantment(enchant, amount);
            }
        }
    } else {
        ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        if (!properties) {
            return;
        }

        for (uint32_t i = 0; i < 5; i++) {
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(properties->m_enchantment[i]);

            if (enchant) {
                this->AddEnchantment(enchant);
            }
        }
    }
}

// OFFSET: 0x0046B5D0/0x140080150
void CGTooltip::AddEmpathyText(CGUnit_C* unit, char* buffer, uint32_t size) {
    int32_t minDamage = static_cast<int32_t>(unit->m_unit->m_unitFieldMindamage);
    int32_t maxDamage = static_cast<int32_t>(ceil(static_cast<double>(unit->m_unit->m_unitFieldMaxdamage)));

    if (minDamage == maxDamage) {
        SStrPrintf(buffer, size, FrameScript_GetText("SINGLE_DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minDamage);
    } else {
        SStrPrintf(buffer, size, FrameScript_GetText("DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minDamage, maxDamage);
    }

    this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);

    int32_t armor = unit->m_unit->m_unitFieldResistances[GetPhysicalDamageClassID()] > 0 ? unit->m_unit->m_unitFieldResistances[GetPhysicalDamageClassID()] : 0;
    SStrPrintf(buffer, size, FrameScript_GetText("ARMOR_TEMPLATE", -1, GENDER_NOT_APPLICABLE), armor);
    this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);

    SStrPrintf(buffer, size, FrameScript_GetText("HP_TEMPLATE", -1, GENDER_NOT_APPLICABLE), unit->m_unit->m_unitFieldHealth);
    this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);

    for (int32_t i = 0; i < 7; i++) {
        if (i == GetPhysicalDamageClassID() || unit->m_unit->m_unitFieldResistances[i] <= 0) {
            continue;
        }

        char token[32];
        SStrPrintf(token, sizeof(token), "RESISTANCE%d_NAME", i);
        const char* name = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);

        if (name && *name) {
            SStrPrintf(buffer, size, FrameScript_GetText("RESISTANCE_TEMPLATE", -1, GENDER_NOT_APPLICABLE), unit->m_unit->m_unitFieldResistances[i], name);
            this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    }

    CreatureFamilyRec* family = g_creatureFamilyDB.GetRecord(unit->GetCreatureFamily());

    if (family && family->m_petFoodMask) {
        char diet[256];
        diet[0] = '\0';
        int32_t first = 1;

        for (int32_t i = 0; i < g_itemPetFoodDB.m_numRecords; i++) {
            ItemPetFoodRec* food = g_itemPetFoodDB.GetRecordByIndex(i);

            if (food && ((1 << (food->m_id - 1)) & family->m_petFoodMask)) {
                if (!first) {
                    SStrPack(diet, ", ", sizeof(diet));
                }

                SStrPack(diet, food->m_name, sizeof(diet));
                first = 0;
            }
        }

        if (!first) {
            SStrPrintf(buffer, size, FrameScript_GetText("PET_DIET_TEMPLATE", -1, GENDER_NOT_APPLICABLE), diet);
            this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    }

    CreatureStats_C* stats = unit->m_creatureCacheEntry;

    if (!stats || !(stats->m_typeFlags[0] & 0x1) || !stats->m_family) {
        this->AddLine(FrameScript_GetText("NOT_TAMEABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        return;
    }

    this->AddLine(FrameScript_GetText(stats->m_typeFlags[0] & 0x10000 ? "TAMEABLE_EXOTIC" : "TAMEABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);

    family = g_creatureFamilyDB.GetRecord(unit->GetCreatureFamily());

    if (!family) {
        return;
    }

    char spells[512];
    spells[0] = '\0';
    int32_t first = 1;

    for (uint32_t j = 0; j < 2; j++) {
        for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
            SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

            if (!ability || ability->m_skillLine != family->m_skillLine[j]) {
                continue;
            }

            SpellRec* spell = g_spellDB.GetRecord(ability->m_spell);

            if (spell && !(spell->m_attributes & 0xC0) && !ability->m_supercededBySpell && ability->m_acquireMethod == 2) {
                if (!first) {
                    SStrPack(spells, ", ", sizeof(spells));
                }

                SStrPack(spells, spell->m_name, sizeof(spells));
                first = 0;
            }
        }
    }

    if (!first) {
        SStrPrintf(buffer, size, FrameScript_GetText("PET_SPELLS_TEMPLATE", -1, GENDER_NOT_APPLICABLE), spells);
        this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }
}

// OFFSET: 0x0046BBD0/0x140080870
void CGTooltip::AddSkinnableText(CGUnit_C* unit, char* buffer, uint32_t size) {
    *buffer = '\0';
    CImVector color = s_skillColors[4];
    const char* suffix = "";
    int32_t skinningSpell = CGSpellBook::GetSkinningSpell(unit);

    if (skinningSpell) {
        SpellRec* spell = g_spellDB.GetRecord(skinningSpell);
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t i = 0; i < count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect) {
                break;
            }

            if (effect->m_effect != 95) {
                continue;
            }

            int32_t skill;
            int32_t maxSkill;
            Spell_C_GetMinMaxPoints(spell, i, &skill, &maxSkill, 0, 0, 0, 0);

            if (skill > 0) {
                color = *GetSkillColor(GetSkinningDifficulty(unit->m_unit->m_unitFieldLevel), skill, &suffix);
            }

            break;
        }
    }

    const char* token;

    switch (unit->GetSkinningType()) {
        case 0:
            token = "UNIT_SKINNABLE_LEATHER";
            break;

        case 1:
            token = "UNIT_SKINNABLE_HERB";
            break;

        case 2:
            token = "UNIT_SKINNABLE_ROCK";
            break;

        case 3:
            token = "UNIT_SKINNABLE_BOLTS";
            break;

        default:
            return;
    }

    SStrPrintf(buffer, size, "%s%s", suffix, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));

    if (*buffer) {
        this->AddLine(buffer, NULL, color, color, 0);
    }
}

// OFFSET: 0x0046BD60
void CGTooltip::AddItemNameDescription(int32_t descriptionId) {
    ItemNameDescriptionRec* description = g_itemNameDescriptionDB.GetRecord(descriptionId);

    if (!description) {
        return;
    }

    CImVector color = CImVector(description->m_color | 0xFF000000);
    this->AddLine(description->m_description, NULL, color, color, 0);
}

// OFFSET: 0x0046BDB0/0x140080BC0
void CGTooltip::SetCorpse(const WOWGUID& guid) {
    this->ClearTooltip();
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (!corpse) {
        this->Hide();
        return;
    }

    this->m_corpseGuid = guid;
    this->m_corpseOwnerGuid = corpse->m_corpse->m_corpseFieldOwner;
    NameCache* name = g_nameDBCache.GetRecord(this->m_corpseOwnerGuid.m_guid, this->m_corpseOwnerGuid, TooltipCorpseCallback, this, true);

    if (name) {
        char line[1024];
        SStrPrintf(line, sizeof(line), FrameScript_GetText("CORPSE_TOOLTIP", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name));
        this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        this->Show();
    }
}

// OFFSET: 0x0046BE90/0x140080CC0
int32_t CGTooltip::AddTalentPrereqs(const TalentRec* talent, const CGTalentGroup_C* group, int32_t isInspect, int32_t isPet, int32_t usePreview) {
    if (!talent) {
        return 0;
    }

    char text[256];
    int32_t met = 1;

    for (uint32_t i = 0; i < 3; i++) {
        int32_t prereqId = talent->m_prereqTalent[i];

        if (!prereqId) {
            continue;
        }

        TalentRec* prereq = g_talentDB.GetRecord(prereqId);

        if (!prereq) {
            continue;
        }

        int32_t rank = talent->m_prereqRank[i];
        const TalentGroupItem_C* item;

        if (group && (item = group->GetTalentItem(prereqId)) && (usePreview ? item->m_previewRank : item->m_rank) >= rank) {
            continue;
        }

        for (uint32_t r = rank; r < 5; r++) {
            int32_t spellId = prereq->m_spellRank[r];

            if (spellId) {
                met = 0;
                SpellRec* spell = g_spellDB.GetRecord(spellId);

                if (spell) {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("TOOLTIP_TALENT_PREREQ", -1, GENDER_NOT_APPLICABLE), rank + 1, static_cast<const char*>(spell->m_name));
                    this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                }

                break;
            }
        }
    }

    if (group && talent->m_tierID > 0) {
        TalentTabRec* tab = g_talentTabDB.GetRecord(talent->m_tabID);

        if (tab) {
            const TalentGroupTab_C* groupTab = group->GetTabFromID(talent->m_tabID);
            uint32_t spent = groupTab ? groupTab->m_pointsSpent + groupTab->m_previewPointsSpent : 0;
            uint32_t required = talent->m_tierID * (2 * (isPet == 0) + 3);

            if (spent < required) {
                SStrPrintf(text, sizeof(text), FrameScript_GetText("TOOLTIP_TALENT_TIER_POINTS", -1, GENDER_NOT_APPLICABLE), required, static_cast<const char*>(tab->m_name));
                this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                met = 0;
            }
        }
    }

    if (talent->m_requiredSpellID && !TalentInfoIsSpellKnown(talent->m_requiredSpellID, isInspect, isPet)) {
        SpellRec* spell = g_spellDB.GetRecord(talent->m_requiredSpellID);

        if (spell) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(spell->m_name));
            this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            return 0;
        }
    }

    return met;
}

// OFFSET: 0x0046C130/0x1400810E0
void CGTooltip::AddTalentLearnText(const TalentRec* talent, const CGTalentGroup_C* group, int32_t prereqsMet, int32_t rank, int32_t maxRank, int32_t a7, int32_t a8, int32_t isPet, int32_t usePreview) {
    if (!talent || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    uint32_t activeGroup = CGTalentInfo::GetActiveTalentGroup(isPet);

    if (group != CGTalentInfo::GetTalentGroup(activeGroup, isPet)) {
        return;
    }

    if (usePreview) {
        if (prereqsMet && !a7 && !a8) {
            int32_t count;

            if (group->CanAddPreviewTalentPoints(talent, 1, count, isPet)) {
                this->AddLine(FrameScript_GetText("TALENT_TOOLTIP_ADDPREVIEWPOINT", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
            }

            if (group->CanAddPreviewTalentPoints(talent, -1, count, isPet)) {
                this->AddLine(FrameScript_GetText("TALENT_TOOLTIP_REMOVEPREVIEWPOINT", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            }
        }
    } else {
        uint32_t unspent = CGTalentInfo::GetUnspentTalentPoints(activeGroup, isPet);

        if (prereqsMet && !a7 && !a8 && unspent && rank != maxRank) {
            this->AddLine(FrameScript_GetText("TOOLTIP_TALENT_LEARN", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        }
    }
}

// OFFSET: 0x0046C290/0x140081290
void CGTooltip::SetCurrency(const CurrencyTypesRec* currency, WOWGUID owner) {
    if (!currency) {
        return;
    }

    this->ClearTooltip();
    this->AddLine(currency->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    this->AddLine(currency->m_description, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);

    if (owner == CGMerchantInfo::m_merchant) {
        VendorItem* item = CGMerchantInfo::FindItem(VENDOR_ITEM_TYPE_CURRENCY, currency->m_id);

        if (item) {
            PlayerConditionRec* condition = g_playerConditionDB.GetRecord(item->m_playerConditionID);

            if (condition) {
                this->AddLine(condition->m_failureDescription, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            }
        }
    }

    PlayerCurrency* info = CGCurrencyTypes::GetCurrency(currency->m_id);

    if (info && (info->m_weeklyCap > 0 || currency->m_totalCap > 0 || info->m_trackedQuantity > 0)) {
        char line[1024];
        char red[256];
        char white[256];
        this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        SStrPrintf(white, sizeof(white), "%s%s", "|c", s_tooltipTextColorString);
        SStrPrintf(red, sizeof(red), "%s%s", "|c", s_tooltipRedColorString);

        if (info->m_weeklyCap > 0) {
            if (info->m_weeklyQuantity < info->m_weeklyCap) {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("CURRENCY_WEEKLY_CAP", -1, GENDER_NOT_APPLICABLE), white, info->m_weeklyQuantity, info->m_weeklyCap);
            } else {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("CURRENCY_WEEKLY_CAP", -1, GENDER_NOT_APPLICABLE), red, info->m_weeklyQuantity, info->m_weeklyCap);
            }

            this->AddLine(line, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        }

        if (currency->m_totalCap > 0) {
            int32_t cap = currency->m_flags & 0x8 ? currency->m_totalCap / 100 : currency->m_totalCap;

            if (info->m_quantity < cap) {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("CURRENCY_TOTAL_CAP", -1, GENDER_NOT_APPLICABLE), white, info->m_quantity, cap);
            } else {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("CURRENCY_TOTAL_CAP", -1, GENDER_NOT_APPLICABLE), red, info->m_quantity, cap);
            }

            this->AddLine(line, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        }

        if (info->m_trackedQuantity > 0) {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("CURRENCY_SEASON_TOTAL", -1, GENDER_NOT_APPLICABLE), white, info->m_trackedQuantity);
            this->AddLine(line, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        }
    }

    this->m_currencyId = currency->m_id;
    this->Show();
}

// OFFSET: 0x0046C560/0x140081640
int32_t CGTooltip::SetQuest(int32_t questId) {
    this->ClearTooltip();
    WOWGUID requester = WOWGUID::Make(0);
    this->m_questId = questId;
    QuestCache* quest = g_questDBCache.GetRecord(questId, requester, TooltipQuestCallback, this, false);

    if (!quest) {
        return 0;
    }

    this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

    if (CGQuestLog::GetQuestIndex(questId) > 0) {
        this->AddLine(FrameScript_GetText("QUEST_TOOLTIP_ACTIVE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
    }

    this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

    char text[1024];
    WOWGUID player = ClntObjMgrGetActivePlayer();
    QuestParserParseText(quest->m_objText, text, sizeof(text), player, NULL, 0);
    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);

    int32_t count = CGQuestLog::GetNumQuestLeaderBoards(questId, TooltipQuestCallback, this);

    if (count > 0) {
        this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        this->AddLine(FrameScript_GetText("QUEST_TOOLTIP_REQUIREMENTS", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

        QuestLogLeaderBoardParams params;
        params.m_showProgress = 0;
        params.m_questCallback = TooltipQuestCallback;
        params.m_objectCallback = TooltipQuestCallback;
        params.m_creatureCallback = TooltipQuestCallback;
        params.m_itemCallback = TooltipQuestCallback;
        params.m_param = this;

        for (int32_t i = 0; i < count; i++) {
            params.m_index = i;

            if (CGQuestLog::GetQuestLeaderBoardByID(questId, &params)) {
                char line[4099] = " - ";
                SStrPack(line, params.m_text, sizeof(line));
                this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }
    }

    if (this->HasScript(this->m_onTooltipSetQuest)) {
        this->RunScript(this->m_onTooltipSetQuest, 0, NULL);
    }

    this->Show();
    return 1;
}

// OFFSET: 0x0046C7F0/0x140081970
int32_t CGTooltip::SetAchievement(int32_t achievementId, WOWGUID guid, bool completed, WowTime time, uint32_t criteria1, uint32_t criteria2, uint32_t criteria3, uint32_t criteria4) {
    AchievementRec* rec = g_achievementDB.GetRecord(achievementId);

    if (!rec) {
        return 0;
    }

    const char* name = NULL;
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) || static_cast<uint32_t>(guid.m_guid))) {
        WOWGUID requester = WOWGUID::Make(0);
        NameCache* cache = g_nameDBCache.GetRecord(guid.m_guid, requester, TooltipAchievementCallback, this, true);

        if (cache) {
            name = cache->m_name;
        }
    } else if ((high & 0xF0000000) == 0x10000000 && (high & 0x0FF00000) == 0x0FF00000) {
        WOWGUID requester = WOWGUID::Make(0);
        GuildStats_C* stats = g_guildInfoCache.GetRecord(guid.m_guid, requester, TooltipAchievementCallback, this, true);

        if (stats) {
            name = stats->m_name;
        }
    }

    this->ClearTooltip();
    this->m_achievementId = achievementId;
    this->m_achievementCompleted = completed;
    this->m_achievementGuid = guid;
    this->m_achievementTime = time;
    this->m_achievementCriteria[0] = criteria1;
    this->m_achievementCriteria[1] = criteria2;
    this->m_achievementCriteria[2] = criteria3;
    this->m_achievementCriteria[3] = criteria4;

    char text[3000];
    QuestParserParseText(rec->m_title, text, sizeof(text), guid, NULL, 0);
    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

    char status[1024];

    if (name) {
        if (completed) {
            SStrPrintf(status, sizeof(status), FrameScript_GetText("ACHIEVEMENT_TOOLTIP_COMPLETE", -1, GENDER_NOT_APPLICABLE), name, time.m_month, time.m_monthDay, time.m_year);
            this->AddLine(status, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        } else {
            SStrPrintf(status, sizeof(status), FrameScript_GetText("ACHIEVEMENT_TOOLTIP_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE), name);
            this->AddLine(status, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        }
    } else if (completed) {
        SStrPrintf(status, sizeof(status), FrameScript_GetText("ACHIEVEMENT_TOOLTIP_COMPLETE_UNKNOWN_NAME", -1, GENDER_NOT_APPLICABLE), time.m_month, time.m_monthDay, time.m_year);
        this->AddLine(status, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
    } else {
        this->AddLine(FrameScript_GetText("ACHIEVEMENT_TOOLTIP_IN_PROGRESS_UNKNOWN_NAME", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
    }

    this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    QuestParserParseText(rec->m_description, text, sizeof(text), guid, NULL, 0);
    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);

    char left[1024];
    char right[1024];
    right[0] = '\0';
    left[0] = '\0';
    this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

    int32_t index = 0;
    CImVector leftColor = CImVector(0);
    CImVector rightColor = CImVector(0);
    ACHIEVEMENT* achievement = s_achievements.Ptr(achievementId, HASHKEY_NONE());

    if (achievement->m_rec->m_sharesCriteria) {
        achievement = s_achievements.Ptr(achievement->m_rec->m_sharesCriteria, HASHKEY_NONE());
    }

    if (achievement && (!achievement->m_rec->m_minimumCriteria || criteria1 != 0xFFFFFFFF || criteria2 != 0xFFFFFFFF || criteria3 != 0xFFFFFFFF || criteria4 != 0xFFFFFFFF)) {
        for (SPECIFIC_CRITERIA* node = achievement->m_criteria.Head(); node; node = achievement->m_criteria.Next(node)) {
            Achievement_CriteriaRec* criteria = node->m_criteria;

            if (!criteria || (criteria->m_flags & 0x2)) {
                continue;
            }

            bool met = false;

            if (index < 32) {
                met = ((1 << index) & criteria1) != 0;
            } else if (index < 64) {
                met = ((1 << (index - 32)) & criteria2) != 0;
            } else if (index < 96) {
                met = ((1 << (index - 64)) & criteria3) != 0;
            } else if (index < 128) {
                met = ((1 << (index - 96)) & criteria4) != 0;
            }

            index++;

            if (left[0]) {
                SStrPrintf(right, sizeof(right), static_cast<const char*>(criteria->m_description));
                rightColor = met ? s_skillColors[1] : s_skillColors[0];
            } else {
                SStrPrintf(left, sizeof(left), static_cast<const char*>(criteria->m_description));
                leftColor = met ? s_skillColors[1] : s_skillColors[0];
            }

            if (right[0]) {
                this->AddLine(left, right, leftColor, rightColor, 0);
                left[0] = '\0';
                right[0] = '\0';
            }
        }
    }

    if (!right[0] && left[0]) {
        this->AddLine(left, NULL, leftColor, leftColor, 0);
    }

    if (this->HasScript(this->m_onTooltipSetAchievement)) {
        this->RunScript(this->m_onTooltipSetAchievement, 0, NULL);
    }

    this->Show();
    return 1;
}

// OFFSET: 0x0046CD00/0x1400821E0
void CGTooltip::SetGlyph(int32_t glyphId, int32_t hideRemove, int32_t unit) {
    this->ClearTooltip();
    GlyphPropertiesRec* glyph = g_glyphPropertiesDB.GetRecord(glyphId);

    if (!glyph) {
        this->AddLine(FrameScript_GetText("GLYPH_INACTIVE", -1, GENDER_NOT_APPLICABLE), NULL, s_skillColors[0], s_skillColors[0], 0);
        this->AddLine(FrameScript_GetText("GLYPH_EMPTY_DESC", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
        this->Show();
        return;
    }

    SpellRec* spell = g_spellDB.GetRecord(glyph->m_spellID);

    if (spell) {
        this->AddLine(spell->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    switch (glyph->m_glyphSlotFlags) {
        case 1:
            this->AddLine(FrameScript_GetText("MINOR_GLYPH", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGlyphColor, s_tooltipGlyphColor, 0);
            break;

        case 0:
            this->AddLine(FrameScript_GetText("MAJOR_GLYPH", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGlyphColor, s_tooltipGlyphColor, 0);
            break;

        case 2:
            this->AddLine(FrameScript_GetText("PRIME_GLYPH", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGlyphColor, s_tooltipGlyphColor, 0);
            break;

        default:
            break;
    }

    if (spell->m_description && *spell->m_description) {
        char text[1024];
        SpellParserParseText(spell, text, sizeof(text), 0, unit, 0, 0, 1, 0, NULL);
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);

        if (!hideRemove) {
            this->AddLine(FrameScript_GetText("GLYPH_SLOT_REMOVE_TOOLTIP", -1, GENDER_NOT_APPLICABLE), NULL, s_skillColors[0], s_skillColors[0], 0);
        }
    }

    this->Show();
}

// OFFSET: 0x0046CEB0/0x140082430
int32_t CGTooltip::SetEquipmentSet(int32_t setId) {
    this->ClearTooltip();
    const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(setId);

    if (!set) {
        return 0;
    }

    int32_t total = 0;
    int32_t equipped = 0;
    int32_t inInventory = 0;
    int32_t numMissing = 0;
    int32_t ignored = 0;
    int32_t missing[19];
    CGEquipmentManager::GetSetStatus(setId, total, equipped, inInventory, numMissing, missing, ignored);

    char text[1024];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEMS_VARIABLE_QUANTITY", -1, GENDER_NOT_APPLICABLE), total);

    if (text[0]) {
        this->AddLine(set->m_name, text, s_tooltipTextColor, s_tooltipTitleColor, 0);
    } else {
        this->AddLine(set->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    if (equipped) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEMS_EQUIPPED", -1, GENDER_NOT_APPLICABLE), equipped);
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    }

    if (inInventory) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEMS_IN_INVENTORY", -1, GENDER_NOT_APPLICABLE), inInventory);
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    if (ignored) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SLOTS_IGNORED", -1, GENDER_NOT_APPLICABLE), ignored);
        this->AddLine(text, NULL, s_tooltipGrayColor, s_tooltipGrayColor, 0);
    }

    while (numMissing > 0) {
        numMissing--;
        const char* slot = FrameScript_GetText(GetInventorySlotToken(missing[numMissing]), -1, GENDER_NOT_APPLICABLE);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_MISSING", -1, GENDER_NOT_APPLICABLE), slot);
        this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
    }

    if (this->HasScript(this->m_onTooltipSetEquipmentSet)) {
        this->RunScript(this->m_onTooltipSetEquipmentSet, 0, NULL);
    }

    this->Show();
    return 0;
}

// OFFSET: 0x0046D100
int32_t CGTooltip::SetSpellFlyout(int32_t flyoutId, bool showDescription) {
    CFlyout* flyout = CFlyout::GetByID(flyoutId);
    const SpellFlyoutRec* rec = flyout ? flyout->m_rec : NULL;

    if (!rec) {
        return 0;
    }

    this->AddLine(rec->m_name, NULL, s_tooltipTextColor, s_tooltipGrayColor, 0);

    if (showDescription) {
        this->AddLine(rec->m_description, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
    }

    this->Show();
    return 0;
}

// OFFSET: 0x0046D170/0x140082800
void CGTooltip::SetInstanceLockCompletedEncounters(WOWGUID guid, int32_t mapId, uint32_t difficulty, uint32_t encounterMask) {
    this->ClearTooltip();
    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);
    MapRec* map = g_mapDB.GetRecord(mapId);

    if (!map || difficulty > 3) {
        return;
    }

    char text[1024];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("INSTANCE_LOCK_SS", -1, GENDER_NOT_APPLICABLE), name ? static_cast<const char*>(name->m_name) : "UNKNOWN", static_cast<const char*>(map->m_mapName));
    this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    uint32_t ineligible = 0;

    if (map->m_flags & 0x8000) {
        ineligible = CGRaidInfo::GetIneligibleEncounterMask(mapId, encounterMask, difficulty);

        if (guid != ClntObjMgrGetActivePlayer()) {
            this->AddLine(FrameScript_GetText(ineligible ? "JOIN_RAID_INSTANCE_INELIGIBLE" : "JOIN_RAID_INSTANCE_ELIGIBLE", -1, GENDER_NOT_APPLICABLE), NULL, 0);
        }
    }

    int32_t first = DungeonEncounterFindFirst(mapId);

    if (first < 0) {
        return;
    }

    for (int32_t i = first; i < g_dungeonEncounterDB.m_numRecords; i++) {
        DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecordByIndex(i);

        if (encounter->m_mapID != mapId) {
            break;
        }

        if (encounter->m_difficulty != -1 && encounter->m_difficulty != static_cast<int32_t>(difficulty)) {
            continue;
        }

        uint32_t bit = 1 << encounter->m_bit;

        if (bit & encounterMask) {
            this->AddLine(encounter->m_name, FrameScript_GetText("BOSS_DEAD", -1, GENDER_NOT_APPLICABLE), s_tooltipTextColor, s_tooltipRedColor, 0);
        } else if (bit & ineligible) {
            this->AddLine(encounter->m_name, FrameScript_GetText("BOSS_ALIVE_INELIGIBLE", -1, GENDER_NOT_APPLICABLE), s_tooltipTextColor, s_tooltipGreenColor, 0);
        } else {
            this->AddLine(encounter->m_name, FrameScript_GetText("BOSS_ALIVE", -1, GENDER_NOT_APPLICABLE), s_tooltipTextColor, s_tooltipGreenColor, 0);
        }
    }

    this->Show();
}

// OFFSET: 0x0046D380/0x140082B60
void CGTooltip::SetFrameStack(bool showHidden) {
    this->ClearTooltip();

    CSimpleTop* top = CSimpleTop::s_instance;
    float x = 0.0f;
    float y = 0.0f;
    NDCToDDC(top->m_mousePosition.m_x, top->m_mousePosition.m_y, &x, &y);

    CSimpleFrame* frame = top->m_frames.Head();

    if (!frame) {
        return;
    }

    char text[1024] = { 0 };
    CRect rect;
    rect.minY = 0.0f;
    rect.minX = 0.0f;
    rect.maxY = 0.0f;
    rect.maxX = 0.0f;
    lua_State* L = FrameScript_GetContext();

    for (; frame; frame = frame->m_framesLink.Next()) {
        if (!frame->GetRect(&rect)) {
            continue;
        }

        uint32_t visible = frame->m_visible;

        if ((!visible && !showHidden) || x < rect.minX || x > rect.maxX || y > rect.maxY || y < rect.minY) {
            continue;
        }

        FrameStackInfo info;
        const char* name = frame->GetName();

        if (!name || !*name) {
            if (!frame->m_luaRegistered) {
                frame->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, frame->m_luaObjectRef);
            SStrPrintf(info.m_name, sizeof(info.m_name), "%s: %p", lua_typename(L, lua_type(L, -1)), lua_topointer(L, -1));
            lua_settop(L, -2);
        } else {
            lua_pushstring(L, name);
            lua_rawget(L, LUA_GLOBALSINDEX);

            if (lua_type(L, -1) == LUA_TTABLE) {
                lua_rawgeti(L, -1, 0);
                void* object = lua_touserdata(L, -1);
                lua_settop(L, -3);

                if (object && object == frame) {
                    SStrCopy(info.m_name, name, sizeof(info.m_name));
                } else {
                    lua_rawgeti(L, LUA_REGISTRYINDEX, frame->GetLuaObjectRef());
                    SStrPrintf(info.m_name, sizeof(info.m_name), "%s: %p (%s)", lua_typename(L, lua_type(L, -1)), lua_topointer(L, -1), name);
                    lua_settop(L, -2);
                }
            } else {
                SStrPrintf(info.m_name, sizeof(info.m_name), "%s: %p (%s)", lua_typename(L, lua_type(L, -1)), lua_topointer(L, -1), name);
                lua_settop(L, -2);
            }
        }

        info.m_strata = frame->m_strata;
        info.m_level = frame->m_level;
        info.m_mouseEnabled = (frame->m_enabledEventsBitmask & 0x4) != 0;
        info.m_visible = visible;
        s_frameStack.Add(1, &info);
    }

    qsort(s_frameStack.m_data, s_frameStack.m_count, sizeof(FrameStackInfo), QSortFrameStack);

    SStrPrintf(text, sizeof(text), "(%.2f, %.2f)", DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x), DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y));
    this->AddLine(FrameScript_GetText("DEBUG_FRAMESTACK", -1, GENDER_NOT_APPLICABLE), text, s_tooltipTextColor, s_tooltipTitleColor, 0);

    int32_t toggle = 1;
    int32_t lastStrata = 9;
    int32_t lastLevel = 0;

    for (uint32_t i = 0; i < s_frameStack.m_count; i++) {
        FrameStackInfo* info = &s_frameStack.m_data[i];
        int32_t strata = info->m_strata;

        if (strata != lastStrata) {
            this->AddLine(FrameStrataToString(static_cast<FRAME_STRATA>(strata)), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            lastLevel = info->m_level;
            lastStrata = strata;
            toggle = 0;
        }

        if (info->m_level != lastLevel) {
            toggle = (toggle + 1) % 2;
            lastLevel = info->m_level;
        }

        SStrPrintf(text, sizeof(text), " <%d> %s", info->m_level, static_cast<const char*>(info->m_name));

        if (info->m_visible) {
            const CImVector* color = info->m_mouseEnabled ? &s_frameStackMouseColors[toggle] : &s_frameStackColors[toggle];
            this->AddLine(text, NULL, *color, *color, 0);
        } else {
            this->AddLine(text, NULL, s_frameStackHiddenColors[toggle], s_frameStackHiddenColors[toggle], 0);
        }
    }

    if (this->HasScript(this->m_onTooltipSetFrameStack)) {
        this->RunScript(this->m_onTooltipSetFrameStack, 0, NULL);
    }

    this->Show();
    s_frameStack.m_count = 0;
}

// OFFSET: 0x0046D870/0x1400831C0
int32_t CGTooltip::SetItemNoStats() {
    this->AddLine(FrameScript_GetText("RETRIEVING_ITEM_INFO", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);

    if (this->HasScript(this->m_onTooltipSetItem)) {
        this->RunScript(this->m_onTooltipSetItem, 0, NULL);
    }

    this->Show();
    this->CalculateSize();
    return 0;
}

// OFFSET: 0x0046D8E0
void CGTooltip::AddDeltaHeader(int32_t& first) {
    if (!first) {
        return;
    }

    this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    this->AddLine(FrameScript_GetText("ITEM_DELTA_DESCRIPTION", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
    first = 0;
}

// OFFSET: 0x0046D930
void CGTooltip::AddColoredItemStat(int32_t value, const char* name, int32_t& first) {
    if (!value) {
        return;
    }

    this->AddDeltaHeader(first);
    char text[512];
    SStrPrintf(text, sizeof(text), "|c%s%+d|r %s", value > 0 ? s_positiveDeltaColorString : s_negativeDeltaColorString, value, name);
    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
}

// OFFSET: 0x0046D9A0
void CGTooltip::SetBuff(int32_t spellId, int32_t casterLevel, int32_t stacks, uint8_t a5, const AuraData* aura) {
    this->ClearTooltip();
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        this->Hide();
        return;
    }

    SpellAuraOptionsRec* options;

    if (!spell->m_spellAuraOptionsID || !(options = g_spellAuraOptionsDB.GetRecord(spell->m_spellAuraOptionsID)) || options->m_cumulativeAura <= 1) {
        stacks = 1;
    }

    const char* dispelName = NULL;

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories && categories->m_dispelType) {
            SpellDispelTypeRec* dispel = g_spellDispelTypeDB.GetRecord(categories->m_dispelType);

            if (dispel && dispel->m_immunityPossible) {
                dispelName = dispel->m_name;
            }
        }
    }

    this->AddLine(spell->m_name, dispelName, s_tooltipTitleColor, s_tooltipTitleColor, 0);

    if (spell->m_auraDescription && *spell->m_auraDescription) {
        char text[1024];
        SpellParserParseText(spell, text, sizeof(text), 0, 0, casterLevel, 1, stacks, 1, aura);
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
    }
}

// OFFSET: 0x0046DF90/0x140083A00
int32_t SetTooltipUnitAura(CGTooltip* tooltip, ScriptAuraInfo* info) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if ((unit->m_auraEffectMask[36] & 0x80) && ClntObjMgrGetActivePlayer() && !unit->IsFriendly(ClntObjMgrGetActivePlayerObj())) {
            return 0;
        }

        if (unit->m_obj->m_guid != CGCharacterInfo::m_inspectTarget) {
            CGPetInfo::GetPet(0);
        }

        if (!(unit->m_modelFlags & 0x10000)) {
            int32_t slot = unit->GetSortedAuraSlot(info->m_index, info->m_name, info->m_rank, info->m_filter);

            if (slot < 0) {
                return 0;
            }

            AuraData* aura = unit->GetAura(slot);
            uint8_t flags = unit->GetAuraFlags(slot);
            uint8_t stackCount = unit->GetAuraStackCount(slot);
            uint8_t level = unit->GetAuraLevel(slot);
            tooltip->SetBuff(unit->GetAuraSpellId(slot), level, stackCount, flags, aura);

            if (aura && aura->m_duration > 0) {
                int32_t remaining = aura->m_endTime - FrameTime::GetCurTimeMs();

                if (remaining < 0) {
                    remaining = 0;
                }

                char buffer[1024];
                CGTooltip::GetDurationString(buffer, sizeof(buffer), remaining, "SPELL_TIME_REMAINING", NULL, 1, false);
                tooltip->AddLine(buffer, NULL, 0);
            }

            tooltip->Show();
            return 0;
        }
    }

    uint32_t count;
    const TSGrowableArray<PartyMemberAuraStates>* auras;

    if (PartyMemberState* state = CGGameUI::GetPartyMemberState(info->m_guid)) {
        if (!(state->m_flags & 0x1)) {
            return 0;
        }

        count = state->m_numAuras;
        auras = &state->m_auras;
    } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(info->m_guid)) {
        count = petState->m_numAuras;
        auras = &petState->m_auras;
    } else {
        return 0;
    }

    if (!auras || !auras->m_count) {
        return 0;
    }

    bool helpful = (info->m_filter & 0x1) != 0;
    bool raid = (info->m_filter & 0x8) != 0;
    int32_t found = 0;

    for (uint32_t i = 0; i < count; i++) {
        const PartyMemberAuraStates* entry = &auras->m_data[i];

        if (!entry->m_spellId) {
            break;
        }

        if (helpful != !(entry->m_flags & 0x80)) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(entry->m_spellId);

        if (!spell) {
            continue;
        }

        WOWGUID caster = WOWGUID::Make(0);

        if (!CGSpellBook::CanShowAura(spell, &caster, (entry->m_flags & 0x8) != 0)) {
            continue;
        }

        int32_t dispelType = 0;

        if (spell->m_spellCategoriesID) {
            SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

            if (categories) {
                dispelType = categories->m_dispelType;
            }
        }

        if (raid && !(helpful ? CGSpellBook::FindRaidBuff(entry->m_spellId) : CGSpellBook::FindRaidDispel(dispelType))) {
            continue;
        }

        if ((entry->m_flags & 0x10) ? (info->m_filter & 0x20) : (info->m_filter & 0x10)) {
            continue;
        }

        if ((info->m_index >= 0 && info->m_index == found) || (info->m_name && !SStrCmpUTF8I(info->m_name, spell->m_name, 0x7FFFFFFF) && (!info->m_rank || !SStrCmpUTF8I(info->m_rank, spell->m_nameSubtext, 0x7FFFFFFF)))) {
            AuraData data;
            AuraData* auraData = NULL;

            if (entry->m_flags & 0x40) {
                data.m_effectAmounts[0] = entry->m_effectAmounts[0];
                data.m_effectAmounts[1] = entry->m_effectAmounts[1];
                data.m_effectAmounts[2] = entry->m_effectAmounts[2];
                auraData = &data;
            }

            tooltip->SetBuff(entry->m_spellId, 0, 1, 0, auraData);
            tooltip->Show();
            return 0;
        }

        found++;
    }

    return 0;
}

// OFFSET: 0x0046E980/0x1400847A0
int32_t CGTooltip::SetItemDelta(const CGItemStatsSummary& summary, int32_t append) {
    if (!append) {
        this->ClearTooltip();
    }

    int32_t first = 1;
    char text[1024];

    if (fabs(summary.m_dps) >= 0.00000023841858f) {
        this->AddDeltaHeader(first);
        SStrPrintf(text, sizeof(text), "|c%s%+.1f|r %s", summary.m_dps > 0.0f ? s_positiveDeltaColorString : s_negativeDeltaColorString, static_cast<double>(summary.m_dps), FrameScript_GetText("ITEM_MOD_DAMAGE_PER_SECOND_SHORT", -1, GENDER_NOT_APPLICABLE));
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    for (uint32_t stat = 0; stat < 89; stat++) {
        if (stat < 81 && (stat < 62 || stat > 67) && stat != 49 && stat != 50) {
            int32_t value = summary.m_stats[stat];
            this->AddColoredItemStat(value, FrameScript_GetText(CGItemStatsSummary::GetStatIntGlobalString(stat, text, sizeof(text)), -1, GENDER_NOT_APPLICABLE), first);
        }
    }

    for (uint32_t stat = 81; stat < 89; stat++) {
        if (!summary.m_stats[stat]) {
            continue;
        }

        char name[1024];
        const char* socket = FrameScript_GetText(CGItemStatsSummary::GetStatIntGlobalString(stat, name, sizeof(name)), -1, GENDER_NOT_APPLICABLE);
        SStrPrintf(text, sizeof(text), "|TInterface\\ItemSocketingFrame\\UI-EmptySocket-%s.blp:12|t  %s", s_socketIdentifiers[stat - 81], socket);
        int32_t value = summary.m_stats[stat];

        if (value) {
            if (first) {
                this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                this->AddLine(FrameScript_GetText("ITEM_DELTA_DESCRIPTION", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
                first = 0;
            }

            char line[512];
            SStrPrintf(line, sizeof(line), "|c%s%+d|r %s", value > 0 ? s_positiveDeltaColorString : s_negativeDeltaColorString, value, text);
            this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    }

    return 1;
}

// OFFSET: 0x0046EBE0
int32_t CGItemStatsSummary::Create(const ItemRecSparse* item, const TooltipExtendedItemInfo* info, int32_t level) {
    this->Clear();

    if (item->m_scalingStatDistribution) {
        this->FillOutScalingItem(item, level);
        this->CleanStatNames();
        return 1;
    }

    this->ParseBasicStats(item);
    this->ScanEnchantSpells(item);
    this->ParseResistances(item);
    this->GetSocketFlags(item);
    this->ParseWeaponDPS(item);

    if (info) {
        this->FillOutRandomProperties(info->m_randomPropertiesId, info->m_propertySeed);
    }

    this->CleanStatNames();
    return 1;
}

// OFFSET: 0x0046F130/0x140085130
int32_t CGTooltip::SetUnit(const WOWGUID& guid, int32_t showHealth, const WOWGUID& healthGuid, int32_t a4) {
    this->ClearTooltip();
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!healthGuid && unit && (unit->m_auraEffectMask[34] & 0x80)) {
        for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
            AuraData* aura = unit->GetAura(i);
            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(aura->m_spellId, count);

            for (uint32_t j = 0; j < count; j++) {
                const SpellEffectRec* effect = effects->m_data[j];

                if (effect && effect->m_effectAura == 279 && (aura->m_flags & (1 << j))) {
                    return this->SetUnit(aura->m_creator, showHealth, guid, 0);
                }
            }
        }
    }

    PartyMemberState* state = CGGameUI::GetPartyMemberState(guid);
    PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(guid);
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);
    NameCache* name = NULL;

    if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) || static_cast<uint32_t>(guid.m_guid))) {
        WOWGUID requester = WOWGUID::Make(0);
        name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);
    }

    CreatureStats_C* creature = NULL;

    if (unit) {
        creature = unit->m_creatureCacheEntry;
    } else if (!state && !petState) {
        WOWGUID requester = WOWGUID::Make(0);

        if (((high & 0xF0F00000) != 0xF0300000 && (high & 0xF0F00000) != 0xF0500000) || !(creature = g_creatureDBCache.GetRecord(high & 0xFFFFF, requester, NULL, NULL, false))) {
            if (!name) {
                this->Hide();
                return 0;
            }
        }

        showHealth = 0;
    }

    this->m_unitGuid = guid;

    if (this->m_statusBar && showHealth) {
        this->m_healthGuid = healthGuid ? healthGuid : this->m_unitGuid;
        CGUnit_C* healthUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_healthGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
        ClntObjMgrSetObjMirrorHandler(this->m_healthGuid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), sizeof(int32_t), HealthUpdateHandler, this->m_statusBar, static_cast<HANDLER_PRIORITY>(0), 0);
        float maxHealth;

        if (healthUnit) {
            maxHealth = static_cast<float>(static_cast<int32_t>(healthUnit->m_unit->m_unitFieldMaxhealth));
        } else if (state) {
            maxHealth = static_cast<float>(static_cast<int32_t>(state->m_maxHealth));
        } else {
            maxHealth = static_cast<float>(static_cast<int32_t>(petState->m_maxHealth));
        }

        this->m_statusBar->SetMinMaxValues(0.0f, maxHealth);
        float health;

        if (healthUnit) {
            health = (healthUnit->m_unit->m_unitDynamicFlags & 0x20) ? 0.0f : static_cast<float>(static_cast<int32_t>(healthUnit->m_unit->m_unitFieldHealth));
        } else if (state) {
            health = static_cast<float>(static_cast<int32_t>(state->m_health));
        } else {
            health = static_cast<float>(static_cast<int32_t>(petState->m_health));
        }

        this->m_statusBar->SetValue(health);
        this->m_statusBar->Show();
    }

    char line[1024];

    if (unit) {
        const char* realm = NULL;
        unit->GetUnitName(&realm, 1);
        char title[1024];

        if (!realm || !*realm) {
            SStrPrintf(title, sizeof(title), unit->GetUnitChosenTitle(line, sizeof(line), 1, NULL));
        } else {
            SStrPrintf(title, sizeof(title), "%s - %s", unit->GetUnitChosenTitle(line, sizeof(line), 1, NULL), realm);
        }

        this->AddLine(title, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && unit->m_obj->m_dataType == 1 && unit->m_obj->m_data) {
            WOWGUID requester = unit->m_obj->m_guid;
            GuildStats_C* guild = g_guildInfoCache.GetRecord(unit->m_obj->m_data.m_guid, requester, TooltipUnitNameCallback, this, false);

            if (guild) {
                this->AddLine(guild->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }
    } else if (name) {
        if (name->m_realmName[0]) {
            SStrPrintf(line, sizeof(line), "%s - %s", static_cast<const char*>(name->m_name), name->m_realmName);
            this->AddLine(line, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        } else {
            this->AddLine(name->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
        }
    } else if (petState) {
        this->AddLine(petState->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    } else if (creature) {
        this->AddLine(creature->m_name[0], NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    }

    CVar* colorblind = CVar::Lookup("colorblindMode");

    if (colorblind && colorblind->m_intValue) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && unit) {
            line[0] = '\0';
            FRAMESCRIPT_GENDER gender = player->m_unit->m_unitFieldBytes0[2] == 1 ? GENDER_FEMALE : GENDER_MALE;
            SStrPrintf(line, sizeof(line), "FACTION_STANDING_LABEL%d", unit->UnitReaction(player) + 1);
            this->AddLine(FrameScript_GetText(line, 0, gender), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    }

    const char* subName = NULL;

    if (unit && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        subName = unit->GetUnitTitle();
    } else if (creature) {
        subName = creature->m_subName;
    }

    if (subName && *subName) {
        this->AddLine(subName, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    line[0] = '\0';
    CGTooltip::GetSummonedByString(unit, line, sizeof(line));

    if (line[0]) {
        this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    char race[128] = {};
    char className[128] = {};
    char type[64] = {};
    char level[32];
    CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t dead = 0;
    CreatureTypeRec* creatureType = NULL;
    int32_t unitLevel = 0;
    bool showLevel = true;

    if (unit) {
        dead = static_cast<int32_t>(unit->m_unit->m_unitFieldHealth) <= 0 || (unit->m_unit->m_unitDynamicFlags & 0x20);
        unitLevel = unit->m_unit->m_unitFieldLevel;

        if (unit->GetCreatureType() != 10 && activePlayer && !unit->IsFriendly(activePlayer)) {
            creatureType = g_creatureTypeDB.GetRecord(unit->GetCreatureType());
        }

        if (activePlayer && unit->UnitReaction(activePlayer) <= 1 && static_cast<int32_t>(activePlayer->m_unit->m_unitFieldLevel) <= unitLevel - 10) {
            showLevel = false;
        } else if (unit->IsBossMob()) {
            showLevel = false;
        }
    } else if (state) {
        dead = (state->m_flags & 0x1) && static_cast<int32_t>(state->m_health) <= 0;
        unitLevel = state->m_level;
    } else {
        showLevel = false;

        if (petState) {
            dead = static_cast<int32_t>(petState->m_health) <= 0;
        } else if (creature && creature->m_type != 10) {
            creatureType = g_creatureTypeDB.GetRecord(creature->m_type);
        }
    }

    if (showLevel && unitLevel > 0) {
        if (activePlayer && (activePlayer->m_player->m_playerBytes3[1] || activePlayer->m_player->m_playerFakeInebriation) && unit && unit->CanAttack(activePlayer, 0)) {
            unitLevel -= RoundToInt64(activePlayer->GetDrunknessTurnValue() * 5.0f);

            if (unitLevel < 1) {
                unitLevel = 1;
            }
        }

        SStrPrintf(level, sizeof(level), "%d", unitLevel);
    } else {
        SStrCopy(level, "??", sizeof(level));
    }

    if (dead) {
        SStrCopy(className, FrameScript_GetText("CORPSE", -1, GENDER_NOT_APPLICABLE), sizeof(className));
    } else if ((unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) || name) {
        const char* raceName = CGUnit_C::GetDisplayRaceName(unit, name);
        const char* classNameText = CGUnit_C::GetDisplayClassName(unit, name);

        if (raceName && classNameText) {
            SStrCopy(race, raceName, sizeof(race));
            SStrCopy(className, classNameText, sizeof(className));
        }
    } else if (creatureType) {
        SStrCopy(className, creatureType->m_name, sizeof(className));
    }

    const char* classification = NULL;

    if (unit) {
        classification = unit->IsBossMob() ? "BOSS" : s_classificationTokens[unit->GetCreatureClassification()];
    } else if (creature) {
        classification = (creature->m_typeFlags[0] & 0x4) ? "BOSS" : s_classificationTokens[creature->m_rank];
    }

    if ((unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) || name) {
        SStrCopy(type, FrameScript_GetText("PLAYER", -1, GENDER_NOT_APPLICABLE), sizeof(type));
    } else if (classification) {
        SStrCopy(type, FrameScript_GetText(classification, -1, GENDER_NOT_APPLICABLE), sizeof(type));
    }

    CreatureStats_C* unitCreature = unit ? unit->m_creatureCacheEntry : NULL;

    if (!unitCreature || !(unitCreature->m_typeFlags[1] & 0x2)) {
        if (race[0]) {
            if (className[0]) {
                if (type[0]) {
                    SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_RACE_CLASS_TYPE", -1, GENDER_NOT_APPLICABLE), level, race, className, type);
                } else {
                    SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_RACE_CLASS", -1, GENDER_NOT_APPLICABLE), level, race, className);
                }
            } else if (type[0]) {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_TYPE", -1, GENDER_NOT_APPLICABLE), level, type);
            } else {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL", -1, GENDER_NOT_APPLICABLE), level);
            }
        } else if (className[0]) {
            if (type[0]) {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_CLASS_TYPE", -1, GENDER_NOT_APPLICABLE), level, className, type);
            } else {
                SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_CLASS", -1, GENDER_NOT_APPLICABLE), level, className);
            }
        } else if (type[0]) {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL_TYPE", -1, GENDER_NOT_APPLICABLE), level, type);
        } else {
            SStrPrintf(line, sizeof(line), FrameScript_GetText("TOOLTIP_UNIT_LEVEL", -1, GENDER_NOT_APPLICABLE), level);
        }

        this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    if (!unit && state && !(state->m_flags & 0x1) && !a4) {
        this->AddLine(FrameScript_GetText("PLAYER_OFFLINE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        this->RunOnTooltipSetUnit();
        this->Show();
        return 0;
    }

    if (activePlayer && unit && unit->ShowFactionTooltip()) {
        FactionTemplateRec* factionTemplate = g_factionTemplateDB.GetRecord(unit->m_unit->m_unitFieldFactiontemplate);
        FactionRec* faction = factionTemplate ? g_factionDB.GetRecord(factionTemplate->m_faction) : NULL;

        if (faction && faction->m_reputationIndex >= 0) {
            for (uint32_t i = 0; i < 4; i++) {
                uint32_t raceMask = faction->m_reputationRaceMask[i];
                uint32_t classMask = faction->m_reputationClassMask[i];

                if (raceMask) {
                    if (!((1 << (activePlayer->m_unit->m_unitFieldBytes0[0] - 1)) & raceMask)) {
                        continue;
                    }
                } else if (!classMask) {
                    continue;
                }

                if (!classMask || ((1 << (activePlayer->m_unit->m_unitFieldBytes0[1] - 1)) & classMask)) {
                    if (!(faction->m_reputationFlags[i] & 0x4)) {
                        this->AddLine(faction->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                    }

                    break;
                }
            }
        }
    }

    if (unit) {
        if (((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x100)) || (unit->m_unit->m_unitFieldBytes2[1] & 0x1)) {
            this->AddLine(FrameScript_GetText("PVP_ENABLED", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        if (static_cast<int32_t>(unit->m_unit->m_unitFieldHealth) <= 0 && (unit->m_obj->m_type & TYPEMASK_PLAYER) && activePlayer && unit->CanAssist(activePlayer, false)) {
            this->AddLine(FrameScript_GetText("RESURRECTABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        }

        if (unit->m_unit->m_unitDynamicFlags & 0x10) {
            this->AddEmpathyText(unit, line, sizeof(line));
        }

        if ((unit->m_unit->m_unitFieldFlags & 0x4000000) && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            this->AddSkinnableText(unit, line, sizeof(line));
        }
    }

    if (CGGameUI::IsPartyMember(guid) && guid != ClntObjMgrGetActivePlayer() && !unit) {
        if (state && AreaListGetName(state->m_zoneID, line, sizeof(line), 0)) {
            this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    } else if (unit && unit->IsPVPLeader()) {
        this->AddLine(FrameScript_GetText("PVP_RANK_LEADER", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    CVar* questTracking = CVar::Lookup("showQuestTrackingTooltips");

    if (creature && !(unit && unit->m_unit->m_unitFieldSummonedby && !(static_cast<uint32_t>(unit->m_unit->m_unitFieldSummonedby.m_guid >> 32) & 0xF0000000)) && questTracking && questTracking->m_intValue) {
        uint32_t entry = unit ? unit->m_obj->m_entryID : (high & 0xFFFFF);
        QuestLogLeaderBoardParams params;
        params.m_questCallback = TooltipUnitNameCallback;
        params.m_objectCallback = TooltipUnitNameCallback;
        params.m_creatureCallback = TooltipUnitNameCallback;
        params.m_itemCallback = TooltipUnitNameCallback;
        params.m_param = this;
        int32_t quests[250];
        CGQuestLog::GetAllOfGroupsQuests(quests);
        char buffer[4099];

        for (uint32_t i = 0; i < 250; i++) {
            int32_t questId = quests[i];

            if (!questId) {
                break;
            }

            bool header = false;
            WOWGUID requester = WOWGUID::Make(0);
            QuestCache* quest = g_questDBCache.GetRecord(questId, requester, TooltipUnitNameCallback, this, true);

            if (!quest) {
                continue;
            }

            for (uint32_t member = 0; member < CGPartyInfo::NumMembers() + 1; member++) {
                CGPlayer_C* player = member ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::GetMember(member - 1), TYPEMASK_PLAYER, __FILE__, __LINE__)) : static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

                if (!player || !CGQuestLog::IsOnQuest(player, quest->m_id)) {
                    continue;
                }

                if (CGQuestLog::GetQuestLeaderBoardForKill(player, quest->m_id, entry, &params)) {
                    if (!header) {
                        this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                        header = true;
                    }

                    if (CGPartyInfo::NumMembers()) {
                        SStrPrintf(buffer, sizeof(buffer), " %s - %s", player->GetVisibleUnitName(NULL, 1), static_cast<const char*>(params.m_text));
                    } else {
                        SStrPrintf(buffer, sizeof(buffer), " - %s", static_cast<const char*>(params.m_text));
                    }

                    this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }

                for (uint32_t j = 0; j < 2; j++) {
                    if (creature->m_killCredit[j] && CGQuestLog::GetQuestLeaderBoardForKill(player, quest->m_id, creature->m_killCredit[j], &params)) {
                        if (!header) {
                            this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                            header = true;
                        }

                        if (CGPartyInfo::NumMembers()) {
                            SStrPrintf(buffer, sizeof(buffer), " %s - %s", player->GetVisibleUnitName(NULL, 1), static_cast<const char*>(params.m_text));
                        } else {
                            SStrPrintf(buffer, sizeof(buffer), " - %s", static_cast<const char*>(params.m_text));
                        }

                        this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                    }
                }

                for (uint32_t j = 0; j < 6; j++) {
                    int32_t itemId = creature->m_questItem[j];

                    if (!itemId) {
                        break;
                    }

                    if (CGQuestLog::GetQuestLeaderBoardForItem(player, quest->m_id, itemId, &params)) {
                        if (!header) {
                            this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                            header = true;
                        }

                        if (CGPartyInfo::NumMembers()) {
                            SStrPrintf(buffer, sizeof(buffer), " %s - %s", player->GetVisibleUnitName(NULL, 1), static_cast<const char*>(params.m_text));
                        } else {
                            SStrPrintf(buffer, sizeof(buffer), " - %s", static_cast<const char*>(params.m_text));
                        }

                        this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                    }

                    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer() && CGQuestLog::IsQuestComplete(player, CGQuestLog::GetQuestIndex(questId) - 1)) {
                        continue;
                    }

                    for (uint32_t k = 0; k < 4; k++) {
                        if (quest->m_intermediateItemId[k] != itemId) {
                            continue;
                        }

                        if (!header) {
                            this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                            header = true;
                        }

                        WOWGUID itemRequester = WOWGUID::Make(0);
                        ItemRecSparse* item = g_itemSparseDB.GetRecord(quest->m_intermediateItemId[k], itemRequester, TooltipUnitNameCallback, this, false);

                        if (item) {
                            if (CGPartyInfo::NumMembers()) {
                                SStrPrintf(buffer, sizeof(buffer), " %s - %s", player->GetVisibleUnitName(NULL, 1), item->GetName(0));
                            } else {
                                SStrPrintf(buffer, sizeof(buffer), " - %s", item->GetName(0));
                            }

                            this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                        }
                    }
                }
            }
        }
    }

    if (unit && dead) {
        if (unit->GetLooter()) {
            WOWGUID requester = WOWGUID::Make(0);
            NameCache* looter = g_nameDBCache.GetRecord(unit->GetLooter(), requester, TooltipUnitNameCallback, this, false);

            if (looter) {
                char buffer[64];
                SStrPrintf(buffer, sizeof(buffer), "%s: %s", FrameScript_GetText("MASTER_LOOTER", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(looter->m_name));
                this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }

        if (unit->GetMasterLooter()) {
            WOWGUID requester = WOWGUID::Make(0);
            NameCache* looter = g_nameDBCache.GetRecord(unit->GetMasterLooter(), requester, TooltipUnitNameCallback, this, false);

            if (looter) {
                char buffer[64];
                SStrPrintf(buffer, sizeof(buffer), "%s: %s", FrameScript_GetText("LOOT", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(looter->m_name));
                this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }
    }

    int32_t result = 0;

    if (activePlayer && CGGameUI::m_threatWarningEnabled && unit) {
        WOWGUID playerGuid = activePlayer->m_obj->m_guid;
        uint8_t status;
        uint8_t rawPercent;
        int32_t threat;

        if (unit->CalculateThreat(playerGuid, &status, &rawPercent, NULL, &threat)) {
            int32_t percent;

            if (activePlayer->m_obj->m_guid == unit->m_highestThreatGuid) {
                WOWGUID target = activePlayer->m_obj->m_guid;
                int32_t secondary = unit->GetHighestThreatExcluding(target);
                percent = secondary ? static_cast<int32_t>(static_cast<float>(threat) * 100.0f / static_cast<float>(secondary)) : 100;
            } else {
                percent = rawPercent;
            }

            SStrPrintf(line, sizeof(line), FrameScript_GetText("THREAT_TOOLTIP", -1, GENDER_NOT_APPLICABLE), percent);
            this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            uint32_t threatStatus = status >= 5 ? 1 : status;
            CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
            this->AddTexture("Interface\\RaidFrame\\UI-RaidFrame-Threat", coords, s_threatStatusColors[threatStatus]);
            result = 1;
        }
    }

    this->RunOnTooltipSetUnit();
    this->Show();
    return result;
}

// OFFSET: 0x004707F0/0x140086F70
const char* CGTooltip::GetItemHyperlinkString(const CGItem_C* item) {
    if (!item) {
        return NULL;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false);
    uint32_t wrapped = (item->m_item->m_itemFieldFlags >> 3) & 1;
    int32_t gems[3];

    for (uint32_t i = 0; i < 3; i++) {
        if (!wrapped && !(const_cast<CGItem_C*>(item)->GetItemStaticFlags(0) & 0x2000)) {
            gems[i] = item->m_item->m_itemFieldEnchantment[2 + i].m_id;
        } else {
            gems[i] = 0;
        }
    }

    int32_t enchant = 0;
    int32_t randomPropertiesId;
    int32_t seed;
    int32_t reforge;

    if (wrapped) {
        reforge = 0;
        seed = 0;
        randomPropertiesId = 0;
    } else {
        randomPropertiesId = item->m_item->m_itemFieldRandomPropertiesId;
        reforge = item->m_item->m_itemFieldEnchantment[8].m_id;
        seed = item->m_item->m_itemFieldPropertySeed;
        enchant = const_cast<CGItem_C*>(item)->GetItemStaticFlags(0) & 0x2000 ? 0 : item->m_item->m_itemFieldEnchantment[0].m_id;
    }

    return CGTooltip::GetItemHyperlinkString(item->m_obj->m_entryID, rec ? rec->m_quality : 1, enchant, gems, randomPropertiesId, seed, reforge);
}

// OFFSET: 0x00470910/0x140087110
void CGTooltip::AddSpellReagents(const SpellRec* spell, const char* token, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success)) {
    if (!spell->m_spellReagentsID) {
        return;
    }

    SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

    if (!reagents) {
        return;
    }

    char line[4096];
    char text[4096];
    char reagent[1024];
    int32_t first = 1;

    for (uint32_t i = 0; i < 8; i++) {
        int32_t itemId = reagents->m_reagent[i];

        if (itemId <= 0) {
            continue;
        }

        WOWGUID guid;
        guid.m_guid = (static_cast<uint64_t>(0x1FE00000) << 32) | static_cast<uint32_t>(spell->m_id);
        ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, guid, callback, this, true);

        if (!item) {
            continue;
        }

        if (first) {
            first = 0;
            text[0] = '\0';
        } else {
            SStrPack(text, ", ", sizeof(text));
        }

        if (reagents->m_reagentCount[i] <= 1) {
            SStrCopy(reagent, item->GetName(0), sizeof(reagent));
        } else {
            SStrPrintf(reagent, sizeof(reagent), "%s (%d)", item->GetName(0), reagents->m_reagentCount[i]);
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && player->m_bag.GetItemTypeCount(reagents->m_reagent[i], 0) >= reagents->m_reagentCount[i]) {
            SStrPack(text, reagent, sizeof(text));
        } else {
            SStrPack(text, "|c", sizeof(text));
            SStrPack(text, s_tooltipRedColorString, sizeof(text));
            SStrPack(text, reagent, sizeof(text));
            SStrPack(text, "|r", sizeof(text));
        }
    }

    if (!first) {
        SStrPrintf(line, sizeof(line), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), text);
        this->AddLine(line, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
    }
}

// OFFSET: 0x00470C60/0x140087580
int32_t CGTooltip::SetSpell(int32_t spellId, int32_t brief, uint32_t cooldown, int32_t isPet, int32_t showSubtext, int32_t isInspect, int32_t hideCallPetName, const TalentRec* talent, const CGTalentGroup_C* talentGroup, int32_t append, int32_t usePreview, int32_t a12, int32_t rank, int32_t maxRank, int32_t a15, int32_t noCastTime, int32_t noCost) {
    if (!append) {
        this->ClearTooltip();
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        this->Hide();
        return 0;
    }

    int32_t id = Spell_C_SpellGetSpellIDForDifficulty(spellId);
    SpellRec* spell = g_spellDB.GetRecord(id);

    if (!spell) {
        if (append) {
            return 0;
        }

        this->Hide();
        return 0;
    }

    if (!append) {
        this->m_spellId = id;
        this->m_pendingQueries = 0;
    }

    CGUnit_C* caster;

    if (isInspect) {
        caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGCharacterInfo::m_inspectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));
    } else if (isPet) {
        caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    } else {
        caster = player;
    }

    uint32_t effectCount;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
    const SpellEffectRec* firstEffect = NULL;
    int32_t firstEffectType = 0;

    if (effectCount && effects->m_data[0]) {
        firstEffect = effects->m_data[0];
        firstEffectType = firstEffect->m_effect;
    }

    int32_t tradeskill = 0;
    int32_t createItem = 0;

    if ((spell->m_attributes & 0x20) && effectCount) {
        for (uint32_t i = 0; i < effectCount; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect) {
                continue;
            }

            if (effect->m_effect == 53) {
                tradeskill = 1;
                break;
            }

            if (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59) {
                tradeskill = 1;
                createItem = 1;
            }
        }
    }

    char cost[256];
    char range[256];
    char runeText[256];
    char token[128];
    char number[32];

    if (talent && append) {
        SStrPrintf(cost, sizeof(cost), "\n%s", FrameScript_GetText("TOOLTIP_TALENT_NEXT_RANK", -1, GENDER_NOT_APPLICABLE));
        this->AddLine(cost, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    } else if (tradeskill) {
        SkillLineAbilityRec* ability = SpellTableLookupAbility(player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[1], id);

        if (ability) {
            SkillLineRec* skill = g_skillLineDB.GetRecord(ability->m_skillLine);

            if (skill) {
                SStrPrintf(cost, sizeof(cost), "%s: %s", static_cast<const char*>(skill->m_displayName), static_cast<const char*>(spell->m_name));
                this->AddLine(cost, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
            }
        }
    } else {
        uint32_t slot = CGStableInfo::FindPetIndexBySpellID(id, 0);
        const StablePetInfo* stable = CGStableInfo::GetPetInfo(slot);
        const char* left;
        const char* right;

        if (!hideCallPetName && stable && stable->m_name[0]) {
            SStrPrintf(token, sizeof(token), FrameScript_GetText("CALL_PET_SPELL_NAME", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(stable->m_name));
            right = brief || showSubtext ? static_cast<const char*>(spell->m_nameSubtext) : NULL;
            left = token;
        } else {
            right = brief || showSubtext ? static_cast<const char*>(spell->m_nameSubtext) : NULL;
            left = spell->m_name;
        }

        this->AddLine(left, right, s_tooltipTextColor, s_tooltipGrayColor, 0);
    }

    int32_t prereqsMet = 1;

    if (talent && !append) {
        if (maxRank >= 0) {
            SStrPrintf(cost, sizeof(cost), FrameScript_GetText("TOOLTIP_TALENT_RANK", -1, GENDER_NOT_APPLICABLE), rank + 1, maxRank + 1);
            this->AddLine(cost, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        if (rank < 0) {
            prereqsMet = this->AddTalentPrereqs(talent, talentGroup, isInspect, isPet, usePreview);
        }
    }

    cost[0] = '\0';
    range[0] = '\0';
    PowerDisplayRec* powerDisplay = NULL;
    int32_t powerBar = 0;

    if (spell->m_spellPowerID) {
        SpellPowerRec* power = g_spellPowerDB.GetRecord(spell->m_spellPowerID);

        if (power) {
            powerBar = power->m_unitPowerBarID;
            powerDisplay = g_powerDisplayDB.GetRecord(power->m_powerDisplayID);
        }
    }

    int32_t powerCost = Spell_C_GetPowerCost(spell, caster);

    if (!brief && !noCost && !(spell->m_attributesExI & 0x8000)) {
        int32_t perSecond = Spell_C_GetPowerCostPerSecond(spell, caster);
        powerCost /= static_cast<int32_t>(Spell_C_GetPowerDisplayMod(spell->m_powerType));
        perSecond /= static_cast<int32_t>(Spell_C_GetPowerDisplayMod(spell->m_powerType));
        const char* costToken = NULL;
        uint32_t powerType = spell->m_powerType;

        if (powerType > 10) {
            costToken = "HEALTH_COST";
        } else if (powerType == 8) {
            if (powerCost < 0) {
                costToken = s_balanceNegativeEnergyCostToken;
                powerCost = -powerCost;
            } else {
                costToken = s_balancePositiveEnergyCostToken;
            }
        } else if (powerType == 10) {
            UnitPowerBarRec* bar;

            if (caster && powerBar && (bar = g_unitPowerBarDB.GetRecord(powerBar)) != NULL) {
                SStrCopy(runeText, "%d ", sizeof(runeText));
                SStrPack(runeText, bar->m_costString, sizeof(runeText));
                costToken = runeText;
            } else {
                powerCost = 0;
                perSecond = 0;
            }
        } else {
            if (powerCost < 0) {
                powerCost = 0;
                perSecond = 0;
            }

            costToken = s_powerCostTokens[powerType];
        }

        if (spell->m_powerType == 5) {
            SpellRuneCostRec* runeCost = g_spellRuneCostDB.GetRecord(spell->m_runeCostID);

            if (runeCost) {
                if (runeCost->m_blood) {
                    SStrPrintf(token, sizeof(token), FrameScript_GetText("RUNE_COST_BLOOD", -1, GENDER_NOT_APPLICABLE), runeCost->m_blood);
                    SStrPack(cost, token, sizeof(cost));

                    if (runeCost->m_unholy || runeCost->m_frost) {
                        SStrPack(cost, " ", sizeof(cost));
                    }
                }

                if (runeCost->m_unholy) {
                    SStrPrintf(token, sizeof(token), FrameScript_GetText("RUNE_COST_UNHOLY", -1, GENDER_NOT_APPLICABLE), runeCost->m_unholy);
                    SStrPack(cost, token, sizeof(cost));

                    if (runeCost->m_frost) {
                        SStrPack(cost, " ", sizeof(cost));
                    }
                }

                if (runeCost->m_frost) {
                    SStrPrintf(token, sizeof(token), FrameScript_GetText("RUNE_COST_FROST", -1, GENDER_NOT_APPLICABLE), runeCost->m_frost);
                    SStrPack(cost, token, sizeof(cost));
                }
            }
        } else if (powerCost) {
            if (perSecond > 0) {
                if (powerDisplay) {
                    FormatString(powerDisplay->m_globalStringBaseTag, -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                    SStrPrintf(cost, sizeof(cost), FrameScript_GetText("POWER_DISPLAY_COST_PER_TIME", -1, GENDER_NOT_APPLICABLE), powerCost, token, perSecond);
                } else {
                    SStrPrintf(token, sizeof(token), "%s_PER_TIME", costToken);
                    SStrPrintf(cost, sizeof(cost), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), powerCost, perSecond);
                }
            } else if (powerDisplay) {
                FormatString(powerDisplay->m_globalStringBaseTag, -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                SStrPrintf(cost, sizeof(cost), FrameScript_GetText("POWER_DISPLAY_COST", -1, GENDER_NOT_APPLICABLE), powerCost, token);
            } else if (powerBar) {
                SStrPrintf(cost, sizeof(cost), costToken, powerCost);
            } else {
                FormatString(costToken, -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                SStrPrintf(cost, sizeof(cost), token, powerCost);
            }
        } else if (perSecond > 0) {
            if (powerDisplay) {
                FormatString(powerDisplay->m_globalStringBaseTag, -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                SStrPrintf(cost, sizeof(cost), FrameScript_GetText("POWER_DISPLAY_COST_PER_TIME", -1, GENDER_NOT_APPLICABLE), powerCost, token, perSecond);
            } else {
                SStrPrintf(token, sizeof(token), "%s_PER_TIME", costToken);
                SStrPrintf(cost, sizeof(cost), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), powerCost, perSecond);
            }
        }
    }

    float minRange = 0.0f;
    float maxRange = 0.0f;
    float friendlyMinRange = 0.0f;
    float friendlyMaxRange = 0.0f;

    if (brief || (spell->m_attributes & 0x404) || (spell->m_attributesExC & 0x40000000)) {
        if (!cost[0]) {
            SStrCopy(cost, range, sizeof(cost));
            range[0] = '\0';
        }
    } else if (SpellRec::IsMeleeRange(spell) && !Spell_C_HasSpellModifiers(spell, 5)) {
        FormatString("MELEE_RANGE", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
        SStrPrintf(range, sizeof(range), token);

        if (!cost[0]) {
            SStrCopy(cost, range, sizeof(cost));
            range[0] = '\0';
        }
    } else {
        Spell_C_GetMinMaxRange(caster, spell, &minRange, &maxRange, 0, 0);
        Spell_C_GetMinMaxRange(caster, spell, &friendlyMinRange, &friendlyMaxRange, 1, 0);

        if (SpellRec::IsMinimumRange(spell)) {
            Spell_C_GetDefaultMinRange(spell, &minRange);
            Spell_C_GetDefaultMinRange(spell, &friendlyMinRange);
        }

        if (!fnotequal(minRange, friendlyMinRange) && !fnotequal(maxRange, friendlyMaxRange)) {
            if (maxRange > 0.0f) {
                if (maxRange < 50000.0f) {
                    if (minRange <= 0.0f) {
                        SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(maxRange));
                    } else {
                        SStrPrintf(number, sizeof(number), "%d-%d", static_cast<int32_t>(minRange), static_cast<int32_t>(maxRange));
                    }

                    FormatString("SPELL_RANGE", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                    SStrPrintf(range, sizeof(range), token, number);
                } else {
                    SStrPrintf(range, sizeof(range), FrameScript_GetText("SPELL_RANGE_UNLIMITED", -1, GENDER_NOT_APPLICABLE));
                }
            }

            if (!cost[0]) {
                SStrCopy(cost, range, sizeof(cost));
                range[0] = '\0';
            }
        } else {
            if (maxRange > 0.0f) {
                if (maxRange < 50000.0f) {
                    if (minRange <= 0.0f) {
                        SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(maxRange));
                    } else {
                        SStrPrintf(number, sizeof(number), "%d-%d", static_cast<int32_t>(minRange), static_cast<int32_t>(maxRange));
                    }

                    FormatString("SPELL_RANGE_DUAL", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                    SStrPrintf(range, sizeof(range), token, FrameScript_GetText("ENEMY", -1, GENDER_NOT_APPLICABLE), number);
                } else {
                    SStrPrintf(range, sizeof(range), FrameScript_GetText("SPELL_RANGE_UNLIMITED", -1, GENDER_NOT_APPLICABLE));
                }
            }

            if (cost[0]) {
                this->AddLine(" ", range, s_tooltipTextColor, s_tooltipTextColor, 0);
            } else {
                SStrCopy(cost, range, sizeof(cost));
                range[0] = '\0';
                this->AddLine(cost, range, s_tooltipTextColor, s_tooltipTextColor, 0);
                cost[0] = '\0';
            }

            if (friendlyMaxRange > 0.0f) {
                if (friendlyMaxRange < 50000.0f) {
                    if (minRange <= 0.0f) {
                        SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(friendlyMaxRange));
                    } else {
                        SStrPrintf(number, sizeof(number), "%d-%d", static_cast<int32_t>(friendlyMinRange), static_cast<int32_t>(friendlyMaxRange));
                    }

                    FormatString("SPELL_RANGE_DUAL", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                    SStrPrintf(range, sizeof(range), token, FrameScript_GetText("FRIENDLY", -1, GENDER_NOT_APPLICABLE), number);
                } else {
                    SStrPrintf(range, sizeof(range), FrameScript_GetText("SPELL_RANGE_UNLIMITED", -1, GENDER_NOT_APPLICABLE));
                }
            }

            if (!cost[0]) {
                SStrCopy(cost, range, sizeof(cost));
                range[0] = '\0';
            }
        }
    }

    this->AddLine(cost, range, s_tooltipTextColor, s_tooltipTextColor, 0);

    int32_t recoveryTime = 0;
    int32_t categoryRecoveryTime = 0;
    int32_t passive = 0;

    if (spell->m_spellCooldownsID) {
        SpellCooldownsRec* cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);

        if (cooldowns) {
            recoveryTime = cooldowns->m_recoveryTime;
            categoryRecoveryTime = cooldowns->m_categoryRecoveryTime;
        }
    }

    Spell_C_ModifySpellValue(spell, recoveryTime, 11);
    Spell_C_ModifyCategoryRecovery(categoryRecoveryTime, spell, 0, 1);

    if (caster) {
        caster->SpellHasteCooldowns(spell, recoveryTime, categoryRecoveryTime);
    }

    int32_t recovery = recoveryTime <= categoryRecoveryTime ? categoryRecoveryTime : recoveryTime;

    if (!brief && !tradeskill) {
        if (firstEffectType == 47 || (spell->m_attributes & 0x40)) {
            passive = 1;
        } else if (spell->m_attributesEx >= 0 && firstEffectType != 78) {
            int32_t castTime = noCastTime ? 0 : Spell_C_GetCastTime(spell, isPet, isInspect, 1);

            if (castTime <= 0) {
                if (spell->m_attributesEx & 0x44) {
                    FormatString("SPELL_CAST_CHANNELED", -1, GENDER_NOT_APPLICABLE, cost, sizeof(cost));
                } else if (castTime >= 0 && (spell->m_attributes & 0x404)) {
                    FormatString("SPELL_ON_NEXT_SWING", -1, GENDER_NOT_APPLICABLE, cost, sizeof(cost));
                } else if (castTime >= 0 && (spell->m_powerType || powerCost <= 0)) {
                    FormatString("SPELL_CAST_TIME_INSTANT_NO_MANA", -1, GENDER_NOT_APPLICABLE, cost, sizeof(cost));
                } else {
                    FormatString("SPELL_CAST_TIME_INSTANT", -1, GENDER_NOT_APPLICABLE, cost, sizeof(cost));
                }
            } else {
                bool minutes = castTime >= 60000;
                FormatString(minutes ? "SPELL_CAST_TIME_MIN" : "SPELL_CAST_TIME_SEC", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                float divisor = minutes ? 60000.0f : 1000.0f;
                SStrPrintf(cost, sizeof(cost), token, static_cast<double>(castTime) / divisor);
                lua_convertdecimal(cost);
            }

            if (recovery <= 0 || (spell->m_attributesExF & 0x1)) {
                range[0] = '\0';
            } else {
                CGTooltip::GetFloatDurationString(range, sizeof(range), static_cast<float>(recovery), "SPELL_RECAST_TIME", NULL);
            }

            uint8_t used;
            uint8_t maxUses;
            int32_t category = Spell_C_GetSpellCategoryWeeklyUses(spell, used, maxUses);

            if (maxUses) {
                if (used >= maxUses) {
                    CGTooltip::GetDurationString(cost, sizeof(cost), 1000 * static_cast<uint64_t>(CGGameUI::GetWeeklyTimeRemaining()), "ITEM_COOLDOWN_TIME", NULL, 1, false);
                } else {
                    FormatString("SPELL_WEEKLY_USAGE_TOOLTIP", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                    SpellCategoryRec* categoryRec = g_spellCategoryDB.GetRecord(category);

                    if (categoryRec) {
                        SStrPrintf(cost, sizeof(cost), token, static_cast<const char*>(categoryRec->m_name), maxUses - used);
                    } else {
                        SStrPrintf(cost, sizeof(cost), token, "UNKNOWN", maxUses - used);
                    }
                }

                range[0] = '\0';
            }

            this->AddLine(cost, range, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    }

    char list[4096];
    char names[512];
    char line[512];
    char description[2048];
    int32_t allPresent = 1;

    if (!noCost && !isPet) {
        int32_t first = 1;

        if (spell->m_spellTotemsID) {
            SpellTotemsRec* totems = g_spellTotemsDB.GetRecord(spell->m_spellTotemsID);

            if (totems) {
                for (uint32_t i = 0; i < 2; i++) {
                    int32_t itemId = totems->m_totem[i];

                    if (itemId <= 0) {
                        continue;
                    }

                    WOWGUID guid;
                    guid.m_guid = 0x1FE0000000000000ULL | static_cast<uint32_t>(spell->m_id);
                    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, guid, TooltipSpellCallback, this, true);

                    if (!item) {
                        this->m_pendingQueries++;
                        continue;
                    }

                    if (first) {
                        first = 0;
                        FormatString("SPELL_TOTEMS", -1, GENDER_NOT_APPLICABLE, list, sizeof(list));
                    } else {
                        SStrPack(list, ", ", sizeof(list));
                    }

                    CGItem_C* found = player->m_bag.FindItemOfType(totems->m_totem[i], 0);

                    if (!found) {
                        SStrPack(list, "|c", sizeof(list));
                        SStrPack(list, s_tooltipRedColorString, sizeof(list));
                        allPresent = 0;
                    }

                    SStrPack(list, item->GetName(0), sizeof(list));

                    if (!found) {
                        SStrPack(list, "|r", sizeof(list));
                    }
                }

                if (!(spell->m_attributesExI & 0x20)) {
                    for (uint32_t i = 0; i < 2; i++) {
                        TotemCategoryRec* category = g_totemCategoryDB.GetRecord(totems->m_requiredTotemCategoryID[i]);

                        if (!category) {
                            continue;
                        }

                        if (first) {
                            first = 0;
                            FormatString("SPELL_TOTEMS", -1, GENDER_NOT_APPLICABLE, list, sizeof(list));
                        } else {
                            SStrPack(list, ", ", sizeof(list));
                        }

                        if (!player->m_bag.FindTotemCategory(totems->m_requiredTotemCategoryID[i], 0)) {
                            SStrPack(list, "|c", sizeof(list));
                            SStrPack(list, s_tooltipRedColorString, sizeof(list));
                            SStrPack(list, category->m_name, sizeof(list));
                            SStrPack(list, "|r", sizeof(list));
                            allPresent = 0;
                        } else {
                            SStrPack(list, category->m_name, sizeof(list));
                        }
                    }
                }

                if (!first && (!allPresent || !brief)) {
                    this->AddLine(list, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }
            }
        }
    }

    uint32_t targetFlags = 0;

    if (spell->m_spellTargetRestrictionsID) {
        SpellTargetRestrictionsRec* restrictions = g_spellTargetRestrictionsDB.GetRecord(spell->m_spellTargetRestrictionsID);

        if (restrictions) {
            targetFlags = restrictions->m_targets;
        }
    }

    if (!(targetFlags & 0x10) && spell->m_spellEquippedItemsID && firstEffectType != 60) {
        SpellEquippedItemsRec* equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID);

        if (equipped && equipped->m_equippedItemClass >= 0 && equipped->m_equippedItemSubclass) {
            names[0] = '\0';
            int32_t foundMask = 0;

            for (uint32_t i = 0; i < static_cast<uint32_t>(g_itemSubClassMaskDB.m_numRecords); i++) {
                ItemSubClassMaskRec* mask = g_itemSubClassMaskDB.GetRecordByIndex(i);

                if (mask->m_classID == equipped->m_equippedItemClass && mask->m_mask == equipped->m_equippedItemSubclass) {
                    SStrCopy(names, mask->m_name, sizeof(names));
                    foundMask = 1;
                    break;
                }
            }

            int32_t first = 1;
            int32_t hasItem = 1;
            allPresent = 1;
            uint32_t inventoryMask;

            if (spell->m_attributesExC & 0x400) {
                inventoryMask = 0x8000;
            } else {
                inventoryMask = (spell->m_attributesExC & 0x1000000) ? 0x10000 : 0xFFFFFFFF;
            }

            if (!player->GetEquippedItemForSpell(equipped, inventoryMask)) {
                hasItem = 0;
                allPresent = 0;
            }

            if ((spell->m_attributesExC & 0x1000400) == 0x1000400 && !player->GetEquippedItemForSpell(equipped, 0x10000)) {
                hasItem = 0;
                allPresent = 0;
            }

            for (uint32_t i = 0; i < static_cast<uint32_t>(g_itemSubClassDB.m_numRecords); i++) {
                ItemSubClassRec* subclass = g_itemSubClassDB.GetRecordByIndex(i);

                if (subclass && subclass->m_classID == equipped->m_equippedItemClass && ((1 << subclass->m_subClassID) & equipped->m_equippedItemSubclass) && !foundMask) {
                    if (first) {
                        first = 0;
                    } else {
                        SStrPack(names, ", ", sizeof(names));
                    }

                    const char* name = subclass->m_verboseName && *subclass->m_verboseName ? subclass->m_verboseName : subclass->m_displayName;
                    SStrPack(names, name, sizeof(names));
                }
            }

            hasItem = allPresent;

            if (isPet) {
                hasItem = 1;
            }

            if (names[0] && (!hasItem || !brief)) {
                FormatString(hasItem || !brief ? "SPELL_EQUIPPED_ITEM" : "SPELL_EQUIPPED_ITEM_NOSPACE", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                SStrPrintf(line, sizeof(line), token, names);
                const CImVector& color = hasItem ? s_tooltipTextColor : s_tooltipRedColor;
                this->AddLine(line, NULL, color, color, 1);
            }
        }
    }

    if (spell->m_spellShapeshiftID) {
        SpellShapeshiftRec* shapeshift = g_spellShapeshiftDB.GetRecord(spell->m_spellShapeshiftID);

        if (shapeshift && (shapeshift->m_shapeshiftMask[0] || shapeshift->m_shapeshiftMask[1])) {
            uint32_t notInForm = spell->m_attributesExB & 0x80000;
            int32_t first = 1;

            if (!notInForm) {
                uint32_t count = g_spellShapeshiftFormDB.m_numRecords > 64 ? 64 : g_spellShapeshiftFormDB.m_numRecords;

                for (uint32_t i = 0; i < count; i++) {
                    if (!Spell_C_SpellHasShapeshiftMask(spell, i)) {
                        continue;
                    }

                    SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecordByIndex(i);

                    if (!form || !form->m_name || !*form->m_name) {
                        continue;
                    }

                    if (first) {
                        SStrCopy(names, form->m_name, sizeof(names));
                    } else {
                        SStrPack(names, ", ", sizeof(names));
                        SStrPack(names, form->m_name, sizeof(names));
                    }

                    first = 0;
                }
            }

            bool inForm = !caster || Spell_C_SpellHasShapeshiftMask(spell, caster->GetShapeshiftForm() - 1) || (notInForm && !caster->IsShapeShifted()) || caster->HasAuraMatchingSpellClass(275, spell);

            if (!first && (!inForm || !brief)) {
                FormatString(inForm || !brief ? "SPELL_REQUIRED_FORM" : "SPELL_REQUIRED_FORM_NOSPACE", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
                SStrPrintf(cost, sizeof(cost), token, names);
                const CImVector& color = inForm ? s_tooltipTextColor : s_tooltipRedColor;
                this->AddLine(cost, NULL, color, color, 0);
            }
        }
    }

    if (spell->m_spellCastingRequirementsID) {
        SpellCastingRequirementsRec* requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID);

        if (requirements && requirements->m_minFactionID) {
            int32_t minReputation = requirements->m_minReputation;
            bool met = GetRepListRepValue(requirements->m_minFactionID) >= CGReputationInfo::m_reactionThresholds[minReputation];

            if (!met || !brief) {
                FactionRec* faction = g_factionDB.GetRecord(requirements->m_minFactionID);
                SStrPrintf(token, sizeof(token), "FACTION_STANDING_LABEL%d", minReputation + 1);
                const char* standing = player->GetFrameScriptText(token, -1);
                SStrPrintf(cost, sizeof(cost), FrameScript_GetText("ITEM_REQ_REPUTATION", -1, GENDER_NOT_APPLICABLE), faction ? static_cast<const char*>(faction->m_name) : "UNKNOWN", standing);
                const CImVector& color = met ? s_tooltipTextColor : s_tooltipRedColor;
                this->AddLine(cost, NULL, color, color, 0);
            }
        }
    }

    if (spell->m_spellLevelsID) {
        SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

        if (levels && passive && caster && static_cast<int32_t>(caster->m_unit->m_unitFieldLevel) < levels->m_baseLevel) {
            FormatString("ITEM_MIN_LEVEL", -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
            SStrPrintf(cost, sizeof(cost), token, levels->m_baseLevel);
            this->AddLine(cost, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
        }
    }

    if (!noCost && !isPet && !Spell_C_HasNoReagentCost(player, spell) && spell->m_spellReagentsID) {
        SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

        if (reagents) {
            int32_t first = 1;
            allPresent = 1;

            for (uint32_t i = 0; i < 8; i++) {
                int32_t itemId = reagents->m_reagent[i];

                if (itemId <= 0) {
                    continue;
                }

                WOWGUID guid;
                guid.m_guid = 0x1FE0000000000000ULL | static_cast<uint32_t>(spell->m_id);
                ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, guid, TooltipSpellCallback, this, true);

                if (!item) {
                    this->m_pendingQueries++;
                    continue;
                }

                if (first) {
                    first = 0;
                    FormatString("SPELL_REAGENTS", -1, GENDER_NOT_APPLICABLE, list, sizeof(list));
                } else {
                    SStrPack(list, ", ", sizeof(list));
                }

                if (reagents->m_reagentCount[i] <= 1) {
                    SStrCopy(token, item->GetName(0), sizeof(token));
                } else {
                    SStrPrintf(token, sizeof(token), "%s (%d)", item->GetName(0), reagents->m_reagentCount[i]);
                }

                if (player->m_bag.GetItemTypeCount(reagents->m_reagent[i], 0) < reagents->m_reagentCount[i]) {
                    SStrPack(list, "|c", sizeof(list));
                    SStrPack(list, s_tooltipRedColorString, sizeof(list));
                    SStrPack(list, token, sizeof(list));
                    SStrPack(list, "|r", sizeof(list));
                    allPresent = 0;
                } else {
                    SStrPack(list, token, sizeof(list));
                }
            }

            if (!first && (!allPresent || !brief)) {
                this->AddLine(list, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
            }
        }
    }

    int32_t result = 0;

    if (cooldown) {
        CGTooltip::GetDurationString(cost, sizeof(cost), cooldown, "ITEM_COOLDOWN_TIME", NULL, 1, false);
        this->AddLine(cost, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        result = 1;
    }

    if (!brief) {
        if (spell->m_attributesEx & 0x2) {
            if (powerDisplay) {
                SStrPrintf(runeText, sizeof(runeText), FrameScript_GetText(static_cast<const char*>(powerDisplay->m_globalStringBaseTag), -1, GENDER_NOT_APPLICABLE));
                SStrPrintf(token, sizeof(token), FrameScript_GetText("SPELL_USE_ALL_POWER_DISPLAY", -1, GENDER_NOT_APPLICABLE), runeText);
            } else {
                const char* useAll;
                uint32_t powerType = spell->m_powerType;

                if (powerType > 10) {
                    useAll = "SPELL_USE_ALL_HEALTH";
                } else if (powerType == 8) {
                    useAll = powerCost < 0 ? s_useAllNegativeEnergyToken : s_useAllPositiveEnergyToken;
                } else {
                    useAll = s_useAllPowerTokens[powerType];
                }

                FormatString(useAll, -1, GENDER_NOT_APPLICABLE, token, sizeof(token));
            }

            this->AddLine(token, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        if (!player->CheckSpellPvPUsability(spell, 0)) {
            this->AddLine(FrameScript_GetText(player->IsInArena() ? "ARENA_NOT_USABLE" : "RATEDBG_NOT_USABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
        }

        CVar* newbieTips = CVar::Lookup("showNewbieTips");
        SpellClassOptionsRec* classOptions = NULL;

        if (spell->m_spellClassOptionsID && newbieTips && newbieTips->m_intValue && (classOptions = g_spellClassOptionsDB.GetRecord(spell->m_spellClassOptionsID)) != NULL && classOptions->m_description && *classOptions->m_description) {
            SpellParserParseTextAsSpell(spell, description, sizeof(description), classOptions->m_description, isPet, isInspect, 0, 1, 0, NULL);
            this->AddLine(description, NULL, 1);
        } else if (spell->m_description && *spell->m_description) {
            SpellParserParseText(spell, description, sizeof(description), isPet, isInspect, 0, 0, 1, 0, NULL);
            this->AddLine(description, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
        }

        if (talent && !isInspect && a15) {
            this->AddTalentLearnText(talent, talentGroup, prereqsMet, rank, maxRank, 0, hideCallPetName, isPet, usePreview);
        }

        if (createItem && firstEffect && firstEffect->m_effectItemType) {
            this->m_itemLinkInfo.Clear();
            WOWGUID itemGuid = WOWGUID::Make(0);
            WOWGUID requester = WOWGUID::Make(0);
            this->SetItem(firstEffect->m_effectItemType, requester, itemGuid, 0, 0, 0, 1, WOWGUID::Make(0), 0, NULL, 0, NULL, 0, 0, 1, 0);
        }
    }

    if (this->HasScript(this->m_onTooltipSetSpell)) {
        this->RunScript(this->m_onTooltipSetSpell, 0, NULL);
    }

    this->Show();
    this->CalculateSize();
    return result;
}

// OFFSET: 0x00473A40
int32_t CGItemStatsSummary::Create(const CGItem_C* item, int32_t level) {
    this->Clear();
    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    if (rec->m_scalingStatDistribution) {
        this->FillOutScalingItem(rec, level);
    } else {
        this->ParseBasicStats(rec);
        this->ScanEnchantSpells(rec);
        this->ParseResistances(rec);
        this->GetSocketFlags(rec);
        this->ParseWeaponDPS(rec);

        if (item->m_item->m_itemFieldRandomPropertiesId) {
            this->FillOutRandomProperties(item->m_item->m_itemFieldRandomPropertiesId, item->m_item->m_itemFieldPropertySeed);
        }
    }

    this->CleanStatNames();
    return 1;
}

// OFFSET: 0x00473B20/0x14008C080
int32_t CGItemStatsSummary::Create(const char* link) {
    this->Clear();
    int32_t randomPropertiesId = 0;
    int32_t seed = 0;
    int32_t level = 0;
    const char* cursor = strstr(link, "item:");

    if (!cursor) {
        return 0;
    }

    cursor += 5;
    int32_t itemId = SStrToInt(cursor);

    if (itemId <= 0) {
        return 0;
    }

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (*cursor) {
        cursor++;
    }

    for (uint32_t i = 0; i < 3; i++) {
        while (*cursor && *cursor != ':') {
            cursor++;
        }

        if (*cursor) {
            cursor++;
        }
    }

    while (*cursor && *cursor != ':') {
        cursor++;
    }

    if (*cursor) {
        cursor++;

        while (*cursor && *cursor != ':') {
            cursor++;
        }

        if (*cursor) {
            cursor++;
            randomPropertiesId = SStrToInt(cursor);

            while (*cursor && *cursor != ':') {
                cursor++;
            }

            if (*cursor) {
                cursor++;
                seed = SStrToInt(cursor);

                while (*cursor && *cursor != ':') {
                    cursor++;
                }

                if (*cursor) {
                    level = SStrToInt(cursor + 1);
                }
            }
        }
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    if (rec->m_scalingStatDistribution) {
        this->FillOutScalingItem(rec, level);
    } else {
        this->ParseBasicStats(rec);
        this->ScanEnchantSpells(rec);
        this->ParseResistances(rec);
        this->GetSocketFlags(rec);
        this->ParseWeaponDPS(rec);

        if (randomPropertiesId) {
            this->FillOutRandomProperties(randomPropertiesId, seed);
        }
    }

    this->CleanStatNames();
    return 1;
}

// OFFSET: 0x00473CF0/0x14008C290
void TooltipItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(param);

    if (tooltip->m_spellId) {
        tooltip->SetSpell(tooltip->m_spellId, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        tooltip->CalculateSize();
    } else if (tooltip->m_itemId) {
        WOWGUID itemGuid = tooltip->m_itemGuid;
        WOWGUID requester = WOWGUID::Make(0);
        tooltip->SetItem(tooltip->m_itemId, requester, itemGuid, tooltip->m_itemNameOnly, tooltip->m_itemCompare, tooltip->m_itemUseLinkInfo, 0, tooltip->m_itemOwner, tooltip->m_itemSocketPreview, NULL, tooltip->m_itemHideCharges, NULL, tooltip->m_itemMinimumLevel, tooltip->m_itemSkipSellPrice, 0, 0);
        tooltip->CalculateSize();
    }
}

// OFFSET: 0x00473DC0/0x14008C3D0
void CGTooltip::SetObject(const WOWGUID& guid) {
    this->ClearTooltip();
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (!object) {
        this->Hide();
        return;
    }

    this->m_objectGuid = guid;

    if (this->m_backdropGenerator) {
        this->m_backdropGenerator->SetVertexColor(s_tooltipObjectBackdropColor);
    }

    this->AddLine(object->CGGameObject_C::GetObjectName(), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    object->AddToolTipInfo(this);
    char text[4096];

    if (object->m_gameObject->m_flags & 0x2) {
        CImVector color = s_skillColors[4];
        int32_t spellId = 0;
        int32_t skillValue = 0;
        int32_t requiredSkill = 0;
        CGItem_C* key = NULL;
        CVar* colorblindMode = CVar::Lookup("colorblindMode");
        const char* suffix = colorblindMode && colorblindMode->m_intValue ? s_colorblindSuffixes[4] : "";

        if (!object->IsLocked(&spellId, &skillValue, &requiredSkill, &key, NULL)) {
            if (!spellId || key) {
                color = s_skillColors[1];
                suffix = "";
            } else {
                color = *GetSkillColor(requiredSkill, skillValue, &suffix);
            }
        }

        SStrPrintf(text, sizeof(text), "%s%s", suffix, FrameScript_GetText("LOCKED", -1, GENDER_NOT_APPLICABLE));
        this->AddLine(text, NULL, color, color, 0);
    }

    LockRec* lock = object->GetLockRec();

    if (lock && object->IsValidOpenAction(lock->m_action[0])) {
        if (lock->m_type[0] == 1) {
            WOWGUID requester = object->m_obj->m_guid;
            ItemRecSparse* item = g_itemSparseDB.GetRecord(lock->m_index[0], requester, TooltipObjectCallback, this, true);

            if (item) {
                SStrPrintf(text, sizeof(text), FrameScript_GetText("LOCKED_WITH_ITEM", -1, GENDER_NOT_APPLICABLE), item->GetName(0));
                this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        } else if (lock->m_type[0] == 2) {
            int32_t spellId = 0;
            int32_t skillValue = 0;
            int32_t requiredSkill = 0;
            CImVector color = s_tooltipTextColor;
            const char* suffix = "";
            char format[128];
            bool show = true;
            object->IsLocked(&spellId, &skillValue, &requiredSkill, NULL, NULL);

            if (spellId) {
                color = *GetSkillColor(requiredSkill, skillValue, &suffix);
                object->GetLockSkillColor(requiredSkill, skillValue, color);
                FormatString("LOCKED_WITH_SPELL_KNOWN", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
            } else if (!(object->m_gameObject->m_flags & 0x2)) {
                color.Set(1.0f, 1.0f, 0.0f, 0.0f);
                FormatString("LOCKED_WITH_SPELL", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
            } else {
                show = false;
            }

            if (show) {
                char skill[256];
                LockTypeRec* lockType = g_lockTypeDB.GetRecord(lock->m_index[0]);
                SStrPrintf(skill, sizeof(skill), format, lockType ? static_cast<const char*>(lockType->m_name) : "UNKNOWN");
                SStrPrintf(text, sizeof(text), "%s%s", suffix, skill);
                this->AddLine(text, NULL, color, color, 0);
            }
        } else if (lock->m_type[0] == 3) {
            SpellRec* spell = g_spellDB.GetRecord(lock->m_index[0]);

            if (spell) {
                this->AddSpellReagents(spell, "LOCKED_WITH_SPELL", TooltipObjectCallback);
            }
        }
    }

    GameObjectStats* stats = object->m_stats;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (stats && stats->m_requiredLevel && player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) < stats->m_requiredLevel) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_MIN_LEVEL", -1, GENDER_NOT_APPLICABLE), stats->m_requiredLevel);
        this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
    }

    CVar* questTracking = CVar::Lookup("showQuestTrackingTooltips");

    if (questTracking && questTracking->m_intValue) {
        QuestLogLeaderBoardParams params;
        params.m_questCallback = TooltipObjectCallback;
        params.m_objectCallback = TooltipObjectCallback;
        params.m_creatureCallback = TooltipObjectCallback;
        params.m_itemCallback = TooltipObjectCallback;
        params.m_param = this;
        int32_t quests[250];
        CGQuestLog::GetAllOfGroupsQuests(quests);
        char buffer[4099];

        for (uint32_t i = 0; i < 250; i++) {
            int32_t questId = quests[i];

            if (!questId) {
                break;
            }

            WOWGUID requester = WOWGUID::Make(0);
            QuestCache* quest = g_questDBCache.GetRecord(questId, requester, TooltipObjectCallback, this, true);

            if (!quest) {
                continue;
            }

            bool header = false;

            for (uint32_t member = 0; member < CGPartyInfo::NumMembers() + 1; member++) {
                CGPlayer_C* partyPlayer = member ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::GetMember(member - 1), TYPEMASK_PLAYER, __FILE__, __LINE__)) : static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

                if (!partyPlayer) {
                    continue;
                }

                if (CGQuestLog::GetQuestLeaderBoardForKill(partyPlayer, quest->m_id, object->m_obj->m_entryID | 0x80000000, &params)) {
                    if (!header) {
                        this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                    }

                    if (CGPartyInfo::NumMembers()) {
                        SStrPrintf(buffer, sizeof(buffer), " %s - %s", partyPlayer->GetVisibleUnitName(NULL, 1), static_cast<const char*>(params.m_text));
                    } else {
                        SStrPrintf(buffer, sizeof(buffer), " - %s", static_cast<const char*>(params.m_text));
                    }

                    this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                    header = true;
                }

                GameObjectStats* objectStats = object->m_stats;

                if (!objectStats) {
                    continue;
                }

                for (uint32_t j = 0; j < 6; j++) {
                    int32_t itemId = objectStats->m_questItems[j];

                    if (!itemId) {
                        break;
                    }

                    if (CGQuestLog::GetQuestLeaderBoardForItem(partyPlayer, quest->m_id, itemId, &params)) {
                        if (!header) {
                            this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                            header = true;
                        }

                        if (CGPartyInfo::NumMembers()) {
                            SStrPrintf(buffer, sizeof(buffer), " %s - %s", partyPlayer->GetVisibleUnitName(NULL, 1), static_cast<const char*>(params.m_text));
                        } else {
                            SStrPrintf(buffer, sizeof(buffer), " - %s", static_cast<const char*>(params.m_text));
                        }

                        this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                    }

                    if (partyPlayer->m_obj->m_guid == ClntObjMgrGetActivePlayer() && CGQuestLog::IsQuestComplete(partyPlayer, CGQuestLog::GetQuestIndex(questId) - 1)) {
                        continue;
                    }

                    for (uint32_t k = 0; k < 4; k++) {
                        if (quest->m_intermediateItemId[k] != itemId) {
                            continue;
                        }

                        if (!header) {
                            this->AddLine(quest->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                            header = true;
                        }

                        WOWGUID itemRequester = WOWGUID::Make(0);
                        ItemRecSparse* item = g_itemSparseDB.GetRecord(quest->m_intermediateItemId[k], itemRequester, TooltipUnitNameCallback, this, false);

                        if (item) {
                            if (CGPartyInfo::NumMembers()) {
                                SStrPrintf(buffer, sizeof(buffer), " %s - %s", partyPlayer->GetVisibleUnitName(NULL, 1), item->GetName(0));
                            } else {
                                SStrPrintf(buffer, sizeof(buffer), " - %s", item->GetName(0));
                            }

                            this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                        }
                    }
                }
            }
        }
    }

    if (this->m_statusBar && object->m_gameObject->m_typeID == 33) {
        this->m_healthGuid = this->m_objectGuid;
        ClntObjMgrSetObjMirrorHandler(this->m_healthGuid, ID_GAMEOBJECT, offsetof(CGGameObjectData, m_animProgress), sizeof(uint8_t), ObjHealthUpdateHandler, this->m_statusBar, static_cast<HANDLER_PRIORITY>(0), 0);
        this->m_statusBar->SetMinMaxValues(0.0f, 1.0f);
        this->m_statusBar->SetValue(static_cast<float>(object->m_gameObject->m_animProgress) * 0.0039215689f);
        this->m_statusBar->Show();
        this->Show();
    } else {
        this->m_healthGuid = WOWGUID::Make(0);
        this->Show();
    }
}

// OFFSET: 0x004746C0/0x14008CF90
void CGTooltip::SetTalent(const TalentRec* talent, const CGTalentGroup_C* group, int32_t isInspect, int32_t a4, int32_t forcedRank, int32_t isPet, int32_t usePreview) {
    if (!talent) {
        this->Hide();
        return;
    }

    int32_t rank = -1;
    int32_t currentRank = -1;
    int32_t maxRank = -1;

    for (int32_t i = 4; i >= 0; i--) {
        if (talent->m_spellRank[i]) {
            maxRank = i;
            break;
        }
    }

    if (a4 && forcedRank >= 0 && forcedRank <= maxRank) {
        currentRank = forcedRank;
    } else if (group) {
        const TalentGroupItem_C* item = group->GetTalentItem(talent->m_id);

        if (item) {
            rank = item->m_previewRank;
            currentRank = item->m_rank;
        }
    }

    if (!usePreview) {
        rank = currentRank;
    }

    if (talent->m_flags & 0x1) {
        bool hasNext = maxRank > 0 && rank >= 0 && rank < maxRank;
        int32_t index = rank < 0 ? 0 : rank;
        this->SetSpell(talent->m_spellRank[index], 0, 0, isPet, 0, isInspect, a4, talent, group, 0, usePreview, index, rank, maxRank, !hasNext, 0, 0);

        if (hasNext) {
            this->SetSpell(talent->m_spellRank[rank + 1], 0, 0, isPet, 0, isInspect, a4, talent, group, 1, usePreview, rank + 1, rank, maxRank, 1, 0, 0);
        }

        return;
    }

    this->ClearTooltip();
    int32_t currentId = talent->m_spellRank[rank];
    int32_t nextId = talent->m_spellRank[rank + 1];
    SpellRec* current = g_spellDB.GetRecord(currentId);
    SpellRec* next = g_spellDB.GetRecord(nextId);
    bool hasCurrent = rank >= 0 && current;
    bool hasNext = rank < maxRank ? next != NULL : false;

    if (!hasCurrent && !hasNext) {
        return;
    }

    this->m_spellId = hasCurrent ? currentId : 0;
    this->m_nextRankSpellId = hasNext ? nextId : 0;

    if (hasCurrent) {
        this->AddLine(current->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    } else if (hasNext) {
        this->AddLine(next->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    char text[2048];

    if (maxRank >= 0) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("TOOLTIP_TALENT_RANK", -1, GENDER_NOT_APPLICABLE), rank + 1, maxRank + 1);
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    int32_t prereqsMet = hasCurrent ? 1 : this->AddTalentPrereqs(talent, group, isInspect, isPet, usePreview);
    int32_t shown = 0;

    if (hasCurrent && current->m_description && *current->m_description) {
        SpellParserParseText(current, text, sizeof(text), isPet, isInspect, 0, 0, 1, 0, NULL);
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
        shown = 1;
    }

    if (hasNext && next->m_description && *next->m_description) {
        if (shown) {
            this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
            this->AddLine(FrameScript_GetText("TOOLTIP_TALENT_NEXT_RANK", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        SpellParserParseText(next, text, sizeof(text), isPet, isInspect, 0, 0, 1, 0, NULL);
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
    }

    this->AddTalentLearnText(talent, group, prereqsMet, currentRank, maxRank, isInspect, a4, isPet, usePreview);

    if (this->HasScript(this->m_onTooltipSetSpell)) {
        this->RunScript(this->m_onTooltipSetSpell, 0, NULL);
    }

    this->Show();
}

// OFFSET: 0x00474AD0/0x14008D610
int32_t CGTooltip::SetPetAction(const PetAction& action) {
    uint32_t value = action.m_data;

    if (!value) {
        return 0;
    }

    char token[64];

    switch ((value >> 24) & 0x3F) {
        case 1:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17: {
            uint32_t start = 0;
            uint32_t duration = 0;
            uint32_t enabled = 0;
            Spell_C_GetSpellCooldown(value & 0xFFFFFF, 1, &start, &duration, &enabled, 0);

            if (!enabled || !duration || !start) {
                return this->SetSpell(action.m_data & 0xFFFFFF, 0, 0, 1, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
            }

            return this->SetSpell(action.m_data & 0xFFFFFF, 0, start + duration - FrameTime::GetCurTimeMs(), 1, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        }

        case 6:
            SStrPrintf(token, sizeof(token), "PET_MODE_%s", CGPetInfo::GetModeToken(value & 0xFFFFFF));
            this->AddLine(FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipGrayColor, 0);
            SStrPrintf(token, sizeof(token), "PET_MODE_%s_TOOLTIP", CGPetInfo::GetModeToken(action.m_data & 0xFFFFFF));
            this->AddLine(FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
            this->Show();
            break;

        case 7:
            SStrPrintf(token, sizeof(token), "PET_ACTION_%s", CGPetInfo::GetOrdersToken(value & 0xFFFFFF));
            this->AddLine(FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipGrayColor, 0);
            SStrPrintf(token, sizeof(token), "PET_ACTION_%s_TOOLTIP", CGPetInfo::GetOrdersToken(action.m_data & 0xFFFFFF));
            this->AddLine(FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
            this->Show();
            break;

        default:
            return 0;
    }

    return 0;
}

// OFFSET: 0x00474CE0/0x14008D910
bool ResolveItemSetItem(ItemSetTooltipData* data, int32_t pass, int32_t itemId, int32_t slot) {
    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, requester, TooltipItemCallback, data->m_tooltip, true);

    if (!item || item->m_itemSet != data->m_set->m_id) {
        return false;
    }

    if (!pass) {
        for (uint32_t i = 0; i < 17; i++) {
            if (!data->m_equipped[i] && itemId == data->m_set->m_itemID[i]) {
                data->m_equipped[i] = itemId;
                data->m_slotUsed[slot] = 1;
                return true;
            }
        }

        return false;
    }

    if (data->m_slotUsed[slot]) {
        return false;
    }

    for (uint32_t i = 0; i < 17; i++) {
        if (data->m_equipped[i] || !data->m_items[i]) {
            continue;
        }

        int32_t inventoryType = item->m_inventoryType;
        int32_t setType = data->m_items[i]->m_inventoryType;

        if (inventoryType == setType || (inventoryType == 5 && setType == 20) || (inventoryType == 20 && setType == 5)) {
            data->m_equipped[i] = itemId;
            return true;
        }
    }

    return true;
}

// INLINED
static bool AppendMasterySpells(char* text, uint32_t size) {
    int32_t tree = TalentInfoGetPrimaryTalentTree(CGTalentInfo::GetActiveTalentGroup(0), 0, 0);
    TalentTabRec* tab = g_talentTabDB.GetRecord(tree);

    if (!tab) {
        return false;
    }

    SpellRec* first = g_spellDB.GetRecord(tab->m_masterySpellID[0]);
    SpellRec* second = g_spellDB.GetRecord(tab->m_masterySpellID[1]);

    if (!first || !first->m_name) {
        return false;
    }

    char format[1024];
    char mastery[1024];

    if (second && second->m_name) {
        FormatString("ITEM_MOD_MASTERY_RATING_TWO_SPELLS", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
        SStrPrintf(mastery, sizeof(mastery), format, static_cast<const char*>(first->m_name), static_cast<const char*>(second->m_name));
    } else {
        FormatString("ITEM_MOD_MASTERY_RATING_SPELL", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
        SStrPrintf(mastery, sizeof(mastery), format, static_cast<const char*>(first->m_name));
    }

    SStrPack(text, " ", size);
    SStrPack(text, mastery, size);
    return true;
}

// OFFSET: 0x00474DE0/0x14008DA50
int32_t CGTooltip::SetItem(int32_t itemId, const WOWGUID& requester, const WOWGUID& itemGuid, int32_t nameOnly, int32_t compare, int32_t useLinkInfo, int32_t append, WOWGUID owner, int32_t socketPreview, FILE* stream, int32_t hideCharges, const TooltipExtendedItemInfo* info, int32_t minimumLevel, int32_t skipLock, int32_t skipSellPrice, const WOWGUID* transmogrify) {
    int32_t result = 0;

    if (!append) {
        this->ClearTooltip();
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        this->Hide();
        return 0;
    }

    CGPlayer_C* ownerPlayer = NULL;

    if (owner) {
        ownerPlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(owner, TYPEMASK_PLAYER, __FILE__, __LINE__));
    }

    if (!append) {
        this->m_itemGuid = itemGuid;
        this->m_itemId = itemId;
        this->m_itemUseLinkInfo = useLinkInfo;
        this->m_itemNameOnly = nameOnly;
        this->m_itemSocketPreview = socketPreview;
        this->m_itemCompare = compare;
        this->m_itemOwner = owner;
        this->m_itemHideCharges = hideCharges;
        this->m_itemMinimumLevel = minimumLevel;
        this->m_itemSkipSellPrice = skipSellPrice;
    }

    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, requester, TooltipItemCallback, this, true);

    if (!item) {
        return this->SetItemNoStats();
    }

    const ItemRec* itemRec = item->GetNonSparse();

    if (!itemRec) {
        return this->SetItemNoStats();
    }

    bool isBag = item->m_inventoryType == 18;
    int32_t needsUpdate = 0;
    CGItem_C* itemObj = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
    ItemReforgeRec* reforge = NULL;

    if ((!itemObj || (reforge = g_itemReforgeDB.GetRecord(itemObj->m_item->m_itemFieldEnchantment[8].m_id)) == NULL) && (!this->m_itemUseLinkInfo || (reforge = g_itemReforgeDB.GetRecord(this->m_itemLinkInfo.m_enchants[8])) == NULL)) {
        if (info) {
            reforge = g_itemReforgeDB.GetRecord(info->m_enchants[8]);
        }
    }

    int32_t wrapped = (itemObj && (itemObj->m_item->m_itemFieldFlags & 0x8)) || (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_wrapped);
    int32_t randomPropertiesId = 0;
    int32_t propertySeed = 0;

    if (!wrapped) {
        if (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_randomPropertiesId) {
            randomPropertiesId = this->m_itemLinkInfo.m_randomPropertiesId;

            if (randomPropertiesId <= 0) {
                ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

                if (suffix) {
                    for (uint32_t i = 0; i < 5; i++) {
                        this->m_itemLinkInfo.m_enchants[10 + i] = suffix->m_enchantment[i];
                    }
                }
            } else {
                ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

                if (properties) {
                    for (uint32_t i = 0; i < 5; i++) {
                        this->m_itemLinkInfo.m_enchants[10 + i] = properties->m_enchantment[i];
                    }
                }
            }

            propertySeed = this->m_itemLinkInfo.m_propertySeed;
        } else if (itemObj) {
            randomPropertiesId = itemObj->m_item->m_itemFieldRandomPropertiesId;
            propertySeed = itemObj->m_item->m_itemFieldPropertySeed;
        }
    }

    int32_t destroyGem = 0;

    if (compare) {
        if (item->m_gemProperties) {
            this->AddLine(FrameScript_GetText("DESTROY_GEM", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            destroyGem = 1;
        } else {
            this->AddLine(FrameScript_GetText("CURRENTLY_EQUIPPED", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGrayColor, s_tooltipGrayColor, 0);
        }
    }

    char name[1024];
    char buffer[1024];
    char text[4096];
    char subclassName[1024];
    char format[128];
    CGItem_C::BuildItemName(name, sizeof(name), itemId, randomPropertiesId);

    if (append) {
        SStrCopy(buffer, name, sizeof(buffer));
        SStrPrintf(name, sizeof(name), "\n%s", buffer);
    }

    if (nameOnly) {
        this->AddLine(name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    } else {
        const CImVector& nameColor = destroyGem ? s_tooltipGrayColor : s_itemQualityColors[item->m_quality];
        this->AddLine(name, NULL, nameColor, nameColor, 0);
        CVar* colorblindMode = CVar::Lookup("colorblindMode");

        if (colorblindMode && colorblindMode->m_intValue) {
            if (item->m_flags[0] & 0x8) {
                SStrPrintf(buffer, sizeof(buffer), "ITEM_HEROIC_QUALITY%d_DESC", destroyGem ? 0 : item->m_quality);
            } else {
                SStrPrintf(buffer, sizeof(buffer), "ITEM_QUALITY%d_DESC", destroyGem ? 0 : item->m_quality);
            }

            this->AddLine(FrameScript_GetText(buffer, -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        } else if (item->m_flags[0] & 0x8) {
            this->AddLine(FrameScript_GetText("ITEM_HEROIC", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        }
    }

    this->AddItemNameDescription(itemId);

    if (itemObj && (itemObj->GetItemStaticFlags(0) & 0x2000)) {
        int32_t petitionId = itemObj->GetPetitionId();
        CGPetition* petition = g_petitionCache.GetRecord(petitionId, itemGuid, TooltipItemCallback, this, true);

        if (!petition) {
            return 0;
        }

        SStrPrintf(text, sizeof(text), FrameScript_GetText(petition->m_staticType ? "PETITION_TITLE" : "GUILD_CHARTER_TITLE", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(petition->m_title));
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
        NameCache* creator = g_nameDBCache.GetRecord(petition->m_ownerGuid.m_guid, petition->m_ownerGuid, TooltipItemCallback, this, true);

        if (creator) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText(petition->m_staticType ? "PETITION_CREATOR" : "GUILD_CHARTER_CREATOR", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(creator->m_name));
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        int32_t signatures = itemObj->GetPetitionSignatures();

        if (signatures) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("PETITION_NUM_SIGNATURES", signatures, GENDER_NOT_APPLICABLE), signatures);
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    }

    if (item->m_flags[0] & 0x2000) {
        this->AddLine(FrameScript_GetText("ITEM_SIGNABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
    }

    if (!nameOnly && !socketPreview) {
        int32_t transmogItemId = 0;

        if (transmogrify) {
            if (!*transmogrify) {
                this->AddLine(FrameScript_GetText("TRANSMOGRIFY_TOOLTIP_REVERT", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTransmogrifyColor, s_tooltipTransmogrifyColor, 0);
            } else {
                CGItem_C* source = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(*transmogrify, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (source) {
                    WOWGUID none = WOWGUID::Make(0);
                    ItemRecSparse* sourceRec = g_itemSparseDB.GetRecord(source->m_obj->m_entryID, none, NULL, NULL, false);

                    if (sourceRec) {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("TRANSMOGRIFY_TOOLTIP_APPLY", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(sourceRec->m_name));
                        this->AddLine(text, NULL, s_tooltipTransmogrifyColor, s_tooltipTransmogrifyColor, 0);
                    }
                }
            }
        } else if (itemObj) {
            transmogItemId = itemObj->GetEnchantmentId(9);
        } else if (ownerPlayer) {
            for (uint32_t slot = 0; slot < 19; slot++) {
                int32_t equippedId;

                if (ownerPlayer->IsActivePlayer()) {
                    CGItem_C* equipped = ownerPlayer->GetBag()->GetItemPointer(slot);

                    if (!equipped) {
                        continue;
                    }

                    equippedId = equipped->m_obj->m_entryID;
                } else {
                    const InspectItem* inspected = ownerPlayer->GetInspectedItem(slot);

                    if (!inspected) {
                        continue;
                    }

                    equippedId = inspected->m_itemId;
                }

                if (!equippedId || equippedId != itemId) {
                    continue;
                }

                const VisibleItem_C* visible = ownerPlayer->GetVisibleItem(slot);

                if (visible && equippedId != abs(visible->m_itemId)) {
                    transmogItemId = abs(visible->m_itemId);
                    break;
                }
            }
        }

        if (transmogItemId) {
            WOWGUID none = WOWGUID::Make(0);
            ItemRecSparse* transmogRec = g_itemSparseDB.GetRecord(transmogItemId, none, NULL, NULL, false);

            if (transmogRec) {
                SStrPrintf(text, sizeof(text), FrameScript_GetText("TRANSMOGRIFIED", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(transmogRec->m_name));
                this->AddLine(text, NULL, s_tooltipTransmogrifyColor, s_tooltipTransmogrifyColor, 0);
            }
        }

        if (item->m_zoneBound) {
            AreaTableRec* area = g_areaTableDB.GetRecord(item->m_zoneBound);

            if (area) {
                this->AddLine(area->m_areaName, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }

        if (item->m_instanceBound) {
            MapRec* map = g_mapDB.GetRecord(item->m_instanceBound);

            if (map) {
                this->AddLine(map->m_mapName, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }

        if (item->m_flags[0] & 0x2) {
            FormatString("ITEM_CONJURED", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            this->AddLine(buffer, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        text[0] = '\0';

        if (itemObj && itemObj->IsBound()) {
            if (item->m_bonding == 4) {
                FormatString("ITEM_BIND_QUEST", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            } else if (itemObj->IsBoundToBattlenetAccount()) {
                FormatString("ITEM_BNETACCOUNTBOUND", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            } else if (itemObj->IsBoundToAccount()) {
                FormatString("ITEM_ACCOUNTBOUND", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            } else {
                FormatString("ITEM_SOULBOUND", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            }
        }

        if (item->m_bonding) {
            if (!text[0]) {
                if (item->m_flags[0] & 0x8000000) {
                    FormatString(item->m_flags[1] & 0x20000 ? "ITEM_BIND_TO_BNETACCOUNT" : "ITEM_BIND_TO_ACCOUNT", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
                } else if (!this->m_itemUseLinkInfo || !this->m_itemLinkInfo.m_soulbound) {
                    switch (item->m_bonding) {
                        case 1:
                            FormatString("ITEM_BIND_ON_PICKUP", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
                            break;

                        case 2:
                            FormatString("ITEM_BIND_ON_EQUIP", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
                            break;

                        case 3:
                            FormatString("ITEM_BIND_ON_USE", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
                            break;

                        case 4:
                            FormatString("ITEM_BIND_QUEST", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
                            break;

                        default:
                            break;
                    }
                } else {
                    FormatString("ITEM_SOULBOUND", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
                }
            }

            if (text[0]) {
                this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }

        if (item->m_flags[0] & 0x80000) {
            FormatString("ITEM_UNIQUE_EQUIPPABLE", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        } else if (item->m_maxCount > 0) {
            if (item->m_maxCount == 1) {
                FormatString("ITEM_UNIQUE", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            } else {
                FormatString("ITEM_UNIQUE_MULTIPLE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(text, sizeof(text), buffer, item->m_maxCount);
            }

            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        } else if (item->m_itemLimitCategory) {
            ItemLimitCategoryRec* category = g_itemLimitCategoryDB.GetRecord(item->m_itemLimitCategory);
            FormatString(category->m_flags & 0x1 ? "ITEM_LIMIT_CATEGORY_MULTIPLE" : "ITEM_LIMIT_CATEGORY", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            SStrPrintf(text, sizeof(text), buffer, static_cast<const char*>(category->m_name), category->m_quantity);
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        if (item->m_startQuest) {
            FormatString("ITEM_STARTS_QUEST", -1, GENDER_NOT_APPLICABLE, text, sizeof(text));
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

        LockRec* lock = NULL;

        if (!skipLock) {
            lock = g_lockDB.GetRecord(item->m_lockID);

            if (!lock && this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_lockId) {
                lock = g_lockDB.GetRecord(this->m_itemLinkInfo.m_lockId);
            }
        }

        if (lock && !(itemObj && (itemObj->m_item->m_itemFieldFlags & 0x4))) {
            CImVector color;
            int32_t spellId = 0;
            int32_t skillValue = 0;
            int32_t requiredSkill = 0;
            int32_t lockType = 0;
            const char* suffix = "";
            CGItem_C::IsLocked(lock, item->m_itemLevel, &spellId, &skillValue, &requiredSkill, &lockType, NULL, NULL);

            if (spellId) {
                color = *GetSkillColor(requiredSkill, skillValue, &suffix);
            } else {
                color = s_skillColors[1];
                suffix = GetColorblindSuffix(1);
            }

            bool encrypted = false;

            if (lockType) {
                encrypted = lockType == 20;
            } else {
                for (uint32_t i = 0; i < 8; i++) {
                    if (lock->m_index[i] == 20) {
                        encrypted = true;
                        break;
                    }
                }
            }

            SStrPrintf(text, sizeof(text), "%s%s", suffix, FrameScript_GetText(encrypted ? "ENCRYPTED" : "LOCKED", -1, GENDER_NOT_APPLICABLE));
            this->AddLine(text, NULL, color, color, 0);

            for (uint32_t i = 0; i < 8; i++) {
                if (lock->m_type[i] == 2) {
                    int32_t skillSpell = 0;
                    int32_t skill = 0;
                    int32_t required = 0;
                    CImVector skillColor = s_tooltipTextColor;
                    CGItem_C::IsLocked(lock, item->m_itemLevel, &skillSpell, &skill, &required, NULL, NULL, NULL);

                    if (skillSpell) {
                        skillColor = *GetSkillColor(required, skill, &suffix);
                        FormatString("ITEM_MIN_SKILL", -1, GENDER_NOT_APPLICABLE, format, sizeof(format));
                        LockTypeRec* type = g_lockTypeDB.GetRecord(lock->m_index[i]);
                        SStrPrintf(text, sizeof(text), format, type ? static_cast<const char*>(type->m_name) : "UNKNOWN", lock->m_skill[i]);
                        this->AddLine(text, NULL, skillColor, skillColor, 0);
                    }

                    break;
                }

                if (lock->m_type[i] == 3) {
                    SpellRec* spell = g_spellDB.GetRecord(lock->m_index[i]);

                    if (spell) {
                        this->AddSpellReagents(spell, "LOCKED_WITH_SPELL", TooltipItemCallback);
                    }

                    break;
                }

                if (lock->m_type[i] == 1) {
                    WOWGUID keyRequester = itemObj ? itemObj->m_obj->m_guid : WOWGUID::Make(0);
                    ItemRecSparse* key = g_itemSparseDB.GetRecord(lock->m_index[i], keyRequester, TooltipItemCallback, this, true);

                    if (key) {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOCKED_WITH_ITEM", -1, GENDER_NOT_APPLICABLE), key->GetName(0));
                        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                    }

                    break;
                }
            }

            needsUpdate = 1;
        }
    }

    ScalingStatDistributionRec* distribution = NULL;
    ScalingStatValuesRec* values = NULL;
    int32_t displayedLevel = 0;

    if (item->m_scalingStatDistribution) {
        distribution = g_scalingStatDistributionDB.GetRecord(item->m_scalingStatDistribution);
        displayedLevel = CGTooltip::GetDislayedScalingItemLevel(item, info, requester, minimumLevel);
        this->m_itemDisplayedLevel = displayedLevel;

        if (displayedLevel - 1 >= 0 && displayedLevel - 1 < g_scalingStatValuesDB.m_numRecords) {
            values = g_scalingStatValuesDB.GetRecordByIndex(displayedLevel - 1);
        }
    }

    ItemSubClassRec* subclass = NULL;
    int32_t canUse = 0;
    subclassName[0] = '\0';

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec && rec->m_classID == itemRec->m_classID && rec->m_subClassID == itemRec->m_subclassID) {
            subclass = rec;
            break;
        }
    }

    if (isBag) {
        if (subclass && subclass->m_displayName && *subclass->m_displayName) {
            FormatString("CONTAINER_SLOTS", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            SStrPrintf(name, sizeof(name), buffer, item->m_containerSlots, static_cast<const char*>(subclass->m_displayName));
            this->AddLine(name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }
    } else if (itemRec->m_classID == 16) {
        for (uint32_t i = 0; i < 5; i++) {
            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(item->m_spellID[i], count);

            for (uint32_t j = 0; j < count; j++) {
                const SpellEffectRec* effect = effects->m_data[j];

                if (!effect || effect->m_effect != 74) {
                    continue;
                }

                GlyphPropertiesRec* glyph = g_glyphPropertiesDB.GetRecord(effect->m_effectMiscValue);

                if (!glyph) {
                    continue;
                }

                const char* glyphType;

                if (glyph->m_glyphSlotFlags == 1) {
                    glyphType = FrameScript_GetText("MINOR_GLYPH", -1, GENDER_NOT_APPLICABLE);
                } else if (glyph->m_glyphSlotFlags) {
                    glyphType = FrameScript_GetText("PRIME_GLYPH", -1, GENDER_NOT_APPLICABLE);
                } else {
                    glyphType = FrameScript_GetText("MAJOR_GLYPH", -1, GENDER_NOT_APPLICABLE);
                }

                this->AddLine(glyphType, NULL, s_tooltipGlyphColor, s_tooltipGlyphColor, 0);
            }
        }
    } else {
        if (subclass && item->m_inventoryType != 16 && !(subclass->m_displayFlags & 0x1) && subclass->m_displayName && *subclass->m_displayName) {
            SStrCopy(subclassName, subclass->m_displayName, sizeof(subclassName));
        }

        if (itemRec->m_classID != 6) {
            if (!FormatString(s_inventoryTypeTokens[item->m_inventoryType], -1, GENDER_NOT_APPLICABLE, name, sizeof(name))) {
                name[0] = '\0';
            }
        } else {
            ItemClassRec* itemClass = g_itemClassDB.GetRecord(6);

            if (itemClass && itemClass->m_className && *itemClass->m_className) {
                SStrCopy(name, itemClass->m_className, sizeof(name));
            } else {
                name[0] = '\0';
            }
        }

        int32_t cantUse = 0;
        int32_t prerequisiteKnown = 0;
        uint32_t proficiency = player->GetProficiency(static_cast<uint8_t>(itemRec->m_classID));

        if (proficiency && !((1 << itemRec->m_subclassID) & proficiency)) {
            if (itemRec->m_classID == 2) {
                if (subclass->m_prerequisiteProficiency != -1) {
                    if ((1 << subclass->m_prerequisiteProficiency) & proficiency) {
                        prerequisiteKnown = 1;
                    } else {
                        cantUse = 1;
                    }
                } else if (subclass->m_postrequisiteProficiency == -1 || !((1 << subclass->m_postrequisiteProficiency) & proficiency)) {
                    cantUse = 1;
                } else {
                    prerequisiteKnown = 1;
                }
            } else {
                int32_t replacement = -1;

                if (values && distribution && itemRec->m_classID == 4 && (itemRec->m_subclassID == 4 || itemRec->m_subclassID == 3)) {
                    ChrClassesRec* classRec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);

                    if (classRec) {
                        if (itemRec->m_subclassID == 4 && (classRec->m_flags & 0x20)) {
                            replacement = ((proficiency & 0x8) | 0x10) >> 3;
                        } else if (itemRec->m_subclassID == 3 && (classRec->m_flags & 0x10) && (proficiency & 0x4)) {
                            replacement = 2;
                        }
                    }
                }

                if (replacement >= 0) {
                    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
                        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

                        if (rec && rec->m_classID == 4 && rec->m_subClassID == replacement) {
                            if (rec->m_displayName && *rec->m_displayName) {
                                SStrCopy(subclassName, rec->m_displayName, sizeof(subclassName));
                            }

                            break;
                        }
                    }
                } else {
                    cantUse = 1;
                }
            }
        }

        int32_t leftRed = (item->m_inventoryType == 22 && !player->CanDualWield()) ? 1 : prerequisiteKnown;

        if (name[0]) {
            this->AddLine(name, subclassName[0] ? subclassName : NULL, leftRed ? s_tooltipRedColor : s_tooltipTextColor, cantUse ? s_tooltipRedColor : s_tooltipTextColor, 0);
        } else if (subclassName[0]) {
            this->AddLine(subclassName, NULL, cantUse ? s_tooltipRedColor : s_tooltipTextColor, cantUse ? s_tooltipRedColor : s_tooltipTextColor, 0);
        }

        canUse = cantUse == 0;
    }

    if (reforge && !wrapped) {
        this->AddLine(FrameScript_GetText("REFORGED", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
    }

    int32_t useSpell = 0;
    int32_t useCharges = 0;
    const SpellItemEnchantmentRec* useEnchant = NULL;

    if (!nameOnly) {
        float minDamage = 0.0f;
        float maxDamage = 0.0f;
        float averageDamage;
        int32_t level = player->m_unit->m_unitFieldMaxitemlevel;

        if (values && CGTooltip::GetScalingStatWeaponDPS(item, values, &minDamage, &maxDamage)) {
            if (item->m_delay) {
                minDamage = minDamage * static_cast<float>(item->m_delay) * 0.001f;
                maxDamage = maxDamage * static_cast<float>(item->m_delay) * 0.001f;
            }

            averageDamage = (maxDamage + minDamage) * 0.5f;
            FormatString("DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            SStrPrintf(name, sizeof(name), buffer, static_cast<int32_t>(minDamage), static_cast<int32_t>(maxDamage));
            FormatString("SPEED", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            SStrPrintf(subclassName, sizeof(subclassName), "%s %.2f", buffer, static_cast<double>(item->m_delay) * 0.001);
            this->AddLine(name, subclassName, s_tooltipTextColor, s_tooltipTextColor, 0);
        } else {
            float minimum = GetEffectiveWeaponDamage(level, item->m_itemLevel, itemRec->m_classID, itemRec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], true);

            if (minimum <= 0.0f) {
                minimum = minimum - 0.99995f;
            }

            int32_t low = static_cast<int32_t>(minimum);
            float maximum = GetEffectiveWeaponDamage(level, item->m_itemLevel, itemRec->m_classID, itemRec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], false);

            if (maximum > 0.0f) {
                maximum = maximum + 0.99995f;
            }

            int32_t high = static_cast<int32_t>(maximum);

            if (!low && !high) {
                goto armor;
            }

            if (item->m_damageType) {
                SStrPrintf(buffer, sizeof(buffer), "SPELL_SCHOOL%d_CAP", item->m_damageType);
                FormatString(buffer, -1, GENDER_NOT_APPLICABLE, text, sizeof(text));

                if (itemRec->m_classID == 6) {
                    float ammo = static_cast<float>(static_cast<double>(low + high) * 0.5);
                    FormatString("AMMO_SCHOOL_DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                    SStrPrintf(name, sizeof(name), buffer, ammo, text);
                } else if (low == high) {
                    FormatString("SINGLE_DAMAGE_TEMPLATE_WITH_SCHOOL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                    SStrPrintf(name, sizeof(name), buffer, low, text);
                } else {
                    FormatString("DAMAGE_TEMPLATE_WITH_SCHOOL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                    SStrPrintf(name, sizeof(name), buffer, low, high, text);
                }
            } else if (itemRec->m_classID == 6) {
                float ammo = static_cast<float>(static_cast<double>(low + high) * 0.5);
                FormatString("AMMO_DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(name, sizeof(name), buffer, ammo);
            } else if (low == high) {
                FormatString("SINGLE_DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(name, sizeof(name), buffer, low);
            } else {
                FormatString("DAMAGE_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(name, sizeof(name), buffer, low, high);
            }

            if (itemRec->m_classID == 2) {
                FormatString("SPEED", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                int32_t delay = item->m_delay ? item->m_delay : 1000;
                SStrPrintf(subclassName, sizeof(subclassName), "%s %.2f", buffer, static_cast<double>(delay) * 0.001);
            } else {
                subclassName[0] = '\0';
            }

            this->AddLine(name, subclassName, s_tooltipTextColor, s_tooltipTextColor, 0);
            float damageHigh = GetEffectiveWeaponDamage(level, item->m_itemLevel, itemRec->m_classID, itemRec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], false);
            float damageLow = GetEffectiveWeaponDamage(level, item->m_itemLevel, itemRec->m_classID, itemRec->m_subclassID, item->m_quality, item->m_armorDamageModifier, item->m_delay, item->m_statScalingFactor, item->m_inventoryType, item->m_flags[1], true);
            averageDamage = (damageLow + damageHigh) * 0.5f;
        }

        if (itemRec->m_classID == 2) {
            FormatString("DPS_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            float dps = item->m_delay ? averageDamage / (static_cast<float>(item->m_delay) * 0.001f) : 0.0f;
            SStrPrintf(name, sizeof(name), buffer, dps);
            this->AddLine(name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        }

    armor:
        int32_t armorValue = CGTooltip::GetScalingStatArmor(item, values);

        if (armorValue > 0) {
            FormatString("ARMOR_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            SStrPrintf(text, sizeof(text), buffer, armorValue);
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        } else {
            armorValue = GetEffectiveArmor(level, item->m_itemLevel, itemRec->m_classID, itemRec->m_subclassID, item->m_quality, item->m_inventoryType);

            for (uint32_t i = 0; i < 10; i++) {
                if (item->m_statModifierBonusStat[i] == 50) {
                    armorValue += CalculateItemBonusFromPercent(level, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
                }
            }

            if (armorValue > 0) {
                bool bonusArmor = false;

                for (uint32_t i = 0; i < 10; i++) {
                    if (item->m_statModifierBonusStat[i] == 50) {
                        bonusArmor = true;
                    }
                }

                FormatString("ARMOR_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(text, sizeof(text), buffer, armorValue);

                if (bonusArmor) {
                    this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
                } else {
                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }
            }
        }

        if (distribution) {
            int32_t budget = CGTooltip::GetScalingStatItemBudget(item, values);

            for (uint32_t i = 0; i < 11; i++) {
                int32_t value = 0;
                uint32_t j = 0;

                for (; j < 10; j++) {
                    if (distribution->m_bonus[j] && (value = distribution->m_bonus[j] * budget / 10000) != 0 && distribution->m_statID[j] == s_primaryStatOrder[i]) {
                        break;
                    }
                }

                if (j == 10) {
                    continue;
                }

                buffer[0] = '\0';
                FormatString(s_itemModTokens[s_primaryStatOrder[i]], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                if (buffer[0]) {
                    SStrPrintf(text, sizeof(text), buffer, 2 * (value <= 0) + '+', abs(value));
                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
                }
            }
        } else {
            for (uint32_t i = 0; i < 11; i++) {
                int32_t value = 0;
                uint32_t j = 0;

                for (; j < 10; j++) {
                    value = CalculateItemBonusFromPercent(level, item->m_itemLevel, item->m_statModifierBonusAmount[j], item->m_statPercentEditor[j], item->m_statPercentageOfSocket[j], item->m_quality, item->m_inventoryType);

                    if (!value || item->m_statModifierBonusStat[j] == -1) {
                        continue;
                    }

                    if (reforge) {
                        CGItem_C::ModStatByReforge(level, item, reforge, item->m_statModifierBonusStat[j], value, randomPropertiesId, propertySeed);
                    }

                    if (item->m_statModifierBonusStat[j] == s_primaryStatOrder[i]) {
                        break;
                    }
                }

                if (j == 10) {
                    continue;
                }

                buffer[0] = '\0';
                FormatString(s_itemModTokens[s_primaryStatOrder[i]], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                if (buffer[0]) {
                    SStrPrintf(text, sizeof(text), buffer, 2 * (value <= 0) + '+', abs(value));
                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
                }
            }

            if (reforge) {
                int32_t value = 0;
                CGItem_C::ModStatByReforge(level, item, reforge, reforge->m_targetStat, value, randomPropertiesId, propertySeed);

                for (uint32_t i = 0; i < 11; i++) {
                    if (reforge->m_targetStat != s_primaryStatOrder[i]) {
                        continue;
                    }

                    buffer[0] = '\0';
                    FormatString(s_itemModTokens[s_primaryStatOrder[i]], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                    if (buffer[0]) {
                        SStrPrintf(text, sizeof(text), buffer, 2 * (value <= 0) + '+', abs(value));
                        this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 1);
                    }

                    break;
                }
            }
        }

        int32_t resistEditor = 0;
        int32_t resistValue = 0;
        int32_t resistCount = 0;
        bool singleResists = false;

        for (uint32_t i = 0; i < 10; i++) {
            int32_t stat = item->m_statModifierBonusStat[i];

            if (stat < 51 || stat > 56) {
                continue;
            }

            if (resistEditor && item->m_statPercentEditor[i] != resistEditor) {
                singleResists = true;
                break;
            }

            resistEditor = item->m_statPercentEditor[i];
            resistCount++;
            resistValue = CalculateItemBonusFromPercent(level, item->m_itemLevel, item->m_statModifierBonusAmount[i], resistEditor, item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
        }

        if (!singleResists && resistCount == 5) {
            if (resistValue) {
                FormatString("ITEM_RESIST_ALL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(text, sizeof(text), buffer, 2 * (resistValue <= 0) + '+', abs(resistValue));
                this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        } else {
            char school[32];

            for (uint32_t i = 0; i < 10; i++) {
                int32_t stat = item->m_statModifierBonusStat[i];

                if (stat < 51 || stat > 56) {
                    continue;
                }

                int32_t value = CalculateItemBonusFromPercent(level, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
                SStrPrintf(buffer, sizeof(buffer), "SPELL_SCHOOL%d_CAP", GetDamageClassFromBonusStat(stat));
                FormatString(buffer, -1, GENDER_NOT_APPLICABLE, school, sizeof(school));
                FormatString("ITEM_RESIST_SINGLE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(text, sizeof(text), buffer, 2 * (value <= 0) + '+', abs(value), school);
                this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }

        int32_t numSockets = 0;

        if (itemObj) {
            numSockets = itemObj->GetNumSockets();
        } else {
            for (uint32_t i = 0; i < 3; i++) {
                if (item->m_socketColor[i]) {
                    numSockets++;
                }
            }

            if (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_enchants[6]) {
                SpellItemEnchantmentRec* prismatic = g_spellItemEnchantmentDB.GetRecord(this->m_itemLinkInfo.m_enchants[6]);

                if (prismatic) {
                    for (uint32_t i = 0; i < 3; i++) {
                        if (prismatic->m_effect[i] == 8) {
                            numSockets += prismatic->m_effectPointsMin[i];
                            break;
                        }
                    }
                }
            }
        }

        if (!wrapped) {
            if (itemObj || this->m_itemUseLinkInfo) {
                int32_t allGemsMatch = 1;

                for (int32_t slot = 0; slot < 15; slot++) {
                    if (slot == 8 || slot == 9) {
                        continue;
                    }

                    int32_t enchantId = itemObj ? itemObj->GetEnchantmentId(slot) : this->m_itemLinkInfo.m_enchants[slot];
                    uint32_t socket = slot - 2;
                    int32_t isSocket = socket <= 2 && static_cast<int32_t>(socket) < numSockets;

                    if (socket <= 2 && socketPreview) {
                        WOWGUID gemGuid = socket < 3 ? CGItemSocketInfo::m_gemItems[socket] : WOWGUID::Make(0);
                        CGItem_C* gemItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(gemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
                        SpellItemEnchantmentRec* current = g_spellItemEnchantmentDB.GetRecord(abs(enchantId));

                        if (gemItem) {
                            WOWGUID none = WOWGUID::Make(0);
                            ItemRecSparse* gem = g_itemSparseDB.GetRecord(gemItem->m_obj->m_entryID, none, TooltipItemCallback, this, true);

                            if (gem) {
                                GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(gem->m_gemProperties);

                                if (properties) {
                                    enchantId = properties->m_enchantID;
                                }

                                if (!CGItemSocketInfo::GemMatchesSocket(item, gem, socket)) {
                                    allGemsMatch = 0;
                                }
                            }
                        } else if (!current && item->m_socketColor[socket]) {
                            allGemsMatch = 0;
                        }
                    }

                    if (!enchantId) {
                        if (socket > 2 || !isSocket || socketPreview) {
                            continue;
                        }

                        int32_t color = item->m_socketColor[socket];

                        if (color) {
                            allGemsMatch = 0;
                            const char* identifier = "Unknown";

                            for (uint32_t i = 0; i < 8; i++) {
                                if (s_socketColors[i] == color) {
                                    identifier = s_socketIdentifiers[i];
                                    break;
                                }
                            }

                            char upper[32];
                            SStrCopy(upper, identifier, sizeof(upper));
                            _strupr(upper);
                            SStrPrintf(buffer, sizeof(buffer), "%s%s", "EMPTY_SOCKET_", upper);
                            SStrPrintf(text, sizeof(text), FrameScript_GetText(buffer, -1, GENDER_NOT_APPLICABLE));
                            this->AddLine(text, NULL, s_tooltipGrayColor, s_tooltipGrayColor, 0);
                            SStrPrintf(text, sizeof(text), "Interface\\ItemSocketingFrame\\UI-EmptySocket-%s.blp", identifier);
                            CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
                            CImVector textureColor(0xFF, 0xFF, 0xFF, 0xFF);
                            this->AddTexture(text, coords, textureColor);
                            continue;
                        }

                        int32_t prismaticId = 0;

                        if (itemObj) {
                            prismaticId = itemObj->GetEnchantmentId(6);
                        } else if (this->m_itemUseLinkInfo) {
                            prismaticId = this->m_itemLinkInfo.m_enchants[6];
                        }

                        SpellItemEnchantmentRec* prismatic = g_spellItemEnchantmentDB.GetRecord(prismaticId);

                        if (prismaticId && prismatic && prismatic->m_requiredSkillID) {
                            int32_t index = player->GetSkillIndex(prismatic->m_requiredSkillID);
                            bool skillOk = index >= 0 && static_cast<int32_t>(player->GetSkillValue(index)) >= prismatic->m_requiredSkillRank;
                            bool levelOk = prismatic->m_minLevel <= static_cast<int32_t>(player->m_unit->m_unitFieldLevel);
                            const CImVector& socketColor = skillOk && levelOk ? s_tooltipGrayColor : s_tooltipRedColor;
                            this->AddLine(FrameScript_GetText("EMPTY_SOCKET_PRISMATIC", -1, GENDER_NOT_APPLICABLE), NULL, socketColor, socketColor, 0);
                            CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
                            CImVector textureColor(0xFF, 0xFF, 0xFF, 0xFF);
                            this->AddTexture("Interface\\ItemSocketingFrame\\UI-EmptySocket-Prismatic.blp", coords, textureColor);

                            if (!skillOk) {
                                SkillLineRec* skill = g_skillLineDB.GetRecord(prismatic->m_requiredSkillID);
                                FormatString(prismatic->m_requiredSkillRank ? "SOCKET_ITEM_MIN_SKILL" : "SOCKET_ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                                if (prismatic->m_requiredSkillRank) {
                                    SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", prismatic->m_requiredSkillRank);
                                } else {
                                    SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN");
                                }

                                this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                            }

                            if (!levelOk) {
                                FormatString("SOCKET_ITEM_REQ_LEVEL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                                SStrPrintf(text, sizeof(text), buffer, prismatic->m_minLevel);
                            }
                        } else {
                            this->AddLine(FrameScript_GetText("EMPTY_SOCKET_PRISMATIC", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGrayColor, s_tooltipGrayColor, 0);
                            CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
                            CImVector textureColor(0xFF, 0xFF, 0xFF, 0xFF);
                            this->AddTexture("Interface\\ItemSocketingFrame\\UI-EmptySocket-Prismatic.blp", coords, textureColor);
                        }

                        continue;
                    }

                    if (slot == 6 || (item->m_socketBonus > 0 && item->m_socketBonus == enchantId)) {
                        continue;
                    }

                    SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(abs(enchantId));

                    if (!enchant) {
                        continue;
                    }

                    if (enchant->m_effect[0] == 7) {
                        useSpell = enchant->m_effectArg[0];
                        useCharges = itemObj ? itemObj->GetEnchantmentCharges(slot) : this->m_itemLinkInfo.m_enchantCharges[slot];
                        useEnchant = enchant;
                        continue;
                    }

                    if (enchant->m_effect[0] == 3 && owner == ClntObjMgrGetActivePlayer()) {
                        SpellRec* spell = g_spellDB.GetRecord(enchant->m_effectArg[0]);

                        if (spell && ownerPlayer && !CGUnit_C::CheckSpellPvPUsability(spell, NULL)) {
                            continue;
                        }
                    }

                    CImVector color = s_tooltipTextColor;

                    if (socket > 2 && slot < 10) {
                        color = enchantId > 0 ? s_tooltipGreenColor : s_tooltipBrightRedColor;
                    }

                    int32_t amount = itemObj ? itemObj->GetRandomPropertyOverrideAmount(slot) : CGItem_C::GetRandomPropertyOverrideAmountEx(slot, this->m_itemLinkInfo.m_randomPropertiesId, static_cast<uint16_t>(this->m_itemLinkInfo.m_propertySeed));

                    if (static_cast<uint32_t>(slot - 10) <= 4 && reforge) {
                        for (uint32_t i = 0; i < 3; i++) {
                            if (enchant->m_effect[i] == 5 && enchant->m_effectArg[i] == reforge->m_sourceStat) {
                                CGItem_C::ModStatByReforge(level, item, reforge, enchant->m_effectArg[i], amount, randomPropertiesId, propertySeed);
                            }
                        }
                    }

                    EnchantmentParserParseText(enchant, buffer, sizeof(buffer), amount, level, item);
                    uint32_t timeLeft;

                    if (itemObj && (timeLeft = itemObj->GetEnchantmentTimeLeft(slot)) != 0) {
                        CGTooltip::GetDurationString(text, sizeof(text), static_cast<int64_t>(static_cast<int32_t>(timeLeft)), "ITEM_ENCHANT_TIME_LEFT", buffer, 1, false);
                        needsUpdate = 1;
                    } else {
                        SStrCopy(text, buffer, sizeof(text));
                    }

                    if (!itemObj || !(itemObj->GetItemStaticFlags(0) & 0x2000)) {
                        if (itemObj ? itemObj->GetEnchantmentCharges(slot) : this->m_itemLinkInfo.m_enchantCharges[slot]) {
                            int32_t charges = itemObj ? itemObj->GetEnchantmentCharges(slot) : this->m_itemLinkInfo.m_enchantCharges[slot];

                            if (itemObj && (itemObj->m_item->m_itemFieldEnchantment[slot].unk_000A & 0xF)) {
                                color = s_tooltipGrayColor;
                            } else {
                                char chargesText[64];
                                SStrPrintf(chargesText, sizeof(chargesText), FrameScript_GetText("ITEM_SPELL_CHARGES", -1, GENDER_NOT_APPLICABLE), charges);
                                SStrPrintf(buffer, sizeof(buffer), " (%s)", chargesText);
                                SStrPack(text, buffer, sizeof(text));
                            }
                        }
                    }

                    char conditions[1024];
                    conditions[0] = '\0';
                    bool skillOk = true;
                    bool levelOk = true;

                    if (enchant->m_requiredSkillID) {
                        int32_t index = player->GetSkillIndex(enchant->m_requiredSkillID);

                        if (index < 0 || static_cast<int32_t>(player->GetSkillValue(index)) < enchant->m_requiredSkillRank) {
                            skillOk = false;
                        }
                    }

                    if (enchant->m_minLevel > static_cast<int32_t>(player->m_unit->m_unitFieldLevel)) {
                        levelOk = false;
                    }

                    int32_t socketEnchantId = 0;
                    bool socketOk = true;
                    bool prismaticEmpty = itemObj ? !itemObj->GetEnchantmentId(6) : !this->m_itemLinkInfo.m_enchants[6];
                    bool conditionLine = true;

                    if (slot < 2 || slot > 4) {
                        if (slot < 10) {
                            const CImVector& lineColor = skillOk && levelOk ? color : s_tooltipRedColor;
                            this->AddLine(text, NULL, lineColor, lineColor, 0);
                            conditionLine = false;
                        }
                    } else if (!item->m_socketColor[socket] || !prismaticEmpty) {
                        if (itemObj) {
                            socketEnchantId = itemObj->GetEnchantmentId(slot);
                        } else if (this->m_itemUseLinkInfo) {
                            socketEnchantId = this->m_itemLinkInfo.m_enchants[slot];
                        }

                        if (socketEnchantId) {
                            SpellItemEnchantmentRec* socketEnchant = g_spellItemEnchantmentDB.GetRecord(socketEnchantId);

                            if (socketEnchant && socketEnchant->m_requiredSkillID) {
                                int32_t index = player->GetSkillIndex(socketEnchant->m_requiredSkillID);
                                socketOk = index >= 0 && static_cast<int32_t>(player->GetSkillValue(index)) >= socketEnchant->m_requiredSkillRank && enchant->m_minLevel <= static_cast<int32_t>(player->m_unit->m_unitFieldLevel);
                            }
                        }
                    }

                    if (conditionLine) {
                        CGPlayer_C* gemOwner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(owner, TYPEMASK_PLAYER, __FILE__, __LINE__));
                        int32_t conditionsMet;

                        if (gemOwner) {
                            gemOwner->CountEquippedGems();
                            conditionsMet = GetEnchantmentConditionText(gemOwner, enchant, conditions, sizeof(conditions), 1);
                        } else {
                            conditionsMet = GetEnchantmentConditionText(player, enchant, conditions, sizeof(conditions), 1);
                        }

                        const char* colorString;

                        if (socketOk && skillOk && levelOk) {
                            colorString = conditionsMet ? s_tooltipTextColorString : s_tooltipGrayColorString;
                        } else {
                            colorString = s_tooltipRedColorString;
                        }

                        SStrPrintf(buffer, sizeof(buffer), "|c%s%s%s", colorString, text, "|r");
                        SStrPack(buffer, conditions, sizeof(buffer));
                        this->AddLine(buffer, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                    }

                    if (enchant->m_srcItemID) {
                        WOWGUID none = WOWGUID::Make(0);
                        ItemRecSparse* gem = g_itemSparseDB.GetRecord(enchant->m_srcItemID, none, TooltipItemCallback, this, true);
                        const ItemRec* gemRec;

                        if (gem && (gemRec = gem->GetNonSparse()) != NULL) {
                            if (!CGItemSocketInfo::GemMatchesSocket(item, gem, socket)) {
                                allGemsMatch = 0;
                            }

                            char texture[260];
                            const char* path = ClientDBStringLookup(3);
                            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(gemRec->m_displayInfoID));
                            CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
                            CImVector textureColor(0xFF, 0xFF, 0xFF, 0xFF);
                            this->AddTexture(texture, coords, textureColor);
                        }
                    }

                    if (!skillOk && enchant->m_requiredSkillID) {
                        SkillLineRec* skill = g_skillLineDB.GetRecord(enchant->m_requiredSkillID);
                        FormatString(enchant->m_requiredSkillRank ? "ENCHANT_ITEM_MIN_SKILL" : "ENCHANT_ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                        if (enchant->m_requiredSkillRank) {
                            SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", enchant->m_requiredSkillRank);
                        } else {
                            SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN");
                        }

                        this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                    }

                    if (!levelOk) {
                        FormatString("ENCHANT_ITEM_REQ_LEVEL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                        SStrPrintf(text, sizeof(text), buffer, enchant->m_minLevel);
                        this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                    }

                    if (!socketOk && socketEnchantId) {
                        SpellItemEnchantmentRec* socketEnchant = g_spellItemEnchantmentDB.GetRecord(socketEnchantId);

                        if (socketEnchant && socketEnchant->m_requiredSkillID) {
                            SkillLineRec* skill = g_skillLineDB.GetRecord(socketEnchant->m_requiredSkillID);
                            FormatString(socketEnchant->m_requiredSkillRank ? "SOCKET_ITEM_MIN_SKILL" : "SOCKET_ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                            if (socketEnchant->m_requiredSkillRank) {
                                SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", socketEnchant->m_requiredSkillRank);
                            } else {
                                SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN");
                            }

                            this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                        }
                    }
                }

                if (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_proposedEnchant) {
                    SpellRec* spell = g_spellDB.GetRecord(this->m_itemLinkInfo.m_proposedEnchant);

                    if (spell) {
                        FormatString("ITEM_PROPOSED_ENCHANT", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                        SStrPrintf(text, sizeof(text), buffer, static_cast<const char*>(spell->m_name));
                        this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
                        this->AddLine(FrameScript_GetText("ITEM_ENCHANT_DISCLAIMER", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipBrightRedColor, s_tooltipBrightRedColor, 0);
                    }
                }

                if (item->m_socketBonus > 0) {
                    SpellItemEnchantmentRec* bonus = g_spellItemEnchantmentDB.GetRecord(abs(item->m_socketBonus));

                    if (bonus) {
                        EnchantmentParserParseText(bonus, buffer, sizeof(buffer), 0, level, item);
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SOCKET_BONUS", -1, GENDER_NOT_APPLICABLE), buffer);
                        CImVector bonusColor = allGemsMatch ? s_tooltipGreenColor : s_tooltipGrayColor;
                        this->AddLine(text, NULL, bonusColor, bonusColor, 0);
                    }
                }
            } else {
                for (int32_t slot = 0; slot < 15; slot++) {
                    if (static_cast<uint32_t>(slot - 2) > 2 || !item->m_socketColor[slot - 2]) {
                        continue;
                    }

                    const char* identifier = "Unknown";

                    for (uint32_t i = 0; i < 8; i++) {
                        if (s_socketColors[i] == item->m_socketColor[slot - 2]) {
                            identifier = s_socketIdentifiers[i];
                            break;
                        }
                    }

                    char upper[32];
                    SStrCopy(upper, identifier, sizeof(upper));
                    _strupr(upper);
                    SStrPrintf(buffer, sizeof(buffer), "%s%s", "EMPTY_SOCKET_", upper);
                    SStrPrintf(text, sizeof(text), FrameScript_GetText(buffer, -1, GENDER_NOT_APPLICABLE));
                    this->AddLine(text, NULL, s_tooltipGrayColor, s_tooltipGrayColor, 0);
                    SStrPrintf(text, sizeof(text), "Interface\\ItemSocketingFrame\\UI-EmptySocket-%s.blp", identifier);
                    CAaRect coords = { 0.0f, 0.0f, 1.0f, 1.0f };
                    CImVector textureColor(0xFF, 0xFF, 0xFF, 0xFF);
                    this->AddTexture(text, coords, textureColor);
                }

                if (item->m_socketBonus > 0) {
                    SpellItemEnchantmentRec* bonus = g_spellItemEnchantmentDB.GetRecord(abs(item->m_socketBonus));

                    if (bonus) {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SOCKET_BONUS", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(bonus->m_name));
                        CImVector bonusColor = s_tooltipGrayColor;
                        this->AddLine(text, NULL, bonusColor, bonusColor, 0);
                    }
                }

                if (item->m_randomProperty || item->m_randomSuffix) {
                    this->AddLine(FrameScript_GetText("ITEM_RANDOM_ENCHANT", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
                }
            }
        }

        if (item->m_gemProperties && !socketPreview) {
            GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(item->m_gemProperties);
            SpellItemEnchantmentRec* gemEnchant;

            if (properties && (gemEnchant = g_spellItemEnchantmentDB.GetRecord(abs(properties->m_enchantID))) != NULL) {
                bool skillOk = true;

                if (item->m_requiredSkill) {
                    int32_t index = player->GetSkillIndex(item->m_requiredSkill);
                    skillOk = index >= 0 && static_cast<int32_t>(player->GetSkillValue(index)) >= item->m_requiredSkillRank;
                }

                if (properties->m_minItemLevel > 0) {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("SOCKETING_ITEM_MIN_LEVEL_I", -1, GENDER_NOT_APPLICABLE), properties->m_minItemLevel);
                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }

                const CImVector& gemColor = destroyGem ? s_tooltipGrayColor : (skillOk ? s_tooltipTextColor : s_tooltipRedColor);
                this->AddLine(gemEnchant->m_name, NULL, gemColor, gemColor, 0);
                GetEnchantmentConditionText(player, gemEnchant, text, sizeof(text), 0);

                if (text[0]) {
                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
                }
            }
        }
    }

    if (!wrapped && !socketPreview) {
        if (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_maxDurability == -1 && itemObj) {
            this->m_itemLinkInfo.m_maxDurability = itemObj->GetMaxDurability();
        }

        if ((itemObj && itemObj->GetMaxDurability()) || (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_maxDurability)) {
            FormatString("DURABILITY_TEMPLATE", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            int32_t durability = (itemObj && itemObj->GetMaxDurability()) ? itemObj->GetDurability() : 0;
            int32_t maxDurability = (itemObj && itemObj->GetMaxDurability()) ? itemObj->GetMaxDurability() : 0;

            if (durability >= maxDurability) {
                durability = maxDurability;
            }

            SStrPrintf(text, sizeof(text), buffer, durability, maxDurability);
            const CImVector& durabilityColor = durability ? s_tooltipTextColor : s_tooltipRedColor;
            this->AddLine(text, NULL, durabilityColor, durabilityColor, 0);
        }
    }

    int32_t duration = item->m_duration;

    if (duration > 0) {
        if (itemObj) {
            duration = itemObj->GetExpirationTimeLeft();
        }

        if (duration >= 0 && !socketPreview) {
            CGTooltip::GetDurationString(text, sizeof(text), static_cast<int64_t>(duration), "ITEM_DURATION", NULL, 1, true);
            this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            needsUpdate = 1;
        }
    }

    if (item->m_holidayID) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), GetHolidayName(item->m_holidayID));
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    if (!socketPreview) {
        text[0] = '\0';
        int32_t allowed = 1;
        uint32_t races = item->m_allowableRace;
        uint32_t classes = item->m_allowableClass;
        uint8_t race = player->m_unit->m_unitFieldBytes0[0];
        uint8_t playerClass = player->m_unit->m_unitFieldBytes0[1];

        if (!((races - 1) & races) && !((classes - 1) & classes)) {
            for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
                ChrRacesRec* rec = g_chrRacesDB.GetRecordByIndex(i);

                if (!(rec->m_flags & 0x1) && rec->m_name && ((1 << (rec->m_id - 1)) & races)) {
                    SStrPrintf(text, sizeof(text), "%s ", static_cast<const char*>(rec->m_name));

                    if (race != rec->m_id) {
                        allowed = 0;
                    }

                    break;
                }
            }

            for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
                ChrClassesRec* rec = g_chrClassesDB.GetRecordByIndex(i);

                if (rec && rec->m_name && ((1 << (rec->m_id - 1)) & classes)) {
                    SStrPack(text, rec->m_name, sizeof(text));

                    if (playerClass != rec->m_id) {
                        allowed = 0;
                    }

                    break;
                }
            }

            if (text[0]) {
                char line[128];
                FormatString("RACE_CLASS_ONLY", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                SStrPrintf(line, sizeof(line), buffer, text);
                const CImVector& lineColor = allowed ? s_tooltipTextColor : s_tooltipRedColor;
                this->AddLine(line, NULL, lineColor, lineColor, 0);
            }
        } else {
            int32_t allRaces = 1;
            int32_t allClasses = 1;

            for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
                ChrRacesRec* rec = g_chrRacesDB.GetRecordByIndex(i);

                if (rec && !(rec->m_flags & 0x1) && !((1 << (rec->m_id - 1)) & races)) {
                    allRaces = 0;
                    break;
                }
            }

            for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
                ChrClassesRec* rec = g_chrClassesDB.GetRecordByIndex(i);

                if (rec && !((1 << (rec->m_id - 1)) & classes)) {
                    allClasses = 0;
                    break;
                }
            }

            if (!allRaces) {
                char list[512];
                list[0] = '\0';
                int32_t first = 1;
                allowed = 0;

                for (int32_t i = 0; i < g_chrRacesDB.m_numRecords; i++) {
                    ChrRacesRec* rec = g_chrRacesDB.GetRecordByIndex(i);

                    if (!(rec->m_flags & 0x1) && rec->m_name && ((1 << (rec->m_id - 1)) & races)) {
                        if (!first) {
                            SStrPack(list, ", ", sizeof(list));
                        }

                        SStrPack(list, rec->m_name, sizeof(list));
                        first = 0;

                        if (race == rec->m_id) {
                            allowed = 1;
                        }
                    }
                }

                if (list[0]) {
                    char line[512];
                    FormatString("ITEM_RACES_ALLOWED", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                    SStrPrintf(line, sizeof(line), buffer, list);
                    const CImVector& lineColor = allowed ? s_tooltipTextColor : s_tooltipRedColor;
                    this->AddLine(line, NULL, lineColor, lineColor, 0);
                }
            }

            if (!allClasses) {
                char list[512];
                list[0] = '\0';
                int32_t first = 1;
                allowed = 0;

                for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
                    ChrClassesRec* rec = g_chrClassesDB.GetRecordByIndex(i);

                    if (rec && rec->m_name && ((1 << (rec->m_id - 1)) & classes)) {
                        if (!first) {
                            SStrPack(list, ", ", sizeof(list));
                        }

                        SStrPack(list, rec->m_name, sizeof(list));
                        first = 0;

                        if (playerClass == rec->m_id) {
                            allowed = 1;
                        }
                    }
                }

                if (list[0]) {
                    char line[512];
                    FormatString("ITEM_CLASSES_ALLOWED", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
                    SStrPrintf(line, sizeof(line), buffer, list);
                    const CImVector& lineColor = allowed ? s_tooltipTextColor : s_tooltipRedColor;
                    this->AddLine(line, NULL, lineColor, lineColor, 0);
                }
            }
        }

        if (canUse) {
            canUse = allowed;
        }
    }

    if (distribution) {
        FormatString("ITEM_LEVEL_RANGE_CURRENT", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
        SStrPrintf(text, sizeof(text), buffer, distribution->m_minlevel > 0 ? distribution->m_minlevel : 1, distribution->m_maxlevel, displayedLevel);
        const CImVector& levelColor = static_cast<int32_t>(player->m_unit->m_unitFieldLevel) < distribution->m_minlevel ? s_tooltipRedColor : s_tooltipTextColor;
        this->AddLine(text, NULL, levelColor, levelColor, 0);
    } else if (item->m_requiredLevel > 1) {
        FormatString("ITEM_MIN_LEVEL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
        SStrPrintf(text, sizeof(text), buffer, item->m_requiredLevel);
        const CImVector& levelColor = item->m_requiredLevel <= static_cast<int32_t>(player->m_unit->m_unitFieldLevel) ? s_tooltipTextColor : s_tooltipRedColor;
        this->AddLine(text, NULL, levelColor, levelColor, 0);
    }

    if (itemRec->m_classID == 2 || itemRec->m_classID == 4 || itemRec->m_classID == 5 || itemRec->m_classID == 6) {
        FormatString("ITEM_LEVEL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
        SStrPrintf(text, sizeof(text), buffer, item->m_itemLevel);
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    }

    if (!socketPreview) {
        if (item->m_requiredSkill) {
            SkillLineRec* skill = g_skillLineDB.GetRecord(item->m_requiredSkill);
            int32_t index = player->GetSkillIndex(item->m_requiredSkill);
            bool skillOk = index >= 0 && static_cast<int32_t>(player->GetSkillValue(index)) >= item->m_requiredSkillRank;

            if (item->m_flags[0] & 0x40000) {
                if (index >= 0 && static_cast<int32_t>(player->GetSkillValue(index)) > 0) {
                    const CImVector& skillColor = skillOk ? s_tooltipTextColor : s_tooltipRedColor;
                    this->AddLine(FrameScript_GetText("ITEM_PROSPECTABLE", -1, GENDER_NOT_APPLICABLE), NULL, skillColor, skillColor, 0);
                }
            } else if (item->m_flags[0] & 0x20000000) {
                if (index >= 0 && static_cast<int32_t>(player->GetSkillValue(index)) > 0) {
                    const CImVector& skillColor = skillOk ? s_tooltipTextColor : s_tooltipRedColor;
                    this->AddLine(FrameScript_GetText("ITEM_MILLABLE", -1, GENDER_NOT_APPLICABLE), NULL, skillColor, skillColor, 0);
                }
            } else {
                FormatString(item->m_requiredSkillRank ? "ITEM_MIN_SKILL" : "ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                if (item->m_requiredSkillRank) {
                    SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", item->m_requiredSkillRank);
                } else {
                    SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN");
                }

                const CImVector& skillColor = skillOk ? s_tooltipTextColor : s_tooltipRedColor;
                this->AddLine(text, NULL, skillColor, skillColor, 0);
            }
        }

        const SpellEffectRec* learnEffect = SpellGetEffect(item->m_spellID[0], 0);

        if (learnEffect && learnEffect->m_effect == 36) {
            int32_t learnedSpell = learnEffect->m_effectTriggerSpell;

            if (!learnedSpell) {
                for (uint32_t i = 0; i < 5; i++) {
                    if (item->m_spellTrigger[i] == 6) {
                        learnedSpell = item->m_spellID[i];
                        break;
                    }
                }
            }

            CGUnit_C* learner;

            if (learnEffect->m_implicitTargetA == 5 || learnEffect->m_implicitTargetB == 5) {
                WOWGUID petGuid = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
                learner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(petGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
            } else {
                learner = player;
            }

            if (learner && (learner->IsSpellKnown(learnedSpell) || learner->IsSpellSuperceded(learnedSpell))) {
                this->AddLine(FrameScript_GetText("ITEM_SPELL_KNOWN", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            }
        }

        if (item->m_requiredSpell) {
            SpellRec* spell = g_spellDB.GetRecord(item->m_requiredSpell);

            if (spell) {
                bool known = player->IsSpellKnown(item->m_requiredSpell) || player->IsSpellSuperceded(item->m_requiredSpell);
                SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(spell->m_name));
                const CImVector& spellColor = known ? s_tooltipTextColor : s_tooltipRedColor;
                this->AddLine(text, NULL, spellColor, spellColor, 0);
            }
        }

        if (item->m_requiredHonorRank) {
            bool rankOk = player->m_player->m_playerFieldBytes[3] >= item->m_requiredHonorRank;
            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), player->GetPVPRankString(item->m_requiredHonorRank));
            const CImVector& rankColor = rankOk ? s_tooltipTextColor : s_tooltipRedColor;
            this->AddLine(text, NULL, rankColor, rankColor, 0);
        }

        if (requester == CGMerchantInfo::m_merchant) {
            const VendorItem* vendorItem = CGMerchantInfo::FindItem(VENDOR_ITEM_TYPE_ITEM, itemId);

            if (vendorItem) {
                WOWGUID none = WOWGUID::Make(0);
                ItemExtendedCostRec* cost = g_itemExtendedCostDB.GetRecord(vendorItem->m_extendedCostID, none, NULL, NULL, false);

                if (cost) {
                    if (cost->m_requiredArenaRating > 0) {
                        int32_t bracket = cost->m_arenaBracket;

                        if (bracket > 3) {
                            bracket = 3;
                        } else if (bracket < 0) {
                            bracket = 0;
                        }

                        bool ratingOk = CGArenaTeamInfo::CheckPersonalAndTeamRating(cost->m_requiredArenaRating, bracket) != 0;

                        if (bracket == 2) {
                            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_ARENA_RATING_5V5", -1, GENDER_NOT_APPLICABLE), cost->m_requiredArenaRating);
                        } else {
                            SStrPrintf(text, sizeof(text), FrameScript_GetText(bracket == 1 ? "ITEM_REQ_ARENA_RATING_3V3_BG" : "ITEM_REQ_ARENA_RATING_BG", -1, GENDER_NOT_APPLICABLE), cost->m_requiredArenaRating);
                        }

                        const CImVector& ratingColor = ratingOk ? s_tooltipTextColor : s_tooltipRedColor;
                        this->AddLine(text, NULL, ratingColor, ratingColor, 0);
                    }

                    for (uint32_t i = 0; i < 5; i++) {
                        int32_t currencyId = cost->m_currencyID[i];

                        if (!currencyId || cost->m_currencyCount[i] <= 0 || !(s_extendedCostCurrencyFlags[i] & cost->m_flags) || CGCurrencyTypes::HasCurrency(currencyId, cost->m_currencyCount[i])) {
                            continue;
                        }

                        CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId);

                        if (!currency) {
                            continue;
                        }

                        int32_t count = cost->m_currencyCount[i];

                        if (currency->m_flags & 0x8) {
                            count /= 100;
                        }

                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_AMOUNT_EARNED", -1, GENDER_NOT_APPLICABLE), count, static_cast<const char*>(currency->m_name));
                        this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                    }

                    if (cost->m_itemPurchaseGroup) {
                        ItemPurchaseGroupRec* group = g_itemPurchaseGroupDB.GetRecord(cost->m_itemPurchaseGroup);

                        if (group) {
                            bool owned = false;

                            for (uint32_t i = 0; i < 8; i++) {
                                if (group->m_itemID[i] && player->GetBag()->FindItemOfType(group->m_itemID[i], 8)) {
                                    owned = true;
                                    break;
                                }
                            }

                            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_PURCHASE_GROUP", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(group->m_name));
                            const CImVector& groupColor = owned ? s_tooltipTextColor : s_tooltipRedColor;
                            this->AddLine(text, NULL, groupColor, groupColor, 0);
                        }
                    }

                    if (cost->m_requiredFactionID) {
                        int32_t standing = cost->m_requiredFactionStanding;
                        bool standingOk = GetRepListRepValue(cost->m_requiredFactionID) >= CGReputationInfo::m_reactionThresholds[standing];
                        FactionRec* faction = g_factionDB.GetRecord(cost->m_requiredFactionID);
                        SStrPrintf(buffer, sizeof(buffer), "FACTION_STANDING_LABEL%d", standing + 1);
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_REPUTATION", -1, GENDER_NOT_APPLICABLE), faction ? static_cast<const char*>(faction->m_name) : "UNKNOWN", player->GetFrameScriptText(buffer, -1));
                        const CImVector& standingColor = standingOk ? s_tooltipTextColor : s_tooltipRedColor;
                        this->AddLine(text, NULL, standingColor, standingColor, 0);
                    }

                    if (cost->m_flags & 0x1) {
                        bool inGuild = player->m_obj->m_dataType == 1 && player->m_obj->m_data;
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_PURCHASE_GUILD", -1, GENDER_NOT_APPLICABLE));
                        const CImVector& guildColor = inGuild ? s_tooltipTextColor : s_tooltipRedColor;
                        this->AddLine(text, NULL, guildColor, guildColor, 0);
                    }

                    if (cost->m_requiredGuildLevel > 0) {
                        bool levelOk = static_cast<int32_t>(player->m_player->m_playerGuildlevel) >= cost->m_requiredGuildLevel;
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_PURCHASE_GUILD_LEVEL", -1, GENDER_NOT_APPLICABLE), cost->m_requiredGuildLevel);
                        const CImVector& guildColor = levelOk ? s_tooltipTextColor : s_tooltipRedColor;
                        this->AddLine(text, NULL, guildColor, guildColor, 0);
                    }

                    if (cost->m_requiredAchievement) {
                        AchievementRec* achievement = g_achievementDB.GetRecord(cost->m_requiredAchievement);

                        if (achievement) {
                            bool earned = CGAchievementInfo::GetAchievement(cost->m_requiredAchievement) != NULL;
                            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_PURCHASE_ACHIEVEMENT", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(achievement->m_title));
                            const CImVector& achievementColor = earned ? s_tooltipTextColor : s_tooltipRedColor;
                            this->AddLine(text, NULL, achievementColor, achievementColor, 0);
                        }
                    }
                }

                PlayerConditionRec* condition = g_playerConditionDB.GetRecord(vendorItem->m_playerConditionID);

                if (condition) {
                    this->AddLine(condition->m_failureDescription, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
                }
            }
        }

        if (item->m_requiredCityRank) {
            bool medalOk = ((1 << (item->m_requiredCityRank - 1)) & player->m_player->m_playerFieldPvpMedals) != 0;
            SStrPrintf(buffer, sizeof(buffer), "PVP_MEDAL%d", item->m_requiredCityRank);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), FrameScript_GetText(buffer, -1, GENDER_NOT_APPLICABLE));
            const CImVector& medalColor = medalOk ? s_tooltipTextColor : s_tooltipRedColor;
            this->AddLine(text, NULL, medalColor, medalColor, 0);
        }

        if (item->m_requiredReputationFaction) {
            int32_t standing = item->m_requiredReputationRank;
            bool standingOk = GetRepListRepValue(item->m_requiredReputationFaction) >= CGReputationInfo::m_reactionThresholds[standing];
            FactionRec* faction = g_factionDB.GetRecord(item->m_requiredReputationFaction);
            SStrPrintf(buffer, sizeof(buffer), "FACTION_STANDING_LABEL%d", standing + 1);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_REQ_REPUTATION", -1, GENDER_NOT_APPLICABLE), faction ? static_cast<const char*>(faction->m_name) : "UNKNOWN", player->GetFrameScriptText(buffer, -1));
            const CImVector& standingColor = standingOk ? s_tooltipTextColor : s_tooltipRedColor;
            this->AddLine(text, NULL, standingColor, standingColor, 0);
        }
    }

    bool arena = false;

    if (itemObj && CGGameUI::IsInArenaOrRatedBattleground(&arena)) {
        if (arena) {
            if (!itemObj->CanBeUsedInArenas()) {
                this->AddLine(FrameScript_GetText("ARENA_NOT_USABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            }
        } else if (!itemObj->CanBeUsedInRatedBattleground()) {
            this->AddLine(FrameScript_GetText("RATEDBG_NOT_USABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
        }
    }

    ItemSetRec* itemSet = NULL;
    int32_t canUseSet = 0;
    int32_t numSetItems = 0;
    int32_t numEquipped = 0;
    int32_t equipped[17];
    memset(equipped, 0, sizeof(equipped));

    if (item->m_itemSet && (itemSet = g_itemSetDB.GetRecord(item->m_itemSet)) != NULL && !socketPreview) {
        ItemSetTooltipData data;
        data.m_tooltip = this;
        data.m_set = itemSet;
        data.m_equipped = equipped;
        memset(data.m_slotUsed, 0, sizeof(data.m_slotUsed));

        for (uint32_t i = 0; i < 17; i++) {
            if (!itemSet->m_itemID[i]) {
                data.m_items[i] = NULL;
                continue;
            }

            WOWGUID none = WOWGUID::Make(0);
            ItemRecSparse* setItem = g_itemSparseDB.GetRecord(itemSet->m_itemID[i], none, TooltipItemCallback, this, true);
            data.m_items[i] = setItem;

            if (!setItem) {
                continue;
            }

            uint32_t k = 0;

            for (; k < i; k++) {
                ItemRecSparse* other = data.m_items[k];

                if (!other) {
                    continue;
                }

                int32_t a = setItem->m_inventoryType;
                int32_t b = other->m_inventoryType;

                if (a == b && a != 11 && a != 12 && a != 13) {
                    break;
                }

                if (a == 5 && b == 20) {
                    break;
                }

                if (a == 20 && b == 5) {
                    break;
                }
            }

            if (k == i) {
                numSetItems++;
            }
        }

        for (int32_t pass = 0; pass < 2; pass++) {
            if (!ownerPlayer || ownerPlayer->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
                for (int32_t slot = 0; slot < 19; slot++) {
                    const CGItem_C* slotItem = player->GetBag()->GetItemPointer(slot);

                    if (slotItem && ResolveItemSetItem(&data, pass, slotItem->m_obj->m_entryID, slot)) {
                        numEquipped++;
                    }
                }
            } else {
                for (int32_t slot = 0; slot < 19; slot++) {
                    const InspectItem* inspected = ownerPlayer->GetInspectedItem(slot);
                    int32_t setItemId;

                    if (!inspected || (setItemId = inspected->m_itemId) == 0) {
                        const VisibleItem_C* visible = ownerPlayer->GetVisibleItem(slot);

                        if (!visible) {
                            continue;
                        }

                        setItemId = abs(visible->m_itemId);

                        if (!setItemId) {
                            continue;
                        }
                    }

                    if (ResolveItemSetItem(&data, pass, setItemId, slot)) {
                        numEquipped++;
                    }
                }
            }
        }

        canUseSet = player->MeetsItemSetSkill(itemSet);
    }

    if (distribution && values) {
        int32_t budget = CGTooltip::GetScalingStatItemBudget(item, values);

        if (budget) {
            for (uint32_t i = 0; i < 10; i++) {
                if (!distribution->m_bonus[i]) {
                    continue;
                }

                int32_t value = budget * distribution->m_bonus[i] / 10000;

                if (!value) {
                    continue;
                }

                uint32_t order = 0;

                for (; order < 45; order++) {
                    if (distribution->m_statID[i] == s_secondaryStatOrder[order]) {
                        break;
                    }
                }

                if (order >= 45) {
                    continue;
                }

                buffer[0] = '\0';
                FormatString(s_itemModTokens[s_secondaryStatOrder[order]], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                if (buffer[0]) {
                    if (strstr(buffer, "%c")) {
                        SStrPrintf(name, sizeof(name), buffer, 2 * (value <= 0) + '+', abs(value));
                    } else {
                        SStrPrintf(name, sizeof(name), buffer, value);
                    }

                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONEQUIP", -1, GENDER_NOT_APPLICABLE), name);

                    if (s_secondaryStatOrder[order] == 49 && canUse && (!ownerPlayer || player == ownerPlayer)) {
                        AppendMasterySpells(text, sizeof(text));
                    }

                    this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 1);
                }
            }
        }

        if ((item->m_flags[1] & 0x200) && values->m_spellPower) {
            buffer[0] = '\0';
            FormatString(s_itemModTokens[45], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

            if (buffer[0]) {
                SStrPrintf(name, sizeof(name), buffer, values->m_spellPower);
                SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONEQUIP", -1, GENDER_NOT_APPLICABLE), name);
                this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 1);
            }
        }
    } else {
        int32_t level = player->m_unit->m_unitFieldMaxitemlevel;

        for (uint32_t i = 0; i < 10; i++) {
            int32_t value = CalculateItemBonusFromPercent(level, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);

            if (!value || item->m_statModifierBonusStat[i] == -1) {
                continue;
            }

            if (reforge) {
                CGItem_C::ModStatByReforge(level, item, reforge, item->m_statModifierBonusStat[i], value, randomPropertiesId, propertySeed);
            }

            uint32_t order = 0;

            for (; order < 45; order++) {
                if (item->m_statModifierBonusStat[i] == s_secondaryStatOrder[order]) {
                    break;
                }
            }

            if (order >= 45) {
                continue;
            }

            buffer[0] = '\0';
            FormatString(s_itemModTokens[s_secondaryStatOrder[order]], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

            if (buffer[0]) {
                if (strstr(buffer, "%c")) {
                    SStrPrintf(name, sizeof(name), buffer, 2 * (value <= 0) + '+', abs(value));
                } else {
                    SStrPrintf(name, sizeof(name), buffer, value);
                }

                SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONEQUIP", -1, GENDER_NOT_APPLICABLE), name);

                int32_t wrap = s_secondaryStatOrder[order] == 49 && canUse && (!ownerPlayer || player == ownerPlayer) && AppendMasterySpells(text, sizeof(text));
                this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, wrap);
            }
        }

        if (reforge && !wrapped) {
            int32_t value = 0;
            CGItem_C::ModStatByReforge(level, item, reforge, reforge->m_targetStat, value, randomPropertiesId, propertySeed);

            for (uint32_t order = 0; order < 45; order++) {
                if (reforge->m_targetStat == s_secondaryStatOrder[order]) {
                    buffer[0] = '\0';
                    FormatString(s_itemModTokens[s_secondaryStatOrder[order]], -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));

                    if (buffer[0]) {
                        if (strstr(buffer, "%c")) {
                            SStrPrintf(name, sizeof(name), buffer, 2 * (value <= 0) + '+', abs(value));
                        } else {
                            SStrPrintf(name, sizeof(name), buffer, value);
                        }

                        SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONEQUIP", -1, GENDER_NOT_APPLICABLE), name);

                        int32_t wrap = s_secondaryStatOrder[order] == 49 && canUse && (!ownerPlayer || player == ownerPlayer) && AppendMasterySpells(text, sizeof(text));
                        this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, wrap);
                    }
                    break;
                }
            }
        }
    }

    bool hasDescription = false;

    for (uint32_t i = 0; i < 5; i++) {
        if (stream) {
            char tag[256];
            sprintf(tag, "\t\t\t<spelldesc_%d>", i);
            fputs(tag, stream);
        }

        if (item->m_spellID[i] <= 0) {
            if (stream) {
                char tag[256];
                sprintf(tag, "</spelldesc_%d>\n", i);
                fputs(tag, stream);
            }

            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(item->m_spellID[i]);

        if (!spell) {
            if (stream) {
                char tag[256];
                sprintf(tag, "</spelldesc_%d>\n", i);
                fputs(tag, stream);
            }

            continue;
        }

        SpellParserParseText(spell, buffer, sizeof(buffer), 0, 0, 0, 0, 1, 0, NULL);

        if (!buffer[0] && item->m_spellTrigger[i] != 6) {
            if (stream) {
                char tag[256];
                sprintf(tag, "</spelldesc_%d>\n", i);
                fputs(tag, stream);
            }

            continue;
        }

        switch (item->m_spellTrigger[i]) {
            case 0: {
                int32_t cooldown = item->m_spellCooldown[i];

                if (cooldown < 0 && item->m_spellCategoryCooldown[i] < 0) {
                    int32_t recovery = 0;
                    int32_t categoryRecovery = 0;
                    SpellCooldownsRec* cooldowns = spell->m_spellCooldownsID ? g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID) : NULL;

                    if (cooldowns) {
                        recovery = cooldowns->m_recoveryTime;
                        categoryRecovery = cooldowns->m_categoryRecoveryTime;
                    }

                    Spell_C_ModifySpellValue(spell, recovery, 11);

                    if (spell->m_attributesExF >= 0) {
                        Spell_C_ModifySpellValue(spell, categoryRecovery, 11);
                    }

                    SpellCategoriesRec* categories = spell->m_spellCategoriesID ? g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID) : NULL;
                    Spell_C_ModifyCategoryRecovery(categoryRecovery, categories ? categories->m_category : 0, 1);
                    cooldown = recovery > categoryRecovery ? recovery : categoryRecovery;
                } else if (cooldown < 0) {
                    cooldown = 0;
                }

                if (item->m_spellCategory[i]) {
                    player->ModifyCategoryCooldown(cooldown, item->m_spellCategory[i]);
                }

                if (cooldown <= 0) {
                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), buffer);
                } else {
                    char cooldownText[1024];
                    CGTooltip::GetItemDurationString(cooldownText, sizeof(cooldownText), static_cast<int64_t>(cooldown), "ITEM_COOLDOWN_TOTAL");
                    SStrPrintf(text, sizeof(text), "%s %s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), buffer, cooldownText);
                }

                break;
            }

            case 1:
                SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONEQUIP", -1, GENDER_NOT_APPLICABLE), buffer);
                break;

            case 2:
                SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONPROC", -1, GENDER_NOT_APPLICABLE), buffer);
                break;

            case 6:
                if (itemRec->m_classID == 16) {
                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), FrameScript_GetText("ITEM_GLYPH_ONUSE", -1, GENDER_NOT_APPLICABLE));
                    this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 1);
                    uint32_t count;
                    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
                    bool found = false;

                    for (uint32_t j = 0; j < count && !found; j++) {
                        const SpellEffectRec* effect = effects->m_data[j];

                        if (!effect || effect->m_effect != 74) {
                            continue;
                        }

                        GlyphPropertiesRec* glyph = g_glyphPropertiesDB.GetRecord(effect->m_effectMiscValue);

                        if (!glyph) {
                            continue;
                        }

                        SpellRec* glyphSpell = g_spellDB.GetRecord(glyph->m_spellID);

                        if (!glyphSpell) {
                            continue;
                        }

                        char glyphText[1024];
                        SpellParserParseText(glyphSpell, glyphText, sizeof(glyphText), 0, 0, 0, 0, 1, 0, NULL);

                        if (glyphText[0]) {
                            SStrPrintf(text, sizeof(text), "\n%s", glyphText);
                            found = true;
                        }
                    }

                    if (!found) {
                        SStrPrintf(text, sizeof(text), "\n%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), buffer);
                    }
                } else {
                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(item->m_description));
                }

                hasDescription = true;
                break;

            default:
                SStrCopy(text, buffer, sizeof(text));
                break;
        }

        int32_t isRecipe = 0;
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
        SpellRec* learned = NULL;

        if (item->m_spellTrigger[i] == 6) {
            learned = spell;
        } else if (effect && effect->m_effect == 36) {
            learned = g_spellDB.GetRecord(effect->m_effectTriggerSpell);
            effect = SpellGetEffect(learned->m_id, 0);
        }

        if (learned && effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59) && effect->m_effectItemType) {
            isRecipe = 1;
        }

        if (stream) {
            char xml[2048];
            CGTooltip::FormatStringForXML(buffer, xml);
            fputs(xml, stream);
        }

        const CImVector& spellColor = isRecipe ? s_tooltipTextColor : s_tooltipGreenColor;
        this->AddLine(text, NULL, spellColor, spellColor, 1);

        if (!hideCharges) {
            int32_t charges;

            if (!this->m_itemUseLinkInfo || this->m_itemLinkInfo.m_soulbound) {
                charges = itemObj ? itemObj->m_item->m_itemFieldSpellCharges[i] : item->m_spellCharges[i];
            } else {
                charges = this->m_itemLinkInfo.m_spellCharges;
            }

            if (item->m_spellCharges[i] && item->m_spellCharges[i] != -1) {
                if (abs(charges)) {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SPELL_CHARGES", -1, GENDER_NOT_APPLICABLE), abs(charges));
                } else {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SPELL_CHARGES_NONE", -1, GENDER_NOT_APPLICABLE));
                }

                if (stream) {
                    char xml[2048];
                    CGTooltip::FormatStringForXML(text, xml);
                    fputs(xml, stream);
                }

                this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            }
        }

        if (effect && isRecipe) {
            WOWGUID none = WOWGUID::Make(0);
            this->SetItem(effect->m_effectItemType, none, none, 0, 0, 0, 1, none, 0, NULL, hideCharges, NULL, 0, 0, 1, NULL);
            text[0] = '\0';
            int32_t first = 1;
            int32_t pending = 0;
            SpellReagentsRec* reagents = spell->m_spellReagentsID ? g_spellReagentsDB.GetRecord(learned->m_spellReagentsID) : NULL;

            if (reagents) {
                for (uint32_t j = 0; j < 8; j++) {
                    if (reagents->m_reagent[j] <= 0) {
                        continue;
                    }

                    WOWGUID reagentRequester = WOWGUID::Make(static_cast<int64_t>(learned->m_id) | 0x1FE0000000000000LL);
                    ItemRecSparse* reagent = g_itemSparseDB.GetRecord(reagents->m_reagent[j], reagentRequester, TooltipItemCallback, this, true);

                    if (!reagent) {
                        pending = 1;
                        continue;
                    }

                    if (first) {
                        first = 0;
                    } else {
                        SStrPack(text, ", ", sizeof(text));
                    }

                    if (reagents->m_reagentCount[j] <= 1) {
                        SStrCopy(buffer, reagent->GetName(0), sizeof(buffer));
                    } else {
                        SStrPrintf(buffer, sizeof(buffer), "%s (%d)", reagent->GetName(0), reagents->m_reagentCount[j]);
                    }

                    SStrPack(text, buffer, sizeof(text));
                }

                if (!first && !pending) {
                    SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE), text);
                    SStrPrintf(name, sizeof(name), "\n%s", buffer);

                    if (stream) {
                        char xml[2048];
                        CGTooltip::FormatStringForXML(name, xml);
                        fputs(xml, stream);
                    }

                    this->AddLine(name, NULL, s_tooltipTextColor, s_tooltipTextColor, 1);
                }
            }
        }

        if (stream) {
            char tag[256];
            sprintf(tag, "</spelldesc_%d>\n", i);
            fputs(tag, stream);
        }
    }

    if (useSpell) {
        SpellRec* spell = g_spellDB.GetRecord(useSpell);

        if (spell) {
            SpellParserParseText(spell, buffer, sizeof(buffer), 0, 0, 0, 0, 1, 0, NULL);
            bool skillOk = true;
            bool levelOk = true;

            if (useEnchant->m_requiredSkillID) {
                int32_t index = player->GetSkillIndex(useEnchant->m_requiredSkillID);

                if (index < 0 || static_cast<int32_t>(player->GetSkillValue(index)) < useEnchant->m_requiredSkillRank) {
                    skillOk = false;
                }
            }

            if (useEnchant->m_minLevel > static_cast<int32_t>(player->m_unit->m_unitFieldLevel)) {
                levelOk = false;
            }

            int32_t recovery = 0;
            int32_t categoryRecovery = 0;
            SpellCooldownsRec* cooldowns = spell->m_spellCooldownsID ? g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID) : NULL;

            if (cooldowns) {
                recovery = cooldowns->m_recoveryTime;
                categoryRecovery = cooldowns->m_categoryRecoveryTime;
            }

            Spell_C_ModifySpellValue(spell, recovery, 11);

            if (spell->m_attributesExF >= 0) {
                Spell_C_ModifySpellValue(spell, categoryRecovery, 11);
            }

            SpellCategoriesRec* categories = spell->m_spellCategoriesID ? g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID) : NULL;
            Spell_C_ModifyCategoryRecovery(categoryRecovery, categories ? categories->m_category : 0, 1);
            int32_t cooldown = recovery > categoryRecovery ? recovery : categoryRecovery;

            if (cooldown <= 0) {
                SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), buffer);
            } else {
                char cooldownText[1024];
                CGTooltip::GetItemDurationString(cooldownText, sizeof(cooldownText), static_cast<int64_t>(cooldown), "ITEM_COOLDOWN_TOTAL");
                SStrPrintf(text, sizeof(text), "%s %s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), buffer, cooldownText);
            }

            if (useCharges && useCharges != -1) {
                char chargesText[64];
                SStrPrintf(chargesText, sizeof(chargesText), FrameScript_GetText("ITEM_SPELL_CHARGES", abs(useCharges), GENDER_NOT_APPLICABLE));
                SStrPrintf(buffer, sizeof(buffer), " (%s)", chargesText);
                SStrPack(text, buffer, sizeof(text));
            }

            const CImVector& useColor = skillOk && levelOk ? s_tooltipGreenColor : s_tooltipRedColor;
            this->AddLine(text, NULL, useColor, useColor, 1);

            if (!skillOk && useEnchant->m_requiredSkillID) {
                char skillFormat[256];
                SkillLineRec* skill = g_skillLineDB.GetRecord(useEnchant->m_requiredSkillID);
                FormatString(useEnchant->m_requiredSkillRank ? "ENCHANT_ITEM_MIN_SKILL" : "ENCHANT_ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE, skillFormat, sizeof(skillFormat));

                if (useEnchant->m_requiredSkillRank) {
                    SStrPrintf(text, sizeof(text), skillFormat, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", useEnchant->m_requiredSkillRank);
                } else {
                    SStrPrintf(text, sizeof(text), skillFormat, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN");
                }

                this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            }

            if (!levelOk) {
                char levelFormat[256];
                FormatString("ENCHANT_ITEM_REQ_LEVEL", -1, GENDER_NOT_APPLICABLE, levelFormat, sizeof(levelFormat));
                SStrPrintf(text, sizeof(text), levelFormat, useEnchant->m_minLevel);
                this->AddLine(text, NULL, s_tooltipRedColor, s_tooltipRedColor, 0);
            }
        }
    }

    if (itemSet && !socketPreview) {
        this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
        FormatString("ITEM_SET_NAME", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
        SStrPrintf(text, sizeof(text), buffer, static_cast<const char*>(itemSet->m_name), numEquipped, numSetItems);
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);

        if (itemSet->m_requiredSkill) {
            FormatString(itemSet->m_requiredSkillRank ? "ITEM_MIN_SKILL" : "ITEM_REQ_SKILL", -1, GENDER_NOT_APPLICABLE, buffer, sizeof(buffer));
            SkillLineRec* skill = g_skillLineDB.GetRecord(itemSet->m_requiredSkill);

            if (itemSet->m_requiredSkillRank) {
                SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN", itemSet->m_requiredSkillRank);
            } else {
                SStrPrintf(text, sizeof(text), buffer, skill ? static_cast<const char*>(skill->m_displayName) : "UNKNOWN");
            }

            int32_t index = player->GetSkillIndex(itemSet->m_requiredSkill);
            const CImVector& skillColor = index < 0 || static_cast<int32_t>(player->GetSkillValue(index)) < itemSet->m_requiredSkillRank ? s_tooltipRedColor : s_tooltipTextColor;
            this->AddLine(text, NULL, skillColor, skillColor, 0);
        }

        for (uint32_t i = 0; i < 17; i++) {
            if (!itemSet->m_itemID[i]) {
                continue;
            }

            WOWGUID none = WOWGUID::Make(0);
            ItemRecSparse* setItem = g_itemSparseDB.GetRecord(equipped[i] ? equipped[i] : itemSet->m_itemID[i], none, NULL, NULL, false);
            const char* setItemName;

            if (setItem && (setItemName = setItem->GetName(0)) != NULL) {
                SStrPrintf(text, sizeof(text), "  %s", setItemName);
                const CImVector& setColor = equipped[i] ? s_tooltipItemSetColor : s_tooltipGrayColor;
                this->AddLine(text, NULL, setColor, setColor, 0);
            }
        }

        this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
        uint8_t order[8];

        for (uint32_t i = 0; i < 8; i++) {
            order[i] = i;
        }

        s_sortItemSet = itemSet;
        qsort(order, 8, 1, SortItemSetBonuses);

        for (uint32_t i = 0; i < 8; i++) {
            uint8_t index = order[i];

            if (stream) {
                char tag[256];
                sprintf(tag, "\t\t\t<setspelldesc_%d>", i);
                fputs(tag, stream);
            }

            SpellRec* bonus = itemSet->m_setSpellID[index] ? g_spellDB.GetRecord(itemSet->m_setSpellID[index]) : NULL;

            if (bonus) {
                SpellParserParseText(bonus, buffer, sizeof(buffer), 0, 0, 0, 0, 1, 0, NULL);

                if (canUseSet && numEquipped >= itemSet->m_setThreshold[index]) {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SET_BONUS", -1, GENDER_NOT_APPLICABLE), buffer);
                    this->AddLine(text, NULL, s_tooltipGreenColor, s_tooltipGreenColor, 1);
                } else {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_SET_BONUS_GRAY", -1, GENDER_NOT_APPLICABLE), itemSet->m_setThreshold[index], buffer);
                    this->AddLine(text, NULL, s_tooltipGrayColor, s_tooltipGrayColor, 1);
                }

                if (stream) {
                    char xml[2048];
                    CGTooltip::FormatStringForXML(buffer, xml);
                    fputs(xml, stream);
                }
            }

            if (stream) {
                char tag[256];
                sprintf(tag, "</setspelldesc_%d>\n", i);
                fputs(tag, stream);
            }
        }
    } else if (stream) {
        for (uint32_t i = 0; i < 8; i++) {
            char tag[256];
            sprintf(tag, "\t\t\t<setspelldesc_%d></setspelldesc_%d>\n", i, i);
            fputs(tag, stream);
        }
    }

    if (this->m_itemUseLinkInfo && this->m_itemLinkInfo.m_cooldown && !socketPreview) {
        CGTooltip::GetItemDurationString(text, sizeof(text), this->m_itemLinkInfo.m_cooldown, "ITEM_COOLDOWN_TIME");
        this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
        needsUpdate = 1;
    } else {
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(itemObj, &duration, NULL, &enable);

        if (duration && !enable) {
            this->AddLine(FrameScript_GetText("COOLDOWN_ON_LEAVE_COMBAT", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
            needsUpdate = 1;
        }
    }

    if (nameOnly) {
        if (this->HasScript(this->m_onTooltipSetItem)) {
            this->RunScript(this->m_onTooltipSetItem, 0, NULL);
        }

        this->Show();
        this->CalculateSize();
        return needsUpdate;
    }

    if (!hasDescription && item->m_description && *item->m_description) {
        SStrPrintf(text, sizeof(text), "\"%s\"", static_cast<const char*>(item->m_description));
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
    }

    if (!socketPreview) {
        bool checkFlags = true;

        if (wrapped) {
            WOWGUID giftCreator;

            if (itemObj) {
                giftCreator = itemObj->m_item->m_itemFieldGiftcreator;
            } else if (this->m_itemUseLinkInfo) {
                giftCreator = this->m_itemLinkInfo.m_giftCreator;
            } else {
                checkFlags = false;
            }

            if (checkFlags && giftCreator) {
                NameCache* creator = g_nameDBCache.GetRecord(giftCreator.m_guid, giftCreator, TooltipItemCallback, this, true);

                if (creator) {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_WRAPPED_BY", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(creator->m_name));
                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }
            }
        } else {
            WOWGUID creatorGuid;

            if (itemObj) {
                creatorGuid = itemObj->m_item->m_itemFieldCreator;
            } else if (this->m_itemUseLinkInfo) {
                creatorGuid = this->m_itemLinkInfo.m_creator;
            } else {
                checkFlags = false;
            }

            if (checkFlags && creatorGuid) {
                NameCache* creator = g_nameDBCache.GetRecord(creatorGuid.m_guid, creatorGuid, TooltipItemCallback, this, true);

                if (creator) {
                    if (itemObj && (itemObj->m_item->m_itemFieldFlags & 0x200)) {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_WRITTEN_BY", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(creator->m_name));
                    } else {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_CREATED_BY", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(creator->m_name));
                    }

                    this->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }
            }
        }

        const char* usage = NULL;
        bool checkSockets = false;

        if (itemObj && !this->m_itemUseLinkInfo && (((item->m_flags[0] & 0x4) && (!item->m_lockID || (itemObj->m_item->m_itemFieldFlags & 0x4))) || ((item->m_flags[0] & 0x200) && (itemObj->m_item->m_itemFieldFlags & 0x8)))) {
            usage = "ITEM_OPENABLE";
            checkSockets = true;
        } else if (item->m_pageText || (itemObj && (itemObj->m_item->m_itemFieldFlags & 0x200))) {
            usage = itemObj ? "ITEM_READABLE" : "ITEM_CAN_BE_READ";
            checkSockets = itemObj != NULL;
        } else {
            checkSockets = itemObj != NULL;
        }

        if (usage) {
            this->AddLine(FrameScript_GetText(usage, -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        }

        if (checkSockets && itemObj->GetNumSockets()) {
            this->AddLine(FrameScript_GetText("ITEM_SOCKETABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
        }

        if (!append && Spell_C_IsTargeting()) {
            uint8_t race = player->m_unit->m_unitFieldBytes0[0];
            uint8_t playerClass = player->m_unit->m_unitFieldBytes0[1];
            SkillLineAbilityRec* ability = SpellTableLookupAbility(race, playerClass, Spell_C_GetTargetingSpell());
            int32_t skillId = ability ? ability->m_skillLine : -1;
            SkillLineRec* skill = g_skillLineDB.GetRecord(skillId);
            const SpellEffectRec* effect = SpellGetEffect(Spell_C_GetTargetingSpell(), 0);

            if (skill && ability && effect && effect->m_effect == 99) {
                int32_t rank = player->GetSkillRank(ability->m_skillLine);
                const CImVector* disenchantColor;

                if (item->GetDisenchantSkillRequired() <= 0) {
                    if (!item->GetDisenchantSkillRequired()) {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_DISENCHANT_ANY_SKILL", -1, GENDER_NOT_APPLICABLE));
                        disenchantColor = &s_tooltipDisenchantColor;
                    } else {
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_DISENCHANT_NOT_DISENCHANTABLE", -1, GENDER_NOT_APPLICABLE));
                        disenchantColor = &s_tooltipRedColor;
                    }
                } else {
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("ITEM_DISENCHANT_MIN_SKILL", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(skill->m_displayName), item->GetDisenchantSkillRequired());
                    disenchantColor = rank >= item->GetDisenchantSkillRequired() ? &s_tooltipDisenchantColor : &s_tooltipRedColor;
                }

                this->AddLine(text, NULL, *disenchantColor, *disenchantColor, 0);
            }
        }
    }

    CVar* hideSets;
    const char* setsFormat;

    if (itemGuid && ((hideSets = CVar::Lookup("dontShowEquipmentSetsOnItems")) == NULL || !hideSets->m_intValue) && (setsFormat = FrameScript_GetText("EQUIPMENT_SETS", -1, GENDER_NOT_APPLICABLE)) != NULL && CGEquipmentManager::GetSetList(buffer, sizeof(buffer), itemGuid)) {
        SStrPrintf(text, sizeof(text), setsFormat, buffer);
        this->AddLine(text, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 1);
    }

    bool purchasePending = false;

    if (itemObj) {
        CGItem_C::ItemPurchaseData* purchase = itemObj->m_itemPurchaseData;

        if (purchase) {
            if (!(purchase->m_flags & 0x1)) {
                uint32_t refundTime = purchase->m_refundTimeLeft;

                if (refundTime >= itemObj->m_item->m_itemFieldCreatePlayedTime) {
                    refundTime = itemObj->m_item->m_itemFieldCreatePlayedTime;
                }

                int32_t remaining = static_cast<int32_t>(refundTime - player->GetPlayedTime() + 7200);

                if (remaining > 0) {
                    if (static_cast<uint32_t>(remaining - 3660) > 3539) {
                        CGTooltip::GetDurationString(buffer, sizeof(buffer), static_cast<int64_t>(remaining), "INT_SPELL_DURATION", NULL, 0, true);
                    } else {
                        char minutes[256];
                        SStrPrintf(text, sizeof(text), FrameScript_GetText("INT_SPELL_DURATION_HOURS", 1, GENDER_NOT_APPLICABLE), 1);
                        int32_t minuteCount = (remaining - 3600) / 60;
                        SStrPrintf(minutes, sizeof(minutes), FrameScript_GetText("INT_SPELL_DURATION_MIN", minuteCount, GENDER_NOT_APPLICABLE), minuteCount);
                        SStrPrintf(buffer, sizeof(buffer), "%s%s%s", text, FrameScript_GetText("TIME_UNIT_DELIMITER", -1, GENDER_NOT_APPLICABLE), minutes);
                    }

                    SStrPrintf(text, sizeof(text), FrameScript_GetText("REFUND_TIME_REMAINING", -1, GENDER_NOT_APPLICABLE), buffer);
                    this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                    this->AddLine(text, NULL, s_tooltipRefundColor, s_tooltipRefundColor, 1);
                    purchasePending = true;
                }
            }
        } else if ((item->m_flags[0] & 0x1000) && !(itemObj->m_itemFlags & 0x4)) {
            itemObj->RequestItemPurchaseData();
            purchasePending = true;
        }

        if (itemObj->IsBound() && !itemObj->IsPermanentlyBoundForTrade()) {
            int32_t remaining = static_cast<int32_t>(itemObj->m_item->m_itemFieldCreatePlayedTime - player->GetPlayedTime() + 7200);

            if (remaining > 0) {
                if (static_cast<uint32_t>(remaining - 3660) > 3539) {
                    CGTooltip::GetDurationString(buffer, sizeof(buffer), static_cast<int64_t>(remaining), "INT_SPELL_DURATION", NULL, 0, true);
                } else {
                    char minutes[256];
                    SStrPrintf(text, sizeof(text), FrameScript_GetText("INT_SPELL_DURATION_HOURS", 1, GENDER_NOT_APPLICABLE), 1);
                    int32_t minuteCount = (remaining - 3600) / 60;
                    SStrPrintf(minutes, sizeof(minutes), FrameScript_GetText("INT_SPELL_DURATION_MIN", minuteCount, GENDER_NOT_APPLICABLE), minuteCount);
                    SStrPrintf(buffer, sizeof(buffer), "%s%s%s", text, FrameScript_GetText("TIME_UNIT_DELIMITER", -1, GENDER_NOT_APPLICABLE), minutes);
                }

                SStrPrintf(text, sizeof(text), FrameScript_GetText("BIND_TRADE_TIME_REMAINING", -1, GENDER_NOT_APPLICABLE), buffer);
                this->AddLine(" ", NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
                this->AddLine(text, NULL, s_tooltipBindTradeColor, s_tooltipBindTradeColor, 1);
            }
        }
    }

    if (CursorGetResetMode() != 17) {
        const VendorItem* vendorItem = CGMerchantInfo::FindItem(VENDOR_ITEM_TYPE_ITEM, itemId);

        if (requester == CGMerchantInfo::m_merchant && vendorItem && (!vendorItem->m_extendedCostID || vendorItem->m_buyCount <= 1)) {
            CGUnit_C* merchant = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(requester, TYPEMASK_UNIT, __FILE__, __LINE__));
            WOWGUID none = WOWGUID::Make(0);
            ItemRecSparse* vendorRec = g_itemSparseDB.GetRecord(itemId, none, TooltipItemCallback, this, true);

            if (merchant && (merchant->m_unit->m_unitNpcFlags & 0x80) && vendorRec->m_stackable > 1) {
                this->AddLine(FrameScript_GetText("ITEM_VENDOR_STACK_BUY", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipGreenColor, s_tooltipGreenColor, 0);
            }
        }

        if (!purchasePending && !this->m_itemSkipSellPrice && !skipSellPrice && !(item->m_flags[1] & 0x1000)) {
            int32_t price = item->GetSellPrice();
            int32_t maxPrice = -1;

            if (price) {
                if (itemObj) {
                    if (itemObj->GetEnchantmentSpell(0) && !itemObj->HasOnUseEnchant() && itemObj->GetUsableSpellCharges() < -1) {
                        price = price * itemObj->GetCurrentSpellCharges() / itemObj->GetUsableSpellCharges();
                    }

                    int32_t repairCost = itemObj->GetRepairCost();

                    if (repairCost < price) {
                        if (repairCost) {
                            price -= repairCost;
                        }
                    } else {
                        price = 1;
                    }

                    int32_t stackCount = itemObj->m_item->m_itemFieldStackCount;

                    if (stackCount > 1) {
                        price *= stackCount;
                    }
                } else if (this->m_itemUseLinkInfo) {
                    price *= this->m_itemLinkInfo.m_stackCount;

                    if (this->m_itemLinkInfo.m_maxStackCount > this->m_itemLinkInfo.m_stackCount) {
                        maxPrice = this->m_itemLinkInfo.m_maxStackCount * item->GetSellPrice();
                    }
                }

                this->AddMoney(price, maxPrice);
            } else if (CGMerchantInfo::m_merchant && itemObj) {
                if (itemObj->m_item->m_itemFieldContained != player->m_obj->m_guid || player->GetBag()->FindItemSlot(itemObj->m_obj->m_guid) >= 23) {
                    this->AddLine(FrameScript_GetText("ITEM_UNSELLABLE", -1, GENDER_NOT_APPLICABLE), NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
                }
            }
        }
    }

    if (this->HasScript(this->m_onTooltipSetItem)) {
        this->RunScript(this->m_onTooltipSetItem, 0, NULL);
    }

    this->Show();
    this->CalculateSize();
    return needsUpdate;
}

// OFFSET: 0x00480330
int32_t CGItemStatsSummary::CreateFromTooltip(CGTooltip* tooltip) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(tooltip->m_itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        return this->Create(item, tooltip->m_itemDisplayedLevel) ? 1 : 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(tooltip->m_itemId, none, NULL, NULL, false);

    if (rec && this->Create(rec, &tooltip->m_itemLinkInfo, tooltip->m_itemDisplayedLevel)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x004803F0/0x14009E6A0
int32_t CGTooltip::SetMacro(uint32_t macroId) {
    this->ClearTooltip();
    MACRONODE* macro = CGUIMacros::GetMacroByID(macroId);

    if (!macro) {
        this->Hide();
        return 0;
    }

    uint32_t dynamic = macro->m_dynamic;

    if (macro->m_showTooltip) {
        if (macro->m_itemID) {
            uint32_t duration = 0;
            uint32_t start = 0;
            uint32_t enable = 0;
            CGItem_C* enchantItem = CGActionBar::FindUseEnchantItemPtr(macro->m_itemID);

            if (enchantItem) {
                Spell_C_GetItemCooldown(enchantItem, &duration, &start, &enable);
            } else {
                Spell_C_GetItemCooldown(macro->m_itemID, &duration, &start, &enable);
            }

            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
            CGItem_C* item;

            if (player && (item = player->m_bag.FindItemOfType(macro->m_itemID, 0)) != NULL) {
                this->m_itemLinkInfo.Clear();
                this->m_itemLinkInfo.m_cooldown = enable && start && duration ? duration + start - FrameTime::GetCurTimeMs() : 0;
                WOWGUID none = WOWGUID::Make(0);
                ItemRecSparse* rec = g_itemSparseDB.GetRecord(macro->m_itemID, none, NULL, NULL, false);

                if (rec && rec->m_spellCharges[0] != -1 && rec->m_spellCharges[0]) {
                    this->m_itemLinkInfo.m_spellCharges = player->m_bag.GetItemTypeCharges(macro->m_itemID, 0);
                }

                WOWGUID guid = item->m_obj->m_guid;
                return this->SetItem(item->m_obj->m_entryID, guid, guid, 0, 0, 1, 0, WOWGUID::Make(0), 0, NULL, 0, NULL, 0, 0, 0, NULL) | dynamic;
            }

            WOWGUID none = WOWGUID::Make(0);
            return this->SetItem(macro->m_itemID, none, none, 0, 0, 0, 0, WOWGUID::Make(0), 0, NULL, 0, NULL, 0, 0, 0, NULL) | dynamic;
        }

        if (macro->m_spellID) {
            uint32_t start = 0;
            uint32_t duration = 0;
            uint32_t enable = 0;
            Spell_C_GetSpellCooldown(macro->m_spellID, macro->m_spellFlags, &start, &duration, &enable, 0);
            int32_t brief = 1;
            CVar* uber = CVar::Lookup("UberTooltips");

            if (uber && uber->m_intValue) {
                brief = 0;
            }

            if (!enable || !duration || !start) {
                return this->SetSpell(macro->m_spellID, brief, 0, 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0) | dynamic;
            }

            return this->SetSpell(macro->m_spellID, brief, duration + start - FrameTime::GetCurTimeMs(), 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0) | dynamic;
        }
    }

    this->AddLine(macro->m_name, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    this->Show();
    return 0;
}

// OFFSET: 0x00480B40
int32_t CGItemStatsSummary::GetItemComparison(CGTooltip* first, CGTooltip* second, CGItemStatsSummary& result) {
    struct ComparisonEntry {
        CGItemStatsSummary m_summary;
        int32_t m_itemIds[2];
        int32_t m_randomProperties[2];
        uint32_t m_seeds[2];
        int32_t m_levels[2];

        ComparisonEntry() {
            this->m_summary.Clear();
            memset(this->m_itemIds, 0, sizeof(this->m_itemIds));
            memset(this->m_randomProperties, 0, sizeof(this->m_randomProperties));
            memset(this->m_seeds, 0, sizeof(this->m_seeds));
            memset(this->m_levels, 0, sizeof(this->m_levels));
        }
    };

    static ComparisonEntry s_cache[2];
    int32_t itemIds[2];
    int32_t randomProperties[2];
    uint32_t seeds[2];
    int32_t levels[2];

    for (int32_t i = 0; i < 2; i++) {
        CGTooltip* tooltip = i ? second : first;
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(tooltip->m_itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            itemIds[i] = item->m_obj->m_entryID;
            randomProperties[i] = item->m_item->m_itemFieldRandomPropertiesId;
            seeds[i] = item->m_item->m_itemFieldPropertySeed;
        } else {
            randomProperties[i] = tooltip->m_itemLinkInfo.m_randomPropertiesId;
            seeds[i] = tooltip->m_itemLinkInfo.m_propertySeed;
            itemIds[i] = tooltip->m_itemId;
        }

        levels[i] = tooltip->m_itemDisplayedLevel;
    }

    for (uint32_t i = 0; i < 2; i++) {
        ComparisonEntry& entry = s_cache[i];

        if (itemIds[0] == entry.m_itemIds[0] && randomProperties[0] == entry.m_randomProperties[0] && (!randomProperties[0] || seeds[0] == entry.m_seeds[0]) && levels[0] == entry.m_levels[0] && itemIds[1] == entry.m_itemIds[1] && randomProperties[1] == entry.m_randomProperties[1] && (!randomProperties[1] || seeds[1] == entry.m_seeds[1]) && levels[1] == entry.m_levels[1]) {
            memcpy(&result, &entry.m_summary, sizeof(CGItemStatsSummary));
            return 1;
        }
    }

    CGItemStatsSummary firstSummary;
    firstSummary.Clear();
    CGItemStatsSummary secondSummary;
    secondSummary.Clear();

    if (!firstSummary.CreateFromTooltip(first) || !secondSummary.CreateFromTooltip(second)) {
        return 0;
    }

    int32_t index = 1 - s_comparisonIndex;
    ComparisonEntry& entry = s_cache[index];
    entry.m_itemIds[0] = itemIds[0];
    entry.m_randomProperties[0] = randomProperties[0];
    entry.m_seeds[0] = seeds[0];
    entry.m_levels[0] = levels[0];
    entry.m_itemIds[1] = itemIds[1];
    entry.m_randomProperties[1] = randomProperties[1];
    entry.m_seeds[1] = seeds[1];
    entry.m_levels[1] = levels[1];
    s_comparisonIndex = index;
    CGItemStatsSummary delta = firstSummary - secondSummary;
    memcpy(&entry.m_summary, &delta, sizeof(CGItemStatsSummary));
    memcpy(&result, &delta, sizeof(CGItemStatsSummary));
    return 1;
}

// OFFSET: 0x00481010
int32_t SetCompareItem(CGTooltip* tooltip, int32_t itemId, int32_t offset, int32_t shift, CGTooltip* compare) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID none = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, none, TooltipItemCallback, tooltip, false);

    if (!player || !rec) {
        return 0;
    }

    uint32_t slots = s_inventoryTypeSlots[rec->m_inventoryType];

    if (offset == 2) {
        if (rec->m_scalingStatDistribution && CGMerchantInfo::m_merchant && !shift) {
            WOWGUID itemGuid = WOWGUID::Make(0);
            WOWGUID merchant = CGMerchantInfo::m_merchant;
            tooltip->SetItem(itemId, merchant, itemGuid, 0, 0, 0, 0, WOWGUID::Make(0), 0, NULL, 0, NULL, 1, 0, 0, NULL);
            return 1;
        }

        return 0;
    }

    const CGItem_C* item = NULL;

    if ((slots & 0x8000) || (slots & 0x10000)) {
        if (((slots & 0x8000) && (slots & 0x10000)) || rec->m_inventoryType == 17) {
            const CGItem_C* mainHand = player->m_bag.GetItemPointer(15);
            const CGItem_C* offHand = player->m_bag.GetItemPointer(16);

            if (mainHand != offHand && mainHand && offHand) {
                item = offset ? mainHand : offHand;
            } else if (offset) {
                return 0;
            } else {
                item = mainHand ? mainHand : offHand;
            }
        } else {
            if (offset) {
                return 0;
            }

            if (slots & 0x8000) {
                item = player->m_bag.GetItemPointer(15);
            } else if (!(slots & 0x10000)) {
                return 0;
            } else {
                item = player->m_bag.GetItemPointer(16);

                if (!item) {
                    const CGItem_C* mainHand = player->m_bag.GetItemPointer(15);
                    item = mainHand && mainHand->GetInventoryType() == 17 ? mainHand : NULL;
                }
            }
        }

        if (!item) {
            return 0;
        }
    } else {
        for (int32_t slot = 0; slot <= 18; slot++) {
            if (!((1 << slot) & slots)) {
                continue;
            }

            const CGItem_C* candidate = player->m_bag.GetItemPointer(slot);

            if (!candidate) {
                continue;
            }

            if (!offset) {
                item = candidate;
                break;
            }

            offset--;
        }

        if (!item) {
            return 0;
        }
    }

    WOWGUID itemGuid = item->m_obj->m_guid;
    WOWGUID requester = player->m_obj->m_guid;
    tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 1, 0, 0, WOWGUID::Make(0), 0, NULL, 0, NULL, 0, 0, 0, NULL);

    if (compare) {
        CGItemStatsSummary summary;
        summary.Clear();

        if (CGItemStatsSummary::GetItemComparison(compare, tooltip, summary)) {
            tooltip->SetItemDelta(summary, 1);
        }
    }

    return 1;
}

// INLINED
void CGTooltip::operator delete(void* ptr) {
    CGTooltip::s_tooltipHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x008C65E0
CSimpleFrame* CGTooltip::Create(CSimpleFrame* parent) {
    void* data = CGTooltip::s_tooltipHeap.GetData(0, typeid(CGTooltip).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGTooltip(parent) : NULL;
}

// OFFSET: 0x008CC0B0
void CGTooltip::SetDefaultAnchor() {
    if (this->HasScript(this->m_onTooltipSetDefaultAnchor)) {
        this->RunScript(this->m_onTooltipSetDefaultAnchor, 0, NULL);
    }
}

// OFFSET: 0x004663A0/0x140079AB0
const char* CGTooltip::GetCurrencyHyperlinkString(int32_t currencyId) {
    const char* reset = *s_currencyLinkColor ? "|r" : "";
    CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId);

    if (!currency) {
        return NULL;
    }

    SStrPrintf(s_itemLink, sizeof(s_itemLink), "|c%s|Hcurrency:%d|h[%s]|h%s", s_currencyLinkColor, currencyId, static_cast<const char*>(currency->m_name), reset);
    return s_itemLink;
}
