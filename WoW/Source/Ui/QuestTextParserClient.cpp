#include "WoW/Source/Ui/QuestTextParser.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/MountCapabilityRec.h"
#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellDescriptionVariablesRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellLevelsRec.h"
#include "WoW/Source/DB/Rec/SpellRadiusRec.h"
#include "WoW/Source/DB/Rec/SpellRangeRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"

// PROVEN: aa772d29 IDENTICAL/NONE
// OFFSET: 0x00A0A8A0/0x1407AB9C0
uint32_t QuestTextParser::GetPlayerBindAreaID() {
    return CGPlayer_C::GetBindAreaID();
}

// PROVEN: 7c39d1a1 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0A8B0/0x1407AB9D0
int32_t QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId) {
    return ::Spell_C_SpellGetSpellIDForDifficulty(spellId);
}

// PROVEN: dd22a3f9 IDENTICAL/NONE
// OFFSET: 0x00A0A8C0/0x1407AB9E0
int32_t QuestTextParser::SpellGetSpellIDForDifficulty(int32_t spellId, int32_t difficulty) {
    return ::SpellGetSpellIDForDifficulty(spellId, difficulty);
}

// PROVEN: 94594466 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0A8D0/0x1407AB9F0
void QuestTextParser::Spell_C_GetMinMaxPoints(const SpellRec* spell, int32_t effectIndex, float* minPoints, float* maxPoints, uint32_t spellLevel, int32_t parsePet, int32_t parseUnit, int32_t noModifiers) {
    ::Spell_C_GetMinMaxPoints(spell, effectIndex, minPoints, maxPoints, spellLevel, parsePet, parseUnit, noModifiers, 0);
}

// PROVEN: 64afd729 IDENTICAL/NONE
// OFFSET: 0x00A0A900
int32_t QuestTextParser::Spell_C_GetDuration(const SpellRec* spell, int32_t parsePet, int32_t parseUnit, int32_t noModifiers, int32_t applyHaste) {
    return ::Spell_C_GetDuration(spell, parsePet, parseUnit, noModifiers, applyHaste);
}

// PROVEN: ba2bc79f IDENTICAL/NONE
// OFFSET: 0x00A0A910/0x1407ABA60
int32_t QuestTextParser::Spell_C_GetAuraPeriod(const SpellRec* spell, uint32_t effectIndex) {
    return ::Spell_C_GetAuraPeriod(spell, effectIndex);
}

// PROVEN: 7da3b425 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0A920/0x1407ABA70
bool QuestTextParser::Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier) {
    return ::Spell_C_ModifySpellValue(spell, value, modifier);
}

// PROVEN: 5215995f IDENTICAL/DIFFERENT
// OFFSET: 0x00A0A930/0x1407ABA80
bool QuestTextParser::Spell_C_ModifySpellValue(const SpellRec* spell, float& value, int32_t modifier) {
    return ::Spell_C_ModifySpellValue(spell, value, modifier);
}

// PROVEN: 6497f754 IDENTICAL/DIFFERENT
// OFFSET: 0x00A0A940/0x1407ABA90
uint32_t QuestTextParser::Spell_C_GetSpellLevel(const SpellRec* spell, int32_t parsePet, int32_t parseUnit) {
    return ::Spell_C_GetSpellLevel(spell, parsePet, parseUnit);
}

// PROVEN: 67e4668f IDENTICAL/NONE
// OFFSET: 0x00A0A950
float QuestTextParser::Spell_C_GetPointsPerCombo(const CGUnit_C* unit, const SpellRec* spell, int32_t effectIndex) {
    return ::Spell_C_GetPointsPerCombo(unit, spell, effectIndex);
}

// PROVEN: 05a2ff93 IDENTICAL/NONE
// OFFSET: 0x00A0A960/0x1407ABAB0
CGUnit_C* QuestTextParser::Spell_C_GetStatTargetUnit(int32_t parsePet, int32_t parseUnit) {
    return ::Spell_C_GetStatTargetUnit(parsePet, parseUnit);
}

