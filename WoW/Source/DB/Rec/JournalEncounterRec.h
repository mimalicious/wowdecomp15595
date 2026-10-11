#ifndef WOW_SOURCE_DB_REC_JOURNALENCOUNTERREC_H
#define WOW_SOURCE_DB_REC_JOURNALENCOUNTERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class JournalEncounterRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_dungeonMapID;
    /* 0008 */ int32_t m_worldMapAreaID;
    /* 000C */ float m_mapX;
    /* 0010 */ float m_mapY;
    /* 0014 */ int32_t m_firstSectionID;
    /* 0018 */ int32_t m_journalInstanceID;
    /* 001C */ int32_t m_orderIndex;
    /* 0020 */ int32_t m_difficultyMask;
    /* 0024 */ DBCString m_name;
    /* 0028 */ DBCString m_description;
};

#endif
