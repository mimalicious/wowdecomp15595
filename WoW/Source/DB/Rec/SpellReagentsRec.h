#ifndef WOW_SOURCE_DB_REC_SPELLREAGENTSREC_H
#define WOW_SOURCE_DB_REC_SPELLREAGENTSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellReagentsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_reagent[8];
    /* 0024 */ int32_t m_reagentCount[8];
};

#endif
