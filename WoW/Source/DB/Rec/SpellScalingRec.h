#ifndef WOW_SOURCE_DB_REC_SPELLSCALINGREC_H
#define WOW_SOURCE_DB_REC_SPELLSCALINGREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellScalingRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_castTimeMin;
    /* 0008 */ int32_t m_castTimeMax;
    /* 000C */ int32_t m_castTimeMaxLevel;
    /* 0010 */ int32_t m_class;
    /* 0014 */ float m_coefficient [3];
    /* 0020 */ float m_variance [3];
    /* 002C */ float m_comboPointsCoefficient [3];
    /* 0038 */ float m_nerfFactor;
    /* 003C */ int32_t m_nerfMaxLevel;
};

#endif
