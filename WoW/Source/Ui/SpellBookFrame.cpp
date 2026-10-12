#include "SpellBookFrame.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CastableRaidBuffsRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"
#include "WoW/Source/DB/Rec/SpellActivationOverlayRec.h"
#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"
#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"
#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellLevelsRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftRec.h"
#include "WoW/Source/DB/Rec/SpellTotemsRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitAreaModelRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/Spell.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/Flyout.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Common/Lightning.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"

// STATIC: 0x00F33BD8/0x140E26BB0
static SpellBookIntroSlot s_introSlots[12];

// STATIC: 0x00F33C38/0x140E26C10
static uint32_t s_dispelMask;

// STATIC: 0x00F33C3C/0x140E26C14
static uint32_t s_spellStealMask;

// STATIC: 0x00F33C40/0x140E26C18
static int32_t s_skinningSpells[4];

// STATIC: 0x00F33C50
static int32_t s_skinPlayerCorpseSpell;

// STATIC: 0x00F33C54/0x140E26C2C
static int32_t s_pendingCompanions;

// STATIC: 0x00F33C58/0x140E26C30
int32_t CGSpellBook::m_autoRangedSpell;

// STATIC: 0x00F33C5C/0x140E26C34
int32_t CGSpellBook::m_summonFriendSpell;

// STATIC: 0x00F33C60/0x140E26C38
int32_t CGSpellBook::m_stuckSpell;

// STATIC: 0x00F33C64/0x140E26C3C
int32_t CGSpellBook::m_duelSpell;

// STATIC: 0x00F33C68/0x140E26C40
uint32_t s_numSkillInfos;

// STATIC: 0x00F33C6C/0x140E26C44
uint32_t CGSpellBook::m_interruptSchoolMask;

// STATIC: 0x00F33C70/0x140E26C48
uint32_t CGSpellBook::m_interruptSpecialSchoolMask;

// STATIC: 0x00F33C74/0x140E26C4C
uint32_t CGSpellBook::m_silenceSchoolMask;

// STATIC: 0x00F33C78/0x140E26C50
uint32_t CGSpellBook::m_silenceSpecialSchoolMask;

// STATIC: 0x00F33C7C/0x140E26C54
static uint8_t s_dontAutoPushSpells;

// STATIC: 0x00F33C80/0x140E26C58
TSBitField<TSFixedArray<uint32_t> > s_knownSpellBits;

// STATIC: 0x00F33C90/0x140E26C70
TSGrowableArray<SpellBookItem*> s_spellBookItems;

// STATIC: 0x00F33CA0/0x140E26C88
TSGrowableArray<SpellBookItem*> s_petSpellBookItems;

// STATIC: 0x00F33CB0/0x140E26CA8
TSGrowableArray<SpellBookSkillInfo*> s_skillInfos;

// STATIC: 0x00F33CC0
TSFixedArray<int32_t> CGSpellBook::m_languageSpells;

// STATIC: 0x00F33CCC/0x140E26CCC
TSGrowableArray<int32_t> s_trackingSpells;

// STATIC: 0x00F33CDC/0x140E26CE4
TSGrowableArray<int32_t> CGSpellBook::m_companions[2];

// STATIC: 0x00F33CFC/0x140E26D10
TSGrowableArray<int32_t> s_openLockSpells;

// STATIC: 0x00F33D0C/0x140E26D28
TSGrowableArray<SpellUsableInfo> s_shapeshiftSpells;

// STATIC: 0x00F33D1C/0x140E26D40
static TSGrowableArray<int32_t> s_dispelTypes;

// STATIC: 0x00F33D2C/0x140E26D58
static TSGrowableArray<int32_t> s_summonCreatures;

// STATIC: 0x00F33D3C/0x140E26D70
static TSGrowableArray<SpellUsableInfo> s_knownSpells;

// STATIC: 0x00F33D4C/0x140BC76B0
static TSHashTable<SPELLSBYNAME, HASHKEY_UTF8I> s_spellsByName;

// STATIC: 0x00F33D78/0x140E26D88
TSGrowableArray<int32_t> CGSpellBook::m_unlearnedSpells;

// STATIC: 0x00F33D88/0x140E26DA0
TSGrowableArray<int32_t> CGSpellBook::m_totemSpells[4];

// STATIC: 0x00F33E38/0x140E26E00
static TSGrowableArray<SpellBookItem*> s_futureSpells;

// STATIC: 0x00D10ED0/0x140BC72D0
const char* s_companionTypes[2] = { "CRITTER", "MOUNT" };

// STATIC: 0x00D10ED8
static int32_t s_multiCastSpells[4] = { 66842, 66843, 66844, 36936 };

// STATIC: 0x00D10EE8
static uint32_t s_petSpellBookActions[8] = { 0x7000002, 0x7000001, 0x7000004, 0x7000000, 0x6000003, 0x6000001, 0x6000000, 0 };

// STATIC: 0x00D110D8/0x140BC77C0
TSList<PASSIVESPELLNODE> CGSpellBook::s_passiveSpells;

// STATIC: 0x00F33E00/0x140BC7760
TSHashTable<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I> CGActionBar::m_overrideSpellsByName;

// OFFSET: 0x008C13D0
TSGrowableArray<int32_t>* CGSpellBook::GetTotemSpells(uint32_t slot) {
    if (slot >= 4) {
        return NULL;
    }

    return &CGSpellBook::m_totemSpells[slot];
}

// OFFSET: 0x0090E1C0/0x140669490
int32_t CGSpellBook::GetCompanionType(const SpellRec* spell) {
    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect) {
        if (effect->m_effect == 28) {
            return 0;
        }

        if (effect->m_effectAura == 78) {
            return 1;
        }
    }

    return 2;
}

// OFFSET: 0x0090E200/0x1406694D0
void CGSpellBook::SendCompanionUpdate(int32_t type) {
    if (type == 2) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMPANION_UPDATE, NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMPANION_UPDATE, "%s", s_companionTypes[type]);
    }
}

// OFFSET: 0x0090E240/0x140669500
void CGSpellBook::UpdateSelection(uint8_t cancelled) {
    CGActionBar::UpdateSelection();
    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENT_SPELL_CAST_CHANGED, "%b", cancelled);
}

// OFFSET: 0x0090E270/0x140669530
int32_t CGSpellBook::GetSpellRankFromSubtext(const SpellRec* spell) {
    const char* subtext = spell->m_nameSubtext;
    uint32_t length = strlen(subtext);
    int32_t rank = 0;

    for (uint32_t i = 0; i < length; i++) {
        if (IsNumeric(static_cast<uint16_t>(subtext[i]))) {
            rank = subtext[i] + 10 * rank - '0';
        } else if (rank) {
            return rank;
        }
    }

    return rank;
}

// OFFSET: 0x0090E5F0
SpellBookSkillInfo* CGSpellBook::GetSkillInfo(uint32_t index) {
    if (!(index >= s_numSkillInfos)) {
        return s_skillInfos.m_data[index];
    }

    return NULL;
}

// OFFSET: 0x0090E610
uint32_t CGSpellBook::GetSkillInfoIndex(int32_t skillLine) {
    for (uint32_t index = 0; index < s_numSkillInfos; index++) {
        if (s_skillInfos.m_data[index]->m_skillLine == skillLine) {
            return index;
        }
    }

    return s_numSkillInfos;
}

// OFFSET: 0x0090E650/0x140669980
uint32_t CGSpellBook::GetSkillLineOffset(uint32_t index) {
    if (!index || index >= s_numSkillInfos) {
        return 0;
    }

    uint32_t offset = 0;

    for (int32_t i = index - 1; i >= 0; i--) {
        SpellBookSkillInfo* info = s_skillInfos.m_data[i];
        offset += info->m_count + info->m_numFuture + info->m_numFlyouts;
    }

    return offset;
}

// OFFSET: 0x0090E6D0/0x140669A00
int __cdecl SortSpellUsableInfo(const void* a, const void* b) {
    const SpellUsableInfo* infoA = static_cast<const SpellUsableInfo*>(a);
    const SpellUsableInfo* infoB = static_cast<const SpellUsableInfo*>(b);
    const SpellRec* spellA = g_spellDB.GetRecord(infoA->m_spellId);
    const SpellRec* spellB = g_spellDB.GetRecord(infoB->m_spellId);

    if (!spellA || !spellB) {
        return 0;
    }

    int32_t orderA = 0;
    const SpellShapeshiftRec* shapeshift;

    if (spellA->m_spellShapeshiftID && (shapeshift = g_spellShapeshiftDB.GetRecord(spellA->m_spellShapeshiftID))) {
        orderA = shapeshift->m_stanceBarOrder;
    }

    int32_t orderB = 0;

    if (spellB->m_spellShapeshiftID && (shapeshift = g_spellShapeshiftDB.GetRecord(spellB->m_spellShapeshiftID))) {
        orderB = shapeshift->m_stanceBarOrder;
    }

    if (orderA < 0 && orderB < 0) {
        return 2 * (spellA->m_id >= spellB->m_id) - 1;
    }

    if (orderA == -1) {
        return 1;
    }

    if (orderB == -1) {
        return -1;
    }

    if (orderA != orderB) {
        return 2 * (orderA >= orderB) - 1;
    }

    return 0;
}

// OFFSET: 0x0090E7E0/0x140669B50
int __cdecl SortCompanions(const void* a, const void* b) {
    CreatureStats_C* statsA = NULL;
    CreatureStats_C* statsB = NULL;
    const SpellEffectRec* effect = SpellGetEffect(*static_cast<const int32_t*>(a), 0);

    if (effect) {
        statsA = g_creatureDBCache.GetRecord(effect->m_effectMiscValue, WOWGUID(), NULL, NULL, false);
    }

    effect = SpellGetEffect(*static_cast<const int32_t*>(b), 0);

    if (effect) {
        statsB = g_creatureDBCache.GetRecord(effect->m_effectMiscValue, WOWGUID(), NULL, NULL, false);
    }

    if (statsA && statsB) {
        return SStrSortUTF8I(statsA->m_name[0], statsB->m_name[0], STORM_MAX_STR);
    }

    return statsB - statsA;
}

// OFFSET: 0x0090E890/0x140669C30
int32_t GetActionBarPageOffset(const CGPlayer_C* player, const SpellRec* spell) {
    uint8_t classId = player->m_unit->m_unitFieldBytes0[1];

    switch (classId) {
        case 1:
        case 4:
        case 11:
            break;

        default:
            return 0;
    }

    const SpellShapeshiftFormRec* form;
    const SpellShapeshiftRec* shapeshift;
    uint32_t mask[2];

    if (spell->m_spellShapeshiftID && (shapeshift = g_spellShapeshiftDB.GetRecord(spell->m_spellShapeshiftID)) && ((mask[0] = shapeshift->m_shapeshiftMask[0]), (mask[1] = shapeshift->m_shapeshiftMask[1]), mask[0] || mask[1]) && !(spell->m_attributesExB & 0x80000)) {
        int32_t bits;
        bool high;

        if (mask[0]) {
            bits = mask[0];
            high = false;
        } else {
            bits = mask[1];
            high = true;
        }

        int32_t formId;

        for (formId = 1; bits; formId++) {
            if (bits & 1) {
                break;
            }

            bits >>= 1;
        }

        if (high) {
            formId += 32;
        }

        form = g_spellShapeshiftFormDB.GetRecord(formId);
    } else {
        if (classId != 1) {
            return 0;
        }

        form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm());
    }

    if (form) {
        return 12 * (form->m_bonusActionBar + 5);
    }

    return 0;
}

// OFFSET: 0x0090E990/0x140669D50
int __cdecl SortSkillInfoByName(const void* a, const void* b) {
    const SpellBookSkillInfo* infoA = *static_cast<SpellBookSkillInfo* const*>(a);
    const SpellBookSkillInfo* infoB = *static_cast<SpellBookSkillInfo* const*>(b);

    if (!infoA->m_skillLine) {
        return -1;
    }

    if (!infoB->m_skillLine) {
        return 1;
    }

    if (infoA->m_isProfession != infoB->m_isProfession) {
        return 2 * (infoA->m_isProfession != 0) - 1;
    }

    if (infoA->m_isGuild != infoB->m_isGuild) {
        return 2 * (infoA->m_isGuild != 0) - 1;
    }

    const SkillLineRec* skillA = g_skillLineDB.GetRecord(infoA->m_skillLine);
    const SkillLineRec* skillB = g_skillLineDB.GetRecord(infoB->m_skillLine);

    if (skillA && skillB) {
        return SStrSortUTF8I(skillA->m_displayName, skillB->m_displayName, STORM_MAX_STR);
    }

    return 0;
}

