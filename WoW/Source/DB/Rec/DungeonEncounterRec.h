#ifndef WOW_SOURCE_DB_REC_DUNGEONENCOUNTERREC_H
#define WOW_SOURCE_DB_REC_DUNGEONENCOUNTERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DungeonEncounterRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_difficulty;
    /* 000C */ int32_t m_orderIndex;
    /* 0010 */ int32_t m_bit;
    /* 0014 */ const char* m_name;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(DungeonEncounterRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
