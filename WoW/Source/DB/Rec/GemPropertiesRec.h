#ifndef WOW_SOURCE_DB_REC_GEMPROPERTIESREC_H
#define WOW_SOURCE_DB_REC_GEMPROPERTIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GemPropertiesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_enchantID;
    /* 0008 */ int32_t m_maxcountInv;
    /* 000C */ int32_t m_maxcountItem;
    /* 0010 */ int32_t m_type;
    /* 0014 */ int32_t m_minItemLevel;
};

#endif
