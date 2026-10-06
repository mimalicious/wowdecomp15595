#ifndef WOW_SOURCE_DB_REC_BANKBAGSLOTPRICESREC_H
#define WOW_SOURCE_DB_REC_BANKBAGSLOTPRICESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class BankBagSlotPricesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(BankBagSlotPricesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
