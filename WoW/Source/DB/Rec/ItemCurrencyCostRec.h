#ifndef WOW_SOURCE_DB_REC_ITEMCURRENCYCOSTREC_H
#define WOW_SOURCE_DB_REC_ITEMCURRENCYCOSTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB2.h"

class ItemCurrencyCostRec {
    public:
    static const DB2Meta s_meta;

    static void FixupRecord(void* record);

    /* 0000 */ int32_t m_ID;
    /* 0004 */ int32_t m_itemID;
};

#endif
