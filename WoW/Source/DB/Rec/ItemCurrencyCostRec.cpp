#include "WoW/Source/DB/Rec/ItemCurrencyCostRec.h"

// STATIC: %0x00B46308
const DBCMeta ItemCurrencyCostRec::s_meta = { "DBFilesClient\\ItemCurrencyCost.dbc", 2, 8, ItemCurrencyCostRec::FixupStrings, 0, NULL, NULL, NULL };

// PROVEN: d7858cf7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemCurrencyCostRec::FixupStrings(void* record, const char* strings) {
}