// OFFSET: 0x0090EA30/0x140669E40
void CGSpellBook::UpdateSilenceAndInterruptsForSpell(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        if (effect->m_effect == 6) {
            if (effect->m_effectAura == 27) {
                CGSpellBook::m_silenceSchoolMask |= spell->m_schoolMask;
            }
        } else if (effect->m_effect == 68) {
            CGSpellBook::m_interruptSchoolMask |= spell->m_schoolMask;
        }
    }

    if (spell->m_attributesExG & 0x800) {
        CGSpellBook::m_interruptSpecialSchoolMask |= spell->m_schoolMask;
    }

    if (spell->m_attributesExG & 0x1000) {
        CGSpellBook::m_silenceSpecialSchoolMask |= spell->m_schoolMask;
    }
}

// OFFSET: 0x0090EAC0/0x140669EE0
void CGSpellBook::UpdateCompanions() {
    if (s_pendingCompanions) {
        return;
    }

    bool sorted = false;

    for (uint32_t i = 0; i < 2; i++) {
        if (CGSpellBook::m_companions[i].m_count > 0) {
            qsort(CGSpellBook::m_companions[i].m_data, CGSpellBook::m_companions[i].m_count, sizeof(int32_t), SortCompanions);
            sorted = true;
        }
    }

    if (sorted) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMPANION_UPDATE, NULL);
    }
}

// OFFSET: 0x0090EB10/0x140669F50
void CGSpellBook::UpdateCooldowns() {
    CGActionBar::UpdateCooldowns();

    if (s_shapeshiftSpells.m_count > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_COOLDOWN, NULL);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_UPDATE_COOLDOWN, NULL);
}

// OFFSET: 0x0090EB40/0x14066EE10
static void PlaySpellDropSound() {
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("igSpellBookSpellIconDrop", NULL, &properties, NULL, __FILE__, __LINE__, "PlaySpellDropSound");
}

// OFFSET: 0x0090EBA0/0x140669F80
static void PlaySpellPickupSound() {
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("igSpellBookSpellIconPickup", NULL, &properties, NULL, __FILE__, __LINE__, "PlaySpellPickupSound");
}

// OFFSET: 0x0090EC00/0x140669FF0
SpellBookItem* CGSpellBook::GetSpellBookItem(int32_t slot, int32_t petSpell) {
    if (slot >= 0) {
        if (petSpell) {
            if (slot < static_cast<int32_t>(s_petSpellBookItems.m_count)) {
                return s_petSpellBookItems.m_data[slot];
            }
        } else if (slot < static_cast<int32_t>(s_spellBookItems.m_count)) {
            return s_spellBookItems.m_data[slot];
        }
    }

    return NULL;
}

// OFFSET: 0x0090EC40/0x14066A030
int32_t CGSpellBook::GetSkinningSpell(const CGObject_C* object) {
    uint32_t type = object->m_obj->m_type;

    if (type & 0x90) {
        return s_skinPlayerCorpseSpell;
    }

    if (type & 0x8) {
        return s_skinningSpells[static_cast<const CGUnit_C*>(object)->GetSkinningType()];
    }

    return 0;
}

// OFFSET: 0x0090EC80/0x14066A080
int32_t CGSpellBook::IsTrackingSpell(int32_t spellId) {
    for (uint32_t i = s_trackingSpells.m_count; i; ) {
        if (spellId == s_trackingSpells.m_data[--i]) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0090ECB0/0x14066A0C0
int32_t CGSpellBook::IsShapeshiftSpell(int32_t spellId) {
    for (uint32_t i = s_shapeshiftSpells.m_count; i; ) {
        if (spellId == s_shapeshiftSpells.m_data[--i].m_spellId) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0090ECE0/0x14066A100
int32_t CGSpellBook::IsAuraCaster(WOWGUID guid, const WOWGUID* caster) {
    if (!guid || !caster || !*caster) {
        return 0;
    }

    if (guid == *caster) {
        return 1;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
    return guid == pet || guid == *unit->GetCharmerOrCreatorGuid();
}

// OFFSET: 0x0090ED60/0x14066A190
int32_t CGSpellBook::FindRaidDispel(int32_t dispelType) {
    for (uint32_t i = 0; i < s_dispelTypes.m_count; i++) {
        if (s_dispelTypes.m_data[i] == dispelType) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0090EDA0/0x14066A1C0
int32_t CGSpellBook::FindSlotBySpellID(int32_t spellId, int32_t petSpell) {
    if (!spellId) {
        return -1;
    }

    TSGrowableArray<SpellBookItem*>& items = petSpell ? s_petSpellBookItems : s_spellBookItems;

    for (int32_t slot = items.m_count - 1; slot >= 0; slot--) {
        SpellBookItem* item = items.m_data[slot];

        if (spellId == item->m_id && item->m_type == SPELLBOOK_ITEM_SPELL) {
            return slot;
        }
    }

    return -1;
}

// OFFSET: 0x0090EE10/0x140669530
int32_t CGSpellBook::GetSpellProgressionRank(const SpellRec* spell) {
    int32_t rank = CGSpellBook::GetSpellRankFromSubtext(spell);

    if (!rank && spell->m_spellLevelsID) {
        const SpellLevelsRec* levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID);

        if (levels) {
            return levels->m_spellLevel;
        }
    }

    return rank;
}

// OFFSET: 0x0090EE60/0x14066A290
void CGSpellBook::GetAppropriateSpellRank(int32_t& spellId, const CGUnit_C* caster, const CGUnit_C* target) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || (spell->m_attributes & 0x4000000) || (spell->m_attributesExB & 0x8)) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    uint32_t i;

    for (i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effectAura) {
            break;
        }
    }

    if (i >= count || !caster) {
        return;
    }

    if (!const_cast<CGUnit_C*>(caster)->IsActivePlayer() && caster->m_obj->m_guid != CGPetInfo::GetPet(0)) {
        return;
    }

    if (!target || !caster->CanAssist(target, false)) {
        return;
    }

    int32_t spellLevel = 0;
    const SpellLevelsRec* levels;

    if (spell->m_spellLevelsID && (levels = g_spellLevelsDB.GetRecord(spell->m_spellLevelsID))) {
        spellLevel = levels->m_spellLevel;
    }

    uint32_t targetLevel = target->m_unit->m_unitFieldLevel;

    if (target->HasSummoner()) {
        CGUnit_C* summoner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target->m_unit->m_unitFieldSummonedby, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (summoner) {
            targetLevel = summoner->m_unit->m_unitFieldLevel;
        }
    }

    int32_t maxLevel = targetLevel + 10;

    if (spellLevel <= maxLevel) {
        return;
    }

    int32_t bestLevel = 0;
    TSGrowableArray<SpellBookItem*>& items = const_cast<CGUnit_C*>(caster)->IsActivePlayer() ? s_spellBookItems : s_petSpellBookItems;

    for (int32_t slot = items.m_count - 1; slot >= 0; slot--) {
        SpellBookItem* item = items.m_data[slot];

        if (item->m_type != SPELLBOOK_ITEM_SPELL) {
            continue;
        }

        const SpellRec* rank = g_spellDB.GetRecord(item->m_id);

        if (!rank) {
            continue;
        }

        if (rank->m_spellLevelsID && (levels = g_spellLevelsDB.GetRecord(rank->m_spellLevelsID))) {
            spellLevel = levels->m_spellLevel;
        }

        if (!strcmp(spell->m_name, rank->m_name) && spellLevel <= maxLevel && spellLevel > bestLevel) {
            spellId = rank->m_id;
            bestLevel = spellLevel;
        }
    }
}

// OFFSET: 0x0090F140/0x14066A680
bool CGSpellBook::IsUnlearnedSpell(int32_t spellId) {
    for (uint32_t i = 0; i < CGSpellBook::m_unlearnedSpells.m_count; i++) {
        if (CGSpellBook::m_unlearnedSpells.m_data[i] == spellId) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0090F920/0x14066AF50
static void PreloadSpellVisualEffectName(int32_t id, int32_t wait) {
    const SpellVisualEffectNameRec* effect;

    if (id && (effect = g_spellVisualEffectNameDB.GetRecord(id)) && *effect->m_fileName) {
        World::GetScene()->PreloadModel(effect->m_fileName, 5, wait);
    }
}

// OFFSET: 0x0090F960/0x14066AFD0
static void PreloadSpellChainEffects(int32_t id, int32_t wait) {
    const SpellChainEffectsRec* chain = g_spellChainEffectsDB.GetRecord(id);

    if (!chain) {
        return;
    }

    const SpellChainEffectsRec* current = chain;

    for (uint32_t i = 0; i < 12; i++) {
        if (!current->m_texture) {
            continue;
        }

        CAsyncObject* object = NULL;

        if (!AsyncFileReadCreateObject(current->m_texture, 5, 1, 0, &object, 0, 0, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0)) {
            continue;
        }

        if (object && wait) {
            AsyncFileReadWait(object);
        }

        int32_t next = CLightning::DecodeChainEffectsID(reinterpret_cast<const uint32_t*>(static_cast<const char*>(chain->m_combo))[i]);

        if (!next || !(current = g_spellChainEffectsDB.GetRecord(next))) {
            break;
        }
    }
}

// OFFSET: 0x0090FA40/0x14066B150
static void PreloadSpellVisualKit(int32_t id, int32_t wait) {
    const SpellVisualKitRec* kit;

    if (!id || !(kit = g_spellVisualKitDB.GetRecord(id))) {
        return;
    }

    PreloadSpellVisualEffectName(kit->m_headEffect, wait);
    PreloadSpellVisualEffectName(kit->m_chestEffect, wait);
    PreloadSpellVisualEffectName(kit->m_baseEffect, wait);
    PreloadSpellVisualEffectName(kit->m_leftHandEffect, wait);
    PreloadSpellVisualEffectName(kit->m_rightHandEffect, wait);
    PreloadSpellVisualEffectName(kit->m_breathEffect, wait);
    PreloadSpellVisualEffectName(kit->m_leftWeaponEffect, wait);
    PreloadSpellVisualEffectName(kit->m_rightWeaponEffect, wait);

    for (uint32_t i = 0; i < 3; i++) {
        PreloadSpellVisualEffectName(kit->m_specialEffect[i], wait);
    }

    PreloadSpellVisualEffectName(kit->m_worldEffect, wait);

    for (uint32_t i = 0; i < 4; i++) {
        int32_t proc = kit->m_charProc[i];

        if (proc == 9) {
            const SpellVisualKitAreaModelRec* model = g_spellVisualKitAreaModelDB.GetRecord(static_cast<int32_t>(kit->m_charParamZero[i]));
            World::GetScene()->PreloadModel(model ? model->m_name : "Spells\\Blizzard_Impact_Base.mdx", 5, wait);
        } else if (!proc || proc == 12) {
            PreloadSpellChainEffects(static_cast<int32_t>(kit->m_charParamZero[i]), wait);
        }
    }

    SE3::PreloadSoundKit(kit->m_soundID, 0);
}

// OFFSET: 0x0090FBF0/0x14066B300
void PrefetchSpellVisuals(int32_t spellId, int32_t wait) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    for (uint32_t i = 0; i < 2; i++) {
        const SpellVisualRec* visual;

        if (!spell->m_spellVisualID[i] || !(visual = g_spellVisualDB.GetRecord(spell->m_spellVisualID[i]))) {
            continue;
        }

        if (visual->m_hasMissile) {
            PreloadSpellVisualEffectName(visual->m_missileModel, wait);
        }

        PreloadSpellVisualKit(visual->m_castKit, wait);
        PreloadSpellVisualKit(visual->m_stateKit, wait);
        PreloadSpellVisualKit(visual->m_channelKit, wait);
        PreloadSpellVisualKit(visual->m_missileTargetingKit, wait);
        PreloadSpellVisualKit(visual->m_instantAreaKit, wait);
        PreloadSpellVisualKit(visual->m_persistentAreaKit, wait);
        PreloadSpellVisualKit(visual->m_impactKit, wait);
        PreloadSpellVisualKit(visual->m_casterImpactKit, wait);
        PreloadSpellVisualKit(visual->m_targetImpactKit, wait);
        PreloadSpellVisualKit(visual->m_impactAreaKit, wait);
        PreloadSpellVisualKit(visual->m_precastKit, wait);
        PreloadSpellVisualKit(visual->m_stateDoneKit, wait);
    }
}

// OFFSET: 0x0090FD50/0x14066B560
int32_t CGSpellBook::GetRealSpellBookSkillLine(int32_t spellId, bool* isProfession) {
    int32_t skillLine = 0;
    bool profession = false;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        SkillLineAbilityRec* ability = player->LookupAbility(spellId);
        const SkillLineRec* skill = NULL;

        if (ability) {
            if (ability->m_skillLine == 810) {
                return 810;
            }

            const SkillRaceClassInfoRec* info = SpellTableLookupInfo(player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[1], ability->m_skillLine);

            if (info) {
                if (static_cast<int8_t>(info->m_flags) >= 0) {
                    skillLine = info->m_skillID;
                } else if (info->m_skillTierID == 23 || info->m_skillTierID == 61 || info->m_skillTierID == 63 || info->m_skillTierID == 224) {
                    profession = true;
                    skillLine = info->m_skillID;
                }
            }

            skill = g_skillLineDB.GetRecord(ability->m_skillLine);
        }

        if (skill && skill->m_categoryID == 11) {
            skillLine = ability->m_skillLine;
            profession = true;
        }
    }

    if (isProfession) {
        *isProfession = profession;
    }

    return skillLine;
}

// OFFSET: 0x0090FE40
uint32_t CGSpellBook::GetSpellSkillInfoIndex(int32_t spellId) {
    return CGSpellBook::GetSkillInfoIndex(CGSpellBook::GetRealSpellBookSkillLine(spellId, NULL));
}

// OFFSET: 0x0090FE80/0x14066B680
void CGSpellBook::CompanionCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    const SpellRec* spell = NULL;

    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if ((high & 0xF0000000) == 0x10000000 && (high & 0xFF00000) == 0xFE00000) {
        spell = g_spellDB.GetRecord(static_cast<int32_t>(guid.m_guid));
    }

    if (s_pendingCompanions <= 0) {
        return;
    }

    s_pendingCompanions--;

    if (param && success) {
        CreatureStats_C* stats = g_creatureDBCache.GetRecord(id, WOWGUID(), NULL, NULL, false);

        if (spell && CGSpellBook::GetCompanionType(spell) == 1) {
            CGGameUI::DisplayError(ERR_LEARN_MOUNT_S, stats->m_name[0]);
        } else {
            CGGameUI::DisplayError(ERR_LEARN_COMPANION_S, stats->m_name[0]);
        }

        CGSpellBook::UpdateCompanions();
        FrameScript_SignalEvent(SCRIPT_EVENT_COMPANION_LEARNED, NULL);
    } else if (!s_pendingCompanions) {
        CGSpellBook::UpdateCompanions();
    }
}

// OFFSET: 0x0090FF70/0x14066B790
static int __cdecl SortTrackingSpells(const void* a, const void* b) {
    const SpellRec* spellA = g_spellDB.GetRecord(*static_cast<const int32_t*>(a));
    const SpellRec* spellB = g_spellDB.GetRecord(*static_cast<const int32_t*>(b));

    if (!spellA || !spellB) {
        return 0;
    }

    uint32_t indexA = CGSpellBook::GetSpellSkillInfoIndex(spellA->m_id);
    uint32_t indexB = CGSpellBook::GetSpellSkillInfoIndex(spellB->m_id);

    if (indexA != indexB) {
        return indexA < indexB ? -1 : 1;
    }

    SpellBookSkillInfo* info;

    if (indexA < s_numSkillInfos && (info = s_skillInfos.m_data[indexA]) && info->m_isProfession) {
        const SpellEffectRec* effectA = SpellGetEffect(spellA->m_id, 0);
        const SpellEffectRec* effectB = SpellGetEffect(spellB->m_id, 0);

        if (effectA && effectA->m_effect == 47 && (!effectB || effectB->m_effect != 47)) {
            return -1;
        }

        if (effectB && effectB->m_effect == 47 && (!effectA || effectA->m_effect != 47)) {
            return 1;
        }
    }

    int32_t result = SStrSortUTF8I(spellA->m_name, spellB->m_name, STORM_MAX_STR);

    if (!result) {
        return SStrSortUTF8I(spellA->m_nameSubtext, spellB->m_nameSubtext, STORM_MAX_STR);
    }

    return result;
}

// OFFSET: 0x009100A0/0x14066B980
void CGSpellBook::UpdateSilenceAndInterrupts() {
    CGSpellBook::m_interruptSchoolMask = 0;
    CGSpellBook::m_interruptSpecialSchoolMask = 0;
    CGSpellBook::m_silenceSchoolMask = 0;
    CGSpellBook::m_silenceSpecialSchoolMask = 0;

    for (uint32_t i = 0; i < s_spellBookItems.m_count; i++) {
        SpellBookItem* item = s_spellBookItems.m_data[i];
        const SpellRec* spell;

        if (item->m_type == SPELLBOOK_ITEM_SPELL && (spell = g_spellDB.GetRecord(item->m_id))) {
            CGSpellBook::UpdateSilenceAndInterruptsForSpell(spell);
        }
    }

    for (uint32_t i = 0; i < s_petSpellBookItems.m_count; i++) {
        SpellBookItem* item = s_petSpellBookItems.m_data[i];
        const SpellRec* spell;

        if (item->m_type == SPELLBOOK_ITEM_SPELL && (spell = g_spellDB.GetRecord(item->m_id))) {
            CGSpellBook::UpdateSilenceAndInterruptsForSpell(spell);
        }
    }
}

// OFFSET: 0x00910170/0x14066BA90
void CGSpellBook::UpdateUsable() {
    CGActionBar::UpdateUsable();
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        for (uint32_t i = 0; i < s_knownSpells.m_count; i++) {
            SpellUsableInfo& info = s_knownSpells.m_data[i];
            const SpellRec* spell = g_spellDB.GetRecord(info.m_spellId);
            bool usable = Spell_C_IsUsableAction(player, spell, WOWGUID(), NULL, false, false, false);
            int32_t powerType;
            int32_t requirement;
            bool power = Spell_C_HaveSpellPower(player, spell, powerType, requirement);
            bool noMana = !power;

            if ((usable && power) != (info.m_usable && !info.m_noMana) && usable && !noMana) {
                SendToCombatText(35, spell->m_name);
            }

            info.m_usable = usable;
            info.m_noMana = noMana;
        }

        bool changed = false;

        for (uint32_t i = 0; i < s_shapeshiftSpells.m_count; i++) {
            SpellUsableInfo& info = s_shapeshiftSpells.m_data[i];
            const SpellRec* spell = g_spellDB.GetRecord(info.m_spellId);
            bool usable = Spell_C_IsUsableAction(player, spell, WOWGUID(), NULL, false, false, false);
            int32_t powerType;
            int32_t requirement;
            bool power = Spell_C_HaveSpellPower(player, spell, powerType, requirement);

            if ((usable && power) != (info.m_usable && !info.m_noMana)) {
                changed = true;
            }

            info.m_usable = usable;
            info.m_noMana = !power;
        }

        if (changed) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_USABLE, NULL);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_UPDATE_USABLE, NULL);
}

