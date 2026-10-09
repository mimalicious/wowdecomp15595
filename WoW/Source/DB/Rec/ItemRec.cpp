#include "WoW/Source/DB/Rec/ItemRec.h"

// STATIC: 0x00C1C620
static const int32_t s_fieldOffsets[] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C1C640
static const int32_t s_fieldSizes[] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1C660
static const int32_t s_fieldTypes[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1C680
const DB2Meta ItemRec::s_meta = { "DBFilesClient\\Item.db2", "Item.adb", 8, 32, ItemRec::FixupRecord, 0, true, false, s_fieldOffsets, s_fieldSizes, s_fieldTypes, 0x50238EC2, 0x919BE54E, 0 };

// PROVEN: 27fe000d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemRec::FixupRecord(void* record) {
}