// PROVEN: 1dc59f1a IDENTICAL/NONE
// OFFSET: 0x00A0A970
bool QuestTextParser::ToolTip_DurationIsInt(float time, float fraction) {
    return CGTooltip::DurationIsInt(time, fraction);
}

// PROVEN: eca4fced IDENTICAL/IDENTICAL
// OFFSET: 0x00A0A990/0x1407ABAD0
void QuestTextParser::ToolTip_GetDurationString(char* buffer, uint32_t size, uint64_t time, const char* token, const char* prefix, int32_t roundUp, bool seconds) {
    CGTooltip::GetDurationString(buffer, size, time, token, prefix, roundUp, seconds);
}

// PROVEN: f2b3ea24 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0A9C0/0x1407ABB00
void QuestTextParser::ToolTip_GetFloatDurationString(char* buffer, uint32_t size, float time, const char* token, const char* prefix) {
    CGTooltip::GetFloatDurationString(buffer, size, time, token, prefix);
}

// PROVEN: f5aec8c4 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0A9F0/0x1407ABB20
uint32_t QuestTextParser::FrameScript_GetPluralIndex(int32_t count) {
    return ::FrameScript_GetPluralIndex(count);
}

// PROVEN: 9dac7cd5 IDENTICAL/NONE
// OFFSET: 0x00A0AA00
const char* QuestTextParser::FrameScript_GetText(const char* token, int32_t count, FRAMESCRIPT_GENDER gender) {
    return ::FrameScript_GetText(token, count, gender);
}

// PROVEN: 047fdee9 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0AA10/0x1407ABB40
const SpellEffectRec* QuestTextParser::GetSpellEffectRecord(uint32_t spellId, uint32_t effectIndex) {
    return SpellGetEffect(spellId, effectIndex);
}

// PROVEN: 2b28aa25 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0AA20/0x1407ABB50
uint32_t QuestTextParser::SpellGetNumEffects(uint32_t spellId) {
    return SpellGetEffectCount(spellId);
}

// PROVEN: 9d12f4aa IDENTICAL/IDENTICAL
// OFFSET: 0x00A0AA30/0x1407ABB60
int32_t QuestTextParser::GetUnitSpellManaCost(const SpellRec* spell, int32_t parseUnit, int32_t parsePet) {
    return Spell_C_GetPowerCost(spell, ::Spell_C_GetStatTargetUnit(parsePet, parseUnit));
}

// PROVEN: cbf53ab0 IDENTICAL/IDENTICAL
// OFFSET: 0x00A0AA50/0x1407ABB90
int32_t QuestTextParser::GetUnitSpellManaCostPerSecond(const SpellRec* spell, int32_t parseUnit, int32_t parsePet) {
    return Spell_C_GetPowerCostPerSecond(spell, ::Spell_C_GetStatTargetUnit(parsePet, parseUnit));
}

// OFFSET: 0x00A0AA70/0x1407ABBC0
int32_t QuestTextParser::Spell_C_GetUnitMountSpeed(int32_t mountCapabilityId, int32_t speedType) {
    if (!speedType) {
        return 0;
    }

    MountCapabilityRec* capability = g_mountCapabilityDB.GetRecord(mountCapabilityId);

    if (!capability) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(capability->m_speedModSpell);

    if (!spell) {
        return 0;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 6) {
            continue;
        }

        bool match;

        if (speedType == 1) {
            match = effect->m_effectAura == 32;
        } else if (speedType == 2) {
            match = effect->m_effectAura == 207;
        } else if (speedType == 3) {
            match = effect->m_effectAura == 58;
        } else {
            continue;
        }

        if (match) {
            int32_t minPoints = 0;
            int32_t maxPoints = 0;
            ::Spell_C_GetMinMaxPoints(spell, i, &minPoints, &maxPoints, 0, 0, 0, 0);
            return maxPoints;
        }
    }

    return 0;
}

// OFFSET: 0x00A0AB70
bool QuestTextParser::FrameScript_GetText(const char* token, char* buffer, uint32_t size, int32_t count, FRAMESCRIPT_GENDER gender) {
    return ::FrameScript_GetText(token, buffer, size, count, gender);
}

