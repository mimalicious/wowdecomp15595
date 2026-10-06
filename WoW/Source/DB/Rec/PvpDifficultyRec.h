#ifndef WOW_SOURCE_DB_REC_PVPDIFFICULTYREC_H
#define WOW_SOURCE_DB_REC_PVPDIFFICULTYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PvpDifficultyRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_rangeIndex;
    /* 000C */ int32_t m_minLevel;
    /* 0010 */ int32_t m_maxLevel;
    /* 0014 */ int32_t m_difficulty;
};

#endif
