#ifndef WOW_SOURCE_DB_REC_UNITBLOODREC_H
#define WOW_SOURCE_DB_REC_UNITBLOODREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class UnitBloodRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_combatBloodSpurtFront [2];
    /* 000C */ int32_t m_combatBloodSpurtBack [2];
    /* 0014 */ DBCString m_groundBlood [5];

    static void FixupStrings(void* record, const char* strings);
};

#endif
