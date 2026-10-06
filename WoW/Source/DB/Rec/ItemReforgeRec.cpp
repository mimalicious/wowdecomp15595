#include "WoW/Source/DB/Rec/ItemReforgeRec.h"

// STATIC: 0x00C23EC4/0x140A193B0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C23ED8/0x140A193C8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C23EEC/0x140A193E0
static const int32_t s_fieldTypes[5] = { 0, 0, 3, 0, 3 };

// STATIC: 0x00C23F00/0x140A193F8
const DBCMeta ItemReforgeRec::s_meta = { "DBFilesClient\\ItemReforge.dbc", 5, 20, ItemReforgeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 225f83aa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemReforgeRec::FixupStrings(void* record, const char* strings) {
}
