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
    /* 0014 */ const char* m_groundBlood [5];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(UnitBloodRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
