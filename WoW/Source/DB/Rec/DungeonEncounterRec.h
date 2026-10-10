#ifndef WOW_SOURCE_DB_REC_DUNGEONENCOUNTERREC_H
#define WOW_SOURCE_DB_REC_DUNGEONENCOUNTERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DungeonEncounterRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_difficulty;
    /* 000C */ int32_t m_orderIndex;
    /* 0010 */ int32_t m_bit;
    /* 0014 */ DBCString m_name;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t m_spellIconID;
};

#endif