// OFFSET: 0x00910370/0x14066BD40
void CGSpellBook::UpdateUsablePower() {
    CGActionBar::UpdateUsablePower();
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < s_knownSpells.m_count; i++) {
        SpellUsableInfo& info = s_knownSpells.m_data[i];

        if (!info.m_usable) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(info.m_spellId);
        int32_t powerType;
        int32_t requirement;
        bool power = Spell_C_HaveSpellPower(player, spell, powerType, requirement);

        if (!power != (info.m_noMana != 0)) {
            if (power) {
                SendToCombatText(35, spell->m_name);
            }

            info.m_noMana = !power;
        }
    }

    bool changed = false;

    for (uint32_t i = 0; i < s_shapeshiftSpells.m_count; i++) {
        SpellUsableInfo& info = s_shapeshiftSpells.m_data[i];

        if (!info.m_usable) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(info.m_spellId);
        int32_t powerType;
        int32_t requirement;
        uint8_t noMana = !Spell_C_HaveSpellPower(player, spell, powerType, requirement);

        if (noMana != info.m_noMana) {
            changed = true;
            info.m_noMana = noMana;
        }
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_SHAPESHIFT_USABLE, NULL);
    }
}

// OFFSET: 0x009104C0/0x14066BEF0
int32_t CGSpellBook::IsSelectedSlot(int32_t slot, int32_t petSpell) {
    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

    if (!item) {
        return 0;
    }

    if (item->m_type != SPELLBOOK_ITEM_SPELL) {
        if (item->m_type != SPELLBOOK_ITEM_PETACTION) {
            return 0;
        }

        PetAction action;
        action.m_data = item->m_id;

        if ((action.m_data & 0x3F000000) == 0x7000000 && (action.m_data & 0xFFFFFF) == 2) {
            return 0;
        }

        return CGPetInfo::PetActionIsActive(action);
    }

    const SpellRec* spell = g_spellDB.GetRecord(item->m_id);

    if (!spell) {
        return 0;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && count && effects->m_data[0] && effects->m_data[0]->m_effect == 47) {
        SkillLineAbilityRec* ability = player->LookupAbility(spell->m_id);

        if (ability && ((!CGTradeSkillInfo::m_linkData && CGTradeSkillInfo::m_skillLineID == ability->m_skillLine) || ability->m_skillLine == CGResearchFrame::m_skillLine)) {
            return 1;
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effectAura == 36) {
            uint32_t form = effects->m_data[i]->m_effectMiscValue;
            return form && player && player->GetShapeshiftForm() == form;
        }
    }

    return 0;
}

// OFFSET: 0x00910630/0x14066C0F0
uint32_t CGSpellBook::GetShapeshiftForm(int32_t requireForm) {
    uint32_t index = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (requireForm && !player->GetShapeshiftForm())) {
        return index;
    }

    for (index = s_shapeshiftSpells.m_count; index; index--) {
        const SpellRec* spell = g_spellDB.GetRecord(s_shapeshiftSpells.m_data[index - 1].m_spellId);

        if (spell && Spell_C_IsActiveSpell(spell, player)) {
            break;
        }
    }

    if (!index && player->GetShapeshiftForm()) {
        return s_shapeshiftSpells.m_count + 1;
    }

    return index;
}

// OFFSET: 0x009106E0/0x14066C1F0
int32_t CGSpellBook::CanShowAura(const SpellRec* spell, const WOWGUID* caster, bool helpful) {
    if ((spell->m_attributes & 0x80) || (spell->m_attributesEx & 0x10000000)) {
        return 0;
    }

    if (spell->m_attributes & 0x40) {
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
        uint32_t i;

        for (i = 0; i < count; i++) {
            if (effects->m_data[i] && SpellIsAreaAuraEffect(effects->m_data[i]->m_effect)) {
                break;
            }
        }

        if (i >= count) {
            return 0;
        }
    }

    if ((helpful ? (spell->m_attributesExE & 0x8000000) : (spell->m_attributesExE & 0x10000000)) || ((spell->m_attributesExF & 0x100000) && !(*caster && (*caster == ClntObjMgrGetActivePlayer() || *caster == CGPetInfo::GetPet(0))))) {
        return 0;
    }

    const SpellCastingRequirementsRec* requirements;

    if (!spell->m_spellCastingRequirementsID || !(requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID)) || !requirements->m_requiredAuraVision) {
        return 1;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    return player && player->HasAuraVision(requirements->m_requiredAuraVision);
}

// OFFSET: 0x00910810/0x14066C350
int32_t CGSpellBook::CanStealAura(const CGUnit_C* unit, const AuraData* aura, const SpellRec* spell) {
    if (!s_spellStealMask && !s_dispelMask) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->CanAttack(unit, false) || static_cast<int8_t>(aura->m_flags) < 0 || (spell->m_attributes & 0x40)) {
        return 0;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && ((1 << i) & aura->m_flags) && SpellIsAreaAuraEffect(effects->m_data[i]->m_effect)) {
            return 0;
        }
    }

    if (spell->m_attributesEx & 0x44) {
        return 0;
    }

    uint8_t dispelType = 0;
    const SpellCategoriesRec* categories;

    if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
        dispelType = categories->m_dispelType;
    }

    return ((1 << dispelType) & s_dispelMask) || (((1 << dispelType) & s_spellStealMask) && !(spell->m_attributesExD & 0x40));
}

