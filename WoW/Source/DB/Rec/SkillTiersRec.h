#ifndef WOW_SOURCE_DB_REC_SKILLTIERSREC_H
#define WOW_SOURCE_DB_REC_SKILLTIERSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SkillTiersRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost [16];
    /* 0044 */ int32_t m_value [16];

    static void FixupStrings(void* record, const char* strings);
};

#endif
