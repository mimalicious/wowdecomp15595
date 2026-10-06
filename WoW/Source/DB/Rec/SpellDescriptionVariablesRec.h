#ifndef WOW_SOURCE_DB_REC_SPELLDESCRIPTIONVARIABLESREC_H
#define WOW_SOURCE_DB_REC_SPELLDESCRIPTIONVARIABLESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellDescriptionVariablesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_variables;

    static void FixupStrings(void* record, const char* strings);
};

#endif