// OFFSET: 0x00910930/0x14066C4B0
int32_t CGSpellBook::CanApplyAura(int32_t spellId, const WOWGUID* target) {
    if (HasSpell(spellId) || HasPetSpell(spellId)) {
        const SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (!spell) {
            return 0;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t i = 0; i < count; i++) {
            if (effects->m_data[i] && SpellIsAuraEffect(effects->m_data[i]->m_effect)) {
                return 1;
            }
        }
    }

    int32_t low = 0;
    int32_t high = g_castableRaidBuffsDB.m_numRecords - 1;

    while (low <= high) {
        int32_t middle = (high + low) >> 1;
        const CastableRaidBuffsRec* buff = g_castableRaidBuffsDB.GetRecordByIndex(middle);

        if (spellId == buff->m_spellID) {
            int32_t castingSpell = buff->m_castingSpellID;

            if (HasSpell(castingSpell) || HasPetSpell(castingSpell)) {
                return 1;
            }

            break;
        }

        if (spellId <= buff->m_spellID) {
            high = middle - 1;
        } else {
            low = middle + 1;
        }
    }

    if (!*target) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->m_creatureCacheEntry) {
        return 0;
    }

    const WOWGUID& owner = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldCreatedby;

    if (!owner) {
        return 0;
    }

    int32_t entry = unit->m_obj->m_entryID;

    for (uint32_t i = 0; i < s_summonCreatures.m_count; i++) {
        if (entry == s_summonCreatures.m_data[i]) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00910AE0/0x14066C710
int32_t CGSpellBook::FindRaidBuff(int32_t spellId) {
    const SpellRec* spell;

    if (!(HasSpell(spellId) || HasPetSpell(spellId)) || !(spell = g_spellDB.GetRecord(spellId))) {
        return 0;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    if (spell->m_attributesExC & 0x4000 || (count && effects->m_data[0] && effects->m_data[0]->m_implicitTargetA == 1)) {
        return 0;
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        switch (effect->m_effect) {
            case 27:
            case 35:
            case 65:
            case 119:
            case 128:
            case 129:
            case 143:
            case 174:
                return 0;

            default:
                break;
        }
    }

    return 1;
}

// OFFSET: 0x00910BE0/0x14066C860
int32_t CGSpellBook::FindCompanionSlotByID(int32_t spellId, int32_t type) {
    if (!spellId) {
        return -1;
    }

    for (int32_t slot = CGSpellBook::m_companions[type].m_count; slot; ) {
        if (spellId == CGSpellBook::m_companions[type].m_data[--slot]) {
            return slot;
        }
    }

    return -1;
}

// OFFSET: 0x00911E80/0x14066DE70
void CGSpellBook::MakeKnownSpellModelsLocal() {
    for (uint32_t i = 0; i < s_spellBookItems.m_count; i++) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (item->m_type == SPELLBOOK_ITEM_SPELL) {
            PrefetchSpellVisuals(item->m_id, 0);
        }
    }
}

// OFFSET: 0x00911F40/0x14066DFA0
void CGSpellBook::ClearPetSpells() {
    for (int32_t i = s_petSpellBookItems.m_count - 1; i >= 0; i--) {
        SpellBookItem* item = s_petSpellBookItems.m_data[i];

        if (item) {
            delete item;
        }
    }

    s_petSpellBookItems.m_count = 0;
}

// OFFSET: 0x00911F80
void CGSpellBook::AddPetActions() {
    for (uint32_t i = 0; i < 7; i++) {
        SpellBookItem* item = static_cast<SpellBookItem*>(SMemAlloc(sizeof(SpellBookItem), __FILE__, __LINE__, 0));
        s_petSpellBookItems.Add(1, &item);
        item->m_type = SPELLBOOK_ITEM_PETACTION;
        item->m_id = s_petSpellBookActions[i];
        item->m_level = 0;
        item->m_skillLine = 0;
        item->m_inFlyout = 0;
    }
}

// OFFSET: 0x00911FE0/0x14066E150
uint32_t CGSpellBook::AddSkillInfo(int32_t skillLine, bool isProfession) {
    uint32_t index;

    for (index = 0; index < s_numSkillInfos; index++) {
        if (s_skillInfos.m_data[index]->m_skillLine == skillLine) {
            return index;
        }
    }

    if (s_numSkillInfos == s_skillInfos.m_count) {
        s_skillInfos.SetCount(s_numSkillInfos + 1);
        s_skillInfos.m_data[s_numSkillInfos] = static_cast<SpellBookSkillInfo*>(SMemAlloc(sizeof(SpellBookSkillInfo), __FILE__, __LINE__, 0));
    }

    SpellBookSkillInfo* info = s_skillInfos.m_data[s_numSkillInfos];
    info->m_skillLine = skillLine;
    info->m_count = 0;
    info->m_numVisible = 0;
    info->m_numFuture = 0;
    info->m_numFlyouts = 0;
    info->m_isProfession = isProfession;
    info->m_isGuild = skillLine == 821 || skillLine == 816;
    s_numSkillInfos++;
    return s_numSkillInfos - 1;
}

// OFFSET: 0x00912130/0x14066E340
void CGSpellBook::InitFutureSpells() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    uint8_t race = player->m_unit->m_unitFieldBytes0[0];
    uint8_t classId = player->m_unit->m_unitFieldBytes0[1];

    for (int32_t i = s_futureSpells.m_count - 1; i >= 0; i--) {
        if (s_futureSpells.m_data[i]) {
            delete s_futureSpells.m_data[i];
        }
    }

    s_futureSpells.m_count = 0;

    for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
        const SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

        if (!SpellTableLookupInfo(race, classId, ability->m_skillLine) || !SpellTableMatchRaceClass(race, classId, ability->m_raceMask, ability->m_classMask, ability->m_excludeRace, ability->m_excludeClass)) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(ability->m_spell);

        if (!spell) {
            continue;
        }

        bool isProfession = false;
        int32_t skillLine = CGSpellBook::GetRealSpellBookSkillLine(ability->m_spell, &isProfession);

        if (isProfession || skillLine == 821 || skillLine == 816) {
            continue;
        }

        bool talent = false;

        for (int32_t tab = CGTalentInfo::GetNumTabs(0) - 1; tab >= 0 && !talent; tab--) {
            TalentTabInfo* info = CGTalentInfo::GetTab(tab, 0);

            if (!info) {
                continue;
            }

            for (uint32_t j = 0; j < info->m_numTalents && !talent; j++) {
                for (uint32_t k = 0; k < 5; k++) {
                    if (info->m_talents[j].m_spellRank[k] == ability->m_spell) {
                        talent = true;
                        break;
                    }
                }
            }
        }

        if (talent) {
            continue;
        }

        int32_t level;

        if (SpellIsCallCompanion(spell) || !player->CanLearnSpell(ability->m_spell, level, true) || level <= 0 || (spell->m_attributesExD & 0x8000) || (spell->m_attributes & 0xA0) || (spell->m_attributesExH & 0x2000)) {
            continue;
        }

        SpellBookItem* item = static_cast<SpellBookItem*>(SMemAlloc(sizeof(SpellBookItem), __FILE__, __LINE__, 0));
        item->m_type = SPELLBOOK_ITEM_FUTURESPELL;
        item->m_id = ability->m_spell;
        item->m_level = level;
        item->m_skillLine = skillLine;
        item->m_inFlyout = 0;
        s_futureSpells.Add(1, &item);
    }
}

// OFFSET: 0x009123C0/0x14066E650
void CGSpellBook::UpdateFutureSpells() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (int32_t i = s_spellBookItems.m_count - 1; i >= 0; i--) {
        if (s_spellBookItems.m_data[i]->m_type == SPELLBOOK_ITEM_FUTURESPELL) {
            if (i != static_cast<int32_t>(s_spellBookItems.m_count) - 1) {
                s_spellBookItems.m_data[i] = s_spellBookItems.m_data[s_spellBookItems.m_count - 1];
            }

            s_spellBookItems.m_count--;
        }
    }

    for (uint32_t i = 0; i < s_numSkillInfos; i++) {
        s_skillInfos.m_data[i]->m_numFuture = 0;
    }

    for (uint32_t i = 0; i < s_futureSpells.m_count; i++) {
        int32_t spellId = s_futureSpells.m_data[i]->m_id;

        if (spellId && CGSpellBook::FindSlotBySpellID(spellId, 0) >= 0) {
            continue;
        }

        if (player->IsSpellSuperceded(spellId)) {
            continue;
        }

        s_spellBookItems.Add(1, &s_futureSpells.m_data[i]);
        uint32_t skillIndex = CGSpellBook::AddSkillInfo(s_futureSpells.m_data[i]->m_skillLine, false);
        s_skillInfos.m_data[skillIndex]->m_numFuture++;
    }
}

// OFFSET: 0x00912520/0x14066E7F0
int32_t CGSpellBook::CanApplyAura(const AuraData* aura) {
    if (!aura) {
        return 0;
    }

    return CGSpellBook::CanApplyAura(aura->m_spellId, &aura->m_creator);
}

// OFFSET: 0x009126C0/0x14066EA70
void CGSpellBook::AddFlyout(int32_t flyoutId, int32_t spellId) {
    SpellBookItem* item = static_cast<SpellBookItem*>(SMemAlloc(sizeof(SpellBookItem), __FILE__, __LINE__, 0));
    s_spellBookItems.Add(1, &item);
    item->m_type = SPELLBOOK_ITEM_FLYOUT;
    item->m_id = flyoutId;
    item->m_level = 0;
    bool isProfession = false;
    item->m_skillLine = CGSpellBook::GetRealSpellBookSkillLine(spellId, &isProfession);
    item->m_inFlyout = 0;
    uint32_t skillIndex = CGSpellBook::AddSkillInfo(item->m_skillLine, isProfession);
    s_skillInfos.m_data[skillIndex]->m_numFlyouts++;
}

// OFFSET: 0x00912740/0x14066EBB0
int32_t CGSpellBook::GetOverrideSpell(int32_t spellId) {
    HASHKEY_NONE key;
    OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

    if (node && (node->m_flags & 0x4)) {
        return node->m_overrideSpellID;
    }

    return spellId;
}

// OFFSET: 0x00912770/0x14066EC10
void CGSpellBook::SetCursorSpell(int32_t spellId) {
    HASHKEY_NONE key;
    OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

    if (node && (node->m_flags & 0x4)) {
        CGGameUI::SetCursorSpell(spellId, node->m_overrideSpellID);
    } else {
        CGGameUI::SetCursorSpell(spellId, 0);
    }
}

// OFFSET: 0x009127C0/0x14066EC80
void CGSpellBook::PickupSpellBookItem(int32_t slot, int32_t petSpell) {
    if (!CGGameUI::CanPerformAction(14)) {
        return;
    }

    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

    if (!item) {
        return;
    }

    int32_t cursorSpell = CGGameUI::GetCursorSpell();
    uint32_t cursorPetAction = CGGameUI::m_cursorPetAction.m_data;
    int32_t cursorFlyout = CGGameUI::m_cursorFlyout;
    CGGameUI::ClearCursor(1, 1);

    if (cursorSpell > 0 || cursorPetAction > 0 || cursorFlyout > 0) {
        return;
    }

    if (item->m_type == SPELLBOOK_ITEM_SPELL) {
        if (!item->m_id) {
            return;
        }

        if (!petSpell) {
            CGSpellBook::SetCursorSpell(item->m_id);
        } else {
            CGGameUI::SetCursorPetAction(CGPetInfo::GetPetSpellAction(item->m_id));
        }

        PlaySpellPickupSound();
    } else if (item->m_type == SPELLBOOK_ITEM_PETACTION) {
        if (!item->m_id) {
            return;
        }

        PetAction action;
        action.m_data = item->m_id;
        CGGameUI::SetCursorPetAction(&action);
        PlaySpellPickupSound();
    } else if (item->m_type == SPELLBOOK_ITEM_FLYOUT) {
        if (!item->m_id) {
            return;
        }

        CGGameUI::SetCursorFlyout(item->m_id);
        PlaySpellPickupSound();
    }
}

// OFFSET: 0x009128F0/0x14066ED90
void CGSpellBook::PickupSpell(int32_t spellId) {
    if (!CGGameUI::CanPerformAction(14)) {
        return;
    }

    int32_t cursorSpell = CGGameUI::GetCursorSpell();
    uint32_t cursorPetAction = CGGameUI::m_cursorPetAction.m_data;
    int32_t cursorFlyout = CGGameUI::m_cursorFlyout;
    CGGameUI::ClearCursor(1, 1);

    if (cursorSpell <= 0 && cursorPetAction <= 0 && cursorFlyout <= 0 && spellId) {
        CGSpellBook::SetCursorSpell(spellId);
        PlaySpellPickupSound();
    }
}

