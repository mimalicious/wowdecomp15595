#ifndef WOW_SOURCE_DB_REC_SPELLAURAOPTIONSREC_H
#define WOW_SOURCE_DB_REC_SPELLAURAOPTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellAuraOptionsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cumulativeAura;
    /* 0008 */ int32_t m_procChance;
    /* 000C */ int32_t m_procCharges;
    /* 0010 */ int32_t m_procTypeMask;
};

#endif
