#ifndef WOW_SOURCE_DB_REC_DECLINEDWORDREC_H
#define WOW_SOURCE_DB_REC_DECLINEDWORDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DeclinedWordRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_word;

    static void FixupStrings(void* record, const char* strings);
};

#endif
