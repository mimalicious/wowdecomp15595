#ifndef WOW_SOURCE_DB_REC_TAXIPATHREC_H
#define WOW_SOURCE_DB_REC_TAXIPATHREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TaxiPathRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_fromTaxiNode;
    /* 0008 */ int32_t m_toTaxiNode;
    /* 000C */ int32_t m_cost;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TaxiPathRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