// PROVEN: 01c9544f IDENTICAL/NONE
// OFFSET: 0x00A0AB90
AreaTableRec* QuestTextParser::GetAreaTableRecord(int32_t id) {
    return g_areaTableDB.GetRecord(id);
}

// PROVEN: 95ceaa63 IDENTICAL/UNPROVEN
// OFFSET: 0x00A0ABC0/0x1407ABDA0
SpellAuraOptionsRec* QuestTextParser::GetSpellAuraOptionsRecord(int32_t id) {
    return g_spellAuraOptionsDB.GetRecord(id);
}

// PROVEN: 6fa8ea0b IDENTICAL/UNPROVEN
// OFFSET: 0x00A0ABF0/0x1407ABDD0
SpellDescriptionVariablesRec* QuestTextParser::GetSpellDescriptionVariablesRecord(int32_t id) {
    return g_spellDescriptionVariablesDB.GetRecord(id);
}

// PROVEN: ebca065f IDENTICAL/NONE
// OFFSET: 0x00A0AC20
SpellLevelsRec* QuestTextParser::GetSpellLevelsRecord(int32_t id) {
    return g_spellLevelsDB.GetRecord(id);
}

// PROVEN: e6f6856c IDENTICAL/UNPROVEN
// OFFSET: 0x00A0AC50/0x1407ABE30
SpellRadiusRec* QuestTextParser::GetSpellRadiusRecord(int32_t id) {
    return g_spellRadiusDB.GetRecord(id);
}

// PROVEN: 671bcff7 IDENTICAL/UNPROVEN
// OFFSET: 0x00A0AC80/0x1407ABE60
SpellRangeRec* QuestTextParser::GetSpellRangeRecord(int32_t id) {
    return g_spellRangeDB.GetRecord(id);
}

// PROVEN: aa43cc81 IDENTICAL/UNPROVEN
// OFFSET: 0x00A0ACB0/0x1407ABE90
SpellTargetRestrictionsRec* QuestTextParser::GetSpellTargetRestrictionsRecord(int32_t id) {
    return g_spellTargetRestrictionsDB.GetRecord(id);
}

// PROVEN: 91abbbfa IDENTICAL/UNPROVEN
// OFFSET: 0x00A0ACE0/0x1407ABEC0
SpellRec* QuestTextParser::GetSpellRecord(int32_t id) {
    return g_spellDB.GetRecord(id);
}

// PROVEN: 12fd4665 IDENTICAL/DIFFERENT
// OFFSET: 0x00A0AD10/0x1407ABEF0
void QuestTextParser::GetPlayerLevel(int32_t& level) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        level = player->m_unit->m_unitFieldLevel;
    }
}

// PROVEN: 777901d6 IDENTICAL/DIFFERENT
// OFFSET: 0x00A0AD50/0x1407ABF40
bool QuestTextParser::GetPlayerGenderConditional(char* buffer, uint32_t size, const NameCache* name, const char*& cursor) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    int32_t gender = player->m_unit->m_unitFieldBytes0[2];

    if (gender == 2) {
        gender = player->m_player->m_playerBytes3[0];
    }

    GenderConditional(buffer, size, gender, player, name, &cursor);
    return true;
}

// PROVEN: 9f29bd40 IDENTICAL/DIFFERENT
// OFFSET: 0x00A0ADB0/0x1407ABFD0
bool QuestTextParser::IsPlayerSpellKnown(int32_t spellId) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        return player->IsSpellKnown(spellId);
    }

    return false;
}

// OFFSET: 0x00A0ADF0/0x1407AC020
bool QuestTextParser::PlayerHasAura(int32_t spellId) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        return player->HasAura(spellId, WOWGUID());
    }

    return false;
}

// PROVEN: 21949c2d IDENTICAL/DIFFERENT
// OFFSET: 0x00A0AE30/0x1407AC070
bool QuestTextParser::PlayerHasItemPassive(int32_t spellId) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        return player->HasItemPassive(spellId);
    }

    return false;
}
