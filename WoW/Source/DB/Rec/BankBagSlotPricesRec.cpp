#include "WoW/Source/DB/Rec/BankBagSlotPricesRec.h"

// STATIC: %0x00B47134
const DBCMeta BankBagSlotPricesRec::s_meta = { "DBFilesClient\\BankBagSlotPrices.dbc", 2, 8, BankBagSlotPricesRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void BankBagSlotPricesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void BankBagSlotPricesRec::ConvertRow(BankBagSlotPricesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_cost, &row[0x0004], sizeof(rec->m_cost));
}
#endif
