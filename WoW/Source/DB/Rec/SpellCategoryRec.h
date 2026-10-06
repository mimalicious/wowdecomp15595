#ifndef WOW_SOURCE_DB_REC_SPELLCATEGORYREC_H
#define WOW_SOURCE_DB_REC_SPELLCATEGORYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellCategoryRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ DBCString unk_000C;

    static void FixupStrings(void* record, const char* strings);
};

#endif
