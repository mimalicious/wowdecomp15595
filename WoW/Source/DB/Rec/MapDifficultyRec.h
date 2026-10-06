#ifndef WOW_SOURCE_DB_REC_MAPDIFFICULTYREC_H
#define WOW_SOURCE_DB_REC_MAPDIFFICULTYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MapDifficultyRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_difficulty;
    /* 000C */ DBCString m_message;
    /* 0010 */ int32_t m_raidDuration;
    /* 0014 */ int32_t m_maxPlayers;
    /* 0018 */ DBCString m_difficultystring;

    static void FixupStrings(void* record, const char* strings);
};

#endif
