#ifndef WOW_SOURCE_UI_QUESTTEXTPARSER_H
#define WOW_SOURCE_UI_QUESTTEXTPARSER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/WGUID.h"

class AreaTableRec;
class AuraData;
class CGUnit_C;
class ItemRecSparse;
class NameCache;
class SpellAuraOptionsRec;
class SpellDescriptionVariablesRec;
class SpellEffectRec;
class SpellItemEnchantmentRec;
class SpellLevelsRec;
class SpellRadiusRec;
class SpellRangeRec;
class SpellRec;
class SpellTargetRestrictionsRec;

struct SpellVariables {
    /* 0000 */ char m_name[16];
    /* 0010 */ char m_text[32];
    /* 0030 */ float m_value;
    /* 0034 */ int32_t m_intValue;
};

namespace QuestTextParser {
    uint32_t GetPlayerBindAreaID();
    int32_t Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId);
    int32_t SpellGetSpellIDForDifficulty(int32_t spellId, int32_t difficulty);
    void Spell_C_GetMinMaxPoints(const SpellRec* spell, int32_t effectIndex, float* minPoints, float* maxPoints, uint32_t spellLevel, int32_t parsePet, int32_t parseUnit, int32_t noModifiers);
    int32_t Spell_C_GetDuration(const SpellRec* spell, int32_t parsePet, int32_t parseUnit, int32_t noModifiers, int32_t applyHaste);
    int32_t Spell_C_GetAuraPeriod(const SpellRec* spell, uint32_t effectIndex);
    bool Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier);
    bool Spell_C_ModifySpellValue(const SpellRec* spell, float& value, int32_t modifier);
    uint32_t Spell_C_GetSpellLevel(const SpellRec* spell, int32_t parsePet, int32_t parseUnit);
    float Spell_C_GetPointsPerCombo(const CGUnit_C* unit, const SpellRec* spell, int32_t effectIndex);
    CGUnit_C* Spell_C_GetStatTargetUnit(int32_t parsePet, int32_t parseUnit);
    bool ToolTip_DurationIsInt(float time, float fraction);
    void ToolTip_GetDurationString(char* buffer, uint32_t size, uint64_t time, const char* token, const char* prefix, int32_t roundUp, bool seconds);
    void ToolTip_GetFloatDurationString(char* buffer, uint32_t size, float time, const char* token, const char* prefix);
    uint32_t FrameScript_GetPluralIndex(int32_t count);
    const char* FrameScript_GetText(const char* token, int32_t count, FRAMESCRIPT_GENDER gender);
    const SpellEffectRec* GetSpellEffectRecord(uint32_t spellId, uint32_t effectIndex);
    uint32_t SpellGetNumEffects(uint32_t spellId);
    int32_t GetUnitSpellManaCost(const SpellRec* spell, int32_t parseUnit, int32_t parsePet);
    int32_t GetUnitSpellManaCostPerSecond(const SpellRec* spell, int32_t parseUnit, int32_t parsePet);
    int32_t Spell_C_GetUnitMountSpeed(int32_t mountCapabilityId, int32_t speedType);
    bool FrameScript_GetText(const char* token, char* buffer, uint32_t size, int32_t count, FRAMESCRIPT_GENDER gender);
    AreaTableRec* GetAreaTableRecord(int32_t id);
    SpellAuraOptionsRec* GetSpellAuraOptionsRecord(int32_t id);
    SpellDescriptionVariablesRec* GetSpellDescriptionVariablesRecord(int32_t id);
    SpellLevelsRec* GetSpellLevelsRecord(int32_t id);
    SpellRadiusRec* GetSpellRadiusRecord(int32_t id);
    SpellRangeRec* GetSpellRangeRecord(int32_t id);
    SpellTargetRestrictionsRec* GetSpellTargetRestrictionsRecord(int32_t id);
    SpellRec* GetSpellRecord(int32_t id);
    void GetPlayerLevel(int32_t& level);
    bool GetPlayerGenderConditional(char* buffer, uint32_t size, const NameCache* name, const char*& cursor);
    bool IsPlayerSpellKnown(int32_t spellId);
    bool PlayerHasAura(int32_t spellId);
    bool PlayerHasItemPassive(int32_t spellId);
}

void ParseCountDownTimerText(char* buffer, uint32_t size, int32_t time);
void GenderConditional(char* buffer, uint32_t size, int32_t gender, const CGUnit_C* unit, const NameCache* name, const char** cursor);
int32_t EnchantmentParserParseText(const SpellItemEnchantmentRec* enchant, char* buffer, uint32_t size, int32_t value, int32_t itemLevel, const ItemRecSparse* item);
int32_t ChatParserParseText(const char* text, char* buffer, uint32_t size);
bool QuestParserParseToken(char* buffer, uint32_t size, const CGUnit_C* unit, const NameCache* record, const char* name, int32_t achievement, const char** cursor, WOWGUID guid);
int32_t QuestParserParseText(const char* text, char* buffer, uint32_t size, const WOWGUID& guid, const char* name, int32_t achievement);
int32_t JournalParserParseText(const char* text, char* buffer, uint32_t size, int32_t difficulty);
int32_t SpellParserParseText(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t useAuraDescription, int32_t scale, int32_t noModifiers, const AuraData* aura);
int32_t SpellParserParseTextAsSpell(const SpellRec* spell, char* buffer, uint32_t size, const char* text, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const AuraData* aura);

#endif
