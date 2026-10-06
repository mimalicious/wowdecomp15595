#ifndef WOW_SOURCE_DB_REC_CURRENCYCATEGORYREC_H
#define WOW_SOURCE_DB_REC_CURRENCYCATEGORYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CurrencyCategoryRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ const char* m_name;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CurrencyCategoryRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
