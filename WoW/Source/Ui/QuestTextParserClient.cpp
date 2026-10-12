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

// OFFSET: 0x00A0A8A0/0x1407AB9C0
uint32_t QuestTextParser::GetPlayerBindAreaID() {
    return CGPlayer_C::GetBindAreaID();
}

// OFFSET: 0x00A0A8B0/0x1407AB9D0
int32_t QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId) {
    return ::Spell_C_SpellGetSpellIDForDifficulty(spellId);
}

// OFFSET: 0x00A0A8C0/0x1407AB9E0
int32_t QuestTextParser::SpellGetSpellIDForDifficulty(int32_t spellId, int32_t difficulty) {
    return ::SpellGetSpellIDForDifficulty(spellId, difficulty);
}

// OFFSET: 0x00A0A8D0/0x1407AB9F0
void QuestTextParser::Spell_C_GetMinMaxPoints(const SpellRec* spell, int32_t effectIndex, float* minPoints, float* maxPoints, uint32_t spellLevel, int32_t parsePet, int32_t parseUnit, int32_t noModifiers) {
    ::Spell_C_GetMinMaxPoints(spell, effectIndex, minPoints, maxPoints, spellLevel, parsePet, parseUnit, noModifiers, 0);
}

// OFFSET: 0x00A0A900
int32_t QuestTextParser::Spell_C_GetDuration(const SpellRec* spell, int32_t parsePet, int32_t parseUnit, int32_t noModifiers, int32_t applyHaste) {
    return ::Spell_C_GetDuration(spell, parsePet, parseUnit, noModifiers, applyHaste);
}

// OFFSET: 0x00A0A910/0x1407ABA60
int32_t QuestTextParser::Spell_C_GetAuraPeriod(const SpellRec* spell, uint32_t effectIndex) {
    return ::Spell_C_GetAuraPeriod(spell, effectIndex);
}

// OFFSET: 0x00A0A920/0x1407ABA70
bool QuestTextParser::Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier) {
    return ::Spell_C_ModifySpellValue(spell, value, modifier);
}

// OFFSET: 0x00A0A930/0x1407ABA80
bool QuestTextParser::Spell_C_ModifySpellValue(const SpellRec* spell, float& value, int32_t modifier) {
    return ::Spell_C_ModifySpellValue(spell, value, modifier);
}

// OFFSET: 0x00A0A940/0x1407ABA90
uint32_t QuestTextParser::Spell_C_GetSpellLevel(const SpellRec* spell, int32_t parsePet, int32_t parseUnit) {
    return ::Spell_C_GetSpellLevel(spell, parsePet, parseUnit);
}

// OFFSET: 0x00A0A950
float QuestTextParser::Spell_C_GetPointsPerCombo(const CGUnit_C* unit, const SpellRec* spell, int32_t effectIndex) {
    return ::Spell_C_GetPointsPerCombo(unit, spell, effectIndex);
}

// OFFSET: 0x00A0A960/0x1407ABAB0
CGUnit_C* QuestTextParser::Spell_C_GetStatTargetUnit(int32_t parsePet, int32_t parseUnit) {
    return ::Spell_C_GetStatTargetUnit(parsePet, parseUnit);
}

// OFFSET: 0x00A0A970/0x1407ABAC0
bool QuestTextParser::ToolTip_DurationIsInt(float time, float fraction) {
    return CGTooltip::DurationIsInt(time, fraction);
}

// OFFSET: 0x00A0A990/0x1407ABAD0
void QuestTextParser::ToolTip_GetDurationString(char* buffer, uint32_t size, uint64_t time, const char* token, const char* prefix, int32_t roundUp, bool seconds) {
    CGTooltip::GetDurationString(buffer, size, time, token, prefix, roundUp, seconds);
}

// OFFSET: 0x00A0A9C0/0x1407ABB00
void QuestTextParser::ToolTip_GetFloatDurationString(char* buffer, uint32_t size, float time, const char* token, const char* prefix) {
    CGTooltip::GetFloatDurationString(buffer, size, time, token, prefix);
}

