#ifndef WOW_SOURCE_DB_REC_SPELLCLASSOPTIONSREC_H
#define WOW_SOURCE_DB_REC_SPELLCLASSOPTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellClassOptionsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_modalNextSpell;
    /* 0008 */ int32_t m_spellClassMask[3];
    /* 0014 */ int32_t m_spellClassSet;
    /* 0018 */ DBCString m_description;
};

#endif
