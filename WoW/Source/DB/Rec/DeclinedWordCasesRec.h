#ifndef WOW_SOURCE_DB_REC_DECLINEDWORDCASESREC_H
#define WOW_SOURCE_DB_REC_DECLINEDWORDCASESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DeclinedWordCasesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_declinedWordID;
    /* 0008 */ int32_t m_caseIndex;
    /* 000C */ DBCString m_declinedWord;

    static void FixupStrings(void* record, const char* strings);
};

#endif
