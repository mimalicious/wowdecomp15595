#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"

// STATIC: 0x00C27070
static const int32_t s_fieldOffsets[] = { 0, 4, 8, 12, 16, 36, 56, 60, 64, 84, 104, 108, 112, 116, 120 };

// STATIC: 0x00C270AC
static const int32_t s_fieldSizes[] = { 4, 4, 4, 4, 20, 20, 4, 4, 20, 20, 4, 4, 4, 4, 4 };

// STATIC: 0x00C270E8
static const int32_t s_fieldTypes[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C27124
const DB2Meta ItemExtendedCostRec::s_meta = { "DBFilesClient\\ItemExtendedCost.db2", "ItemExtendedCost.adb", 15, 124, ItemExtendedCostRec::FixupRecord, 0, true, false, s_fieldOffsets, s_fieldSizes, s_fieldTypes, 0xBB858355, 0x00000000, 0 };

// PROVEN: d2ba585a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemExtendedCostRec::FixupRecord(void* record) {
}
