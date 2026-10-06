#ifndef WOW_SOURCE_DB_REC_RANDPROPPOINTSREC_H
#define WOW_SOURCE_DB_REC_RANDPROPPOINTSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class RandPropPointsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_epic [5];
    /* 0018 */ int32_t m_superior [5];
    /* 002C */ int32_t m_good [5];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(RandPropPointsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
