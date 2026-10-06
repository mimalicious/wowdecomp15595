#ifndef WOW_SOURCE_DB_REC_CURRENCYTYPESREC_H
#define WOW_SOURCE_DB_REC_CURRENCYTYPESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CurrencyTypesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ DBCString unk_0008;
    /* 000C */ DBCString unk_000C;
    /* 0010 */ DBCString unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ DBCString unk_0028;
};

#endif
