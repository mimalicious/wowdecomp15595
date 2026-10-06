#ifndef WOW_SOURCE_DB_REC_ZONEINTROMUSICTABLEREC_H
#define WOW_SOURCE_DB_REC_ZONEINTROMUSICTABLEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ZoneIntroMusicTableRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_soundID;
    /* 000C */ int32_t m_priority;
    /* 0010 */ int32_t m_minDelayMinutes;

    static void FixupStrings(void* record, const char* strings);
};

#endif
