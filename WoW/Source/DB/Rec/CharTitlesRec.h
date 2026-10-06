#ifndef WOW_SOURCE_DB_REC_CHARTITLESREC_H
#define WOW_SOURCE_DB_REC_CHARTITLESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CharTitlesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_conditionID;
    /* 0008 */ DBCString m_name;
    /* 000C */ DBCString m_name1;
    /* 0010 */ int32_t m_maskID;
    /* 0014 */ int32_t unk_0014;

    static void FixupStrings(void* record, const char* strings);
};

#endif
