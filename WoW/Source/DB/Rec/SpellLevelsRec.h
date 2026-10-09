#ifndef WOW_SOURCE_DB_REC_SPELLLEVELSREC_H
#define WOW_SOURCE_DB_REC_SPELLLEVELSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellLevelsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_baseLevel;
    /* 0008 */ int32_t m_maxLevel;
    /* 000C */ int32_t m_spellLevel;
};

#endif
