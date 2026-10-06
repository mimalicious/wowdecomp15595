#include "WoW/Source/DB/Rec/BankBagSlotPricesRec.h"

// STATIC: 0x00C26914/0x140A1C918
const DBCMeta BankBagSlotPricesRec::s_meta = { "DBFilesClient\\BankBagSlotPrices.dbc", 2, 8, BankBagSlotPricesRec::FixupStrings, 0 };

// PROVEN: de6b52f9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void BankBagSlotPricesRec::FixupStrings(void* record, const char* strings) {
}