// OFFSET: 0x00912950/0x14066EE10
void CGSpellBook::PickupCompanion(int32_t type, uint32_t index) {
    if (!CGGameUI::CanPerformAction(14) || index >= CGSpellBook::m_companions[type].m_count) {
        return;
    }

    int32_t spellId = CGSpellBook::m_companions[type].m_data[index];
    int32_t cursorSpell = CGGameUI::GetCursorSpell();
    CGGameUI::ClearCursor(1, 1);

    if (spellId == cursorSpell) {
        PlaySpellDropSound();
    } else if (cursorSpell <= 0) {
        CGSpellBook::SetCursorSpell(spellId);
        PlaySpellPickupSound();
    }
}

// OFFSET: 0x009129C0/0x14066EF00
void CGSpellBook::CastSpell(int32_t slot, int32_t petSpell, WOWGUID target) {
    if (CGGameUI::GetCursorSpell() || CGGameUI::m_cursorPetAction.m_data || CGGameUI::m_cursorFlyout) {
        CGGameUI::ClearCursor(1, 1);
        return;
    }

    CGGameUI::ClearCursor(1, 1);
    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

    if (!item) {
        return;
    }

    if (item->m_type == SPELLBOOK_ITEM_PETACTION) {
        PetAction action;
        action.m_data = item->m_id;
        CGPetInfo::SendPetAction(action, target, 0);
    } else if (item->m_type == SPELLBOOK_ITEM_SPELL) {
        int32_t spellId = item->m_id;

        if (petSpell) {
            PetAction action;
            action.m_data = 0x1000000 | spellId;
            CGPetInfo::SendPetAction(action, target, 0);
            return;
        }

        int32_t baseSpell = 0;
        int32_t ignoreCost = 0;
        HASHKEY_NONE key;
        OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

        if (node) {
            baseSpell = spellId;
            spellId = node->m_overrideSpellID;
            ignoreCost = (node->m_flags >> 1) & 0x1;
        }

        Spell_C_ClickSpell(spellId, NULL, target, baseSpell, ignoreCost);
    }
}

// OFFSET: 0x00912AB0/0x14066F030
int32_t CGSpellBook::GetSpell(int32_t slot, int32_t petSpell) {
    if (slot < 0) {
        return 0;
    }

    if (petSpell) {
        if (slot < static_cast<int32_t>(s_petSpellBookItems.m_count)) {
            SpellBookItem* item = s_petSpellBookItems.m_data[slot];

            if (item->m_type == SPELLBOOK_ITEM_SPELL || item->m_type == SPELLBOOK_ITEM_FUTURESPELL) {
                return item->m_id;
            }
        }
    } else if (slot < static_cast<int32_t>(s_spellBookItems.m_count)) {
        SpellBookItem* item = s_spellBookItems.m_data[slot];

        if (item->m_type == SPELLBOOK_ITEM_SPELL || item->m_type == SPELLBOOK_ITEM_FUTURESPELL) {
            return CGSpellBook::GetOverrideSpell(item->m_id);
        }
    }

    return 0;
}

// OFFSET: 0x00912B20/0x14066F090
int32_t CGSpellBook::FindSpellByName(const char* name, const char* subtext, int32_t& petSpell) {
    if (!name || !*name) {
        return 0;
    }

    SPELLSBYNAME* entry = s_spellsByName.Ptr(name);

    if (entry) {
        SPELLRANKNODE* node = entry->m_spells.Head();

        if (subtext && *subtext) {
            for (; node; node = entry->m_spells.Next(node)) {
                if (node->m_subtext.m_str && !SStrCmpUTF8I(node->m_subtext.m_str, subtext, STORM_MAX_STR)) {
                    break;
                }
            }
        } else {
            for (SPELLRANKNODE* rank = node; rank; rank = entry->m_spells.Next(rank)) {
                const char* text = rank->m_subtext.m_str;

                if (!text || !*text) {
                    node = rank;
                    break;
                }
            }
        }

        if (node) {
            petSpell = node->m_petSpell;
            return node->m_spellId;
        }
    }

    return 0;
}

// OFFSET: 0x00912BE0/0x14066F190
bool CGActionBar::ClearOverrideSpellclass(int32_t spellId) {
    bool cleared = false;

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    int32_t cursorSpell = CGGameUI::GetCursorSpell();
    int32_t restoreCursor = 0;

    for (OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.m_fulllist.Head(); node; ) {
        OVERRIDE_SPELL_NODE* next = CGActionBar::m_overrideSpells.m_fulllist.Next(node);

        if (node->m_overrideSpellID == spellId) {
            const SpellRec* spell = g_spellDB.GetRecord(node->m_overrideSpellID);

            if (spell) {
                OVERRIDE_SPELL_BY_NAME_NODE* byName = CGActionBar::m_overrideSpellsByName.Ptr(spell->m_name);

                if (byName) {
                    CGActionBar::m_overrideSpellsByName.Delete(byName);
                }
            }

            int32_t baseSpell = node->m_hashval;

            if (cursorSpell == baseSpell) {
                restoreCursor = cursorSpell;
            }

            CGActionBar::m_overrideSpells.Delete(node);
            CGActionBar::UpdateSpell(baseSpell);
            cleared = true;
        }

        node = next;
    }

    if (restoreCursor) {
        CGGameUI::SetCursorSpell(restoreCursor, 0);
    }

    return cleared;
}

// OFFSET: 0x00912D10/0x14066F320
int32_t CGActionBar::GetOverrideBaseSpellByName(const char* name) {
    OVERRIDE_SPELL_BY_NAME_NODE* node;

    if (name && *name && (node = CGActionBar::m_overrideSpellsByName.Ptr(name))) {
        return node->m_baseSpellID;
    }

    return 0;
}

// OFFSET: 0x00912D40/0x14066F360
int32_t CGActionBar::GetOverrideBaseSpell(int32_t spellId) {
    for (OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.m_fulllist.Head(); node; node = CGActionBar::m_overrideSpells.m_fulllist.Next(node)) {
        if (node->m_overrideSpellID == spellId) {
            return node->m_hashval;
        }
    }

    return 0;
}

// OFFSET: 0x00912D90/0x14066F3B0
const char* CGSpellBook::GetSpellTexture(int32_t spellId) {
    const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetOverrideSpell(spellId));

    if (!spell) {
        return NULL;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect && effect->m_effect == 78) {
        return CGActionBar::GetAttackTexture();
    }

    const char* texture;

    if ((spell->m_attributes & 0x2) && (spell->m_attributesExB & 0x20) && (texture = CGActionBar::GetRangedTexture())) {
        return texture;
    }

    uint32_t petIndex = CGStableInfo::FindPetIndexBySpellID(spell->m_id, 0);

    if (petIndex < 25) {
        StablePetInfo* pet = CGStableInfo::GetPetInfo(petIndex);

        if (pet) {
            CreatureStats_C* stats = g_creatureDBCache.GetRecord(pet->m_entry, WOWGUID(), NULL, NULL, false);
            const CreatureFamilyRec* family;

            if (stats && (family = g_creatureFamilyDB.GetRecord(stats->m_family))) {
                return family->m_iconFile;
            }
        }
    }

    const SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

    if (icon) {
        return icon->m_textureFilename;
    }

    return NULL;
}

// OFFSET: 0x00912ED0/0x14066F580
const char* CGSpellBook::GetSpellBookItemTexture(int32_t slot, int32_t petSpell) {
    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

    if (!item) {
        return NULL;
    }

    switch (item->m_type) {
        case SPELLBOOK_ITEM_SPELL:
        case SPELLBOOK_ITEM_FUTURESPELL:
            return CGSpellBook::GetSpellTexture(item->m_id);

        case SPELLBOOK_ITEM_PETACTION: {
            PetAction action;
            action.m_data = item->m_id;
            return CGPetInfo::GetPetActionTexture(action);
        }

        case SPELLBOOK_ITEM_FLYOUT: {
            const SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(item->m_id);
            const SpellIconRec* icon;

            if (!flyout || !(icon = g_spellIconDB.GetRecord(flyout->m_spellIconID))) {
                return NULL;
            }

            return icon->m_textureFilename;
        }

        default:
            return NULL;
    }
}

// OFFSET: 0x00913570/0x14066FDE0
void CGSpellBook::LeaveWorld(bool reload) {
    if (!reload) {
        CGActionBar::m_overrideSpells.InternalClear(0);
        CGActionBar::m_overrideSpellsByName.InternalClear(0);
    }
}

// OFFSET: 0x009135A0/0x14066FEF0
void CGSpellBook::SetBarSlotFromIntro(int32_t slot) {
    for (uint32_t i = 0; i < 12; i++) {
        if (s_introSlots[i].m_slot != slot) {
            continue;
        }

        int32_t spellId = s_introSlots[i].m_spellId;

        if (spellId <= 0) {
            continue;
        }

        CGSpellBook::SetCursorSpell(spellId);
        CGActionBar::PutActionInSlot(s_introSlots[i].m_slot, 0);
        s_introSlots[i].m_spellId = 0;
    }
}

// OFFSET: 0x00913620
void CGSpellBook::DelSpellFromNameCache(int32_t spellId, int32_t petSpell) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    SPELLSBYNAME* entry = s_spellsByName.Ptr(spell->m_name);

    if (!entry) {
        return;
    }

    for (SPELLRANKNODE* node = entry->m_spells.Head(); node; node = entry->m_spells.Next(node)) {
        if (node->m_spellId == spellId && petSpell == node->m_petSpell) {
            entry->m_spells.DeleteNode(node);
            return;
        }
    }
}

// OFFSET: 0x0090E180
static int32_t GetSpellBookItemSortOrder(int32_t type) {
    switch (type) {
        case SPELLBOOK_ITEM_SPELL:
        case SPELLBOOK_ITEM_FLYOUT:
            return 1;

        case SPELLBOOK_ITEM_FUTURESPELL:
            return 3;

        case SPELLBOOK_ITEM_PETACTION:
            return 2;

        default:
            return type + 100;
    }
}

// OFFSET: 0x009136A0/0x14066FFC0
int __cdecl SortSpellsForDisplay(const void* a, const void* b) {
    const SpellBookItem* itemA = *static_cast<SpellBookItem* const*>(a);
    const SpellBookItem* itemB = *static_cast<SpellBookItem* const*>(b);

    if (!itemA || !itemB) {
        return 0;
    }

    uint32_t indexA = CGSpellBook::GetSkillInfoIndex(itemA->m_skillLine);
    uint32_t indexB = CGSpellBook::GetSkillInfoIndex(itemB->m_skillLine);

    if (indexA != indexB) {
        return indexA < indexB ? -1 : 1;
    }

    if (itemA->m_inFlyout != itemB->m_inFlyout) {
        return 2 * (itemA->m_inFlyout != 0) - 1;
    }

    int32_t orderA = GetSpellBookItemSortOrder(itemA->m_type);
    int32_t orderB = GetSpellBookItemSortOrder(itemB->m_type);

    if (orderA != orderB) {
        return orderA - orderB;
    }

    if (orderA == 1 && orderB == 1) {
        SpellBookSkillInfo* info = CGSpellBook::GetSkillInfo(indexA);

        if (info && info->m_isProfession) {
            const SpellEffectRec* effectA = SpellGetEffect(itemA->m_id, 0);
            const SpellEffectRec* effectB = SpellGetEffect(itemB->m_id, 0);

            if (effectA && effectA->m_effect == 47 && (!effectB || effectB->m_effect != 47)) {
                return -1;
            }

            if (effectB && effectB->m_effect == 47 && (!effectA || effectA->m_effect != 47)) {
                return 1;
            }
        }
    }

    if (itemA->m_type == SPELLBOOK_ITEM_FUTURESPELL && itemB->m_type == SPELLBOOK_ITEM_FUTURESPELL && itemA->m_level != itemB->m_level) {
        return itemA->m_level - itemB->m_level;
    }

    if (itemA->m_type != SPELLBOOK_ITEM_SPELL && itemA->m_type != SPELLBOOK_ITEM_FUTURESPELL && itemA->m_type != SPELLBOOK_ITEM_FLYOUT) {
        return 0;
    }

    const char* nameA = NULL;
    const char* nameB = NULL;

    if (itemA->m_type == SPELLBOOK_ITEM_SPELL || itemA->m_type == SPELLBOOK_ITEM_FUTURESPELL) {
        const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetOverrideSpell(itemA->m_id));

        if (spell) {
            nameA = spell->m_name;
        }
    } else {
        const SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(itemA->m_id);

        if (flyout) {
            nameA = flyout->m_name;
        }
    }

    if (itemB->m_type == SPELLBOOK_ITEM_SPELL || itemB->m_type == SPELLBOOK_ITEM_FUTURESPELL) {
        const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetOverrideSpell(itemB->m_id));

        if (spell) {
            nameB = spell->m_name;
        }
    } else if (itemB->m_type == SPELLBOOK_ITEM_FLYOUT) {
        const SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(itemB->m_id);

        if (flyout) {
            nameB = flyout->m_name;
        }
    }

    if (!nameA || !nameB) {
        return 0;
    }

    return SStrSortUTF8I(nameA, nameB, STORM_MAX_STR);
}

