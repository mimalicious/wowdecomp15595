#ifndef WOW_SOURCE_DB_REC_LFGDUNGEONEXPANSIONREC_H
#define WOW_SOURCE_DB_REC_LFGDUNGEONEXPANSIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LfgDungeonExpansionRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_lfgID;
    /* 0008 */ int32_t m_expansionLevel;
    /* 000C */ int32_t m_randomID;
    /* 0010 */ int32_t m_hardLevelMin;
    /* 0014 */ int32_t m_hardLevelMax;
    /* 0018 */ int32_t m_targetLevelMin;
    /* 001C */ int32_t m_targetLevelMax;

    static void FixupStrings(void* record, const char* strings);
};

#endif
