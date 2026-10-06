#ifndef WOW_SOURCE_DB_REC_SPELLCASTTIMESREC_H
#define WOW_SOURCE_DB_REC_SPELLCASTTIMESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellCastTimesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_base;
    /* 0008 */ int32_t m_perLevel;
    /* 000C */ int32_t m_minimum;
};

#endif