// OFFSET: 0x00913890/0x140670320
void CGSpellBook::UpdateSpells(int32_t spells, int32_t petSpells, int32_t silenceAndInterrupts) {
    if (!s_futureSpells.m_count) {
        CGSpellBook::InitFutureSpells();
    }

    if (spells) {
        CGSpellBook::UpdateFutureSpells();
        qsort(s_skillInfos.m_data, s_numSkillInfos, sizeof(SpellBookSkillInfo*), SortSkillInfoByName);
        qsort(s_spellBookItems.m_data, s_spellBookItems.m_count, sizeof(SpellBookItem*), SortSpellsForDisplay);

        if (s_trackingSpells.m_count) {
            qsort(s_trackingSpells.m_data, s_trackingSpells.m_count, sizeof(int32_t), SortTrackingSpells);
        }
    }

    if (petSpells) {
        if (!spells) {
            qsort(s_skillInfos.m_data, s_numSkillInfos, sizeof(SpellBookSkillInfo*), SortSkillInfoByName);
        }

        qsort(s_petSpellBookItems.m_data, s_petSpellBookItems.m_count, sizeof(SpellBookItem*), SortSpellsForDisplay);
    }

    if (silenceAndInterrupts) {
        CGSpellBook::UpdateSilenceAndInterrupts();
    }

    if (spells || petSpells) {
        CGUIMacros::UpdateMacroSpells();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_SPELLS_CHANGED, NULL);
}

// OFFSET: 0x00913980/0x140670430
int32_t CGSpellBook::FindSlotByName(const char* name, const char* subtext, int32_t& petSpell) {
    int32_t spellId = CGSpellBook::FindSpellByName(name, subtext, petSpell);
    return CGSpellBook::FindSlotBySpellID(spellId, petSpell);
}

// OFFSET: 0x009139B0/0x1406704C0
int32_t CGSpellBook::FindSpellByName(const char* name, int32_t& petSpell) {
    if (!name || !*name) {
        return 0;
    }

    char spellName[1024];
    char subtext[512];
    spellName[0] = 0;
    subtext[0] = 0;

    if (*name == '!') {
        name++;
    }

    SStrCopy(spellName, name, sizeof(spellName));
    int32_t spellId = CGSpellBook::FindSpellByName(spellName, subtext, petSpell);

    if (spellId) {
        return spellId;
    }

    char* paren = strrchr(spellName, '(');

    if (paren) {
        *paren = 0;
        SStrCopy(subtext, paren + 1, sizeof(subtext));
    } else {
        subtext[0] = 0;
    }

    char* cursor = subtext;
    int32_t chars;
    uint32_t code;

    while ((code = sgetu8(reinterpret_cast<const uint8_t*>(cursor), &chars)) != 0xFFFFFFFF) {
        if (code == ')') {
            *cursor = 0;
            break;
        }

        cursor += chars;
    }

    return CGSpellBook::FindSpellByName(spellName, subtext, petSpell);
}

// OFFSET: 0x009140C0/0x140670C80
void CGSpellBook::ClearSpells() {
    if (s_knownSpellBits.m_count) {
        memset(s_knownSpellBits.m_bits.m_data, 0, sizeof(uint32_t) * s_knownSpellBits.m_bits.m_count);
    }

    for (int32_t i = s_spellBookItems.m_count - 1; i >= 0; i--) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (item->m_type != SPELLBOOK_ITEM_FUTURESPELL && item) {
            delete item;
        }
    }

    s_spellBookItems.Clear();

    for (uint32_t i = 0; i < s_skillInfos.m_count; i++) {
        if (s_skillInfos.m_data[i]) {
            delete s_skillInfos.m_data[i];
        }
    }

    s_skillInfos.Clear();
    s_skinningSpells[0] = 0;
    s_skinningSpells[1] = 0;
    s_skinningSpells[2] = 0;
    s_skinningSpells[3] = 0;
    s_skinPlayerCorpseSpell = 0;
    s_numSkillInfos = 0;
    CGSpellBook::m_duelSpell = 0;
    CGSpellBook::m_stuckSpell = 0;
    CGSpellBook::m_summonFriendSpell = 0;
    CGSpellBook::m_autoRangedSpell = 0;
    s_pendingCompanions = 0;

    for (uint32_t i = CGSpellBook::m_languageSpells.m_count; i; ) {
        CGSpellBook::m_languageSpells.m_data[--i] = 0;
    }

    s_trackingSpells.Clear();

    for (uint32_t i = 0; i < 2; i++) {
        CGSpellBook::m_companions[i].Clear();
    }

    s_openLockSpells.Clear();
    s_shapeshiftSpells.Clear();
    s_dispelTypes.Clear();
    s_summonCreatures.Clear();
    s_spellStealMask = 0;
    s_dispelMask = 0;
    s_knownSpells.Clear();

    for (uint32_t i = 0; i < 4; i++) {
        CGSpellBook::m_totemSpells[i].Clear();
    }

    CFlyout::ClearKnownSpells();
    CGSpellBook::s_passiveSpells.Clear();
}

// OFFSET: 0x009143D0/0x1406710C0
void CGSpellBook::DelKnownSpell(int32_t spellId, int32_t replacing) {
    s_knownSpellBits.m_bits.m_data[static_cast<uint32_t>(spellId) >> 5] &= ~(1 << (spellId & 0x1F));

    if (spellId == CGSpellBook::m_duelSpell) {
        CGSpellBook::m_duelSpell = 0;
    }

    if (spellId == CGSpellBook::m_stuckSpell) {
        CGSpellBook::m_stuckSpell = 0;
    }

    if (spellId == CGSpellBook::m_summonFriendSpell) {
        CGSpellBook::m_summonFriendSpell = 0;
    }

    if (spellId == CGSpellBook::m_autoRangedSpell) {
        CGSpellBook::m_autoRangedSpell = 0;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (spellId == s_skinningSpells[i]) {
            s_skinningSpells[i] = 0;
        }
    }

    if (spellId == s_skinPlayerCorpseSpell) {
        s_skinPlayerCorpseSpell = 0;
    }

    const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spellId);

    if (overlay && overlay->m_triggerType == 1) {
        CGSpellActivationOverlay::DelOverlayOnAvailableSpell(spellId);
    }

    CGSpellActivationOverlay::DelOverlayGlowableSpell(spellId);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    if (spell->m_attributesExD & 0x200) {
        for (uint32_t i = 0; i < s_knownSpells.m_count; i++) {
            const SpellRec* known = g_spellDB.GetRecord(s_knownSpells.m_data[i].m_spellId);

            if (!strcmp(spell->m_name, known->m_name)) {
                if (i != s_knownSpells.m_count - 1) {
                    s_knownSpells.m_data[i] = s_knownSpells.m_data[s_knownSpells.m_count - 1];
                }

                s_knownSpells.m_count--;
                break;
            }
        }
    }

    if (spell->m_attributes & 0x40) {
        for (PASSIVESPELLNODE* node = CGSpellBook::s_passiveSpells.Head(); node; ) {
            PASSIVESPELLNODE* next = CGSpellBook::s_passiveSpells.Next(node);

            if (node->m_spellId == spellId) {
                CGSpellBook::s_passiveSpells.DeleteNode(node);
            }

            node = next;
        }
    }

    CGSpellBook::DelSpellFromNameCache(spellId, 0);
    bool changed = false;

    for (int32_t i = s_spellBookItems.m_count - 1; i >= 0; i--) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (item->m_type != SPELLBOOK_ITEM_SPELL || item->m_id != spellId) {
            continue;
        }

        int32_t skillLine = item->m_skillLine;
        uint8_t inFlyout = item->m_inFlyout;
        delete item;

        if (i != static_cast<int32_t>(s_spellBookItems.m_count) - 1) {
            s_spellBookItems.m_data[i] = s_spellBookItems.m_data[s_spellBookItems.m_count - 1];
        }

        s_spellBookItems.m_count--;
        changed = true;

        for (uint32_t j = 0; j < s_numSkillInfos; j++) {
            SpellBookSkillInfo* info = s_skillInfos.m_data[j];

            if (info->m_skillLine == skillLine) {
                info->m_count--;

                if (!inFlyout) {
                    info->m_numVisible--;
                }

                break;
            }
        }

        break;
    }

    for (uint32_t i = CGSpellBook::m_languageSpells.m_count; i; ) {
        if (CGSpellBook::m_languageSpells.m_data[--i] == spellId) {
            CGSpellBook::m_languageSpells.m_data[i] = 0;
        }
    }

    for (uint32_t i = s_trackingSpells.m_count; i; ) {
        if (s_trackingSpells.m_data[--i] == spellId) {
            if (i < s_trackingSpells.m_count - 1) {
                memcpy(&s_trackingSpells.m_data[i], &s_trackingSpells.m_data[i + 1], sizeof(int32_t) * (s_trackingSpells.m_count - 1 - i));
            }

            s_trackingSpells.SetCount(s_trackingSpells.m_count - 1);
            break;
        }
    }

    for (uint32_t i = s_openLockSpells.m_count; i; ) {
        if (s_openLockSpells.m_data[--i] == spellId) {
            if (i < s_openLockSpells.m_count - 1) {
                memcpy(&s_openLockSpells.m_data[i], &s_openLockSpells.m_data[i + 1], sizeof(int32_t) * (s_openLockSpells.m_count - 1 - i));
            }

            s_openLockSpells.SetCount(s_openLockSpells.m_count - 1);
            break;
        }
    }

    if (!replacing) {
        for (uint32_t i = s_shapeshiftSpells.m_count; i; ) {
            if (s_shapeshiftSpells.m_data[--i].m_spellId == spellId) {
                if (i < s_shapeshiftSpells.m_count - 1) {
                    memcpy(&s_shapeshiftSpells.m_data[i], &s_shapeshiftSpells.m_data[i + 1], sizeof(SpellUsableInfo) * (s_shapeshiftSpells.m_count - 1 - i));
                }

                s_shapeshiftSpells.SetCount(s_shapeshiftSpells.m_count - 1);
                Script_SendUniqueSignal(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORMS);
                break;
            }
        }
    }

    bool totemsEmptied = false;

    for (uint32_t t = 0; t < 4; t++) {
        TSGrowableArray<int32_t>& totems = CGSpellBook::m_totemSpells[t];

        for (uint32_t i = totems.m_count; i; ) {
            if (totems.m_data[--i] == spellId) {
                if (i < totems.m_count - 1) {
                    memcpy(&totems.m_data[i], &totems.m_data[i + 1], sizeof(int32_t) * (totems.m_count - 1 - i));
                }

                totems.SetCount(totems.m_count - 1);

                if (!totems.m_count) {
                    totemsEmptied = true;
                }

                break;
            }
        }
    }

    bool multiCast = false;

    for (uint32_t i = 0; i < 4; i++) {
        if (spellId == s_multiCastSpells[i]) {
            multiCast = true;
            break;
        }
    }

    if (multiCast || totemsEmptied) {
        CGActionBar::UpdateMultiCastBar();
    }

    if (SpellIsCallCompanion(spell)) {
        int32_t type = CGSpellBook::GetCompanionType(spell);
        TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];
        uint32_t count = companions.m_count;
        bool found = false;

        for (uint32_t i = 0; i < count; i++) {
            if (companions.m_data[i] == spellId) {
                found = true;
            } else if (!found) {
                continue;
            }

            companions.m_data[i] = i == count - 1 ? 0 : companions.m_data[i + 1];
        }

        if (found) {
            companions.SetCount(count - 1);
            CGSpellBook::UpdateCompanions();
            FrameScript_SignalEvent(SCRIPT_EVENT_COMPANION_UNLEARNED, NULL);
        }
    }

    CFlyout::DelKnownSpell(spellId);

    for (int32_t i = s_spellBookItems.m_count - 1; i >= 0; i--) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (item->m_type != SPELLBOOK_ITEM_FLYOUT) {
            continue;
        }

        CFlyout* flyout = CFlyout::GetByID(item->m_id);

        if (!flyout || flyout->m_numKnownSpells) {
            continue;
        }

        int32_t skillLine = item->m_skillLine;
        delete item;

        if (i != static_cast<int32_t>(s_spellBookItems.m_count) - 1) {
            s_spellBookItems.m_data[i] = s_spellBookItems.m_data[s_spellBookItems.m_count - 1];
        }

        s_spellBookItems.m_count--;
        changed = true;

        for (uint32_t j = 0; j < s_numSkillInfos; j++) {
            if (s_skillInfos.m_data[j]->m_skillLine == skillLine) {
                s_skillInfos.m_data[j]->m_numFlyouts--;
                break;
            }
        }
    }

    if (!s_futureSpells.m_count) {
        CGSpellBook::InitFutureSpells();
    }

    if (changed) {
        CGSpellBook::UpdateFutureSpells();
        qsort(s_skillInfos.m_data, s_numSkillInfos, sizeof(SpellBookSkillInfo*), SortSkillInfoByName);
        qsort(s_spellBookItems.m_data, s_spellBookItems.m_count, sizeof(SpellBookItem*), SortSpellsForDisplay);

        if (s_trackingSpells.m_count) {
            qsort(s_trackingSpells.m_data, s_trackingSpells.m_count, sizeof(int32_t), SortTrackingSpells);
        }
    }

    CGSpellBook::UpdateSilenceAndInterrupts();

    if (changed) {
        CGUIMacros::UpdateMacroSpells();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_SPELLS_CHANGED, NULL);

    if (!changed) {
        return;
    }

    for (uint32_t i = 0; i < s_numSkillInfos; i++) {
        SpellBookSkillInfo* info = s_skillInfos.m_data[i];

        if (info->m_count || info->m_numFlyouts || info->m_numFuture) {
            continue;
        }

        s_numSkillInfos--;

        for (uint32_t j = i; j < s_numSkillInfos; j++) {
            *s_skillInfos.m_data[j] = *s_skillInfos.m_data[j + 1];
        }

        i++;
    }
}