// OFFSET: 0x00A0A9F0/0x1407ABB20
uint32_t QuestTextParser::FrameScript_GetPluralIndex(int32_t count) {
    return ::FrameScript_GetPluralIndex(count);
}

// OFFSET: 0x00A0AA00/0x1407ABB30
const char* QuestTextParser::FrameScript_GetText(const char* token, int32_t count, FRAMESCRIPT_GENDER gender) {
    return ::FrameScript_GetText(token, count, gender);
}

// OFFSET: 0x00A0AA10/0x1407ABB40
const SpellEffectRec* QuestTextParser::GetSpellEffectRecord(uint32_t spellId, uint32_t effectIndex) {
    return SpellGetEffect(spellId, effectIndex);
}

// OFFSET: 0x00A0AA20/0x1407ABB50
uint32_t QuestTextParser::SpellGetNumEffects(uint32_t spellId) {
    return SpellGetEffectCount(spellId);
}

// OFFSET: 0x00A0AA30/0x1407ABB60
int32_t QuestTextParser::GetUnitSpellManaCost(const SpellRec* spell, int32_t parseUnit, int32_t parsePet) {
    return Spell_C_GetPowerCost(spell, ::Spell_C_GetStatTargetUnit(parsePet, parseUnit));
}

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

// OFFSET: 0x00A0AB70/0x1407ABD00
bool QuestTextParser::FrameScript_GetText(const char* token, char* buffer, uint32_t size, int32_t count, FRAMESCRIPT_GENDER gender) {
    return ::FrameScript_GetText(token, buffer, size, count, gender);
}

// OFFSET: 0x00A0AB90/0x1407ABD70
AreaTableRec* QuestTextParser::GetAreaTableRecord(int32_t id) {
    return g_areaTableDB.GetRecord(id);
}

// OFFSET: 0x00A0ABC0/0x1407ABDA0
SpellAuraOptionsRec* QuestTextParser::GetSpellAuraOptionsRecord(int32_t id) {
    return g_spellAuraOptionsDB.GetRecord(id);
}

// OFFSET: 0x00A0ABF0/0x1407ABDD0
SpellDescriptionVariablesRec* QuestTextParser::GetSpellDescriptionVariablesRecord(int32_t id) {
    return g_spellDescriptionVariablesDB.GetRecord(id);
}

// OFFSET: 0x00A0AC20
SpellLevelsRec* QuestTextParser::GetSpellLevelsRecord(int32_t id) {
    return g_spellLevelsDB.GetRecord(id);
}

// OFFSET: 0x00A0AC50/0x1407ABE30
SpellRadiusRec* QuestTextParser::GetSpellRadiusRecord(int32_t id) {
    return g_spellRadiusDB.GetRecord(id);
}

// OFFSET: 0x00A0AC80/0x1407ABE60
SpellRangeRec* QuestTextParser::GetSpellRangeRecord(int32_t id) {
    return g_spellRangeDB.GetRecord(id);
}

// OFFSET: 0x00A0ACB0/0x1407ABE90
SpellTargetRestrictionsRec* QuestTextParser::GetSpellTargetRestrictionsRecord(int32_t id) {
    return g_spellTargetRestrictionsDB.GetRecord(id);
}

// OFFSET: 0x00A0ACE0/0x1407ABEC0
SpellRec* QuestTextParser::GetSpellRecord(int32_t id) {
    return g_spellDB.GetRecord(id);
}

// OFFSET: 0x00A0AD10/0x1407ABEF0
void QuestTextParser::GetPlayerLevel(int32_t& level) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        level = player->m_unit->m_unitFieldLevel;
    }
}

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

// OFFSET: 0x00A0AE30/0x1407AC070
bool QuestTextParser::PlayerHasItemPassive(int32_t spellId) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        return player->HasItemPassive(spellId);
    }

    return false;
}
