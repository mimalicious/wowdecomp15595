#ifndef WOW_SOURCE_DB_REC_CHARTITLESREC_H
#define WOW_SOURCE_DB_REC_CHARTITLESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CharTitlesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_conditionID;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_name1;
    /* 0010 */ int32_t m_maskID;
    /* 0014 */ int32_t unk_0014;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CharTitlesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