// OFFSET: 0x00914CB0/0x140671CA0
int32_t CGSpellBook::FindSlotByName(const char* name, int32_t& petSpell) {
    int32_t spellId = CGSpellBook::FindSpellByName(name, petSpell);
    return CGSpellBook::FindSlotBySpellID(spellId, petSpell);
}

// OFFSET: 0x0090E170
static bool OnDontAutoPushSpellsToActionBar(ClientDontAutoPushSpellsToActionBar* msg) {
    s_dontAutoPushSpells = 1;
    return true;
}

// OFFSET: 0x00916D70/0x140676820
void CGSpellBook::InitializeGame() {
    s_knownSpellBits.m_count = g_spellDB.m_maxID + 1;
    uint32_t words = g_spellDB.m_maxID == -1 ? 0 : (static_cast<uint32_t>(g_spellDB.m_maxID) >> 5) + 1;

    if (words != s_knownSpellBits.m_bits.m_count) {
        if (words) {
            s_knownSpellBits.m_bits.ReallocData(words);
            s_knownSpellBits.m_bits.m_count = words;
        } else {
            s_knownSpellBits.m_bits.Clear();
        }
    }

    uint32_t languages = g_languagesDB.m_maxID + 1;

    if (languages != CGSpellBook::m_languageSpells.m_count) {
        if (g_languagesDB.m_maxID == -1) {
            CGSpellBook::m_languageSpells.Clear();
        } else {
            CGSpellBook::m_languageSpells.SetCount(languages);
        }
    }

    ClientDontAutoPushSpellsToActionBar::s_handler = JamHandlerEncode(&OnDontAutoPushSpellsToActionBar, g_jamHandlerKey_CFCDC0);
    CGSpellBook::ClearSpells();
    CGSpellBook::ClearPetSpells();

    for (uint32_t i = 0; i < 12; i++) {
        s_introSlots[i].m_spellId = 0;
    }
}

// OFFSET: 0x00916E40/0x1406769A0
void CGSpellBook::ShutdownGame() {
    for (uint32_t i = 0; i < s_skillInfos.m_count; i++) {
        if (s_skillInfos.m_data[i]) {
            delete s_skillInfos.m_data[i];
        }
    }

    s_skillInfos.Clear();
    s_numSkillInfos = 0;
    s_knownSpellBits.m_count = 0;

    if (s_knownSpellBits.m_bits.m_count) {
        s_knownSpellBits.m_bits.Clear();
    }

    CGSpellBook::m_languageSpells.Clear();
    s_spellsByName.Clear();
    CGSpellBook::s_passiveSpells.Clear();
    CGSpellBook::ClearSpells();

    for (int32_t i = s_petSpellBookItems.m_count - 1; i >= 0; i--) {
        if (s_petSpellBookItems.m_data[i]) {
            delete s_petSpellBookItems.m_data[i];
        }
    }

    s_petSpellBookItems.m_count = 0;
    s_dontAutoPushSpells = 0;
    ClientDontAutoPushSpellsToActionBar::s_handler = NULL;

    for (int32_t i = s_futureSpells.m_count - 1; i >= 0; i--) {
        if (s_futureSpells.m_data[i]) {
            delete s_futureSpells.m_data[i];
        }
    }

    s_futureSpells.m_count = 0;
}

// OFFSET: 0x00918250/0x140678600
void CGSpellBook::AddSpellToNameCache(int32_t spellId, int32_t petSpell) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    SPELLSBYNAME* entry = s_spellsByName.Ptr(spell->m_name);

    if (!entry) {
        entry = s_spellsByName.New(spell->m_name, 0, 0);
    }

    uint32_t rank = CGSpellBook::GetSpellProgressionRank(spell);
    SPELLRANKNODE* before = NULL;

    for (SPELLRANKNODE* node = entry->m_spells.Head(); node; node = entry->m_spells.Next(node)) {
        if (node->m_spellId == spellId) {
            return;
        }

        const SpellRec* other = g_spellDB.GetRecord(node->m_spellId);

        if ((other->m_attributesExD ^ spell->m_attributesExD) & 0x8000) {
            if (other->m_attributesExD & 0x8000) {
                before = node;
                break;
            }
        } else if (rank > node->m_rank && !petSpell) {
            before = node;
            break;
        }
    }

    SPELLRANKNODE* node = new SPELLRANKNODE();
    node->m_spellId = spellId;
    node->m_petSpell = petSpell;

    if (*spell->m_nameSubtext) {
        node->m_subtext.Copy(spell->m_nameSubtext);
    }

    node->m_rank = rank;

    if (before) {
        entry->m_spells.LinkNode(node, 2, before);
    } else {
        entry->m_spells.LinkToTail(node);
    }

    CGItem_C::RemoveNameFromCache(spell->m_name);
}

// OFFSET: 0x009184B0/0x140678A30
bool CGActionBar::SetOverrideSpellclass(const SpellRec* spell, uint32_t effectIndex, int32_t spellId) {
    bool set = false;

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    for (uint32_t i = 0; i < s_spellBookItems.m_count; i++) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (item->m_type != SPELLBOOK_ITEM_SPELL) {
            continue;
        }

        int32_t baseSpell = item->m_id;
        HASHKEY_NONE key;

        if (CGActionBar::m_overrideSpells.Ptr(baseSpell, key)) {
            continue;
        }

        const SpellRec* base = g_spellDB.GetRecord(baseSpell);

        if (!base || !SpellMatchesAuraSpellClass(base, spell, effectIndex)) {
            continue;
        }

        OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.New(baseSpell, key, 0, 0);
        node->m_overrideSpellID = spellId;
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, effectIndex);

        if (effect && effect->m_effectAura == 333) {
            node->m_flags |= 0x1;
        }

        if (spell->m_attributesExE & 0x40000) {
            node->m_flags |= 0x2;
        }

        if (spell->m_attributesExF & 0x400000) {
            node->m_flags |= 0x4;
        }

        if (baseSpell == CGGameUI::GetCursorSpell()) {
            CGGameUI::SetCursorSpell(baseSpell, spellId);
        }

        const SpellRec* overrideSpell = g_spellDB.GetRecord(spellId);

        if (overrideSpell) {
            OVERRIDE_SPELL_BY_NAME_NODE* byName = CGActionBar::m_overrideSpellsByName.New(overrideSpell->m_name, 0, 0);

            if (byName) {
                byName->m_overrideSpellID = spellId;
                byName->m_baseSpellID = baseSpell;
            }
        }

        CGActionBar::UpdateSpell(baseSpell);
        set = true;
    }

    return set;
}

