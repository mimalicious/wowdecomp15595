#ifndef WOW_SOURCE_DB_REC_SPELLEFFECTREC_H
#define WOW_SOURCE_DB_REC_SPELLEFFECTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellEffectRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_effect;
    /* 0008 */ float m_effectValueMultiplier;
    /* 000C */ int32_t m_effectAura;
    /* 0010 */ int32_t m_effectAuraPeriod;
    /* 0014 */ int32_t m_effectBasePoints;
    /* 0018 */ float m_effectBonusMultiplier;
    /* 001C */ float m_effectDamageMultiplier;
    /* 0020 */ int32_t m_effectChainTargets;
    /* 0024 */ int32_t m_effectDieSides;
    /* 0028 */ int32_t m_effectItemType;
    /* 002C */ int32_t m_effectMechanic;
    /* 0030 */ int32_t m_effectMiscValue;
    /* 0034 */ int32_t m_effectMiscValueB;
    /* 0038 */ float m_effectPointsPerResource;
    /* 003C */ int32_t m_effectRadiusIndex;
    /* 0040 */ int32_t m_effectRadiusMaxIndex;
    /* 0044 */ float m_effectRealPointsPerLevel;
    /* 0048 */ int32_t m_effectSpellClassMaskA;
    /* 004C */ int32_t m_effectSpellClassMaskB;
    /* 0050 */ int32_t m_effectSpellClassMaskC;
    /* 0054 */ int32_t m_effectTriggerSpell;
    /* 0058 */ int32_t m_implicitTargetA;
    /* 005C */ int32_t m_implicitTargetB;
    /* 0060 */ int32_t m_spellID;
    /* 0064 */ int32_t m_effectIndex;
    /* 0068 */ int32_t m_effectAttributes;
};

#endif
