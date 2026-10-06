#ifndef WOW_SOURCE_DB_REC_HOLIDAYSREC_H
#define WOW_SOURCE_DB_REC_HOLIDAYSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class HolidaysRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_duration [10];
    /* 002C */ int32_t m_date [26];
    /* 0094 */ int32_t m_region;
    /* 0098 */ int32_t m_looping;
    /* 009C */ int32_t m_calendarFlags [10];
    /* 00C4 */ int32_t m_holidayNameID;
    /* 00C8 */ int32_t m_holidayDescriptionID;
    /* 00CC */ DBCString m_textureFileName;
    /* 00D0 */ int32_t m_priority;
    /* 00D4 */ int32_t m_calendarFilterType;
    /* 00D8 */ int32_t m_flags;
};

#endif
