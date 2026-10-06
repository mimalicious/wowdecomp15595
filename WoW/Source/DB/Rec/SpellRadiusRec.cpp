#include "WoW/Source/DB/Rec/SpellRadiusRec.h"

// STATIC: 0x00C20E18/0x140A15838
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C20E28/0x140A15848
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C20E38/0x140A15858
static const int32_t s_fieldTypes[4] = { 0, 3, 3, 3 };

// STATIC: 0x00C20E48/0x140A15868
const DBCMeta SpellRadiusRec::s_meta = { "DBFilesClient\\SpellRadius.dbc", 4, 16, SpellRadiusRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ff33f4ef IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellRadiusRec::FixupStrings(void* record, const char* strings) {
}
