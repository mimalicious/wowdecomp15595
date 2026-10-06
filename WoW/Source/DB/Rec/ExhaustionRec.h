#ifndef WOW_SOURCE_DB_REC_EXHAUSTIONREC_H
#define WOW_SOURCE_DB_REC_EXHAUSTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ExhaustionRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_xp;
    /* 0008 */ float m_factor;
    /* 000C */ float m_outdoorHours;
    /* 0010 */ float m_innHours;
    /* 0014 */ const char* m_name;
    /* 0018 */ float m_threshold;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ExhaustionRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
