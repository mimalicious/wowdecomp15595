#include "WoW/Source/DB/Rec/ItemCurrencyCostRec.h"

// STATIC: 0x00C26FE8
static const int32_t s_fieldOffsets[] = { 0, 4 };

// STATIC: 0x00C26FF0
static const int32_t s_fieldSizes[] = { 4, 4 };

// STATIC: 0x00C26FF8
static const int32_t s_fieldTypes[] = { 0, 0 };

// STATIC: 0x00C27000
const DB2Meta ItemCurrencyCostRec::s_meta = { "DBFilesClient\\ItemCurrencyCost.db2", "ItemCurrencyCost.adb", 2, 8, ItemCurrencyCostRec::FixupRecord, 0, true, false, s_fieldOffsets, s_fieldSizes, s_fieldTypes, 0x6FE05AE9, 0x00000000, 0 };

// OFFSET: 0x00401000/0x1407D7E80
void ItemCurrencyCostRec::FixupRecord(void* record) {
}
