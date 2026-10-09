#ifndef WOW_SOURCE_DB_REC_SPELLREC_H
#define WOW_SOURCE_DB_REC_SPELLREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);
    static int32_t IsHelpfulOrHarmful(const SpellRec* spell);
    static bool WillSummonCritter(const SpellRec* spell);
    static const int32_t* GetClassMask(const SpellRec* spell, int32_t effect);
    static bool IsAffectedBy(const SpellRec* spell, const SpellRec* modifier, int32_t effect);
    static bool IsMeleeRange(const SpellRec* spell);
    static bool IsMinimumRange(const SpellRec* spell);
    static void GetMinRange(const SpellRec* spell, float* range);
    static bool HasModifier(const SpellRec* spell, int32_t modifier);
    static bool NotUsableInShapeshiftForm(const SpellRec* spell, int32_t form);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_attributes;
    /* 0008 */ int32_t m_attributesEx;
    /* 000C */ int32_t m_attributesExB;
    /* 0010 */ int32_t m_attributesExC;
    /* 0014 */ int32_t m_attributesExD;
    /* 0018 */ int32_t m_attributesExE;
    /* 001C */ int32_t m_attributesExF;
    /* 0020 */ int32_t m_attributesExG;
    /* 0024 */ int32_t m_attributesExH;
    /* 0028 */ int32_t m_attributesExI;
    /* 002C */ int32_t m_attributesExJ;
    /* 0030 */ int32_t m_castingTimeIndex;
    /* 0034 */ int32_t m_durationIndex;
    /* 0038 */ int32_t m_powerType;
    /* 003C */ int32_t m_rangeIndex;
    /* 0040 */ float m_speed;
    /* 0044 */ int32_t m_spellVisualID [2];
    /* 004C */ int32_t m_spellIconID;
    /* 0050 */ int32_t m_activeIconID;
    /* 0054 */ DBCString m_name;
    /* 0058 */ DBCString m_nameSubtext;
    /* 005C */ DBCString m_description;
    /* 0060 */ DBCString m_auraDescription;
    /* 0064 */ int32_t m_schoolMask;
    /* 0068 */ int32_t m_runeCostID;
    /* 006C */ int32_t m_spellMissileID;
    /* 0070 */ int32_t m_spellDescriptionVariableID;
    /* 0074 */ int32_t m_difficulty;
    /* 0078 */ float m_spellCoefficient;
    /* 007C */ int32_t m_spellScalingID;
    /* 0080 */ int32_t m_spellAuraOptionsID;
    /* 0084 */ int32_t m_spellAuraRestrictionsID;
    /* 0088 */ int32_t m_spellCastingRequirementsID;
    /* 008C */ int32_t m_spellCategoriesID;
    /* 0090 */ int32_t m_spellClassOptionsID;
    /* 0094 */ int32_t m_spellCooldownsID;
    /* 0098 */ int32_t unk_0098;
    /* 009C */ int32_t m_spellEquippedItemsID;
    /* 00A0 */ int32_t m_spellInterruptsID;
    /* 00A4 */ int32_t m_spellLevelsID;
    /* 00A8 */ int32_t m_spellPowerID;
    /* 00AC */ int32_t m_spellReagentsID;
    /* 00B0 */ int32_t m_spellShapeshiftID;
    /* 00B4 */ int32_t m_spellTargetRestrictionsID;
    /* 00B8 */ int32_t m_spellTotemsID;
    /* 00BC */ int32_t m_researchProjectID;
};

#endif
