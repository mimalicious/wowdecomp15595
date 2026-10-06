#ifndef WOW_SOURCE_DB_REC_SPELLRUNECOSTREC_H
#define WOW_SOURCE_DB_REC_SPELLRUNECOSTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellRuneCostRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_blood;
    /* 0008 */ int32_t m_unholy;
    /* 000C */ int32_t m_frost;
    /* 0010 */ int32_t m_runicPower;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SpellRuneCostRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