// OFFSET: 0x00918680/0x140678E80
void CGSpellBook::AddKnownSpell(int32_t spellId, int32_t notify, int32_t replaceSpell) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    bool isProfession = false;
    int32_t skillLine = CGSpellBook::GetRealSpellBookSkillLine(spellId, &isProfession);

    if (skillLine == 810) {
        CGGlyphInfo::AddKnownGlyph(spellId);
        return;
    }

    if (isProfession) {
        CGSpellBook::AddSkillInfo(skillLine, isProfession);
        CGSpellBook::UpdateSpells(1, 0, 0);
    }

    s_knownSpellBits.m_bits.m_data[static_cast<uint32_t>(spellId) >> 5] |= 1 << (spellId & 0x1F);
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    const SpellEffectRec* first = count && effects->m_data[0] ? effects->m_data[0] : NULL;
    const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spellId);

    if (overlay && overlay->m_triggerType == 1) {
        CGSpellActivationOverlay::AddOverlayOnAvailableSpell(spellId);
    }

    CGSpellActivationOverlay::AddOverlayGlowableSpell(spellId);

    if (first) {
        switch (first->m_effect) {
            case 28:
            case 56: {
                int32_t creature = first->m_effectMiscValue;

                if (creature) {
                    uint32_t i;

                    for (i = 0; i < s_summonCreatures.m_count; i++) {
                        if (creature == s_summonCreatures.m_data[i]) {
                            break;
                        }
                    }

                    if (i >= s_summonCreatures.m_count) {
                        *s_summonCreatures.New() = creature;
                    }
                }

                break;
            }

            case 33:
                *s_openLockSpells.New() = spellId;
                break;

            case 39:
                CGSpellBook::m_languageSpells.m_data[first->m_effectMiscValue] = spellId;
                CGChat::UpdateLanguages();
                break;

            case 83:
                CGSpellBook::m_duelSpell = spellId;
                break;

            case 84:
                CGSpellBook::m_stuckSpell = spellId;
                break;

            case 95:
                s_skinningSpells[first->m_effectMiscValue] = spellId;
                break;

            case 116:
                s_skinPlayerCorpseSpell = spellId;
                break;

            case 152:
                CGSpellBook::m_summonFriendSpell = spellId;
                break;

            default:
                break;
        }
    }

    if (SpellIsTrackingAura(spell) && static_cast<int8_t>(spell->m_attributes) >= 0) {
        *s_trackingSpells.New() = spellId;
        CGSpellBook::AddSkillInfo(skillLine, isProfession);
        CGSpellBook::UpdateSpells(1, 0, 0);
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 38 || effect->m_implicitTargetA == 6) {
            continue;
        }

        uint32_t j;

        for (j = 0; j < s_dispelTypes.m_count; j++) {
            if (effect->m_effectMiscValue == s_dispelTypes.m_data[j]) {
                break;
            }
        }

        if (j >= s_dispelTypes.m_count) {
            *s_dispelTypes.New() = effect->m_effectMiscValue;
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 38 || effect->m_implicitTargetA != 6) {
            continue;
        }

        const SpellDispelTypeRec* dispel = g_spellDispelTypeDB.GetRecord(effect->m_effectMiscValue);
        uint32_t mask = dispel ? dispel->m_mask : 0;

        if (!mask) {
            mask = 1 << effect->m_effectMiscValue;
        }

        s_dispelMask |= mask;
    }

    if (spellId == 527) {
        const SpellDispelTypeRec* dispel = g_spellDispelTypeDB.GetRecord(1);
        s_dispelMask |= dispel && dispel->m_mask ? dispel->m_mask : 2;
    }

    if (first && first->m_effect == 126 && first->m_implicitTargetA == 6) {
        const SpellDispelTypeRec* dispel = g_spellDispelTypeDB.GetRecord(first->m_effectMiscValue);
        s_spellStealMask = dispel && dispel->m_mask ? dispel->m_mask : 1 << first->m_effectMiscValue;
    }

    if (spell->m_attributesExD & 0x1000000) {
        CGSpellBook::m_autoRangedSpell = spellId;
    }

    if (spell->m_attributesExD & 0x200) {
        uint32_t i;

        for (i = 0; i < s_knownSpells.m_count; i++) {
            const SpellRec* known = g_spellDB.GetRecord(s_knownSpells.m_data[i].m_spellId);

            if (!strcmp(spell->m_name, known->m_name)) {
                break;
            }
        }

        if (i < s_knownSpells.m_count) {
            const SpellRec* known = g_spellDB.GetRecord(s_knownSpells.m_data[i].m_spellId);

            if (CGSpellBook::GetSpellProgressionRank(spell) > CGSpellBook::GetSpellProgressionRank(known)) {
                s_knownSpells.m_data[i].m_spellId = spellId;
            }
        } else {
            SpellUsableInfo* info = s_knownSpells.New();
            info->m_spellId = spellId;
            info->m_usable = 0;
            info->m_noMana = 0;
        }
    }

    if (spell->m_attributesExB & 0x8100) {
        int32_t category = 0;
        const SpellCategoriesRec* categories;

        if (spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID))) {
            category = categories->m_category;
        }

        int32_t recovery = 0;
        const SpellCooldownsRec* cooldowns;

        if (spell->m_spellCooldownsID && (cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID))) {
            recovery = cooldowns->m_recoveryTime;
        }

        Spell_C_SetCooldownLeft(spell->m_id, 0, category, recovery, 0, true, 0);
    }

    if (!(spell->m_attributesExB & 0x2)) {
        uint32_t i;

        for (i = 0; i < count; i++) {
            if ((effects->m_data[i] && effects->m_data[i]->m_effectAura == 36) || (spell->m_attributesExB & 0x10)) {
                break;
            }
        }

        if (i < count) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (replaceSpell) {
                for (uint32_t j = 0; j < s_shapeshiftSpells.m_count; j++) {
                    SpellUsableInfo& info = s_shapeshiftSpells.m_data[j];

                    if (info.m_spellId != replaceSpell) {
                        continue;
                    }

                    info.m_spellId = spellId;
                    info.m_usable = Spell_C_IsUsableAction(player, spell, WOWGUID(), NULL, false, false, false);
                    int32_t powerType;
                    int32_t requirement;
                    info.m_noMana = !Spell_C_HaveSpellPower(player, spell, powerType, requirement);
                    Script_SendUniqueSignal(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORMS);
                    break;
                }
            } else {
                uint32_t j;

                for (j = 0; j < s_shapeshiftSpells.m_count; j++) {
                    if (s_shapeshiftSpells.m_data[j].m_spellId == spellId) {
                        break;
                    }
                }

                if (j >= s_shapeshiftSpells.m_count) {
                    SpellUsableInfo* info = s_shapeshiftSpells.New();
                    info->m_spellId = spellId;
                    info->m_usable = Spell_C_IsUsableAction(player, spell, WOWGUID(), NULL, false, false, false);
                    int32_t powerType;
                    int32_t requirement;
                    info->m_noMana = !Spell_C_HaveSpellPower(player, spell, powerType, requirement);
                    qsort(s_shapeshiftSpells.m_data, s_shapeshiftSpells.m_count, sizeof(SpellUsableInfo), SortSpellUsableInfo);
                    Script_SendUniqueSignal(SCRIPT_EVENT_UPDATE_SHAPESHIFT_FORMS);
                }
            }
        }
    }

    if (spell->m_attributes & 0x40) {
        PASSIVESPELLNODE* node = CGSpellBook::s_passiveSpells.NewNode(2, 0, 0x8);
        node->m_spellId = spellId;
    }

    if (spell->m_attributes & 0x80) {
        return;
    }

    if (spell->m_attributes & 0x20) {
        if (notify) {
            CGGameUI::DisplayError(ERR_LEARN_RECIPE_S, spell->m_name);
        }

        return;
    }

    CGSpellBook::AddSpellToNameCache(spellId, 0);

    if (SpellIsCallCompanion(spell)) {
        int32_t type = CGSpellBook::GetCompanionType(spell);
        *CGSpellBook::m_companions[type].New() = spellId;

        if (type == 0) {
            CGTutorial::TriggerTutorial(51);
        } else if (type == 1) {
            CGTutorial::TriggerTutorial(52);
        }

        int32_t creature = first ? first->m_effectMiscValue : 0;
        WOWGUID guid;
        guid.m_guid = static_cast<uint32_t>(spellId) | 0x1FE0000000000000ULL;
        CreatureStats_C* stats = g_creatureDBCache.GetRecord(creature, guid, &CGSpellBook::CompanionCallback, reinterpret_cast<void*>(static_cast<intptr_t>(notify)), false);

        if (stats) {
            if (notify) {
                CGGameUI::DisplayError(type == 1 ? ERR_LEARN_MOUNT_S : ERR_LEARN_COMPANION_S, stats->m_name[0]);
                CGSpellBook::UpdateCompanions();
                FrameScript_SignalEvent(SCRIPT_EVENT_COMPANION_LEARNED, NULL);
            }
        } else {
            s_pendingCompanions++;
        }

        return;
    }

    if (notify && !IsGuildPerkSpell(spellId)) {
        CGGameUI::DisplayError((spell->m_attributes & 0x10) ? ERR_LEARN_ABILITY_S : ERR_LEARN_SPELL_S, CGTooltip::GetSpellHyperlinkString(spell->m_id));
    }

    if (spell->m_attributesExD & 0x8000) {
        return;
    }

    bool inFlyout = CFlyout::AddKnownSpell(spellId);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t multiCast = 0;
    const SpellTotemsRec* totems;

    if (spell->m_spellTotemsID && (totems = g_spellTotemsDB.GetRecord(spell->m_spellTotemsID))) {
        bool tutorialDone = CGTutorial::m_tutorialFlags.m_count && !(CGTutorial::m_tutorialFlags.m_bits.m_data[1] & 0x4000);

        if (spell->m_attributesExG & 0x20) {
            for (uint32_t c = 0; c < 2; c++) {
                int32_t category = totems->m_requiredTotemCategoryID[c];
                uint32_t mask = CGActionBar::GetMultiCastSlotMaskForTotemCategory(category);

                for (uint32_t t = 0; t < 4; t++) {
                    if (!((1 << t) & mask)) {
                        continue;
                    }

                    TSGrowableArray<int32_t>& list = CGSpellBook::m_totemSpells[t];
                    uint32_t i;

                    for (i = 0; i < list.m_count; i++) {
                        if (spellId == list.m_data[i]) {
                            break;
                        }
                    }

                    if (i < list.m_count) {
                        continue;
                    }

                    int32_t rank = CGSpellBook::GetSpellProgressionRank(spell);
                    bool handled = false;

                    if (rank) {
                        for (i = 0; i < list.m_count; i++) {
                            const SpellRec* other = g_spellDB.GetRecord(list.m_data[i]);

                            if (!other || strcmp(spell->m_name, other->m_name)) {
                                continue;
                            }

                            if (CGSpellBook::GetSpellProgressionRank(other) < rank) {
                                list.m_data[i] = spellId;
                                multiCast = notify;
                            }

                            handled = true;
                            break;
                        }
                    }

                    if (!handled) {
                        *list.New() = spellId;
                        multiCast = notify;
                    }

                    if (!tutorialDone && multiCast && player && player->m_bag.FindTotemCategory(category, 8)) {
                        CGTutorial::TriggerTutorial(46);
                        tutorialDone = true;
                    }
                }
            }
        }
    }

    SpellBookItem* item = static_cast<SpellBookItem*>(SMemAlloc(sizeof(SpellBookItem), __FILE__, __LINE__, 0));
    s_spellBookItems.Add(1, &item);
    item->m_type = SPELLBOOK_ITEM_SPELL;
    item->m_id = spellId;
    int32_t level = 0;
    player->CanLearnSpell(spell->m_id, level, true);
    item->m_level = level;
    item->m_skillLine = skillLine;
    item->m_inFlyout = inFlyout;
    uint32_t index = CGSpellBook::AddSkillInfo(skillLine, isProfession);
    s_skillInfos.m_data[index]->m_count++;

    if (!item->m_inFlyout) {
        s_skillInfos.m_data[index]->m_numVisible++;
    }

    if (notify) {
        CGSpellBook::UpdateSpells(1, 0, 1);
        FrameScript_SignalEvent(SCRIPT_EVENT_LEARNED_SPELL_IN_TAB, "%d%d", spellId, index + 1);
        CGTutorial::TriggerTutorial(39);

        for (uint32_t i = 0; i < 4; i++) {
            if (spellId == s_multiCastSpells[i]) {
                multiCast = 1;
                break;
            }
        }

        if (player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) <= 10 && !s_dontAutoPushSpells && CGSpellBook::GetSpellRankFromSubtext(spell) <= 1 && !(spell->m_attributes & 0x40) && !(spell->m_attributesExD & 0x8000) && !(spell->m_attributesExB & 0x10) && !isProfession && !SpellHasAuraEffect(spell, 36) && !CFlyout::FindFlyoutForSpell(spell->m_id) && !CGActionBar::HasSpellAction(spell->m_id)) {
            uint32_t offset = GetActionBarPageOffset(player, spell);
            uint32_t end = offset + 12;
            bool intro = offset / 12 == 0;

            if (CGActionBar::m_bonusBarOffset) {
                intro = CGActionBar::m_bonusBarOffset + 5 == offset / 12;
            }

            intro = intro && !CGActionBar::m_currentPage;

            for (uint32_t slot = offset; slot < end; slot++) {
                if (CGActionBar::HasAction(slot)) {
                    continue;
                }

                if (!intro) {
                    CGSpellBook::SetCursorSpell(spell->m_id);
                    CGActionBar::PutActionInSlot(slot, 0);
                    break;
                }

                uint32_t i;

                for (i = 0; i < 12; i++) {
                    if (s_introSlots[i].m_slot == static_cast<int32_t>(slot) && s_introSlots[i].m_spellId > 0) {
                        break;
                    }
                }

                if (i < 12) {
                    continue;
                }

                for (i = 0; i < 12; i++) {
                    if (!s_introSlots[i].m_spellId) {
                        s_introSlots[i].m_spellId = spell->m_id;
                        s_introSlots[i].m_slot = slot;
                        FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_PUSHED_TO_ACTIONBAR, "%d%d%d", spellId, slot + 1, slot % 12 + 1);
                        break;
                    }
                }

                break;
            }
        }
    }

    if (multiCast) {
        CGActionBar::UpdateMultiCastBar();
    }
}

// OFFSET: 0x00919860/0x14067A6F0
void CGSpellBook::ReplaceSpell(int32_t oldSpell, int32_t newSpell) {
    CGSpellBook::DelKnownSpell(oldSpell, 1);
    CGSpellBook::AddKnownSpell(newSpell, 1, oldSpell);
}

// OFFSET: 0x00919890/0x14067A730
void CGSpellBook::AddPetSpell(int32_t spellId) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || (spell->m_attributes & 0x80) || (spell->m_attributesExD & 0x8000)) {
        return;
    }

    CGSpellBook::AddSpellToNameCache(spellId, 1);
    SpellBookItem* item = static_cast<SpellBookItem*>(SMemAlloc(sizeof(SpellBookItem), __FILE__, __LINE__, 0));
    s_petSpellBookItems.Add(1, &item);
    item->m_type = SPELLBOOK_ITEM_SPELL;
    item->m_id = spellId;
    item->m_level = 0;
    item->m_skillLine = CGSpellBook::GetRealSpellBookSkillLine(item->m_id, NULL);
    item->m_inFlyout = 0;
}

// OFFSET: 0x005ACD00
bool HasPetSpell(int32_t spellId) {
    if (static_cast<uint32_t>(spellId) > static_cast<uint32_t>(g_spellDB.m_maxID)) {
        return false;
    }

    for (uint32_t i = 0; i < s_petSpellBookItems.m_count; i++) {
        SpellBookItem* item = s_petSpellBookItems.m_data[i];

        if (item->m_type == SPELLBOOK_ITEM_SPELL && item->m_id == spellId) {
            return true;
        }
    }

    return false;
}
