#ifndef WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTCONDITIONREC_H
#define WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTCONDITIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellItemEnchantmentConditionRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_ltOperandType [5];
    /* 000C */ int32_t m_ltOperand [5];
    /* 0020 */ int8_t m_operator [5];
    /* 0025 */ int8_t m_rtOperandType [5];
    /* 002C */ int32_t m_rtOperand [5];
    /* 0040 */ int8_t m_logic [5];
};

#endif
