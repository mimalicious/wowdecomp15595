#ifndef WOW_SOURCE_DB_REC_SPELLSHAPESHIFTREC_H
#define WOW_SOURCE_DB_REC_SPELLSHAPESHIFTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellShapeshiftRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_shapeshiftExclude [2];
    /* 000C */ int32_t m_shapeshiftMask [2];
    /* 0014 */ int32_t m_stanceBarOrder;
};

#endif
