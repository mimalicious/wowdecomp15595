#ifndef WOW_SOURCE_DB_REC_SPELLCATEGORIESREC_H
#define WOW_SOURCE_DB_REC_SPELLCATEGORIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellCategoriesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_category;
    /* 0008 */ int32_t m_defenseType;
    /* 000C */ int32_t m_dispelType;
    /* 0010 */ int32_t m_mechanic;
    /* 0014 */ int32_t m_preventionType;
    /* 0018 */ int32_t m_startRecoveryCategory;
};

#endif
