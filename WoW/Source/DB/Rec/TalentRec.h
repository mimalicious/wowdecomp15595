#ifndef WOW_SOURCE_DB_REC_TALENTREC_H
#define WOW_SOURCE_DB_REC_TALENTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TalentRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_tabID;
    /* 0008 */ int32_t m_tierID;
    /* 000C */ int32_t m_columnIndex;
    /* 0010 */ int32_t m_spellRank [5];
    /* 0024 */ int32_t m_prereqTalent [3];
    /* 0030 */ int32_t m_prereqRank [3];
    /* 003C */ int32_t m_flags;
    /* 0040 */ int32_t m_requiredSpellID;
    /* 0044 */ int32_t m_categoryMask [2];
};

#endif
