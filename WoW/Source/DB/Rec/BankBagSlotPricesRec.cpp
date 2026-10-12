#include "WoW/Source/DB/Rec/BankBagSlotPricesRec.h"

// STATIC: 0x00C268FC/0x140A1C900
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C26904/0x140A1C908
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C2690C/0x140A1C910
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C26914/0x140A1C918
const DBCMeta BankBagSlotPricesRec::s_meta = { "DBFilesClient\\BankBagSlotPrices.dbc", 2, 8, BankBagSlotPricesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void BankBagSlotPricesRec::FixupStrings(void* record, const char* strings) {
}
