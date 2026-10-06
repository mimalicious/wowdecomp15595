#include "WoW/Source/DB/Rec/MaterialRec.h"

// STATIC: 0x00C233BC/0x140A18608
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C233D0/0x140A18620
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C233E4/0x140A18638
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C233F8/0x140A18650
const DBCMeta MaterialRec::s_meta = { "DBFilesClient\\Material.dbc", 5, 20, MaterialRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7c2127f5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MaterialRec::FixupStrings(void* record, const char* strings) {
}
