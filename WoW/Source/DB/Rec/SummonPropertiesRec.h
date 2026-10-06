#ifndef WOW_SOURCE_DB_REC_SUMMONPROPERTIESREC_H
#define WOW_SOURCE_DB_REC_SUMMONPROPERTIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SummonPropertiesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_control;
    /* 0008 */ int32_t m_faction;
    /* 000C */ int32_t m_title;
    /* 0010 */ int32_t m_slot;
    /* 0014 */ int32_t m_flags;

    static void FixupStrings(void* record, const char* strings);
};

#endif
