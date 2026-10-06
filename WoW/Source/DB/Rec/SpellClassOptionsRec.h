#ifndef WOW_SOURCE_DB_REC_SPELLCLASSOPTIONSREC_H
#define WOW_SOURCE_DB_REC_SPELLCLASSOPTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellClassOptionsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ DBCString unk_0018;

    static void FixupStrings(void* record, const char* strings);
};

#endif
