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
    /* 000C */ const char* m_message;
    /* 0010 */ int32_t m_raidDuration;
    /* 0014 */ int32_t m_maxPlayers;
    /* 0018 */ const char* m_difficultystring;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(MapDifficultyRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
